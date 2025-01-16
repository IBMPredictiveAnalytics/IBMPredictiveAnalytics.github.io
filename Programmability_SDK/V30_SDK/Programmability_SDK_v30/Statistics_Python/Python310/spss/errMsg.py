#/***********************************************************************
# * Licensed Materials - Property of IBM 
# *
# * IBM SPSS Products: Statistics Common
# *
# * (C) Copyright IBM Corp. 1989, 2014
# *
# * US Government Users Restricted Rights - Use, duplication or disclosure
# * restricted by GSA ADP Schedule Contract with IBM Corp. 
# ************************************************************************/

#!/usr/bin/python

"""

This module provides error handling.
"""

#Copyright (C) 2008 by SPSS Inc.

global error,errTable
error=0
errTable={}
noError = "No error."

class errCode:
    """Error code from IBM SPSS Statistics

    Attributes:
        errLevel -- The error Level from IBM SPSS Statistics
        errMsg -- The meaning of an errLevel
    """

    def __init__(self):
        self.errLevel = 0
        self.errMsg = errTable['okay'][str(0)]

    def Reset(self):
        self.errLevel = 0
        self.errMsg = noError

    def SetErrorCode(self, errLevel):
        self.errLevel = errLevel
        find = False
        for key in list(errTable.keys()):
            if str(errLevel) in errTable[key]:
                self.errMsg = errTable[key][str(errLevel)]
                find = True
                break;
        if not find:
            self.errMsg = "unknown error."

    def IsError(self):
        if str(self.errLevel) in errTable['okay']:
            return False
        if str(self.errLevel) in errTable['warning']:
            return False
        return True

class Error(Exception):
    """Base class for exceptions in this module."""
    pass

class SpssError(Error):
    """Exception raised for error in IBM SPSS Statistics.

    Attributes:
        errLevel -- The error Level from IBM SPSS Statistics
        message -- The error message related to the errLevel
    """

    def __init__(self,err):
        self.errLevel = err.errLevel
        errList = [err.errMsg]
        self.args = tuple(errList)

    def __str__(self):
        return (
            "[errLevel %s] %s" %
            (self.errLevel, self.args[0])
            )

class StrError(Error):
    """Execption raised for sample error.
    """
    
    def __init__(self, message):
        errMsgList = [message]
        self.args = tuple(errMsgList)
        
    def __str__(self):
        return ("%s" %(self.args[0]))

__all__ = ["errCode","error","SpssError"]
from . import version
__version__ = version.version


