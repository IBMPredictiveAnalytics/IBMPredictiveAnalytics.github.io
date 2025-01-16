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


import struct
import adapter.messageids as messageids
import adapter.libmessage as libmessage


# Instantiates a new message from the provided input stream
# Returns a tuple of message, payloadsize
def instantiate_message(connection):
    typelen = 2
    instancelen = 2
    psizelen = 4
    hdrlen = typelen + instancelen + psizelen
    if connection.inbuffer and len(connection.inbuffer) >= hdrlen:
        # libmessage.trace('Instantiating message from buffer:', repr(connection.inbuffer))
        msgtype = struct.unpack("<H", connection.inbuffer[:typelen])[0]
        msginstance = struct.unpack("<H", connection.inbuffer[typelen:psizelen])[0]
        payloadsize = struct.unpack("<I", connection.inbuffer[psizelen:hdrlen])[0]
        message = _message_from_type(msgtype, msginstance)
        if message is None:
            print("Unknown message type", msgtype)
            raise RuntimeError("Unknown message type: " + str(msgtype))
        connection.inbuffer = connection.inbuffer[hdrlen:]
        return (message, payloadsize)
    return None

# Factory method to generate a message from a type id and instance id
def _message_from_type(msgtype, msginstance):
    if msgtype == messageids.MSG_RESPONSE:
        return libmessage.msg_response(msginstance)
    elif msgtype == messageids.MSG_LOAD_EXTENSION:
        return libmessage.msg_load_extension(msginstance)
    elif msgtype == messageids.MSG_CLOSE_PROCESS:
        return libmessage.msg_close_process(msginstance)
    elif msgtype == messageids.MSG_GET_NAME:
        return libmessage.msg_get_name(msginstance)
    elif msgtype == messageids.MSG_GET_VERSION:
        return libmessage.msg_get_version(msginstance)
    elif msgtype == messageids.MSG_GET_PROPERTY_SPEC:
        return libmessage.msg_get_property_spec(msginstance)
    elif msgtype == messageids.MSG_GET_SYNTAX_SPEC:
        return libmessage.msg_get_syntax(msginstance)
    elif msgtype == messageids.MSG_EXECUTE:
        return libmessage.msg_execute(msginstance)
    elif msgtype == messageids.MSG_NEXT_RECORD:
        return libmessage.msg_get_record(msginstance)
    elif msgtype == messageids.MSG_CREATE_VARIABLES:
        return libmessage.msg_create_variables(msginstance)
    elif msgtype == messageids.MSG_RUN_SYNTAX:
        return libmessage.msg_run_syntax(msginstance)
    elif msgtype == messageids.MSG_GENERATE_OUTPUT:
        return libmessage.msg_generate_output(msginstance)
    elif msgtype == messageids.MSG_FINISH:
        return libmessage.msg_finish(msginstance)
    else:
        print('Could not find message type:', msgtype)
    return None
