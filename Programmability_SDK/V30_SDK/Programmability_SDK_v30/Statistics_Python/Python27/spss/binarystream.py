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

"""

This module implements transmission data with Binary stream.

"""
import tempfile
import struct
import PyInvokeSpss,spss
from errMsg import *
from spss import error
import spssutil
import time
import datetime
import os

varFormat={1:"A", 2:"HEX", 3:"COMMA", 4:"DOLLAR", 5:"F", 6:"IB", 7:"PIBHEX", 8:"P", 9:"PIB", 10:"PK", 11:"RB", 
           12:"RBHEX", 15:"Z", 16:"N", 17:"E", 20:"DATE", 21:"TIME", 22:"DATETIME", 23:"ADATE", 24:"JDATE", 
           25:"DTIME", 26:"WKDAY", 27:"MONTH", 28:"MOYR", 29:"QYR", 30:"WKYR", 31:"PCT", 32:"DOT", 33:"CCA", 
           34:"CCB", 35:"CCC", 36:"CCD", 37:"CCE", 38:"EDATE", 39:"SDATE", 85:"MTIME", 86:"YMDHMS"}

class BaseBinary(object):
    """The base binary class. It creates the binary object.
       
       --usage
         Binary(var)
         
       --arguments
         var: A list or tuple, indicating the position of the variables in the
              active dataset,starting with 0 for the first variable in the
              file order. It is useful in read and write mode.
                      
       --details
         When the var is none, data for all variables are returned. 
    """
    
    def __init__(self, var = ()):
        self.receivedatatempfile = tempfile.NamedTemporaryFile(delete=False)
        self.senddatatempfile = tempfile.NamedTemporaryFile(delete=False)
        self.receivedatatempfile.close()
        self.receivedatafile = file
        varFmt = []
        caseLen = 0
        self.varName = []
        self.varCount = spss.GetVariableCount()
        self.varType = []
        self.varFormat = []
        self.varLen = []
            
        self.var = []
        if 0 == len(var):
            for i in xrange(self.varCount):
                self.var.append(i)
                self.varName.append(spss.GetVariableName(i))
                self.varFormat.append(spss.GetVariableFormat(i))
        else:
            for i in var:
                if i<0 or i>self.varCount:
                    error.SetErrorCode(1028)
                    raise SpssError, spss.error
            self.var = list(var)
            for i in var:
                self.varName.append(spss.GetVariableName(i))
                self.varFormat.append(spss.GetVariableFormat(i))
            
        for i in self.var:
            vtype = spss.GetVariableType(i)
            self.varType.append(vtype)
            varLen = 0
            if 0 == vtype:
                varFmt.append('d')
                varLen = 8
                caseLen += varLen
            else:
                varLen = vtype + (8 - vtype % 8) % 8
                varFmt.append('%ds' % varLen)
                caseLen += varLen
                
            self.varLen.append(varLen)
                
        name = PyInvokeSpss.GetActive()[0]
        if "*" == name:
            name = PyInvokeSpss.GetNewDatasetName()[0]
        
        self.name = name
        self.varBinaryFmt = ''.join(varFmt)
        self.caseLen = caseLen
        self.__cacheSize = 20
        self.caseCacheCount = (self.__cacheSize * 1024 * 1024) / caseLen
        self.caseLenForWrite = 0
        self.caseNumForWrite = 0
        self.writeVarType = []
        self.writeVarName = []
        self.writeVarFormat = []
        self.writeVarLen = []
        self.writeBinaryFmt = ""
        self.cacheForAppend = ""
        
        self.remainCaseCount = spss.GetCaseCount()
        self.totalCaseCount = self.remainCaseCount
        self.systemMissingValue = PyInvokeSpss.GetSystemMissingValue()
        
        self.newCaseValue = {}
        
        self._utf8mode = PyInvokeSpss.IsUTF8mode()
        
    def readreceivebinaryfile(self, readlen):
        data = self.receivedatafile.read(readlen)
        if data:
            return data
    
    def unpackdata(self, binarydata):
        case = list(struct.unpack_from(self.varBinaryFmt, binarydata, 0))
            
        strVarList = [i for i in xrange(len(self.varType)) if self.varType[i] > 0]
        for i in strVarList:
            case[i] = case[i][:self.varType[i]]
        if self._utf8mode:
            for i in strVarList:
                case[i] = unicode(case[i], encoding='utf-8')           
        
        return case
            
    def lengthForWriteMode(self):
        caseLen = 0
        varFmt = []
        varLen = 0
        for i in self.writeVarType:
            if 0 == i:
                varFmt.append('d')
                varLen = 8
                caseLen += varLen
            else:
                varLen = i + (8 - i % 8) % 8
                varFmt.append('%ds' % varLen)
                caseLen += varLen
            
            self.writeVarLen.append(varLen)
                
        self.writeBinaryFmt = ''.join(varFmt)
        self.caseLenForWrite = caseLen
        self.caseNumForWrite = (self.__cacheSize * 1024 * 1024) / caseLen
        if self.caseNumForWrite > self.remainCaseCount:
            self.caseNumForWrite = self.remainCaseCount
        
    def setWriteVarType(self, varType):
        self.writeVarType.extend(varType)
        for i in varType:
            if 0 == i:
                self.writeVarFormat.append("F8.2")
            else:
                self.writeVarFormat.append("A%d" % i)
        
    def setWriteVarName(self, varName):
        self.writeVarName.extend(varName)
        
    def setOneWriteVarType(self, varType):
        self.writeVarType.append(varType)
        if 0 == varType:
            self.writeVarFormat.append("F8.2")
        else:
            self.writeVarFormat.append("A%d" % varType)
        
    def setOneWriteVarName(self, varName):
        self.writeVarName.append(varName)
        
    def packdataForAppend(self, v1):
        try:
            tempdata = struct.pack(self.varBinaryFmt, *v1)
        except struct.error as err:
            if "string" in err:
                spss.error.SetErrorCode(56)
                raise SpssError,spss.error
            else:
                spss.error.SetErrorCode(57)
                raise SpssError,spss.error
            
        self.writeSendDataFile(tempdata)
        tempdata = None
    
    def openReceiveDataFile(self):
        if not os.path.exists(self.receivedatatempfile.name):
            error.SetErrorCode(1072)
            raise SpssError, spss.error
            
        self.receivedatafile = open(self.receivedatatempfile.name, 'rb')
        
    def setReceiveFilePosition(self, offset, whence = 0):
        self.receivedatafile.seek(offset, whence)
        
    def closeReceiveDataFile(self):
        if not self.receivedatafile.closed:
            self.receivedatafile.close()
            self.receivedatafile = None
        
    def writeSendDataFile(self, data):
        self.senddatatempfile.write(data)
        
    def setValueForChange(self, varName, varValue):
        self.newCaseValue[varName] = varValue
        
    def clearCaseValueForChange(self):
        self.newCaseValue.clear()
        
    def getCaseValueForWrite(self):
        case = []
        writeVarNameSet = set(self.writeVarName)
        for vname in self.newCaseValue:
            if vname not in writeVarNameSet:
                spss.error.SetErrorCode(82)
                if spss.error.IsError():
                    raise SpssError,spss.error
        for vname in self.writeVarName:
            index = self.writeVarName.index(vname)
            if vname in self.newCaseValue:
                value = self.newCaseValue[vname]
                if 0 != self.writeVarType[index]:
                    try:
                        blanklen = self.writeVarLen[index] - len(value)
                        blankstr = ''
                        for i in xrange(blanklen):
                            blankstr = "%s " % blankstr
                        value = "%s%s" % (value, blankstr)
                    except:
                        pass
                case.append(value)
            else:
                if 0 == self.writeVarType[index]:
                    case.append(self.systemMissingValue)
                else:
                    case.append("")
                    
        self.clearCaseValueForChange()
        return case
                    
    def getCaseValueForAppend(self):
        case = []
        for vname in self.varName:
            index = self.varName.index(vname)
            if vname in self.newCaseValue:
                value = self.newCaseValue[vname]
                if 0 !=  self.varType[index]:
                    try:
                        blanklen = self.varLen[index] - len(value)
                        blankstr = ''
                        for i in xrange(blanklen):
                            blankstr = "%s " % blankstr
                        value = "%s%s" % (value, blankstr)
                    except:
                        pass
                case.append(value)
            else:
                if 0 == self.varType[index]:
                    case.append(self.systemMissingValue)
                else:
                    case.append("")
                    
        self.clearCaseValueForChange()
        return case
        
    def getRemainCaseCount(self):
        return self.remainCaseCount
        
    def _getcachesize(self):
        return self.__cacheSize
    
    def _setcachesize(self, size):
        if 0>=size or not isinstance(size, int):
            error.SetErrorCode(1071)
            raise SpssError, spss.error
        self.__cacheSize = size
        
    cachesize = property(_getcachesize, _setcachesize)
    
class CursorBinary(BaseBinary):
    def __init__(self, var = None, accessType = "r", pos = 0):
        self.accessType = accessType
        BaseBinary.__init__(self, var)
        self.cursorPos = -1
        self.userMissing = []
        self.incMissing = False
        varTypeIndex = 0
        for index in self.var:
            if 0 == self.varType[varTypeIndex]:
                self.userMissing.append(PyInvokeSpss.GetVarNMissingValues(index)[:-1])
            else:
                self.userMissing.append(PyInvokeSpss.GetVarCMissingValues(index)[:-1])
                
            varTypeIndex += 1
                
        self.haveUserMissingIndex = []
        
        self.readdataCache = ""
        self.cachePos = 0
        self.writeNextcase = False
        self.pos = pos
        self.splitIndexes = ()
        
        for index in xrange(len(self.userMissing)):
            if len(set(self.userMissing[index][1:])) > 1:
                self.haveUserMissingIndex.append(index)
        
        if "a" != accessType:
            self.firstRead = True
            
        self.writereset = False
        self.endAppendChanges = False
        self.dictionaryChanges = False
        self.caseChanges = False
        self.newCaseCount = 0
        
    def readcache(self, binaryData):          
        currentcase = self.unpackdata(binaryData)
        
        if len(currentcase)>0:
            while self.systemMissingValue in currentcase:
                currentcase[currentcase.index(self.systemMissingValue)] = None
            if not self.incMissing:
                for i in self.haveUserMissingIndex:
                    if self.userMissing[i][1] is not None:
                        if self.varType[i] == 0:
                            if currentcase[i] in self.userMissing[i][1:]:
                                currentcase[i] = None
                                continue
                            if 0 != self.userMissing[i][0]:
                                if currentcase[i] is not None and self.userMissing[i][2]>currentcase[i]>self.userMissing[i][1]:
                                    currentcase[i] = None
                        else:
                            for missing in self.userMissing[i][1:]:
                                if missing is not None:
                                    if currentcase[i].rstrip() == missing.rstrip():
                                        currentcase[i] = None
                                        break
                
            return tuple(currentcase)
        
    def nextcase(self):
        if self.writeNextcase and self.dictionaryChanges:
            if not self.writereset:
                self.packdataForWrite(self.getCaseValueForWrite())
                self.writeNextcase = False
        self.cursorPos += 1
        if self.remainCaseCount<=0:
            return None
        
        if self.cursorPos in self.splitIndexes:
            return None
        
        if self.cachePos >= (self.caseCacheCount * self.caseLen) or self.remainCaseCount == self.totalCaseCount:
            self.readdataCache = self.readreceivebinaryfile(self.caseCacheCount * self.caseLen)
            self.cachePos = 0
            
        tempdata = self.readdataCache[self.cachePos:(self.cachePos+self.caseLen)]
        self.cachePos += self.caseLen
        self.remainCaseCount -= 1
        if "w" == self.accessType:
            self.writeNextcase = True
        return tempdata
    
    def readsendbinaryfile(self, readlen):
        data = self.senddatatempfile.file.read(readlen)
        if data:
            return data
    
    def nextWriteCase(self):
        tempdata = self.readsendbinaryfile(self.caseLenForWrite)
        return tempdata
    
    def unpackWriteData(self, binarydata):
        case = list(struct.unpack_from(self.writeBinaryFmt, binarydata, 0))
            
        strVarList = [i for i in xrange(len(self.writeVarType)) if self.writeVarType[i] > 0]
        for i in strVarList:
            case[i] = case[i][:self.writeVarType[i]]
        if self._utf8mode:
            for i in strVarList:
                case[i] = unicode(case[i], encoding='utf-8')           
        
        return case
    
    def readWritecache(self, binaryData):
        currentcase = self.unpackWriteData(binaryData)
        
        if len(currentcase)>0:
            while self.systemMissingValue in currentcase:
                currentcase[currentcase.index(self.systemMissingValue)] = None
                
            return currentcase
        
    def fetchData(self):
        data = self.nextcase()
        if data is None:
            return None
        
        activeCase = self.readcache(data)
        if not self.writereset:
            return activeCase
        else:
            tempdata = self.nextWriteCase()
            if tempdata is None:
                return activeCase
            else:
                writeCase = self.readWritecache(tempdata)
                actualCase = []
                actualCase.extend(activeCase)
                actualCase.extend(writeCase)
                return tuple(actualCase)
            
    def isEndSplit(self):
        if self.cursorPos in self.splitIndexes:
            return 1
        else:
            return 0
        
    def reset(self):
        try:
            self.receivedatafile.seek(0)
        except:
            pass
        self.remainCaseCount = self.totalCaseCount
        self.cursorPos = -1
        self.pos = 0
        if "w" == self.accessType:
            self.writereset = True
            self.senddatatempfile.file.flush()
            self.senddatatempfile.file.seek(0)
        
    def submitAppendCasesCmd(self, srcDataset):
        cmd = "ADD FILES /FILE=* /FILE='%s'." % srcDataset
        PyInvokeSpss.Submit(cmd, len(cmd))
        cmd = "EXECUTE."
        PyInvokeSpss.Submit(cmd, len(cmd))
        
        cmd = "DATASET CLOSE %s." % srcDataset
        PyInvokeSpss.Submit(cmd, len(cmd))

    def submitAppendVariablesCmd(self, srcDataset, renamelist, droplist):
        cmd = "MATCH FILES /FILE=* /RENAME (%s) /FILE='%s' /DROP=%s." % (renamelist, srcDataset, droplist)
        PyInvokeSpss.QueueCommandPart(cmd, len(cmd))
        cmd = "EXECUTE."
        PyInvokeSpss.Submit(cmd, len(cmd))
        
        cmd = "DATASET CLOSE %s." % srcDataset
        PyInvokeSpss.Submit(cmd, len(cmd))
    
    def setUserMissingInclude(self, incMissing):
        self.incMissing = incMissing
        
    def packdataForWrite(self, v1):
        try:
            if not self.cursorPos < 0 and self.cursorPos < self.totalCaseCount + len(self.splitIndexes):
                tempdata = struct.pack(self.writeBinaryFmt, *v1)
                self.writeSendDataFile(tempdata)
                tempdata = None
                self.caseChanges = True
        except struct.error:
            spss.error.SetErrorCode(57)
            raise SpssError, spss.error
        
        if self.writeNextcase:    
            self.writeNextcase = False
        else:
            self.cursorPos += 1
            self.remainCaseCount -= 1
            
    def commitCase(self):
        self.newCaseCount += 1
        
    def getNewCaseCount(self):
        return self.newCaseCount
    
    def endChanges(self):
        self.endAppendChanges = True
        
    def getEndChangesStatus(self):
        return self.endAppendChanges
        
    def commitDictionary(self):
        self.writereset = False
        self.dictionaryChanges = True
        
    def isSplit(self):
        if len(self.splitIndexes) > 1:
            return True
        else:
            return False
    
    def getSplitRemainCaseCount(self):
        cursorPos = self.cursorPos + 1
        if cursorPos > self.totalCaseCount + len(self.splitIndexes) - 1:
            return 0
        pos = len([i for i in self.splitIndexes if cursorPos>i])
        return self.splitIndexes[pos] - cursorPos
    
    def saveData(self):
        if self.firstRead:
            if self.varCount == len(self.var) and self.__isSorted(self.var):
                PyInvokeSpss.SaveDataToTempFile(self.receivedatatempfile.name, ())
            else:
                PyInvokeSpss.SaveDataToTempFile(self.receivedatatempfile.name, tuple(self.var))
            result = PyInvokeSpss.GetRowCountInTempFile()
            errLevel = result[1]
            spss.error.SetErrorCode(errLevel)
            if spss.error.IsError():
                raise SpssError,spss.error
            self.remainCaseCount = result[0]
            self.totalCaseCount = self.remainCaseCount
            self.openReceiveDataFile()
            self.setReceiveFilePosition(self.pos*self.caseLen)
            result = PyInvokeSpss.GetSplitEndIndex()
            errLevel = result[2]
            spss.error.SetErrorCode(errLevel)
            if spss.error.IsError():
                raise SpssError,spss.error
            if result[1]>0:
                self.splitIndexes = result[0]
                
            self.firstRead = False
            self.writereset = False
        else:
            pass

    def __del__(self):
        if "a" != self.accessType:
            self.closeReceiveDataFile()
            os.unlink(self.receivedatatempfile.name)
            
        if "r" != self.accessType:
            self.senddatatempfile.close()
            
        if "w" == self.accessType:
            if self.dictionaryChanges and self.caseChanges:
                newvarsCount = len(self.writeVarName)
                if newvarsCount > 0:
                    index = 0
                    renamelist = ""
                    droplist = ""
                    for i in xrange(newvarsCount):
                        while 1:
                            tempname = "d%d" % index
                            varlist = self.varName + self.writeVarName
                            if tempname not in varlist:
                                renamelist = "%s %s" % (renamelist, self.writeVarName[i])
                                droplist = "%s %s" % (droplist, tempname)
                                index += 1
                                break
                            index += 1
                            
                    renamelist = "%s =%s" % (renamelist, droplist)
                    initname = spssutil.CheckStr(self.name)
                    cmd = "DATASET NAME %s" % initname
                    PyInvokeSpss.Submit(cmd, len(cmd))
                    PyInvokeSpss.GetDataFromTempFile(self.senddatatempfile.name, newvarsCount, self.caseLenForWrite)
                    name = PyInvokeSpss.GetActive()[0]
                    cmd = "DATASET ACTIVATE %s." % initname
                    PyInvokeSpss.Submit(cmd, len(cmd))
                    self.submitAppendVariablesCmd(name, renamelist, droplist)
                    
        if "a" == self.accessType:
            if self.endAppendChanges and self.newCaseCount > 0:
                initname = spssutil.CheckStr(self.name)
                cmd = "DATASET NAME %s" % initname
                PyInvokeSpss.Submit(cmd, len(cmd))
                PyInvokeSpss.GetDataFromTempFile(self.senddatatempfile.name, 0, self.caseLen)
                name = PyInvokeSpss.GetActive()[0]
                cmd = "DATASET ACTIVATE %s." % initname
                PyInvokeSpss.Submit(cmd, len(cmd))
                self.submitAppendCasesCmd(name)
                self.newCaseCount = 0
        
        if "r" != self.accessType:
            os.unlink(self.senddatatempfile.name)

    def __isSorted(self, lst):
        return all( a < b for a, b in zip(lst[:-1], lst[1:]) )

__all__ = ["CursorBinary"]
import version
__version__ = version.version