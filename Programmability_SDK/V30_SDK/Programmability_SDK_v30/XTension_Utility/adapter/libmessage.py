#/***********************************************************************
# * Licensed Materials - Property of IBM
# *
# * IBM SPSS Products: Statistics Common
# *
# * (C) Copyright IBM Corp. 2021
# *
# * US Government Users Restricted Rights - Use, duplication or disclosure
# * restricted by GSA ADP Schedule Contract with IBM Corp.
# ************************************************************************/

from threading import Lock
import sys
import struct
import ctypes
import adapter.messageids as messageids


# Adapter debug trace level. Set to 0 to turn off debug tracing before shipping
#ADAPTER_DEBUG_LEVEL = 0
ADAPTER_DEBUG_LEVEL = 1
#ADAPTER_DEBUG_LEVEL = 2


# Internal class for unique message ids
class id_generator:
    def __init__(self):
        self.current_id = 1
        self.lock = Lock()
    def generateId(self):
        retVal = self.current_id
        self.lock.acquire()
        self.current_id += 1
        if self.current_id == 65535:
            self.current_id = 1
        self.lock.release()
        return retVal


# Create a single id generator
_id_generator = id_generator()


# Base class for all messages
class msg_base:
    # Constructor -
    # msg_type is one of the legal message type integers from messageids.py
    # msg_instance is the instance id for use in deserialization
    def __init__(self, msg_type, msg_instance=None):
        self._msg_type = msg_type
        self._instance_id = msg_instance if msg_instance else _id_generator.generateId()
    # Returns the message type from messageids.py
    def get_msg_type(self):
        return self._msg_type
    # Returns the unique message identifier
    def get_instance(self):
        return self._instance_id
    # Reads parameters from the provided stream into class members
    def read(self, connection):
        return True
    # Writes message payload to the returned byte array
    def write(self):
        return b''
    # Reads the message-specific response
    def read_response(self, connection):
        return None
    # Reads a utf-8 string from the provided stream
    def readstring(self, connection):
        byteslen = self.readuint(connection)
        if len(connection.inbuffer) >= byteslen:
            strvalue = connection.inbuffer[:byteslen].decode("utf-8", "ignore")
            connection.inbuffer = connection.inbuffer[byteslen:]
            return strvalue
        trace('Could not read string data. Length to read:', byteslen,
                ', length available:', len(connection.inbuffer))
        return None
    # Writes a utf-8 string to the returned byte array
    def writestring(self, stringvalue):
        bytes = stringvalue.encode('utf-8')
        return struct.pack("<I", len(bytes)) + bytes
    # Reads a 4-byte signed integer
    def readint(self, connection):
        valuelen = 4
        if len(connection.inbuffer) >= valuelen:
            value = struct.unpack("<i", connection.inbuffer[:valuelen])[0]
            connection.inbuffer = connection.inbuffer[valuelen:]
            return value
        return None
    # Writes a 4-byte signed integer
    def writeint(self, value):
        return struct.pack("<i", value)
    # Reads a 4-byte UINT from the provided stream
    def readuint(self, connection):
        valuelen = 4
        if len(connection.inbuffer) >= valuelen:
            value = struct.unpack("<I", connection.inbuffer[:valuelen])[0]
            connection.inbuffer = connection.inbuffer[valuelen:]
            return value
        return None
    # Writes a 4-byte UINT to the returned byte array
    def writeuint(self, value):
        return struct.pack("<I", value)
    # Reads a 8-byte UINT64 from the provided stream
    def readuint64(self, connection):
        valuelen = 8
        if len(connection.inbuffer) >= valuelen:
            value = struct.unpack("<Q", connection.inbuffer[:valuelen])[0]
            connection.inbuffer = connection.inbuffer[valuelen:]
            return value
        return None
    # Writes a 8-byte UINT to the returned byte array
    def writeuint64(self, value):
        return struct.pack("<Q", value)
    # Reads an 8-byte IEEE 754 double from the provided stream
    def readdouble(self, connection):
        valuelen = 8
        if len(connection.inbuffer) >= valuelen:
            value = struct.unpack("<d", connection.inbuffer[:valuelen])[0]
            connection.inbuffer = connection.inbuffer[valuelen:]
            return value
        return None
    # Writes an 8-byte IEEE 754 double to the returned byte array
    def writedouble(self, value):
        return struct.pack("<d", value)


# Response message for all requests. Payload semantics depend upon the original
# message type, and are read by the read_response() method of the calling message.
# Call with an instance-id 4-byte integer and a payload that is an array of bytes.
class msg_response(msg_base):
    def __init__(self, msg_instance=None, payload=None):
        super().__init__(messageids.MSG_RESPONSE, msg_instance)
        self._payload = payload
    def get_payload(self):
        return self._payload
    def read(self, connection):
        request_dict = connection.requests.get(self._instance_id)
        # Ensure that we have a cached copy of the original request
        if not request_dict or not request_dict['request']:
            print("Unable to locate request for response", self.get_instance())
        else:
            # Set the response into the original request object
            if ADAPTER_DEBUG_LEVEL > 1:
                print('Reading response payload for request type:', \
                       request_dict['request'].get_msg_type())
            self._payload = request_dict['request'].read_response(connection)
        return self._payload is not None
    def write(self):
        if self._payload is not None:
            return self._payload
        else:
            print("No payload for response", self.get_instance())
        return b''


# Backend->Extension load extension request
class msg_load_extension(msg_base):
    def __init__(self, instance=None, name=None):
        super().__init__(messageids.MSG_LOAD_EXTENSION, instance)
        self._extension_name = name
    def get_extension_name(self):
        return self._extension_name
    def read(self, connection):
        self._extension_name = self.readstring(connection)
        return len(self._extension_name) > 0
    def write(self):
        return self.writestring(self._extension_name)
    def read_response(self, connection):
        return self.readint(connection)


# Backend->Extension Shutdown request
class msg_close_process(msg_base):
    def __init__(self, instance=None):
        super().__init__(messageids.MSG_CLOSE_PROCESS, instance)
    def read(self, connection):
        return True
    def write(self):
        return b''
    def read_response(self, connection):
        return self.readstring(connection)


# Backend->Extension name request
class msg_get_name(msg_base):
    def __init__(self, instance=None):
        super().__init__(messageids.MSG_GET_NAME, instance)
    def read(self, connection):
        return True
    def write(self):
        return b''
    def read_response(self, connection):
        return self.readstring(connection)


# Backend->Extension version request
class msg_get_version(msg_base):
    def __init__(self, instance=None):
        super().__init__(messageids.MSG_GET_VERSION, instance)
    def read(self, connection):
        return True
    def write(self):
        return b''
    def read_response(self, connection):
        return self.readstring(connection)


# Backend->Extension property specification request
class msg_get_property_spec(msg_base):
    def __init__(self, instance=None):
        super().__init__(messageids.MSG_GET_PROPERTY_SPEC, instance)
    def read(self, connection):
        return True
    def write(self):
        return b''
    def read_response(self, connection):
        return self.readstring(connection)


# Backend->Extension syntax specification request
class msg_get_syntax(msg_base):
    def __init__(self, instance=None):
        super().__init__(messageids.MSG_GET_SYNTAX_SPEC, instance)
    def read(self, connection):
        return True
    def write(self):
        return b''
    def read_response(self, connection):
        return self.readstring(connection)


# Backend->Extension execution request
class msg_execute(msg_base):
    def __init__(self, instance=None, iter_id=None, data_model=None, settings=None, lang="en"):
        super().__init__(messageids.MSG_EXECUTE, instance)
        self._iter_id = iter_id
        self._data_model = data_model
        self._settings = settings
        self._language = lang
    # Return the 4-byte data iterator id
    def get_iter_id(self):
        return self._iter_id
    # Return the JSON data model string
    def get_data_model(self):
        return self._data_model
    # Return the JSON execution settings string
    def get_settings(self):
        return self._settings
    def get_lang(self):
        return self._language
    def read(self, connection):
        self._iter_id = self.readuint(connection)
        self._data_model = self.readstring(connection)
        self._settings = self.readstring(connection)
        self._language = self.readstring(connection)
        return self._settings is not None
    def write(self):
        if not self._iter_id or not self._data_model or not self._settings:
            raise RuntimeError("No member data to write in msg_execute")
        return self.writeuint(self._iter_id) + \
                self.writestring(self._data_model) + \
                self.writestring(self._settings) + \
                self.writestring(self._language)
    def read_response(self, connection):
        return self.readint(connection)


# Extension->Backend record request
class msg_get_record(msg_base):
    def __init__(self, instance=None, iteratorid=None, columncount=None):
        super().__init__(messageids.MSG_NEXT_RECORD, instance)
        self._iter_id = iteratorid
        self._columncount = columncount
    def get_iter_id(self):
        return self._iter_id
    def read(self, connection):
        self._iter_id = self.readuint(connection)
        return self._iter_id is not None
    def write(self):
        if not self._iter_id:
            raise RuntimeError("No member data to write in msg_get_record")
        return self.writeuint(self._iter_id)
    # The record parameter should be a list of IEEE 754 doubles
    def read_response(self, connection):
        formatstring = '<{0}d'.format(self._columncount)
        valuelen = 8 * self._columncount
        if len(connection.inbuffer) >= valuelen:
            value = struct.unpack(formatstring, connection.inbuffer[:valuelen])
            connection.inbuffer = connection.inbuffer[valuelen:]
            return value
        # This enables the socket to continue buffering input until we have a complete record
        return None


# Extension->Backend data model modification
class msg_create_variables(msg_base):
    def __init__(self, instance=None, datamodelstring=None, record_count=0,
                 start_record=0, record_data=b''):
        super().__init__(messageids.MSG_CREATE_VARIABLES, instance)
        self._dmstring = datamodelstring
        self._record_count = record_count
        self._start_record = start_record
        self._record_data = record_data
    def get_data_model(self):
        return self._dmstring
    def get_record_count(self):
        return self._record_count
    def get_start_record(self):
        return self._start_record
    def get_record_data(self):
        return self._record_data
    def read(self, connection):
        self._dmstring = self.readstring(connection)
        self._record_count = self.readuint64(connection)
        self._start_record = self.readuint64(connection)
        size = self.readuint(connection)
        self._record_data = connection.inbuffer[:size]
        connection.inbuffer = connection.inbuffer[size:]
        return self._dmstring is not None
    def write(self):
        return self.writestring(self._dmstring) + \
               self.writeuint64(self._record_count) + \
               self.writeuint64(self._start_record) + \
               self.writeuint(len(self._record_data)) + \
               self._record_data
    def read_response(self, connection):
        return self.readint(connection)


# Extension->Backend syntax execution request
class msg_run_syntax(msg_base):
    def __init__(self, instance=None, syntaxstring=None):
        super().__init__(messageids.MSG_RUN_SYNTAX, instance)
        self._syntax = syntaxstring
    def get_syntax(self):
        return self._syntax
    def read(self, connection):
        self._syntax = self.readstring(connection)
        return self._syntax is not None
    def write(self):
        return self.writestring(self._syntax)
    def read_response(self, connection):
        return self.readint(connection)


# Extension->Backend output generation request
class msg_generate_output(msg_base):
    def __init__(self, instance=None, statjsonstring=None):
        super().__init__(messageids.MSG_GENERATE_OUTPUT, instance)
        self._statjson = statjsonstring
    def get_stat_json(self):
        return self._statjson
    def read(self, connection):
        self._statjson = self.readstring(connection)
        return self._statjson is not None
    def write(self):
        return self.writestring(self._statjson)
    def read_response(self, connection):
        return self.readint(connection)


# Extension->Backend completion notification message
class msg_finish(msg_base):
    def __init__(self, instance=None, statuscode=None):
        super().__init__(messageids.MSG_FINISH, instance)
        self._statuscode = statuscode
    def get_status_code(self):
        return self._statuscode
    def read(self, connection):
        self._statuscode = self.readint(connection)
        return self._statuscode is not None
    def write(self):
        return self.writeint(self._statuscode)
    def read_response(self, connection):
        return self.readint(connection)

# Debug trace
def trace(*objects):
    if ADAPTER_DEBUG_LEVEL:
        print(objects)
