#/***********************************************************************
# * Licensed Materials - Property of IBM 
# *
# * IBM SPSS Products: Statistics Common
# *
# * (C) Copyright IBM Corp. 1989, 2018
# *
# * US Government Users Restricted Rights - Use, duplication or disclosure
# * restricted by GSA ADP Schedule Contract with IBM Corp. 
# ************************************************************************/

#!/usr/bin/python
# -*- coding: cp936 -*-

"""

This module provides functions that invoke IBM SPSS Statistics
and execute commands.
"""
#Copyright (C) 2008 by SPSS Inc.

import atexit,os,locale,sys,codecs
import PyInvokeSpss
from errMsg import *

#Initial global object 'error'
#Move up to this, the other importer file can get an initialized error object
error = errCode()

import spssutil
import cursors,preaction
from dataStep import Dataset, PythonDatasets

global OutputSwitch
OutputSwitch=True

global runStartSPSS
runStartSPSS=False

global ProcDictionary
ProcDictionary = {}


def isPythonDrive():
    import PyInvokeSpss
    return 1 == PyInvokeSpss.GetXDriveMode()[1]

def __PostOuputToPython():
    global ropSPSS,tempfileS
    curMode,originMode,errLevel = PyInvokeSpss.GetXDriveMode()
    if originMode == 2:
        return
    curPos = ropSPSS.tell()
    ropSPSS.seek(curPos)
    text=ropSPSS.readlines()
    
    if PyInvokeSpss.IsUTF8mode():
           f = codecs.open(tempfileS,"r","utf-8")
           f.seek(curPos)
           text = f.readlines()
           f.close()
           
    if OutputSwitch:       
        if PyInvokeSpss.IsUTF8mode(): 
            for item in text:  
                if not locale.getlocale(locale.LC_CTYPE)[1] is None:  
                    if sys.platform == "win32":        
                        sys.stdout.write(item.encode(locale.getlocale(locale.LC_CTYPE)[1],'replace'))
                    else:
                        sys.stdout.write(item.encode('utf-8','replace'))
                else:
                    sys.stdout.write(item)
        else:
            for item in text:            
                sys.stdout.write(item)

def __PostOutputToSpss():
    #Create output to spss log when spss drive mode
    #if not isPythonDrive():
    if len(preaction.wop) > 0:
        preaction.wop[-1].stream.flush()
        preaction.rop[-1].stream.seek(preaction.rop[-1].tell())
        text=preaction.rop[-1].stream.read().split("\n")
        if not text[len(text)-1]:
            del text[len(text)-1]
        #Set the width of output line
        width=255
        for item in text:
            if not item:
                item = spssutil.CheckOutputStrU(item)
                error.SetErrorCode(PyInvokeSpss.PostSpssOutput(item,len(item)))
            else:
                length = len(item)
                loop = length/width
                rm=length%width
                for i in range(loop):
                    temp=item[i*width:(i+1)*width]
                    temp = spssutil.CheckOutputStrU(temp)
                    error.SetErrorCode(PyInvokeSpss.PostSpssOutput(temp,len(temp)))
                if rm > 0:
                    temp=item[-rm:]
                    temp = spssutil.CheckOutputStrU(temp)
                    error.SetErrorCode(PyInvokeSpss.PostSpssOutput(temp,len(temp)))
                if error.IsError():
                    raise SpssError

def __locale_map():
    pass
    

def GetLastErrorLevel():
    """Returns a number corresponding to an error in the preceding IBM SPSS Statistics
    package function.

        --details
          For the spss.Submit function, it returns the maximum error
          level for the submitted IBM SPSS Statistics commands.
          An error level less than 3 does not stop a command
          from being processed.
          An error level of 3 or higher causes an exception.
          An error level of 3 or higher means that at least one 
          command was not processed.
          For other functions in the IBM SPSS Statistics package, it returns an error code.
          Error codes from 6 to 22 are from the XD API.
          Error codes from 1000 to 1013 are from the IBM SPSS Statistics Package.


    """
    return error.errLevel

def GetLastErrorMessage():
    """Returns a text message corresponding to an error in the preceding
    IBM SPSS Statistics package function.

        --details
          For the spss.Submit function, it returns text that indicates
          severity level of the error for the last submitted command.
          For other functions in the IBM SPSS Statistics package, it returns the error
          message text from the associated API or from Python.

        --example
                try:
                   spss.Submit("GET FILE='demo.sav'.")
                   spss.Submit('''
                   COMPUTE newvar= ed*10.
                   COMPUTE badvar=newvar/4.
                   DESCRIPTIVES VARIABLES=ALL.
                   Exec.
                   ''')
                except:
                   errorLevel=str(spss.GetLastErrorLevel())
                   errorMsg=spss.GetLastErrorMessage()
                   print("Error level " + errorLevel + ": " + errorMsg)
                   print("At least one command did not run.")

    """
    return error.errMsg


def IsBackendReady():
    """Checks to see if IBM SPSS Statistics is ready to process commands.

        --usage
          IsBackendReady()

        --value
          Returns True when the IBM SPSS Statistics backend is ready, otherwise returns False.

        --example
          import spss
          print spss.IsBackendReady()
          if not spss.IsBackendReady():
              print "***Backend not ready!***"

    """

    error.Reset()
    return spssutil.cmnglb._cmnglb__backendready

def StartSPSS(hide="",show="",notes=False, nfc=True, nl=True):
    """When used from Python, starts IBM SPSS Statistics. Note that the spss.Submit function
    automatically starts IBM SPSS Statistics.

        --details
          Use this function when running programs from Python. When used from
          a IBM SPSS Statistics program block this function has no effect.

        --example
          import spss
          try:
              spss.StartSPSS()
          except spss.SpssError:
              print "Error."
          print spss.GetLastErrorMessage()

    """
    error.Reset()
    global ropSPSS,tempfileS,runStartSPSS

    if IsBackendReady():
        error.SetErrorCode(1010)
        return

    #check the type of the arguments
    runStartSPSS=True
    if not isinstance(hide,basestring):
        error.SetErrorCode(1001)
        raise SpssError,error
    elif isinstance(hide,unicode):
        hide=hide.encode(locale.getdefaultlocale()[1])

    if not isinstance(show,basestring):
        error.SetErrorCode(1001)
        raise SpssError,error
    elif isinstance(show,unicode):
        show=show.encode(locale.getdefaultlocale()[1])

    if not isinstance(notes,bool):
        error.SetErrorCode(1008)
        raise SpssError,error
    if not isinstance(nfc,bool):
        error.SetErrorCode(1008)
        raise SpssError,error
    if not isinstance(nl,bool):
        error.SetErrorCode(1008)
        raise SpssError,error

    #Convert the arguments to a spss commandLine
    cmd=""
    if hide and not hide.isspace():
        cmd = " ".join([cmd,"-hide",hide])
    if show and not show.isspace():
        cmd = " ".join([cmd,"-show",show])
    if notes:
        cmd = " ".join([cmd,"-notes"])
    if not nfc:
        cmd = " ".join([cmd,"-nfc"])
    if not nl:
        cmd = " ".join([cmd,"-nl"])

    #Create temp file and set spss commandLine when external language drive mode
    import tempfile
    tempfileS=tempfile.mkstemp()
    os.close(tempfileS[0])
    tempfileS=tempfileS[1]
    os.remove(tempfileS)
    commandLine = "".join(["-out ","\"",tempfileS,"\"",cmd])

    #Start spss processor
    error.SetErrorCode(PyInvokeSpss.StartSpss(commandLine))
    #If start spss processor failure
    if error.IsError():
        raise SpssError,error

    cur=sys.platform

    old_locale = locale.getlocale()[0]
    #spss_locale = (PyInvokeSpss.GetSPSSLocale())[0]
    cur_locale = locale.getlocale()[0]
    if cur_locale == "" or cur_locale == "C" or cur_locale is None:
        #Set locale to English
        import platform
        if 'linux' in cur.lower():
            spss_locale = "en_US.UTF-8"
        elif ('darwin' in cur.lower()) and ('i386' not in platform.uname()):
                spss_locale = "en_US"
        else:
            spss_locale = "English"
        cmd = "set locale = '" + spss_locale + "'."
        PyInvokeSpss.Submit(cmd,len(cmd))

        if old_locale is None or old_locale.find("English") == -1:
            #warning: locale has been set to spss locale.
            error.SetErrorCode(1012)
        
    spssutil.cmnglb.update(utf8mode = bool(PyInvokeSpss.IsUTF8mode()),
                           xdriven = bool(isPythonDrive()),
                           backendready = bool(PyInvokeSpss.IsBackendReady()))

    if spssutil.cmnglb._cmnglb__utf8mode:
        ropSPSS = codecs.open(tempfileS,'r','utf-8')
    else:
        ropSPSS = codecs.open(tempfileS,'r')

    #Run StopPythonSPSS to clear up temp files when exiting python
    atexit.register(StopPythonSPSS)


def CreateXPathDictionary(handle):
    """Create an XPath dictionary DOM for the active dataset.

        --usage
          CreateXPathDictionary(handle)

        --arguments
          handle: A handle name used to identify this DOM.

        --details
          The dictionary DOM will be created in the XML workspace.
          The contents of the dictionary DOM can be accessed with XPath
          expressions and the EvaluateXPath function. If there is no
          dictionary, no exception is raised and an empty DOM is created.

        --example
          import spss
          try:
              spss.Submit("GET FILE='demo.sav'.")
              handle = "demo"
              spss.CreateXPathDictionary(handle)
              spss.DeleteXPathHandle(handle)
          except spss.SpssError:
              print "Error."
          print spss.GetLastErrorMessage()
    """
    error.Reset()

    if not IsBackendReady(): StartSPSS()

    #Check the type of the argument
    handle = spssutil.CheckStr(handle)

    theHandle = handle.strip()
    if not theHandle:
        error.SetErrorCode(1013)
        raise SpssError,error

    #Create an xpath dom for the dictionary
    error.SetErrorCode(PyInvokeSpss.CreateXPathDictionary(theHandle))
    if error.IsError():
        raise SpssError,error

def DeleteXPathHandle(handle):
    """Deletes the XPath dictionary DOM or output DOM with the specified handle
    name.

        --usage
          DeleteXPathHandle(handle)

        --arguments
          handle: A handle name that was defined with a previous
                  spss.CreateXPathDictionary function or, for example, by
                  the XMLWORKSPACE keyword on an OMS command.

        --details
          The dictionary DOM or output DOM will be deleted from the XML
          workspace.

        --example
          import spss
          try:
              spss.Submit("GET FILE='demo.sav'.")
              handle = "demo"
              spss.CreateXPathDictionary(handle)
              spss.DeleteXPathHandle(handle)
          except spss.SpssError:
              print "Error."
          print spss.GetLastErrorMessage()

    """
    error.Reset()

    if not IsBackendReady(): StartSPSS()

    #Check the type of the argument
    handle = spssutil.CheckStr(handle)

    theHandle = handle.strip()
    if not theHandle:
        error.SetErrorCode(1013)
        raise SpssError,error

    #Delete an object from the XML workspace
    error.SetErrorCode(PyInvokeSpss.RemoveXPathHandle(theHandle))
    if error.IsError():
        raise SpssError,error

def EvaluateXPath(handle,context,xpath):
    """Evaluates an XPath expression against a
    specified XPath DOM and returns the result as a list.

        --usage
          EvaluateXPath(handle,context,xpath)

        --arguments
          handle: Specifies the handle of the particular XPath DOM.
          context: Defines the XPath context for the expression.
                   Set context to /dictionary for a dictionary DOM.
                   Set context to /outputTree for an output DOM.
          xpath: Specifies the remainder of the XPath expression.

        --details
          Evaluate the XPath expression and return the values as a list.
          If the result is empty, the list will be empty.
          If the result is a number, string, or boolean value, the list will
          contain the string form of that value.
          If the result is a set of nodes, the list will contain the string
          form of those nodes.

        --value
          The result of search.

        --example
          import spss
          try:
              spss.Submit("GET FILE='demo.sav'.")
              handle = "demo"
              spss.CreateXPathDictionary(handle)
              context = "/dictionary"
              xpath = "variable[1]/@name"
              print spss.EvaluateXPath(handle,context,xpath)
              spss.DeleteXPathHandle(handle)
          except spss.SpssError:
              print "Error."
          print spss.GetLastErrorMessage()

    """
    error.Reset()

    if not IsBackendReady(): StartSPSS()

    #Check the type of the argument
    handle = spssutil.CheckStr(handle)
    context = spssutil.CheckStr(context)
    xpath = spssutil.CheckStr(xpath)

    theHandle = handle.strip()
    if not theHandle:
        error.SetErrorCode(1013)
        raise SpssError,error

    #Evaluate the given XPath expression within the given context
    #the last element in result is error code from spss
    result = PyInvokeSpss.EvaluateXPath(theHandle,context,xpath)
    out = result[0:-1]
    #get the error code from spss
    errLevel = result[-1]

    error.SetErrorCode(errLevel)
    if error.IsError():
        raise SpssError,error

    return out

def GetHandleList():
    """Returns a list of currently defined dictionary and output DOMs
    available for use with spss.EvaluateXpath.

        --usage
          GetHandleList()

        --details
          If there is no valid handle, the list will empty.

        --value
          A list of all handles that exist in the XML workspace.

        --example
          import spss
          try:
              spss.Submit("GET FILE='demo.sav'.")
              handle = "demo"
              spss.CreateXPathDictionary(handle)
              handle='b'
              spss.CreateXPathDictionary(handle)
              handlelist = spss.GetHandleList()
              for handle in handlelist:
                  print handle
                  spss.DeleteXPathHandle(handle)
          except spss.SpssError:
              print "Error."
          print spss.GetLastErrorMessage()

    """
    error.Reset()
    if not IsBackendReady():
        StartSPSS()

    #Get a list of existing handles, the last element is error code from spss
    result = PyInvokeSpss.GetHandleList()
    out = result[0:-1]
    errLevel = result[-1]

    error.SetErrorCode(errLevel)
    if error.IsError():
        raise SpssError,error

    return out

def GetXmlUtf16(handle, filename=""):
    """Writes the XML for the specified handle from XML workspace to a
    specified file or returns the XML if no filename is specified.

        --usage
          GetXmlUtf16(handle,filename='')

        --arguments
          handle: A handle of an existing object in the XML workspace.
          filename: The name of an output file.

        --details
          When writing and debugging XPath expressions, it is often
          useful to have a sample file that shows the XML structure.
          The function returns the XML representation of
          an object in the XML workspace as a string.
          The string will be written to the specified output file if the
          output file is a valid filename and path.

        --value
          A string containing the XML representation of the requested
          handle.

        --example
          import spss
          try:
              spss.Submit("GET FILE='demo.sav'.")
              handle = "demo"
              spss.CreateXPathDictionary(handle)
              spss.GetXmlUtf16(handle,'/temp/out.xml')
              spss.DeleteXPathHandle(handle)
          except spss.SpssError:
              print "Error."
          print spss.GetLastErrorMessage()

    """
    error.Reset()

    if not IsBackendReady(): StartSPSS()

    #Check the type of the argument
    handle = spssutil.CheckStr(handle)
    filename = spssutil.CheckStr(filename)

    theHandle = handle.strip()
    if not theHandle:
        error.SetErrorCode(1013)
        raise SpssError,error



    #Retrieve an XML object from the XML workspace.
    #result is a list, first element is the XML object
    #second element is error code from spss
    result = PyInvokeSpss.GetXmlUtf16(theHandle)
    out = result[0]
    errLevel = result[1]

    error.SetErrorCode(errLevel)
    if error.IsError():
        raise SpssError,error

    #If filename argument is not empty, write the XML object
    #to the file
    if filename:
        import codecs
        fp = codecs.open(filename,mode='w',encoding='utf-16')
        fp.write(out)
        fp.close()
        return
    else:
        return out

def GetImage(handle,filename):
    """This function returns a tuple object of the image associated with the
       specified handle and filename,in addition to the information about the image.

        --usage
          GetImage(handle,filename)

        --arguments
          handle:   A string specifying the handle of the XPath DOM associated with the image.
          filename: A string specifying the file name associated with the image.
                    This is the value of the imageFile attribute for this image
                    in the XPath DOM associated with the image.

        --details
          The GetImage function returns a tuple object of the image associated
          with the specified handle and filename, in addition to the information
          about the image, including image size and image type("PNG", "JPG", "EMF", "BMP", and "VML").

        --value
          A tuple object contains the image object,image size and image type in return to represent the requested.
          
        --example
          import spss
          try:
            spss.Submit(
            "GET FILE='Employee data.sav'.
            OMS
            /SELECT CHARTS
            /IF COMMANDS=['Frequencies']
            /DESTINATION FORMAT=OXML XMLWORKSPACE ='handle'
            IMAGES=yes
            IMAGEFORMAT=JPG 
            CHARTSIZE=100
            CHARTFORMAT=IMAGE 
            TREEFORMAT=IMAGE
            MODELFORMAT=IMAGE.
            FREQUENCIES VARIABLES=id jobcat
            /BARCHART FREQ
            /ORDER=ANALYSIS.
            OMSEND.")
            handle = "handle"
            filename = "000.jpg"

            imageObj = spss.GetImage(handle,filename)
            f = file('/temp/myimage.jpg','wb')
            f.truncate(imageObj[1])
            f.write(imageObj[0])
            f.close()

            imagetpye = imageObj[2]
            print imagetpye
            spss.DeleteXPathHandle(handle)
        except spss.SpssError:
            print "Error."
            print spss.GetLastErrorMessage()

    """
    error.Reset()

    if not IsBackendReady(): StartSPSS()

    #Check the type of the argument
    handle = spssutil.CheckStr(handle)
    filename = spssutil.CheckStr(filename)

    theHandle = handle.strip()
    imageName = filename.strip()
    if not theHandle and imageName :
        error.SetErrorCode(1013)
        raise SpssError,error

    result = PyInvokeSpss.GetImage(theHandle,imageName)
   
    errLevel = result[-1]
    error.SetErrorCode(errLevel)
    if error.IsError():
        raise SpssError,error
    else:
         out=(result[0],result[1],result[2])
         
    return out


def GetSetting(setting,option=""):
    """The function returns the value of an options setting,
       as set from the SET command.
       --usage
          GetSetting(setting,option='')

        --arguments
          setting: A string specifying the setting to retrieve.
          The possible values of setting are the names of the
          SET subcommands,the argment can be given by either
          upper case or lower case.
          For example,"WORKSPACE", "OLANG", "XVERSION","workspace"
          ,"olang","xversion".
          
          option: A string specifying an option
          associated with the value of the setting argument.
          Currently,this only applies to the MIOUTPUT subcommand
          of SET,for which there is a separate setting for each
          of the keywords OBSERVED, IMPUTED,POOLED, and DIAGNOSTICS.
          When setting equals MIOUTPUT,option can be set to any of 
          those four keywords to obtain the associated value
          of the keyword "Yes" or "No". By default,option is NULL.

        --details
          Getting the value of the specified setting from "SET"
          subcommand as a string.

        --value
          Returns the value of the specified setting, as a string.

        --example
          import spss
          try:
              val = spss.GetSetting("OLANG")
              print val
          except spss.SpssError:
              print "Error."
              print spss.GetLastErrorMessage()
    """
    error.Reset()
    if not IsBackendReady():
        StartSPSS()

    handle = spssutil.CheckStr(setting)
    result = PyInvokeSpss.GetSetting(handle,option)
        
    errLevel = result[-1]
    error.SetErrorCode(errLevel)
    if error.IsError():
        raise SpssError,error
    else:
        out = result[0]

    return out

def GetOMSTagList():
    """The function returns a list of tags associated
       with any active OMS requests.
       --usage
          GetOMSTagList()

        --details
          A list of strings specifying the tags for any
          active OMS requests. The structure of this list
          is the same as returned from the GetHandleList
          function, so the functions GetStringListLength
          and GetStringFromList may be used to obtain the
          string value of the handles.RemoveStringList can
          be used to free the storage relating to this list.
          
        --value
          Get a list of tags from any active OMS requests.

        --example
          import spss
          try:
            spss.Submit("GET FILE='Employee data.sav'.
              OMS
             /SELECT TABLES
             /IF COMMANDS=['Frequencies'] SUBTYPES=['Frequencies']
             /DESTINATION FORMAT=OXML IMAGES=NO OUTFILE='/temp/GetOMSTagList.xml'
             /TAG='myTag'.")
            tags = spss.GetOMSTagList()
            for tag in tags:
                print tag
         except spss.SpssError:
              print "Error."
              print spss.GetLastErrorMessage()
    """
    error.Reset()
    if not IsBackendReady():
        StartSPSS()
        
    result = PyInvokeSpss.GetOMSTagList()
    out = result[0:-1]
    errLevel = result[-1]

    error.SetErrorCode(errLevel)
    if error.IsError():
        raise SpssError,error

    return out

def GetCaseCount():
    """Returns the total number of cases (rows) in the active dataset.

        --usage
          GetCaseCount()

        --details
          The return value will be -1 when the number of cases (rows) is
          unknown.
          The return value may be unknown prior to the complete pass of the
          active dataset.

        --value
          The number of cases (rows) in the active dataset.

        --example
          import spss
          try:
              spss.Submit("GET FILE='demo.sav'.")
              print spss.GetCaseCount()
          except spss.SpssError:
              print "Error."
          print spss.GetLastErrorMessage()

    """
    error.Reset()
    if not IsBackendReady():
        StartSPSS()

    #Get the number of cases in the active data file
    #result is a list, first element is the number of cases
    #second element is error code from spss
    result = PyInvokeSpss.GetCaseCount()
    out = result[0]
    errLevel = result[1]
    if HasCursor() == 1:
        out += cursors.newCaseCount

    error.SetErrorCode(errLevel)
    if error.IsError():
        raise SpssError,error

    return out

def GetVariableCount():
    """Returns the number of variables in the active dataset.

        --usage
          GetVariableCount()

        --value
          Return the number of variables in the active dataset.

        --example
          import spss
          try:
              spss.Submit("GET FILE='demo.sav'.")
              print spss.GetVariableCount()
          except spss.SpssError:
              print "Error."
          print spss.GetLastErrorMessage()

    """
    error.Reset()
    if not IsBackendReady():
        StartSPSS()

    #Get the number of variables in the active data file
    #result is a list, first element is the number of variables
    #second element is error code from spss
    result = PyInvokeSpss.GetVariableCount()
    out = result[0]
    errLevel = result[1]

    error.SetErrorCode(errLevel)
    if error.IsError():
        raise SpssError,error

    return out

def GetVariableFormat(index):
    """Returns a string containing the display format for the
    variable in the active dataset indicated by the index value.

        --usage
          GetVariableFormat(index)

        --arguments
          index: The position of the variable in the active dataset,
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
          "MTIME": Time of the general form mm:ss.ss.
          "YMDHMS": Date and time of the general form yyyy-mm-dd hh:mm:ss.ss.

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
              varcount= spss.GetVariableCount()
              for index in xrange(varcount):
                 print spss.GetVariableFormat(index)
          except spss.SpssError:
              print "Error."
          print spss.GetLastErrorMessage()

    """
    error.Reset()
    #Check the type of the argument
    if not isinstance(index,int):
        error.SetErrorCode(1000)
        raise SpssError,error

    if not IsBackendReady():
        StartSPSS()

    #Get the format of a variable in the active data file
    #result is a list, first element is the format
    #second element is error code from spss
    result = PyInvokeSpss.GetVariableFormat(index)
    out = result[0]
    errLevel = result[1]

    error.SetErrorCode(errLevel)
    if error.IsError():
        raise SpssError,error

    return out

def GetVariableLabel(index):
    """Returns a character string containing the variable label
    for the variable in the active dataset indicated by the index value.

        --usage
          GetVariableLabel(index)

        --arguments
          index: The position of the variable in the active dataset,
          starting with 0 for the first variable in file order.

        --details
          If the variable does not have a defined value label, an
          empty string is returned.

        --value
          A string containing the variable label.

        --example
          try:
              spss.Submit("GET FILE='demo.sav'.")
              index = 0
              print spss.GetVariableLabel(index)
          except spss.SpssError:
              print "Error."
          print spss.GetLastErrorMessage()

    """
    error.Reset()
    #Check the type of the argument
    if not isinstance(index,int):
        error.SetErrorCode(1000)
        raise SpssError,error

    if not IsBackendReady():
        StartSPSS()

    #Get the label of a variable in the active data file
    #result is a list, first element is the label
    #second element is error code from spss
    result = PyInvokeSpss.GetVariableLabel(index)
    out = result[0]
    errLevel = result[1]

    error.SetErrorCode(errLevel)
    if error.IsError():
        raise SpssError,error

    return out

def GetVariableMeasurementLevel(index):
    """Returns a string value that indicates the measurement level for
    the variable in the active dataset indicated by the index value.

        --usage
          GetVariableMeasurementLevel(index)

        --arguments
          index: The position of the variable in the active dataset,
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
              varcount = spss.GetVariableCount()
              for index in xrange(varcount):
                   print spss.GetVariableMeasurementLevel(index)
          except spss.SpssError:
              print "Error."
          print spss.GetLastErrorMessage()

    """
    error.Reset()
    #Check the type of the argument
    if not isinstance(index,int):
        error.SetErrorCode(1000)
        raise SpssError,error

    if not IsBackendReady():
        StartSPSS()

    measurementlevel = {0:"error",
                        1:"unknown",
                        2:"nominal",
                        3:"ordinal",
                        4:"scale"
                       }

    #Get the measurement level of a variable in the active data file
    #result is a list, first element is the measurement level
    #second element is error code from spss
    result = PyInvokeSpss.GetVariableMeasurementLevel(index)
    out = measurementlevel[result[0]]
    errLevel = result[1]

    error.SetErrorCode(errLevel)
    if error.IsError():
        raise SpssError,error

    return out

def GetVariableName(index):
    """ Returns a character string containing the variable name for
    the variable in the active dataset indicated by the index value.

        --usage
          GetVariableName(index)

        --arguments
          index: The position of the variable in the active dataset,
                 starting with 0 for the first variable in file order.

        --value
          A string containing the variable name.

        --example
          import spss
          try:
              spss.Submit("GET FILE='demo.sav'.")
              index = 0
              print spss.GetVariableName(index)
          except spss.SpssError:
              print "Error."
          print spss.GetLastErrorMessage()

    """
    error.Reset()
    #Check the type of the argument
    if not isinstance(index,int):
        error.SetErrorCode(1000)
        raise SpssError,error

    if not IsBackendReady():
        StartSPSS()

    #Get the name of a variable in the active data file
    #result is a list, first element is the name
    #second element is error code from spss
    result = PyInvokeSpss.GetVariableName(index)
    out = result[0]
    errLevel = result[1]

    error.SetErrorCode(errLevel)
    if error.IsError():
        raise SpssError,error

    return out

def GetVariableType(index):
    """Returns 0 for numeric variables or the defined length for string
    variables for the variable in the active dataset indicated by the
    index value.

        --usage
          GetVariableType(index)

        --arguments
          index: The position of the variable in the active dataset,
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
              varcount = spss.GetVariableCount()
              for index in xrange(varcount):
                 print spss.GetVariableType(index)
          except spss.SpssError:
              print "Error."
          print spss.GetLastErrorMessage()

    """
    error.Reset()
    #Check the type of the argument
    if not isinstance(index,int):
        error.SetErrorCode(1000)
        raise SpssError,error

    if not IsBackendReady():
        StartSPSS()

    #Get the type of a variable in the active data file
    #result is a list, first element is the type
    #second element is error code from spss
    result = PyInvokeSpss.GetVariableType(index)
    out = result[0]
    errLevel = result[1]

    error.SetErrorCode(errLevel)
    if error.IsError():
        raise SpssError,error

    return out

def GetVarMissingValues(index):
    """Returns a tuple of variable missing values from the active data source.
        For numeric variables, the tuple will consist of one of the following:
           up to three discrete missing values
           Two missing values that are the endpoints of a range
           Three missing values that are the endpoints of a range and a single discrete missing value.

        --usage
          spss.GetVarMissingValues(index)

        --arguments
          index: The position of the variable in the active data source,
          starting with 0 for the first variable in file order.

        --value
          a tuple indicating the missing values.

        --example
          import spss
          try:
              spss.Submit("GET FILE='Employee data.sav'.")
              varcount = spss.GetVariableCount()
              for index in xrange(varcount):
                 print spss.GetVarMissingValues(index)
          except spss.SpssError:
              print "Error."
          print spss.GetLastErrorMessage()
    """
    error.Reset()

    if not isinstance(index,(int,long)):
        error.SetErrorCode(1000)
        raise SpssError,error

    type = GetVariableType(index)

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

    error.SetErrorCode(errLevel)
    if error.IsError():
        raise SpssError,error

    return tuple(out)

def GetVarAttributeNames(index):
    """It returns a tuple of attribute names for the indexed variable from the active data source.
       The index is variable index in the active data source.

       --examples:

             import spss
             spss.Submit("get file = 'demo.sav'.")
             spss.Submit("variable attribute variables=age, marital attribute=test[1] ('1') test[2] ('2').")
             var = spss.GetVarAttributeNames(0)
             print var

    """
    error.Reset()

    if not isinstance(index,(int,long)):
        error.SetErrorCode(1000)
        raise SpssError,error

    result = PyInvokeSpss.GetVarAttributeNames(index)
    out = result[0]
    errLevel = result[1]

    error.SetErrorCode(errLevel)
    if error.IsError():
        raise SpssError,error

    return out

def GetVarAttributes(index,attrName):
    """It returns a tuple of the variable attributes for attribute name in the
        indexed variable.The index is variable index in the active data source.
        The attrName should be gotten from GetVarAttributeNames(index).

        --examples:

          import spss
          spss.Submit("get file = 'demo.sav'.")
          spss.Submit("variable attribute variables=age, marital attribute=test[1] ('1') test[2] ('2').")
          var = spss.GetVarAttributeNames(0)
          print var
          attrValue=spss.GetVarAttributes(0,"test")
          print attrValue
    """
    error.Reset()

    if not IsBackendReady(): StartSPSS()

    if not isinstance(index,(int,long)):
        error.SetErrorCode(1000)
        raise SpssError,error

    #Check the type of the argument
    attrName = spssutil.CheckStr(attrName)

    result = PyInvokeSpss.GetVarAttributes(index,attrName)

    out = result[0]
    errLevel = result[1]

    error.SetErrorCode(errLevel)
    if error.IsError():
        raise SpssError,error

    return out

def PostOutput(text):
    """This function is not part of the programmability API.
    Calling this function directly may have unexpected side effects.
    The interface of this function may change in future releases
    without prior notice.

    """
    error.Reset()

    if not IsBackendReady(): StartSPSS()

    #Check the type of the argument
    text = spssutil.CheckOutputStrU(text)    

    #Post text messages to SPSS output log
    error.SetErrorCode(PyInvokeSpss.PostSpssOutput(text,len(text)))
    if error.IsError():
        raise SpssError,error

def StopSPSS():
    """Stops IBM SPSS Statistics so that it cannot process commands.

        --usage
          StopSPSS()

        --details
          Use this function when ruuning programs from Python. When used from
          a IBM SPSS Statistics program block this function has no effect.

        --example
          import spss
          try:
              spss.StopSPSS()
          except spss.SpssError:
              print "Error."
          print spss.GetLastErrorMessage()

    """
    error.Reset()
    if not IsBackendReady():
        return

    depth = PyInvokeSpss.GetNestDepth()[0]
    #Prohibit to stop spss from spss drive mode
    if not isPythonDrive() or depth > 0 :
        error.SetErrorCode(1006)
        return

    if HasCursor() == 1:
        error.SetErrorCode(1020)
        raise SpssError,error

    EndDataStep()

    curMode,originMode,errLevel = PyInvokeSpss.GetXDriveMode()
    if originMode != 2:
        ropSPSS.close()

    PyInvokeSpss.StopSpss()
    spssutil.cmnglb.update(backendready = False)

    os.remove(tempfileS)

def GetNestState():
    """Show the state of nest.

        --usage
          GetNestState()
          
        --details
            Returns True or False.

        --example
          import spss
          try:
              spss.StartSPSS()
              spss.GetNestState()
          except spss.SpssError:
              print "Error."
          print spss.GetLastErrorMessage()

    """
    error.Reset()
    neststate = False
    if not IsBackendReady():
        return neststate

    depth = PyInvokeSpss.GetNestDepth()[0]
    if depth > 0:
        neststate =  True
        
    return neststate

def StopPythonSPSS():
    """
      Stop IBM SPSS Statistics when Python application exits.
      
      --details
          This method will be invoked by Python application automatically when 
          exiting.
          Almost the same as StopSPSS(), the only difference is that if a cursor
          is opened, it will close cursor before invoking 
          PyInvokeSpss.StopSpss()  
    """
    error.Reset()
    if not IsBackendReady():
        return

    #Prohibit to stop spss from spss drive mode
    if not isPythonDrive():
        error.SetErrorCode(1006)
        return
    #close cursor when python exits 
    if HasCursor() == 1:
        PyInvokeSpss.close()
        
    EndDataStep()

    curMode,originMode,errLevel = PyInvokeSpss.GetXDriveMode()
    if originMode != 2:
        ropSPSS.close()

    PyInvokeSpss.StopSpss()
    spssutil.cmnglb.update(backendready = False)
    
    os.remove(tempfileS)
    
def IsOutputOn():
    """Show the state of displaying IBM SPSS Statistics output in Python.

        --usage
            IsOutputon()

        --details
            Returns True or False.

        --example
            import spss
            print spss.IsOutputOn()
    """
    error.Reset()
    if OutputSwitch:
        return True
    else:
        return False

def SetOutput(output):
    """Controls the display of IBM SPSS Statistics output in Python.

        --usage
          SetOutput(switch value)

        --arguments
         on: Display IBM SPSS Statistics output in Python.
         off: Do not display IBM SPSS Statistics output in Python.

        --details
          Use this function when ruuning programs from Python. When used from
          a IBM SPSS Statistics program block this function has no effect.
        --example
          import spss
          spss.SetOutput("on")

    """
    error.Reset()
    global OutputSwitch

    if not IsBackendReady(): StartSPSS()

    #Check the type of the argument
    output = spssutil.CheckStr(output)

    #Check the value of the argument
    if output.upper() not in ["ON","OFF"]:
        error.SetErrorCode(1007)
        raise SpssError,error

    #Setting the switch of output when external language drive mode
    if isPythonDrive():
        if output.upper() == "ON":
            OutputSwitch=True
        else:
            OutputSwitch=False
    #Give a warning when spss drive mode
    else:
        error.SetErrorCode(1009)


def Submit(cmdList):
    """Submits the command text to IBM SPSS Statistics for processing.

        --usage
          Submit(cmdList)

        --arguments
          cmdList: A string, list, or tuple of command lines.

        --details
          The argument should resolve to one or more complete IBM SPSS Statistics commands.
          For lists and tuples, each element must resolve to a string.
          If the argument is a string, it may contain new-line
          characters. String arguments are split at the new-line
          characters and the resulting list of lines is submitted. The command
          terminator is optional with a single command. You can also use the
          Python triple-quoted string convention to specify blocks of IBM SPSS Statistics
          commands on multiple lines that more closely resemble the way 
          command syntax is normally written.

        --example
          import spss
          try:
              spss.Submit("GET FILE='demo.sav'.")
              cmd = '''LIST VAR=ALL /CASES=10.
                    SHOW LIC.'''
              spss.Submit(cmd)
          except spss.SpssError:
              print "Error."
          print spss.GetLastErrorMessage()

    """
    error.Reset()

    if not IsBackendReady(): StartSPSS()

    #Check the type of the argument
    if not isinstance(cmdList,(basestring,list,tuple)):
        error.SetErrorCode(1004)
        raise SpssError,error

    tempList = []
    if isinstance((cmdList),basestring):
        cmdList = spssutil.CheckStr(cmdList)
        #Split the string by "\n"
        tempList = cmdList.split("\n")
    else:
        #Split every string element of command list or tuple by "\n"
        for x in cmdList:
            #Every element of command list or tuple must be string
            x = spssutil.CheckStr(x)
            tempList += x.split("\n")

    #Queue all command lines except the last one
    for cmd in tempList[:-1]:
        error.SetErrorCode(PyInvokeSpss.QueueCommandPart(cmd,len(cmd)))
        if error.IsError():
            raise SpssError

    #Create output to spss log when has spss output
    __PostOutputToSpss()

    #Queue the last command and execute all
    error.SetErrorCode(PyInvokeSpss.Submit(tempList[-1],len(tempList[-1])))

    spssutil.cmnglb.update(utf8mode = bool(PyInvokeSpss.IsUTF8mode()))
    
    if sys.platform == 'win32':
        cLocale = (PyInvokeSpss.GetCLocale())[0]
        locale.setlocale(locale.LC_ALL,cLocale)

    #Create output to external language log when external language drive mode
    if spssutil.cmnglb._cmnglb__xdriven and runStartSPSS:
        __PostOuputToPython()

    if error.IsError():
        raise SpssError,error


def __toString(value):
    """This function is not part of the programmability API.
    Calling this function directly may have unexpected side effects.
    The interface of this function may change in future releases
    without prior notice.

    """
    error.Reset()
    templist=[]
    if isinstance(value, unicode) and isPythonDrive():
        if PyInvokeSpss.IsUTF8mode():
            value = value.encode("utf-8")
        else:
            value = value.encode(locale.getlocale(locale.LC_CTYPE)[1])
    if isinstance(value, basestring):
        templist.append(value)
    elif isinstance(value,(int,long,float)):
        templist.append(str(value))
    elif isinstance(value,(tuple,list)):
        for x in list(value):
            templist.append(__toString(x))
    else:
        error.SetErrorCode(1004)
        raise SpssError,error

    return ' '.join(templist)

def SetMacroValue(MName,MValue):
    """Defines a IBM SPSS Statistics macro that can be used outside of a
    BEGIN/END program block in command syntax.

        --usage
          SetMacroValue(MacroName,MacroValue)

        --arguments
          MacroName: The name of the macro to be created.
          MacroValue: The value may be a number, string, tuple, or list.
          The value is converted to a string.

        --details
          SetMacroValue creates a macro with the specified name and value.
          It is a good idea to begin macro names with "!" so that the macro
          name does not conflict with IBM SPSS Statistics keywords or variables. When IBM SPSS Statistics 
          encounters a macro name in syntax, it replaces the macro name with
          the macro value before it processes the command.
          Macros cannot be used to generate Python code.

        --example
          import spss
          try:
              spss.Submit("GET FILE='demo.sav'.")
              spss.SetMacroValue('!myvarlist',['ed','age'])
              spss.Submit("FREQUENCIES VAR=!myvarlist")
          except spss.SpssError:
              print "Error."
          print spss.GetLastErrorMessage()

    """
    error.Reset()
    Submit(("define "+MName+"()",__toString(MValue),"!enddefine."))

def StartProcedure(procedureName, omsIdentifier=""):
    """Starts a user procedure.After you finish the procedure, you should call
       EndProcedrue() to end it.
       --examples:
          import spss
          try:
              spss.Submit("GET FILE='demo.sav'.")
              spss.StartProcedure("procname")
              spss.EndProcedure()
          except spss.SpssError:
              print "Error."
          print spss.GetLastErrorMessage()
    """
    global ProcDictionary
    error.Reset()
    if not IsBackendReady():
        StartSPSS()
    if(cmp(omsIdentifier,"")!=0):
        error.SetErrorCode(PyInvokeSpss.StartProcedure(spssutil.CheckStr(procedureName),spssutil.CheckStr(omsIdentifier)))
        ProcDictionary[procedureName]=omsIdentifier
    else:
        error.SetErrorCode(PyInvokeSpss.StartProcedure(spssutil.CheckStr(procedureName),spssutil.CheckStr(procedureName)))
        ProcDictionary[procedureName]=procedureName
    if error.IsError():
        ProcDictionary.clear()
        raise SpssError,error

    ## clear Python dataset
    for dataset in PythonDatasets.values():
        del dataset

def AddProcedureFootnotes(footnotes):
    """Add footnotes to an existing user procedure.
       --examples:
          import spss
          try:
              spss.Submit("GET FILE='demo.sav'.")
              spss.StartProcedure("procname")
              spss.AddProcedureFootnotes("My Procedure")
              spss.EndProcedure()
          except spss.SpssError:
              print "Error."
          print spss.GetLastErrorMessage()
    """
    error.Reset()
    footnotes = spssutil.CheckStr(footnotes)

    if not IsBackendReady():
        StartSPSS()
    error.SetErrorCode(PyInvokeSpss.AddProcFootnotes(str(footnotes)))

    if error.IsError():
        raise SpssError,error

def SplitChange(procedureName):
    """Change a splited file from one splited group to another.

        --usage
          SplitChange(procedureName)

        --arguments
          procedureName: the name of the procedure which SplitChange is used to.

        --example:
            import spss
            from spss import CellText
            from spss import FormatSpec

            spss.Submit(r"GET FILE='employee data.sav'.")
            spss.Submit("SORT CASES BY GENDER.")
            spss.Submit("SPLIT FILE LAYERED BY GENDER.")

            spss.StartProcedure("spss.com.demo")

            table = spss.BasePivotTable("Table Title","OMS table subtype")
            table.Append(spss.Dimension.Place.row,"Minority Classification")
            table.Append(spss.Dimension.Place.column,"coldim",hideName=True)

            cur=spss.Cursor()
            salary = 0; salarym = 0; n = 0; m = 0
            minorityIndex = 9
            salaryIndex = 5

            row = cur.fetchone()
            spss.SplitChange("spss.com.demo")
            while True:
               if cur.IsEndSplit():
                  if n>0:
                     salary=salary/n
                  if m>0:
                     salarym=salarym/m
                  table[(CellText.String("No"),CellText.String("Average Salary"))] = CellText.Number(salary,FormatSpec.Count)
                  table[(CellText.String("Yes"),CellText.String("Average Salary"))] = CellText.Number(salarym,FormatSpec.Count)
                  salary=0; salarym=0; n = 0; m = 0
                  row=cur.fetchone()
                  if row is not None:
                     ##There is another case, it must be a new split
                     spss.SplitChange("spss.com.demo")
                  else:
                     ##There are no more cases, so quit
                     break
               if not row:
                   break
               if row[minorityIndex]==1:
                  salarym += row[salaryIndex]
                  m += 1
               elif row[minorityIndex]==0:
                  salary += row[salaryIndex]
                  n += 1
               row=cur.fetchone()

            cur.close()
            spss.EndProcedure()
    """
    global ProcDictionary
    error.Reset()
    if not IsBackendReady():
        StartSPSS()
    
    try:
        pName = ProcDictionary[procedureName]
    except:
        pName = procedureName
    error.SetErrorCode(PyInvokeSpss.SplitChange(pName))

    if error.IsError():
        raise SpssError,error

def EndProcedure():
    """Ends a user procedure.After you finish the procedure started with
    StartProcedure, you should call EndProcedure() to end it.
       --usage:
         EndProcedure()

       --examples:
          import spss
          try:
              spss.Submit("GET FILE='demo.sav'.")
              spss.StartProcedure("procname")
              spss.EndProcedure()
          except spss.SpssError:
              print "Error."
          print spss.GetLastErrorMessage()
    """
    global ProcDictionary
    error.Reset()
    if not IsBackendReady():
        return

    #Create output to spss log when spss drive mode
    __PostOutputToSpss()
    error.SetErrorCode(PyInvokeSpss.EndProcedure())

    ## clear Python dataset
    for ds in PythonDatasets.values():
        ds._Dataset__destroy()
        del ds
    PythonDatasets.clear()

    #Create output to external language log when external language drive mode
    if isPythonDrive() and runStartSPSS:
        __PostOuputToPython()

    if error.IsError():
        raise SpssError,error
    else:
        ProcDictionary.clear()

def HasCursor():
    """Returns an integer indicating whether there is an open cursor.

       --usage:
         spss.HasCursor()

       --details:
         A value 0 means that there is no running cursor, and a value of
         1 means that there is a running cursor.

       --example:
         import spss
         try:
             hasCur = spss.HasCursor()
         except spss.SpssError:
             print "Error."
         print spss.GetLastErrorMessage()
    """
    error.Reset()
    if not IsBackendReady():
        StartSPSS()

    result = PyInvokeSpss.HasCursor()
    out = result[0]
    errLevel = result[1]

    error.SetErrorCode(errLevel)
    if error.IsError():
        raise SpssError,error

    return out

def GetSPSSLowHigh():
    """Returns the values IBM SPSS Statistics uses for LO and HI as a tuple of two values.

       --usage:
         spss.GetSPSSLowHigh()

       --details:
         The first element in the tuple is the value for LO and the second is the value for
         HI. These values can used to specify missing value ranges for new numeric variables
         with the SetVarNMissingValues method.

       --example:
         import spss
         try:
             spss.GetSPSSLowHigh()
         except spss.SpssError:
             print "Error"
         print spss.GetLastErrorMessage()
    """
    error.Reset()

    if not IsBackendReady():
        StartSPSS()

    result = PyInvokeSpss.GetSPSSLowHigh()
    low = result[0]
    high = result[1]
    errLevel = result[2]

    error.SetErrorCode(errLevel)
    if error.IsError():
        raise SpssError,error

    return low,high


def GetWeightVar():
    """Returns the name of the current weight variable, or None if there is no weight variable.

      --usage:
        GetWeightVar()

      --example:
        import spss
        try:
            spss.GetWeightVar()
        except spss.SpssError:
            print "Error"
        print spss.GetLastErrorMessage()
    """
    error.Reset()

    if not IsBackendReady():
        StartSPSS()

    result = PyInvokeSpss.GetWeightVar()
    out = result[0]
    errLevel = result[1]

    error.SetErrorCode(errLevel)
    if error.IsError():
        raise SpssError,error

    return out

def StartDataStep():
    """Start a data step. Data steps allow you to create and manage multiple datasets.

       --details
       You cannot use the following classes and functions within a data step: the Cursor class,
       the BasePivotTable class, the BaseProcedure class, the TextBlock class, the StartProcedure
       function, and the Submit function. In addition, the StartDataStep function cannot be used
       if there are pending transformations.

       --example
       import spss
       spss.StartDataStep()
       # Create a new dataset
       datasetObj = spss.Dataset(name=None)
       # Store the auto-generated name of the new dataset for use outside of the data step
       dshandle = datasetObj.name
       # Add variables and cases to the new dataset
       datasetObj.varlist.append('numvar',0)
       datasetObj.varlist.append('strvar',1)
       datasetObj.varlist['numvar'].label = 'Sample numeric variable'
       datasetObj.varlist['strvar'].label = 'Sample string variable'
       datasetObj.cases.append([1,'a'])
       datasetObj.cases.append([2,'b'])
       spss.EndDataStep()
    """
    error.Reset()

    if not IsBackendReady():
        StartSPSS()

    errLevel = PyInvokeSpss.StartDataStep()

    error.SetErrorCode(errLevel)
    if error.IsError():
        raise SpssError,error

def EndDataStep():
    """End a data step initiated with StartDataStep. The EndDataStep function destroys any existing
    Dataset objects.

       --example
       import spss
       spss.StartDataStep()
       # Create a new dataset
       datasetObj = spss.Dataset(name=None)
       # Store the auto-generated name of the new dataset for use outside of the data step
       dshandle = datasetObj.name
       # Add variables and cases to the new dataset
       datasetObj.varlist.append(0,'numvar')
       datasetObj.varlist.append(1,'strvar')
       datasetObj.varlist['numvar'].label = 'Sample numeric variable'
       datasetObj.varlist['strvar'].label = 'Sample string variable'
       datasetObj.cases.append([1,'a'])
       datasetObj.cases.append([2,'b'])
       spss.EndDataStep()
    """
    error.Reset()

    if not IsBackendReady():
        StartSPSS()

    for ds in PythonDatasets.values():
        ds._Dataset__rows.commitCase()
    
    errLevel = PyInvokeSpss.EndDataStep()

    error.SetErrorCode(errLevel)
    if error.IsError():
        raise SpssError,error

    ## clear Python dataset
    for ds in PythonDatasets.values():
        ds._Dataset__destroy()
        del ds
    PythonDatasets.clear()

def ActiveDataset():
    """Get the name of the active dataset.

       --returned value
       If the active dataset is unnamed, '*' is returned. If the current active data source is not
       associated with a dataset, None is returned.

       --example
       import spss
       name = spss.ActiveDataset()
    """
    error.Reset()

    if not IsBackendReady():
        error.SetErrorCode(17)
        raise SpssError,error

    result = PyInvokeSpss.GetActive()
    out = result[0]
    errLevel = result[1]

    error.SetErrorCode(errLevel)
    if error.IsError():
        raise SpssError,error

    isAc = False
    for ds in PythonDatasets.values():
        if ds.name == out:
            isAc = True
            break
    if not isAc:
        error.SetErrorCode(93)

    return out

def IsActive(ds):
    """Return a logical value indicating whether the specified dataset is the active one. True if the
    specified dataset is active, False otherwise.

       --arguments
       ds: A Dataset object.

       --details
       The IsActive function can only be used within a data step. Data steps are initiated with the
       StartDataStep function and are used to create and manage multiple datasets.

       --example
       import spss
       spss.StartDataStep()
       datasetObj = spss.Dataset(name="file1")
       if not spss.IsActive(datasetObj):
          spss.SetActive(datasetObj)
       spss.EndDataStep()
    """
    error.Reset()

    if not isinstance(ds, Dataset):
        raise TypeError("Expects a Dataset argument")

    if not IsBackendReady():
        error.SetErrorCode(17)
        raise SpssError,error

    result = PyInvokeSpss.GetActive()
    activeName = result[0]
    errLevel = result[1]

    if errLevel != 93:
        error.SetErrorCode(errLevel)
        if error.IsError():
            raise SpssError,error

    if activeName == ds.name:
        return True
    else:
        return False

def SetActive(ds):
    """Set the specified dataset to be the active one. The function has no effect if the specified
    dataset is already the active one.

       --arguments
       ds: A Dataset object.

       --details
       The SetActive function can only be used within a data step. Data steps are initiated with the
       StartDataStep function and are used to create and manage multiple datasets.

       --example
       import spss
       spss.StartDataStep()
       # Create a new dataset
       datasetObj = spss.Dataset(name=None)
       # Store the auto-generated name of the new dataset for use outside of the data step
       dshandle = datasetObj.name
       # Add variables and cases to the new dataset
       datasetObj.varlist.append('numvar',0)
       datasetObj.varlist.append('strvar',1)
       datasetObj.varlist['numvar'].label = 'Sample numeric variable'
       datasetObj.varlist['strvar'].label = 'Sample string variable'
       datasetObj.cases.append([1,'a'])
       datasetObj.cases.append([2,'b'])
       # Set the new dataset to be the active one
       spss.SetActive(datasetObj)
       spss.EndDataStep()
    """
    error.Reset()

    if not isinstance(ds, Dataset):
        raise TypeError("Expects a Dataset argument")

    if not IsBackendReady():
        error.SetErrorCode(17)
        raise SpssError,error

    errLevel = PyInvokeSpss.SetActive(ds.name)

    error.SetErrorCode(errLevel)
    if error.IsError():
        raise SpssError,error

def GetDatasets():
    """Get a list of the Dataset objects available within the current data step.

       --details
       The GetDatasets function can only be used within a data step. Data steps are initiated with the
       StartDataStep function and are used to create and manage multiple datasets.

       --example
       import spss
       spss.StartDataStep()
       # Create a Dataset object for the active dataset
       datasetObj1 = spss.Dataset()
       # Create a new and empty dataset
       datasetObj2 = spss.Dataset(name=None)
       datasetNames = [item.name for item in spss.GetDatasets()]
       spss.EndDataStep()
    """
    error.Reset()

    if not IsBackendReady():
        error.SetErrorCode(17)
        raise SpssError,error

    out = PythonDatasets.values()
    return out

def GetSplitVariableNames():
    """Find out the variable names of current SPLIT specification.
    """
    error.Reset()
    if not IsBackendReady():
        StartSPSS()

    result = PyInvokeSpss.GetSplitVariableNames()
    out = result[0:-1]
    #get the error code from spss
    errLevel = result[-1]

    error.SetErrorCode(errLevel)
    if error.IsError():
        raise SpssError,error

    return out

def GetDataFileAttributeNames():
    """It returns a tuple of attribute names of the active data source.

       --examples:

             import spss
             spss.Submit("get file = 'demo.sav'.")
             spss.Submit("DATAFILE ATTRIBUTE ATTRIBUTE=OriginalVersion ('1').")
             attrNames = spss.GetDataFileAttributeNames()
             print attrNames

    """
    error.Reset()

    if not IsBackendReady(): StartSPSS()

    result = PyInvokeSpss.GetDataFileAttributeNames()
    out = result[0]
    errLevel = result[1]

    error.SetErrorCode(errLevel)
    if error.IsError():
        raise SpssError,error

    return out

def GetDataFileAttributes(attrName):
    """It returns a tuple of the attributes for attribute name of the active data source.

        --examples:

          import spss
          spss.Submit("get file = 'demo.sav'.")
          spss.Submit("DATAFILE ATTRIBUTE ATTRIBUTE=OriginalVersion ('1').")
          attrNames = spss.GetDataFileAttributeNames()
          print attrNames
          attrValue=spss.GetDataFileAttributes(attrNames[0])
          print attrValue
    """
    error.Reset()

    if not IsBackendReady(): StartSPSS()

    #Check the type of the argument
    attrName = spssutil.CheckStr(attrName)

    result = PyInvokeSpss.GetDataFileAttributes(attrName)

    out = result[0]
    errLevel = result[1]

    error.SetErrorCode(errLevel)
    if error.IsError():
        raise SpssError,error

    return out

def GetMultiResponseSetNames():
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
         names = spss.GetMultiResponseSetNames()
         print names
         for i in range(len(names)):
             print spss.GetMultiResponseSet(names[i])
    """
    error.Reset()

    if not IsBackendReady(): StartSPSS()

    result = PyInvokeSpss.GetMultiResponseSetNames()
    out = result[:-1]
    errLevel = result[-1]

    error.SetErrorCode(errLevel)
    if error.IsError():
        raise SpssError,error

    if len(out) != 0:
        out=out[0]

    return out

def GetMultiResponseSet(mrsetName):
    """Returns multiple response set from procedure data source.

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

         names = spss.GetMultiResponseSetNames()
         print names
         for i in range(len(names)):
             print spss.GetMultiResponseSet(names[i])
    """
    error.Reset()

    if not IsBackendReady(): StartSPSS()

    #Check the type of the argument
    mrsetName = spssutil.CheckStr(mrsetName)

    result = PyInvokeSpss.GetMultiResponseSet(mrsetName)

    errLevel = result[-1]
    error.SetErrorCode(errLevel)
    if error.IsError():
        raise SpssError,error

    return result[:-1]

def GetSPSSLocale():
    """Retrieve the locale of the IBM SPSS Statistics processor.
         --examples:

         import spss
         spss.StartSPSS()
         print spss.GetSPSSLocale()
    """
    error.Reset()

    if not IsBackendReady(): StartSPSS()
    result = PyInvokeSpss.GetSPSSLocale()

    errLevel = result[1]
    error.SetErrorCode(errLevel)
    if error.IsError():
        raise SpssError,error

    return result[0]


def SetOutputLanguage(olang):
    """Set the output language for the IBM SPSS Statistics processor. The argument is one of the following
    strings:
        English
        French
        German
        Italian
        Japanese
        Korean
        Polish
        Russian
        SChinese
        Spanish
        TChinese
        BPortugu

    
         --examples:

         import spss
         spss.StartSPSS()
         spss.SetOutputLanguage("German")
    """
    error.Reset()

    if not IsBackendReady(): StartSPSS()

    #Check the type of the argument
    mrsetName = spssutil.CheckStr(olang)
    result = PyInvokeSpss.SetOutputLanguage(olang)

    errLevel = result
    error.SetErrorCode(errLevel)
    if error.IsError():
        raise SpssError,error
    else:
        if ("Simplified Chinese" == olang):
            olang = "SChinese"
        elif ("Traditional Chinese" == olang):
            olang = "TChinese"
        os.environ["LANGUAGE"]=olang
        spssutil.SetErrorMessage()

def GetFileHandles():
    """Returns a list of currently defined file handls.

        --usage
          GetFileHandles()

        --details
          If there is no valid handle, the list will empty.

        --value
          A list of all files handles that exist in the processor.

        --example
          import spss
          try:
              spss.Submit("FILE HANDLE demo /NAME='demo.sav'.")
              handlelist = spss.GetFileHandles()
              for handle in handlelist:
                  print handle
          except spss.SpssError:
              print "Error."
          print spss.GetLastErrorMessage()

    """
    error.Reset()
    if not IsBackendReady():
        StartSPSS()

    #Get a list of existing handles, the last element is error code from spss
    result = PyInvokeSpss.GetFileHandles()
    out = result[0:-1]
    tripleout = []
    x = 0
    while x < len(out):
        aHandle = [out[x],out[x+1],out[x+2]]
        x += 3
        tripleout.append(aHandle)

    errLevel = result[-1]

    error.SetErrorCode(errLevel)
    if error.IsError():
        raise SpssError,error

    return tripleout
    
def GetVariableRole(index):
    """Returns a string value that indicates the role for
    the variable in the active dataset indicated by the index value.

        --usage
          GetVariableRole(index)

        --arguments
          index: The position of the variable in the active dataset,
                 starting with 0 for the first variable in file order.

        --details
          The value returned can be: "Input", "Target", "Both",
          "None", "Partition", or "Split".

        --value
          A string containing the role.

        --example
          import spss
          try:
              spss.Submit("GET FILE='demo.sav'.")
              varcount = spss.GetVariableCount()
              for index in xrange(varcount):
                   print spss.GetVariableRole(index)
          except spss.SpssError:
              print "Error."
          print spss.GetLastErrorMessage()

    """
    error.Reset()
    #Check the type of the argument
    if not isinstance(index,int):
        error.SetErrorCode(1000)
        raise SpssError,error

    if not IsBackendReady():
        StartSPSS()

    varRole = { 0:"Input",
                1:"Target",
                2:"Both",
                3:"None",
                4:"Partition",
                5:"Split",
                6:"Frequency"
              }

    #Get the role of a variable in the active data file
    #result is a list, first element is the role
    #second element is error code from spss
    result = PyInvokeSpss.GetVariableRole(index)
    out = varRole[result[0]]
    errLevel = result[1]

    error.SetErrorCode(errLevel)
    if error.IsError():
        raise SpssError,error

    return out
    
    
def IsDistributedMode():
    """Returns a logical value indicating whether IBM SPSS Statistics is in distributed mode. True if 
    SPSS Statistics is in distributed mode, False otherwise.

       --details
         The IsDistributedMode function always returns False when an external Python process is 
         controlling the SPSS Statistics backend.

       --example
         import spss
         spss.IsDistributedMode()
    """
    
    error.Reset()
    result = PyInvokeSpss.IsDistributedMode()
    return bool(result)
    
def CreateSocket():
    """Create and return the port for thin client"""
    
    error.Reset()
    result = PyInvokeSpss.CreateSocket()
    port = result[0]
    errLevel = result[1]
    
    error.SetErrorCode(errLevel)
    if error.IsError():
        raise SpssError,error
        
    return port
    
def AcceptSocket():
    """Accept the connect from thin client"""
    
    error.Reset()
    errLevel = PyInvokeSpss.AcceptSocket()
    
    error.SetErrorCode(errLevel)
    if error.IsError():
        raise SpssError,error


class DataStep(object):
    """Supports the Python with statement for data steps.
       This eliminates the need to call StartDataStep and EndDataStep.
       Note that 'with DataStep' will execute any pending transformations.
         --examples:
            from __future__ import with_statement
            from spss import *
            with DataStep():
                # Create a new dataset
                datasetObj = Dataset(name=None)
                # Store the auto-generated name of the new dataset for use outside of the data step
                dshandle = datasetObj.name
                # Add variables and cases to the new dataset
                datasetObj.varlist.append('numvar')
                datasetObj.varlist.append('strvar',1)
                datasetObj.varlist['numvar'].label = 'Sample numeric variable'
                datasetObj.varlist['strvar'].label = 'Sample string variable'
                datasetObj.cases.append([1,'a'])
                datasetObj.cases.append([2,'b'])
    """
    def __enter__(self):
        """initialization for with statement"""
        try:
            StartDataStep()
        except:
            Submit("EXECUTE")
            StartDataStep()
        return self

    def __exit__(self, type, value, tb):
        EndDataStep()
        return False

class Procedure(object):
    """Supports the Python with statement for user procedures.
       This eliminates the need to call StartProcedure and EndProcedure.
         --examples:
            from __future__ import with_statement
            from spss import *
            Submit("GET FILE='demo.sav'.")
            with Procedure("procname"):
                text = TextBlock("Text Block","Sample text")
    """
    def __init__(self,procName, omsIdentifier=""):
        self.procName = procName
        self.omsIdentifier = omsIdentifier
        #return self
    
    def __enter__(self):
        """initialization for with statement"""
        try:
            StartProcedure(self.procName, self.omsIdentifier)
        except:
            Submit("EXECUTE")
            StartProcedure(self.procName, self.omsIdentifier)
        return self

    def __exit__(self, type, value, tb):
        EndProcedure()
        return False


__all__ = ["CreateXPathDictionary",
           "DeleteXPathHandle",
           "EvaluateXPath",
           "GetCaseCount",
           "GetVariableCount",
           "GetVariableFormat",
           "GetVariableLabel",
           "GetVariableMeasurementLevel",
           "GetVariableRole",
           "GetVariableName",
           "GetVariableType",
           "StartSPSS",
           "StopSPSS",
           "Submit",
           "IsBackendReady",
           "GetLastErrorLevel",
           "GetLastErrorMessage",
           "SpssError",
           "SetMacroValue",
           "GetXmlUtf16",
           "GetImage",
           "GetSetting",
           "GetOMSTagList",
           "GetHandleList",
           "SetOutput",
           "IsOutputOn",
           "StartProcedure",
           "SplitChange",
           "EndProcedure",
           "HasCursor",
           "GetVarMissingValues",
           "GetVarAttributeNames",
           "GetVarAttributes",
           "GetSPSSLowHigh",
           "GetWeightVar",
           "StartDataStep",
           "EndDataStep",
           "ActiveDataset",
           "IsActive",
           "SetActive",
           "GetDatasets",
           "GetSplitVariableNames",
           "AddProcedureFootnotes",
           "GetDataFileAttributeNames",
           "GetDataFileAttributes",
           "GetMultiResponseSetNames",
           "GetMultiResponseSet",
           "GetSPSSLocale",
           "SetOutputLanguage",
           "GetFileHandles",
           "DataStep",
           "Procedure",
           "GetNestState",
           "IsDistributedMode",
           "CreateSocket",
           "AcceptSocket"]

import version
__version__ = version.version
