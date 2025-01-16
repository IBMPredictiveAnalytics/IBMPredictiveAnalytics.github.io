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

"""
An extension wrapper for testing.
"""

import sys
import struct
from pathlib import Path
import util.xtutil as utilities
import json

_client = None

def set_client(client):
    global _client
    _client = client

def get_name():
    name = 'KRR Test Driver'
    print('Returning GetName() request:', name)
    return name

def get_version():
    version = '1.0'
    print('Returning GetVersion() request:', version)
    return version

def get_property_spec():
    path = Path(__file__).parent / "testwrapper-properties.json"
    props = Path(path).read_text()
    print('Returning GetPropertySpec() request:', props if len(props) < 100
        else '(long text)')
    return props

def get_syntax_spec():
    print('Returning GetSyntax() request:', None)
    return None

def execute(iteratorId, json_dm, settings, lang):
    global _client
    global _iteratorId
    global _callback_count
    print('execute({0}, {1}, {2}, {3})'.format(iteratorId,
        json_dm if len(json_dm) < 100 else '(long string)',
        settings if len(settings) < 100 else '(long string)',
        lang))
    if _client:
        _callback_count = 0
        _iteratorId = iteratorId
        _client.get_records(iteratorId, json_dm, record_iteration)
    return int(0)

def record_iteration(data):
    global _client
    global _iteratorId
    global _callback_count
    record_count = len(data) if data else '(EOF)'
    print(f'Received {record_count} record(s)')
    if _client and data:
        if _iteratorId < 0:
            _client.next_record(_iteratorId, 5, record_iteration)
        else:
            if _callback_count == 0:
                utilities.dump_array(data)
            _callback_count += 1
    else:
        _iteratorId = 0
        print('End of record iteration')
        put_variables()

def put_variables():
    global _client
    def callback(result):
        print('Result from create_variables():', result)
        if result == 0:
            generate_output()
    if _client:
        path = Path(__file__).parent / "datamodel2.json"
        datamodel = Path(path).read_text()
        path = Path(__file__).parent / "new_data.dat"
        with open(path, mode='rb') as file:
            fileContent = file.read()
        print('Sending create_variables(). File content length: ' + str(len(fileContent)))
        _client.create_variables(datamodel, 10, 0, fileContent, callback)


def generate_output():
    global _client
    def callback(result):
        print('Result from generate_output():', result)
        # Send a finished message
        _client.finish(0)
    if _client:
        print('Sending generate_output()')
        _client.generate_output("StatJSON1", callback)


"""
def write_binary():
    fout = open('new_data.dat', 'wb')
    for i in range(10):
        strvalue = 'Record ' + str(i+1)
        strbytes = strvalue.encode()
        record = struct.pack("<d", i+1) + \
                struct.pack("<I", len(strbytes)) + \
                strbytes
        fout.write(record)
    fout.close()
"""
