# ***********************************************************************
# * Licensed Materials - Property of IBM
# *
# * IBM SPSS Products: Statistics Common
# *
# * (C) Copyright IBM Corp. 1989, 2022
# *
# * US Government Users Restricted Rights - Use, duplication or disclosure
# * restricted by GSA ADP Schedule Contract with IBM Corp.
# ************************************************************************

import json
import random
import struct
import os
import sys

from enum import Enum

from .parser import SchemaParser
from adapter.clientadapter import clientadapter
from util.xtintl import xtintl


# data model definition
class Role(Enum):
    INPUT = "input"
    TARGET = "target"
    BOTH = "both"
    NONE = "none"
    PARTITION = "partition"
    SPLIT = "split"


class Measure(Enum):
    SCALE = "scale"
    ORDINAL = "ordinal"
    NOMINAL = "nominal"


schemas = {}

_current_wp_name = None
_current_wp_path = None
_current_settings = {}
_output_settings = {}

_client = None


def init_wrapper(name, path):
    """
    Init current extension wrapper.
    :param name: String for wrapper name
    :param path: String for properties schema path
    """
    global schemas
    global _current_wp_name
    global _current_wp_path

    _current_wp_name = name
    _current_wp_path = os.path.dirname(path)

    if _current_wp_name in schemas:
        return schemas[_current_wp_name]
    else:
        schemas[_current_wp_name] = SchemaParser(path)


def get_name():
    """
    Get current extension name.

    :return: String for extension name, the value should be defined in properties schema.
    """
    return schemas[_current_wp_name].get_name()


def get_version():
    """
    Get current extension version.

    :return: String for extension version, the value should be defined in properties schema.
    """
    return schemas[_current_wp_name].get_version()


def get_id():
    """
    Get current extension id

    :return: String for extension id, the value should be defined in properties schema.
    """
    return schemas[_current_wp_name].get_id()


def get_property_spec():
    """
    Get the properties schema of extension

    :return: String for properties schema text
    """
    return schemas[_current_wp_name].get_property_spec()


def get_syntax_spec():
    """
    Get for syntax of extension

    :return: String for syntax text
    """
    return schemas[_current_wp_name].get_syntax_spec()


def set_client(client):
    """
    Set and run the extension client

    :param client: Client adapter object
    """
    global _client
    if not _client and isinstance(client, clientadapter):
        _client = client


def check_settings(settings, fields):
    """
    Check the validity of settings

    :param settings: Json format for user's settings
    :param fields: List for variables metadata
    """
    """ Get the settings from the properties child (if it exists) """
    if "properties" in settings:
        updated_settings = settings["properties"]
    else:
        updated_settings = settings.copy()
    schemas[_current_wp_name].check_settings(updated_settings, fields)
    global _current_settings
    _current_settings = updated_settings.copy()

    """" Get the output_settings child """
    global _output_settings
    if "output_settings" in settings:
        _output_settings = settings["output_settings"].copy()


def get_value(prop_name):
    """
    Get the value of the property.

    :param prop_name: String for property name
    :return: The value of property, if not set, use the default value
    """
    return _current_settings.get(prop_name, schemas[_current_wp_name].get_default_value(prop_name))

def get_output_value(prop_name):
    """
    Get the value of the output property.

    :param prop_name: String for property name
    :return: The value of output property, if not set, use the default value
    """
    return _output_settings.get(prop_name, schemas[_current_wp_name].get_default_value(prop_name))


def get_fields():
    """
    Get all fields from property set

    :return: List for all fields
    """
    return schemas[_current_wp_name].get_fields()


def is_set(prop_name):
    """
    Check if the property is set

    :param prop_name: String for property name
    :return: True if the property is set
    """
    return prop_name in _current_settings


def get_lang_resource(lang):
    """
    Get language resources class instance

    :param lang: language name
    :return: xtintl class instance
    """
    intl = xtintl(_current_wp_path, get_id(), lang)
    return intl


def get_records(iter_id, json_dm, callback):
    """
    Get the data from backend

    :param iter_id: Integer from a backend call to execute()
    :param json_dm: Variable metadata in json format
    :param callback: A callback to receive the result records.
    """
    if _client:
        _client.get_records(iter_id, json_dm, callback)


def create_new_field(name, field_type, measure, modeling_role, description=""):
    """
    Create metadata for a new variable

    :param name: String for variable name
    :param field_type: String for variable type
    :param measure: Measure object for variable measure
    :param modeling_role: Role object for variable role
    :param description: String for variable description

    :return: A dict object for new variable
    """
    metadata = {"description": description, "measure": measure.value, "modeling_role": modeling_role.value}
    field = {"name": name, "type": field_type, "metadata": metadata}

    return field


def put_variables(fields, record_count, start_record, record_data, callback):
    """
    Add new variables to backend

    :param fields: List for variables metadata
    :param record_count: Integer for the count of records added in this call (can be zero for metadata-only)
    :param start_record: Integer for the start record index
    :param record_data: byte array for an array of casewise data
    :param callback: A callback to receive the result code
    """
    if _client and len(fields) > 0:
        json_schema = "http://api.dataplatform.ibm.com/schemas/common-pipeline/datarecord-metadata/datarecord-metadata-v3" \
                      "-schema.json"

        model_id = random.random()

        data_model = {"json_schema": json_schema, "id": model_id, "fields": fields}
        _client.create_variables(json.dumps(data_model), record_count, start_record, record_data, callback)


def generate_output(statjson, callback):
    """
    Generate output in IBM SPSS Statistics

    :param statjson: JSON String describing procedure output
    :param callback: A callback to receive the result code
    :return:
    """
    if _client:
        _client.generate_output(statjson, callback)


def finish():
    """
    Invoke to finish XTension processing when execution is complete.
    """
    if _client:
        _client.finish(0)


class RecordData:
    """
    Use this class to convert data and binary stream
    """

    def __init__(self, rows=None):
        """
        Constructor.

        :param rows: record rows
        """
        self._binaries = None

        self._columns = []
        self._types = []
        if rows is None:
            self._rows = []
        else:
            self._rows = rows

        self._types_map = {"integer": RecordData.CellType.DOUBLE,
                           "double": RecordData.CellType.DOUBLE,
                           "string": RecordData.CellType.STRING}

    class CellType(Enum):
        DOUBLE = "double"
        STRING = "string"

    @staticmethod
    def __pack_double(value):
        if value is None:
            value = -sys.float_info.max
        return struct.pack("<d", value)

    @staticmethod
    def __pack_string(value):
        bytes_value = value.encode('utf-8')
        return struct.pack("<I", len(bytes_value)), bytes_value

    def add_type(self, var_type):
        """
        Add the cell type, the type should

        :param var_type: String or RecordData.CellType for variable type
        """
        if var_type in self._types_map:
            self._types.append(self._types_map[var_type])
        elif isinstance(var_type, RecordData.CellType):
            self._types.append(var_type)

    def add_columns(self, values):
        """
        Add the columns data for RecordData class

        :param values: List for a column of data
        """
        if isinstance(values, (list, tuple)):
            self._columns.append(values)

    def get_binaries(self):
        """
        Get the binary stream

        :return: Bytes for binary stream
        """
        if self._binaries is not None:
            return self._binaries
        elif len(self._rows) > 0:
            binaries = []
            for i in range(len(self._rows)):
                for j in range(len(self._types)):
                    if self._types[j] is RecordData.CellType.DOUBLE:
                        binaries.append(RecordData.__pack_double(self._rows[i][j]))
                    elif self._types[j] is RecordData.CellType.STRING:
                        len_bytes, bytes_value = RecordData.__pack_string(self._rows[i][j])
                        binaries.append(len_bytes)
                        binaries.append(bytes_value)

            self._binaries = b''.join(binaries)
            return self._binaries
        elif 0 < len(self._columns) == len(self._types):
            binaries = []
            for i in range(len(self._columns[0])):
                for j in range(len(self._types)):
                    if self._types[j] is RecordData.CellType.DOUBLE:
                        binaries.append(RecordData.__pack_double(self._columns[j][i]))
                    elif self._types[j] is RecordData.CellType.STRING:
                        len_bytes, bytes_value = RecordData.__pack_string(self._columns[j][i])
                        binaries.append(len_bytes)
                        binaries.append(bytes_value)

            self._binaries = b''.join(binaries)
            return self._binaries

    def get_rows(self):
        """
        Get the row-wise data

        :return: List for row-wise
        """
        if len(self._rows) > 0:
            return self._rows
        elif len(self._columns) > 0:
            self._rows = list(map(list, zip(*self._columns)))
            return self._rows
        elif self._binaries is not None and len(self._types) > 0:
            index = 0
            row_data = []
            for i in range(len(self._types)):
                if self._types[i] is RecordData.CellType.DOUBLE:
                    value = struct.unpack("<d", self._binaries[index:index + 8])[0]
                    index += 8
                    row_data.append(value)
                elif self._types[i] is RecordData.CellType.STRING:
                    length = struct.unpack("<I", self._binaries[index:index + 4])[0]
                    index += 4
                    value = self._binaries[index:index + length].decode("utf-8", "ignore")
                    index += length
                    row_data.append(value)

            self._rows.append(row_data)
            return self._rows

    def get_columns(self):
        """
        Get the column-wise data

        :return: List for row-wise
        """
        if len(self._columns) > 0:
            return self._columns
        elif len(self._rows) > 0:
            self._columns = list(map(list, zip(*self._rows)))
            return self._columns
        elif self._binaries is not None and len(self._types) > 0:
            index = 0
            self._columns = [[]] * len(self._types)
            for i in range(len(self._types)):
                if self._types[i] is RecordData.CellType.DOUBLE:
                    value = struct.unpack("<d", self._binaries[index:index + 8])[0]
                    index += 8
                    self._columns[i].append(value)
                elif self._types[i] is RecordData.CellType.STRING:
                    length = struct.unpack("<I", self._binaries[index:index + 4])[0]
                    index += 4
                    value = self._binaries[index:index + length].decode("utf-8", "ignore")
                    index += length
                    self._columns[i].append(value)

            return self._columns
