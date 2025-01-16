/************************************************************************
** Licensed Materials - Property of IBM 
**
** IBM SPSS Products: Statistics Common
**
** (C) Copyright IBM Corp. 1989, 2011
**
** US Government Users Restricted Rights - Use, duplication or disclosure
** restricted by GSA ADP Schedule Contract with IBM Corp. 
************************************************************************/

/**
 * testxd.h -
 *     load and unload spssxd library dynamically.
 * 
 * COPYRIGHT
 *     Copyright 2005 by SPSS Inc.  All rights reserved.
 */

#ifndef _TESTXD_H_
#define _TESTXD_H_

/**
 *typedef function object.
 */
typedef bool                  (*FP_IsBackendReady)();
typedef bool                  (*FP_IsXDriven)();
typedef int                   (*FP_StartSpss)(const char* commandline);
typedef void                  (*FP_StopSpss)();
typedef int                   (*FP_Submit)(const char* command, int length);
typedef int                   (*FP_QueueCommandPart)(const char* line, int length);
typedef int                   (*FP_PostSpssOutput)(const char* text, int length);
typedef int                   (*FP_GetVariableType)(int index,int& errCode);
typedef unsigned              (*FP_GetVariableCount)(int& errCode);
typedef long                  (*FP_GetRowCount)(int& errCode);
typedef const char*           (*FP_GetVariableName)(int index,int& errCode);
typedef const char*           (*FP_GetVariableLabel)(int index,int& errCode);
typedef const char*           (*FP_GetVariableFormat)(int index,int& errCode);
typedef int                   (*FP_GetVariableMeasurementLevel)(int index,int& errCode);
typedef int                   (*FP_CreateXPathDictionary)(const char* handle);
typedef int                   (*FP_RemoveXPathHandle)(const char* handle);
typedef void*                 (*FP_EvaluateXPath)(const char* handle, const char* context, const char* expression, int& errCode);
typedef int                   (*FP_GetStringListLength)(void* listHandle);
typedef const char*           (*FP_GetStringFromList)(void* listHandle, int index); 
typedef const wchar_t*        (*FP_GetXmlUtf16)(const char* handle,int& errCode);
typedef void                  (*FP_RemoveStringList)(void* listHandle);
typedef void*                 (*FP_GetHandleList)(int& errLevel); 
typedef int                   (*FP_GetNumericValue)(unsigned varindex, double &result, int &isMissing);
typedef int                   (*FP_GetStringValue)(unsigned varindex, char* &result, int bufferLength, int &isMissing);
typedef int                   (*FP_NextCase)();
typedef int                   (*FP_RemoveCaseCursor)();
typedef int                   (*FP_GetVariableFormatType)(int index, int &formatType, int& formatWidth, int& formatDecimal);
typedef int                   (*FP_GetCursorPosition)(int &curPos);
typedef int                   (*FP_MakeCaseCursor)(const char *accessType);
typedef int                   (*FP_StartPivotTable)(const char* outLine,const char* title,const char* templateName,bool isSplit);
typedef int                   (*FP_AddDimension)(const char* outLine,const char* title,const char* templateName,bool isSplit,
                                                 const char* dimName,int place,int position, bool hideName, bool hideLabels);
typedef int                   (*FP_AddStringCategory)(const char* outLine,const char* title,const char* templateName,bool isSplit,
                                                      const char* dimName,int place,int position, bool hideName, bool hideLabels,
                                                      const char* category);
typedef int                   (*FP_SetFormatSpecCount)();
typedef int                   (*FP_SetNumberCell)(const char* outLine,const char* title,const char* templateName,bool isSplit,
                                                  const char* dimName,int place,int position, bool hideName, bool hideLabels,
                                                  double cellVal);
typedef int                   (*FP_AddCellFootnotes)(const char* outLine,const char* title,const char* templateName,bool isSplit,
                                                     const char* dimName,int place,int position, bool hideName, bool hideLabels,
                                                     const char* footnotes);
typedef int                   (*FP_AddTextBlock)(const char* outLine, const char* name, const char* line, int nSkip);
typedef int                   (*FP_StartProcedure)(const char* procName, const char* translatedName);
typedef int                   (*FP_EndProcedure)();

//data step
typedef int              (*FP_StartDataStep)();
typedef int              (*FP_CreateDataset)(const char *name, const bool isEmpty);
typedef int              (*FP_GetCaseCountInDS)(const char* dsName, int& errLevel ); 
typedef int              (*FP_InsertCase)(const char* dsName, const int rowIndex);  
typedef int              (*FP_DeleteCase)(const char* dsName, const int rowIndex);
typedef int              (*FP_GetVarTypeInDS)(const char* dsName, const int index, int& errLevel);   
typedef double           (*FP_GetNCellValue)(const char* dsName, const int rowIndex, const int columnIndex, int& isMissing, int& errLevel ); 
typedef const char*      (*FP_GetCCellValue)(const char* dsName, const int rowIndex, const int columnIndex, int& isMissing, int& errLevel );  
typedef int              (*FP_SetNCellValue)(const char* dsName, const int rowIndex, const int columnIndex, const double value );  
typedef int              (*FP_SetCCellValue)(const char* dsName, const int rowIndex, const int columnIndex, const char* value );
typedef int              (*FP_EndDataStep)();  
//end data step

/**
 *declare the function pointer.
 */
/*
extern FP_IsBackendReady                   IsBackendReady;
extern FP_IsXDriven                        IsXDriven;
extern FP_StartSpss                        StartSpss;
extern FP_StopSpss                         StopSpss;
extern FP_Submit                           Submit;
extern FP_QueueCommandPart                 QueueCommandPart;
extern FP_PostSpssOutput                   PostSpssOutput;
extern FP_GetVariableCount                 GetVariableCount;
extern FP_GetRowCount                      GetRowCount;
extern FP_GetVariableName                  GetVariableName;
extern FP_GetVariableLabel                 GetVariableLabel;
extern FP_GetVariableType                  GetVariableType;
extern FP_GetVariableFormat                GetVariableFormat;
extern FP_GetVariableMeasurementLevel      GetVariableMeasurementLevel;
extern FP_CreateXPathDictionary            CreateXPathDictionary;
extern FP_RemoveXPathHandle                RemoveXPathHandle;
extern FP_EvaluateXPath                    EvaluateXPath;
extern FP_GetStringListLength              GetStringListLength;
extern FP_GetStringFromList                GetStringFromList;
extern FP_RemoveStringList                 RemoveStringList;
extern FP_GetXmlUtf16                      GetXmlUtf16;
extern FP_GetHandleList                    GetHandleList;
extern FP_GetNumericValue                   GetNumericValue;                                               
extern FP_GetStringValue                    GetStringValue;                                                
extern FP_NextCase                          NextCase;                                                        
extern FP_RemoveCaseCursor                  RemoveCaseCursor;                                                
extern FP_GetVariableFormatType             GetVariableFormatType;                                           
extern FP_GetCursorPosition                 GetCursorPosition;                                               
extern FP_MakeCaseCursor                    MakeCaseCursor;                                                  
extern FP_StartPivotTable                 StartPivotTable;                                                  
extern FP_AddDimension                    AddDimension;                                                  
extern FP_AddStringCategory               AddStringCategory;                                                  
extern FP_SetFormatSpecCount              SetFormatSpecCount;                                                  
extern FP_SetNumberCell                   SetNumberCell;                                                  
extern FP_AddCellFootnotes                AddCellFootnotes;                                                  
extern FP_AddTextBlock                    AddTextBlock;                                                  
extern FP_StartProcedure                  StartProcedure;                                                  
extern FP_EndProcedure                    EndProcedure;                                                  
*/
/**
 *load spssxd.dll dynamically
 *
 *@return 0 if success; Otherwise, return 1011 as error level
 */
int LoadLib();
/**
 *unload spssxd.dll dynamically
 */
void FreeLib();

#endif

