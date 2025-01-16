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
import os
import json

from wrapper import wraputil


class SchemaParser:
    """
    A class to parse extension properties schema.

    Use this class to parse the properties schema of extension and
    verify the settings which passed from the backend are valid.
    """

    def __init__(self, path):
        """
        Initialize the schema parser

        :param path: String for the properties schema file path
        """
        self.__schema = {}
        self.__summary = {}

        self.__syntax = None

        if os.path.exists(path):
            with open(path) as schema_file:
                schema = json.load(schema_file)
                schema_file.close()
                self.__summary = schema.get("summary")

            self.__schema = schema

            self.__init_properties(schema.get("properties"),
                                   schema.get("complex_types"),
                                   schema.get("validations"),
                                   schema.get("backend_processing"))

    def __init_properties(self, props, cts, vds, bp):
        """
        Initialize properties for the extension schema

        :param props: Properties list which defined in properties schema
        :param cts: complex_types list which defined in properties schema
        :param vds: validations list which defined in properties schema
        :param bp: backend_processing dict with defined in properties schema
        """
        self.__props = {}
        self.__ct = {}
        for ct in cts:
            c = ComplexType(ct)
            self.__ct[c.id] = c.parameters

        for prop in props:
            p = Property(prop)
            self.__props[p.id] = p

        for valid in vds:
            pass

        self.__delete_missing = bp["delete_missing"]
        self.__handle_missing = bp["handle_missing"]
        self.__rounding_weight = bp["rounding_weight"]
        self.__add_cps_table = bp["add_cps_table"]
        self.__records_per_chunk = bp["records_per_chunk"]

    def get_name(self):
        """
        Get current extension name.

        :return: String for extension name, the value should be defined in properties schema.
        """
        return self.__summary.get("label", "")

    def get_version(self):
        """
        Get current extension version.

        :return: String for extension version, the value should be defined in properties schema.
        """
        return self.__summary.get("version", "")

    def get_id(self):
        """
        Get current extension id.

        :return: String for extension id, the value should be defined in properties schema.
        """
        return self.__summary.get("id", "")

    def get_property_spec(self):
        """
        Get the properties schema of extension

        :return: String for properties schema text
        """
        return json.dumps(self.__schema)

    def get_syntax_spec(self):
        """
        Get for syntax of extension

        :return: String for syntax text
        """
        return self.__syntax

    def check_settings(self, settings, fields):
        """
        Check the validity of settings.

        :param settings: json format, send from Statistics backend
        :param fields: List, variables metadata of dataset
        """
        def _check_field(names, storage):
            if isinstance(names, str):
                index = wraputil.get_index(fields, names)
                if index >= 0:
                    field = fields[index]
                    if storage is not None and wraputil.FIELD_TYPE_MAP[field["type"]] not in storage:
                        raise wraputil.WrapperError("Variable type does not match the definition")
                else:
                    raise wraputil.WrapperError("Cannot find the specified variable name: " + names)
            elif isinstance(names, (list, tuple)):
                for name in names:
                    _check_field(name, storage)

        def _check_settings(section_props, section_settings):
            requires = [p for p in section_props if section_props[p].is_required is True]
            for r in requires:
                if r not in section_settings:
                    raise wraputil.WrapperError(r + " is required to execute this extension.")

            if isinstance(section_settings, dict):
                for sub_key, sub_value in section_settings.items():
                    is_reserved = False
                    if sub_key == "original_factors":
                        is_reserved = True
                        tp = wraputil.PROP_STRING
                    elif section_props[sub_key].is_array:
                        tp = section_props[sub_key].subtype
                    else:
                        tp = section_props[sub_key].type

                    if tp in cts:
                        if section_props[sub_key].is_array and isinstance(sub_value, list):
                            for v in sub_value:
                                _check_settings(self.__ct[tp], v)
                        else:
                            _check_settings(self.__ct[tp], sub_value)
                    elif is_reserved == False:
                        if not section_props[sub_key].validation(sub_value):
                            raise wraputil.WrapperError(
                                "Invalid value: {0} in {1}".format(sub_value, section_props[sub_key]))
                        if section_props[sub_key].role == "field":
                            _check_field(sub_value, section_props[sub_key].storage)

        cts = [x for x in self.__ct]
        _check_settings(self.__props, settings)

    def get_default_value(self, prop_name):
        """
        Get default value from properties schema.

        :param prop_name: String for property name.

        :return: The default value for specified property name
        """
        if prop_name in self.__props:
            prop_obj = self.__props[prop_name]
            if prop_obj.is_array:
                tp = prop_obj.subtype
            else:
                tp = prop_obj.type

            if tp in self.__ct:
                ct_default = {}
                for par in self.__ct[tp]:
                    ct_default[par] = self.__ct[tp][par].default

                if prop_obj.is_array:
                    return [ct_default, ]
                else:
                    return ct_default
            else:
                return prop_obj.default
        else:
            return None

    def get_fields(self):
        """
        Get all fields from properties schema.

        :return: List for all fields
        """
        fields = []
        for key, prop in self.__props.items():
            if prop.role == "field":
                fields.append(key)


class BaseSection:
    """
    A class for base section object.

    This is a parent class to parse each property section in the properties schema
    """

    def __init__(self, prop):
        """
        Initialize the section class

        :param prop: Dictionary for section
        """
        self.__id = prop[wraputil.SECTION_ID]

    @property
    def id(self):
        """
        Get section name

        :return: String for section name
        """
        return self.__id


class Property(BaseSection):
    """
    A class for property section.

    Use this class to parse each property section in the properties schema
    """

    def __init__(self, prop):
        """
        Initialize the property class

        :param prop: Dictionary for property section
        """
        BaseSection.__init__(self, prop)
        self.__type = None
        self.__subtype = None
        self.__value = None
        self.__value_args = []

        self.__required = False
        self.__role = None
        self.__storage = None

        if wraputil.SECTION_ENUMERATION in prop:
            self.__type = wraputil.SECTION_ENUMERATION
            self.__value_args.append(prop[wraputil.SECTION_ENUMERATION])
        elif wraputil.SECTION_TYPE in prop:
            prop_type = prop[wraputil.SECTION_TYPE]
            if prop_type.find(wraputil.PROP_ARRAY) >= 0:
                self.__type = wraputil.PROP_ARRAY
                self.__subtype = prop_type[6:-1]

                self.__value_args.append(self.__subtype)
            else:
                self.__type = prop_type
                self.__value_args.append(prop_type)

        if wraputil.SECTION_DEFAULT in prop:
            default_value = prop[wraputil.SECTION_DEFAULT]
            if self.validation(default_value):
                self.default = default_value

        if wraputil.SECTION_REQUIRED in prop:
            self.__required = prop[wraputil.SECTION_REQUIRED]

        if wraputil.SECTION_ROLE in prop:
            self.__role = prop[wraputil.SECTION_ROLE]

        if wraputil.SECTION_STORAGE in prop:
            self.__storage = prop[wraputil.SECTION_STORAGE]

    @property
    def type(self):
        """
        Get the type of property section.

        :return: String for current property type
        """
        return self.__type

    @property
    def subtype(self):
        """
        Get the sub-type of property section.

        :return: String for current property subtype. This value is only valid when the type is "array".
        """
        return self.__subtype

    @property
    def default(self):
        """
        Get default value of property section.

        :return: Default value for current property section
        """
        return self.__value

    @default.setter
    def default(self, value):
        """
        Set the default value for current property section.

        :param value: Default value
        """
        self.__value = value

    @property
    def is_array(self):
        """
        Check if the type of current property section is an array.

        :return: Boolean, True is "array" type
        """
        return self.type == wraputil.PROP_ARRAY

    @property
    def is_required(self):
        """
        Check if the current property section is required.

        :return: Boolean, True is required property
        """
        return self.__required

    @property
    def role(self):
        """
        Get the role of property section.

        :return: String for role value of current property section
        """
        return self.__role

    @property
    def storage(self):
        """
        Get the storage of property section.

        :return: List for storage list of current property section
        """
        return self.__storage

    def validation(self, value):
        """
        Verify that the value is valid

        :param value: The value to verify

        :return: Boolean, True is valid.
        """
        arg_names = wraputil.get_arg_names(self.__type)
        arg_values = self.__value_args + [value]
        if len(arg_names) is not len(arg_values):
            raise wraputil.WrapperError("Incorrect number of arguments.")

        args = dict(zip(arg_names, arg_values))
        fun = wraputil.switch(self.__type)

        return fun(**args)


class ComplexType(BaseSection):
    """
    A class for complex_types section.

    Use this class to parse complex_types in the properties schema
    """

    def __init__(self, prop):
        """
        Initialize the ComplexType class

        :param prop: Dictionary for property section
        """
        BaseSection.__init__(self, prop)
        _parameters = prop[wraputil.SECTION_PARAMETERS]

        result = {}
        for parm in _parameters:
            prop_obj = Property(parm)
            result[prop_obj.id] = prop_obj

        self.__parameters = result

    def __len__(self):
        """
        Get parameter count of current complex_types.

        :return: Integer for parameters count
        """
        return len(self.__parameters)

    @property
    def parameters(self):
        """
        Get the dictionary object of parameters.

        :return: Dictionary for current complex_types
        """
        return self.__parameters
