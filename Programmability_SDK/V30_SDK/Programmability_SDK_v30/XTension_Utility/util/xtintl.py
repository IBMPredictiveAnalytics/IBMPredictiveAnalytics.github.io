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

"""
Python extension localization and internationalization utilities.
"""

import json
import sys
import os


class xtintl:
    """
    Use this class to load language resources.
    """

    def __init__(self, path, name, lang):
        """
        Constructor.
        :param path: String containing the root path to resources.
                     Directory should contain an 'intl' sub-directory.
        :param name: The name prefix used for JSON resources
        :param lang: The language suffix for a localized language
        """
        try:
            if not lang or len(lang) == 0:
                filename = name + ".json"
            else:
                filename = os.path.join("intl", "".join([name, "_", lang, ".json"]))
            path = os.path.join(path, filename)
            with open(path, encoding='utf-8') as json_file:
                self._resources = json.load(json_file)
            self._name = name
            self._lang = lang
        except:
            print("Unexpected error:", sys.exc_info()[0])

    def loadstring(self, key):
        """
        Loads a string from resources.
        :param key: The key name to load from resources
        :return: A translated string or the key if the resource can't be located
        """
        retval = key
        try:
            retval = self._resources[key]
        except KeyError:
            print('Unable to load resource key:', key)
        return retval
