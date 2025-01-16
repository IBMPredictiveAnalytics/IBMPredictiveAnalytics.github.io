#/***********************************************************************
# * Licensed Materials - Property of IBM 
# *
# * IBM SPSS Products: Statistics Common
# *
# * (C) Copyright IBM Corp. 1989, 2017
# *
# * US Government Users Restricted Rights - Use, duplication or disclosure
# * restricted by GSA ADP Schedule Contract with IBM Corp. 
# ************************************************************************/

"""

This module implements a Cursor for a IBM SPSS Statistics Data Source.

"""
#Copyright (C) 2008 by SPSS Inc.


import PyInvokeSpss,spss
from converters import conversions
from errMsg import *
from spss import error
from dataStep import PythonDatasets
import spssutil
import time
import datetime
import binarystream
import weakref

PythonCursors = weakref.WeakValueDictionary()

global newCaseCount
newCaseCount = 0

class BaseCursor(object):

    """The base Cursor class. It creates the cursor object.

       --usage
         Cursor(var,accessType)

       --arguments
         var :A list or tuple, indicating the position of the variables in the
                active dataset,starting with 0 for the first variable in the
                file order. It is useful in read and write mode.
         accessType: A string, indicating the access mode. "r" is read mode, "w" is write
                             mode, and "a" is append mode.
         cvtDates: A list or tuple specifying the index values of the variables in the active dataset to be converted to Python datetime objects.

       --details
         When the index is none, data for all variables are returned. The default mode is
         read mode.

       --examples
         import spss
         spss.Submit("GET FILE='demo.sav'.")
         cur=spss.Cursor()
         r1=cur.fetchone()
         print r1
         cur.close()
         del cur

         import spss
         spss.Submit("GET FILE='demo.sav'.")
         i=(1,3,5)
         cur=spss.Cursor(i)
         r1=cur.fetchone()
         print r1
         cur.close()
         del cur

    """
    #for the user missing value type
    DISCRETE_TYPE = 0
    RANGE_TYPE = 1
    RANGE_DISCRETE_TYPE = 2

    def __init__(self, var = None, accessType = "r", cvtDates=(), isBinary = True):
        self.running = False
        self.accessType = accessType
        if not spss.IsBackendReady():
            try:
                spss.StartSPSS()
            except:
                raise
        spss.error.Reset()
        varCount = spss.GetVariableCount()
        if 0 == varCount:
                spss.error.SetErrorCode(1024)
                raise SpssError,spss.error
        self.conv = conversions.copy()
        self.varTuple = ()
        self.cvtDateIndexes = ()
        self.cvtDates = ()
        if var != None:
            if isinstance(var,(tuple,list)):
                for i in range(len(var)):
                    if isinstance(var[i],int):
                        continue
                    else:
                        spss.error.SetErrorCode(1026)
                        raise SpssError,spss.error
                self.varTuple = tuple(var)
            else:
                spss.error.SetErrorCode(1027)
                raise SpssError,spss.error

        self.var = []
        if var != None:
            self.var = list(var)
        else:
            for i in range(varCount):
                self.var.append(i)

        if cvtDates:
            if cvtDates[0] == "ALL" or cvtDates == "ALL":
                if var != None:
                    cvtDates = tuple(var)
                else:
                    cvtDates = tuple(range(varCount))
            if not set(cvtDates).issubset(self.var):
                spss.error.SetErrorCode(1068)
                raise SpssError,spss.error
            for i in range(len(cvtDates)):
                if isinstance(cvtDates[i],int):
                    if cvtDates[i] <varCount:
                        continue
                    else:
                        spss.error.SetErrorCode(10)
                        raise SpssError,spss.error
                else:
                    spss.error.SetErrorCode(1026)
                    raise SpssError,spss.error

            self.cvtDateIndexes = cvtDates
            self.cvtDates = cvtDates

        result = PyInvokeSpss.GetActive()
        
        result = PyInvokeSpss.cursor(self.conv,self.varTuple,self.accessType)
        self.datasrcInfo = result[0]
        spss.error.SetErrorCode(result[1])
        if spss.error.IsError():
            raise SpssError,spss.error

        if not self.datasrcInfo:
                spss.error.SetErrorCode(1024)
                raise SpssError,spss.error
            
        result = PyInvokeSpss.IsUseOrFilter()
        isUseOrFilter = result[0]
        errLevel = result[1]
        
        if isUseOrFilter != 0 and "w" == accessType:
            isBinary = False
        
        if isBinary:
            self.binaryStream = binarystream.CursorBinary(self.varTuple, accessType)
            PyInvokeSpss.SetMode(1)

        self.running = True
        self.newvars = []
        self.isBinary = isBinary
        self.__id = id(self)
        PythonCursors[self.__id] = self

    def __del__(self):
        if self.running:
            spss.error.Reset()
            PyInvokeSpss.close()
            ## clear Python dataset
            for ds in PythonDatasets.values():
                ds._Dataset__destroy()
                del ds
            PythonDatasets.clear()
            
            global newCaseCount
            newCaseCount = 0
            
            if self.isBinary:
                del self.binaryStream
                
            try:
                PythonCursors.pop(self.__id)
                self.__id = 0
            except KeyError:
                pass
            except:
                pass
           
    def __IsClose__(self):
        """Check whether or not curosr close. If close, throw an exception.
        """
        spss.error.Reset()
        if not self.running:
            spss.error.SetErrorCode(1030)
            raise SpssError,spss.error

    def close(self):
        """Close the cursor.

           --example
             import spss
             try:
                 spss.Submit("GET FILE='demo.sav'.")
                 cur=spss.Cursor()
                 print cur.fetchone()
                 cur.close()
                 print cur.fetchone()
             except:
                 print spss.GetLastErrorLevel()
                 print spss.GetLastErrorMessage()

        """
        spss.error.Reset()
        if self.running:
            PyInvokeSpss.close()
            ## clear Python dataset
            for ds in PythonDatasets.values():
                ds._Dataset__destroy()
                del ds
            PythonDatasets.clear()
            
            global newCaseCount
            newCaseCount = 0
            
            if self.isBinary:
                PyInvokeSpss.SetMode(0)
                del self.binaryStream
                
            try:
                PythonCursors.pop(self.__id)
                self.__id = 0
            except KeyError:
                pass
            except:
                pass

        self.running = False

    def GetCaseCount(self):
        """Returns the total number of cases (rows) in the procedure datasource.

            --usage
              GetCaseCount()

            --details
              The return value will be -1 when the number of cases (rows) is
              unknown.
              The return value may be unknown prior to the complete pass of the
              procedure datasource.

            --value
              The number of cases (rows) in the procedure data source.

            --example
              import spss
              try:
                  spss.Submit("GET FILE='demo.sav'.")
                  cur = spss.Cursor(accessType='w')
                  print cur.GetCaseCount()
                  cur.close()
              except spss.SpssError:
                  print "Error."
              print spss.GetLastErrorMessage()

              """
        self.__IsClose__()
        spss.error.Reset()
        result = PyInvokeSpss.GetCaseCountInProcDS()
        out = result[0]
        errLevel = result[1]
        if self.isBinary:
            if self.binaryStream.getEndChangesStatus():
                global newCaseCount
                newCaseCount = self.binaryStream.getNewCaseCount()
                out += newCaseCount

        spss.error.SetErrorCode(errLevel)
        if spss.error.IsError():
            raise SpssError,spss.error

        return out

    def GetVariableCount(self):
        """Returns the number of variables in the procedure data source.

           --usage
             cur.GetVariableCount()

           --value
             Return the number of variables in the procedure data source.

           --example
             import spss
             try:
                 spss.Submit("GET FILE='demo.sav'.")
                 cur = spss.Cursor()
                 varCount = cur.GetVariableCount()
                 print varCount
                 cur.close()
             except spss.SpssError:
                 print spss.GetLastErrorMessage()
        """
        self.__IsClose__()

        spss.error.Reset()
        result = PyInvokeSpss.GetVariableCountInProcDS()
        out = result[0]
        errLevel = result[1]

        spss.error.SetErrorCode(errLevel)
        if spss.error.IsError():
            raise SpssError,spss.error

        return out

    def GetVariableFormat(self,index):
        """Returns a string containing the display format for the indexed
    variable in the procedure data source.

        --usage
          GetVariableFormat(index)

        --arguments
          index: The position of the variable in the procedure data source,
          starting with 0 for the first variable in file order.

        --details
          Possible formats include:
          "A": Standard characters.
          "ADATE": American date of the general form mm/dd/yyyy.
          "JDATE": Julian date of the general form yyyyddd.
          "AHEX": Hexadecimal characters.
          "CCA": custom currency format 1.
          "CCB": custom currency format 2.
          "CCC": custom currency format 3.
          "CCD": custom currency format 4.
          "CCE": custom currency format 5.
          "COMMA": Numbers with commas as grouping symbol and period as
                   decimal indicator. For example: 1,234,567.89.
          "DATE": International date of the general form dd-mmm-yyyy
          "DATETIME": Date and time of the general form dd-mmm-yyyy
                      hh:mm:ss.ss.
          "DOLLAR": Numbers with a leading dollar sign ($), commas as
                    grouping symbol, and period as decimal indicator.
                    For example: $1,234,567.89.
          "F": Standard numeric.
          "IB": Integer binary.
          "PIBHEX": Hexadecimal of PIB.
          "DOT": Numbers with period as grouping symbol and comma as decimal
                 indicator. For example: 1.234.567,89.
          "DTIME": Days and time of the general form dd hh:mm:ss.ss.
          "E": Scientific notation.
          "EDATE": European date.
          "MONTH": Month.
          "MOYR": Month and year.
          "N": Restricted numeric.
          "P": Packed decimal.
          "PIB": Positive integer binary.
          "PK": Unsigned packed decimal.
          "QYR": Quarter and year of the general form qQyyyy.
          "WKYR": Week and year.
          "PCT": Percent sign after numbers.
          "RB": Real binary.
          "RBHEX": Hexadecimal of RB.
          "SDATE": Sortable date of the general form yyyy/mm/dd.
          "TIME":Time of the general form hh:mm:ss.ss.
          "WKDAY": Day of the week.
          "Z": Zoned decimal.

        --value
          A string containing the display format.
          Each format string contains a numeric component after the format
          name that indicates the defined width, and optionally the number
          of decimal positions for numeric formats. For example, A4 is a
          string format with maximum width of four bytes, and F8.2 is a
          standard numeric format with a display format of 8 digits,
          including two decimal positions and a decimal indicator.

        --example
          import spss
          try:
              spss.Submit("GET FILE='demo.sav'.")
              cur = spss.Cursor()
              varcount= cur.GetVariableCount()
              for index in xrange(varcount):
                 print cur.GetVariableFormat(index)
              cur.close()
          except spss.SpssError:
              print "Error."
              print spss.GetLastErrorMessage()
        """
        self.__IsClose__()

        spss.error.Reset()
        #Check the type of the argument
        if not isinstance(index,(int,long)):
            error.SetErrorCode(1000)
            raise SpssError,spss.error

        result = PyInvokeSpss.GetVariableFormatInProcDS(index)
        out = result[0]
        errLevel = result[1]

        spss.error.SetErrorCode(errLevel)
        if spss.error.IsError():
            raise SpssError,spss.error

        return out

    def GetVariableLabel(self,index):
        """Returns a character string containing the variable label
    for the indexed variable in the procedure data source.

        --usage
          GetVariableLabel(index)

        --arguments
          index: The position of the variable in the procedure data source,
          starting with 0 for the first variable in file order.

        --details
          If the variable does not have a defined variable label, an
          empty string is returned.

        --value
          A string containing the variable label.

        --example
          import spss
          try:
              spss.Submit("GET FILE='demo.sav'.")
              cur = spss.Cursor()
              index = 0
              print cur.GetVariableLabel(index)
              cur.close()
          except spss.SpssError:
              print "Error."
              print spss.GetLastErrorMessage()
        """
        self.__IsClose__()

        spss.error.Reset()

        if not isinstance(index,(int,long)):
            spss.error.SetErrorCode(1000)
            raise SpssError,spss.error

        result = PyInvokeSpss.GetVariableLabelInProcDS(index)
        out = result[0]
        errLevel = result[1]

        spss.error.SetErrorCode(errLevel)
        if spss.error.IsError():
            raise SpssError,spss.error

        return out

    def GetVariableMeasurementLevel(self,index):
        """Returns a string value that indicates the measurement level for
    the indexed variable in the procedure data source.

        --usage
          GetVariableMeasurementLevel(index)

        --arguments
          index: The position of the variable in the procedure data source,
                 starting with 0 for the first variable in file order.

        --details
          The value returned can be: "nominal", "ordinal", "scale", or
          "unknown"."unknown" only occurs for numeric variables prior
          to the first data pass when the measurement level has not been
          explicitly set, such as data read from an external source or
          newly created variables. The measurement level for string
          variables is always "nominal".

        --value
          A string containing the measurement level.

        --example
          import spss
          try:
              spss.Submit("GET FILE='demo.sav'.")
              cur = spss.Cursor()
              varcount = cur.GetVariableCount()
              for index in xrange(varcount):
                   print cur.GetVariableMeasurementLevel(index)
              cur.close()
          except spss.SpssError:
              print "Error."
              print spss.GetLastErrorMessage()

        """
        self.__IsClose__()

        spss.error.Reset()
        #Check the type of the argument
        if not isinstance(index,(int,long)):
            spss.error.SetErrorCode(1000)
            raise SpssError,spss.error

        measurementlevel = {0:"error",
                            1:"unknown",
                            2:"nominal",
                            3:"ordinal",
                            4:"scale"
                            }

        result = PyInvokeSpss.GetVariableMeasurementLevelInProcDS(index)
        out = measurementlevel[result[0]]
        errLevel = result[1]

        spss.error.SetErrorCode(errLevel)
        if spss.error.IsError():
            raise SpssError,spss.error

        return out

    def GetVariableName(self,index):
        """Returns a character string containing the variable name for
    the indexed variable in the procedure data source.

        --usage
          GetVariableName(index)

        --arguments
          index: The position of the variable in the procedure data source,
                 starting with 0 for the first variable in file order.

        --value
          A string containing the variable name.

        --example
          import spss
          try:
              spss.Submit("GET FILE='demo.sav'.")
              cur = spss.Cursor()
              index = 0
              print cur.GetVariableName(index)
              cur.close()
          except spss.SpssError:
              print "Error."
              print spss.GetLastErrorMessage()

        """
        self.__IsClose__()

        spss.error.Reset()

        if not isinstance(index,(int,long)):
            spss.error.SetErrorCode(1000)
            raise SpssError,spss.error

        result = PyInvokeSpss.GetVariableNameInProcDS(index)
        out = result[0]
        errLevel = result[1]

        spss.error.SetErrorCode(errLevel)
        if spss.error.IsError():
            raise SpssError,spss.error

        return out

    def GetVariableType(self,index):
        """Returns 0 for numeric variables or the defined length for string
    variables for the indexed variable in the procedure data source.

        --usage
          GetVariableType(index)

        --arguments
          index: The position of the variable in the procedure data source,
          starting with 0 for the first variable in file order.

        --details
          0 indicates that the variable is a numeric.
          A value n, greater than zero, indicates a string variable
          with maximum length n bytes.

        --value
          An integer indicating the type of a variable.

        --example
          import spss
          try:
              spss.Submit("GET FILE='Employee data.sav'.")
              cur = spss.Cursor()
              varcount = cur.GetVariableCount()
              for index in xrange(varcount):
                 print cur.GetVariableType(index)
              cur.close()
          except spss.SpssError:
              print "Error."
          print spss.GetLastErrorMessage()

        """
        self.__IsClose__()

        spss.error.Reset()

        if not isinstance(index,(int,long)):
            spss.error.SetErrorCode(1000)
            raise SpssError,spss.error

        result = PyInvokeSpss.GetVariableTypeInProcDS(index)
        out = result[0]
        errLevel = result[1]

        spss.error.SetErrorCode(errLevel)
        if spss.error.IsError():
            raise SpssError,spss.error

        return out

    def GetVarMissingValues(self,index):
        """Returns a tuple of the indexed variable missing values from the procedure data source.
        For numeric variables, the tuple will consist of one of the following:
           Up to three discrete missing values
           Two missing values that are the endpoints of a range
           Three missing values that are the endpoints of a range and a single discrete missing value.

        --usage
          GetVarMissingValues(index)

        --arguments
          index: The position of the variable in the procedure data source,
          starting with 0 for the first variable in file order.

        --value
          A tuple indicating the missing values.

        --example
          import spss
          try:
              spss.Submit("GET FILE='Employee data.sav'.")
              cur = spss.Cursor()
              varcount = cur.GetVariableCount()
              for index in xrange(varcount):
                 print cur.GetVarMissingValues(index)
              cur.close()
          except spss.SpssError:
              print "Error."
          print spss.GetLastErrorMessage()

       """
        self.__IsClose__()

        spss.error.Reset()

        if not isinstance(index,(int,long)):
            spss.error.SetErrorCode(1000)
            raise SpssError,spss.error

        type = self.GetVariableType(index)

        if type == 0:
            result = PyInvokeSpss.GetVarNMissingValues(index)
            out = result[:-1]
        else:
            result = PyInvokeSpss.GetVarCMissingValues(index)
            out = list(result[:-1])
            out[0] = 0
            for i in range(len(out) - 1):
                temp = out[i+1]
                if temp is None:
                    continue
                else:
                    out[i+1] = (str(temp))[:type]

        errLevel = result[-1]

        spss.error.SetErrorCode(errLevel)
        if spss.error.IsError():
            raise SpssError,spss.error

        return tuple(out)

    def GetVarAttributeNames(self,index):
        """Returns a tuple of attribute names for the indexed variable in the
           procedure data source.

           --usage:
             GetVarAttributeNames(index)

           --arguments
             index: The position of the variable in the procedure dta source,starting with
                    0 for the first variable in file order.

           --examples:

             import spss
             spss.Submit("get file = 'demo.sav'.")
             spss.Submit("variable attribute variables=age, marital attribute=test[1] ('1') test[2] ('2').")
             cur=spss.Cursor()
             var = cur.GetVarAttributeNames(0)
             print var
             cur.close()

        """
        self.__IsClose__()

        spss.error.Reset()

        if not isinstance(index,(int,long)):
            spss.error.SetErrorCode(1000)
            raise SpssError,spss.error

        result = PyInvokeSpss.GetVarAttributeNamesInProcDS(index)
        out = result[0]
        errLevel = result[1]

        spss.error.SetErrorCode(errLevel)
        if spss.error.IsError():
            raise SpssError,spss.error

        return out

    def GetVarAttributes(self,index,attrName):
        """Returns a tuple of the variable attributes for attribute name in the
        indexed variable in the procedure data source.

        --usage:
          GetVarAttributes(index,attrName)

        --arguments:
          index: The position of the variable in the procedure dta source,starting with
                    0 for the first variable in file order.
          attrName: The attribute name. Get this from GetVarAttributeNames(index)

        --examples:

          import spss
          spss.Submit("get file = 'demo.sav'.")
          spss.Submit("variable attribute variables=age, marital attribute=test[1] ('1') test[2] ('2').")
          cur=spss.Cursor()
          var = cur.GetVarAttributeNames(0)
          print var
          attrValue=cur.GetVarAttributes(0,"test")
          print attrValue
          cur.close()

        """
        self.__IsClose__()

        spss.error.Reset()

        if not isinstance(index,(int,long)):
            spss.error.SetErrorCode(1000)
            raise SpssError,spss.error

        attrName = spssutil.CheckStr(attrName)
        result = PyInvokeSpss.GetVarAttributesInProcDS(index,attrName)

        out = result[0]
        errLevel = result[1]

        spss.error.SetErrorCode(errLevel)
        if spss.error.IsError():
            raise SpssError,spss.error

        return out
    def GetDataFileAttributeNames(self):
        """Returns a tuple of attribute names of the active data source.

           --usage:
             GetDataFileAttributeNames()

           --examples:

             import spss
             spss.Submit("get file = 'demo.sav'.")
             spss.Submit("DATAFILE ATTRIBUTE ATTRIBUTE=OriginalVersion ('1').")
             cur=spss.Cursor()
             var = cur.GetDataFileAttributeNames()
             print var
             cur.close()

        """
        self.__IsClose__()

        spss.error.Reset()

        result = PyInvokeSpss.GetDataFileAttributeNamesInProcDS()
        out = result[0]
        errLevel = result[1]

        spss.error.SetErrorCode(errLevel)
        if spss.error.IsError():
            raise SpssError,spss.error

        return out

    def GetDataFileAttributes(self,attrName):
        """Returns a tuple of the attributes for attribute name of the active data source.

        --usage:
          GetDataFileAttributes(attrName)

        --arguments:
          attrName: The attribute name. Get this from GetDataFileAttributeNames()

        --examples:

          import spss
          spss.Submit("get file = 'demo.sav'.")
          spss.Submit("DATAFILE ATTRIBUTE ATTRIBUTE=OriginalVersion ('1').")          
          cur=spss.Cursor()
          var = cur.GetDataFileAttributeNames()
          print var
          attrValue=cur.GetDataFileAttributes(var[0])
          print attrValue
          cur.close()

        """
        self.__IsClose__()

        spss.error.Reset()
        attrName = spssutil.CheckStr(attrName)

        result = PyInvokeSpss.GetDataFileAttributesInProcDS(attrName)

        out = result[0]
        errLevel = result[1]

        spss.error.SetErrorCode(errLevel)
        if spss.error.IsError():
            raise SpssError,spss.error

        return out

    def GetMultiResponseSetNames(self):
        """Returns multiple response set names from active data source.
     
         --examples:
  
         import spss
         spss.Submit(["DATA LIST /Newspaper 1 TV 3 Web 5.",
"Begin data.",
"1 1 1",
"1 1 1",
"1 1 1",
"End data.",
"MRSETS",
"  /MDGROUP NAME=$mltnews LABEL='News sources' ",
"   VARIABLES=Newspaper TV Web ",
"   VALUE=1.",
"MRSETS",
"  /MCGROUP NAME=$abc LABEL='abcd' VARIABLES=Newspaper TV."
])
         cur = spss.Cursor()
         names = cur.GetMultiResponseSetNames()
         print names
         for i in range(len(names)):
             print cur.GetMultiResponseSet(names[i])
         cur.close()
    """
        self.__IsClose__()

        spss.error.Reset()
        
        result = PyInvokeSpss.GetMultiResponseSetNamesInProcDS()
        out = result[0]
        errLevel = result[1]
        
        error.SetErrorCode(errLevel)
        if error.IsError():
            raise SpssError,spss.error
        
        return out

    def GetMultiResponseSet(self,mrsetName):
        """Returns multiple response set from procedure data source.
        
            --arguments:
                mrsetName: The multi response name. Get this from GetMultiResponseSetNames()

            --examples:

            import spss
            spss.Submit(["DATA LIST /Newspaper 1 TV 3 Web 5.",
"Begin data.",
"1 1 1",
"1 1 1",
"1 1 1",
"End data.",
"MRSETS",
"  /MDGROUP NAME=$mltnews LABEL='News sources' ",
"   VARIABLES=Newspaper TV Web ",
"   VALUE=1.",
"MRSETS",
"  /MCGROUP NAME=$abc LABEL='abcd' VARIABLES=Newspaper TV."
])
            cur = spss.Cursor()
            names = cur.GetMultiResponseSetNames()
            print names
            for i in range(len(names)):
                print cur.GetMultiResponseSet(names[i])
            cur.close()
    """
        self.__IsClose__()
        
        spss.error.Reset()

        #Check the type of the argument
        mrsetName = spssutil.CheckStr(mrsetName.upper())

        result = PyInvokeSpss.GetMultiResponseSetInProcDS(mrsetName)
        
        errLevel = result[-1]
        spss.error.SetErrorCode(errLevel)
        if spss.error.IsError():
            raise SpssError,spss.error
        
        return result[:-1]  
        
    def GetVariableRole(self,index):
        """Returns a string value that indicates the role for
        the indexed variable in the procedure data source.

        --usage
          GetVariableRole(index)

        --arguments
          index: The position of the variable in the procedure data source,
                 starting with 0 for the first variable in file order.

        --details
          The value returned can be: "Input", "Target", "Both",
          "None", "Partition", "Split", or "Frequency".

        --value
          A string containing the role.

        --example
          import spss
          try:
              spss.Submit("GET FILE='demo.sav'.")
              cur = spss.Cursor()
              varcount = cur.GetVariableCount()
              for index in xrange(varcount):
                   print cur.GetVariableRole(index)
              cur.close()
          except spss.SpssError:
              print "Error."
              print spss.GetLastErrorMessage()

        """
        self.__IsClose__()

        spss.error.Reset()
        #Check the type of the argument
        if not isinstance(index,(int,long)):
            spss.error.SetErrorCode(1000)
            raise SpssError,spss.error

        varRole = { 0:"Input",
                    1:"Target",
                    2:"Both",
                    3:"None",
                    4:"Partition",
                    5:"Split",
                    6:"Frequency"
                  }

        result = PyInvokeSpss.GetVariableRoleInProcDS(index)
        out = varRole[result[0]]
        errLevel = result[1]

        spss.error.SetErrorCode(errLevel)
        if spss.error.IsError():
            raise SpssError,spss.error

        return out
        
    def GetSelectedRowCount(self):
        """Returns the selected number of cases (rows) in the procedure datasource.

            --usage
              GetSelectedRowCount()

            --details
              The return value will be -1 when the number of cases (rows) is
              unknown.
              The return value may be unknown prior to the complete pass of the
              procedure datasource.

            --value
              The number of selected cases (rows) in the procedure data source.

            --example
              import spss
              try:
                  spss.Submit("GET FILE='demo.sav'.")
                  cur = spss.Cursor()
                  print cur.GetSelectedRowCount()
                  cur.close()
              except spss.SpssError:
                  print "Error."
              print spss.GetLastErrorMessage()

              """
        self.__IsClose__()
        spss.error.Reset()
        result = PyInvokeSpss.GetSelectedRowCountInProcDS()
        out = result[0]
        errLevel = result[1]

        spss.error.SetErrorCode(errLevel)
        if spss.error.IsError():
            raise SpssError,spss.error

        return out


class ReadCursor(BaseCursor):
    """The ReadCursor derives from BaseCursor. It procides functions to read case data.
    """
    def reset(self):
        """Resets the current cursor position to the first case
        (row) of data in the procedure data source.

           --usage:
             reset()

           --example
             import spss
             spss.Submit("GET FILE='demo.sav'.")
             cur=spss.Cursor()
             print cur.fetchmany(4)
             print cur.fetchone()
             cur.reset()
             print cur.fetchmany(4)
             print cur.fetchone()
             cur.close()
             del cur

        """
        self.__IsClose__()

        spss.error.Reset()

        spss.error.SetErrorCode(PyInvokeSpss.ResetDataPass())
        if spss.error.IsError():
            raise SpssError,spss.error
        
        if self.isBinary:
            self.binaryStream.reset()

    def SetFetchVarList(self, var = None):
        """Set variable list to fetch data.

          --usage:
            SetFetcVarList(var)

          --arguments:
            var: A tuple or list to save the variable index.

          --examples:

             import spss
             spss.Submit("get file = 'demo.sav'.")
             var=(3,4,5)
             cur = spss.Cursor(var)
             cur.fetchone()
             cur.fetchmany(5)
             var=(1,2,3,6,7)
             cur.SetFetchVarList(var)
             cur.fetchone()
             cur.close()
        """
        self.__IsClose__()

        spss.error.Reset()

        self.varTuple = ()
        if var != None:
            if isinstance(var,(tuple,list)):
                for i in range(len(var)):
                    if isinstance(var[i],int):
                        continue
                    else:
                        spss.error.SetErrorCode(1026)
                        raise SpssError,spss.error
                self.varTuple = tuple(var)
            else:
                spss.error.SetErrorCode(1027)
                raise SpssError,spss.error

        if self.isBinary:
            reCases = self.binaryStream.getRemainCaseCount()
            pos = self.GetCaseCount() - reCases
            del self.binaryStream
            self.binaryStream = binarystream.CursorBinary(self.varTuple, self.accessType, pos)
        else:
            spss.error.SetErrorCode(PyInvokeSpss.SetFetchVarList(self.datasrcInfo,self.varTuple))
            if spss.error.IsError():
                raise SpssError,spss.error

        if var != None:
            self.var = list(var)
        else:
            self.var = []

    def IsEndSplit(self):
        """Returns a Python boolean value to indicate whether or not the current procedure data
           cursor is at the end of a split.

           --usage:
             IsEndSplit()

           --examples:

             import spss
             spss.Submit("get file='Employee data.sav'.")
             spss.Submit("SORT CASES BY GENDER.")
             spss.Submit("SPLIT FILE LAYERED BY GENDER.")
             cur=spss.Cursor()
             for i in range(spss.GetCaseCount()*2):
                case = cur.fetchone()
                splitend = cur.IsEndSplit()
                if not splitend and case is None:
                    break
                if splitend:
                    print "Found split end. New split begins at case: ", (i+1)

             cur.close()

        """
        self.__IsClose__()

        spss.error.Reset()

        if self.isBinary:
            out = self.binaryStream.isEndSplit()
        else:
            result = PyInvokeSpss.IsEndSplit()
            out = result[0]
            errLevel = result[1]
    
            spss.error.SetErrorCode(errLevel)
            if spss.error.IsError():
                raise SpssError,spss.error
        
        if out == 1:
            return True
        else:
            return False

    def SetUserMissingInclude(self,incMissing):
        """Controls how user missing values are handled. When incMissing is false,
        user missing values are converted to None,otherwize they are not converted.
        The default is false.

           --usage:
             SetUserMissingInclude(incMissing)

           --arguments:
             incMissing: false converts missing values to None,true does not convert
                         missing values.

           --examples:

             import spss
             spss.Submit("get file = 'demo.sav'.")
             i=(3,4,5)
             cur = spss.Cursor(i)
             cur.SetUserMissingInclude(False)
             print cur.fetchmany(4)
             cur.close()
             cur = spss.Cursor(i)
             cur.SetUserMissingInclude(True)
             print cur.fetchmany(4)
             cur.close()

        """
        self.__IsClose__()

        spss.error.Reset()

        if not isinstance(incMissing,bool):
            spss.error.SetErrorCode(1008)
            raise SpssError,spss.error

        if self.isBinary:
            self.binaryStream.setUserMissingInclude(incMissing)
        else:
            PyInvokeSpss.SetUserMissingInclude(self.datasrcInfo,incMissing)

    def fetchone(self):
        """Returns the next case (row) of data from the active dataset as
        a Python tuple.

           --usage:
             fetchone()

           --details:
             The next row of data is with respect to the current 
             cursor position. When there are no more rows of data,
             returns None.

           --example:
             import spss
             spss.Submit("GET FILE='demo.sav'.")
             casecount = spss.GetCaseCount()
             i=(1,3,5)
             cur=spss.Cursor(i)
             for index in xrange(casecount):
                 r=cur.fetchone()
                 print r
             cur.close()

        """
        self.__IsClose__()

        spss.error.Reset()
        
        if self.isBinary:
            self.binaryStream.saveData()
            size = self.binaryStream.getRemainCaseCount()
            if 0 == size:
                spss.error.SetErrorCode(23)
            data = self.binaryStream.fetchData()
        else:
            result = PyInvokeSpss.fetchone(self.datasrcInfo)
            errLevel = result[0]
            spss.error.SetErrorCode(errLevel)
            if spss.error.IsError():
                raise SpssError,spss.error
    
            data = result[1]
        
        if self.cvtDateIndexes:
            self._getDateIndexes()

        data = self._dateconverter(data)
        return data

    def fetchmany(self, size):
        """Returns the  next n cases (rows) of data from the procedure data
        source as a list of Python tuples.

           --usage:
             fetchmany()

           --arguments
             size: An integer indicating the number of rows to return.

           --details
             The next row of data is with respect to the current 
             cursor position. When n is greater than the number of
             remaining rows, returns all remaining rows. When there
             are no more rows of data, returns an empty tuple.

           --example
             import spss
             spss.Submit("GET FILE='demo.sav'.")
             cur=spss.Cursor()
             print cur.fetchmany(4)
             cur.close()

        """
        self.__IsClose__()

        spss.error.Reset()

        if not isinstance(size,(int,long)):
            spss.error.SetErrorCode(1000)
            raise SpssError,spss.error

        if size < 1:
            spss.error.SetErrorCode(1035)
            raise SpssError,spss.error

        if self.isBinary:
            data = []
            remainsCase = 0
            self.binaryStream.saveData()
            if self.binaryStream.isSplit():
                remainsCase = self.binaryStream.getSplitRemainCaseCount()
            else:
                remainsCase = self.binaryStream.getRemainCaseCount()
            if 0 == remainsCase:
                spss.error.SetErrorCode(23)
            else:
                hasError = False
                if size >= remainsCase:
                    size = remainsCase
                    hasError = True
                
                for i in xrange(size):
                    data.append(self.binaryStream.fetchData())
                
                if hasError:
                    self.binaryStream.nextcase()
                    spss.error.SetErrorCode(67)
        else:
            result = PyInvokeSpss.fetchmany(self.datasrcInfo,size)
            errLevel = result[0]
            spss.error.SetErrorCode(errLevel)
            if spss.error.IsError():
                raise SpssError,spss.error
            data = list(result[1])
        
        if self.cvtDateIndexes:
            self._getDateIndexes()

        for item in range(len(data)):
            data[item] = self._dateconverter(data[item])
        
        return tuple(data)

    def fetchall(self):
        """Returns the remaining cases (rows) of data from the procedure
        data source as a list Python tuples.

           --usage:
             fetchall()

           --details:
             The remaining rows of data are with respect to the current
             cursor position. When there are no more rows of data,
             returns an empty tuple.

           --examples:

             import spss
             spss.Submit("GET FILE='demo.sav'.")
             cur=spss.Cursor()
             print cur.fetchall()
             cur.close()

        """
        self.__IsClose__()

        spss.error.Reset()

        if self.isBinary:
            data = []
            size = 0
            self.binaryStream.saveData()
            if self.binaryStream.isSplit():
                size = self.binaryStream.getSplitRemainCaseCount()
            else:
                size = self.binaryStream.getRemainCaseCount()
            if 0 == size:
                spss.error.SetErrorCode(23)
            else:
                for i in xrange(size):
                    data.append(self.binaryStream.fetchData())
                    
                self.binaryStream.nextcase()
        else:
            result = PyInvokeSpss.fetchall(self.datasrcInfo)
            errLevel = result[0]
            spss.error.SetErrorCode(errLevel)
            if spss.error.IsError():
                raise SpssError,spss.error
            data = list(result[1])

        if self.cvtDateIndexes:
            self._getDateIndexes()

        for item in range(len(data)):
            data[item] = self._dateconverter(data[item])
        
        return tuple(data)
        
    def _dateconverter(self, row):
        """convert any values in row specified in cvtDates to Python date values."""
        if not self.cvtDateIndexes:
            return row
        if isinstance(row[self.cvtDates[0]], datetime.datetime):  #already converted?
            return row
        row = list(row)
        for index in self.cvtDates:
            if row[index] is not None:
                row[index] = spssutil.CvtSpssDatetime(row[index])
        return tuple(row)
        
    def _isDateVariable(self,var):
        vfmt = self.GetVariableFormat(var)
        return spssutil.isDateVariable(vfmt)
        
    def _getDateIndexes(self):
        self.cvtDateIndexes = filter(self._isDateVariable, self.cvtDateIndexes)
        self.cvtDates = [self.var.index(item) for item in self.cvtDateIndexes]

class BaseWriteAppendCursor(object):
    """This is a base class for the WriteCursor and AppendCursor.
    """
    def SetValueChar(self,varName,varValue):
        """Sets a data value (varValue) for the specified string variable
           (varName) at the current cursor position.

           --usage:
             SetValueChar(varName,varValue)

           --arguments:
             varName: The string name of the variable for which to set the value.
             varValue: The string value to set.

           --examples:

             import spss
             spss.Submit("get file = 'demo.sav'.")
             cur = spss.Cursor(accessType = "w")
             cur.AllocNewVarsBuffer(16)
             varName=("test1","test2")
             varType=(8,8)
             cur.SetVarNameAndType(varName,varType)
             cur.CommitDictionary()
             caseCount = cur.GetCaseCount()
             for i in range(caseCount):
                 cur.fetchone()
                 cur.SetValueChar("test1","ABC")
                 cur.SetValueChar("test2","ABCDEF")
                 cur.CommitCase()

             cur.close()

        """
        self.__IsClose__()

        spss.error.Reset()
        varName = spssutil.CheckStr(varName)
        varValue = spssutil.CheckStr(varValue)

        if self.isBinary:
            varName = varName.rstrip()
            self.binaryStream.setValueForChange(varName, varValue)
        else:
            spss.error.SetErrorCode(PyInvokeSpss.SetValueChar(varName,varValue))
            if spss.error.IsError():
                raise SpssError,spss.error

    def SetValueNumeric(self,varName,varValue):
        """Sets a data value for the specified numeric variable at the current position.

           --usage:
             SetValueNumeric(varName,varValue)

           --arguments:
             varName: The string name of the variable for which to set the value.
             varValue: The numeric value to set, which can be integer or float (double).

           --examples:

             import spss
             spss.Submit("get file = 'demo.sav'.")
             cur = spss.Cursor(accessType = "w")
             cur.AllocNewVarsBuffer(16)
             varName=("test1","test2")
             varType=(0,0)
             cur.SetVarNameAndType(varName,varType)
             cur.CommitDictionary()
             caseCount = cur.GetCaseCount()
             for i in range(caseCount):
                 cur.fetchone()
                 cur.SetValueNumeric(varName[0],1.1*i)
                 cur.SetValueNumeric(varName[1],1.1*i)
                 cur.CommitCase()

             cur.close()

        """
        self.__IsClose__()

        spss.error.Reset()
        varName = spssutil.CheckStr(varName)

        if varValue is None:
            return

        if isinstance(varValue, time.struct_time):
            varValue = datetime.datetime(varValue.tm_year, varValue.tm_mon, varValue.tm_mday, varValue.tm_hour, varValue.tm_min, varValue.tm_sec)
        if isinstance(varValue, datetime.date):
            if not isinstance(varValue, datetime.datetime):
                varValue = datetime.datetime(varValue.year, varValue.month, varValue.day, 0, 0, 0)
        if isinstance(varValue, datetime.datetime):
            origtime = datetime.datetime(1582, 10, 14, 0, 0, 0)
            diffdays = (varValue-origtime).days
            newtime = datetime.datetime(varValue.year, varValue.month, varValue.day, 0, 0, 0)
            diffsecs = (varValue-newtime).seconds
            diffmicrosecs = varValue.microsecond
            varValue = diffdays*86400 + diffsecs + float(diffmicrosecs)/ 10**6
        
        if self.isBinary:
            varName = varName.rstrip()
            self.binaryStream.setValueForChange(varName, varValue)
        else:
            spss.error.SetErrorCode(PyInvokeSpss.SetValueNumeric(varName,varValue))
            if spss.error.IsError():
                raise SpssError,spss.error

    def CommitCase(self):
        """Declare it as virtual fuction, the sub-class should implement it.
        """
        pass

class WriteCursor(ReadCursor,BaseWriteAppendCursor):
    """This is class to add new variables, set their attribute, and
    set their data values.
    """
    def reset(self):
        """Resets the current cursor position to the first case
        (row) of data in the procedure data source.

           --usage:
             reset()

           --example

             import spss
             spss.Submit(("GET FILE='demo.sav'."))
             cur=spss.Cursor()
             print cur.fetchmany(4)
             print cur.fetchone()
             cur.reset()
             print cur.fetchmany(4)
             print cur.fetchone()
             cur.close()
             del cur

        """
        self.__IsClose__()

        spss.error.Reset()

        if self.var != None:
            varCount = self.GetVariableCount()
            for i in range(varCount):
                varName = self.GetVariableName(i)
                if varName in self.newvars:
                    self.var.append(i)

        spss.error.SetErrorCode(PyInvokeSpss.ResetDataPassWrite(self.conv,self.datasrcInfo,tuple(self.var)))
        if spss.error.IsError():
            raise SpssError,spss.error

        self.newvars = []
        if self.isBinary:
            self.binaryStream.reset()

    def AllocNewVarsBuffer(self,bufSize):
        """Allocate bufSize bytes memory to add new variables.

          --usage:
           AllocNewVarsBuffer(bufSize)

          --arguments:
            bufSize : memory size to store new variable's data.

          --details:
            If need to add new variables, this function should be called first. After all it, the
            SetOneVarNameAndType or SetVarNameAndType might be called to add new variable. If you
            have read some data by fetch functions, the reset() should be called first, then add
            new variables.
            For a numeric variable, the memory size is 8 bytes. For string variable, its size must
            be octuple.For example, if the variable type is 1, the memory size will be 8. and if the
            type is 9, the memory size is 16.

          --examples:

            import spss
            spss.Submit("get file='demo.sav'.")

            cur=spss.Cursor(accessType='w')

            cur.AllocNewVarsBuffer(28)

            varName=("t1","t2")
            varType=(0,20)

            caseCount = cur.GetCaseCount()

            for i in range(len(varName)):
                cur.SetOneVarNameAndType(varName[i],varType[i])
                cur.CommitDictionary()
                if varType[i] == 0:
                    for j in range(caseCount):
                        cur.fetchone()
                        cur.SetValueNumeric(varName[i],1.1*j)
                        cur.CommitCase()
                else:
                    for j in range(caseCount):
                        cur.fetchone()
                        cur.SetValueChar(varName[i],"A"*varType[i])
                        cur.CommitCase()
                cur.reset()

            cur.close()
        """
        self.__IsClose__()

        spss.error.Reset()

        if not isinstance(bufSize,(int,long)):
            spss.error.SetErrorCode(1000)
            raise SpssError,spss.error

        if bufSize <= 0:
            spss.error.SetErrorCode(1056)
            raise SpssError,spss.error

        spss.error.SetErrorCode(PyInvokeSpss.AllocNewVarsBuffer(bufSize))
        if spss.error.IsError():
            raise SpssError,spss.error

    def SetOneVarNameAndType(self,varName,varType):
        """Add one new variable.

          --usage:
            SetOneVarNameAndType(varName,varType)

          --arguments:
            varName: variable name.
            varType: variable type.

          --example:

            import spss
            spss.Submit("get file='demo.sav'.")

            cur=spss.Cursor(accessType='w')

            cur.AllocNewVarsBuffer(28)

            varName=("t1","t2")
            varType=(0,20)

            caseCount = cur.GetCaseCount()

            for i in range(len(varName)):
                cur.SetOneVarNameAndType(varName[i],varType[i])
                cur.CommitDictionary()
                if varType[i] == 0:
                    for j in range(caseCount):
                        cur.fetchone()
                        cur.SetValueNumeric(varName[i],1.1*j)
                        cur.CommitCase()
                else:
                    for j in range(caseCount):
                        cur.fetchone()
                        cur.SetValueChar(varName[i],"A"*varType[i])
                        cur.CommitCase()
                cur.reset()

            cur.close()
        """
        self.__IsClose__()

        spss.error.Reset()
        varName = spssutil.CheckStr(varName)

        if not isinstance(varType,(int,long)):
            spss.error.SetErrorCode(1000)
            raise SpssError,spss.error
        if varType < 0:
            spss.error.SetErrorCode(1067)
            raise SpssError,spss.error

        spss.error.SetErrorCode(PyInvokeSpss.SetOneVarNameAndType(varName,varType))
        if spss.error.IsError():
            raise SpssError,spss.error

        if self.isBinary:
            self.binaryStream.setOneWriteVarName(varName)
            self.binaryStream.setOneWriteVarType(varType)
            
        self.newvars.append(varName)

    def SetVarNameAndType(self,varName,varType):
        """Adds new variables and sets their type.

           --usage:
             SetVarNameAndType(varName,varType)

           --arguments:
             varName: A tuple or a list of variable names. Each item is a string up to 64 byes in length.
             varType: A tuple or a list of variable types. Each item is an integer. Valid settings include:
                      0    numeric
                      1    String length 1 byte (equivalent to format A1)
                      2    String length 2 bytes (equivalent to format A2)

           --details:
             This function can only be called once for a cursor instance. The tuples or lists in the arguments
             must be the same length,and are mathched by position. For example,the third varName will have the
             type of the third varType.

           --examples:

             import spss
             spss.Submit("get file = 'demo.sav'.")
             cur = spss.Cursor(accessType = "w")
             cur.AllocNewVarsBuffer(28)
             varName=("test1","test2")
             varType=(0,8)
             cur.SetVarNameAndType(varName,varType)
             cur.CommitDictionary()
             cur.close()

        """
        self.__IsClose__()

        spss.error.Reset()

        if not isinstance(varName,(tuple,list)):
            spss.error.SetErrorCode(1002)
            raise SpssError,spss.error

        if isinstance(varName,tuple): varName = list(varName)

        for i in range(len(varName)):
            varName[i] = spssutil.CheckStr(varName[i])

        if len(varName) == 0:
            spss.error.SetErrorCode(1054)
            raise SpssError,spss.error

        if not isinstance(varType,(tuple,list)):
            spss.error.SetErrorCode(1002)
            raise SpssError,spss.error
        for i in range(len(varType)):
            if not isinstance(varType[i],(int,long)):
                spss.error.SetErrorCode(1000)
                raise SpssError,spss.error
            if varType[i] < 0:
                spss.error.SetErrorCode(1067)
                raise SpssError,spss.error

        if len(varType) == 0:
            spss.error.SetErrorCode(1054)
            raise SpssError,spss.error

        if len(varName) != len(varType):
            spss.error.SetErrorCode(1055)
            raise SpssError,spss.error

        spss.error.SetErrorCode(PyInvokeSpss.SetVarName(varName,varType))
        if spss.error.IsError():
            raise SpssError,spss.error
        
        if self.isBinary:
            self.binaryStream.setWriteVarName(varName)
            self.binaryStream.setWriteVarType(varType)

        self.newvars += varName

    def SetVarLabel(self,varName,varLabel):
        """Sets the variable label of the named newly-created variable.
           The varName and varLabel arguments are strings.

           --usage:
             SetVarLabel(varName,varLabel)

           --arguments:
             varName: The string name of the variable for which to set the label.
             varLabel: The string label to set.

           --examples:

             import spss
             spss.Submit("get file = 'demo.sav'.")
             cur = spss.Cursor(accessType='w')
             cur.AllocNewVarsBuffer(28)
             newVarName = ("test1","test2")
             newVarType = (0,8)
             cur.SetVarNameAndType(newVarName,newVarType)
             newVarLabel = ("label1","label2")
             for i in range(len(newVarName)):
                 cur.SetVarLabel(newVarName[i],newVarLabel[i])

             cur.CommitDictionary()
             cur.close()

        """
        self.__IsClose__()

        spss.error.Reset()
        varName = spssutil.CheckStr(varName)
        varLabel = spssutil.CheckStr(varLabel)

        spss.error.SetErrorCode(PyInvokeSpss.SetVarLabel(varName,varLabel))
        if spss.error.IsError():
            raise SpssError,spss.error

    def SetVarCValueLabel(self,varName,value,label):
        """Sets a value label for the specified value of the named,newly-created
        string variable.

           --usage:
             SetVarCValueLabel(varName,value,label)

           --arguments:
             varName:The string name of the string variable for which to set the value label.
             value: The string value for which to set the label.
             label: The string value label to set.

           --examples:
             import spss
             spss.Submit("get file = 'demo.sav'.")
             cur = spss.Cursor(accessType='w')
             cur.AllocNewVarsBuffer(16)
             newVarName = ("test1","test2")
             newVarType = (8,8)
             cur.SetVarNameAndType(newVarName,newVarType)
             cValue=("abc","def")
             label=("valuelabel1","valuelabel2")
             cur.SetVarCValueLabel(newVarName[0],cValue[0],label[0])
             cur.SetVarCValueLabel(newVarName[1],cValue[1],label[1])
             cur.CommitDictionary()
             cur.close()

        """
        self.__IsClose__()

        spss.error.Reset()
        varName = spssutil.CheckStr(varName)
        value = spssutil.CheckStr(value)
        label = spssutil.CheckStr(label)

        spss.error.SetErrorCode(PyInvokeSpss.SetVarCValueLabel(varName,value,label))
        if spss.error.IsError():
            raise SpssError,spss.error

    def SetVarNValueLabel(self,varName,value,label):
        """Sets the value label for the specified value of the named, newly-created
        numeric variable.

           --usage:
             SetVarNValueLabel(varName,value,label)

           --arguments:
             varName: The string name of the numeric variable for which to set the value label.
             value: The numeric value for which to set the label, which can be integer or float (double).
             label: The string value label to set.

           --examples:

             import spss
             spss.Submit("get file = 'demo.sav'.")
             cur = spss.Cursor(accessType='w')
             cur.AllocNewVarsBuffer(16)
             newVarName = ("test1","test2")
             newVarType = (0,0)
             cur.SetVarNameAndType(newVarName,newVarType)
             nValue=(1,2)
             label=("valueLabel1","valueLabel2")
             cur.SetVarNValueLabel(newVarName[0],nValue[0],label[0])
             cur.SetVarNValueLabel(newVarName[1],nValue[1],label[1])
             cur.CommitDictionary()
             cur.close()

        """
        self.__IsClose__()

        spss.error.Reset()
        varName = spssutil.CheckStr(varName)

        if not isinstance(value,(float,int,long)):
            spss.error.SetErrorCode(1033)
            raise SpssError,spss.error

        label = spssutil.CheckStr(label)

        nValue = float(value)

        spss.error.SetErrorCode(PyInvokeSpss.SetVarNValueLabel(varName,nValue,label))
        if spss.error.IsError():
            raise SpssError,spss.error

    def SetVarCMissingValues(self,varName,missingVal1=None,missingVal2=None,missingVal3=None):
        """Sets missing values for the named,newly-created string variable

           --usage:
             SetVarCMissingValues(varName,missingVal1,missingVal2,missingVal3)

           --arguments:
              varName: The string name of the string variable for which to set the value label.
              missingVal1: The string missing values.
              missingVal2: The string missing values.
              missingVal3: The string missing values.

           --examples:

             import spss
             spss.Submit("get file = 'demo.sav'.")
             cur = spss.Cursor(accessType='w')
             cur.AllocNewVarsBuffer(16)
             newVarName = ("test1","test2")
             newVarType = (8,8)
             cur.SetVarNameAndType(newVarName,newVarType)
             cur.SetVarCMissingValues("test1","missing1")
             cur.SetVarCMissingValues("test2","missing0","missing1","missing2")
             cur.CommitDictionary()
             cur.close()

        """
        self.__IsClose__()

        spss.error.Reset()
        varName = spssutil.CheckStr(varName)
        
        if missingVal1: missingVal1 = spssutil.CheckStr(missingVal1)
        if missingVal2: missingVal2 = spssutil.CheckStr(missingVal2)
        if missingVal3: missingVal3 = spssutil.CheckStr(missingVal3)

        format = 0
        temp = []

        if missingVal1 != None:
            format += 1
            temp.append(missingVal1)

        if missingVal2 != None:
            format += 1
            temp.append(missingVal2)

        if missingVal3 != None:
            format += 1
            temp.append(missingVal3)

        if format == 0:
            spss.error.SetErrorCode(PyInvokeSpss.SetVarCMissingValues(varName,format))
        elif format == 1:
            spss.error.SetErrorCode(PyInvokeSpss.SetVarCMissingValues(varName,format,temp[0]))
        elif format == 2:
            spss.error.SetErrorCode(PyInvokeSpss.SetVarCMissingValues(varName,format,temp[0],temp[1]))
        elif format == 3:
            spss.error.SetErrorCode(PyInvokeSpss.SetVarCMissingValues(varName,format,temp[0],temp[1],temp[2]))

        if spss.error.IsError():
            raise SpssError,spss.error

    def SetVarNMissingValues(self,varName,missingFormat,missingVal1=None,missingVal2=None,missingVal3=None):
        """Sets missing values for the named, newly-created numeric variable.

          --usage:
            SetVarNMissingValues(varName,missingFormat,missingVal1,missingVal2,missingVal3)

          --arguments:
            varName: The string name of the numeric variable for which to set missing values.
            missingFormat: An integer. Valid settings include:
                           0   a discrete list of missing values, e.g., 0, 9, 99
                           1   a range of missing values, e.g., 9-99
                           2   a combination of a discrete value and a range, e.g., 9-99, 0
            missingVal: The numeric missing values, which can be integer or float (double).
                        Depending on the missingFormat argument, the following missingVal arguments
                        can be defined: each missingVal1 is a discrete value. missingVal1 is the
                        starting point of the range and missingVal2 is the endpoint.missingVal1 is
                        the starting point of the range, missingVal2 is the endpoint of the range,
                        and missingVal3 is a discrete missing value.

          --details:
            The user can set missingVal1, missingVal2, and missingVal3 to none, in which case the missing values
            for the named numeric variable will be cleared.  For example SetVarNMissingValues("myvar") will clear
            missing values for myvar.

          --examples:

            import spss
            spss.Submit("get file = 'demo.sav'.")
            cur = spss.Cursor(accessType='w')
            cur.AllocNewVarsBuffer(24)
            newVarName = ("test1","test2","test3")
            newVarType = (0,0,0)
            cur.SetVarNameAndType(newVarName,newVarType)
            cur.SetVarNMissingValues(newVarName[0],0,1.0,2.0,3.0)
            cur.SetVarNMissingValues(newVarName[1],1,9,99)
            cur.SetVarNMissingValues(newVarName[2],2,9,99,100)
            cur.CommitDictionary()
            cur.close()

        """
        self.__IsClose__()

        spss.error.Reset()
        varName = spssutil.CheckStr(varName)

        if not isinstance(missingFormat,(int,long)):
            spss.error.SetErrorCode(1000)
            raise SpssError,spss.error

        if not (missingFormat == self.DISCRETE_TYPE or \
                missingFormat == self.RANGE_TYPE    or \
                missingFormat == self.RANGE_DISCRETE_TYPE):
            spss.error.SetErrorCode(1052)
            raise SpssError,spss.error

        if missingVal1 and not isinstance(missingVal1,(float,int,long)):
            spss.error.SetErrorCode(1033)
            raise SpssError,spss.error

        if missingVal2 and not isinstance(missingVal2,(float,int,long)):
            spss.error.SetErrorCode(1033)
            raise SpssError,spss.error

        if missingVal3 and not isinstance(missingVal3,(float,int,long)):
            spss.error.SetErrorCode(1033)
            raise SpssError,spss.error

        format = 0
        temp = []
        if missingFormat == self.DISCRETE_TYPE:
            if missingVal1 != None:
                format += 1
                temp.append(missingVal1)
            if missingVal2 != None:
                format += 1
                temp.append(missingVal2)
            if missingVal3 != None:
                format += 1
                temp.append(missingVal3)
            for i in range(3 - format):
                temp.append(None)
        elif missingFormat == self.RANGE_TYPE:
            if missingVal1 is None or missingVal2 is None:
                spss.error.SetErrorCode(1033)
                raise SpssError,spss.error

            if missingVal1 > missingVal2:
                spss.error.SetErrorCode(1053)
                raise SpssError,spss.error

            temp.append(missingVal1)
            temp.append(missingVal2)
            temp.append(None)
            format = -2
        else:
            if missingVal1 is None or missingVal2 is None or missingVal3 is None:
                spss.error.SetErrorCode(1033)
                raise SpssError,spss.error

            if missingVal1 > missingVal2:
                spss.error.SetErrorCode(1053)
                raise SpssError,spss.error

            temp.append(missingVal1)
            temp.append(missingVal2)
            temp.append(missingVal3)
            format = -3

        if abs(format) == 0:
            spss.error.SetErrorCode(PyInvokeSpss.SetVarNMissingValues(varName,format))
        elif abs(format) == 1:
            spss.error.SetErrorCode(PyInvokeSpss.SetVarNMissingValues(varName,format,temp[0]))
        elif abs(format) == 2:
            spss.error.SetErrorCode(PyInvokeSpss.SetVarNMissingValues(varName,format,temp[0],temp[1]))
        elif abs(format) == 3:
            spss.error.SetErrorCode(PyInvokeSpss.SetVarNMissingValues(varName,format,temp[0],temp[1],temp[2]))

        if spss.error.IsError():
            raise SpssError,spss.error

    def SetVarMeasureLevel(self,varName,measureLevel):
        """Sets the measurement level of the named, newly-created variable.

           --usage:
             SetVarMeasureLevel(varName,measureLevel)

           --arguments:
             varName: The string name of the variable for which to set the measurement level.
             measureLevel: An integer. Valid settings include:
                           2   nominal
                           3   ordinal
                           4   scale

           --examples:

             import spss
             spss.Submit("get file = 'demo.sav'.")
             cur = spss.Cursor(accessType='w')
             cur.AllocNewVarsBuffer(24)
             newVarName = ("test1","test2","test3")
             newVarType = (0,0,0)
             cur.SetVarNameAndType(newVarName,newVarType)
             cur.SetVarMeasureLevel(newVarName[0],2)
             cur.SetVarMeasureLevel(newVarName[1],3)
             cur.SetVarMeasureLevel(newVarName[2],4)
             cur.CommitDictionary()
             cur.close()

        """
        self.__IsClose__()

        spss.error.Reset()
        varName = spssutil.CheckStr(varName)

        if not isinstance(measureLevel,(int,long)):
            spss.error.SetErrorCode(1000)
            raise SpssError,spss.error

        spss.error.SetErrorCode(PyInvokeSpss.SetVarMeasureLevel(varName,measureLevel))
        if spss.error.IsError():
            raise SpssError,spss.error

    def SetVarAlignment(self,varName,aligment):
        """Sets the alignment of the named, newly-created variable.

           --usage:
             SetVarAlignment(varName,aligment)

           --arguments:
             varName: The string name of the variable for which to set the measurement level.
             alignment: An integer. Valid settings include:
                        0    left
                        1    right
                        2    center

           --examples:

             import spss
             spss.Submit("get file = 'demo.sav'.")
             cur = spss.Cursor(accessType='w')
             cur.AllocNewVarsBuffer(24)
             newVarName = ("test1","test2","test3")
             newVarType = (0,0,0)
             cur.SetVarNameAndType(newVarName,newVarType)
             cur.SetVarAlignment(newVarName[0],0)
             cur.SetVarAlignment(newVarName[1],1)
             cur.SetVarAlignment(newVarName[2],2)
             cur.CommitDictionary()
             cur.close()

        """
        self.__IsClose__()

        spss.error.Reset()
        varName = spssutil.CheckStr(varName)

        if not isinstance(aligment,(int,long)):
            spss.error.SetErrorCode(1000)
            raise SpssError,spss.error

        spss.error.SetErrorCode(PyInvokeSpss.SetVarAlignment(varName,aligment))
        if spss.error.IsError():
            raise SpssError,spss.error

    def SetVarFormat(self,varName,type,width,decimal=None):
        """Sets the display format of the named,newly-created variable.

            --usage:
              SetVarFormat(varName,type,width,decimal):

            --arguments:
              varName: The string name of the variable for which to set the measurement level.
              type: An integer which represents a variable format. Valid settings include:
                    1    A
                    2    AHEX
                    3    COMMA
                    4    DOLLAR
                    5    F
                    6    IB
                    7    PIBHEX
                    8    P
                    9    PIB
                    10   PK
                    11   RB
                    12   RBHEX
                    15   Z
                    16   N
                    17   E
                    20   DATE
                    21   TIME
                    22   DATETIME
                    23   ADATE
                    24   JDATE
                    25   DTIME
                    26   WKDAY
                    27   MONTH
                    28   MOYR
                    29   QYR
                    30   WKYR
                    31   PCT
                    32   DOT
                    33   CCA
                    34   CCB
                    35   CCC
                    36   CCD
                    37   CCE
                    38   EDATE
                    39   SDATE
                    85   MTIME
                    86   YMDHMS
                Width and decimals:
                Allowable settings for decimal and width depend on the specified type.
                For a list of the minimum and maximum widths and maximum decimals for
                commonly-used format types, see the FORMATS command in the Command
                Syntax Reference.

           --examples:

             import spss
             spss.Submit("get file = 'demo.sav'.")
             cur = spss.Cursor(accessType='w')
             cur.AllocNewVarsBuffer(16)
             newVarName = ("test1","test2")
             newVarType = (0,8)
             cur.SetVarNameAndType(newVarName,newVarType)
             cur.SetVarFormat(newVarName[0],5,10,2)
             cur.SetVarFormat(newVarName[1],1,5,0)
             cur.CommitDictionary()
             cur.close()

        """
        self.__IsClose__()

        spss.error.Reset()
        varName = spssutil.CheckStr(varName)

        if not isinstance(type,(int,long)):
            spss.error.SetErrorCode(1000)
            raise SpssError,spss.error

        if not decimal:
            decimal = 0

        if not isinstance(decimal,(int,long)):
            spss.error.SetErrorCode(1000)
            raise SpssError,spss.error

        if not isinstance(width,(int,long)):
            spss.error.SetErrorCode(1000)
            raise SpssError,spss.error

        spss.error.SetErrorCode(PyInvokeSpss.SetVarFormat(varName,type,width,decimal))
        if spss.error.IsError():
            raise SpssError,spss.error

    def SetVarAttributes(self,varName,attrName,attrValue,index):
        """Sets the indexed variable attribute's value for the named, newly-created variable.

           --usage:
             SetVarAttributes(varName,attrName,attrValue,index)

           --arguments:
             varName: The string name of the variable for which to set the attribute values.
             attrName: The string name of the attribute.
             attrValue: The string value of the attribute at the indexed position.
             index: The position of the attribute value in the attribute,starting with 0 for the first attribute value.

           --examples:

             import spss
             spss.Submit("get file = 'demo.sav'.")
             cur=spss.Cursor(accessType='w')
             cur.AllocNewVarsBuffer(8)
             cur.SetVarNameAndType(['numvar'],[0])
             cur.SetVarAttributes('numvar','myattribute','1',1)
             cur.SetVarAttributes('numvar','myattribute','2',2)
             cur.CommitDictionary()
             cur.close()

        """
        self.__IsClose__()

        spss.error.Reset()
        varName = spssutil.CheckStr(varName)
        attrName = spssutil.CheckStr(attrName)
        attrValue = spssutil.CheckStr(attrValue)

        if not isinstance(index,(int,long)):
            spss.error.SetErrorCode(1000)
            raise SpssError,spss.error

        spss.error.SetErrorCode(PyInvokeSpss.SetVarAttributes(varName,attrName,attrValue,index))
        if spss.error.IsError():
            raise SpssError,spss.error

    def CommitDictionary(self):
        """Commits the metadata for the newly-created variables.

           --usage:
             CommitDictionary()

           --details:
             After you finish using the methods in the WriteCursor class  to set metadata
             for new variables, you must commit the dictionary. You can only use
             CommitDictionary once for each WriteCursor instance, so set all of the metadata
             before you commit it

           --examples:

             import spss
             spss.Submit("get file = 'demo.sav'.")
             cur = spss.Cursor(accessType = "w")
             cur.AllocNewVarsBuffer(16)
             varName=("test1","test2")
             varType=(0,8)
             cur.SetVarNameAndType(varName,varType)
             cur.CommitDictionary()
             cur.close()

        """
        self.__IsClose__()

        spss.error.Reset()

        spss.error.SetErrorCode(PyInvokeSpss.CommitHeader())
        if spss.error.IsError():
            raise SpssError,spss.error
        
        if self.isBinary:
            self.binaryStream.lengthForWriteMode()
            self.binaryStream.commitDictionary()

    def CommitManyCases(self,newVars,caseData):
        """Commit many cases for new variables. It can be faster than CommitCase.
        
           --usage:
             CommitManyCases(newVars,caseData)
             
           --arguments:
             newVars: variables that will be set new data. The newVars must be new variable.
                      It can be a string or tuple/list
             caseData: A tuple/list take all case data.
          
           --examples: 
             
             import spss
             spss.Submit("get file = 'demo.sav'.")
             cur=spss.Cursor(accessType='w')
             cur.AllocNewVarsBuffer(16)
             newVarName = ("test1","test2")
             newVarType = (0,8)
             cur.SetVarNameAndType(newVarName,newVarType)
             cur.CommitDictionary()
             newVars=("test1","test2")
             caseData=((1,"A"),(2,"B"),(3,"C"),(4,"D"))
             cur.CommitManyCases(newVars,caseData)             
             cur.close()
        """
        self.__IsClose__()
        spss.error.Reset()
        
        if not isinstance(caseData,(tuple,list)):
            spss.error.SetErrorCode(1027)
            raise SpssError,spss.error
        
        def isValidVarName(self,myName): 
            if not isinstance(myName, basestring):
                spss.error.SetErrorCode(27)
                raise SpssError,spss.error         
            varIndex = -1
            varType = -1
            varCount = self.GetVariableCount()
            for i in range(varCount):
                varName = self.GetVariableName(i)
                if varName == myName:  
                    varIndex = i                  
                    varType = self.GetVariableType(varIndex)
                    break
            if varType == -1:
                spss.error.SetErrorCode(27)
                raise SpssError,spss.error
            return varIndex,varType
            
        newVarList = []
        newVarListType = []
        newCaseData=[]
        if isinstance(newVars, basestring):
            index,type = isValidVarName(self,newVars)
            newVarList.append(newVars)
            newVarListType.append(type)
            for x in caseData: 
                if isinstance(x,(tuple,list)):
                    newCaseData.append(x)
                else:
                    newCaseData.append(x[0])
            caseData = newCaseData
        elif isinstance(newVars,(tuple,list)):
            for name in newVars: 
                index,type = isValidVarName(self,name)
                newVarList.append(name)
                newVarListType.append(type)
        else:
            spss.error.SetErrorCode(27)
            raise SpssError,spss.error
        
        if self.isBinary:
            for i in xrange(len(caseData)):
                self.binaryStream.packdataForWrite(caseData[i])
        else:
            spss.error.SetErrorCode(PyInvokeSpss.CommitManyCases(tuple(newVarList),caseData,tuple(newVarListType)))
            if spss.error.IsError():
                raise SpssError,spss.error                 
        
    def CommitCase(self):
        """For the WriteCursor, commits the data for the current case (row) into
        newly-created variables procedure data source.

           --usage:
             CommitCase()

           --details:
             fter you finish using the methods in the WriteCursor class to set the
             data values for a case (row), you must call CommitCase() to save these
             data into the procedure data source before you set data values for the
             next case (row). CommitCase does not change the cursor position. To move
             to the next case, use fetchone

           --examples:

             import spss
             spss.Submit("get file = 'demo.sav'.")
             cur=spss.Cursor(accessType='w')
             cur.AllocNewVarsBuffer(16)
             newVarName = ("test1","test2")
             newVarType = (0,8)
             cur.SetVarNameAndType(newVarName,newVarType)
             cur.CommitDictionary()
             caseCount = cur.GetCaseCount()
             for i in range(caseCount):
                 cur.fetchone()
                 cur.SetValueNumeric(newVarName[0],1.1*i)
                 cur.SetValueChar(newVarName[1],"AAABBB")
                 cur.CommitCase()

             cur.close()

        """
        self.__IsClose__()

        spss.error.Reset()

        if self.isBinary:
            writeCase = self.binaryStream.getCaseValueForWrite()
            self.binaryStream.packdataForWrite(writeCase)
        else:
            spss.error.SetErrorCode(PyInvokeSpss.CommitCaseRecord())
            if spss.error.IsError():
                raise SpssError,spss.error
            
    def SetVarRole(self,varName,varRole):
        """Sets the role for variable, newly-created variable.

           --usage:
             SetVarRole(varName,varRole)

           --arguments:
             varName: The string name of the variable for which to set the role.
             varRole: A string. Valid settings include:
                           Input
                           Target
                           Both
                           None
                           Partition
                           Split
                           Frequency

           --examples:

             import spss
             spss.Submit("get file = 'demo.sav'.")
             cur = spss.Cursor(accessType='w')
             cur.AllocNewVarsBuffer(24)
             newVarName = ("test1","test2","test3")
             newVarType = (0,0,0)
             cur.SetVarNameAndType(newVarName,newVarType)
             cur.SetVarRole(newVarName[0],"Both")
             cur.SetVarRole(newVarName[1],"None")
             cur.SetVarRole(newVarName[2],"Partition")
             cur.CommitDictionary()
             cur.close()

        """
        self.__IsClose__()

        spss.error.Reset()
        varName = spssutil.CheckStr(varName)

        if not isinstance(varRole, basestring):
            spss.error.SetErrorCode(1001)
            raise SpssError,spss.error

        varDic = { "Input":0,
                    "Target":1,
                    "Both":2,
                    "None":3,
                    "Partition":4,
                    "Split":5,
                    "Frequency":6
                  }
                  
        if varRole not in varDic:
            error.SetErrorCode(118)
            raise SpssError, error
                  
        varRoleNo=varDic[varRole]
        
        spss.error.SetErrorCode(PyInvokeSpss.SetVarRole(varName,varRoleNo))
        if spss.error.IsError():
            raise SpssError,spss.error

class AppendCursor(BaseCursor,BaseWriteAppendCursor):
    """The AppendCursor class derives from BaseWriteAppedCursor and BaseCursor. The AppendCursor
       can only create new cases and write data values for them-it cannot read data values,
       nor can it set variable metadata.
    """
    def reset(self):
        """delete the old cursor and create new cursor.

           --example
             import spss
             spss.Submit(("GET FILE='demo.sav'."))
             cur=spss.Cursor()
             print cur.fetchmany(4)
             print cur.fetchone()
             cur.reset()
             print cur.fetchmany(4)
             print cur.fetchone()
             cur.close()
             del cur

        """
        self.__IsClose__()

        if self.running:
            PyInvokeSpss.close()
            self.running = False

        result = PyInvokeSpss.cursor(self.conv,self.varTuple,self.accessType)
        self.datasrcInfo = result[0]
        spss.error.SetErrorCode(result[1])
        if spss.error.IsError():
            raise SpssError,spss.error

        if not self.datasrcInfo:
            del self
            spss.error.SetErrorCode(1024)
            raise SpssError,spss.error

        if self.isBinary:
            del self.binaryStream
            self.binaryStream = binarystream.CursorBinary(self.varTuple, self.accessType)
            global newCaseCount
            newCaseCount = 0
        self.running = True

    def EndChanges(self):
        """For the AppendCursor,commits all the data into procedure data source.

           --usage:
             EndChanges()

           --examples:

             import spss
             spss.Submit("get file = 'demo.sav'.")
             cur = spss.Cursor(accessType='a')
             varCount = cur.GetVariableCount()
             caseCount = 10
             for i in range(caseCount):
                 for j in range(varCount):
                     cur.SetValueNumeric(cur.GetVariableName(j),1.1*i)
                 cur.CommitCase()

             cur.EndChanges()
             cur.close()

        """
        self.__IsClose__()

        spss.error.Reset()
        
        if self.isBinary:
            self.binaryStream.endChanges()
            if not self.binaryStream.getNewCaseCount() > 0:
                spss.error.SetErrorCode(75)
                if spss.error.IsError():
                    raise SpssError, spss.error
            spss.error.SetErrorCode(PyInvokeSpss.EndChanges())
            if spss.error.IsError():
                raise SpssError, spss.error
        else:
            spss.error.SetErrorCode(PyInvokeSpss.EndChanges())
            if spss.error.IsError():
                raise SpssError, spss.error

    def CommitCase(self):
        """For the AppendCursor, creates new cases (rows) and commits their data
        into the procedure data source.

           --usage:
             CommitCase()

           --details:
             Each time you complete setting the data values for a case with an AppendCursor,
             you must call CommitNewCase() to save these data into the procedure data source
             before you set data values for the next row. CommitCase automatically moves the
             cursor position to the next row

           --examples:

             import spss
             spss.Submit("get file = 'demo.sav'.")
             cur = spss.Cursor(accessType='a')
             varCount = cur.GetVariableCount()
             caseCount = 10
             for i in range(caseCount):
                 for j in range(varCount):
                     cur.SetValueNumeric(cur.GetVariableName(j),1.1*i)
                 cur.CommitCase()

             cur.EndChanges()
             cur.close()

        """
        self.__IsClose__()

        spss.error.Reset()
        
        if self.isBinary:
            appendCase = self.binaryStream.getCaseValueForAppend()
            self.binaryStream.packdataForAppend(appendCase)
            self.binaryStream.commitCase()
        else:
            spss.error.SetErrorCode(PyInvokeSpss.CommitNewCase())
            if spss.error.IsError():
                raise SpssError,spss.error

class Cursor(object):
    """This class provides the ability to read cases, append cases, and add
    new variables to the active data source. There are three usage modes:
    read ("r"), write ("w") and append ("a"). The default is read mode. The
    argument accessType is used to specify the mode. Each usage mode of the
    Cursor class supports its own set of methods as shown below. To obtain help
    for a particular method, create an instance of the Cursor class in a mode
    that supports that method and use help(cur.<method>), where cur is the Cursor
    instance and <method> is the method name.

       The function list of each type is:
       Read Mode Function List:
         -- close
         -- reset
         -- fetchone
         -- fetchmany
         -- fetchall
         -- GetCaseCount
         -- GetVarAttributeNames
         -- GetVarAttributes
         -- GetVariableCount
         -- GetVariableFormat
         -- GetVariableLabel
         -- GetVariableMeasurementLevel
         -- GetVariableRole
         -- GetVariableName
         -- GetVariableType
         -- GetVarMissingValues
         -- IsEndSplit
         -- SetUserMissingInclude

       Write Mode Function List:
         -- close
         -- reset
         -- fetchone
         -- fetchmany
         -- fetchall
         -- GetCaseCount
         -- GetVarAttributeNames
         -- GetVarAttributes
         -- GetVariableCount
         -- GetVariableFormat
         -- GetVariableLabel
         -- GetVariableMeasurementLevel
         -- GetVariableRole
         -- GetVariableName
         -- GetVariableType
         -- GetVarMissingValues
         -- IsEndSplit
         -- SetUserMissingInclude
         -- SetValueChar
         -- SetValueNumeric
         -- SetVarAlignment
         -- SetVarAttributes
         -- SetVarCMissingValues
         -- SetVarCValueLabel
         -- SetVarFormat
         -- SetVarLabel
         -- SetVarMeasureLevel
         -- SetVarRole
         -- SetVarNMissingValues
         -- SetVarNValueLabel
         -- SetVarNameAndType
         -- CommitDictionary
         -- CommitCase

      Append Mode Function List:
        -- close
        -- reset
        -- GetCaseCount
        -- GetVarAttributeNames
        -- GetVarAttributes
        -- GetVariableCount
        -- GetVariableFormat
        -- GetVariableLabel
        -- GetVariableMeasurementLevel
        -- GetVariableRole
        -- GetVariableName
        -- GetVariableType
        -- GetVarMissingValues
        -- SetValueChar
        -- SetValueNumeric
        -- EndChanges
        -- CommitCase
    """
    def __new__(self, var = None, accessType = "r", cvtDates=(), isBinary = True):
        spss.error.Reset()
        if not isinstance(accessType,str):
            spss.error.SetErrorCode(1001)
            raise SpssError,spss.error

        if len(spss.ProcDictionary) > 0:
            isBinary = False
        
        if accessType == "r":
            return ReadCursor(var,accessType,cvtDates,isBinary)
        elif accessType == "w":
            return WriteCursor(var,accessType,cvtDates,isBinary)
        elif accessType == "a":
            if cvtDates:
                spss.error.SetErrorCode(1070)
                raise SpssError,spss.error
            return AppendCursor(var,accessType,isBinary=isBinary)
        else:
            spss.error.SetErrorCode(1034)
            raise SpssError,spss.error

__all__ = ["Cursor"]
import version
__version__ = version.version
