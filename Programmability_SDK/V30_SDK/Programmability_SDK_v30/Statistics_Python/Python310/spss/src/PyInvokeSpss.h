/************************************************************************
** Licensed Materials - Property of IBM 
**
** IBM SPSS Products: Statistics Common
**
** (C) Copyright IBM Corp. 1989, 2015
**
** US Government Users Restricted Rights - Use, duplication or disclosure
** restricted by GSA ADP Schedule Contract with IBM Corp. 
************************************************************************/

/**
 * NAME
 *     PyInvokeSpss.h
 *
 * DESCRIPTION
 *     An extension module for invoking PASW Statistics from python.
 *
 *--------------------------------------------------------
 *
 * Copyright (c) 2008 SPSS Inc. All rights reserved.
 */


#ifndef _PYINVOKESPSS_H
#define _PYINVOKESPSS_H

#ifdef MS_WINDOWS

  #ifdef PYINVOKESPSS_EXPORTS
  #define PYINVOKESPSS_API __declspec(dllexport)
  #else
  #define PYINVOKESPSS_API __declspec(dllimport)
  #endif

#else
  #define PYINVOKESPSS_API
#endif

#include "Python.h"

extern "C"{

//static PyObject *my_callback = NULL;//my_set_callback

/**
 *typedef function object.
 */
 
//typedef void             (*FP_set_callback_xd)(PyObject *the_callback);
typedef bool             (*FP_IsBackendReady)();
typedef bool             (*FP_IsXDriven)();
typedef void             (*FP_SetXNameAndSHome)(const char* xLanguageName, const char* statisticsHome);
typedef int              (*FP_StartSpss)(const char* commandline);
typedef void             (*FP_StopSpss)();
typedef int              (*FP_Submit)(const char* command, int length);
typedef int              (*FP_QueueCommandPart)(const char* line, int length);
typedef int              (*FP_PostSpssOutput)(const char* text, int length);
typedef int              (*FP_GetVariableType)(int index,int& errCode);
typedef unsigned         (*FP_GetVariableCount)(int& errCode);
typedef long             (*FP_GetRowCount)(int& errCode);
typedef const char*      (*FP_GetVariableName)(int index,int& errCode);
typedef const char*      (*FP_GetVariableLabel)(int index,int& errCode);
typedef const char*      (*FP_GetVariableFormat)(int index,int& errCode);
typedef int              (*FP_GetVariableMeasurementLevel)(int index,int& errCode);
typedef int              (*FP_CreateXPathDictionary)(const char* handle);
typedef int              (*FP_RemoveXPathHandle)(const char* handle);
typedef void*            (*FP_EvaluateXPath)(const char* handle, const char* context, const char* expression, int& errCode);
typedef int              (*FP_GetStringListLength)(void* listHandle);
typedef const char*      (*FP_GetStringFromList)(void* listHandle, int index);
typedef const wchar_t*   (*FP_GetXmlUtf16)(const char* handle,int& errCode);
typedef const char*      (*FP_GetImage)(const char* handle,  const char* fileName,int& imageSize, char** imageType,int& errcode);
typedef const char*      (*FP_GetSetting)(const char* seting, int &errCode, const char* option);
typedef void*            (*FP_GetOMSTagList)(int& errLevel);
typedef void             (*FP_RemoveStringList)(void* listHandle);
typedef void*            (*FP_GetHandleList)(int& errLevel);
typedef void*            (*FP_GetFileHandles)(int& errLevel);
typedef const char*		 (*FP_TransCode)(const char* orig, int& errLevel);
typedef int              (*FP_GetNumericValue)(unsigned varindex, double &result, int &isMissing);
typedef int              (*FP_GetStringValue)(unsigned varindex, char* &result, int bufferLength, int &isMissing);
typedef int              (*FP_GetColumnCountInProcDS)(int &columnCount);
typedef int              (*FP_NextCase)();
typedef int              (*FP_RemoveCaseCursor)();
typedef int              (*FP_GetVariableFormatType)(int index, int &formatType, int& formatWidth, int& formatDecimal);
typedef int              (*FP_GetCursorPosition)(int &curPos);
typedef int              (*FP_MakeCaseCursor)(const char*);
typedef int              (*FP_StartProcedure)(const char* procName, const char* translatedName);
typedef int              (*FP_SplitChange)(const char* procName);
typedef int              (*FP_EndProcedure)();

typedef int              (*FP_StartPivotTable)(const char* outline, const char* title,const char* templateName, bool isSplit);
typedef int              (*FP_HidePivotTableTitle)(const char* outline, const char* title,const char* templateName, bool isSplit);
typedef int              (*FP_PivotTableCaption)(const char* outline, const char* title,const char* templateName, bool isSplit,const char* caption);
typedef int              (*FP_MinDataColumnWidth)(const char* outline, const char* title, const char* templateName,bool isSplit,int num);
typedef int              (*FP_MaxDataColumnWidth)(const char* outline, const char* title, const char* templateName,bool isSplit,int num);
typedef int              (*FP_AddDimension)(const char* outline, const char* title, const char* templateName, bool isSplit,const char* dimName,int place,int position,bool hideName,bool hideLabels);
typedef int              (*FP_AddNumberCategory)(const char* outline, const char* title, const char* templateName,bool isSplit,const char* dimName,int place,int position,bool hideName,bool hideLabels,double category);
typedef int              (*FP_AddStringCategory)(const char* outline, const char* title, const char* templateName,bool isSplit,const char* dimName,int place,int position,bool hideName,bool hideLabels,const char* category);
typedef int              (*FP_AddVarNameCategory)(const char* outline, const char* title, const char* templateName,bool isSplit,const char* dimName,int place,int position,bool hideName,bool hideLabels,int category);
typedef int              (*FP_AddVarValueDoubleCategory)(const char* outline, const char* title, const char* templateName,bool isSplit,const char* dimName,int place,int position,bool hideName,bool hideLabels,int category, double d);
typedef int              (*FP_AddVarValueStringCategory)(const char* outline, const char* title, const char* templateName,bool isSplit,const char* dimName,int place,int position,bool hideName,bool hideLabels,int category, const char* ch);
typedef int              (*FP_SetNumberCell)(const char* outline, const char* title, const char* templateName,bool isSplit,const char* dimName,int place,int position,bool hideName,bool hideLabels,double cellVal);
typedef int              (*FP_SetStringCell)(const char* outline, const char* title, const char* templateName,bool isSplit,const char* dimName,int place,int position,bool hideName,bool hideLabels,const char* cellVal);
typedef int              (*FP_SetVarNameCell)(const char* outline, const char* title, const char* templateName,bool isSplit,const char* dimName,int place,int position,bool hideName,bool hideLabels,int cellVal);
typedef int              (*FP_SetVarValueDoubleCell)(const char* outline, const char* title, const char* templateName,bool isSplit,const char* dimName,int place,int position,bool hideName,bool hideLabels,int cellVal, double d);
typedef int              (*FP_SetVarValueStringCell)(const char* outline, const char* title, const char* templateName,bool isSplit,const char* dimName,int place,int position,bool hideName,bool hideLabels,int cellVal, const char* ch);

typedef int              (*FP_SetFormatSpecCoefficient)();
typedef int              (*FP_SetFormatSpecCoefficientSE)();
typedef int              (*FP_SetFormatSpecCoefficientVar)();
typedef int              (*FP_SetFormatSpecCorrelation)();
typedef int              (*FP_SetFormatSpecGeneralStat)();
typedef int              (*FP_SetFormatSpecMean)(int varIndex);
typedef int              (*FP_SetFormatSpecCount)();
typedef int              (*FP_SetFormatSpecPercent)();
typedef int              (*FP_SetFormatSpecPercentNoSign)();
typedef int              (*FP_SetFormatSpecProportion)();
typedef int              (*FP_SetFormatSpecSignificance)();
typedef int              (*FP_SetFormatSpecResidual)();
typedef int              (*FP_SetFormatSpecVariable)(int varIndex);
typedef int              (*FP_SetFormatSpecStdDev)(int varIndex);
typedef int              (*FP_SetFormatSpecDifference)(int varIndex);
typedef int              (*FP_SetFormatSpecSum)(int varIndex);
typedef int              (*FP_AddCellFootnotes)(const char* outline, const char* title, const char* templateName, bool isSplit,const char* dimName,int place,int position,bool hideName,bool hideLabels,const char* footnotes);

typedef int              (*FP_AddProcFootnotes)(const char *footnotes);
typedef int              (*FP_AddOutlineFootnotes)(const char* outline, const char* title, 
                                            const char* templateName, const char *footnotes,bool isSplit);
typedef int              (*FP_AddTitleFootnotes)(const char* outline, const char* title, 
                                            const char* templateName, const char *footnotes,bool isSplit);
typedef int              (*FP_AddDimFootnotes)(const char* outline, const char* title, 
                                            const char* templateName, bool isSplit,
                                            const char* dimName,int place,int position,bool hideName,
                                            bool hideLabels,const char* footnotes);
typedef int              (*FP_AddCategoryFootnotes)(const char* outline, const char* title, 
                                            const char* templateName, bool isSplit,
                                            const char* dimName,int place,int position,bool hideName,
                                            bool hideLabels,const char* footnotes);


typedef int              (*FP_AddTextBlock)(const char* outLine, const char* name, const char* line, int nSkip);

typedef int              (*FP_CommitCaseRecord)();
typedef int              (*FP_CommitNewCase)();
typedef int              (*FP_CommitManyCases)(char ** newVarList, int * varTypes, void *caseData, int varSize, int totalCaseSize, int caseNumber);
typedef int              (*FP_HasProcedure)();
typedef const char*      (*FP_GetWeightVar)(int &errLevel);
typedef int              (*FP_GetSPSSLowHigh)(double *low, double *high);
typedef int              (*FP_HasCursor)(int &hasCur);
typedef int              (*FP_IsEndSplit)(int &endSplit);

typedef long             (*FP_GetRowCountInProcDS)(int& errCode);
typedef unsigned         (*FP_GetVariableCountInProcDS)(int& errCode);
typedef const char*      (*FP_GetVariableLabelInProcDS)(int index,int& errCode);
typedef int              (*FP_GetVariableMeasurementLevelInProcDS)(int index,int& errCode);
typedef const char*      (*FP_GetVariableNameInProcDS)(int index,int& errCode);
typedef int              (*FP_GetVariableTypeInProcDS)(int index,int& errCode);
typedef int              (*FP_GetVarAttributeNamesInProcDS)(const int index, char ***name, int *numOfNames);
typedef int              (*FP_GetVarAttributesInProcDS)(const int index, const char *attrName, char ***attr, int *numOfAttr);
typedef int              (*FP_GetVarCMissingValuesInProcDS)(const int index, int *missingFormat,char * v1, char *v2, char *v3);
typedef int              (*FP_GetVarNMissingValuesInProcDS)(const int index,int *missingFormat,double *v1, double *v2, double *v3);
typedef int              (*FP_GetVariableFormatTypeInProcDS)(int index, int &formatType, int& formatWidth, int& formatDecimal);

typedef int              (*FP_GetVarCMissingValues)(const int index, int *missingFormat,char * v1, char *v2, char *v3);
typedef int              (*FP_GetVarNMissingValues)(const int index,int *missingFormat,double *v1, double *v2, double *v3);
typedef int              (*FP_GetVarAttributeNames)(const int index, char ***name, int *numOfName);
typedef int              (*FP_FreeAttributeNames)(char **name, const int numOfNames);
typedef int              (*FP_GetVarAttributes)(const int index, const char *attrName, char ***attr, int *numOfAttr);
typedef int              (*FP_FreeAttributes)(char **attr, const int numOfAttr);
typedef int              (*FP_SetVarAttributes)(const char *varName,const  char *attrName,const char *value,const int index);
typedef const char*      (*FP_GetVariableFormatInProcDS)(int index,int& errCode);
typedef int              (*FP_SetVarNameAndType)(char *varName[],const int *varType,const unsigned int numOfVar);
typedef int              (*FP_SetVarLabel)(const char *varName,const char *varLabel);
typedef int              (*FP_SetVarCValueLabel)(const char *varName,const char *value,const char *label);
typedef int              (*FP_SetVarNValueLabel)(const char *varName,const double value,const char *label);
typedef int              (*FP_SetVarCMissingValues)(const char *varName,const int missingFormat,const char *missingVal1,const char *missingVal2,const char *missingVal3);
typedef int              (*FP_SetVarNMissingValues)(const char *varName,const int missingFormat,const double missingVal1,const double missingVal2,const double missingVal3);
typedef int              (*FP_SetVarMeasureLevel)(const char * varName,const int measureLevel);
typedef int              (*FP_SetVarAlignment)(const char * varName,const int alignment);
typedef int              (*FP_SetVarFormat)(const char *varName,const int writeType,const int writeDec,const int writeWid);
typedef int              (*FP_CommitHeader)();
typedef int              (*FP_SetValueChar)(const char *varName,const char* value,const int length);
typedef int              (*FP_SetValueNumeric)(const char *varName,const double value);
typedef int              (*FP_CommitCaseRecord)();
typedef int              (*FP_EndChanges)();
typedef int              (*FP_IsEndSplit)(int & endSplit);
typedef int              (*FP_ResetDataPass)();
typedef int              (*FP_AllocNewVarsBuffer)(unsigned int size);
typedef int              (*FP_SetOneVarNameAndType)(const char *varName, int varType);
typedef int              (*FP_SetXDriveMode)(int mode);

// data step
typedef int              (*FP_StartDataStep)();    
typedef int              (*FP_EndDataStep)();    
typedef int              (*FP_CreateDataset)(const char *name, const bool isEmpty, const bool hidden);     
typedef int              (*FP_SetDatasetName)(const char *oldname, const char *newname);        
typedef int              (*FP_GetNewDatasetName)(char* dsName);             
typedef const char*      (*FP_GetActive)(int& errLevel);    
typedef int              (*FP_SetActive)(const char *name);
typedef int              (*FP_CopyDataset)( const char* oriDs, const char* desDs);                                                      
typedef int              (*FP_GetSpssDatasets)( char*** nameList, int& length);  
typedef int              (*FP_GetDatastepDatasets)( char*** nameList, int& length);                                                     
typedef int              (*FP_FreeStringArray)(char **array,  const int length);        
typedef int              (*FP_CloseDataset)(const char* name);          
typedef int              (*FP_InsertVariable)(const char* dsName, const int index, const char* varName, const int type);                                                                
typedef int              (*FP_DeleteVariable)(const char* dsName, const int index);   
typedef unsigned         (*FP_GetVarCountInDS)(const char* name, int& errLevel);
typedef const char*      (*FP_GetVarNameInDS)(const char* dsName, const int index, int& errLevel);                                                                        
typedef int              (*FP_SetVarNameInDS)(const char* dsName,  const int index,  const char* varName);                                                        
typedef const char*      (*FP_GetVarLabelInDS)(const char* dsName, const int index, int& errLevel);                                                                       
typedef int              (*FP_SetVarLabelInDS)(const char* dsName, const int index, const char* varLabel);                                                        
typedef int              (*FP_GetVarTypeInDS)(const char* dsName, const int index, int& errLevel);                                                        
typedef int              (*FP_SetVarTypeInDS)(const char* dsName, const int index, const int varType);                                                            
typedef const char*      (*FP_GetVarFormatInDS)( const char* dsName, const int index, int& errLevel);                                                                             
typedef int              (*FP_SetVarFormatInDS)(  const char* dsName, const int index, const int formatType, const int formatWidth, const int formatDecimal);                                                             
typedef int              (*FP_GetVarAlignmentInDS)(const char* dsName,  const int index,  int& errLevel);                                                         
typedef int              (*FP_SetVarAlignmentInDS)(const char* dsName, const int index, const int varAlignment);                                                                  
typedef const char*      (*FP_GetVarMeasurementLevelInDS)(const char* dsName, const int index, int& errLevel);                                                                                            
typedef int              (*FP_SetVarMeasurementLevelInDS)(const char* dsName, const int index, const char* varMeasure);           
typedef int              (*FP_GetVarNMissingValuesInDS)(  const char* dsName, const int index, int& missingFormat, double* missingValue1, double* missingValue2, double* missingValue3);                  
typedef int              (*FP_GetVarCMissingValuesInDS)(  const char* dsName, const int index, int& missingFormat, char* missingValue1, char* missingValue2, char* missingValue3);                
typedef int              (*FP_SetVarNMissingValuesInDS)(const char* dsName, const int index, const int missingFormat, const double missingValue1, const double missingValue2, const double missingValue3);      
typedef int              (*FP_SetVarCMissingValuesInDS)(const char* dsName, const int index, const int missingFormat, const char* missingValue1, const char* missingValue2, const char* missingValue3);                                                                         
typedef int              (*FP_GetVarAttributesNameInDS)(const char* dsName, const int index, char*** names, int& length);                                                                               
typedef int              (*FP_GetVarAttributesInDS)(const char* dsName, const int index, const char* attName, char*** values, int& length);                                                                             
typedef int              (*FP_SetVarAttributesInDS)(const char* dsName, const int index, const char* attrName, char** attributes, const int length);                                                                    
typedef int              (*FP_DelVarAttributesInDS)(const char* dsName, const int index, const char* attrName); 
typedef int              (*FP_GetVarNValueLabelInDS)( const char* dsName, const int index, double** values, char*** labels, int& num);                                                                    
typedef int              (*FP_FreeDoubleArray)(double* array,  int length);     
typedef int              (*FP_GetVarCValueLabelInDS)( const char* dsName, const int index, char*** values, char*** labels, int& num);                                                                     
typedef int              (*FP_SetVarNValueLabelInDS)(const char* dsName, const int index, const double value, const char* label);                                                                       
typedef int              (*FP_SetVarCValueLabelInDS)(const char* dsName, const int index, const char* value, const char* label);                                                                        
typedef int              (*FP_DelVarValueLabelInDS)(const char* dsName, const int index);       
typedef int              (*FP_DelVarNValueLabelInDS)(const char* dsName, const int index, const double delValue);  
typedef int              (*FP_DelVarCValueLabelInDS)(const char* dsName, const int index, const char* delValue);                                                                
typedef int              (*FP_InsertCase)(const char* dsName, const int rowIndex);                                                              
typedef int              (*FP_DeleteCase)(const char* dsName, const int rowIndex);                                                              
typedef int              (*FP_GetCaseCountInDS)(const char* dsName, int& errLevel );                                                            
typedef double           (*FP_GetNCellValue)(const char* dsName, const int rowIndex, const int columnIndex, int& isMissing, int& errLevel );                                                            
typedef const char*      (*FP_GetCCellValue)(const char* dsName, const int rowIndex, const int columnIndex, int& isMissing, int& errLevel );                                                    
typedef int              (*FP_SetNCellValue)(const char* dsName, const int rowIndex, const int columnIndex, const double value );                                                               
typedef int              (*FP_SetCCellValue)(const char* dsName, const int rowIndex, const int columnIndex, const char* value );
typedef int              (*FP_SetDataFileAttributesInDS)(const char* dsName,
                                                         const char* attrName,
                                                         char** attributes,
                                                         const int length);
typedef void             (*FP_SetMultiResponseSetInDS)(const char *datasetName, 
                                                       const char *mrsetName,
                                                       const char *mrsetLabel,
                                                       const int mrsetCodeAs,
                                                       const char *mrsetCountedValue,
                                                       const char **elemVarNames,
                                                       const int numOfVars,
                                                       int &errLevel);
typedef int             (*FP_GetDataFileAttributeNamesInDS)(const char* dsName, 
                                                            char ***name, 
                                                            int *numOfNames);
typedef int             (*FP_GetDataFileAttributesInDS)(const char* dsName, 
                                                         const char *attrName, 
                                                         char ***attr, 
                                                         int *numOfAttr);   
typedef void*           (*FP_GetMultiResponseSetNamesInDS)(const char* dsName, int &errLevel);                                                                                                                 
typedef void            (*FP_GetMultiResponseSetInDS)(const char* dsName, 
                                                     const char *mrsetName, 
                                                     char **mrsetLabel, 
                                                     int &mrsetCodeAs,
                                                     char **mrsetCountedValue,
                                                     int &mrsetDataType,
                                                     void **elemVarNames, 
                                                     int &errLevel);
typedef double          (*FP_GetNCellValueFromCache)(const char* dsName,
                                                     const int columnIndex,
                                                     int& isMissing,
                                                     int& errLevel);
typedef const char*     (*FP_GetCCellValueFromCache)(const char* dsName,
                                                     const int columnIndex,
                                                     int& errLevel);
typedef int             (*FP_SetNCellValueFromCache)(const char* dsName,
                                                     const int columnIndex,
                                                     const double value);
typedef int             (*FP_SetCCellValueFromCache)(const char* dsName,
                                                     const int columnIndex,
                                                     const char* value);
typedef void            (*FP_GetVarInfo)(const char* dsName, int** varInfo, int& size);
typedef void            (*FP_GetCaseValue)(const char* dsName, 
                                          const int rowIndex, 
                                          bool isCache,
                                          int& len,
                                          int& errCode);
typedef int             (*FP_SetCasePartValue)(const char* dsName, 
                                               const int rowIndex, 
                                               int* columnIndexList,
                                               bool isCache,
                                               int size);
typedef int             (*FP_SetCacheInDS)(const char* dsName, bool isCache);
                                                     
// end data step

typedef int              (*FP_IsUTF8mode)();
typedef void*            (*FP_GetSplitVariableNames)(int& errLevel);
typedef int              (*FP_GetDataFileAttributes)(const char *attrName, char ***attr, int *numOfAttr);
typedef int              (*FP_GetDataFileAttributeNames)(char ***name, int *numOfNames);
typedef int              (*FP_GetDataFileAttributesInProcDS)(const char *attrName, char ***attr, int *numOfAttr);
typedef int              (*FP_GetDataFileAttributeNamesInProcDS)(char ***name, int *numOfNames);
typedef void*            (*FP_GetMultiResponseSetNames)(int &errLevel);
typedef void             (*FP_GetMultiResponseSet)(const char *mrsetName, 
                                                   char **mrsetLabel, 
                                                   int &mrsetCodeAs,
                                                   char **mrsetCountedValue,
                                                   int &mrsetDataType,
                                                   void **elemVarNames, 
                                                   int &errLevel);
typedef void*            (*FP_GetMultiResponseSetNamesInProcDS)(int &errLevel);
typedef void             (*FP_GetMultiResponseSetInProcDS)(const char *mrsetName, 
                                                           char **mrsetLabel, 
                                                           int &mrsetCodeAs,
                                                           char **mrsetCountedValue,
                                                           int &mrsetDataType,
                                                           void **elemVarNames, 
                                                           int &errLevel);
typedef void             (*FP_FreeString)(char *str);
    
typedef int              (*FP_GetVarColumnWidthInDS)(const char* dsName,
                                                      const int index,
                                                      int& errLevel);   
                                         
typedef int              (*FP_SetVarColumnWidthInDS)(const char* dsName,
                                                      const int index,
                                                      const int width);
                                                      
typedef int              (*FP_SaveFileInDS)(const char* dsName,
                                            const char* savePath,
                                            const bool compressed);      

typedef int             (*FP_DelDataFileAttributesInDS)(const char* dsName, 
                                                        const char *attrName);
                                        
typedef int             (*FP_DelMultiResponseSetInDS)(const char* dsName, 
                                                      const char *mrsetName);     

typedef int             (*FP_GetXDriveMode)(int &currMode, int &originMode);    
                                                                                        
typedef const char*     (*FP_GetSPSSLocale)(int &errCode);
typedef int             (*FP_GetCLocale)(char **xdlocale);
typedef void            (*FP_SetOutputLanguage)(const char *language,int &errCode);
typedef int             (*FP_GetNestDepth)(int &depth);
typedef const char*     (*FP_GetOutputLanguage)(int& errCode);


typedef double          (*FP_GetNCellValueCache)(const char* dsName,const long rowIndex, const int columnIndex,int& isMissing,int& errCode);
typedef const char*     (*FP_GetCCellValueCache)(const char* dsName,const long rowIndex, const int columnIndex,int& isMissing,int& errCode);
typedef void            (*FP_GetRowList)(const char* dsName,const long rowIndex,const int columnIndex, long& rowStart, long& rowEnd, long& rowCount, int& errCode);
typedef void            (*FP_GetVarTypeInDSCache)(int varType[], int varNumber, int& errCode);
typedef void            (*FP_ClearDatastepBatch)();

typedef int             (*FP_GetVariableRole)(int index,int& errCode);
typedef int             (*FP_GetVariableRoleInProcDS)(int index,int& errCode);
typedef int             (*FP_SetVarRole)(const char * varName, const int varRole);
typedef int             (*FP_GetVarRoleInDS)(const char* dsName, const int index, int& errLevel);
typedef int             (*FP_SetVarRoleInDS)(const char* dsName, const int index, const int varRole);
typedef int             (*FP_TransportData)(const char* portNum, const char* varList, bool needSleep);
typedef int             (*FP_GetDataFromTempFile)(const char* filename, int newVarCount, int caseLen);
typedef int             (*FP_SaveDataToTempFile)(const char* filename, int varIndexes[], int keepCount);
typedef int*            (*FP_GetSplitEndIndex)(int& size, int& errCode);
typedef int             (*FP_SetMode)(int mode);
typedef long            (*FP_GetRowCountInTempFile)(int& errCode);
typedef bool            (*FP_IsDistributedMode)();
typedef int             (*FP_GetXmlUtf16Length)();
typedef int             (*FP_IsUseOrFilter)(int &isUse);
   /**
    * Queue a line of command text but do not execute it.
    * The processor needs to be started before calling this function.
    *
    * @param self The argument is only used when the C function implements a
    *             built-in method, not a function. It will always be a NULL
    *             pointer when we are defining a function, not a method.
    * @param args A tuple of arguments.
    *             - args[0] The command line to be queued.
    *             - args[1] The length of the command line.
    * @return the error level from PASW Statistics. 0 means success.
    */
    PYINVOKESPSS_API PyObject * ext_QueueCommandPart( PyObject *self,
                                                      PyObject *args
                                                      );
    /**
     * Create an XPath DOM for the dictionary of the active file in the XML
     * workspace.
     * The processor needs to be started before calling this
     * function.
     *
     * @param self The argument is only used when the C function implements a
     *             built-in method, not a function. It will always be a NULL
     *             pointer, when we are defining a function, not a method.
     * @param args The handle of the dictionary.
     * @return the error level from PASW Statistics. 0 means success.
     */
    PYINVOKESPSS_API PyObject * ext_CreateXPathDictionary( PyObject *self,
                                                           PyObject *args
                                                           );

    /**
     * Remove an object from the XML workspace.
     * The processor needs to be started before calling this function.
     *
     * @param self The argument is only used when the C function implements a
     *             built-in method, not a function. It will always be a NULL
     *             pointer, when we are defining a function, not a method.
     * @param args The handle of an object in the XML workspace.
     * @return the error level from PASW Statistics. 0 means success.
     */
    PYINVOKESPSS_API PyObject * ext_RemoveXPathHandle( PyObject *self,
                                                       PyObject *args
                                                       );

    /**
     * Evaluate the given XPath expression within the given context.
     * The processor needs to be started before calling this function.
     *
     * @param self The argument is only used when the C function implements a
     *             built-in method, not a function. It will always be a NULL
     *             pointer, when we are defining a function, not a method.
     * @param args A tuple of arguments.
     *             - args[0] A handle of an existing object in the XML workspace.
     *             - args[1] The context or the evaluation.
     *             - args[3] A XPath expression.
     * @return a list for search results, append the error level from PASW Statistics as
     *         its last element.
     */
    PYINVOKESPSS_API PyObject * ext_EvaluateXPath( PyObject *self,
                                                   PyObject *args
                                                   );

    /**
     * Queue a line of a command and execute all of the currently
     * queued lines.
     * The processor needs to be started before calling this function.
     *
     * @param self The argument is only used when the C function implements a
     *             built-in method, not a function. It will always be a NULL
     *             pointer, when we are defining a function, not a method.
     * @param args A tuple of arguments.
     *             - args[0] the content of the line.
     *             - args[1] The length of the line in bytes.
     * @return the error level from PASW Statistics. 0 means success.
     */
    PYINVOKESPSS_API PyObject * ext_Submit( PyObject *self,
                                            PyObject *args
                                            );

    /**
     * Get the number of cases in the active data file.
     * The processor needs to be started before calling this function.
     *
     * @param self The argument is only used when the C function implements a
     *             built-in method, not a function. It will always be a NULL
     *             pointer, when we are defining a function, not a method.
     * @param args A empty tuple, or NULL.
     * @return a list. The first element is the number of cases in the active data
     *         file. This value will be -1 when value is unknown. The value will be
     *         unknown prior to the complete pass of data from a database or text
     *         file. The second one is the error level from PASW Statistics.
     */
    PYINVOKESPSS_API PyObject * ext_GetCaseCount( PyObject *self,
                                                  PyObject *args
                                                  );

    /**
     * Get the number of variables in the active data file.
     *
     * @param self The argument is only used when the C function implements a
     *             built-in method, not a function. It will always be a NULL
     *             pointer, when we are defining a function, not a method.
     * @param args A empty tuple, or NULL.
     * @return a list. The first element is the number of variables in the active
     *         data file. The second one is the error level from PASW Statistics.
     */
    PYINVOKESPSS_API PyObject * ext_GetVariableCount( PyObject *self,
                                                      PyObject *args
                                                      );

    /**
     * Get the format of a variable in the active data file.
     * The processor needs to be started before calling this function.
     *
     * @param self The argument is only used when the C function implements a
     *             built-in method, not a function. It will always be a NULL
     *             pointer, when we are defining a function, not a method.
     * @param args Variable index, starting from 0.
     * @return a list. The first element is the format of the variable in current
     *         data file. The second one is the error level from PASW Statistics.
     */
    PYINVOKESPSS_API PyObject * ext_GetVariableFormat( PyObject *self,
                                                       PyObject *args
                                                       );

    /**
     * Get the label of a variable in the active data file.
     * The processor needs to be started before calling this function.
     *
     * @param self The argument is only used when the C function implements a
     *             built-in method, not a function. It will always be a NULL
     *             pointer, when we are defining a function, not a method.
     * @param args Variable index, starting from 0.
     * @return a list. The first element is the label of the variable in current
     *         data file. The second one is the error level from PASW Statistics.
     */
    PYINVOKESPSS_API PyObject * ext_GetVariableLabel( PyObject *self,
                                                      PyObject *args
                                                      );

    /**
     * Get the measurementLevel of a variable in the active data file.
     * The processor needs to be started before calling this function.
     *
     * @param self The argument is only used when the C function implements a
     *             built-in method, not a function. It will always be a NULL
     *             pointer, when we are defining a function, not a method.
     * @param args Variable index, starting from 0.
     * @return a list. The first element is the measurement level of the variable
     *         in current data file, 0 = error, 1 = unknown, 2 = nominal,
     *         3 = ordinal, 4 = scale .
     *         The second element is the error level from PASW Statistics.
     */
    PYINVOKESPSS_API PyObject * ext_GetVariableMeasurementLevel( PyObject *self,
                                                                 PyObject *args
                                                                 );

    /**
     * Get the name of a variable in the active data file.
     * The processor needs to be started before calling this function.
     *
     * @param self The argument is only used when the C function implements a
     *             built-in method, not a function. It will always be a NULL
     *             pointer, when we are defining a function, not a method.
     * @param args Variable index, starting from 0.
     * @return a list. The first element is the name of the variable in current
     *         data file. The second one is the error level from PASW Statistics.
     */
    PYINVOKESPSS_API PyObject * ext_GetVariableName( PyObject *self,
                                                     PyObject *args
                                                     );

    /**
     * Get the type of a variable in the active data file
     * The processor needs to be started before calling this function.
     *
     * @param self The argument is only used when the C function implements a
     *             built-in method, not a function. It will always be a NULL
     *             pointer, when we are defining a function, not a method.
     * @param args Variable index, starting from 0.
     * @return a list. The first element is the type of the variable in current
     *         data file. 0 means a numeric variable; A value n, greater than zero,
     *         indicates a string variable with maximum length n bytes.
     *         The second element is the error level from PASW Statistics.
     */
    PYINVOKESPSS_API PyObject * ext_GetVariableType( PyObject *self,
                                                     PyObject *args
                                                     );

    /**
     * determine the mode of operation. If PASW Statistics is not
     * the main program, the mode is 'External', or 'XDriven'.
     *
     * @param self The argument is only used when the C function implements a
     *             built-in method, not a function. It will always be a NULL
     *             pointer, when we are defining a function, not a method.
     * @param args A empty tuple, or NULL.
     * @return 1 if Python is controlling the PASW Statistics backend ('External'), 0 if PASW Statistics is controlling the backend.
     */
    PYINVOKESPSS_API PyObject * ext_IsXDriven( PyObject *self,
                                               PyObject *args
                                               );

    /**
     * Post text messages to the PASW Statistics output log.
     * The processor needs to be started before calling this function.
     *
     * @param self The argument is only used when the C function implements a
     *             built-in method, not a function. It will always be a NULL
     *             pointer, when we are defining a function, not a method.
     * @param args A tuple of arguments.
     *             - args[0] The text to be posted.
     *             - args[1] The length of the text.
     * @return the error level from PASW Statistics. 0 means success.
     */
    PYINVOKESPSS_API PyObject * ext_PostSpssOutput( PyObject *self,
                                                    PyObject *args
                                                    );

    /**
     * Start the PASW Statistics processor.
     *
     * @param self The argument is only used when the C function implements a
     *             built-in method, not a function. It will always be a NULL
     *             pointer, when we are defining a function, not a method.
     * @param args The commandline to start spss processor, can be "".
     * @return the error level from PASW Statistics. 0 means success.
     */
    PYINVOKESPSS_API PyObject * ext_StartSpss( PyObject *self,
                                               PyObject *args
                                               );

    /**
     * Stop PASW Statistics processor
     *
     * @param self The argument is only used when the C function implements a
     *             built-in method, not a function. It will always be a NULL
     *             pointer, when we are defining a function, not a method.
     * @param args A empty tuple, or NULL.
     */
    PYINVOKESPSS_API PyObject * ext_StopSpss( PyObject *self,
                                              PyObject *args
                                              );

    /**
     * Check if the PASW Statistics processor is ready.
     *
     * @param self The argument is only used when the C function implements a
     *             built-in method, not a function. It will always be a NULL
     *             pointer, when we are defining a function, not a method.
     * @param args A empty tuple, or NULL.
     * @return if backend is ready, return 1; 0 otherwise.
     */
    PYINVOKESPSS_API PyObject * ext_IsBackendReady( PyObject *self,
                                                    PyObject *args
                                                    );

    /**
     * Retrieve an XML object from the XML workspace.
     * The processor needs to be started before calling this function.
     *
     * @param self The argument is only used when the C function implements a
     *             built-in method, not a function. It will always be a NULL
     *             pointer, when we are defining a function, not a method.
     * @param args A tuple of arguments.
     *             - args[0] A handle of an existing object in XML the workspace.
     *             - args[1] An output file.
     * @return a list. The first element is the XML representation of the requested
     *         object. The second one is the error level from PASW Statistics.
     */
    PYINVOKESPSS_API PyObject * ext_GetXmlUtf16( PyObject *self,
                                                 PyObject *args
                                                 );


     /**
     * Retrieve images from the XML workspace.
     * The processor needs to be started before calling this function.
     *
     * @param self The argument is only used when the C function implements a
     *             built-in method, not a function. It will always be a NULL
     *             pointer, when we are defining a function, not a method.
     * @param args A tuple of arguments.
     *             - args[0] A handle of an existing object in XML the workspace.
     *             - args[1] An image file name.
     * @return a tuple. The first element is the image representation of the requested
     *         object. The second is the size of the requested image.  
     *         The last one is the error level from PASW Statistics.
     *         
     */
    PYINVOKESPSS_API PyObject * ext_GetImage( PyObject *self,
                                                 PyObject *args
                                                 );
      /** 
      *Get the value of an options setting, as set from the SET command. 
      *
      *  @param self The argument is only used when the C function implements a
      *             built-in method, not a function. It will always be a NULL
      *             pointer, when we are defining a function, not a method.
      * @param args A tuple of arguments.
      *             - args[0]  setting  A string specifying the setting to retrieve. The possible values of setting 
      *                                   are the names of the SET subcommands,  in upper case¡ªfor example, 
      *                                    ¡°WORKSPACE¡±, ¡°OLANG¡±, ¡°XVERSION¡±.
      *             - args[1] option  An uppercase string specifying an option associated with the value of the setting 
      *                             argument. Currently, this only applies to the MIOUTPUT subcommand of SET,  
      *                           for which there is a separate setting for each of the keywords OBSERVED, IMPUTED, 
      *                            POOLED, and DIAGNOSTICS. When setting equals MIOUTPUT, option can be set to 
      *                             any of those four keywords to obtain the associated value of the keyword¡ªYes or  No¡±
      *                             By default, option is NULL.
      *             - args[2] errLevel take the error when to call this function.
      * @return a tuple. return Returns the value of the specified setting, as a string.
      */
    PYINVOKESPSS_API PyObject* ext_GetSetting(PyObject*self,
                                                PyObject* args
                                                );

    /** 
    *Return  a list of tags associated with any active OMS requests.
    *  @param self The argument is only used when the C function implements a
    *             built-in method, not a function. It will always be a NULL
    *             pointer, when we are defining a function, not a method.
    * @param args A tuple of arguments.
    *             - args[0]  errLevel   take the error when to call this function. 
    * @return A list of strings specifying the tags for any active OMS requests. 
    *              The structure of this list is the same as returned from the GetHandleList function, 
    *              so the functions GetStringListLength and GetStringFromList may be used to obtain 
    *              the string value of the handles. RemoveStringList can be used to free the storage
    *              relating to this list.
    */
    PYINVOKESPSS_API PyObject* ext_GetOMSTagList(PyObject *self, 
                                               PyObject *args
                                               );
    /**
     * Return a list of existing handles.
     * The processor needs to be started before calling this function.
     *
     * @param self The argument is only used when the C function implements a
     *             built-in method, not a function. It will always be a NULL
     *             pointer, when we are defining a function, not a method.
     * @param args A empty tuple, or NULL.
     * @return a list of all exiting handles of objects in the XML workspace.
     *         append the error level from PASW Statistics as its last element.
     */
    PYINVOKESPSS_API PyObject * ext_GetHandleList( PyObject *self,
                                                   PyObject *args
                                                   );

    PYINVOKESPSS_API PyObject * ext_GetFileHandles( PyObject *self,
                                                   PyObject *args
                                                   );

    PYINVOKESPSS_API PyObject *  ext_GetVarCMissingValues( PyObject *self,
                                                           PyObject *args
                                                           );

    PYINVOKESPSS_API PyObject *  ext_GetVarNMissingValues( PyObject *self,
                                                           PyObject *args
                                                           );

    PYINVOKESPSS_API PyObject *  ext_GetVarAttributeNames( PyObject *self,
                                                           PyObject *args
                                                           );

    PYINVOKESPSS_API PyObject *  ext_GetVarAttributes( PyObject *self,
                                                       PyObject *args
                                                       );

    PYINVOKESPSS_API PyObject *  ext_SetVarAttributes( PyObject *self,
                                                       PyObject *args
                                                       );

    PYINVOKESPSS_API PyObject * ext_GetColumnCountInProcDS( PyObject *self,
                                                            PyObject *args
                                                            );

    /**
     * Return a tuple or None.
     * fetch the next case from data source.
     * The processor needs to be started and has data source before calling
     *  this function.
     *
     * @param self The argument is only used when the C function implements a
     *             built-in method, not a function. It will always be a NULL
     *             pointer, when we are defining a function, not a method.
     * @param args datasrcInfo.
     * @return a single tuple or None when no more data is available.
     */
    PYINVOKESPSS_API PyObject * ext_fetchone( PyObject *self,
                                              PyObject *args);


    /**
     * Return a list of tupls or empty tuple.
     * fetch the next value of size cases from data source.
     * The processor needs to be started and has data source before calling
     *  this function.
     *
     * @param self The argument is only used when the C function implements a
     *             built-in method, not a function. It will always be a NULL
     *             pointer, when we are defining a function, not a method.
     * @param args datasrcInfo and the row size.
     * @return a list of tuple or Empty tuple when no more data is available.
     */
    PYINVOKESPSS_API PyObject * ext_fetchmany( PyObject *self,
                                               PyObject *args
                                               );

    /**
     * Return a list of tuples or empty tuple.
     * fetch all the remianing cases from data source.
     * The processor needs to be started and has data source before calling
     *  this function.
     *
     * @param self The argument is only used when the C function implements a
     *             built-in method, not a function. It will always be a NULL
     *             pointer, when we are defining a function, not a method.
     * @param args datasrcInfo.
     * @return a list of tuples or Empty tuple when no more data is available.
     */
    PYINVOKESPSS_API PyObject * ext_fetchall( PyObject *self,
                                              PyObject *args
                                              );

    /**
     * Return Py_None.
     * close the data source
     * The processor needs to be started and cursor object should be created
     * before calling this function.
     *
     * @param self The argument is only used when the C function implements a
     *             built-in method, not a function. It will always be a NULL
     *             pointer, when we are defining a function, not a method.
     * @param args datasrcInfo.
     * @return Py_None.
     */
    PYINVOKESPSS_API PyObject * ext_close( PyObject *self,
                                           PyObject *args
                                           );

    /**
     * Return datasrcInfo.
     * create the cursor object.
     * The processor needs to be started and has data source before calling
     * this function.
     *
     * @param self The argument is only used when the C function implements a
     *             built-in method, not a function. It will always be a NULL
     *             pointer, when we are defining a function, not a method.
     * @param args copy of conversions and varTuple.
     * @return datasrcInfo.
     */
    PYINVOKESPSS_API PyObject * ext_cursor( PyObject *self,
                                            PyObject *args
                                            );



    PYINVOKESPSS_API PyObject * ext_StartProcedure( PyObject *self,
                                                    PyObject *args
                                                    );

    PYINVOKESPSS_API PyObject * ext_SplitChange( PyObject *self,
                                                 PyObject *args
                                                 );


    PYINVOKESPSS_API PyObject * ext_EndProcedure( PyObject *self,
                                                  PyObject *args
                                                  );

    PYINVOKESPSS_API PyObject * ext_SimplePivotTable( PyObject *self,
                                                      PyObject *args
                                                      );

    PYINVOKESPSS_API PyObject * ext_StartPivotTable( PyObject *self,
                                                     PyObject *args
                                                     );

    PYINVOKESPSS_API PyObject * ext_HidePivotTableTitle( PyObject *self,
                                                         PyObject *args
                                                         );

    PYINVOKESPSS_API PyObject * ext_PivotTableCaption( PyObject *self,
                                                       PyObject *args
                                                       );

    PYINVOKESPSS_API PyObject * ext_AddDimension( PyObject *self,
                                                  PyObject *args
                                                  );



    PYINVOKESPSS_API PyObject * ext_SetFormatSpecCoefficient( PyObject *self,
                                                              PyObject *args
                                                              );
    PYINVOKESPSS_API PyObject * ext_SetFormatSpecCoefficientSE( PyObject *self,
                                                                PyObject *args
                                                                );
    PYINVOKESPSS_API PyObject * ext_SetFormatSpecCoefficientVar( PyObject *self,
                                                                 PyObject *args
                                                                 );
    PYINVOKESPSS_API PyObject * ext_SetFormatSpecCorrelation( PyObject *self,
                                                              PyObject *args
                                                              );
    PYINVOKESPSS_API PyObject * ext_SetFormatSpecGeneralStat( PyObject *self,
                                                              PyObject *args
                                                              );
    PYINVOKESPSS_API PyObject * ext_SetFormatSpecMean( PyObject *self,
                                                       PyObject *args
                                                       );
    PYINVOKESPSS_API PyObject * ext_SetFormatSpecCount( PyObject *self,
                                                        PyObject *args
                                                        );

    PYINVOKESPSS_API PyObject * ext_SetFormatSpecPercent( PyObject *self,
                                                          PyObject *args
                                                          );
    PYINVOKESPSS_API PyObject * ext_SetFormatSpecPercentNoSign( PyObject *self,
                                                 PyObject *args
                                                 );
    PYINVOKESPSS_API PyObject * ext_SetFormatSpecProportion( PyObject *self,
                                                 PyObject *args
                                                 );
    PYINVOKESPSS_API PyObject * ext_SetFormatSpecSignificance( PyObject *self,
                                                 PyObject *args
                                                 );
    PYINVOKESPSS_API PyObject * ext_SetFormatSpecResidual( PyObject *self,
                                                 PyObject *args
                                                 );
    PYINVOKESPSS_API PyObject * ext_SetFormatSpecVariable( PyObject *self,
                                                 PyObject *args
                                                 );
    PYINVOKESPSS_API PyObject * ext_SetFormatSpecStdDev( PyObject *self,
                                                 PyObject *args
                                                 );
    PYINVOKESPSS_API PyObject * ext_SetFormatSpecDifference( PyObject *self,
                                                 PyObject *args
                                                 );
    PYINVOKESPSS_API PyObject * ext_SetFormatSpecSum( PyObject *self,
                                                 PyObject *args
                                                 );
    PYINVOKESPSS_API PyObject * ext_AddNumberCategory( PyObject *self,
                                                 PyObject *args
                                                 );
    PYINVOKESPSS_API PyObject * ext_AddStringCategory( PyObject *self,
                                                 PyObject *args
                                                 );
    PYINVOKESPSS_API PyObject * ext_AddVarNameCategory( PyObject *self,
                                                 PyObject *args
                                                 );
    PYINVOKESPSS_API PyObject * ext_AddVarValueDoubleCategory( PyObject *self,
                                                 PyObject *args
                                                 );
    PYINVOKESPSS_API PyObject * ext_AddVarValueStringCategory( PyObject *self,
                                                 PyObject *args
                                                 );
    PYINVOKESPSS_API PyObject * ext_SetNumberCell( PyObject *self,
                                                 PyObject *args
                                                 );
    PYINVOKESPSS_API PyObject * ext_SetStringCell( PyObject *self,
                                                 PyObject *args
                                                 );
    PYINVOKESPSS_API PyObject * ext_SetVarNameCell( PyObject *self,
                                                 PyObject *args
                                                 );
    PYINVOKESPSS_API PyObject * ext_SetVarValueDoubleCell( PyObject *self,
                                                 PyObject *args
                                                 );
    PYINVOKESPSS_API PyObject * ext_SetVarValueStringCell( PyObject *self,
                                                 PyObject *args
                                                 );




    PYINVOKESPSS_API PyObject * ext_AddCellFootnotes(  PyObject *self,
                                                       PyObject *args
                                                    );

    PYINVOKESPSS_API PyObject * ext_AddProcFootnotes(  PyObject *self,
                                                       PyObject *args
                                                    );
    PYINVOKESPSS_API PyObject * ext_AddOutlineFootnotes(  PyObject *self,
                                                       PyObject *args
                                                    );
    PYINVOKESPSS_API PyObject * ext_AddTitleFootnotes(  PyObject *self,
                                                       PyObject *args
                                                    );
    PYINVOKESPSS_API PyObject * ext_AddDimFootnotes(  PyObject *self,
                                                       PyObject *args
                                                    );
    PYINVOKESPSS_API PyObject * ext_AddCategoryFootnotes(  PyObject *self,
                                                       PyObject *args
                                                    );



    PYINVOKESPSS_API PyObject * ext_AddTextBlock( PyObject *self,
                                                  PyObject *args
                                                 );



    /**
     * This function creates a new variable named varName, which will be either numeric or
     * string based on varLength. If the latter is zero, a numeric variable with a default format
     * of F8.2 will be created; if it is greater than 0 and less than or equal to 32767, a string
     * variable with length varLength will be created; any other value will be rejected as
     * invalid.
     *
     * @param self The argument is only used when the C function implements a
     *             built-in method, not a function. It will always be a NULL
     *             pointer, when we are defining a function, not a method.
     * @param args argument array, it includes the below item:
     *             (1) Handle to the data file.
     *                 Type: int
     *             (2) Variable name.
     *                 Type: string
     *             (3) varLength(Type and size of the variable).
     *                 Type: int
     *
     * @return     error code. 0 means success, the other means error.
     */
    PYINVOKESPSS_API PyObject *  ext_SetVarName( PyObject *self,
                                                 PyObject *args
                                                 );


    /**
     * This function sets the label of a variable.
     *
     * @param self The argument is only used when the C function implements a
     *             built-in method, not a function. It will always be a NULL
     *             pointer, when we are defining a function, not a method.
     * @param args argument array, it includes the below item:
     *             (1) Handle to the data file.
     *                 Type: int
     *             (2) Variable name.
     *                 Type: string
     *             (3) Variable label.
     *                 Type: string
     *
     * @return     errro code. 0 means success. The othere means error.
     */
    PYINVOKESPSS_API PyObject *  ext_SetVarLabel( PyObject *self,
                                                  PyObject *args
                                                  );

    /**
     * Write the data dictionary to the data file.
     *
     * @param self The argument is only used when the C function implements a
     *             built-in method, not a function. It will always be a NULL
     *             pointer, when we are defining a function, not a method.
     * @param args None
     *
     * @return     errro code. 0 means success. The othere means error.
     */
    PYINVOKESPSS_API PyObject *  ext_CommitHeader( PyObject *self,
                                                   PyObject *args
                                                   );


    /**
     * set variable value label for string type varaiable.
     * @param self The argument is only used when the C function implements a
     *             built-in method, not a function. It will always be a NULL
     *             pointer, when we are defining a function, not a method.
     * @parm args  argument array, it includes the below item:
     *             (1) varName variable name with null-terminated.
     *                 Type: string
     *             (2) value to be labeled with null-terminated.
     *                 Type: string
     *             (3) label value with null-terminated.
     *                 Type: string
     * @return     errro code. 0 means success. The othere means error.
     */
    PYINVOKESPSS_API PyObject * ext_SetVarCValueLabel( PyObject *self,
                                                       PyObject *args
                                                       );

    /**
     * Changes or adds a value label for the specified value of a numeric
     * variable. The label should be a null-terminated string not exceeding 60
     * @param self The argument is only used when the C function implements a
     *             built-in method, not a function. It will always be a NULL
     *             pointer, when we are defining a function, not a method.
     * @parm args  argument array, it includes the below item:
     *             (1) varName variable name with null-terminated.
     *                 Type: string
     *             (2) value to be labeled
     *                 Type: double
     *             (3) label value with null-terminated.
     *                 Type: string
     * @return     errro code. 0 means success. The othere means error.
     *
     */
    PYINVOKESPSS_API PyObject * ext_SetVarNValueLabel( PyObject *self,
                                                       PyObject *args
                                                       );

    /**
     * Set missing values for a short string variable. The argument missingFormat
     * must be set to a value in the range 0-3 to indicate the number of missing
     * values supplied. When fewer than three missing values are to be defined, the
     * redundant arguments must still be present, although their values are not
     * inspected. For example, if missingFormat is 2, missingVal3 is unused. The
     * supplied missing values must be nullterminated and not longer than the length
     * of the variable unless the excess length is made up of blanks,which are ignored.
     * If the missing value is shorter than the length of the variable, trailing blanks
     * are assumed.
     *
     * @parm self The argument is only used when the C function implements a
     *             built-in method, not a function. It will always be a NULL
     *             pointer, when we are defining a function, not a method.
     * @parm args  argument array, it includes the below item:
     *             (1) varName variable name with null-terminated.
     *                 Type: string
     *             (2) missingFormat Missing format code.
     *                 Type: int
     *             (3) missingVal1 First missing value.
     *                 Type: string
     *             (4) missingVal2 Second missing value.
     *                 Type: string
     *             (5) missingVal3 Third missing value.
     *                 Type: string
     * @return     errro code. 0 means success. The othere means error.
     *
     */
    PYINVOKESPSS_API PyObject * ext_SetVarCMissingValues( PyObject *self,
                                                          PyObject *args
                                                          );

    /**
     * Sets missing values for a numeric variable. The interpretation of the
     * arguments missingVal1, missingVal2, and missingVal3 depends on the value of
     * missingFormat. If missingFormat is set to -2, missingVal1 and missingVal2
     * are taken as the upper and lower limits, respectively, of the range, and
     * missingVal3 is ignored. If missingFormat is -3, missingval1 and missingVal2
     * are taken as limits of the range and missingVal3 is taken as the discrete
     * missing value. If missingFormat is neither of the above, it must be in the range 0-3,
     * indicating the number of discrete missing values present. For example, if
     * missingFormat is 2, missingVal1 and missingVal2 are taken as two discrete missing
     * values and missingVal3 is ignored.
     *
     * @parm self The argument is only used when the C function implements a
     *             built-in method, not a function. It will always be a NULL
     *             pointer, when we are defining a function, not a method.
     * @parm args  argument array, it includes the below item:
     *             (1) varName variable name with null-terminated.
     *                 Type: string
     *             (2) missingFormat Missing format code.
     *                 Type: int
     *             (3) missingVal1 First missing value.
     *                 Type: double
     *             (4) missingVal2 Second missing value.
     *                 Type: double
     *             (5) missingVal3 Third missing value.
     *                 Type: double
     * @return     errro code. 0 means success. The othere means error.
     */
    PYINVOKESPSS_API PyObject * ext_SetVarNMissingValues( PyObject *self,
                                                          PyObject *args
                                                          );
    /**
     * Set the value of the measurement level attribute of a variable.
     *
     * @parm self The argument is only used when the C function implements a
     *             built-in method, not a function. It will always be a NULL
     *             pointer, when we are defining a function, not a method.
     * @parm args  argument array, it includes the below item:
     *             (1) varName variable name with null-terminated.
     *                 Type: string
     *             (2) measureLevel measurement level.
     *                 Type: int
     * @return     errro code. 0 means success. The othere means error.
     */
    PYINVOKESPSS_API PyObject * ext_SetVarMeasureLevel( PyObject *self,
                                                        PyObject *args
                                                        );

    /**
     * Set the value of the alignment attribute of a variable.
     *
     * @parm self The argument is only used when the C function implements a
     *             built-in method, not a function. It will always be a NULL
     *             pointer, when we are defining a function, not a method.
     * @parm args  argument array, it includes the below item:
     *             (1) varName variable name with null-terminated.
     *                 Type: string
     *             (2) alignment alignment mode.
     *                 Type: int
     * @return     errro code. 0 means success. The othere means error.
     */
    PYINVOKESPSS_API PyObject * ext_SetVarAlignment( PyObject *self,
                                                     PyObject *args
                                                     );

    /**
     * Set the write format of a variable.
     *
     * @parm self The argument is only used when the C function implements a
     *             built-in method, not a function. It will always be a NULL
     *             pointer, when we are defining a function, not a method.
     * @parm args  argument array, it includes the below item:
     *             (1) varName variable name with null-terminated.
     *                 Type: string
     *             (2) type format type code.
     *                 Type: int
     *             (3) formatWidth format width.
     *                 Type: int
     *             (4) formatDecimal number of digits after the decimal.
     *                 Type: int
     * @return     errro code. 0 means success. The othere means error.
   */
    PYINVOKESPSS_API PyObject * ext_SetVarFormat( PyObject *self,
                                                  PyObject *args
                                                  );

    /**
     * Set the value of a string variable for the current case. The current case is
     * not written out to the data file until CommitCaseRecord is called.
     *
     * @parm self The argument is only used when the C function implements a
     *             built-in method, not a function. It will always be a NULL
     *             pointer, when we are defining a function, not a method.
     * @parm args  argument array, it includes the below item:
     *             (1) varName variable name with null-terminated.
     *                 Type: string
     *             (2) value varibale's valule without null-terminated.
     *                 Type: string
     *
     * @return     errro code. 0 means success. The othere means error.
     */
    PYINVOKESPSS_API PyObject * ext_SetValueChar( PyObject *self,
                                                  PyObject *args
                                                  );

    /**
     * Set the value of a numeric variable for the current case. The current case
     * is not written out to the data file until CommitCaseRecord is called.
     *
     * @parm self The argument is only used when the C function implements a
     *             built-in method, not a function. It will always be a NULL
     *             pointer, when we are defining a function, not a method.
     * @parm args  argument array, it includes the below item:
     *             (1) varName variable name with null-terminated.
     *                 Type: string
     *             (2) variable's value
     *                 Type: double
     *
     * @return     errro code. 0 means success. The othere means error.
     */
    PYINVOKESPSS_API PyObject *  ext_SetValueNumeric( PyObject *self,
                                                      PyObject *args
                                                      );

    /**
     * Write a case to the active data source, just for the WriteCursor.
     *
     * @parm self The argument is only used when the C function implements a
     *             built-in method, not a function. It will always be a NULL
     *             pointer, when we are defining a function, not a method.
     * @parm args  None
     *
     * @return     errro code. 0 means success. The othere means error.
     */
    PYINVOKESPSS_API PyObject *  ext_CommitCaseRecord( PyObject *self,
                                                       PyObject *args
                                                       );
    /**
     * Write a case to the active data source,just for the AppendCursor.
     *
     * @parm self The argument is only used when the C function implements a
     *             built-in method, not a function. It will always be a NULL
     *             pointer, when we are defining a function, not a method.
     * @parm args  None
     *
     * @return     errro code. 0 means success. The othere means error.
     */
    PYINVOKESPSS_API PyObject *  ext_CommitNewCase( PyObject *self,
                                                    PyObject *args
                                                    );
       
    /**
     * Write many cases to the active data source,just for the WriteCursor.
     *
     * @parm self The argument is only used when the C function implements a
     *             built-in method, not a function. It will always be a NULL
     *             pointer, when we are defining a function, not a method.
     * @parm args  argument array, it includes the below item:
     *             (1) newVarList: variable name that will be set new data
     *             (2) caseData: all data
     *             (3) newVarListType: variable type for each newVarList
     *
     * @return     errro code. 0 means success. The othere means error.
     */                                             
    PYINVOKESPSS_API PyObject *  ext_CommitManyCases( PyObject *self,
                                                     PyObject *args
                                                     );                                                    

    /**
     * After append new cases, call the function.
     *
     * @parm self The argument is only used when the C function implements a
     *             built-in method, not a function. It will always be a NULL
     *             pointer, when we are defining a function, not a method.
     * @parm args  None
     *
     * @return     errro code. 0 means success. The othere means error.
     */
    PYINVOKESPSS_API PyObject *  ext_EndChanges( PyObject *self,
                                                 PyObject *args
                                                );

    /**
     * check the backend whether has a running  cursor. o means no cursor,
     * and 1 means there has a cursor.
     *
     * @parm self The argument is only used when the C function implements a
     *             built-in method, not a function. It will always be a NULL
     *             pointer, when we are defining a function, not a method.
     * @parm args  None
     *
     * @return     errro code. 0 means success. The othere means error.
     */
    PYINVOKESPSS_API PyObject *  ext_HasCursor( PyObject *self,
                                                PyObject *args
                                                );

   /**
    * check whether does the split end or not? 0 means no end. 1 means end.
    *
    * @parm self The argument is only used when the C function implements a
    *              built-in method, not a function. It will always be a NULL
    *              pointer, when we are defining a function, not a method.
    * @parm args  None
    *
    * @return     errro code. 0 means success. The othere means error.
    */
    PYINVOKESPSS_API PyObject *  ext_IsEndSplit( PyObject *self,
                                                 PyObject *args
                                                 );

    /**
     * Get the number of cases in the procedure data source.
     * The processor needs to be started before calling this function.
     *
     * @param self The argument is only used when the C function implements a
     *             built-in method, not a function. It will always be a NULL
     *             pointer, when we are defining a function, not a method.
     * @param args A empty tuple, or NULL.
     * @return a list. The first element is the number of cases in the active data
     *         file. This value will be -1 when value is unknown. The value will be
     *         unknown prior to the complete pass of data from a database or text
     *         file. The second one is the error level from PASW Statistics.
     */
    PYINVOKESPSS_API PyObject * ext_GetCaseCountInProcDS( PyObject *self,
                                                          PyObject *args
                                                          );

    /**
     * Get the number of variables in the procedrue data source.
     *
     * @param self The argument is only used when the C function implements a
     *             built-in method, not a function. It will always be a NULL
     *             pointer, when we are defining a function, not a method.
     * @param args A empty tuple, or NULL.
     * @return a list. The first element is the number of variables in the active
     *         data file. The second one is the error level from PASW Statistics.
     */
    PYINVOKESPSS_API PyObject * ext_GetVariableCountInProcDS( PyObject *self,
                                                              PyObject *args
                                                              );

    /**
     * Get the format of a variable in the procedure data source.
     * The processor needs to be started before calling this function.
     *
     * @param self The argument is only used when the C function implements a
     *             built-in method, not a function. It will always be a NULL
     *             pointer, when we are defining a function, not a method.
     * @param args Variable index, starting from 0.
     * @return a list. The first element is the format of the variable in current
     *         data file. The second one is the error level from PASW Statistics.
     */
    PYINVOKESPSS_API PyObject * ext_GetVariableFormatInProcDS( PyObject *self,
                                                               PyObject *args
                                                               );

    /**
     * Get the label of a variable in the procedure data source.
     * The processor needs to be started before calling this function.
     *
     * @param self The argument is only used when the C function implements a
     *             built-in method, not a function. It will always be a NULL
     *             pointer, when we are defining a function, not a method.
     * @param args Variable index, starting from 0.
     * @return a list. The first element is the label of the variable in current
     *         data file. The second one is the error level from PASW Statistics.
     */
    PYINVOKESPSS_API PyObject * ext_GetVariableLabelInProcDS( PyObject *self,
                                                              PyObject *args
                                                              );

    /**
     * Get the measurementLevel of a variable in the procedrue data source.
     * The processor needs to be started before calling this function.
     *
     * @param self The argument is only used when the C function implements a
     *             built-in method, not a function. It will always be a NULL
     *             pointer, when we are defining a function, not a method.
     * @param args Variable index, starting from 0.
     * @return a list. The first element is the measurement level of the variable
     *         in current data file, 0 = error, 1 = unknown, 2 = nominal,
     *         3 = ordinal, 4 = scale .
     *         The second element is the error level from PASW Statistics.
     */
    PYINVOKESPSS_API PyObject * ext_GetVariableMeasurementLevelInProcDS( PyObject *self,
                                                                         PyObject *args
                                                                         );

    /**
     * Get the name of a variable in the procedure data source.
     * The processor needs to be started before calling this function.
     *
     * @param self The argument is only used when the C function implements a
     *             built-in method, not a function. It will always be a NULL
     *             pointer, when we are defining a function, not a method.
     * @param args Variable index, starting from 0.
     * @return a list. The first element is the name of the variable in current
     *         data file. The second one is the error level from PASW Statistics.
     */
    PYINVOKESPSS_API PyObject * ext_GetVariableNameInProcDS( PyObject *self,
                                                             PyObject *args
                                                             );

    /**
     * Get the type of a variable in the procedure data source.
     * The processor needs to be started before calling this function.
     *
     * @param self The argument is only used when the C function implements a
     *             built-in method, not a function. It will always be a NULL
     *             pointer, when we are defining a function, not a method.
     * @param args Variable index, starting from 0.
     * @return a list. The first element is the type of the variable in current
     *         data file. 0 means a numeric variable; A value n, greater than zero,
     *         indicates a string variable with maximum length n bytes.
     *         The second element is the error level from PASW Statistics.
     */
    PYINVOKESPSS_API PyObject * ext_GetVariableTypeInProcDS( PyObject *self,
                                                             PyObject *args
                                                             );

    PYINVOKESPSS_API PyObject *  ext_GetVarCMissingValuesInProcDS( PyObject *self,
                                                                   PyObject *args
                                                                   );

    PYINVOKESPSS_API PyObject *  ext_GetVarNMissingValuesInProcDS( PyObject *self,
                                                                   PyObject *args
                                                                   );

    PYINVOKESPSS_API PyObject *  ext_GetVarAttributeNamesInProcDS( PyObject *self,
                                                                   PyObject *args
                                                                   );

    PYINVOKESPSS_API PyObject *  ext_GetVarAttributesInProcDS( PyObject *self,
                                                               PyObject *args
                                                               );

    PYINVOKESPSS_API PyObject *  ext_SetVarAttributesInProcDS( PyObject *self,
                                                               PyObject *args
                                                               );

    PYINVOKESPSS_API PyObject *  ext_SetUserMissingInclude( PyObject *self,
                                                            PyObject *args
                                                            );

    PYINVOKESPSS_API PyObject *  ext_HasProcedure( PyObject *self,
                                                   PyObject *args
                                                   );

    PYINVOKESPSS_API PyObject *  ext_GetSPSSLowHigh( PyObject *self,
                                                     PyObject *args
                                                     );

    PYINVOKESPSS_API PyObject *  ext_GetWeightVar( PyObject *self,
                                                   PyObject *args
                                                   );

    PYINVOKESPSS_API PyObject *  ext_ResetDataPass( PyObject *self,
                                                    PyObject *args
                                                    );

    PYINVOKESPSS_API PyObject *  ext_ResetDataPassWrite( PyObject *self,
                                                         PyObject *args
                                                         );
    
    PYINVOKESPSS_API PyObject *  ext_AllocNewVarsBuffer( PyObject *self,
                                                         PyObject *args
                                                         );
    
    PYINVOKESPSS_API PyObject *  ext_SetOneVarNameAndType( PyObject *self,
                                                           PyObject *args
                                                           );
    PYINVOKESPSS_API PyObject *  ext_SetFetchVarList( PyObject *self,
                                                      PyObject *args
                                                      );

    PYINVOKESPSS_API PyObject *  ext_SetXDriveMode( PyObject *self,
                                                    PyObject *args
                                                    );
    
    // data step                                                  
    PYINVOKESPSS_API PyObject * ext_StartDataStep(
                                                      PyObject *self,
                                                      PyObject *args);
        
        PYINVOKESPSS_API PyObject * ext_EndDataStep(
                                                      PyObject *self,
                                                      PyObject *args);
        
        PYINVOKESPSS_API PyObject * ext_CreateDataset(
                                                      PyObject *self,
                                                      PyObject *args);
        
        PYINVOKESPSS_API PyObject * ext_SetDatasetName(
                                                      PyObject *self,
                                                      PyObject *args);
        
        PYINVOKESPSS_API PyObject * ext_GetNewDatasetName(
                                                      PyObject *self,
                                                      PyObject *args);
                                                                                                                                
        PYINVOKESPSS_API PyObject * ext_GetActive(
                                                      PyObject *self,
                                                      PyObject *args);
        
        PYINVOKESPSS_API PyObject * ext_SetActive(
                                                      PyObject *self,
                                                      PyObject *args);
        
        PYINVOKESPSS_API PyObject * ext_CopyDataset( 
                                                      PyObject *self,
                                                      PyObject *args);
                                                
        PYINVOKESPSS_API PyObject * ext_GetSpssDatasets(
                                                      PyObject *self,
                                                      PyObject *args);
                                                      
        PYINVOKESPSS_API PyObject * ext_GetDatastepDatasets(
                                                      PyObject *self,
                                                      PyObject *args);
        
        PYINVOKESPSS_API PyObject * ext_CloseDataset(
                                                      PyObject *self,
                                                      PyObject *args);
        
        PYINVOKESPSS_API PyObject * ext_InsertVariable(
                                                      PyObject *self,
                                                      PyObject *args);
                                                        
        PYINVOKESPSS_API PyObject * ext_DeleteVariable(
                                                      PyObject *self,
                                                      PyObject *args);
        
        PYINVOKESPSS_API PyObject * ext_GetVarCountInDS(
                                                      PyObject *self,
                                                      PyObject *args);
        
        PYINVOKESPSS_API PyObject * ext_GetVarNameInDS(
                                                      PyObject *self,
                                                      PyObject *args);
                                                                  
        PYINVOKESPSS_API PyObject * ext_SetVarNameInDS(
                                                      PyObject *self,
                                                      PyObject *args);
                                                  
        PYINVOKESPSS_API PyObject * ext_GetVarLabelInDS(
                                                      PyObject *self,
                                                      PyObject *args);
                                                                  
        PYINVOKESPSS_API PyObject * ext_SetVarLabelInDS(
                                                      PyObject *self,
                                                      PyObject *args);
                                                  
        PYINVOKESPSS_API PyObject * ext_GetVarTypeInDS(
                                                      PyObject *self,
                                                      PyObject *args);
                                                                  
        PYINVOKESPSS_API PyObject * ext_SetVarTypeInDS(
                                                      PyObject *self,
                                                      PyObject *args);
                                                          
        PYINVOKESPSS_API PyObject * ext_GetVarFormatInDS(
                                                      PyObject *self,
                                                      PyObject *args);
                                                                          
        PYINVOKESPSS_API PyObject * ext_SetVarFormatInDS(
                                                      PyObject *self,
                                                      PyObject *args);
                                                                  
        PYINVOKESPSS_API PyObject * ext_GetVarAlignmentInDS(
                                                      PyObject *self,
                                                      PyObject *args);
                                                          
        PYINVOKESPSS_API PyObject * ext_SetVarAlignmentInDS(
                                                      PyObject *self,
                                                      PyObject *args);
                                                                  
        PYINVOKESPSS_API PyObject * ext_GetVarMeasurementLevelInDS(
                                                      PyObject *self,
                                                      PyObject *args);
                                                                                          
        PYINVOKESPSS_API PyObject * ext_SetVarMeasurementLevelInDS(
                                                      PyObject *self,
                                                      PyObject *args);
                                                                          
        PYINVOKESPSS_API PyObject * ext_GetVarNMissingValuesInDS(
                                                      PyObject *self,
                                                      PyObject *args);
                                                                          
        PYINVOKESPSS_API PyObject * ext_GetVarCMissingValuesInDS(
                                                      PyObject *self,
                                                      PyObject *args);
                                                                          
        PYINVOKESPSS_API PyObject * ext_SetVarNMissingValuesInDS(
                                                      PyObject *self,
                                                      PyObject *args);
                                                                        
        PYINVOKESPSS_API PyObject * ext_SetVarCMissingValuesInDS(
                                                      PyObject *self,
                                                      PyObject *args);
                                                                        
        PYINVOKESPSS_API PyObject * ext_GetVarAttributesNameInDS(
                                                      PyObject *self,
                                                      PyObject *args);
                                                                        
        PYINVOKESPSS_API PyObject * ext_GetVarAttributesInDS(
                                                      PyObject *self,
                                                      PyObject *args);
                                                                        
        PYINVOKESPSS_API PyObject * ext_SetVarAttributesInDS(
                                                      PyObject *self,
                                                      PyObject *args);
                                                                        
        PYINVOKESPSS_API PyObject * ext_DelVarAttributesInDS(
                                                      PyObject *self,
                                                      PyObject *args);
                                                                        
        PYINVOKESPSS_API PyObject * ext_GetVarNValueLabelInDS(
                                                      PyObject *self,
                                                      PyObject *args);
                                
        PYINVOKESPSS_API PyObject * ext_GetVarCValueLabelInDS(
                                                      PyObject *self,
                                                      PyObject *args);
                                                                                                                  
        PYINVOKESPSS_API PyObject * ext_SetVarNValueLabelInDS(
                                                      PyObject *self,
                                                      PyObject *args);
                                                                        
        PYINVOKESPSS_API PyObject * ext_SetVarNValueLabelsInDS(
                                                      PyObject *self,
                                                      PyObject *args);
                                                                        
        PYINVOKESPSS_API PyObject * ext_SetVarCValueLabelInDS(
                                                      PyObject *self,
                                                      PyObject *args);
                                                                        
        PYINVOKESPSS_API PyObject * ext_SetVarCValueLabelsInDS(
                                                      PyObject *self,
                                                      PyObject *args);
                                                                        
        PYINVOKESPSS_API PyObject * ext_DelVarValueLabelInDS(
                                                      PyObject *self,
                                                      PyObject *args);

        PYINVOKESPSS_API PyObject * ext_DelVarNValueLabelInDS(
                                                      PyObject *self,
                                                      PyObject *args);

        PYINVOKESPSS_API PyObject * ext_DelVarCValueLabelInDS(
                                                      PyObject *self,
                                                      PyObject *args);
                                                                                                                                                                        
        PYINVOKESPSS_API PyObject * ext_InsertCase(
                                                      PyObject *self,
                                                      PyObject *args);
                                                        
        PYINVOKESPSS_API PyObject * ext_DeleteCase(
                                                      PyObject *self,
                                                      PyObject *args);
                                                        
        PYINVOKESPSS_API PyObject * ext_GetCaseCountInDS(
                                                      PyObject *self,
                                                      PyObject *args);
                                                        
        PYINVOKESPSS_API PyObject * ext_GetNCellValue(
                                                      PyObject *self,
                                                      PyObject *args);
                                                        
        PYINVOKESPSS_API PyObject * ext_GetCCellValue(
                                                      PyObject *self,
                                                      PyObject *args);
                                                      
        PYINVOKESPSS_API PyObject * ext_GetCellsValue(
                                                      PyObject *self,
                                                      PyObject *args);
                                                        
        PYINVOKESPSS_API PyObject * ext_SetNCellValue(
                                                      PyObject *self,
                                                      PyObject *args);
                                                        
        PYINVOKESPSS_API PyObject * ext_SetCCellValue(
                                                      PyObject *self,
                                                      PyObject *args);
                                                      
        PYINVOKESPSS_API PyObject * ext_IsUTF8mode(PyObject *self,
                                                   PyObject *args);                                              

        PYINVOKESPSS_API PyObject * ext_GetSplitVariableNames(PyObject *self,
                                                              PyObject *args);

        PYINVOKESPSS_API PyObject * ext_GetDataFileAttributes(PyObject *self,
                                                              PyObject *args);

        PYINVOKESPSS_API PyObject * ext_GetDataFileAttributeNames(PyObject *self,
                                                                  PyObject *args);

        PYINVOKESPSS_API PyObject * ext_GetDataFileAttributesInProcDS(PyObject *self,
                                                                      PyObject *args);

        PYINVOKESPSS_API PyObject * ext_GetDataFileAttributeNamesInProcDS(PyObject *self,
                                                                          PyObject *args);
    
        PYINVOKESPSS_API PyObject * ext_GetMultiResponseSetNames(PyObject *self,
                                                                 PyObject *args);

        PYINVOKESPSS_API PyObject * ext_GetMultiResponseSet(PyObject *self,
                                                            PyObject *args);

        PYINVOKESPSS_API PyObject * ext_GetMultiResponseSetNamesInProcDS(PyObject *self,
                                                                         PyObject *args);

        PYINVOKESPSS_API PyObject * ext_GetMultiResponseSetInProcDS(PyObject *self,
                                                                    PyObject *args);
    
        PYINVOKESPSS_API PyObject * ext_SetDataFileAttributesInDS(PyObject *self,
                                                                  PyObject *args);

        PYINVOKESPSS_API PyObject * ext_SetMultiResponseSetInDS(PyObject *self,
                                                                PyObject *args);
                                                                
        PYINVOKESPSS_API PyObject * ext_GetDataFileAttributeNamesInDS(PyObject *self,
                                                                      PyObject *args);

        PYINVOKESPSS_API PyObject * ext_GetDataFileAttributesInDS(PyObject *self,
                                                                  PyObject *args);

        PYINVOKESPSS_API PyObject * ext_GetMultiResponseSetNamesInDS(PyObject *self,
                                                                     PyObject *args);

        PYINVOKESPSS_API PyObject * ext_GetMultiResponseSetInDS(PyObject *self,
                                                                PyObject *args);
                                                                    
        PYINVOKESPSS_API PyObject * ext_GetVarColumnWidthInDS(PyObject *self,
                                                              PyObject *args);
    
        PYINVOKESPSS_API PyObject * ext_SetVarColumnWidthInDS(PyObject *self,
                                                              PyObject *args);
                                                                    
        PYINVOKESPSS_API PyObject * ext_SaveFileInDS(PyObject *self,
                                                     PyObject *args);                                                            
        
        PYINVOKESPSS_API PyObject * ext_DelDataFileAttributesInDS(PyObject *self,
                                                                  PyObject *args);
                                                                    
        PYINVOKESPSS_API PyObject * ext_DelMultiResponseSetInDS(PyObject *self,
                                                                PyObject *args);    

        PYINVOKESPSS_API PyObject * ext_GetXDriveMode(PyObject *self,
                                                      PyObject *args);    

        PYINVOKESPSS_API PyObject * ext_GetSPSSLocale(
                                                        PyObject *self,
                                                        PyObject *args);
                                                        
        PYINVOKESPSS_API PyObject * ext_GetCLocale(
                                                    PyObject *self,
                                                    PyObject *args);
                                                        
        PYINVOKESPSS_API PyObject * ext_SetOutputLanguage(
                                                        PyObject *self,
                                                        PyObject *args);
                                                        
        PYINVOKESPSS_API PyObject * ext_set_callback(PyObject *dummy, PyObject *args);
        
        PYINVOKESPSS_API PyObject * ext_GetSystemMissingValue(
                                                        PyObject *self,
                                                        PyObject *args);
                                                        
        PYINVOKESPSS_API PyObject * ext_GetNestDepth(
                                                        PyObject *self,
                                                        PyObject *args);
                                                        
        PYINVOKESPSS_API PyObject * ext_GetOutputLanguage(
                                                        PyObject *self,
                                                        PyObject *args);
                                                        
        PYINVOKESPSS_API PyObject * ext_TransCode(
                                                        PyObject *self,
                                                        PyObject *args);
                                                        
        PYINVOKESPSS_API PyObject * ext_GetCellsValueCache(
                                                        PyObject *self,
                                                        PyObject *args);

        PYINVOKESPSS_API PyObject * ext_SetDefaultEncoding(
                                                        PyObject *self,
                                                        PyObject *args);
                                                        
        PYINVOKESPSS_API PyObject * ext_GetVariableRole(
                                                        PyObject *self,
                                                        PyObject *args);
                                                        
        PYINVOKESPSS_API PyObject * ext_GetVariableRoleInProcDS(
                                                        PyObject *self,
                                                        PyObject *args);
                                                        
        PYINVOKESPSS_API PyObject * ext_SetVarRole(
                                                        PyObject *self,
                                                        PyObject *args);
                                                        
        PYINVOKESPSS_API PyObject * ext_GetVarRoleInDS(
                                                        PyObject *self,
                                                        PyObject *args);
                                                        
        PYINVOKESPSS_API PyObject * ext_SetVarRoleInDS(
                                                        PyObject *self,
                                                        PyObject *args);
                                                        
        PYINVOKESPSS_API PyObject * ext_TransportData(
                                                       PyObject *self,
                                                       PyObject *args);
                                                       
        PYINVOKESPSS_API PyObject * ext_NextCase(
                                                       PyObject *self,
                                                       PyObject *args);
                                                       
        PYINVOKESPSS_API PyObject * ext_GetDataFromTempFile(
                                                       PyObject *self,
                                                       PyObject *args);
                                                       
        PYINVOKESPSS_API PyObject * ext_SaveDataToTempFile(
                                                       PyObject *self,
                                                       PyObject *args);
        PYINVOKESPSS_API PyObject * ext_GetSplitEndIndex(
                                                       PyObject *self,
                                                       PyObject *args);
                                                       
        PYINVOKESPSS_API PyObject * ext_SetMode(
                                                       PyObject *self,
                                                       PyObject *args);
        PYINVOKESPSS_API PyObject * ext_GetRowCountInTempFile(
                                                       PyObject *self,
                                                       PyObject *args);
        PYINVOKESPSS_API PyObject * ext_GetCaseValue(
                                                        PyObject *self,
                                                        PyObject *args);
        PYINVOKESPSS_API PyObject * ext_SetCaseValue(
                                                        PyObject *self,
                                                        PyObject *args);
        PYINVOKESPSS_API PyObject * ext_SetCacheInDS(
                                                        PyObject *self,
                                                        PyObject *args);
                                                        
        PYINVOKESPSS_API PyObject * ext_IsDistributedMode(
                                                        PyObject *self,
                                                        PyObject *args);
        PYINVOKESPSS_API PyObject * ext_IsUseOrFilter(
                                                        PyObject *self,
                                                        PyObject *args);
                                                                
}

#endif
