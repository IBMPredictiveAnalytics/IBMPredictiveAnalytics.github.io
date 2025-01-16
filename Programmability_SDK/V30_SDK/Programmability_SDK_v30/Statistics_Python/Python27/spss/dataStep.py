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

#!/usr/bin/python

"""Data Step.

This module implements a Data Step for a IBM SPSS Statistics Data Source.
"""
#Copyright (C) 2008 by SPSS Inc.

import PyInvokeSpss,spss
from errMsg import *
from spss import error
import spssutil
import exceptions
import weakref
import time
import datetime

PythonDatasets = weakref.WeakValueDictionary()

class Dataset(object):
    """Provides methods to create new datasets, read from existing datasets, and modify existing datasets.

       --details
       An instance of the Dataset class can only be created within a data step, and is not
       available outside of the data step in which it was created. Data steps are initiated
       with the spss.StartDataStep function.

       --example
       import spss
       spss.StartDataStep()
       # Create a new dataset
       datasetObj = spss.Dataset(name=None)
       # Store the auto-generated name of the new dataset for use outside of the data step
       dshandle = datasetObj.name
       # Add variables and cases to the new dataset
       datasetObj.varlist.append('numvar')
       datasetObj.varlist.append('strvar',1)
       datasetObj.varlist['numvar'].label = 'Sample numeric variable'
       datasetObj.varlist['strvar'].label = 'Sample string variable'
       datasetObj.cases.append([1,'a'])
       datasetObj.cases.append([2,'b'])
       spss.EndDataStep()
    """

    def __init__(self, name="*", hidden=False, cvtDates=False):
        """Create a Dataset instance for a named open dataset or a new empty dataset.

           --arguments
           name: An optional argument that specifies the name of an open dataset for which a
                 Dataset object will be created. Specifying name = "*" or omitting the argument
                 will create a Dataset object for the active dataset. If None or the empty string
                 is specified then a new empty dataset will be created with an automatically
                 generated name. The name is available from the name property of the Dataset object.
           hidden: An optional argument that specifies if the new created dataset should be hidden
                 or not. True for hidden.
           cvtDates: A Boolean specifying whether SPSS Statistics variables with date of datetime formats are converted to Python datetime objects.
                 The default is False.
        """
        error.Reset()
        self.__isOpen = False
        self.__hidden = hidden
        self.__cvtDates = cvtDates

        if not spss.IsBackendReady():
            spss.StartSPSS()

        if not isinstance(name, basestring):
            if name is None:
                name = ""
            else:
                error.SetErrorCode(1057)
                raise SpssError,error

        name = spssutil.CheckStr16(name).rstrip()

        self.__isEmpty = False
        if name == "":
            self.__isEmpty = True
            name = PyInvokeSpss.GetNewDatasetName()[0]

        if self.__isEmpty == False:
            spssdatasets = PyInvokeSpss.GetSpssDatasets()
            dss = spssdatasets[0]
            errLevel = spssdatasets[-1]
            error.SetErrorCode(errLevel)
            if error.IsError():
                raise SpssError,error
            if name != "*"  and name not in dss:
                error.SetErrorCode(99)   ## Can't create specified dataset.
                if error.IsError():
                    raise SpssError,error
            ## If exist a Python dataset with same name, throw error
            handle = name.rstrip()
            if name == "*":
                temp = PyInvokeSpss.GetActive()
                if temp[1] == 0:
                    handle = temp[0]
            if handle != "*":
                for dds in PythonDatasets.values():
                    if dds.name == handle:
                        error.SetErrorCode(91)   ## A dataset with the specified name already exists.
                        raise SpssError,error
        inputname = spssutil.CheckStr(name) ## only for argument

        errLevel = PyInvokeSpss.CreateDataset(inputname, self.__isEmpty, self.__hidden)

        error.SetErrorCode(errLevel)
        if error.IsError():
            raise SpssError,error

        if name == "*":
            active = PyInvokeSpss.GetActive()
            name = active[0]
            errLevel = active[1]

            if errLevel != 93:
                error.SetErrorCode(errLevel)
                if error.IsError():
                    raise SpssError,error

        self.__name = name
        self.__isOpen = True
        self.__vars = VariableList(self)
        self.__rows = CaseList(self)
        self.__id = id(self)
        PythonDatasets[self.__id] = self
        self.__fileAttr = Dataset.FileAttribute(self)
        self.__mrset    = Dataset.MResponseSet(self)
        self.__optimized = True
        self.__cache = False

    def close(self):
        """Close the dataset.

           --details
           If the associated dataset is not the active dataset, that dataset is closed and no
           longer available in the session. If the associated dataset is the active dataset, the
           association with the dataset's name is broken. The active dataset remains active but has
           no name. When used, this method must be called prior to the spss.EndDataStep function.

           --example
           import spss
           spss.StartDataStep()
           datasetObj1 = spss.Dataset()
           datasetObj2 = datasetObj1.deepCopy(name="copy1")
           datasetObj1.close()
           datasetObj2.close()
           spss.EndDataStep()
        """
        error.Reset()
        if self.__isOpen:
            self.__rows.commitCase()
            self.cache = False
            errLevel = PyInvokeSpss.CloseDataset(spssutil.CheckStr(self.name))
            error.SetErrorCode(errLevel)
            if error.IsError():
                raise SpssError,error

            try:
                PythonDatasets.pop(self.__id)
                self.__id = 0
            except KeyError:
                pass
            except:
                pass

            self.__destroy()

    def __len__(self):
        """Return the number of variables in the dataset.

           --example
           import spss
           spss.StartDataStep()
           datasetObj = spss.Dataset()
           nvars = len(datasetObj)
           spss.EndDataStep()
        """
        error.Reset()
        self.__OnClosed()
        return len(self.__vars)

    def _getvarlist(self):
        """Return a VariableList instance representing the variables in the dataset.

           --example
           import spss
           spss.StartDataStep()
           datasetObj = spss.Dataset()
           # Get a VariableList instance for the active dataset
           varlistObj = datasetObj.varlist
           spss.EndDataStep()
        """
        error.Reset()
        self.__OnClosed()
        return self.__vars

    def _getcases(self):
        """Return a CaseList instance representing the cases (rows) in the dataset.

           --example
           import spss
           spss.StartDataStep()
           datasetObj = spss.Dataset()
           # Get a CaseList instance for the active dataset
           caselistObj = datasetObj.cases
           spss.EndDataStep()
        """
        error.Reset()
        self.__OnClosed()
        return self.__rows

    def _getname(self):
        """Return the name of the dataset.

          --example
          import spss
          spss.StartDataStep()
          # Create a new dataset and obtain the auto-generated name of the dataset
          datasetObj = spss.Dataset(name=None)
          dshandle = datasetObj.name
          spss.EndDataStep()
       """
        error.Reset()
        self.__OnClosed()
        return self.__name

    def deepCopy(self, name):
        """Create a deep copy of the Dataset instance and a copy of the associated dataset.

           --arguments
           name: The name of the new dataset as a quoted string. This cannot be the name of
                 the dataset being copied or a blank string. If name = "*" is specified, the copy
                 becomes the active dataset with a name that is automatically generated. The name
                 is available from the name property of the new Dataset object.

           --example
           import spss
           spss.StartDataStep()
           datasetObj1 = spss.Dataset()
           # Make a copy of the active dataset and assign it the name "copy1"
           datasetObj2 = datasetObj1.deepCopy(name="copy1")
           datasetObj2.close()
           spss.EndDataStep()
        """
        error.Reset()
        self.__OnClosed()

        name = spssutil.CheckStr16(name)

        name = name.rstrip()
        isactive = False
        if name is '*':
            name = PyInvokeSpss.GetNewDatasetName()[0]
            isactive = True

        errLevel = PyInvokeSpss.CopyDataset(spssutil.CheckStr(self.name), spssutil.CheckStr(name))

        error.SetErrorCode(errLevel)
        if error.IsError():
            raise SpssError,error

        x=Dataset(name, cvtDates=self.__cvtDates)

        if isactive:
            errLevel = PyInvokeSpss.SetActive(spssutil.CheckStr(name))
            error.SetErrorCode(errLevel)
            if error.IsError():
                raise SpssError,error

        return x

    class FileAttribute:

        """Get or set data file attributes. The FileAttribute class represent by the Python dict and is
        instantiated from the dataFileAttributes property of a Dataset object. The set of file attributes for a
        particular dataset is represented as a Python dictionary whose keys are the attribute names and whose values
        are the values of the attributes. Each value in the dictionary is either a simple value or a list of values
        (used to represent an attribute array).
        """

        def __init__(self, ds):
            """Create a FileAttribute instance for a specified Dataset instance.

               --arguments
               ds: An instance of the Dataset class.
            """
            error.Reset()
            self.__ds = ds
            self.data = {}
            self.__fillAll()

        def __getitem__(self, name):
            error.Reset()

            attname = spssutil.CheckStr(name).rstrip()

            result = PyInvokeSpss.GetDataFileAttributesInDS(spssutil.CheckStr(self.__ds.name), attname)
            out = result[0]
            errLevel = result[1]

            error.SetErrorCode(errLevel)
            if error.IsError():
                raise SpssError, error
            inname=spssutil.CheckStr16(name.rstrip())
            self.data[inname] = out
            return self.data[inname]

        def __setitem__(self, name, values):
            error.Reset()

            attname = spssutil.CheckStr(name).rstrip()

            if isinstance(values, basestring):
                attvalue = [spssutil.CheckStr(values).rstrip()]
            elif isinstance(values, (tuple, list)):
                values = list(values)
                for i in range(len(values)):
                    values[i] = spssutil.CheckStr(values[i]).rstrip()
                attvalue = values
            else:
                error.SetErrorCode(1062)
                raise SpssError, error

            errLevel = PyInvokeSpss.SetDataFileAttributesInDS(spssutil.CheckStr(self.__ds.name), attname, attvalue, len(attvalue))

            error.SetErrorCode(errLevel)
            if error.IsError():
                raise SpssError, error
            self.__fillAll()

        def __delitem__(self, name):
            error.Reset()
            name = spssutil.CheckStr(name)
            attname = name.rstrip()

            errLevel = PyInvokeSpss.DelDataFileAttributesInDS(spssutil.CheckStr(self.__ds.name), attname)

            error.SetErrorCode(errLevel)
            if error.IsError():
                raise SpssError, error
            self.__fillAll()

        def clear(self):
            error.Reset()

            for attname in self.data.keys():
                errLevel = PyInvokeSpss.DelDataFileAttributesInDS(spssutil.CheckStr(self.__ds.name), attname)

                error.SetErrorCode(errLevel)
                if error.IsError():
                    raise SpssError, error

            self.data.clear()

        def __repr__(self):
            return self.data.__repr__()

        def __len__(self):
            return len(self.data)

        def __eq__(self, other):
            if not isinstance(other, Dataset.FileAttribute):
                return False
            return self.data.__eq__(other.data)

        def __ne__(self, other):
            if not isinstance(other, Dataset.FileAttribute):
                return False
            return self.data.__ne__(other.data)

        def __fillAll(self):
            result = PyInvokeSpss.GetDataFileAttributeNamesInDS(spssutil.CheckStr(self.__ds.name))
            ret = result[0]
            errLevel = result[1]
            error.SetErrorCode(errLevel)
            if error.IsError():
                raise SpssError, error

            self.data.clear()
            if ret is not None:
                for i in ret:
                    self.__getitem__( i )


    def _getdatafileattributes(self):
        """Get, set or delete data file attributes. The set of file attributes for a particular dataset is
        represented as a Python dictionary whose keys are the attribute names and whose values are the values
        of the attributes. Each value in the dictionary is either a simple value or a list of values (used to
        represent an attribute array).

           --examples
           # Get all datafile attributes for a dataset
           import spss
           spss.StartDataStep()
           datasetObj = spss.Dataset()
           attr = datasetObj.dataFileAttributes
           spss.EndDataStep()

           # Get the value of a particular datafile attribute for a dataset
           import spss
           spss.StartDataStep()
           datasetObj = spss.Dataset()
           # Get the value of the attribute named 'Demo'
           attrvalue = datasetObj.dataFileAttributes['Demo']
           spss.EndDataStep()

           # Resetting datafile attributes
           import spss
           spss.StartDataStep()
           datasetObj = spss.Dataset()
           # Any existing attributes for the dataset are cleared and replaced with the specified set.
           datasetObj.dataFileAttributes = {'attr1':'value','attr2':['val1','val2']}
           spss.EndDataStep()

           # Adding or modifying a single datafile attribute
           import spss
           spss.StartDataStep()
           datasetObj = spss.Dataset()
           # If the attribute 'State' exists, its value is updated to 'Draft'. If the attribute
           # doesn't exist, it is created and added to the dataset.
           datasetObj.dataFileAttributes['State'] = 'Draft'
           spss.EndDataStep()

           # Deleting a specified datafile attribute for the dataset
           import spss
           spss.StartDataStep()
           datasetObj = spss.Dataset()
           del datasetObj.dataFileAttributes['State']
           spss.EndDataStep()

           # Deleting all datafile attributes for the dataset
           import spss
           spss.StartDataStep()
           datasetObj = spss.Dataset()
           del datasetObj.dataFileAttributes
           spss.EndDataStep()
        """
        error.Reset()
        self.__OnClosed()
        return self.__fileAttr

    def _setdatafileattributes(self, value):
        error.Reset()
        self.__OnClosed()

        if not isinstance(value, dict):
            raise TypeError("Expects a dict argument.")

        self.__fileAttr.clear()

        for i in value.items():
            self.__fileAttr.__setitem__(i[0], i[1])

    def _deldatafileattributes(self):
        error.Reset()
        self.__OnClosed()
        self.__fileAttr.clear()

    class MResponseSet:

        """Get or set multiple response sets. The MResponseSet class represent by the Python dict and is
        instantiated from the multiResponseSet property of a Dataset object. The set of multiple response sets
        for a particular dataset is represented as a Python dictionary whose keys are the set names and whose values
        are the values of the set. Each value in the dictionary is a list of values used to represent the label,
        the variable coding, the counted value (if any), and the elementary variables that define the set.
        """

        def __init__(self, ds):
            """Create a MResponseSet instance for a specified Dataset instance.

               --arguments
               ds: An instance of the Dataset class.
            """
            error.Reset()
            self.__ds = ds
            self.data = {}
            self.__fillAll()

        def __getitem__(self, mrsetName):
            error.Reset()

            if not isinstance(mrsetName, basestring):
                error.SetErrorCode(101)
                raise SpssError, error

            if not mrsetName.startswith('$'):
                mrsetName = '$'+mrsetName
            mrsetName = mrsetName.rstrip()

            name = spssutil.CheckStr(mrsetName)
##            temp = PyInvokeSpss.GetMultiResponseSetNamesInDS(spssutil.CheckStr(self.__ds.name))
##            if not name in temp:
##                error.SetErrorCode(106)
##                if error.IsError():
##                    raise SpssError, error


            result = PyInvokeSpss.GetMultiResponseSetInDS(spssutil.CheckStr(self.__ds.name), name)

            errLevel = result[-1]

            error.SetErrorCode(errLevel)
            if error.IsError():
                raise SpssError, error

            out = result[0:-1]
            mrsetName = mrsetName.upper()
            inname = spssutil.CheckStr16(mrsetName)
            self.data[inname] = out
            return self.data[inname]

        def __setitem__(self, mrsetName, values):
            error.Reset()

            if isinstance(values, (tuple, list)):
                if len(values)==4:
                    self.setMultiResponseSet(mrsetName,values[0],values[1],values[2],values[3])
                else:
                    error.SetErrorCode(1066)
                    raise SpssError, error
            else:
                error.SetErrorCode(1062)
                raise SpssError, error

        def __delitem__(self, name):
            error.Reset()
            name = spssutil.CheckStr(name)
            mrsetName = name.rstrip()

            if not mrsetName.startswith('$'):
                mrsetName = '$'+mrsetName
            mrsetName = mrsetName.upper()

            errLevel = PyInvokeSpss.DelMultiResponseSetInDS(spssutil.CheckStr(self.__ds.name), mrsetName)

            error.SetErrorCode(errLevel)
            if error.IsError():
                raise SpssError, error
            self.__fillAll()

        def clear(self):
            error.Reset()

            for mrsetName in self.data.keys():
                errLevel = PyInvokeSpss.DelMultiResponseSetInDS(spssutil.CheckStr(self.__ds.name), mrsetName)

                error.SetErrorCode(errLevel)
                if error.IsError():
                    raise SpssError, error

            self.data.clear()

        def setMultiResponseSet(self,mrsetName,mrsetLabel,mrsetCodeAs,mrsetCountedValue,varNames):
            error.Reset()

            mrsetName = spssutil.CheckStr(mrsetName.rstrip())

            if not mrsetName.startswith('$'):
                mrsetName = '$'+mrsetName
            mrsetName = mrsetName.upper()

            if mrsetLabel is None:
                mrsetLabel = ""
            mrsetLabel = spssutil.CheckStr(mrsetLabel.rstrip())

            code = mrsetCodeAs
            if isinstance(mrsetCodeAs, basestring):
                code = spssutil.CheckStr(mrsetCodeAs.strip().upper())
                if code == "CATEGORIES":
                    code = 1
                elif code == "DICHOTOMIES":
                    code = 2
                else:
                    error.SetErrorCode(101)
                    raise SpssError, error
            if not isinstance(code, (int, long)):
                error.SetErrorCode(1000)
                raise SpssError, error

            if mrsetCountedValue is None:
                mrsetCountedValue = ""
            mrsetCountedValue = spssutil.CheckStr(mrsetCountedValue.rstrip())

            if isinstance(varNames, basestring):
                elemVarNames = [spssutil.CheckStr(varNames.rstrip())]
            elif isinstance(varNames, (tuple, list)):
                varNames=list(varNames)
                for i in range(len(varNames)):
                    varNames[i] = spssutil.CheckStr(varNames[i])

                elemVarNames = varNames
            else:
                error.SetErrorCode(1062)
                raise SpssError, error

            errLevel = PyInvokeSpss.SetMultiResponseSetInDS(spssutil.CheckStr(self.__ds.name), mrsetName,
                                    mrsetLabel,code,mrsetCountedValue,
                                    elemVarNames,len(elemVarNames))

            error.SetErrorCode(errLevel)
            if error.IsError():
                raise SpssError, error

            self.__fillAll()

        def __repr__(self):
            return self.data.__repr__()

        def __len__(self):
            return len(self.data)

        def __eq__(self, other):
            if not isinstance(other, Dataset.MResponseSet):
                return False
            return self.data.__eq__(other.data)

        def __ne__(self, other):
            if not isinstance(other, Dataset.MResponseSet):
                return False
            return self.data.__ne__(other.data)

        def __fillAll(self):
            result = PyInvokeSpss.GetMultiResponseSetNamesInDS(spssutil.CheckStr(self.__ds.name))
            ret = result[0:-1]
            errLevel = result[-1]
            error.SetErrorCode(errLevel)
            if error.IsError():
                raise SpssError, error

            self.data.clear()

            if ret is not None and len(ret)!=0:
                for i in ret[0]:
                    self.__getitem__( i )


    def _getmultiresponseset(self):
        """Get, set or delete multiple response sets. The set of multiple response sets for a particular dataset
        is represented as a Python dictionary whose keys are the set names and whose values are the values of
        the set. Each value in the dictionary is a list of values used to represent the label, the variable coding,
        the counted value (if any), and the elementary variables that define the set.
        mrsetLabel: A string specifying a label for the set.
        mrsetCodeAs: An integer or a string specifying the variable coding--1 or Categories for multiple category sets,
                     2 or Dichotomies for multiple dichotomy sets. Case not sensitive.
        mrsetCountedValue: A string specifying the counted value. Only used when mrsetCodeAs is 2.
        varNames: A tuple or list of strings specifying the names of the elementary variables that define the set
                  (the list must include at least two variables).

           --examples
           # Get all multiple response sets for a dataset
           import spss
           spss.StartDataStep()
           datasetObj = spss.Dataset()
           mrsets = datasetObj.multiResponseSet
           spss.EndDataStep()

           # Get the value of a particular multiple response set for a dataset
           import spss
           spss.StartDataStep()
           datasetObj = spss.Dataset()
           # Get the value of the set named 'Demo'
           mrset = datasetObj.multiResponseSet['Demo']
           spss.EndDataStep()

           # Resetting multiple response sets
           import spss
           spss.StartDataStep()
           datasetObj = spss.Dataset()
           # Any existing sets for the dataset are cleared and replaced with the specified sets.
           datasetObj.multiResponseSet = {'$mltnews':["News Sources",2,"1",["Newspaper","TV","Web"]],
                                          '$mltent':["Entertainment Sources",2,"1",["TV","Movies","Theatre","Music"]]}
           spss.EndDataStep()

           # Adding or modifying a single multiple response set
           import spss
           spss.StartDataStep()
           datasetObj = spss.Dataset()
           # If the set named '$mltnews' exists, its value is updated with the specified value. If the set
           # doesn't exist, it is created and added to the dataset.
           datasetObj.multiResponseSet['$mltnews'] = ["News Sources",2,"1",["Newspaper","TV","Web"]]
           spss.EndDataStep()

           # Deleting a specified multiple response set from the dataset
           import spss
           spss.StartDataStep()
           datasetObj = spss.Dataset()
           del datasetObj.multiResponseSet['Demo']
           spss.EndDataStep()

           # Deleting all multiple response sets from the dataset
           import spss
           spss.StartDataStep()
           datasetObj = spss.Dataset()
           del datasetObj.multiResponseSet
           spss.EndDataStep()
        """
        error.Reset()
        self.__OnClosed()
        return self.__mrset

    def _setmultiresponseset(self, value):
        error.Reset()
        self.__OnClosed()

        if not isinstance(value, dict):
            raise TypeError("Expects a dict argument.")

        self.__mrset.clear()

        for i in value.items():
            self.__mrset.__setitem__(i[0], i[1])

    def _delmultiresponseset(self):
        error.Reset()
        self.__OnClosed()
        self.__mrset.clear()

    def __OnClosed(self):
        if not self.__isOpen:
            error.SetErrorCode(1065)
            if error.IsError():
                raise SpssError,error

    def __destroy(self):
        del self.__vars
        del self.__rows
        self.__name = None
        del self.__id
        del self.__isEmpty
        del self.__fileAttr
        del self.__mrset
        self.__isOpen = False

    def _getcvtDates(self):
        error.Reset()
        self.__OnClosed()
        return self.__cvtDates

    def _getoptimized(self):
        error.Reset()
        self.__OnClosed()
        return self.__optimized

    def _setoptimized(self, value):
        error.Reset()
        self.__OnClosed()
        self.__optimized = value

    def _setdsCache(self, value):
        error.Reset()
        self.__OnClosed()
        if isinstance(value, bool):
            if (value != self.__cache):
                errLevel = PyInvokeSpss.SetCacheInDS(self.__name, value)
                error.SetErrorCode(errLevel)
                if error.IsError():
                    raise SpssError,error

                self.__cache = value

    def _getdsCache(self):
        error.Reset()
        self.__OnClosed()
        return self.__cache

    def trendsInfo(self):
        error.Reset()
        self.__OnClosed()

        result = PyInvokeSpss.GetTrendsInfoInDS(spssutil.CheckStr(self.__name))
        out = result[0]
        errLevel = result[1]

        error.SetErrorCode(errLevel)
        if error.IsError():
            raise SpssError, error
        return out

    varlist = property(_getvarlist)
    cases   = property(_getcases)
    name    = property(_getname)
    dataFileAttributes = property(_getdatafileattributes,_setdatafileattributes,_deldatafileattributes)
    multiResponseSet   = property(_getmultiresponseset,_setmultiresponseset,_delmultiresponseset)
    cvtDates = property(_getcvtDates)
    optimized = property(_getoptimized,_setoptimized)
    cache = property(_getdsCache,_setdsCache)

class VariableList(object):

    """Provides access to the properties of the variables in a dataset, allowing you to get and
    set properties of existing variables, and add new variables to the dataset. An instance of the
    VariableList class can be created from the varlist property of a Dataset object.

       --example
       import spss
       spss.StartDataStep()
       datasetObj = spss.Dataset()
       # Get a VariableList instance for the active dataset
       varlistObj = datasetObj.varlist
       spss.EndDataStep()
    """

    def __init__(self, ds):
        """Create a VariableList instance for a specified Dataset instance.

           --arguments
           ds: An instance of the Dataset class.
        """
        if not isinstance(ds, Dataset):
            raise TypeError("Expects a Dataset argument")
        self.__ds = ds
        self.__vars = []
        self.__vdic = {}

        varcount = PyInvokeSpss.GetVarCountInDS(spssutil.CheckStr(self.__ds.name))[0]
        self.__varcount = varcount

        if not self.__ds._Dataset__isEmpty:
            for varindex in xrange(varcount):
                var = Variable( self.__ds, varindex)
                self.__vars.insert(varindex, var)
                self.__vdic[var.name] = var


    def __len__(self):
        """Return the number of variables in the associated dataset.

           --example
           import spss
           spss.StartDataStep()
           datasetObj = spss.Dataset()
           varlistObj = datasetObj.varlist
           nvars = len(varlistObj)
           spss.EndDataStep()
        """
        error.Reset()
        return self.__varcount

    def __getitem__(self, indexOrName):
        """Return an instance of the Variable class for a specified variable.

           --arguments
           indexOrName: The name or index of the variable. Index values represent position in
                        the active dataset, starting with 0 for the first variable in file order.

           --examples
           # Get a Variable object by variable name
           import spss
           spss.Submit("GET FILE='Employee data.sav'.")
           spss.StartDataStep()
           datasetObj = spss.Dataset()
           # Create a Variable object for the variable named 'salary'
           varObj = datasetObj.varlist['salary']
           spss.EndDataStep()

           # Get a Variable object by variable index
           import spss
           spss.Submit("GET FILE='Employee data.sav'.")
           spss.StartDataStep()
           datasetObj = spss.Dataset()
           # Create a Variable object for the variable with index value 5
           varObj = datasetObj.varlist[5]
           spss.EndDataStep()
        """
        if isinstance(indexOrName, (int, long)):
            return self.__vars[indexOrName]
        elif isinstance(indexOrName, basestring):
            indexOrName = spssutil.CheckStr16(indexOrName)
            return self.__vdic[indexOrName]
        else:
            error.SetErrorCode(1058)
            raise SpssError,error

    def __delitem__(self, indexOrName):
        """Delete an instance of the Variable class for a specified variable.

           --arguments
           indexOrName: The name or index of the variable. Index values represent position in
                        the active dataset, starting with 0 for the first variable in file order.

           --details
           Deleting an instance of the Variable class removes the variable from the associated dataset.

           --examples
           # Delete a Variable object by variable name
           import spss
           spss.Submit("GET FILE='Employee data.sav'.")
           spss.StartDataStep()
           datasetObj = spss.Dataset()
           # Delete the Variable object for the variable named 'salary'
           del datasetObj.varlist['salary']
           spss.EndDataStep()

           # Delete a Variable object by variable index
           import spss
           spss.Submit("GET FILE='Employee data.sav'.")
           spss.StartDataStep()
           datasetObj = spss.Dataset()
           # Delete the Variable object for the variable with index value 5
           del datasetObj.varlist[5]
           spss.EndDataStep()
        """
        error.Reset()

        if self.__ds.cache:
            error.SetErrorCode(1074)
            raise SpssError,error

        if isinstance(indexOrName, (int, long)):
            delname = self.__vars[indexOrName].name
            delindex = indexOrName

        elif isinstance(indexOrName, basestring):
            indexOrName = spssutil.CheckStr16(indexOrName)
            delindex = self.__vdic[indexOrName].index
            delname = indexOrName

        else:
            error.SetErrorCode(1058)
            raise SpssError,error

        errLevel = PyInvokeSpss.DeleteVariable(
                spssutil.CheckStr(self.__ds.name), delindex)

        error.SetErrorCode(errLevel)
        if error.IsError():
            raise SpssError,error

        self.__varcount -= 1

        del self.__vdic[delname]
        del self.__vars[delindex]

        for var in self.__vars[delindex:]:
            var.index -= 1

        self.__ds.cases.updateVarType(2, delindex)

    def append(self, name, type = 0):
        """Append a new variable to the associated dataset and append a corresponding Variable
        object to the associated VariableList instance.

           --arguments
           name: The variable name.
           type: An optional argument specifies the variable type--0 for numeric and an integer equal to
                 the defined length for a string (maximum of 32767).

           --example
           import spss
           spss.StartDataStep()
           # Create a new dataset
           datasetObj = spss.Dataset(name=None)
           # Add variables to the dataset
           datasetObj.varlist.append('numvar')
           datasetObj.varlist.append('strvar',1)
           datasetObj.varlist['numvar'].label = 'Sample numeric variable'
           datasetObj.varlist['strvar'].label = 'Sample string variable'
           spss.EndDataStep()
        """
        self.insert(name, type)

    def insert(self, name, type = 0, index=None):
        """Insert a new variable into the associated dataset and insert a corresponding Variable
        object into the associated VariableList instance.

           --arguments
           name: The variable name.
           type: An optional argument specifies the variable type--0 for numeric and an integer equal to
                 the defined length for a string (maximum of 32767).
           index: An optional argument specifying the position for the inserted variable and Variable
                  object (the first position has an index value of 0). Can take on the values 0,1,...,n
                  where n is the number of variables in the dataset. If the argument is omitted or equal
                  to n, the variable is appended to the end of the list.

           --example
           import spss
           spss.StartDataStep()
           # Create a new dataset
           datasetObj = spss.Dataset(name=None)
           # Add variables to the dataset
           datasetObj.varlist.append('numvar')
           datasetObj.varlist.append('strvar1',1)
           datasetObj.varlist.insert('strvar2',8,2)
           datasetObj.varlist['numvar'].label = 'Sample numeric variable'
           datasetObj.varlist['strvar1'].label = 'Sample string variable 1'
           datasetObj.varlist['strvar2'].label = 'Sample string variable 2'
           spss.EndDataStep()
        """
        error.Reset()

        if self.__ds.cache:
            error.SetErrorCode(1074)
            raise SpssError,error

        if not isinstance(type, (int,long)):
            error.SetErrorCode(1000)
            raise SpssError,error
        vartype = type

        varname = spssutil.CheckStr(name).rstrip()

        if index is None:
            varindex = len(self)
        else:
            if not isinstance(index, (int, long)):
                error.SetErrorCode(1000)
                raise SpssError,error
            if index < 0:
                error.SetErrorCode(10)
                raise SpssError,error
            varindex = index

        errLevel = PyInvokeSpss.InsertVariable(spssutil.CheckStr(self.__ds.name), varindex, varname, vartype)

        error.SetErrorCode(errLevel)
        if error.IsError():
            raise SpssError,error

        self.__varcount += 1

        var = Variable( self.__ds, varindex )
        self.__vars.insert(varindex, var)
        self.__vdic[var.name] = var

        for var in self.__vars[varindex + 1:]:
            var.index += 1

        self.__ds.cases.updateVarType(1, varindex, vartype)

    def move(self, fromIdxList, targetIdx):
        """move variables

           --arguments
           fromIdxList: The variable list or tuple which need to be moved
           targetIdx: The target variable index

           --example
           import spss
           spss.Submit("GET FILE='demo.sav'.")
           spss.StartDataStep()
           # Create a new dataset
           datasetObj = spss.Dataset()
           # Add variables to the dataset
           datasetObj.varlist.move((1, 2), 3)
           spss.EndDataStep()
        """
        error.Reset()

        if self.__ds.cache:
            error.SetErrorCode(1074)
            raise SpssError,error

        if not isinstance(targetIdx, (int, long)):
            error.SetErrorCode(1000)
            raise SpssError,error
        if targetIdx < 0:
            error.SetErrorCode(10)
            raise SpssError,error

        if not isinstance(fromIdxList, (list, tuple)):
            error.SetErrorCode(1002)
            raise SpssError,error
        else:
            tuple(fromIdxList)

        errLevel = PyInvokeSpss.MoveVariables(spssutil.CheckStr(self.__ds.name), fromIdxList, targetIdx)

        error.SetErrorCode(errLevel)
        if error.IsError():
            raise SpssError,error

    def __iter__(self):
        """Iterate over the variables.

           --example
           import spss
           spss.StartDataStep()
           datasetObj = spss.Dataset()
           # Iterate through the variables in the active dataset
           for varObj in datasetObj.varlist:
              print varObj.name
           spss.EndDataStep()
        """
        for i in range(len(self)):
            yield self.__vars[i]

class CaseList(object):

    """Provides access to the cases in a dataset, allowing you to read existing cases,
    modify case values, and add new cases. An instance of the CaseList class can be created
    from the cases property of a Dataset object.

       --example
       import spss
       spss.StartDataStep()
       datasetObj = spss.Dataset()
       # Get a CaseList instance for the active dataset
       caselistObj = datasetObj.cases
       spss.EndDataStep()
    """

    def __init__(self, ds):
        """Create a CaseList instance for a specified Dataset instance.

           --arguments
           ds: An instance of the Dataset class.
        """
        self.__ds = ds
        self.__getRowNum = -1
        self.__getRowData = []
        self.__vartype = []
        self.__setRowNum = -1
        self.__setRowData = {}
        curDateVarIndex = []

        for column in range(len(self.__ds)):
            vartype = self.__ds.varlist[column].type
            self.__vartype.append(vartype)
            vfmt = self.__ds.varlist[column].format
            if spssutil.isDateVariable(vfmt):
                curDateVarIndex.append(column)

        casecout = PyInvokeSpss.GetCaseCountInDS(spssutil.CheckStr(self.__ds.name))[0]

        self.__dateVarIndex = set(curDateVarIndex)
        self.__casecount = casecout

    def __len__(self):
        """Return the number of cases in the associated dataset.

           --example
           import spss
           spss.StartDataStep()
           datasetObj = spss.Dataset()
           caselistObj = datasetObj.cases
           ncases = len(caselistObj)
           spss.EndDataStep()
        """
        error.Reset()
        return self.__casecount

    def __getitem__(self, indexOrCoordinate):
        """Return case values from the associated dataset. You can retrieve one or more complete cases
        or a subset of variable values from one or more cases.

           --arguments
           indexOrCoordinate: A 0-based integer specifying a case number or a tuple or list with
                              two 0-based integers specifying a case number and variable index.
                              Variable indexes represent position in the dataset starting with 0 for
                              the first variable in file order. Slices and negative index values are
                              supported.

           --return value
           When a single case number is specified, the return value is a list whose elements are the case
           values of the variables, in file order. Values for multiple cases are returned as a list of
           elements, each of which is a list of values for a single case.

           System-missing values are returned as None. Values of variables with date or datetime formats
           are returned as integers representing the number of seconds from October 14, 1582. Values of
           variables with time formats are returned as integers representing the number of seconds from
           midnight.

           --examples
           # Get a single case
           import spss
           spss.Submit("GET FILE='demo.sav'.")
           spss.StartDataStep()
           datasetObj = spss.Dataset()
           # Get the first case
           row = datasetObj.cases[0]
           spss.EndDataStep()

           # Get a single value within a case
           import spss
           spss.Submit("GET FILE='demo.sav'.")
           spss.StartDataStep()
           datasetObj = spss.Dataset()
           # Get the value of the variable with index 1 for the first case
           value = datasetObj.cases[0,1]
           spss.EndDataStep()
        """
        error.Reset()

        def __getbyindex(indexOrCoordinate, part=None):
            if isinstance(indexOrCoordinate, (int, long)):
                index = self.__validRowindex(indexOrCoordinate)
                return self.__getrow(index, part)
            elif isinstance(indexOrCoordinate, slice):
                indices = self.__validRowindices(indexOrCoordinate)
                out = []
                for i in indices:
                    out.append(self.__getrow(i, part))
                return out
            else:
                return None


        out = __getbyindex(indexOrCoordinate)
        if out is not None:
            return out
        elif isinstance(indexOrCoordinate, (tuple, list)):
            if len(indexOrCoordinate) != 2:
                error.SetErrorCode(1059)
                raise SpssError,error

            Columnslice = False
            if isinstance(indexOrCoordinate[1],(int,long)):
                part = [self.__validColumnindex(indexOrCoordinate[1])]
            elif isinstance(indexOrCoordinate[1],slice):
                Columnslice = True
                part = self.__validColumnindices(indexOrCoordinate[1])
            else:
                error.SetErrorCode(1059)
                raise SpssError,error

            if isinstance(indexOrCoordinate[0], slice):
                indices = self.__validRowindices(indexOrCoordinate[0])
                out = []
                for i in indices:
                    self.__getRowData = __getbyindex(i)
                    self.__getRowNum = i
                    if Columnslice:
                        out.append(self.__getRowData[indexOrCoordinate[1]])
                    else:
                        out.append([self.__getRowData[indexOrCoordinate[1]]])
            else:
                if (self.__getRowNum==indexOrCoordinate[0]) and (-1!=self.__getRowNum):
                    pass
                else:
                    self.__getRowData = __getbyindex(indexOrCoordinate[0])
                if Columnslice:
                    out = self.__getRowData[indexOrCoordinate[1]]
                else:
                    out = [self.__getRowData[indexOrCoordinate[1]]]

            if out is None:
                error.SetErrorCode(1059)
                raise SpssError,error
            else:
                return out

    def __setitem__ (self, indexOrCoordinate, value):
        """Set case values for existing cases in the associated dataset. You can modify the values
        for a specific case or a range of cases, and you can set the value of a particular variable
        or a range of variables within those cases.

           --arguments
           indexOrCoordinate: A 0-based integer specifying a case number or a tuple or list with
                              two 0-based integers specifying a case number and variable index.
                              Variable indexes represent position in the dataset starting with 0 for
                              the first variable in file order. Slices and negative index values are
                              supported.
           value: For a single case, values are specified as a list or tuple. The first element
                  corresponds to the first variable in file order, the second element corresponds to the
                  second variable in file order, and so on. For multiple cases, values are specified as a
                  list or tuple of elements, each of which is a list or tuple of values for a single case.

           --details
           Values of None are converted to system-missing for numeric variables and blanks for string
           variables. Values of numeric variables with a date or datetime format should be specified
           as the number of seconds from October 14, 1582.

           --examples
           # Set the values for a single case
           import spss
           spss.Submit("GET FILE='demo.sav'.")
           spss.StartDataStep()
           datasetObj = spss.Dataset()
           # Set the values for case number 1
           datasetObj.cases[1] = [35,150,100,2110,19,2006,3,4,0]
           spss.EndDataStep()

           # Set a variable value within a case
           import spss
           spss.Submit("GET FILE='demo.sav'.")
           spss.StartDataStep()
           datasetObj = spss.Dataset()
           # Set the value of the variable with index value 0 for case number 1
           datasetObj.cases[1,0] = 14
           spss.EndDataStep()
        """
        error.Reset()

        def __setbyindex(indexOrCoordinate, value, part=None):
            ret = True

            if isinstance(indexOrCoordinate, (int, long)):
                index = self.__validRowindex(indexOrCoordinate)

                if isinstance(value, (tuple, list)):
                    for column in xrange(len(value)):
                        if column in self.__dateVarIndex:
                            value[column] = spssutil.CvtPythonDatetime(value[column])
                        value[column] = self.__transformdata(value[column])
                else:
                    if part[0] in self.__dateVarIndex:
                        value = spssutil.CvtPythonDatetime(value)
                        value = [value]
                    else:
                        value = self.__transformdata(value)
                        value = [value]

                if not isinstance(value, (tuple, list)):
                    error.SetErrorCode(1027)
                    raise SpssError,error
                self.__setrow(index, value, part)
            elif isinstance(indexOrCoordinate, slice):
                indices = self.__validRowindices(indexOrCoordinate)
                if not isinstance(value, (tuple, list)):
                    error.SetErrorCode(1027)
                    raise SpssError,error
                if len(indices) != len(value):
                    error.SetErrorCode(1060)
                    raise SpssError,error

                j=0
                for i in indices:
                    if isinstance(value[j], (int, long, float)):
                        value[j] = [value[j]]

                    if isinstance(value[j],basestring):
                        value[j] = spssutil.CheckStr(value[j])

                    if not isinstance(value[j], (tuple, list)):
                        error.SetErrorCode(1027)
                        raise SpssError,error
                    self.__setrow(i, value[j], part)
                    j += 1
            else:
                ret = False
            return ret

        if not __setbyindex(indexOrCoordinate, value):
            if len(indexOrCoordinate) != 2:
                error.SetErrorCode(1059)
                raise SpssError,error

            if isinstance(indexOrCoordinate[1],(int,long)):
                part = [self.__validColumnindex(indexOrCoordinate[1])]
            elif isinstance(indexOrCoordinate[1],slice):
                part = self.__validColumnindices(indexOrCoordinate[1])
            else:
                error.SetErrorCode(1059)
                raise SpssError,error

            if not __setbyindex(indexOrCoordinate[0], value, part):
                error.SetErrorCode(1059)
                raise SpssError,error

    def __delitem__(self, index):
        """Delete a specified case from the CaseList object.

           --arguments
           index: The case number. Case numbers start from 0.

           --details
           Deleting a case from a CaseList object also deletes the case from the associated dataset.

           --example
           import spss
           spss.StartDataStep()
           datasetObj = spss.Dataset()
           # Delete the first case
           del datasetObj.cases[0]
           spss.EndDataStep()
        """
        error.Reset()

        if not isinstance(index, (int, long)):
            error.SetErrorCode(1000)
            raise SpssError, error
        caseindex = self.__validRowindex(index)

        errLevel = PyInvokeSpss.DeleteCase(spssutil.CheckStr(self.__ds.name), caseindex)
        error.SetErrorCode(errLevel)
        if error.IsError():
            raise SpssError,error

        self.__casecount -= 1

    def append(self, case=None):
        """Append a case (row) to the CaseList instance and append a corresponding case to the
        associated dataset.

           --arguments
           case: A tuple or list specifying the case values. Values of None are converted to
                 system-missing for numeric variables and blanks for string variables. Values of
                 numeric variables with a date or datetime format should be specified as the number
                 of seconds from October 14, 1582.

           --example
           import spss
           spss.StartDataStep()
           # Create a new dataset
           datasetObj = spss.Dataset(name=None)
           # Add variables
           datasetObj.varlist.append('numvar')
           datasetObj.varlist.append('strvar',1)
           # Append cases
           datasetObj.cases.append([1,'a'])
           datasetObj.cases.append([2,'b'])
           spss.EndDataStep()
        """
        self.insert(case )

    def insert(self, case=None, caseNumber=None):
        """Insert a case (row) into the CaseList instance and insert a corresponding case into the
        associated dataset.

           --arguments
           case: A tuple or list specifying the case values. Values of None are converted to
                 system-missing for numeric variables and blanks for string variables. Values of
                 numeric variables with a date or datetime format should be specified as the number
                 of seconds from October 14, 1582.
           caseNumber: An optional argument specifying the position of the inserted case (case numbers
                       start from 0). Can take on the values 0,1,...,n where n is the number of cases
                       in the dataset. If the argument is omitted or equal to n, the case is appended.

           --example
           import spss
           spss.StartDataStep()
           # Create a new dataset
           datasetObj = spss.Dataset(name=None)
           # Add variables
           datasetObj.varlist.append('numvar')
           datasetObj.varlist.append('strvar',1)
           # Add cases
           datasetObj.cases.append([1,'a'])
           datasetObj.cases.append([3,'c'])
           datasetObj.cases.insert([2,'b'],1)
           spss.EndDataStep()
        """
        error.Reset()

        if self.__ds.cache:
            error.SetErrorCode(1074)
            raise SpssError,error

        if caseNumber is None:
            caseindex = len(self)
        else:
            if not isinstance(caseNumber, (int, long)):
                error.SetErrorCode(1000)
                raise SpssError,error
            if caseNumber < 0 or caseNumber > len(self):
                error.SetErrorCode(10)
                raise SpssError,error
            caseindex = caseNumber

        if case:
            if not isinstance(case, (tuple, list)):
                error.SetErrorCode(1027)
                raise SpssError,error

        errLevel = PyInvokeSpss.InsertCase(spssutil.CheckStr(self.__ds.name), caseindex)
        error.SetErrorCode(errLevel)
        if error.IsError():
            raise SpssError,error

        self.__casecount += 1

        if case:
            for i in xrange(len(case)):
                if i in self.__dateVarIndex:
                    case[i] = spssutil.CvtPythonDatetime(case[i])
            self.__setrow(caseindex, case)

    def __iter__(self):
        """Iterate over the cases.

           --example
           import spss
           spss.StartDataStep()
           datasetObj = spss.Dataset()
           # Iterate through the cases in the active dataset
           for row in datasetObj.cases:
              print row
           spss.EndDataStep()
        """
        for i in range(len(self)):
            yield self.__getitem__(i)

    def commitCase(self):
        if len(self.__setRowData):
            PyInvokeSpss.SetCasePartValue(spssutil.CheckStr(self.__ds.name), self.__setRowNum, self.__setRowData, self.__ds.cache)
            self.__setRowData.clear()

    def updateDateVarIndex(self, index):
        self.__dateVarIndex.add(index)

    def updateVarType(self, mode, index, type=0):
        if 0 == mode:
            self.__vartype[index] = type
        elif 1 == mode:
            self.__vartype.insert(index, type)
        elif 2 == mode:
            self.__vartype.pop(index)

    def __validRowindex(self, index):
        caseSize = self.__casecount
        out = index
        if index < 0:
            out = len(self) + index
        if out < 0 or out >= len(self):
            error.SetErrorCode(97)
            raise SpssError,error
        return out

    def __validRowindices(self, indices):

        start = self.__validRowindex(indices.start or 0)

        if indices.step is None:
            step = 1
        elif indices.step <= 0 or indices.step > len(self):
            step = 1
        else:
            step = indices.step

        stop  = indices.stop

        if stop < 0:
            stop = len(self) + stop
        if stop < 0 or stop > len(self):
            error.SetErrorCode(10)
            raise SpssError,error

        return range(start, stop, step)

    def __validColumnindex(self, index):
        columnSize = len(self.__ds)
        out = index
        if index < 0:
            out = columnSize + index
        if out < 0 or out >= columnSize:
            error.SetErrorCode(10)
            raise SpssError,error
        return out

    def __validColumnindices(self, indices):

        start = self.__validColumnindex(indices.start or 0)

        if indices.step is None:
            step = 1
        elif indices.step <= 0 or indices.step > len(self.__ds):
            step = 1
        else:
            step = indices.step

        stop  = indices.stop

        if stop < 0:
            stop = len(self.__ds) + stop
        if stop < 0 or stop > len(self.__ds):
            error.SetErrorCode(10)
            raise SpssError,error

        return range(start, stop, step)

    def __getrow(self, row, part = None):
        """
        part is a list
        """
        if row < 0 or row > len(self):
            error.SetErrorCode(10)
            raise SpssError,error

        if row == self.__setRowNum:
            self.commitCase()
            self.__getRowNum = -1

        if row == self.__getRowNum:
            return self.__getRowData

        r = [range(len(self.__ds))][0]
        r1 = ( part==None and [r] or [part])[0]

        result = PyInvokeSpss.GetCaseValue(spssutil.CheckStr(self.__ds.name), row, self.__ds.cache)
        out = result[0]
        errLevel = result[1]

        if part is not None:
            [out[i] for i in r1]

        error.SetErrorCode(errLevel)
        if error.IsError():
            raise SpssError, error

        if self.__ds.cvtDates:
            for i in range(len(out)):
                vfmt = self.__ds.varlist[i].format
                if spssutil.isDateVariable(vfmt):
                    out[i] = spssutil.CvtSpssDatetime(out[i])

        self.__getRowNum = row
        self.__getRowData = out
        return out

    def __setrow(self, row, values, part=None):
        """
        values and part are list
        """
        if row < 0 or row > self.__casecount:
            error.SetErrorCode(97)
            raise SpssError,error

        if row != self.__setRowNum and (-1 != self.__setRowNum):
            self.commitCase()

        self.__setRowNum = row
        if len(values) > len(self.__vartype):
            indexLen = len(self.__vartype)
        else:
            indexLen = len(values)
        r = ( part==None and [xrange(indexLen)] or [part])[0]
        for i in xrange(indexLen):
            if self.__vartype[r[i]] == 0:
                if not isinstance(values[i], (int, long, float)):
                    if values[i] is None:
                        pass
                    else:
                        error.SetErrorCode(57)
                        raise SpssError,error
            else:
                if not isinstance(values[i], (basestring, unicode)):
                    if values[i] is None:
                        values[i] = ""
                    else:
                        error.SetErrorCode(56)
                        raise SpssError,error
            self.__setRowData[r[i]] = values[i]

    def __transformdata(self, value):
        if isinstance(value, (int, long)):
            value = float(value)

        if isinstance(value,basestring):
            value = spssutil.CheckStr(value)

        return value



class Variable(object):
    """Provides methods to get and set the following properties of IBM SPSS Statistics variables:

       type (numeric or string)
       name
       label
       format
       alignment
       column width
       measurement level
       variable attributes
       value labels
       user-missing values
       role

    Instances of the Variable class for each variable in the associated dataset are generated when the
    VariableList class is instantiated. In addition, the append and insert methods of a VariableList object
    create associated instances of the Variable class for appended and inserted variables.

    Specific variables are accessed from the VariableList instance associated with a Dataset object and
    can be accessed by name or index (index values represent position in the dataset starting with 0 for
    the first variable in file order).

       --examples
       # Get a Variable object by variable name
       import spss
       spss.Submit("GET FILE='Employee data.sav'.")
       spss.StartDataStep()
       datasetObj = spss.Dataset()
       # Create a Variable object for the variable named 'salary'
       varObj = datasetObj.varlist['salary']
       spss.EndDataStep()

       # Get a Variable object by variable index
       import spss
       spss.Submit("GET FILE='Employee data.sav'.")
       spss.StartDataStep()
       datasetObj = spss.Dataset()
       # Create a Variable object for the variable with index value 5
       varObj = datasetObj.varlist[5]
       spss.EndDataStep()
    """

    def __init__(self, ds, index ):
        """Create a Variable instance with specified index and type for the specified Dataset instance.

           --arguments
           ds: An instance of the Dataset class.
           index: The variable index. Index values represent position in the dataset starting with 0 for
                  the first variable in file order.
        """
        self.__ds = ds
        self.index = index
        self.__attr = Variable.Attribute(self)
        self.__vallbl = Variable.ValueLabel(self)

    def _gettype(self):
        """Get or set the variable type.

           --details
           Numeric variables have a type of 0 and string variables have an integer type equal to the defined
           length up to a maximum of 32767.

           --examples
           # Get the variable type
           import spss
           spss.StartDataStep()
           datasetObj = spss.Dataset()
           varObj = datasetObj.varlist['strvar']
           vartype = varObj.type
           spss.EndDataStep()

           # Set the variable type
           import spss
           spss.StartDataStep()
           datasetObj = spss.Dataset()
           varObj = datasetObj.varlist['strvar']
           varObj.type = 10
           spss.EndDataStep()
        """
        error.Reset()

        result = PyInvokeSpss.GetVarTypeInDS(spssutil.CheckStr(self.__ds.name), self.index)
        out = result[0]
        errLevel = result[1]

        error.SetErrorCode(errLevel)
        if error.IsError():
            raise SpssError, error
        return out

    def _settype(self, value):
        error.Reset()

        if self.__ds.cache:
            error.SetErrorCode(1074)
            raise SpssError,error

        if not isinstance(value, (int, long)):
            error.SetErrorCode(1000)
            raise SpssError, error

        if value < 0:
            error.SetErrorCode(26)
            raise SpssError, error

        errLevel = PyInvokeSpss.SetVarTypeInDS(spssutil.CheckStr(self.__ds.name), self.index, value)

        error.SetErrorCode(errLevel)
        if error.IsError():
            raise SpssError, error

        self.__ds.cases.updateVarType(0, self.index, value)

    def _getname(self):
        """Get or set the variable name.

          --examples
          # Get the variable name
          import spss
          spss.StartDataStep()
          datasetObj = spss.Dataset()
          varObj = datasetObj.varlist[0]
          varname = varObj.name
          spss.EndDataStep()

          # Set the variable name
          import spss
          spss.StartDataStep()
          datasetObj = spss.Dataset()
          varObj = datasetObj.varlist[0]
          varObj.name = 'newname'
          spss.EndDataStep()
        """
        error.Reset()

        result = PyInvokeSpss.GetVarNameInDS(spssutil.CheckStr(self.__ds.name), self.index)
        out = result[0]
        errLevel = result[1]

        error.SetErrorCode(errLevel)
        if error.IsError():
            raise SpssError, error
        return out

    def _setname(self, value):
        error.Reset()

        value = spssutil.CheckStr(value)

        oldname = self._getname()
        errLevel = PyInvokeSpss.SetVarNameInDS(spssutil.CheckStr(self.__ds.name), self.index, value.rstrip())

        error.SetErrorCode(errLevel)
        if error.IsError():
            raise SpssError, error
        ## update VariableList vdic
        dic = self.__ds.varlist._VariableList__vdic
        del dic[oldname]
        dic[self._getname()] = self.__ds.varlist[self.index]

    def _getlabel(self):
        """Get or set the variable label.

           --examples
           # Get the variable label
           import spss
           spss.StartDataStep()
           datasetObj = spss.Dataset()
           varObj = datasetObj.varlist['bdate']
           varlabel = varObj.label
           spss.EndDataStep()

           # Set the variable label
           import spss
           spss.StartDataStep()
           datasetObj = spss.Dataset()
           varObj = datasetObj.varlist['bdate']
           varObj.label = 'Birth Date'
           spss.EndDataStep()
        """
        error.Reset()

        result = PyInvokeSpss.GetVarLabelInDS(spssutil.CheckStr(self.__ds.name), self.index)
        out = result[0]
        errLevel = result[1]

        error.SetErrorCode(errLevel)
        if error.IsError():
            raise SpssError, error
        return out

    def _setlabel(self, value):
        error.Reset()
        value = spssutil.CheckStr(value)

        errLevel = PyInvokeSpss.SetVarLabelInDS(spssutil.CheckStr(self.__ds.name), self.index, value.rstrip())

        error.SetErrorCode(errLevel)
        if error.IsError():
            raise SpssError, error

    def _getformat(self):
        """Get or set the format of the variable.

           --returned value from get usage
           A string representation of the format, e.g. F8.2, consisting of a character portion (upper case)
           that specifies the format type, followed by a numeric component that indicates the defined width,
           and an optional component that specifies the number of decimal positions for numeric formats.
           For example, A4 is a string format with a maximum width of four bytes, and F8.2 is a standard
           numeric format with a display format of eight digits, including two decimal positions and a
           decimal indicator.

           --value for set usage
           A tuple or list of three integers, indicating the format type, width, and the number of
           decimal digits (for numeric formats) in that order. The width must include enough positions to
           accommodate any punctuation characters such as decimal points, commas, dollar signs, or date and
           time delimiters. For a list of the minimum and maximum widths and maximum decimal places for
           commonly-used format types, see Variable Types and Formats in the Universals section of the
           Command Syntax Reference. The available format types and associated integer codes are:

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

           --examples
           # Get the variable format
           import spss
           spss.StartDataStep()
           datasetObj = spss.Dataset()
           varObj = datasetObj.varlist['id']
           varformat = varObj.format
           spss.EndDataStep()

           # Set the variable format
           import spss
           spss.StartDataStep()
           datasetObj = spss.Dataset()
           varObj = datasetObj.varlist['id']
           varObj.format = [5,5,0]
           spss.EndDataStep()
        """
        error.Reset()

        result = PyInvokeSpss.GetVarFormatInDS(spssutil.CheckStr(self.__ds.name), self.index)
        out = result[0]
        errLevel = result[1]

        error.SetErrorCode(errLevel)
        if error.IsError():
            raise SpssError, error
        return out

    def _setformat(self, value):
        error.Reset()

        if not isinstance(value, (tuple,list)):
            error.SetErrorCode(1027)
            raise SpssError, error

        if len(value) != 3:
            error.SetErrorCode(1061)
            raise SpssError, error
        for i in list(value):
            if not isinstance(i, (int, long)):
                error.SetErrorCode(1000)
                raise SpssError, error

        errLevel = PyInvokeSpss.SetVarFormatInDS(spssutil.CheckStr(self.__ds.name), self.index, value[0], value[1], value[2])

        error.SetErrorCode(errLevel)
        if error.IsError():
            raise SpssError, error

        if spssutil.isDateVariable(value[0]):
            self.__ds.cases.updateDateVarIndex(self.index)

    def _getalignment(self):
        """Get or set the alignment of the variable (the alignment of data values displayed in the Data Editor).
        The variable alignment is an integer with one of the following values: 0 (left), 1 (right), 2 (center).

           --examples
           # Get the alignment
           import spss
           spss.StartDataStep()
           datasetObj = spss.Dataset()
           varObj = datasetObj.varlist['gender']
           varalign = varObj.alignment
           spss.EndDataStep()

           # Set the alignment
           import spss
           spss.StartDataStep()
           datasetObj = spss.Dataset()
           varObj = datasetObj.varlist['gender']
           varObj.alignment = 1
           spss.EndDataStep()
        """
        error.Reset()

        result = PyInvokeSpss.GetVarAlignmentInDS(spssutil.CheckStr(self.__ds.name), self.index)
        out = result[0]
        errLevel = result[1]

        error.SetErrorCode(errLevel)
        if error.IsError():
            raise SpssError, error
        return out

    def _setalignment(self, value):
        error.Reset()

        if not isinstance(value, (int, long)):
            error.SetErrorCode(1000)
            raise SpssError, error

        if value < 0 or value > 2:
            error.SetErrorCode(35)
            raise SpssError, error

        errLevel = PyInvokeSpss.SetVarAlignmentInDS(spssutil.CheckStr(self.__ds.name), self.index, value)

        error.SetErrorCode(errLevel)
        if error.IsError():
            raise SpssError, error

    def _getcolumnWidth(self):
        """Get or set the column width of data values displayed in the Data Editor.
        Changing the column width does not change the defined width of a variable.
        When setting the column width, the specified value must be a positive integer.

           --examples
           # Get the column width
           import spss
           spss.StartDataStep()
           datasetObj = spss.Dataset()
           varObj = datasetObj.varlist['prevexp']
           varwidth = varObj.columnWidth
           spss.EndDataStep()

           # Set the column width
           import spss
           spss.StartDataStep()
           datasetObj = spss.Dataset()
           varObj = datasetObj.varlist['prevexp']
           varObj.columnWidth = 3
           spss.EndDataStep()
        """
        error.Reset()

        result = PyInvokeSpss.GetVarColumnWidthInDS(spssutil.CheckStr(self.__ds.name), self.index)
        out = result[0]
        errLevel = result[1]

        error.SetErrorCode(errLevel)
        if error.IsError():
            raise SpssError, error
        return out

    def _setcolumnWidth(self, value):
        error.Reset()

        if not isinstance(value, (int, long)):
            error.SetErrorCode(1000)
            raise SpssError, error

        errLevel = PyInvokeSpss.SetVarColumnWidthInDS(spssutil.CheckStr(self.__ds.name), self.index, value)

        error.SetErrorCode(errLevel)
        if error.IsError():
            raise SpssError, error

    def _getmeasurementLevel(self):
        """Get or set the measurement level of the variable.

           --details
           When getting, the returned value is an uppercase string with one of the following values:
           NOMINAL, SCALE, ORDINAL, or UNKNOWN. UNKNOWN may be returned for numeric variables prior to the
           first data pass when the measurement level has not been explicitly set, such as data read from an
           external source or newly created variables.

           When setting, the allowed values are: 'NOMINAL', 'SCALE', or 'ORDINAL'.

           --examples
           # Get the measurement level
           import spss
           spss.StartDataStep()
           datasetObj = spss.Dataset()
           varObj = datasetObj.varlist['minority']
           varlevel = varObj.measurementLevel
           spss.EndDataStep()

           # Set the measurement level
           import spss
           spss.StartDataStep()
           datasetObj = spss.Dataset()
           varObj = datasetObj.varlist['minority']
           varObj.measurementLevel = "NOMINAL"
           spss.EndDataStep()
        """
        error.Reset()

        result = PyInvokeSpss.GetVarMeasurementLevelInDS(spssutil.CheckStr(self.__ds.name), self.index)
        out = result[0]
        errLevel = result[1]

        error.SetErrorCode(errLevel)
        if error.IsError():
            raise SpssError, error
        return out

    def _setmeasurementLevel(self, value):
        error.Reset()
        value = spssutil.CheckStr(value)

        errLevel = PyInvokeSpss.SetVarMeasurementLevelInDS(spssutil.CheckStr(self.__ds.name), self.index, value.upper().rstrip())

        error.SetErrorCode(errLevel)
        if error.IsError():
            raise SpssError, error

    def _getrole(self):
        """Get or set the role of the variable.

           --details
           When getting, the returned value is string with one of the following values:
           "Input", "Target", "Both", "None", "Partition", "Split", or "Frequency".

            When setting, the allowed values are:
               "Input"
               "Target"
               "Both"
               "None"
               "Partition"
               "Split"
               "Frequency"

           --examples
           # Get the role
           import spss
           spss.Submit("GET FILE='demo.sav'.")
           spss.StartDataStep()
           datasetObj = spss.Dataset()
           varObj = datasetObj.varlist['marital']
           varrole = varObj.role
           spss.EndDataStep()

           # Set the measurement level
           import spss
           spss.Submit("GET FILE='demo.sav'.")
           spss.StartDataStep()
           datasetObj = spss.Dataset()
           varObj = datasetObj.varlist['marital']
           varObj.role = 1
           spss.EndDataStep()
        """
        error.Reset()

        varRole = { 0:"Input",
                    1:"Target",
                    2:"Both",
                    3:"None",
                    4:"Partition",
                    5:"Split",
                    6:"Frequency"
                  }

        result = PyInvokeSpss.GetVarRoleInDS(spssutil.CheckStr(self.__ds.name), self.index)
        out = varRole[result[0]]
        errLevel = result[1]

        error.SetErrorCode(errLevel)
        if error.IsError():
            raise SpssError, error
        return out

    def _setrole(self, value):
        error.Reset()

        varRole = { "Input":0,
                    "Target":1,
                    "Both":2,
                    "None":3,
                    "Partition":4,
                    "Split":5,
                    "Frequency":6
                  }

        if value not in varRole:
            error.SetErrorCode(118)
            raise SpssError, error

        value=varRole[value]

        errLevel = PyInvokeSpss.SetVarRoleInDS(spssutil.CheckStr(self.__ds.name), self.index, value)

        error.SetErrorCode(errLevel)
        if error.IsError():
            raise SpssError, error

    class Attribute:

        """Get or set custom variable attributes. The Attribute class represent by the Python dict and is
        instantiated from the attributes property of a Variable object. The set of variable attributes for a
        particular variable is represented as a Python dictionary whose keys are the attribute names and whose values
        are the values of the attributes. Each value in the dictionary is either a simple value or a list of values
        (used to represent an attribute array).
        """

        def __init__(self, variable):
            """Create an Attribute instance for a specified Variable instance.

               --arguments
               variable: An instance of the Variable class.
            """
            error.Reset()
            self.__var = variable
            self.data = {}
            self.__fillAll()

        def __getitem__(self, name):
            error.Reset()
            attname = spssutil.CheckStr(name.rstrip())
            attname = attname.rstrip()

            result = PyInvokeSpss.GetVarAttributesInDS(spssutil.CheckStr(self.__var._Variable__ds.name), self.__var.index, attname)
            out = result[0]
            errLevel = result[1]

            error.SetErrorCode(errLevel)
            if error.IsError():
                raise SpssError, error
            inname=spssutil.CheckStr16(name.rstrip())
            self.data[inname] = out
            return self.data[inname]

        def __setitem__(self, name, values):
            error.Reset()
            name = spssutil.CheckStr(name)
            attname = name.rstrip()

            if isinstance(values, basestring):
                attvalue = [spssutil.CheckStr(values.rstrip())]
            elif isinstance(values, (tuple, list)):
                values=list(values)
                for i in range(len(values)):
                    values[i] = spssutil.CheckStr(values[i])
                attvalue = values
            else:
                error.SetErrorCode(1062)
                raise SpssError, error

            errLevel = PyInvokeSpss.SetVarAttributesInDS(spssutil.CheckStr(self.__var._Variable__ds.name), self.__var.index, attname, attvalue)

            error.SetErrorCode(errLevel)
            if error.IsError():
                raise SpssError, error
            self.__fillAll()

        def __delitem__(self, name):
            error.Reset()
            name = spssutil.CheckStr(name)
            attname = name.rstrip()

            errLevel = PyInvokeSpss.DelVarAttributesInDS(spssutil.CheckStr(self.__var._Variable__ds.name), self.__var.index, attname)

            error.SetErrorCode(errLevel)
            if error.IsError():
                raise SpssError, error
            self.__fillAll()

        def __repr__(self):
            return self.data.__repr__()

        def __len__(self):
            return len(self.data)

        def __eq__(self, other):
            if not isinstance(other, Variable.Attribute):
                return False
            return self.data.__eq__(other.data)

        def __ne__(self, other):
            if not isinstance(other, Variable.Attribute):
                return False
            return self.data.__ne__(other.data)

        def clear(self):
            error.Reset()

            for i in self.data.keys():
                errLevel = PyInvokeSpss.DelVarAttributesInDS(spssutil.CheckStr(self.__var._Variable__ds.name), self.__var.index, i)

                error.SetErrorCode(errLevel)
                if error.IsError():
                    raise SpssError, error

            self.data.clear()

        def __fillAll(self):

            result = PyInvokeSpss.GetVarAttributesNameInDS(spssutil.CheckStr(self.__var._Variable__ds.name), self.__var.index)
            ret = result[0]
            errLevel = result[1]
            error.SetErrorCode(errLevel)
            if error.IsError():
                raise SpssError, error

            self.data.clear()
            if ret is not None:
                for i in ret:
                    self.__getitem__( i )


    def _getattributes(self):
        """Get, set or delete custom variable attributes. The set of variable attributes for a particular variable is
        represented as a Python dictionary whose keys are the attribute names and whose values are the values
        of the attributes. Each value in the dictionary is either a simple value or a list of values (used to
        represent an attribute array).

           --examples
           # Get all attributes for a specified variable
           import spss
           spss.StartDataStep()
           datasetObj = spss.Dataset()
           varObj = datasetObj.varlist['minority']
           varattr = varObj.attributes
           spss.EndDataStep()

           # Get the value of a particular attribute for a specified variable
           import spss
           spss.StartDataStep()
           datasetObj = spss.Dataset()
           varObj = datasetObj.varlist['minority']
           # Get the value of the attribute named 'DemographicVars'
           attrvalue = varObj.attributes['DemographicVars']
           spss.EndDataStep()

           # Resetting attributes
           import spss
           spss.StartDataStep()
           datasetObj = spss.Dataset()
           varObj = datasetObj.varlist['gender']
           # Any existing attributes for the variable are cleared and replaced with the specified set.
           varObj.attributes = {'DemographicVars':'1','Binary':'Yes'}
           spss.EndDataStep()

           # Adding or modifying a single attribute
           import spss
           spss.StartDataStep()
           datasetObj = spss.Dataset()
           varObj = datasetObj.varlist['age']
           # If the attribute 'AnswerFormat' exists, its value is updated to 'Fill-in'. If the attribute
           # doesn't exist, it is created and added to any existing ones for the variable.
           varObj.attributes['AnswerFormat'] = 'Fill-in'
           spss.EndDataStep()

           # Deleting a specified attribute for the associated variable
           import spss
           spss.StartDataStep()
           datasetObj = spss.Dataset()
           varObj = datasetObj.varlist['gender']
           del varObj.attributes['Binary']
           spss.EndDataStep()

           # Deleting all attributes for the associated variable
           import spss
           spss.StartDataStep()
           datasetObj = spss.Dataset()
           varObj = datasetObj.varlist['gender']
           del varObj.attributes
           spss.EndDataStep()
        """
        return self.__attr

    def _setattributes(self, value):
        error.Reset()

        if not isinstance(value, dict):
            raise TypeError("Expects a dict argument.")

        self.__attr.clear()

        for i in value.items():
            self.__attr.__setitem__(i[0], i[1])

    def _delattributes(self):
        self.__attr.clear()


    class ValueLabel:

        """Get or set value labels. The ValueLabel class represent by the Python dict and is
        instantiated from the valueLabels property of a Variable object. The set of value labels for a
        particular variable is represented as a Python dictionary whose keys are the values for which
        labels are being set and whose values are the associated labels. Labels must be specified as
        quoted strings.
        """

        def __init__(self, variable):
            """Create a ValueLabel instance for a specified Variable instance.

               --arguments
               variable: An instance of the Variable class.
            """
            error.Reset()
            self.__var = variable
            self.data = {}
            self.__fillAll()

        def __getitem__(self, value):
            error.Reset()
            vartype = self.__var.type
            if vartype == 0:
                if not isinstance(value, (int,long,float)):
                    error.SetErrorCode(1033)
                    raise SpssError, error
                attvalue = value
            else:
                attvalue = spssutil.CheckStr16(value)

            self.__fillAll()
            return self.data[attvalue]

        def __setitem__(self, value, label):
            error.Reset()

            vartype = self.__var.type
            if vartype == 0:
                if not isinstance(value, (int,long,float)):
                    error.SetErrorCode(1033)
                    raise SpssError, error
                attvalue = value
                label = spssutil.CheckStr(label)
                attlabel = label.rstrip()

                errLevel = PyInvokeSpss.SetVarNValueLabelInDS(spssutil.CheckStr(self.__var._Variable__ds.name), self.__var.index, attvalue, attlabel)

            else:
                value = spssutil.CheckStr(value)
                attvalue = value[0:vartype]
                label = spssutil.CheckStr(label)
                attlabel = label.rstrip()

                errLevel = PyInvokeSpss.SetVarCValueLabelInDS(spssutil.CheckStr(self.__var._Variable__ds.name), self.__var.index, attvalue, attlabel)

            error.SetErrorCode(errLevel)
            if error.IsError():
                raise SpssError, error

            self.__fillAll()

        def __setitems__(self, values, labels):
            error.Reset()

            vartype = self.__var.type
            if vartype == 0:
                for value in values:
                    if not isinstance(value, (int,long,float)):
                        error.SetErrorCode(1033)
                        raise SpssError, error
                labels = map(spssutil.CheckStr,labels)
                labels = [x.rstrip() for x in labels]
                errLevel = PyInvokeSpss.SetVarNValueLabelsInDS(spssutil.CheckStr(self.__var._Variable__ds.name), self.__var.index, values, labels)

            else:
                values = [str(value) for value in values ]
                values = map(spssutil.CheckStr,values)
                values = [x[0:vartype] for x in values]

                labels = map(spssutil.CheckStr,labels)
                labels = [x.rstrip() for x in labels]

                errLevel = PyInvokeSpss.SetVarCValueLabelsInDS(spssutil.CheckStr(self.__var._Variable__ds.name), self.__var.index, values, labels)

            error.SetErrorCode(errLevel)
            if error.IsError():
                raise SpssError, error

            #self.__fillAll()

        def __delitem__(self, value):
            error.Reset()
            vartype = self.__var.type
            if vartype == 0:
                if not isinstance(value, (int,long,float)):
                    error.SetErrorCode(1033)
                    raise SpssError, error
                attvalue = value

                errLevel = PyInvokeSpss.DelVarNValueLabelInDS(spssutil.CheckStr(self.__var._Variable__ds.name), self.__var.index, attvalue)

            else:
                value = spssutil.CheckStr(value)
                attvalue = value.rstrip()
                errLevel = PyInvokeSpss.DelVarCValueLabelInDS(spssutil.CheckStr(self.__var._Variable__ds.name), self.__var.index, attvalue)
                attvalue = value.ljust(vartype)

            error.SetErrorCode(errLevel)
            if error.IsError():
                raise SpssError, error
            self.__fillAll()

        def __repr__(self):
            return self.data.__repr__()

        def __len__(self):
            return len(self.data)

        def __eq__(self, other):
            if not isinstance(other, Variable.ValueLabel):
                return False
            return self.data.__eq__(other.data)

        def __ne__(self, other):
            if not isinstance(other, Variable.ValueLabel):
                return False
            return self.data.__ne__(other.data)

        def clear(self):
            error.Reset()

            errLevel = PyInvokeSpss.DelVarValueLabelInDS(spssutil.CheckStr(self.__var._Variable__ds.name), self.__var.index)
            error.SetErrorCode(errLevel)
            if error.IsError():
                raise SpssError, error
            self.data.clear()

        def __fillAll(self):

            if self.__var.type == 0:
                result = PyInvokeSpss.GetVarNValueLabelInDS(spssutil.CheckStr(self.__var._Variable__ds.name), self.__var.index)
                out = result[0]
                errLevel = result[1]

            else:
                result = PyInvokeSpss.GetVarCValueLabelInDS(spssutil.CheckStr(self.__var._Variable__ds.name), self.__var.index)
                out = result[0]
                errLevel = result[1]

            error.SetErrorCode(errLevel)
            if error.IsError():
                raise SpssError, error

            self.data.clear()
            if out is not None:
                self.data.__init__(out)


    def _getvalueLabels(self):
        """Get, set or delete value labels. The set of value labels for a particular variable is represented
        as a Python dictionary whose keys are the values for which labels are being set and whose
        values are the associated labels. Labels must be specified as quoted strings.

           --examples
           # Get all value labels for a specified variable
           import spss
           spss.StartDataStep()
           datasetObj = spss.Dataset()
           varObj = datasetObj.varlist['minority']
           vallabels = varObj.valueLabels
           spss.EndDataStep()

           # Get the label for a particular value for a specified variable
           import spss
           spss.StartDataStep()
           datasetObj = spss.Dataset()
           varObj = datasetObj.varlist['minority']
           # Get the label for the value 1
           vallabel = varObj.valueLabels[1]
           spss.EndDataStep()

           # Resetting value labels
           import spss
           spss.StartDataStep()
           datasetObj = spss.Dataset()
           varObj = datasetObj.varlist['origin']
           # Any existing value labels for the variable are cleared and replaced with the specified set.
           varObj.valueLabels = {1:'American',2:'Japanese',3:'European',4:'Korean',5:'Chinese'}
           spss.EndDataStep()

           # Adding or modifying a single value label
           import spss
           spss.StartDataStep()
           datasetObj = spss.Dataset()
           varObj = datasetObj.varlist['origin']
           # If a label for the value 4 exists, its value is updated to 'Korean'. If a label for the value 4
           # doesn't exist, it is added to any existing value labels for the variable.
           varObj.valueLabels[4] = 'Korean'
           spss.EndDataStep()

           # Deleting a specified value label for the associated variable
           import spss
           spss.StartDataStep()
           datasetObj = spss.Dataset()
           varObj = datasetObj.varlist['origin']
           del varObj.valueLabels[1]
           spss.EndDataStep()

           # Deleting all value labels for the associated variable
           import spss
           spss.StartDataStep()
           datasetObj = spss.Dataset()
           varObj = datasetObj.varlist['origin']
           del varObj.valueLabels
           spss.EndDataStep()
        """
        return self.__vallbl

    def _setvalueLabels(self, valueLabels):
        error.Reset()

        if not isinstance(valueLabels, dict):
            raise TypeError("Expects a dict argument.")

        self.__vallbl.clear()

        num = len(valueLabels.items())

        ## when there are less than smallSize value labels, set them one by one.
        ## otherwise, divide them into groups and set group by group.
        smallSize = 10
        groupSize = 100

        if num < smallSize:
            for x in valueLabels.items():
                self.__vallbl.__setitem__(x[0], x[1])
        else:
            loop = num // groupSize
            for i in range(loop):
                values = valueLabels.keys()[i*groupSize:(i+1)*groupSize]
                labels = valueLabels.values()[i*groupSize:(i+1)*groupSize]
                self.__vallbl.__setitems__(values, labels)

            if num > loop * groupSize:
                values = valueLabels.keys()[loop*groupSize:]
                labels = valueLabels.values()[loop*groupSize:]
                self.__vallbl.__setitems__(values, labels)

        self.__vallbl._ValueLabel__fillAll()


    def _delvalueLabels(self):
        self.__vallbl.clear()

    def _getmissingValues(self):
        """Get or set the user-missing values of the associated variable.

           --details
           When getting missing values, the returned value is a tuple of four elements where the first element
           specifies the missing value type and the remaining three elements specify the missing values or the
           endpoints of a range of missing values. The integer missing value types and the associated structure of
           the results follow below. When setting missing values, the same structure is used but the specified value
           can be a tuple or list.

                -3 -- A range of values and a single discrete missing value. The 2nd and 3rd elements of the tuple
                      contain the start and end of the range respectively and the 4th element contains the
                      discrete value.
                -2 -- A range of values. The 2nd and 3rd elements of the tuple contain the start and end of the
                      range respectively and the 4th element is None.
                 0 -- No missing values. The 2nd, 3rd, and 4th elements of the tuple are None.
                 1 -- A single missing value. The 2nd element of the tuple contains the missing value and the
                      remaining elements in the tuple are None.
                 2 -- Two discrete missing values. The 2nd and 3rd elements of the tuple contain the missing values
                      and the 4th element is None.
                 3 -- Three discrete missing values. The 2nd, 3rd, and 4th elements of the tuple contain the
                      missing values.

           --examples
           # Get the missing values
           import spss
           spss.StartDataStep()
           datasetObj = spss.Dataset()
           varObj = datasetObj.varlist['numvar']
           missingvals = varObj.missingValues
           spss.EndDataStep()

           # Set missing values
           import spss
           spss.StartDataStep()
           datasetObj = spss.Dataset()
           varObj = datasetObj.varlist['numvar']
           # Set missing values of a numeric variable to the range 9 thru 99 and the single value 0.
           varObj.missingValues = [-3,9,99,0]
           spss.EndDataStep()
        """
        error.Reset()

        vartype = self._gettype()
        if vartype == 0:
            result = PyInvokeSpss.GetVarNMissingValuesInDS(spssutil.CheckStr(self.__ds.name), self.index)
            out = tuple(result[:-1])
        else:
            result = PyInvokeSpss.GetVarCMissingValuesInDS(spssutil.CheckStr(self.__ds.name), self.index)
            format = lambda value: value and value[:vartype]
            out = (result[0], format(result[1]), format(result[2]), format(result[3]))

        errLevel = result[4]

        error.SetErrorCode(errLevel)
        if error.IsError():
            raise SpssError, error
        return out

    def _setmissingValues(self, value):
        error.Reset()

        if not isinstance(value, (tuple,list)):
            error.SetErrorCode(1027)
            raise SpssError, error

        value=list(value)
        if len(value) != 4:
            error.SetErrorCode(1064)
            raise SpssError, error

        if not isinstance(value[0], (int, long)):
            error.SetErrorCode(1000)
            raise SpssError, error

        if self._gettype() == 0:
            for i in range(1,4):
                if value[i] is None:
                    value[i] = 0
                if not isinstance(value[i], (int, long, float)):
                    error.SetErrorCode(1033)
                    raise SpssError, error
            errLevel = PyInvokeSpss.SetVarNMissingValuesInDS(spssutil.CheckStr(self.__ds.name), self.index, value[0], value[1], value[2], value[3])
        else:
            for i in range(1,4):
                if value[i] is None:
                    value[i] = ""
                else:
                    value[i] = spssutil.CheckStr(value[i])
            errLevel = PyInvokeSpss.SetVarCMissingValuesInDS(spssutil.CheckStr(self.__ds.name), self.index, value[0], value[1].rstrip(), value[2].rstrip(), value[3].rstrip())

        error.SetErrorCode(errLevel)
        if error.IsError():
            raise SpssError, error


    type             = property(_gettype, _settype)
    name             = property(_getname, _setname)
    label            = property(_getlabel, _setlabel)
    format           = property(_getformat,_setformat)
    alignment        = property(_getalignment, _setalignment)
    columnWidth      = property(_getcolumnWidth, _setcolumnWidth)
    measurementLevel = property(_getmeasurementLevel, _setmeasurementLevel)
    attributes       = property(_getattributes,_setattributes,_delattributes)
    valueLabels      = property(_getvalueLabels,_setvalueLabels,_delvalueLabels)
    missingValues    = property(_getmissingValues,_setmissingValues)
    role             = property(_getrole, _setrole)

__all__ = ["Dataset", "VariableList", "Variable", "CaseList"]

import version
__version__ = version.version
