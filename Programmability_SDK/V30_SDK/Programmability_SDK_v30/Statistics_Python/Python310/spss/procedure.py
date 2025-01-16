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

"""user procedure base class.

This module provides functions that implement a user
procedure.
"""
#Copyright (C) 2008 by SPSS Inc.

from . import PyInvokeSpss,spss
from .errMsg import *
from . import spssutil

class BaseProcedure(object):
    """
    This is the base class for a user procedure. All user procedures must
    be derived from the base class.
    """
    def __init__(self,procName, omsIdentifier=""):
        """Create the user procedure

           --usage:
             BaseProcedure(procName):

           --arguments:
             procName: the user procedure name.

           --examples:

             import spss
             spss.Submit("get file='demo.sav'.")
             class TestProc(spss.BaseProcedure):
                 def execUserProcedure(args):
                     print("execute user procedure")

             myProc = TestProc("name")
             myProc.execProcedure()

        """
        spss.error.Reset()
        procName = spssutil.CheckStr(procName)
        omsIdentifier = spssutil.CheckStr(omsIdentifier)
        self.procName = procName
        self.omsIdentifier = omsIdentifier

    def execProcedure(self):
        """Executes the user procedure.

           --usage:
             execProcedure()

           --examples:
             import spss
             spss.Submit("get file='demo.sav'.")
             class TestProc(spss.BaseProcedure):
                 def execUserProcedure(args):
                     print("execute user procedure")

             myProc = TestProc("name")
             myProc.execProcedure()

        """
        spss.StartProcedure(self.procName, self.omsIdentifier)
        self.execUserProcedure()
        spss.EndProcedure()

    def execUserProcedure(self):
        """should be implemented by the subclass
        """
        pass

__all__ = ["BaseProcedure"]

from . import version
__version__ = version.version

