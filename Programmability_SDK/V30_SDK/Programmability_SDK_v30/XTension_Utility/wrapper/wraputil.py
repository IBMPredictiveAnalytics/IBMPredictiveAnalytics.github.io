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

# Supported property types in properties schema
PROP_STRING = "string"
PROP_INTEGER = "integer"
PROP_BOOLEAN = "boolean"
PROP_DOUBLE = "double"
PROP_ENUM = "enum"
PROP_ARRAY = "array"
PROP_NUMBER = "number"

# The attribute name in section
SECTION_ID = "id"
SECTION_TYPE = "type"
SECTION_DEFAULT = "default"
SECTION_REQUIRED = "required"
SECTION_ROLE = "role"
SECTION_STORAGE = "storage"
SECTION_PARAMETERS = "parameters"
SECTION_ENUMERATION = "enum"

# Basic types, corresponding built-in type
BASE_PROP_TYPES = {PROP_STRING: str, PROP_INTEGER: int, PROP_DOUBLE: float, PROP_BOOLEAN: bool, PROP_NUMBER: float}

# Arguments list of property types
ARG_NAMES = {PROP_ENUM: ("valid", "value"), PROP_ARRAY: ("type_name", "values")}


# Field type maps to storage
FIELD_TYPE_MAP = {"integer": "numeric", "double": "numeric", "string": "string"}


def switch(tp):
    """
    A switch function for wrapper.
    Use this function to get the callback function for different types to verify the value.

    :param tp: String for property type.

    :return: The verify function for specified type
    """
    fun = {PROP_ENUM: check_enum, PROP_ARRAY: check_array}

    return fun.get(tp, check_default)


def check_enum(valid, value):
    """
    Check the value of the enumeration type.

    :param valid: List for all valid values
    :param value: Value for setting

    :return: Boolean, True is pass the check
    """
    result = False
    if value in valid:
        result = True

    return result


def check_array(type_name, values):
    """
    Check the echo value of the array type. The type is just basic types

    :param type_name: String, the type of the value in the array.
    :param values: Value for setting. It should is list

    :return: Boolean, True is pass the check
    """
    result = False
    if isinstance(values, list) and type_name in BASE_PROP_TYPES:
        for val in values:
            if isinstance(val, BASE_PROP_TYPES.get(type_name)):
                result = True
            else:
                result = False
                break

    return result


def check_default(type_name, value):
    """
    Check the value of the other basic types.

    :param type_name: String, basic type name
    :param value: Value for setting

    :return: Boolean, True is pass the check
    """
    result = False
    if type_name in BASE_PROP_TYPES:
        if isinstance(value, BASE_PROP_TYPES.get(type_name)):
            result = True

    return result


def get_arg_names(prop_type):
    """
    Get the arguments list of specified type.

    :param prop_type: String for property type

    :return: Arguments list
    """
    if prop_type in ARG_NAMES:
        return ARG_NAMES[prop_type]
    else:
        return ["type_name", "value", ]


def get_index(fields, var_name):
    """
    Get the index of the specified variable in the fields list. Return -1 if not found.

    :param fields: Fields List
    :param var_name: String for variable name

    :return: The variable index
    """
    for i in range(len(fields)):
        if fields[i]["name"] == var_name:
            return i

    return -1


class WrapperError(Exception):
    """
    A class to handle extension exception.

    Use this to handle all exceptions for the python wrapper.
    """
    pass
