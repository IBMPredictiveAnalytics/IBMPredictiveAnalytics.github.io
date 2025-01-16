#/***********************************************************************
# * Licensed Materials - Property of IBM
# *
# * IBM SPSS Products: Statistics Common
# *
# * (C) Copyright IBM Corp. 2022
# *
# * US Government Users Restricted Rights - Use, duplication or disclosure
# * restricted by GSA ADP Schedule Contract with IBM Corp.
# ************************************************************************/

"""
Python extension utilities.
"""

import struct
import os

# Converts a string to a buffer of utf-8 bytes
from util.statjson import Text


def stringtobuffer(string):
    bytes = string.encode()
    return struct.pack("<I", len(bytes)) + bytes


# Debug hex dump. Call with a string prefix to display, followed by
# the array of bytes to dump
def dumphex(prefix, bytes):
    hex = ''.join(r'\x' + format(x, '02X') for x in bytes)
    print(prefix, hex)


# Dumps an array to console
def dump_array(array, max_records=50):
    current_line = 0;
    record_count = len(array)
    if record_count >= max_records:
        print(f'Array size: {record_count} -> dumping the first {max_records} records...')
    print('[', end='')
    for line in array:
        end_o_data = current_line == min(record_count-1, max_records-1)
        print('[', end='')
        current_field = 0
        for j in line:
            end_o_line = current_field == len(line)-1
            if end_o_line and end_o_data:
                print(j, end=']]\n')
            elif end_o_line:
                print(j, end='],\n ')
            else:
                print(j, end=", ")
            current_field += 1
        current_line += 1
        if current_line >= max_records:
            break

# Writes input parameter to a file if AS_STAT_DEBUG_DIR environment variable is set
def write_debug(info):
    debug_dir = os.getenv('AS_STAT_DEBUG_DIR')
    if debug_dir is not None:
        debug_file_path = os.path.join(debug_dir,"debugOutput.txt")
        debug_file = open(debug_file_path, "a")
        debug_file.write(str(info) + "\n")
        debug_file.close()

# Outputs debug text to output window
def output_debug(output_json, info):
    debug_text = Text("Debug Info")
    debug_text.set_content(info)
    output_json.add_text(debug_text)
