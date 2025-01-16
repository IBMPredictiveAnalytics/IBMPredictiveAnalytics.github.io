#/***********************************************************************
# * Licensed Materials - Property of IBM 
# *
# * IBM SPSS Products: Statistics Common
# *
# * (C) Copyright IBM Corp. 1989, 2020
# *
# * US Government Users Restricted Rights - Use, duplication or disclosure
# * restricted by GSA ADP Schedule Contract with IBM Corp. 
# ************************************************************************/

""" Preparation work for running python syntax

This module redirects python stdout and stderr
into a tempfile that IBM SPSS Statistics can read
"""
#Copyright (C) 2008 by SPSS Inc.
global wop,rop,filelist
wop=[]
rop=[]
filelist=[]



def start(nestedDepth=0,spssTemp=None):
    global wop,rop,oldstdout,oldstderr,filelist
    import os,os.path,sys,codecs,locale
    from . import PyInvokeSpss, spssutil
    from .spssutil import extrapath
    ispdrive = (1 == PyInvokeSpss.GetXDriveMode()[1])
    if ispdrive and nestedDepth > 0:
        nestedDepth -= 1
        
    extrapath()
    
    spss_olang = (PyInvokeSpss.GetOutputLanguage())[0]
    if spss_olang is not None:
        os.environ["LANGUAGE"]=spss_olang
        spssutil.SetErrorMessage()
    spss_locale = (PyInvokeSpss.GetSPSSLocale())[0]
    if sys.platform == 'win32':
        cLocale = (PyInvokeSpss.GetCLocale())[0]
        locale.setlocale(locale.LC_ALL,cLocale)
        
    if spss_locale is not None:
        py_locale = locale.getlocale()[0]
        if py_locale is not None:
            os.environ["PYTHON_LOCALE"]= "".join(['"',py_locale,'"'])

    isU8mode = (PyInvokeSpss.IsUTF8mode()==1)
       
    spssutil.cmnglb.update(utf8mode = bool(isU8mode),
                           xdriven = bool(ispdrive),
                           backendready = bool(PyInvokeSpss.IsBackendReady()))
    
    ## Use python temp folder instead of SPSSDXTEMP to avoid the encoding convert
    ## between UTF-8 and codepage failed problem.
    import tempfile
    temppath = tempfile.gettempdir() 
    ## Add pid to ensure different process of Statistics wouldn't conflict with 
    ## each other in temp file 
    processid = str(os.getpid())
    filename = processid + 'py_spss.tmp' + str(nestedDepth)
    if temppath:
        tempfullname = os.path.join(temppath, filename)
    else:
        tempfullname = os.path.join(spssTemp, filename)

    try:
        wop[nestedDepth]
    except:
        ver = sys.version
        if ver.split(' ')[0] < "2.6.0":
            sys.argv = ' '
        else:
            sys.argv = [' ']

    dataenc = codecs.lookup("utf-8")
    ## Always using utf-8 encoding    
    fileenc = codecs.lookup("raw_unicode_escape")

    filew = open(tempfullname,'wb')
    filer = open(tempfullname,'r')

    if isU8mode:
        PyInvokeSpss.SetDefaultEncoding("utf-8")
    elif locale.getlocale()[1] is not None:
        PyInvokeSpss.SetDefaultEncoding(locale.getlocale()[1])

    from .spssutil import ConvertibleStreamRecoder
    cSRwriter = ConvertibleStreamRecoder(filew, dataenc.encode, dataenc.decode, 
            fileenc.streamreader, fileenc.streamwriter, "replace")
    cSRreader = ConvertibleStreamRecoder(filer, dataenc.encode, dataenc.decode, 
            fileenc.streamreader, fileenc.streamwriter, "replace")     
    wop.append(cSRwriter)
    rop.append(cSRreader)
    filelist.append(tempfullname)      

    #try:
    #    os.chmod(tempfullname, 0777)
    #except:
    #    pass

    if nestedDepth == 0:
        oldstdout = sys.stdout
        oldstderr = sys.stderr
    
    sys.stdout = wop[-1]    
    sys.stderr = wop[-1]


from . import version
__version__ = version.version
