# ***********************************************************************
# * Licensed Materials - Property of IBM
# *
# * IBM SPSS Products: Statistics Common
# *
# * (C) Copyright IBM Corp. 2021
# *
# * US Government Users Restricted Rights - Use, duplication or disclosure
# * restricted by GSA ADP Schedule Contract with IBM Corp.
# ************************************************************************/

import sys
import socket
import struct
import asyncio
import types
import json
import adapter.libmessage as libmsg
from adapter.libmessage import trace
import adapter.messageids as messageids
import adapter.message_factory as factory
import util.xtutil as utilities


class clientadapter:
    """
    A class to communicate extension directives with the Statistics backend.

    Use this class to connect to the backend and have the backend invoke the
    extension via syntax.
    """

    def __init__(self, host, port, wrapper_callback):
        """
        Parameters
        ----------
        host : string
            Hostname to connect back to
        port : integer
            The port number to connect to
        wrapper_callback : an object containing the following callbacks:
            load_extension(name) - none of the other methods are valid until
                this method has been successfully invoked.
            close_process() - to close and exit the Python process
            get_name() - to return the extension name
            get_version() - returns the version of the extension runtime for this item
            get_property_spec() - returns the prop spec for the extension
            get_syntax_spec() - Returns syntax spec or None for internal
            execute(iteratorId, dataModel, settings) - Executes the extension
        """

        self._wrapper_callback = wrapper_callback
        self._connection = types.SimpleNamespace(
            host=host,
            port=port,
            inbuffer=b'',
            outbuffer=b'',
            requests={}
        )
        self._writer = None
        self._max_records_per_chunk = -1

    def run_client(self):
        """Public method to start background socket processing.

        Once control is passed to this function, the backend will be in control
        of the current thread's execution, using the client as a proxy for
        communication.
        """

        # This is needed to support non-western languages on Windows
        sys.stdin.reconfigure(encoding='utf-8')
        sys.stdout.reconfigure(encoding='utf-8')

        loop = asyncio.get_event_loop()
        try:
            loop.run_until_complete(self._do_run())
        except KeyboardInterrupt:
            print('Caught interrupt')
        finally:
            if self._writer:
                trace('About to close socket after client run complete...')
                loop.run_until_complete(self._close_socket(self._writer))
                self._writer = None
            trace('Client run complete')

    def get_records(self, iterid, json_dm, callback):
        """Request data records from the backend.

        This method should be invoked with the idValue passed in the first
        parameter to execute(). When iterid is positive, this method is called
        only once to initiate the reception of data. The provided callback will
        then be invoked repeatedly with a 2-dimensional array as the lone
        parameter. The record count in the array is governed by the value of
        the 'records_per_chunk' extension descriptor setting. A None value
        indicates EOF.

        Parameters
        ----------
        iterid : integer
            idValue from a backend call to execute()
        json_dm : json data model object
            Data model for the expected records - acquired from execute()
        callback : function
            a callback to receive the result records. Results are in the form
            of a 2D array of doubles, each record containing the number of
            variables defined in the data model sent to the execute() function
        """
        if iterid > 0:
            # Retrieve the data in chunks
            loop = asyncio.get_event_loop()
            loop.create_task(self._handle_chunk_request(iterid, json_dm, callback))
        else:
            callback(None)

    def create_variables(self, datamodel, record_count, start_record,
                         record_data, callback):
        """Request the backend to create one or more new variables.

        Parameters
        ----------
        datamodel : string;json
            Data model containing only those variables to be added
        record_count : unsigned integer
            The count of records added in this call (can be zero for metadata-only)
        start_record : unsigned integer
            The start record index
        record_data : byte array
            An array of casewise data in which doubles are represented by IEEE
            754 8-byte values, and strings are represented by a 4-byte unsigned
            integer for the length in bytes, followed by the encoded UTF-8 bytes
            for the string value. The content of the data in each record is
            determined by the order and type of the new variables described in
            the provided datamodel
        callback : function
            A callback to receive the result code
        """
        self._exec_message(libmsg.msg_create_variables(None, datamodel,
                                                       record_count, start_record, record_data), callback)

    def run_syntax(self, syntax, callback):
        """Execute syntax in the backend.

        Cannot be called during data iteration.

        Parameters
        ----------
        syntax : string
            The syntax to execute
        callback : function
            A callback to receive the result code
        """
        self._exec_message(libmsg.msg_run_syntax(None, syntax), callback)

    def generate_output(self, statjson, callback):
        """Generates output in the viewer.

        Parameters
        ----------
        statjson : string;json
            A StatJSON string describing procedure output
        callback : function
            A callback to receive the result code
        """
        self._exec_message(libmsg.msg_generate_output(None, statjson), callback)

    def finish(self, statuscode):
        """Invoke to finish XTension processing when execution is complete.

        Parameters
        ----------
        statuscode : 4-byte integer
            A completion status code. Zero for success, non-zero for error.
        """
        def cb(val):
            pass
        trace('Received finish() call from the XTension wrapper - sending to the backend')
        self._exec_message(libmsg.msg_finish(None, statuscode), cb)


    ################ Internal Methods ################

    # Internal method to open the connection and run the input and output tasks
    async def _do_run(self):
        writer = None
        try:
            conn = self._connection
            reader, writer = await asyncio.open_connection(conn.host, conn.port)
            self._writer = writer
            loop = asyncio.get_event_loop()
            trace(f'Connection from {writer.get_extra_info("peername")}')
            reader_task = loop.create_task(self._process_incoming(reader, writer))
            await reader_task
        finally:
            trace('Closing socket after reader_task has completed')
            await self._close_socket(writer)

    # Close the socket connection
    @staticmethod
    async def _close_socket(writer):
        if writer is not None and not writer.is_closing():
            writer.write(b'')
            trace('Client adapter: Closing socket')
            writer.close()
            await writer.wait_closed()

    # Process incoming messages
    async def _process_incoming(self, reader, writer):
        trace('Client: Process incoming')
        message = None
        while True:
            try:
                # Should be ready to read
                in_stream = await reader.read(4096)
                if libmsg.ADAPTER_DEBUG_LEVEL > 1:
                    utilities.dumphex('Client read:', in_stream)
            except BlockingIOError:
                # Resource temporarily unavailable (errno EWOULDBLOCK)
                pass
            else:
                if in_stream and in_stream != b'':
                    self._connection.inbuffer += in_stream
                else:
                    trace('Read empty buffer (zero bytes)')
                    break
            # Extract the next message(s)
            while self._connection.inbuffer != b'':
                if not message:
                    # Begin a new message
                    message, payloadsize = factory.instantiate_message(self._connection)
                    if libmsg.ADAPTER_DEBUG_LEVEL > 1:
                        trace('Received message:', type(message),
                              ' payload size:', payloadsize)
                if message:
                    # Read message payload
                    if 0 < payloadsize <= len(self._connection.inbuffer):
                        success = message.read(self._connection)
                    elif payloadsize > 0:
                        # Break to allow more payload to be read
                        trace('Message payload too large:', payloadsize)
                        break
                    else:
                        # Some request messages have no payload
                        success = payloadsize == 0
                    if success:
                        msg = message
                        message = None
                        self._dispatch_message(msg)
                    else:
                        trace('Unable to read payload for message type:',
                              message.get_msg_type(), ' instance:',
                              message.getinstance())
                else:
                    trace('No message or out of sync - messages may have been dropped')
                    break
        trace('End of incoming message processing')

    # Processes messages and responses being sent to the server
    async def _process_outgoing(self):
        if self._connection.outbuffer:
            temp = self._connection.outbuffer
            self._connection.outbuffer = b''
            self._writer.write(temp)
            await self._writer.drain()
            if libmsg.ADAPTER_DEBUG_LEVEL > 1:
                utilities.dumphex('Client write:', temp)
        # Sleep for 0 seconds for optimal time slicing
        await asyncio.sleep(0)

    # Handles response messages
    def _handle_response(self, msg):
        instance = msg.get_instance()
        trace('Response received on client. Instance-id:', instance,
              'Payload:', msg.get_payload())
        request_dict = self._uncache_request(instance)
        if request_dict and request_dict['callback']:
            request_dict['callback'](msg.get_payload())
        else:
            trace("Missing request dictonary for response instance:", instance)

    # Handle incoming messages
    def _handle_message(self, msg):
        resp = None
        wcb = self._wrapper_callback
        msgtype = msg.get_msg_type()
        instance = msg.get_instance()
        buffer = b''
        if msgtype == messageids.MSG_LOAD_EXTENSION:
            retcode = wcb['load_extension'](msg.get_extension_name())
            buffer = struct.pack("<i", retcode)
        elif msgtype == messageids.MSG_CLOSE_PROCESS:
            retcode = wcb['close_process']()
            buffer = struct.pack("<i", retcode)
        elif msgtype == messageids.MSG_GET_NAME:
            buffer = utilities.stringtobuffer(wcb['get_name']())
        elif msgtype == messageids.MSG_GET_VERSION:
            buffer = utilities.stringtobuffer(wcb['get_version']())
        elif msgtype == messageids.MSG_GET_PROPERTY_SPEC:
            buffer = utilities.stringtobuffer(wcb['get_property_spec']())
        elif msgtype == messageids.MSG_GET_SYNTAX_SPEC:
            spec = wcb['get_syntax_spec']()
            buffer = utilities.stringtobuffer(spec) if spec else b''
        elif msgtype == messageids.MSG_EXECUTE:
            settings = json.loads(msg.get_settings())
            chunksize = None
            trace('Settings:', settings)
            if "records_per_chunk" in settings:
                chunksize = settings["records_per_chunk"]
                trace('Settings record count per chunk:', chunksize)
            self._max_records_per_chunk = chunksize if chunksize else -1
            retcode = wcb['execute'](msg.get_iter_id(),
                                     json.loads(msg.get_data_model()),
                                     json.loads(msg.get_settings()),
                                     msg.get_lang())
            buffer = struct.pack("<i", retcode)
        else:
            raise RuntimeError("Unknown message type", msgtype)
        # Send the response message
        resp = libmsg.msg_response(instance, buffer)
        self.send_message(resp)

    # Processes an incoming message
    def _dispatch_message(self, message):
        if message:
            if message.get_msg_type() == messageids.MSG_RESPONSE:
                self._handle_response(message)
            else:
                self._handle_message(message)

    # Save an incoming request for processing the response
    def _cache_request(self, request, callback):
        # Save the request in the request dictionary, awaiting the response
        request_dict = {
            'callback': callback,
            'request': request
        }
        self._connection.requests[request.get_instance()] = request_dict

    # Remove cached data for an incoming request for processing the response
    def _uncache_request(self, instanceid):
        return self._connection.requests.pop(instanceid, None)

    # Sends message and caches request
    def _exec_message(self, message, callback):
        self._cache_request(message, callback)
        self.send_message(message)

    # Buffers an outgoing message
    def send_message(self, message):
        if not isinstance(message, libmsg.msg_base):
            raise RuntimeError("Unknown message", message)
        # Prepare the payload and header
        payload = message.write()
        message_hdr = struct.pack("<H", message.get_msg_type()) \
                      + struct.pack("<H", message.get_instance()) \
                      + struct.pack("<I", len(payload))
        # Buffer the payload and force it out
        self._connection.outbuffer += message_hdr + payload
        asyncio.create_task(self._process_outgoing())

    # Responds to requests for chunks of record data
    async def _handle_chunk_request(self, port, json_dm, callback):
        record_buffer = b''
        records = []
        total_bytes_received = 0
        try:
            reader, writer = await asyncio.open_connection(self._connection.host, port)
            while True:
                in_stream = await reader.read(40960)
                if in_stream == b'':
                    trace('Stopped reading data stream when an empty buffer was received.')
                    break
                else:
                    length = len(in_stream)
                    total_bytes_received += length
                    #print(f'Read incoming data stream of size: {length}')
                    record_buffer += in_stream
                    record_buffer = self._process_record_stream(json_dm,
                                                                record_buffer, records, callback)
            # Send any remaining records and an EOF message
            if len(records) > 0:
                trace(f'Received last block of records. Size: {len(records)}')
                callback(records)
            callback(None)
        except GeneratorExit as ge:
            trace('GeneratorExit')
            raise ge
        except Exception as e:
            trace(f'Exception in _handle_chunk_request: {str(e)}')
            raise e
        else:
            trace(f'Total data set bytes received and processed: {total_bytes_received}')
            trace('Closing socket after chunk request')
            await self._close_socket(writer)

    # Decodes incoming record data - currently supports doubles, integers, and strings
    def _process_record_stream(self, json_dm, buffer, records, callback):
        index = 0
        length = 0
        columns = len(json_dm['fields'])
        # Loop while we have at least one record of values
        while(len(buffer[index:]) >= columns * 4):
            # Read a record
            record = []
            recstart = index
            incomplete_record = False
            for field in json_dm['fields']:
                type = field['type']
                if type == "double" or type == "integer":
                    if len(buffer[index:]) >= 8:
                        value = struct.unpack("<d", buffer[index:index+8])[0]
                        index += 8
                        record.append(value)
                    else:
                        incomplete_record = True
                        break
                elif type == "string":
                    if len(buffer[index:]) >= 4:
                        length = struct.unpack("<I", buffer[index:index+4])[0]
                        index += 4
                    else:
                        incomplete_record = True
                        break
                    if len(buffer[index:]) >= length:
                        strvalue = buffer[index:index+length].decode("utf-8", "ignore")
                        index += length
                        record.append(strvalue)
                    else:
                        incomplete_record = True
                        break
                else:
                    print('Unknown field type:', type)
                    break
            if incomplete_record:
                index = recstart
                break
            else:
                # Append the record to the result
                records.append(record)
            # If we have reached our chunk size max, send the records
            if self._max_records_per_chunk == len(records):
                callback(records)
                records.clear()
        # Return the unprocessed portion of the read buffer
        return buffer[index:]
