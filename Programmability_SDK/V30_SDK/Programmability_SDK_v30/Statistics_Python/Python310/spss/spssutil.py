#/***********************************************************************
# * Licensed Materials - Property of IBM
# *
# * IBM SPSS Products: Statistics Common
# *
# * (C) Copyright IBM Corp. 1989, 2022
# *
# * US Government Users Restricted Rights - Use, duplication or disclosure
# * restricted by GSA ADP Schedule Contract with IBM Corp.
# ************************************************************************/


#Copyright (C) 2008 by SPSS Inc.

import locale, os, codecs, sys
from . import PyInvokeSpss
from .errMsg import *
from .spss import error
from .errMsg import errTable
from .errMsg import noError
from os.path import exists
import datetime
import time
import codecs

class __cmnglb:
    def __init__(self):
        self.__utf8mode = False
        self.__xdriven = True
        self.__backendready = False

    def update(self, utf8mode = None, xdriven = None, backendready = None):
        if utf8mode is not None:
            self.__utf8mode = utf8mode
        if xdriven is not None:
            self.__xdriven = xdriven
        if backendready is not None:
            self.__backendready = backendready

    def __repr__(self):
        return " ".join([str(self.__utf8mode), str(self.__xdriven), str(self.__backendready)])

##StreamRecoder convert codepage string into unicode string
class ConvertibleStreamRecoder(codecs.StreamRecoder):
    def __init__(self, stream, encode, decode, Reader, Writer,
                 errors='strict'):
        import sys
        self.enc = sys.getdefaultencoding()
        codecs.StreamRecoder.__init__(self, stream, encode, decode, Reader, Writer,
                 errors)
    def write(self, data):
        codecs.StreamRecoder.write(self,data.encode())

global cmnglb
cmnglb = __cmnglb()

## For temp file using UTF-8 encoding no matter what whether Statistics
## setting unicode on or off
def CheckOutputStrU(mystr):
    """check the argument of string type, and match the backend unicode or code page mode.
    """
    global cmnglb
    if isinstance(mystr,str):
        try:
            return mystr.encode("raw_unicode_escape").decode("raw_unicode_escape")
        except UnicodeDecodeError:
            return mystr
    else:
        error.SetErrorCode(1001)
        raise SpssError(error)

def CvtSpssDatetime(dt):
    """Return a Python datetime object from an SPSS datetime value.

    Note that SPSS day of week and month of year values cannot be converted."""
    if not dt:
        return None
    if dt < 86400:
        error.SetErrorCode(1069)
        raise SpssError(error)

    #t = dt//86400   # date part
    #time = dt % 86400
    t, time = divmod(dt, 86400)
    j = t + 578041
    y = (4*j-1)//146097
    j = (4*j) -1 - (146097*y)
    d = j//4
    j = (4*d+3)//1461
    d = (4*d) + 3 -(1461*j)
    d = (d+4)//4
    m = (5*d-3)//153
    d = (5*d) - 3 - (153*m)
    d = (d+5)//5
    y = 100*y + j
    if (m < 10):
        m = m + 3
    else:
        m = m - 9
        y = y + 1

    itime = int(time)
    hr, itime = divmod(itime, 60*60)
    minute, itime = divmod(itime, 60)
    return datetime.datetime(int(y), int(m), int(d), int(hr), int(minute), int(itime), int(round((time-int(time)),3)*10**6))

def CvtPythonDatetime(dt):
    """Return an SPSS datetime value from a Python datetime object.

    Note that SPSS day of week and month of year values cannot be converted."""
    if isinstance(dt, time.struct_time):
        dt = datetime.datetime(dt.tm_year, dt.tm_mon, dt.tm_mday, dt.tm_hour, dt.tm_min, dt.tm_sec)
    elif isinstance(dt, datetime.date) and not isinstance(dt, datetime.datetime):
        dt = datetime.datetime(dt.year, dt.month, dt.day, 0, 0, 0)

    if isinstance(dt, datetime.datetime):
        origtime = datetime.datetime(1582, 10, 14, 0, 0, 0)
        diffdays = (dt-origtime).days
        newtime = datetime.datetime(dt.year, dt.month, dt.day, 0, 0, 0)
        diffsecs = (dt-newtime).seconds
        diffmicrosecs = dt.microsecond
        dt = diffdays*86400 + diffsecs + float(diffmicrosecs)/ 10**6

    return dt

def isDateVariable(var):
    """return True if var is an SPSS date variable.

    That is, its format is any of
    DATE, ADATE, EDATE, JDATE, SDATE, QYR, MOYR, WKYR, or DATETIME.  These are the variables that
    can be converted to Python dates.  TIME, WKDAY, and MONTH
    formats cannot be converted to dates, because they do not represent a particular point in
    time and, hence, this function returns False for them.

    var may be an SPSS dictionary variable index, or a Variable reference from a VariableDict object.
    If a VariableDict object is supplied, var may be a string holding a variable name."""
    if isinstance(var, str):
        return var[:3] in ['DAT','ADA','EDA','JDA','SDA','QYR','MOY','WKY','DAT','MTI','YMD']
    elif isinstance(var, int):
        return var in set([20,22,23,24,28,29,30,38,39,85,86])

## Define with python duck-typing.
def CheckStr(mystr):
    """check the argument of string type, and match the backend unicode or code page mode.
    """
    global cmnglb
    if isinstance(mystr,str):
        return mystr
    else:
        error.SetErrorCode(1001)
        raise SpssError(error)

def CheckStr16(mystr):
    """check the argument of string type, and match the backend unicode for utf16 or code page mode.
    """
    global cmnglb
    if hasattr(mystr, "encode"):
        if cmnglb._cmnglb__utf8mode:
            try:
                result = str(mystr,locale.getlocale(locale.LC_CTYPE)[1])
            except TypeError:
                result = str(mystr)
            except LookupError:
                result = str(mystr)
            return result
        else:
            try:
                result = mystr.encode(locale.getlocale(locale.LC_CTYPE)[1])
            except TypeError:
                result = mystr.encode()
            except LookupError:
                result = mystr.encode()
            return result
    else:
        error.SetErrorCode(1001)
        raise SpssError(error)


def extrapath():
    import sys, os, configparser
    pkgPath = os.path.dirname(os.path.abspath(__file__))
    spssxdcfg = os.path.join(pkgPath, 'spssxdcfg.ini')
    if os.path.exists(spssxdcfg):
        # Read spssxd config file
        config = configparser.ConfigParser()
        try:
            config.read(spssxdcfg, encoding='utf-8')
        except:
            config.read(spssxdcfg)

        try:
            SpssxdVersionType = config.get('version','SpssxdVersionType')
            if SpssxdVersionType == "thinClient":
                extBasePath = "Subscription"
            else:
                extBasePath = "one"
        except Exception:
            SpssxdVersionType = "onprem"
            extBasePath = "one"

        if sys.platform.lower() == "win32":
            spssHome = os.path.abspath(os.path.join(pkgPath, "../../../.."))
        elif sys.platform.lower() == "darwin":
            if SpssxdVersionType == "thinClient":
                spssHome = os.path.abspath(os.path.join(pkgPath, "../../../../.."))
            else:
                spssHome = os.path.abspath(os.path.join(pkgPath, "../../../../../../IBM SPSS Statistics.app/Contents"))
        else:
            spssHome = os.path.abspath(os.path.join(pkgPath, "../../../../.."))

        # Set Application extension path
        if sys.platform.lower() == "win32":
            try:
                import ctypes
                from ctypes import wintypes, windll
                pathBuf = ctypes.create_unicode_buffer(wintypes.MAX_PATH)
                windll.shell32.SHGetFolderPathW(0, 0x001a, 0, 0, pathBuf)

                winAppPath = pathBuf.value
            except ImportError:
                winAppPath = os.getenv("APPDATA")
            spssAppPath = os.sep.join([winAppPath,"IBM\\SPSS Statistics"])
        elif sys.platform.lower() == "darwin":
            spssAppPath = os.sep.join([os.path.expanduser("~"),"Library/Application Support/IBM/SPSS Statistics"])
        else:
            spssAppPath = os.sep.join([os.path.expanduser("~"),".IBM/SPSS Statistics"])
        spssExtension = os.sep.join([spssAppPath, extBasePath, "extensions"])
        if os.path.isdir(spssExtension) and spssExtension not in sys.path:
            sys.path.append(spssExtension)

        # Set default extension path
        if sys.platform.lower() != "darwin":
            spssExtension = os.sep.join([os.path.realpath(spssHome), "extensions"])
        if os.path.isdir(spssExtension) and spssExtension not in sys.path:
            sys.path.append(spssExtension)

    if "SPSS_EXTENSIONS_PATH" in os.environ:
        spss_ext_path = os.environ["SPSS_EXTENSIONS_PATH"]
        if sys.platform == "win32":
            spss_ext_path = spss_ext_path.split(";")
        else:
            spss_ext_path = spss_ext_path.split(":")
        for spssExtension in spss_ext_path[::-1]:
            if os.path.isdir(spssExtension):
                spssExtension = spssExtension.replace("/", os.sep)
                if spssExtension not in sys.path:
                    sys.path.append(spssExtension)

    if "SPSSEX_EXTENSIONS_PATH" in os.environ:
        spssex_ext_path = os.environ["SPSSEX_EXTENSIONS_PATH"]
        if sys.platform == "win32":
            spssex_ext_path = spssex_ext_path.split(";")
        else:
            spssex_ext_path = spssex_ext_path.split(":")
        for spssExtension in spssex_ext_path[::-1]:
            if os.path.isdir(spssExtension):
                spssExtension = spssExtension.replace("/", os.sep)
                if spssExtension not in sys.path:
                    sys.path.append(spssExtension)


global pythonpath

def SetPythonPath(ppath):
    global pythonpath
    pythonpath = ppath

def GetLanguage():
    lang = "en"
    spss_olang = os.getenv("LANGUAGE")

    if("SChinese" == spss_olang):
        lang = "zh_cn"
    elif( "TChinese" == spss_olang):
        lang = "zh_tw"
    elif( "German" == spss_olang):
        lang = "de"
    elif( "Spanish" == spss_olang):
        lang = "es"
    elif( "French" == spss_olang):
        lang = "fr"
    elif( "Italian" == spss_olang):
        lang = "it"
    elif( "Japanese" == spss_olang):
        lang = "ja"
    elif( "Korean" == spss_olang):
        lang = "ko"
    elif( "Polish" == spss_olang):
        lang = "pl"
    elif( "Russian" == spss_olang):
        lang = "ru"
    elif("English" == spss_olang):
        lang = "en"
    elif("BPortugu" == spss_olang):
        lang = "pt_BR"

    return lang

def findPreferredOutEncoding(spss_olang=None):
    if spss_olang is None:
        spss_olang = os.getenv("LANGUAGE")
    lang_key = spss_olang
    if("SChinese" == spss_olang):
        lang_key = "Chinese-PRC"
    elif( "TChinese" == spss_olang):
        lang_key = "Chinese-Taiwan"
    elif( "BPortugu" == spss_olang):
        lang_key = "Portuguese-Brazil"
    prefEncoding = "1252" ## English as the default encoding
    from .localeMap import localeDict
    if lang_key in localeDict:
        encodinglist=[ i.split(".")[1] for i in localeDict[lang_key] if len(i.split("."))==2 ]
        for encoding in encodinglist:
            try:
                codecs.lookup(encoding)
                prefEncoding = encoding
                break
            except:
                pass
    return prefEncoding

def findLocalizedErrfile(language):
    global pythonpath
    rootPath = pythonpath
    #rootPath = __GetSpssXdPath()
    langPath = os.sep.join([rootPath, "lang", language])
    fileName = "spsspy.properties"
    errfile = os.sep.join([langPath, fileName])
    if not exists(errfile):
        language = "en"
        langPath = os.sep.join([rootPath,"lang", language])
        errfile = os.sep.join([langPath, fileName])
        print("Can not find localized error message file. Using default spsspy.properties for English.")
    if not exists(errfile):
        raise FileNotFoundError("Can not find error message file.")

    return errfile

def SetErrorMessage():
    """Read the error messages from spsspy.properties file,
    and initial global object 'errTable' with the error messages.
    """
    # read error messages from messages.err file
    language = GetLanguage()
    errfile = findLocalizedErrfile(language)

    fp = codecs.open(errfile,"rb", "utf8")
    #fp = open(errfile,"r")
    errLines = fp.readlines()
    fp.close()
    sep = ("[", "]", "=")
    # initial global object 'errTable' with error messages
    for errLine in errLines:
        errLine = errLine.strip()
        if errLine.startswith("#"):
            continue
        if not errLine.startswith(sep[0]):
            continue
        #errLine = PyInvokeSpss.TransCode(errLine)[0]
        errType = errLine.split(sep[0])[1].split(sep[1])[0].strip().lower()
        other = errLine.split(sep[0])[1].split(sep[1])[1][1:]
        errLevel = other.split(sep[2])[0].strip()
        errMsg = other.split(sep[2])[1].strip()
        if errType in errTable:
            errTable[errType][errLevel] = errMsg

        else:
            errTable[errType] = {errLevel:errMsg}

    noError = errTable['okay'][str(0)]

__all__ = ["CheckStr","CheckStr16","CvtSpssDatetime"]


from . import version
__version__ = version.version
