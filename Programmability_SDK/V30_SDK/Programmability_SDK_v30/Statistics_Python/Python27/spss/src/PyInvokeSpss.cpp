/************************************************************************
** Licensed Materials - Property of IBM 
**
** IBM SPSS Products: Statistics Common
**
** (C) Copyright IBM Corp. 1989, 2017
**
** US Government Users Restricted Rights - Use, duplication or disclosure
** restricted by GSA ADP Schedule Contract with IBM Corp. 
************************************************************************/

/**
 * NAME
 *     PyInvokeSpss.cpp -- PyInvokeSpss.h
 *
 * DESCRIPTION
 *     An extension module for invoking PASW Statistics from python.
 *
 *--------------------------------------------------------
 *
 * Copyright (c) 2008 SPSS Inc. All rights reserved.
 */

#include "PyInvokeSpss.h"
#include <locale.h>
#include "wchar.h"

#ifdef MS_WINDOWS
  #include <windows.h>
#else
  #include <dlfcn.h>
  #include <float.h>
  #include "IBM_SPSS_Copyright.h"
#endif

extern "C"{

    static PyMethodDef PyInvokeSpss_methods[] = {
    {"QueueCommandPart", ext_QueueCommandPart, METH_VARARGS,
     "Queue a command."},
    {"CreateXPathDictionary", ext_CreateXPathDictionary, METH_VARARGS,
     "Create a xpath dictionary."},
    {"RemoveXPathHandle", ext_RemoveXPathHandle, METH_VARARGS,
     "Remove a xpath dictionary."},
    {"EvaluateXPath", ext_EvaluateXPath, METH_VARARGS,
     "Search xpath with the given context."},
    {"Submit", ext_Submit, METH_VARARGS,
     "Submit a command."},
    {"GetCaseCount", ext_GetCaseCount, METH_VARARGS,
     "Get the number of cases."},
    {"GetVariableCount", ext_GetVariableCount, METH_VARARGS,
     "Get the number of variables."},
    {"GetVariableFormat", ext_GetVariableFormat, METH_VARARGS,
     "Get the format of the given variable."},
    {"GetVariableLabel", ext_GetVariableLabel, METH_VARARGS,
     "Get the label of the given variable."},
    {"GetVariableMeasurementLevel", ext_GetVariableMeasurementLevel, METH_VARARGS,
     "Get the measurement level of the given variable."},
    {"GetVariableName", ext_GetVariableName, METH_VARARGS,
     "Get the name of the given variable."},
    {"GetVariableType", ext_GetVariableType, METH_VARARGS,
     "Get the type of the given variable."},
    {"IsXDriven", ext_IsXDriven, METH_VARARGS,
     "An integer indicating how the backend is being run. A return value of 1 indicates that Python controls the backend. A return value of 0 indicates that PASW Statistics controls the backend."},
    {"PostSpssOutput", ext_PostSpssOutput, METH_VARARGS,
     "Post output to PASW Statistics."},
    {"StartSpss", ext_StartSpss, METH_VARARGS,
     "Start PASW Statistics."},
    {"StopSpss", ext_StopSpss, METH_VARARGS,
     "Stop PASW Statistics."},
    {"IsBackendReady", ext_IsBackendReady, METH_VARARGS,
     "Report if backend ready."},
    {"GetXmlUtf16", ext_GetXmlUtf16, METH_VARARGS,
     "Get xml From xml work space with encoding utf-16."},
    {"GetImage", ext_GetImage, METH_VARARGS,
    "Get the images from XML workspace."},
    {"GetSetting", ext_GetSetting, METH_VARARGS,
    "Get the value of an options setting"},
    {"GetOMSTagList", ext_GetOMSTagList, METH_VARARGS,
    "Returns a list of tags associated with any active OMS requests"},
    {"GetHandleList", ext_GetHandleList, METH_VARARGS,
     "Get handle list from TransSpace."},
    {"GetFileHandles", ext_GetFileHandles, METH_VARARGS,
     "Get file handle list."},
    {"fetchone", ext_fetchone, METH_VARARGS,
     "get the data from data source."},
    {"fetchmany", ext_fetchmany, METH_VARARGS,
     "get many data from data source."},
    {"fetchall", ext_fetchall, METH_VARARGS,
     "get all data from data source."},
    {"close", ext_close, METH_VARARGS,
     "clsoe the case cursor."},
    {"cursor", ext_cursor, METH_VARARGS,
     "intialize the cursor object."},
    {"StartProcedure", ext_StartProcedure, METH_VARARGS,
     "StartProcedure."},
    {"SplitChange", ext_SplitChange, METH_VARARGS,
     "SplitChange."},
    {"EndProcedure", ext_EndProcedure, METH_VARARGS,
     "EndProcedure."},

    {"StartPivotTable", ext_StartPivotTable, METH_VARARGS,
     "StartPivotTable."},
    {"HidePivotTableTitle", ext_HidePivotTableTitle, METH_VARARGS,
     "HidePivotTableTitle."},
    {"PivotTableCaption", ext_PivotTableCaption, METH_VARARGS,
     "PivotTableCaption."},
    {"AddDimension", ext_AddDimension, METH_VARARGS,
     "AddDimension."},

    {"SetFormatSpecCoefficient", ext_SetFormatSpecCoefficient, METH_VARARGS,
     "SetFormatSpecCoefficient."},
    {"SetFormatSpecCoefficientSE", ext_SetFormatSpecCoefficientSE, METH_VARARGS,
     "SetFormatSpecCoefficientSE."},
    {"SetFormatSpecCoefficientVar", ext_SetFormatSpecCoefficientVar, METH_VARARGS,
     "SetFormatSpecCoefficientVar."},
    {"SetFormatSpecCorrelation", ext_SetFormatSpecCorrelation, METH_VARARGS,
     "SetFormatSpecCorrelation."},
    {"SetFormatSpecGeneralStat", ext_SetFormatSpecGeneralStat, METH_VARARGS,
     "SetFormatSpecGeneralStat."},
    {"SetFormatSpecMean", ext_SetFormatSpecMean, METH_VARARGS,
     "SetFormatSpecMean."},
    {"SetFormatSpecCount", ext_SetFormatSpecCount, METH_VARARGS,
     "SetFormatSpecCount."},
    {"SetFormatSpecPercent", ext_SetFormatSpecPercent, METH_VARARGS,
     "SetFormatSpecPercent."},
    {"SetFormatSpecPercentNoSign", ext_SetFormatSpecPercentNoSign, METH_VARARGS,
     "SetFormatSpecPercentNoSign."},
    {"SetFormatSpecProportion", ext_SetFormatSpecProportion, METH_VARARGS,
     "SetFormatSpecProportion."},
    {"SetFormatSpecSignificance", ext_SetFormatSpecSignificance, METH_VARARGS,
     "SetFormatSpecSignificance."},
    {"SetFormatSpecResidual", ext_SetFormatSpecResidual, METH_VARARGS,
     "SetFormatSpecResidual."},
    {"SetFormatSpecVariable", ext_SetFormatSpecVariable, METH_VARARGS,
     "SetFormatSpecVariable."},
    {"SetFormatSpecStdDev", ext_SetFormatSpecStdDev, METH_VARARGS,
     "SetFormatSpecStdDev."},
    {"SetFormatSpecDifference", ext_SetFormatSpecDifference, METH_VARARGS,
     "SetFormatSpecDifference."},
    {"SetFormatSpecSum", ext_SetFormatSpecSum, METH_VARARGS,
     "SetFormatSpecSum."},

    {"AddNumberCategory", ext_AddNumberCategory, METH_VARARGS,
     "AddNumberCategory."},
    {"AddStringCategory", ext_AddStringCategory, METH_VARARGS,
     "AddStringCategory."},
    {"AddVarNameCategory", ext_AddVarNameCategory, METH_VARARGS,
     "AddVarNameCategory."},
    {"AddVarValueDoubleCategory", ext_AddVarValueDoubleCategory, METH_VARARGS,
     "AddVarValueDoubleCategory."},
    {"AddVarValueStringCategory", ext_AddVarValueStringCategory, METH_VARARGS,
     "AddVarValueStringCategory."},
    {"SetNumberCell", ext_SetNumberCell, METH_VARARGS,
     "SetNumberCell."},
    {"SetStringCell", ext_SetStringCell, METH_VARARGS,
     "SetStringCell."},
    {"SetVarNameCell", ext_SetVarNameCell, METH_VARARGS,
     "SetVarNameCell."},
    {"SetVarValueDoubleCell", ext_SetVarValueDoubleCell, METH_VARARGS,
     "SetVarValueDoubleCell."},
    {"SetVarValueStringCell", ext_SetVarValueStringCell, METH_VARARGS,
     "SetVarValueStringCell."},

    {"AddCellFootnotes", ext_AddCellFootnotes, METH_VARARGS,
     "AddCellFootnotes."},

    {"AddProcFootnotes", ext_AddProcFootnotes, METH_VARARGS,
     "AddProcFootnotes."},
    {"AddOutlineFootnotes", ext_AddOutlineFootnotes, METH_VARARGS,
     "AddOutlineFootnotes."},
    {"AddTitleFootnotes", ext_AddTitleFootnotes, METH_VARARGS,
     "AddTitleFootnotes."},
    {"AddDimFootnotes", ext_AddDimFootnotes, METH_VARARGS,
     "AddDimFootnotes."},
    {"AddCategoryFootnotes", ext_AddCategoryFootnotes, METH_VARARGS,
     "AddCategoryFootnotes."},


    {"AddTextBlock", ext_AddTextBlock, METH_VARARGS,
     "AddTextBlock."},

    {"SetVarName", ext_SetVarName, METH_VARARGS,
     "SetVarName."},
    {"SetVarLabel", ext_SetVarLabel, METH_VARARGS,
     "SetVarLabel."},
    {"CommitHeader", ext_CommitHeader, METH_VARARGS,
     "CommitHeader."},
    {"SetValueChar", ext_SetValueChar, METH_VARARGS,
     "SetValueChar."},
    {"SetValueNumeric", ext_SetValueNumeric, METH_VARARGS,
     "SetValueNumeric."},
    {"CommitCaseRecord", ext_CommitCaseRecord, METH_VARARGS,
     "CommitCaseRecord."},
     {"CommitManyCases", ext_CommitManyCases, METH_VARARGS,
     "CommitManyCases."},
    {"CommitNewCase", ext_CommitNewCase, METH_VARARGS,
     "CommitNewCase."},
    {"EndChanges", ext_EndChanges, METH_VARARGS,
     "EndChagnes."},
    {"SetVarCValueLabel", ext_SetVarCValueLabel, METH_VARARGS,
     "SetVarCValueLabel."},
    {"SetVarNValueLabel", ext_SetVarNValueLabel, METH_VARARGS,
     "SetVarNValueLabel."},
    {"SetVarCMissingValues", ext_SetVarCMissingValues, METH_VARARGS,
     "SetVarCMissingValues."},
    {"SetVarNMissingValues", ext_SetVarNMissingValues, METH_VARARGS,
     "SetVarNMissingValues."},
    {"SetVarMeasureLevel", ext_SetVarMeasureLevel, METH_VARARGS,
     "SetVarMeasureLevel."},
    {"SetVarAlignment", ext_SetVarAlignment, METH_VARARGS,
     "SetVarAlignment."},
    {"SetVarFormat", ext_SetVarFormat, METH_VARARGS,
     "SetVarFormat."},
    {"HasCursor", ext_HasCursor, METH_VARARGS,
     "HasCursor."},
    {"IsEndSplit", ext_IsEndSplit, METH_VARARGS,
     "IsEndSplit."},
    {"GetVarCMissingValues", ext_GetVarCMissingValues, METH_VARARGS,
     "GetVarCMissingValues."},
    {"GetVarNMissingValues", ext_GetVarNMissingValues, METH_VARARGS,
     "GetVarNMissingValues."},
    {"GetVarAttributeNames", ext_GetVarAttributeNames, METH_VARARGS,
     "GetVarAttributeNames."},
    {"GetVarAttributes", ext_GetVarAttributes, METH_VARARGS,
     "GetVarAttributes."},
    {"SetVarAttributes", ext_SetVarAttributes, METH_VARARGS,
     "SetVarAttributes."},
    {"SetVarAttributes", ext_SetVarAttributes, METH_VARARGS,
     "SetVarAttributes."},
    {"GetCaseCountInProcDS", ext_GetCaseCountInProcDS, METH_VARARGS,
     "GetCaseCountInProcDS."},
    {"GetVariableCountInProcDS", ext_GetVariableCountInProcDS, METH_VARARGS,
     "GetVariableCountInProcDS."},
    {"GetVariableFormatInProcDS", ext_GetVariableFormatInProcDS, METH_VARARGS,
     "GetVariableFormatInProcDS."},
    {"GetVariableLabelInProcDS", ext_GetVariableLabelInProcDS, METH_VARARGS,
     "GetVariableLabelInProcDS."},
    {"GetVariableMeasurementLevelInProcDS", ext_GetVariableMeasurementLevelInProcDS, METH_VARARGS,
     "GetVariableMeasurementLevelInProcDS."},
    {"GetVariableNameInProcDS", ext_GetVariableNameInProcDS, METH_VARARGS,
     "GetVariableNameInProcDS."},
    {"GetVariableTypeInProcDS", ext_GetVariableTypeInProcDS, METH_VARARGS,
     "GetVariableTypeInProcDS."},
    {"GetVarCMissingValuesInProcDS", ext_GetVarCMissingValuesInProcDS, METH_VARARGS,
     "GetVarCMissingValuesInProcDS."},
    {"GetVarNMissingValuesInProcDS", ext_GetVarNMissingValuesInProcDS, METH_VARARGS,
     "GetVarNMissingValuesInProcDS."},
    {"GetVarAttributeNamesInProcDS", ext_GetVarAttributeNamesInProcDS, METH_VARARGS,
     "GetVarAttributeNamesInProcDS."},
    {"GetVarAttributesInProcDS", ext_GetVarAttributesInProcDS, METH_VARARGS,
     "GetVarAttributesInProcDS."},
    {"SetUserMissingInclude", ext_SetUserMissingInclude, METH_VARARGS,
     "SetUserMissingInclude."},
    {"HasProcedure", ext_HasProcedure, METH_VARARGS,
     "HasProcedure."},
    {"GetSPSSLowHigh", ext_GetSPSSLowHigh, METH_VARARGS,
     "GetSPSSLowHigh."},
    {"GetWeightVar", ext_GetWeightVar, METH_VARARGS,
     "GetWeightVar."},
    {"ResetDataPass", ext_ResetDataPass, METH_VARARGS,
     "ResetDataPass."},
    {"ResetDataPassWrite", ext_ResetDataPassWrite, METH_VARARGS,
     "ResetDataPassWrite."},
    {"AllocNewVarsBuffer", ext_AllocNewVarsBuffer, METH_VARARGS,
     "AllocNewVarsBuffer."},
    {"SetOneVarNameAndType", ext_SetOneVarNameAndType, METH_VARARGS,
     "SetOneVarNameAndType."},
    {"SetFetchVarList", ext_SetFetchVarList, METH_VARARGS,
     "SetFetchVarList."},

    {"StartDataStep", ext_StartDataStep, METH_VARARGS,
     "StartDataStep."},
    {"EndDataStep", ext_EndDataStep, METH_VARARGS,
     "EndDataStep."},
    {"CreateDataset", ext_CreateDataset, METH_VARARGS,
     "CreateDataset."},
    {"SetDatasetName", ext_SetDatasetName, METH_VARARGS,
     "SetDatasetName."},
    {"GetNewDatasetName", ext_GetNewDatasetName, METH_VARARGS,
     "GetNewDatasetName."},
    {"GetActive", ext_GetActive, METH_VARARGS,
     "GetActive."},
    {"SetActive", ext_SetActive, METH_VARARGS,
     "SetActive."},
    {"CopyDataset", ext_CopyDataset, METH_VARARGS,
     "CopyDataset."},
    {"GetSpssDatasets", ext_GetSpssDatasets, METH_VARARGS,
     "GetSpssDatasets."},
    {"GetDatastepDatasets", ext_GetDatastepDatasets, METH_VARARGS,
     "GetDatastepDatasets."},
    {"CloseDataset", ext_CloseDataset, METH_VARARGS,
     "CloseDataset."},
    {"InsertVariable", ext_InsertVariable, METH_VARARGS,
     "InsertVariable."},
    {"DeleteVariable", ext_DeleteVariable, METH_VARARGS,
     "DeleteVariable."},
    {"GetVarCountInDS", ext_GetVarCountInDS, METH_VARARGS,
     "GetVarCountInDS."},
    {"GetVarNameInDS", ext_GetVarNameInDS, METH_VARARGS,
     "GetVarNameInDS."},
    {"SetVarNameInDS", ext_SetVarNameInDS, METH_VARARGS,
     "SetVarNameInDS."},
    {"GetVarLabelInDS", ext_GetVarLabelInDS, METH_VARARGS,
     "GetVarLabelInDS."},
    {"SetVarLabelInDS", ext_SetVarLabelInDS, METH_VARARGS,
     "SetVarLabelInDS."},
    {"GetVarTypeInDS", ext_GetVarTypeInDS, METH_VARARGS,
     "GetVarTypeInDS."},
    {"SetVarTypeInDS", ext_SetVarTypeInDS, METH_VARARGS,
     "SetVarTypeInDS."},
    {"GetVarFormatInDS", ext_GetVarFormatInDS, METH_VARARGS,
     "GetVarFormatInDS."},
    {"SetVarFormatInDS", ext_SetVarFormatInDS, METH_VARARGS,
     "SetVarFormatInDS."},
    {"GetVarAlignmentInDS", ext_GetVarAlignmentInDS, METH_VARARGS,
     "GetVarAlignmentInDS."},
    {"SetVarAlignmentInDS", ext_SetVarAlignmentInDS, METH_VARARGS,
     "SetVarAlignmentInDS."},
    {"GetVarMeasurementLevelInDS", ext_GetVarMeasurementLevelInDS, METH_VARARGS,
     "GetVarMeasurementLevelInDS."},
    {"SetVarMeasurementLevelInDS", ext_SetVarMeasurementLevelInDS, METH_VARARGS,
     "SetVarMeasurementLevelInDS."},
    {"GetVarNMissingValuesInDS", ext_GetVarNMissingValuesInDS, METH_VARARGS,
     "GetVarNMissingValuesInDS."},
    {"GetVarCMissingValuesInDS", ext_GetVarCMissingValuesInDS, METH_VARARGS,
     "GetVarCMissingValuesInDS."},
    {"SetVarNMissingValuesInDS", ext_SetVarNMissingValuesInDS, METH_VARARGS,
     "SetVarNMissingValuesInDS."},
    {"SetVarCMissingValuesInDS", ext_SetVarCMissingValuesInDS, METH_VARARGS,
     "SetVarCMissingValuesInDS."},
    {"GetVarAttributesNameInDS", ext_GetVarAttributesNameInDS, METH_VARARGS,
     "GetVarAttributesNameInDS."},
    {"GetVarAttributesInDS", ext_GetVarAttributesInDS, METH_VARARGS,
     "GetVarAttributesInDS."},
    {"SetVarAttributesInDS", ext_SetVarAttributesInDS, METH_VARARGS,
     "SetVarAttributesInDS."},
    {"DelVarAttributesInDS", ext_DelVarAttributesInDS, METH_VARARGS,
     "DelVarAttributesInDS."},
    {"GetVarNValueLabelInDS", ext_GetVarNValueLabelInDS, METH_VARARGS,
     "GetVarNValueLabelInDS."},
    {"GetVarCValueLabelInDS", ext_GetVarCValueLabelInDS, METH_VARARGS,
     "GetVarCValueLabelInDS."},
    {"SetVarNValueLabelInDS", ext_SetVarNValueLabelInDS, METH_VARARGS,
     "SetVarNValueLabelInDS."},
    {"SetVarNValueLabelsInDS", ext_SetVarNValueLabelsInDS, METH_VARARGS,
     "SetVarNValueLabelsInDS."},
    {"SetVarCValueLabelInDS", ext_SetVarCValueLabelInDS, METH_VARARGS,
     "SetVarCValueLabelInDS."},
    {"SetVarCValueLabelsInDS", ext_SetVarCValueLabelsInDS, METH_VARARGS,
     "SetVarCValueLabelsInDS."},
    {"DelVarValueLabelInDS", ext_DelVarValueLabelInDS, METH_VARARGS,
     "DelVarValueLabelInDS."},
    {"DelVarNValueLabelInDS", ext_DelVarNValueLabelInDS, METH_VARARGS,
     "DelVarNValueLabelInDS."},
    {"DelVarCValueLabelInDS", ext_DelVarCValueLabelInDS, METH_VARARGS,
     "DelVarCValueLabelInDS."},
    {"InsertCase", ext_InsertCase, METH_VARARGS,
     "InsertCase."},
    {"DeleteCase", ext_DeleteCase, METH_VARARGS,
     "DeleteCase."},
    {"GetCaseCountInDS", ext_GetCaseCountInDS, METH_VARARGS,
     "GetCaseCountInDS."},
    {"GetNCellValue", ext_GetNCellValue, METH_VARARGS,
     "GetNCellValue."},
    {"GetCellsValue", ext_GetCellsValue, METH_VARARGS,
     "GetCellsValue."},
    {"GetCCellValue", ext_GetCCellValue, METH_VARARGS,
     "GetCCellValue."},
    {"SetNCellValue", ext_SetNCellValue, METH_VARARGS,
     "SetNCellValue."},
    {"SetCCellValue", ext_SetCCellValue, METH_VARARGS,
     "SetCCellValue."},
    {"SetXDriveMode", ext_SetXDriveMode, METH_VARARGS,
     "SetXDriveMode."},
    {"IsUTF8mode", ext_IsUTF8mode, METH_VARARGS,
     "IsUTF8mode."},
    {"GetSplitVariableNames", ext_GetSplitVariableNames, METH_VARARGS,
     "GetSplitVariableNames."},
    {"GetDataFileAttributeNames", ext_GetDataFileAttributeNames, METH_VARARGS,
     "GetDataFileAttributeNames."},
    {"GetDataFileAttributes", ext_GetDataFileAttributes, METH_VARARGS,
     "GetDataFileAttributes."},
   {"GetDataFileAttributeNamesInProcDS", ext_GetDataFileAttributeNamesInProcDS, METH_VARARGS,
     "GetDataFileAttributeNamesInProcDS."},
    {"GetDataFileAttributesInProcDS", ext_GetDataFileAttributesInProcDS, METH_VARARGS,
     "GetDataFileAttributesInProcDS."},
    {"GetMultiResponseSetNames", ext_GetMultiResponseSetNames, METH_VARARGS,
     "GetMultiResponseSetNames."},
    {"GetMultiResponseSet", ext_GetMultiResponseSet, METH_VARARGS,
     "GetMultiResponseSet."},
    {"GetMultiResponseSetNamesInProcDS", ext_GetMultiResponseSetNamesInProcDS, METH_VARARGS,
     "GetMultiResponseSetNamesInProcDS."},
    {"GetMultiResponseSetInProcDS", ext_GetMultiResponseSetInProcDS, METH_VARARGS,
     "GetMultiResponseSetInProcDS."},
    {"SetDataFileAttributesInDS", ext_SetDataFileAttributesInDS, METH_VARARGS,
     "SetDataFileAttributesInDS."},
    {"SetMultiResponseSetInDS", ext_SetMultiResponseSetInDS, METH_VARARGS,
     "SetMultiResponseSetInDS."},
    {"GetDataFileAttributeNamesInDS", ext_GetDataFileAttributeNamesInDS, METH_VARARGS,
    "GetDataFileAttributeNamesInDS."},
    {"GetDataFileAttributesInDS", ext_GetDataFileAttributesInDS, METH_VARARGS,
    "GetDataFileAttributesInDS."},
    {"GetMultiResponseSetNamesInDS", ext_GetMultiResponseSetNamesInDS, METH_VARARGS,
    "GetMultiResponseSetNamesInDS."},
    {"GetMultiResponseSetInDS", ext_GetMultiResponseSetInDS, METH_VARARGS,
    "GetMultiResponseSetInDS."},
    {"GetVarColumnWidthInDS", ext_GetVarColumnWidthInDS, METH_VARARGS,
    "GetVarColumnWidthInDS."},
    {"SetVarColumnWidthInDS", ext_SetVarColumnWidthInDS, METH_VARARGS,
    "SetVarColumnWidthInDS."},
    {"SaveFileInDS", ext_SaveFileInDS, METH_VARARGS,
    "SaveFileInDS."},
    {"DelDataFileAttributesInDS", ext_DelDataFileAttributesInDS, METH_VARARGS,
    "DelDataFileAttributesInDS."},
    {"DelMultiResponseSetInDS", ext_DelMultiResponseSetInDS, METH_VARARGS,
    "DelMultiResponseSetInDS."},
    {"GetXDriveMode", ext_GetXDriveMode, METH_VARARGS,
    "GetXDriveMode."},
    {"GetSPSSLocale", ext_GetSPSSLocale, METH_VARARGS,
     "Get the current PASW Statistics locale."},
    {"GetCLocale", ext_GetCLocale, METH_NOARGS,
     "Get the current PASW Statistics C locale."},
    {"SetOutputLanguage", ext_SetOutputLanguage, METH_VARARGS,
     "Set the current PASW Statistics output language."},
    {"GetSystemMissingValue", ext_GetSystemMissingValue, METH_VARARGS,
     "Get the System missing value."},
    {"GetNestDepth", ext_GetNestDepth, METH_VARARGS,
    "Get nest depth."},
    {"GetOutputLanguage", ext_GetOutputLanguage, METH_VARARGS,
    "Get XD output language."},
    {"TransCode", ext_TransCode, METH_VARARGS,
    "Transform string to UTF."},
    {"GetCellsValueCache", ext_GetCellsValueCache, METH_VARARGS,
    "GetCellsValueCache"},
    {"SetDefaultEncoding", ext_SetDefaultEncoding, METH_VARARGS,
    "Sets the currently active default encoding."},
    {"GetVariableRole", ext_GetVariableRole, METH_VARARGS,
    "Get the role of the given variable."},
    {"GetVariableRoleInProcDS", ext_GetVariableRoleInProcDS, METH_VARARGS,
    "GetVariableRoleInProcDS."},
    {"SetVarRole", ext_SetVarRole, METH_VARARGS,
    "SetVarRole."},
    {"GetVarRoleInDS", ext_GetVarRoleInDS, METH_VARARGS,
    "GetVarRoleInDS."},
    {"SetVarRoleInDS", ext_SetVarRoleInDS, METH_VARARGS,
    "SetVarRoleInDS."},
    {"NextCase", ext_NextCase, METH_VARARGS,
    "NextCase."},
    {"TransportData", ext_TransportData, METH_VARARGS,
    "TransportData."},
    {"GetDataFromTempFile", ext_GetDataFromTempFile, METH_VARARGS,
    "GetDataFromTempFile."},
    {"SaveDataToTempFile", ext_SaveDataToTempFile, METH_VARARGS,
    "SaveDataToTempFile."},
    {"GetSplitEndIndex", ext_GetSplitEndIndex, METH_VARARGS,
    "GetSplitEndIndex."},
    {"SetMode", ext_SetMode, METH_VARARGS,
    "SetMode."},
    {"GetRowCountInTempFile", ext_GetRowCountInTempFile, METH_VARARGS,
    "GetRowCountInTempFile."},
    {"GetCaseValue", ext_GetCaseValue, METH_VARARGS,
    "GetCaseValue"},
    {"SetCasePartValue", ext_SetCaseValue, METH_VARARGS,
    "SetCasePartValue"},
    {"SetCacheInDS", ext_SetCacheInDS, METH_VARARGS,
    "SetCacheInDS"},
    {"IsDistributedMode", ext_IsDistributedMode, METH_VARARGS,
     "Report if client is remote."},
    {"IsUseOrFilter", ext_IsUseOrFilter, METH_VARARGS,
    "IsUseOrFilter"},
    {"MoveVariables", ext_MoveVariables, METH_VARARGS,
    "MoveVariables"},
    {"CreateSocket", ext_CreateSocket, METH_VARARGS,
    "CreateSocket"},
    {"AcceptSocket", ext_AcceptSocket, METH_VARARGS,
    "AcceptSocket"},
    {"GetTrendsInfoInDS", ext_GetTrendsInfoInDS, METH_VARARGS,
    "GetTrendsInfoInDS"},
    {"GetSelectedRowCountInProcDS", ext_GetSelectedRowCountInProcDS, METH_VARARGS,
    "GetSelectedRowCountInProcDS"},
    {NULL, NULL}
    };
    //declare the function pointer.
    static FP_IsBackendReady IsBackendReady = NULL;
    static FP_IsXDriven IsXDriven = NULL;
    static FP_SetXNameAndSHome SetXNameAndSHome = NULL;
    static FP_StartSpss StartSpss = NULL;
    static FP_StopSpss StopSpss = NULL;
    static FP_Submit Submit = NULL;
    static FP_QueueCommandPart QueueCommandPart = NULL;
    static FP_PostSpssOutput PostSpssOutput = NULL;
    static FP_GetVariableCount GetVariableCount = NULL;
    static FP_GetRowCount GetRowCount = NULL;
    static FP_GetVariableName GetVariableName = NULL;
    static FP_GetVariableLabel GetVariableLabel = NULL;
    static FP_GetVariableType GetVariableType = NULL;
    static FP_GetVariableFormat GetVariableFormat = NULL;
    static FP_GetVariableMeasurementLevel GetVariableMeasurementLevel = NULL;
    static FP_CreateXPathDictionary CreateXPathDictionary = NULL;
    static FP_RemoveXPathHandle RemoveXPathHandle = NULL;
    static FP_EvaluateXPath EvaluateXPath = NULL;
    static FP_GetStringListLength GetStringListLength = NULL;
    static FP_GetStringFromList GetStringFromList = NULL;
    static FP_RemoveStringList RemoveStringList = NULL;
    static FP_GetXmlUtf16 GetXmlUtf16 = NULL;
    static FP_GetImage GetImage = NULL;
    static FP_GetSetting GetSetting = NULL;
    static FP_GetOMSTagList GetOMSTagList = NULL;
    static FP_GetHandleList GetHandleList = NULL;
    static FP_GetFileHandles GetFileHandles = NULL;
    static FP_GetNumericValue GetNumericValue = NULL;
    static FP_GetStringValue GetStringValue = NULL;
    static FP_NextCase NextCase = NULL;
    static FP_RemoveCaseCursor RemoveCaseCursor = NULL;
    static FP_GetVariableFormatType GetVariableFormatType = NULL;
    static FP_GetCursorPosition GetCursorPosition = NULL;
    static FP_MakeCaseCursor    MakeCaseCursor = NULL;
    static FP_GetColumnCountInProcDS    GetColumnCountInProcDS = NULL;
    static FP_StartProcedure StartProcedure = NULL;
    static FP_SplitChange SplitChange = NULL;
    static FP_EndProcedure  EndProcedure = NULL;
    static FP_StartPivotTable  StartPivotTable = NULL;
    static FP_HidePivotTableTitle  HidePivotTableTitle = NULL;
    static FP_PivotTableCaption  PivotTableCaption = NULL;
    static FP_AddDimension  AddDimension = NULL;

    static FP_AddNumberCategory AddNumberCategory  = NULL;
    static FP_AddStringCategory AddStringCategory  = NULL;
    static FP_AddVarNameCategory AddVarNameCategory  = NULL;
    static FP_AddVarValueDoubleCategory AddVarValueDoubleCategory  = NULL;
    static FP_AddVarValueStringCategory AddVarValueStringCategory  = NULL;
    static FP_SetNumberCell SetNumberCell = NULL;
    static FP_SetStringCell SetStringCell   = NULL;
    static FP_SetVarNameCell SetVarNameCell   = NULL;
    static FP_SetVarValueDoubleCell SetVarValueDoubleCell   = NULL;
    static FP_SetVarValueStringCell SetVarValueStringCell  = NULL;

    static FP_SetFormatSpecCoefficient SetFormatSpecCoefficient = NULL;
    static FP_SetFormatSpecCoefficientSE SetFormatSpecCoefficientSE  = NULL;
    static FP_SetFormatSpecCoefficientVar SetFormatSpecCoefficientVar  = NULL;
    static FP_SetFormatSpecCorrelation SetFormatSpecCorrelation  = NULL;
    static FP_SetFormatSpecGeneralStat SetFormatSpecGeneralStat  = NULL;
    static FP_SetFormatSpecMean SetFormatSpecMean  = NULL;
    static FP_SetFormatSpecCount SetFormatSpecCount  = NULL;
    static FP_SetFormatSpecPercent SetFormatSpecPercent  = NULL;
    static FP_SetFormatSpecPercentNoSign SetFormatSpecPercentNoSign  = NULL;
    static FP_SetFormatSpecProportion SetFormatSpecProportion  = NULL;
    static FP_SetFormatSpecSignificance SetFormatSpecSignificance  = NULL;
    static FP_SetFormatSpecResidual SetFormatSpecResidual = NULL;
    static FP_SetFormatSpecVariable SetFormatSpecVariable  = NULL;
    static FP_SetFormatSpecStdDev SetFormatSpecStdDev  = NULL;
    static FP_SetFormatSpecDifference SetFormatSpecDifference  = NULL;
    static FP_SetFormatSpecSum SetFormatSpecSum  = NULL;
    static FP_AddCellFootnotes AddCellFootnotes = NULL;

    static FP_AddProcFootnotes AddProcFootnotes = NULL;
    static FP_AddOutlineFootnotes AddOutlineFootnotes = NULL;
    static FP_AddTitleFootnotes AddTitleFootnotes = NULL;
    static FP_AddDimFootnotes AddDimFootnotes = NULL;
    static FP_AddCategoryFootnotes AddCategoryFootnotes = NULL;

    static FP_AddTextBlock AddTextBlock = NULL;
    static FP_MinDataColumnWidth MinDataColumnWidth = NULL;
    static FP_MaxDataColumnWidth MaxDataColumnWidth = NULL;

    static FP_HasCursor HasCursor = NULL;
    static FP_GetRowCountInProcDS GetRowCountInProcDS = NULL;
    static FP_GetVariableCountInProcDS GetVariableCountInProcDS = NULL;
    static FP_GetVariableLabelInProcDS GetVariableLabelInProcDS = NULL;
    static FP_GetVariableMeasurementLevelInProcDS GetVariableMeasurementLevelInProcDS = NULL;
    static FP_GetVariableFormatTypeInProcDS GetVariableFormatTypeInProcDS = NULL;

    static FP_GetVariableNameInProcDS GetVariableNameInProcDS  = NULL;
    static FP_GetVariableTypeInProcDS GetVariableTypeInProcDS  = NULL;
    static FP_GetVarAttributeNamesInProcDS GetVarAttributeNamesInProcDS  = NULL;
    static FP_GetVarAttributesInProcDS GetVarAttributesInProcDS  = NULL;
    static FP_GetVarCMissingValuesInProcDS GetVarCMissingValuesInProcDS  = NULL;
    static FP_GetVarNMissingValuesInProcDS GetVarNMissingValuesInProcDS  = NULL;
    static FP_GetVarCMissingValues GetVarCMissingValues = NULL;
    static FP_GetVarNMissingValues GetVarNMissingValues = NULL;
    static FP_GetVarAttributeNames GetVarAttributeNames  = NULL;
    static FP_FreeAttributeNames FreeAttributeNames  = NULL;
    static FP_GetVarAttributes GetVarAttributes  = NULL;
    static FP_FreeAttributes FreeAttributes  = NULL;
    static FP_SetVarAttributes SetVarAttributes  = NULL;
    static FP_GetVariableFormatInProcDS GetVariableFormatInProcDS  = NULL;
    static FP_SetVarNameAndType SetVarNameAndType  = NULL;
    static FP_SetVarLabel SetVarLabel  = NULL;
    static FP_SetVarCValueLabel SetVarCValueLabel  = NULL;
    static FP_SetVarNValueLabel SetVarNValueLabel  = NULL;
    static FP_SetVarCMissingValues SetVarCMissingValues  = NULL;
    static FP_SetVarNMissingValues SetVarNMissingValues  = NULL;
    static FP_SetVarMeasureLevel SetVarMeasureLevel  = NULL;
    static FP_SetVarAlignment SetVarAlignment  = NULL;
    static FP_SetVarFormat SetVarFormat  = NULL;
    static FP_CommitHeader CommitHeader  = NULL;
    static FP_SetValueChar SetValueChar  = NULL;
    static FP_SetValueNumeric SetValueNumeric  = NULL;
    static FP_CommitCaseRecord CommitCaseRecord  = NULL;
    static FP_CommitNewCase CommitNewCase  = NULL;
    static FP_CommitManyCases CommitManyCases  = NULL;
    static FP_EndChanges EndChanges  = NULL;
    static FP_IsEndSplit IsEndSplit  = NULL;
    static FP_HasProcedure HasProcedure = NULL;
    static FP_GetSPSSLowHigh GetSPSSLowHigh  = NULL;
    static FP_GetWeightVar GetWeightVar = NULL;
    static FP_ResetDataPass ResetDataPass = NULL;
    static FP_AllocNewVarsBuffer AllocNewVarsBuffer = NULL;
    static FP_SetOneVarNameAndType SetOneVarNameAndType = NULL;
    static FP_SetXDriveMode SetXDriveMode = NULL;

    static FP_StartDataStep StartDataStep = NULL;
    static FP_EndDataStep EndDataStep = NULL;
    static FP_CreateDataset CreateDataset = NULL;
    static FP_SetDatasetName SetDatasetName = NULL;
    static FP_GetNewDatasetName GetNewDatasetName = NULL;
    static FP_GetActive GetActive = NULL;
    static FP_SetActive SetActive = NULL;
    static FP_CopyDataset CopyDataset = NULL;
    static FP_GetSpssDatasets GetSpssDatasets = NULL;
    static FP_GetDatastepDatasets GetDatastepDatasets = NULL;
    static FP_FreeStringArray FreeStringArray = NULL;
    static FP_CloseDataset CloseDataset = NULL;
    static FP_InsertVariable InsertVariable = NULL;
    static FP_DeleteVariable DeleteVariable = NULL;
    static FP_GetVarCountInDS GetVarCountInDS = NULL;
    static FP_GetVarNameInDS GetVarNameInDS = NULL;
    static FP_SetVarNameInDS SetVarNameInDS = NULL;
    static FP_GetVarLabelInDS GetVarLabelInDS = NULL;
    static FP_SetVarLabelInDS SetVarLabelInDS = NULL;
    static FP_GetVarTypeInDS GetVarTypeInDS = NULL;
    static FP_SetVarTypeInDS SetVarTypeInDS = NULL;
    static FP_GetVarFormatInDS GetVarFormatInDS = NULL;
    static FP_SetVarFormatInDS SetVarFormatInDS = NULL;
    static FP_GetVarAlignmentInDS GetVarAlignmentInDS = NULL;
    static FP_SetVarAlignmentInDS SetVarAlignmentInDS = NULL;
    static FP_GetVarMeasurementLevelInDS GetVarMeasurementLevelInDS = NULL;
    static FP_SetVarMeasurementLevelInDS SetVarMeasurementLevelInDS = NULL;
    static FP_GetVarNMissingValuesInDS GetVarNMissingValuesInDS = NULL;
    static FP_GetVarCMissingValuesInDS GetVarCMissingValuesInDS = NULL;
    static FP_SetVarNMissingValuesInDS SetVarNMissingValuesInDS = NULL;
    static FP_SetVarCMissingValuesInDS SetVarCMissingValuesInDS = NULL;
    static FP_GetVarAttributesNameInDS GetVarAttributesNameInDS = NULL;
    static FP_GetVarAttributesInDS GetVarAttributesInDS = NULL;
    static FP_SetVarAttributesInDS SetVarAttributesInDS = NULL;
    static FP_DelVarAttributesInDS DelVarAttributesInDS = NULL;
    static FP_GetVarNValueLabelInDS GetVarNValueLabelInDS = NULL;
    static FP_FreeDoubleArray FreeDoubleArray = NULL;
    static FP_GetVarCValueLabelInDS GetVarCValueLabelInDS = NULL;
    static FP_SetVarNValueLabelInDS SetVarNValueLabelInDS = NULL;
    static FP_SetVarCValueLabelInDS SetVarCValueLabelInDS = NULL;
    static FP_DelVarValueLabelInDS DelVarValueLabelInDS = NULL;
    static FP_DelVarNValueLabelInDS DelVarNValueLabelInDS = NULL;
    static FP_DelVarCValueLabelInDS DelVarCValueLabelInDS = NULL;
    static FP_InsertCase InsertCase = NULL;
    static FP_DeleteCase DeleteCase = NULL;
    static FP_GetCaseCountInDS GetCaseCountInDS = NULL;
    static FP_GetNCellValue GetNCellValue = NULL;
    static FP_GetCCellValue GetCCellValue = NULL;
    static FP_SetNCellValue SetNCellValue = NULL;
    static FP_SetCCellValue SetCCellValue = NULL;

    static FP_IsUTF8mode IsUTF8mode = NULL;
    static FP_GetSplitVariableNames GetSplitVariableNames = NULL;
    static FP_GetDataFileAttributes GetDataFileAttributes = NULL;
    static FP_GetDataFileAttributeNames GetDataFileAttributeNames = NULL;
    static FP_GetDataFileAttributesInProcDS GetDataFileAttributesInProcDS = NULL;
    static FP_GetDataFileAttributeNamesInProcDS GetDataFileAttributeNamesInProcDS = NULL;
    static FP_GetMultiResponseSetNames GetMultiResponseSetNames  = NULL;
    static FP_GetMultiResponseSet GetMultiResponseSet = NULL;
    static FP_GetMultiResponseSetNamesInProcDS GetMultiResponseSetNamesInProcDS  = NULL;
    static FP_GetMultiResponseSetInProcDS GetMultiResponseSetInProcDS = NULL;
    static FP_SetDataFileAttributesInDS SetDataFileAttributesInDS = NULL;
    static FP_SetMultiResponseSetInDS SetMultiResponseSetInDS = NULL;
    static FP_FreeString FreeString = NULL;
    static FP_GetDataFileAttributeNamesInDS GetDataFileAttributeNamesInDS = NULL;
    static FP_GetDataFileAttributesInDS GetDataFileAttributesInDS = NULL;
    static FP_GetMultiResponseSetNamesInDS GetMultiResponseSetNamesInDS = NULL;
    static FP_GetMultiResponseSetInDS GetMultiResponseSetInDS = NULL;
    static FP_GetVarColumnWidthInDS GetVarColumnWidthInDS = NULL;
    static FP_SetVarColumnWidthInDS SetVarColumnWidthInDS = NULL;
    static FP_SaveFileInDS SaveFileInDS = NULL;
    static FP_DelDataFileAttributesInDS DelDataFileAttributesInDS = NULL;
    static FP_DelMultiResponseSetInDS DelMultiResponseSetInDS = NULL;
    static FP_GetXDriveMode GetXDriveMode = NULL;
    static FP_GetSPSSLocale GetSPSSLocale = NULL;
    static FP_GetCLocale GetCLocale = NULL;
    static FP_SetOutputLanguage SetOutputLanguage = NULL;
    static FP_GetNestDepth GetNestDepth = NULL;
    static FP_GetOutputLanguage GetOutputLanguage = NULL;
    
    static FP_TransCode TransCode = NULL;
    
    static FP_GetNCellValueCache GetNCellValueCache = NULL;
    static FP_GetCCellValueCache GetCCellValueCache = NULL;
    static FP_GetRowList GetRowList = NULL;
    static FP_GetVarTypeInDSCache GetVarTypeInDSCache = NULL;
    static FP_ClearDatastepBatch ClearDatastepBatch = NULL;
    
    static FP_GetVariableRole GetVariableRole = NULL;
    static FP_GetVariableRoleInProcDS GetVariableRoleInProcDS = NULL;
    static FP_SetVarRole SetVarRole = NULL;
    static FP_GetVarRoleInDS GetVarRoleInDS = NULL;
    static FP_SetVarRoleInDS SetVarRoleInDS = NULL;
    
    static FP_TransportData TransportData = NULL;
    static FP_GetDataFromTempFile GetDataFromTempFile = NULL;
    static FP_SaveDataToTempFile SaveDataToTempFile = NULL;
    static FP_GetSplitEndIndex GetSplitEndIndex = NULL;
    static FP_SetMode SetMode = NULL;
    static FP_GetRowCountInTempFile GetRowCountInTempFile = NULL;
    
    static FP_GetNCellValueFromCache GetNCellValueFromCache = NULL;
    static FP_GetCCellValueFromCache GetCCellValueFromCache = NULL;
    static FP_SetNCellValueFromCache SetNCellValueFromCache = NULL;
    static FP_SetCCellValueFromCache SetCCellValueFromCache = NULL;
    static FP_GetVarInfo GetVarInfo = NULL;
    static FP_GetCaseValue GetCaseValue = NULL;
    static FP_SetCasePartValue SetCasePartValue = NULL;
    static FP_SetCacheInDS SetCacheInDS = NULL;
    
    static FP_IsDistributedMode IsDistributedMode = NULL;
    static FP_IsUseOrFilter IsUseOrFilter = NULL;
    
    static FP_MoveVariables MoveVariables = NULL;
    
    static FP_CreateSocket CreateSocket = NULL;
    static FP_AcceptSocket AcceptSocket = NULL;
    
    static FP_GetTrendsInfoInDS GetTrendsInfoInDS = NULL;
    static FP_GetSelectedRowCountInProcDS GetSelectedRowCountInProcDS = NULL;
    
    const char *dlError = NULL; /* Pointer to error string */
#ifdef MS_WINDOWS
  #define LIBHANDLE        HMODULE
  #define GETADDRESS       GetProcAddress
  #define LIBNAME          "spssxd_p.dll"
#else
  #define LIBHANDLE        void*
  #define GETADDRESS       dlsym
  #ifdef __MACOSX__
      #define LIBNAME      "libspssxd_p.dylib"
  #elif __HPUX64__
      #define LIBNAME      "libspssxd_p.sl"
  #else
      #define LIBNAME      "libspssxd_p.so"
  #endif
#endif

#ifdef _AMD64_
  #define PYSIZE Py_ssize_t
#else
  #ifdef __MACOSX__
       #define PYSIZE Py_ssize_t
  #elif __PYTHON27__
       #define PYSIZE Py_ssize_t
  #else
       #define PYSIZE int
  #endif
#endif


  LIBHANDLE pLib = NULL;
  static LIBHANDLE pLib1 = NULL;
  static LIBHANDLE pLib2 = NULL;
  static LIBHANDLE pLib3 = NULL;
  const char libName[] = LIBNAME;

  const int PY_TRUE = 1;
  const int PY_FALSE = 0;
  const int LOAD_FAIL = 1011;
  const int LOAD_SUCCESS = 0;
  const int NO_MORE_DATA = 23;
  const int NO_MEMORY = 1021;           //no memory
  const int CONVERTER_FAIL = 1022;      //make converter fail.
  const int ERROR_PARAMETER = 1023;     //parameter error.
  const int ERROR_PYTHON = 1030;
  //const int CELL_ARGS_ERROR = 1032;   // Invalid pivot table cell values.
  const int PARSE_TUPLE_FAIL = 1033;
  const int SIZE_NOT_EQUAL = 1034;

  const int MISSING_VAL_LEN = 9;
  const int DS_MODE = 0;
  const int PROCDS_MODE = 1;

  static int (*StartReceivePyThread)() = 0;

  //Initialize the function pointer
  void InitializeFP()
  {
    IsBackendReady = (FP_IsBackendReady)GETADDRESS(pLib,"IsBackendReady");
    IsXDriven = (FP_IsXDriven)GETADDRESS(pLib,"IsXDriven");
    SetXNameAndSHome = (FP_SetXNameAndSHome)GETADDRESS(pLib,"SetXNameAndSHome");
    StartSpss = (FP_StartSpss)GETADDRESS(pLib ,"StartSpss");
    StopSpss = (FP_StopSpss)GETADDRESS(pLib,"StopSpss");
    Submit = (FP_Submit)GETADDRESS(pLib,"Submit");
    QueueCommandPart = (FP_QueueCommandPart)GETADDRESS(pLib,"QueueCommandPart");
    PostSpssOutput = (FP_PostSpssOutput)GETADDRESS(pLib,"PostSpssOutput");
    GetVariableCount = (FP_GetVariableCount)GETADDRESS(pLib,"GetVariableCount");
    GetRowCount = (FP_GetRowCount)GETADDRESS(pLib,"GetRowCount");
    GetVariableName = (FP_GetVariableName)GETADDRESS(pLib,"GetVariableName");
    GetVariableLabel = (FP_GetVariableLabel)GETADDRESS(pLib,"GetVariableLabel");
    GetVariableType = (FP_GetVariableType)GETADDRESS(pLib,"GetVariableType");
    GetVariableFormat = (FP_GetVariableFormat)GETADDRESS(pLib,"GetVariableFormat");
    GetVariableMeasurementLevel = (FP_GetVariableMeasurementLevel)GETADDRESS(pLib,"GetVariableMeasurementLevel");
    CreateXPathDictionary = (FP_CreateXPathDictionary)GETADDRESS(pLib,"CreateXPathDictionary");
    RemoveXPathHandle = (FP_RemoveXPathHandle)GETADDRESS(pLib,"RemoveXPathHandle");
    EvaluateXPath =(FP_EvaluateXPath)GETADDRESS(pLib,"EvaluateXPath");
    GetStringListLength = (FP_GetStringListLength)GETADDRESS(pLib,"GetStringListLength");
    GetStringFromList = (FP_GetStringFromList)GETADDRESS(pLib,"GetStringFromList");
    RemoveStringList = (FP_RemoveStringList)GETADDRESS(pLib,"RemoveStringList");
    GetXmlUtf16 = (FP_GetXmlUtf16)GETADDRESS(pLib,"GetXmlUtf16");
    GetImage = (FP_GetImage)GETADDRESS(pLib,"GetImage");
    GetSetting = (FP_GetSetting)GETADDRESS(pLib,"GetSetting");
    GetOMSTagList = (FP_GetOMSTagList)GETADDRESS(pLib,"GetOMSTagList");
    GetHandleList = (FP_GetHandleList)GETADDRESS(pLib,"GetHandleList");
    GetFileHandles = (FP_GetFileHandles)GETADDRESS(pLib,"GetFileHandles");

    GetNumericValue = (FP_GetNumericValue)GETADDRESS(pLib,"GetNumericValue");
    GetStringValue = (FP_GetStringValue)GETADDRESS(pLib,"GetStringValue");
    NextCase = (FP_NextCase)GETADDRESS(pLib,"NextCase");
    RemoveCaseCursor = (FP_RemoveCaseCursor)GETADDRESS(pLib,"RemoveCaseCursor");
    GetVariableFormatType = (FP_GetVariableFormatType)GETADDRESS(pLib,"GetVariableFormatType");
    GetCursorPosition = (FP_GetCursorPosition)GETADDRESS(pLib,"GetCursorPosition");
    MakeCaseCursor = (FP_MakeCaseCursor)GETADDRESS(pLib,"MakeCaseCursor");
    GetColumnCountInProcDS = (FP_GetColumnCountInProcDS)GETADDRESS(pLib,"GetColumnCountInProcDS");
    StartProcedure = (FP_StartProcedure)GETADDRESS(pLib,"StartProcedure");
    SplitChange = (FP_SplitChange)GETADDRESS(pLib,"SplitChange");
    EndProcedure = (FP_EndProcedure)GETADDRESS(pLib,"EndProcedure");

    StartPivotTable = (FP_StartPivotTable)GETADDRESS(pLib,"StartPivotTable");
    HidePivotTableTitle = (FP_HidePivotTableTitle)GETADDRESS(pLib,"HidePivotTableTitle");
    PivotTableCaption = (FP_PivotTableCaption)GETADDRESS(pLib,"PivotTableCaption");
    AddDimension = (FP_AddDimension)GETADDRESS(pLib,"AddDimension");

    AddNumberCategory = (FP_AddNumberCategory)GETADDRESS(pLib,"AddNumberCategory");
    AddStringCategory = (FP_AddStringCategory)GETADDRESS(pLib,"AddStringCategory");
    AddVarNameCategory = (FP_AddVarNameCategory)GETADDRESS(pLib,"AddVarNameCategory");
    AddVarValueDoubleCategory = (FP_AddVarValueDoubleCategory)GETADDRESS(pLib,"AddVarValueDoubleCategory");
    AddVarValueStringCategory = (FP_AddVarValueStringCategory)GETADDRESS(pLib,"AddVarValueStringCategory");
    SetNumberCell = (FP_SetNumberCell)GETADDRESS(pLib,"SetNumberCell");
    SetStringCell = (FP_SetStringCell)GETADDRESS(pLib,"SetStringCell");
    SetVarNameCell = (FP_SetVarNameCell)GETADDRESS(pLib,"SetVarNameCell");
    SetVarValueDoubleCell = (FP_SetVarValueDoubleCell)GETADDRESS(pLib,"SetVarValueDoubleCell");
    SetVarValueStringCell = (FP_SetVarValueStringCell)GETADDRESS(pLib,"SetVarValueStringCell");
    AddCellFootnotes = (FP_AddCellFootnotes)GETADDRESS(pLib,"AddCellFootnotes");

    AddProcFootnotes = (FP_AddProcFootnotes)GETADDRESS(pLib,"AddProcFootnotes");
    AddOutlineFootnotes = (FP_AddOutlineFootnotes)GETADDRESS(pLib,"AddOutlineFootnotes");
    AddTitleFootnotes = (FP_AddTitleFootnotes)GETADDRESS(pLib,"AddTitleFootnotes");
    AddDimFootnotes = (FP_AddDimFootnotes)GETADDRESS(pLib,"AddDimFootnotes");
    AddCategoryFootnotes = (FP_AddCategoryFootnotes)GETADDRESS(pLib,"AddCategoryFootnotes");


    AddTextBlock = (FP_AddTextBlock)GETADDRESS(pLib,"AddTextBlock");
    SetFormatSpecCoefficient = (FP_SetFormatSpecCoefficient)GETADDRESS(pLib,"SetFormatSpecCoefficient");
    SetFormatSpecCoefficientSE = (FP_SetFormatSpecCoefficientSE)GETADDRESS(pLib,"SetFormatSpecCoefficientSE");
    SetFormatSpecCoefficientVar = (FP_SetFormatSpecCoefficientVar)GETADDRESS(pLib,"SetFormatSpecCoefficientVar");
    SetFormatSpecCorrelation = (FP_SetFormatSpecCorrelation)GETADDRESS(pLib,"SetFormatSpecCorrelation");
    SetFormatSpecGeneralStat = (FP_SetFormatSpecGeneralStat)GETADDRESS(pLib,"SetFormatSpecGeneralStat");
    SetFormatSpecMean = (FP_SetFormatSpecMean)GETADDRESS(pLib,"SetFormatSpecMean");
    SetFormatSpecCount = (FP_SetFormatSpecCount)GETADDRESS(pLib,"SetFormatSpecCount");
    SetFormatSpecPercent = (FP_SetFormatSpecPercent)GETADDRESS(pLib,"SetFormatSpecPercent");
    SetFormatSpecPercentNoSign = (FP_SetFormatSpecPercentNoSign)GETADDRESS(pLib,"SetFormatSpecPercentNoSign");
    SetFormatSpecProportion = (FP_SetFormatSpecProportion)GETADDRESS(pLib,"SetFormatSpecProportion");
    SetFormatSpecSignificance = (FP_SetFormatSpecSignificance)GETADDRESS(pLib,"SetFormatSpecSignificance");
    SetFormatSpecResidual = (FP_SetFormatSpecResidual)GETADDRESS(pLib,"SetFormatSpecResidual");
    SetFormatSpecVariable = (FP_SetFormatSpecVariable)GETADDRESS(pLib,"SetFormatSpecVariable");
    SetFormatSpecStdDev = (FP_SetFormatSpecStdDev)GETADDRESS(pLib,"SetFormatSpecStdDev");
    SetFormatSpecDifference = (FP_SetFormatSpecDifference)GETADDRESS(pLib,"SetFormatSpecDifference");
    SetFormatSpecSum = (FP_SetFormatSpecSum)GETADDRESS(pLib,"SetFormatSpecSum");

    HasCursor = (FP_HasCursor)GETADDRESS(pLib,"HasCursor");
    IsEndSplit = (FP_IsEndSplit)GETADDRESS(pLib,"IsEndSplit");

    GetRowCountInProcDS = (FP_GetRowCountInProcDS)GETADDRESS(pLib,"GetRowCountInProcDS");
    GetVariableCountInProcDS = (FP_GetVariableCountInProcDS)GETADDRESS(pLib,"GetVariableCountInProcDS");
    GetVariableLabelInProcDS = (FP_GetVariableLabelInProcDS)GETADDRESS(pLib,"GetVariableLabelInProcDS");
    GetVariableMeasurementLevelInProcDS = (FP_GetVariableMeasurementLevelInProcDS)GETADDRESS(pLib,"GetVariableMeasurementLevelInProcDS");
    GetVariableNameInProcDS = (FP_GetVariableNameInProcDS)GETADDRESS(pLib,"GetVariableNameInProcDS");
    GetVariableTypeInProcDS = (FP_GetVariableTypeInProcDS)GETADDRESS(pLib,"GetVariableTypeInProcDS");
    GetVarAttributeNamesInProcDS = (FP_GetVarAttributeNamesInProcDS)GETADDRESS(pLib,"GetVarAttributeNamesInProcDS");
    GetVarAttributesInProcDS = (FP_GetVarAttributesInProcDS)GETADDRESS(pLib,"GetVarAttributesInProcDS");
    GetVarCMissingValuesInProcDS = (FP_GetVarCMissingValuesInProcDS)GETADDRESS(pLib,"GetVarCMissingValuesInProcDS");
    GetVarNMissingValuesInProcDS = (FP_GetVarNMissingValuesInProcDS)GETADDRESS(pLib,"GetVarNMissingValuesInProcDS");
    GetVariableFormatTypeInProcDS = (FP_GetVariableFormatTypeInProcDS)GETADDRESS(pLib,"GetVariableFormatTypeInProcDS");
    GetVarCMissingValues = (FP_GetVarCMissingValues)GETADDRESS(pLib,"GetVarCMissingValues");
    GetVarNMissingValues = (FP_GetVarNMissingValues)GETADDRESS(pLib,"GetVarNMissingValues");
    GetVarAttributeNames = (FP_GetVarAttributeNames)GETADDRESS(pLib,"GetVarAttributeNames");
    FreeAttributeNames = (FP_FreeAttributeNames)GETADDRESS(pLib,"FreeAttributeNames");
    GetVarAttributes = (FP_GetVarAttributes)GETADDRESS(pLib,"GetVarAttributes");
    FreeAttributes = (FP_FreeAttributes)GETADDRESS(pLib,"FreeAttributes");
    SetVarAttributes = (FP_SetVarAttributes)GETADDRESS(pLib,"SetVarAttributes");
    GetVariableFormatInProcDS = (FP_GetVariableFormatInProcDS)GETADDRESS(pLib,"GetVariableFormatInProcDS");

    SetVarNameAndType = (FP_SetVarNameAndType)GETADDRESS(pLib,"SetVarNameAndType");
    SetVarLabel = (FP_SetVarLabel)GETADDRESS(pLib,"SetVarLabel");
    SetVarCValueLabel = (FP_SetVarCValueLabel)GETADDRESS(pLib,"SetVarCValueLabel");
    SetVarNValueLabel = (FP_SetVarNValueLabel)GETADDRESS(pLib,"SetVarNValueLabel");
    SetVarCMissingValues = (FP_SetVarCMissingValues)GETADDRESS(pLib,"SetVarCMissingValues");
    SetVarNMissingValues = (FP_SetVarNMissingValues)GETADDRESS(pLib,"SetVarNMissingValues");
    SetVarMeasureLevel = (FP_SetVarMeasureLevel)GETADDRESS(pLib,"SetVarMeasureLevel");
    SetVarAlignment = (FP_SetVarAlignment)GETADDRESS(pLib,"SetVarAlignment");
    SetVarFormat = (FP_SetVarFormat)GETADDRESS(pLib,"SetVarFormat");
    CommitHeader = (FP_CommitHeader)GETADDRESS(pLib,"CommitHeader");
    SetValueChar = (FP_SetValueChar)GETADDRESS(pLib,"SetValueChar");
    SetValueNumeric = (FP_SetValueNumeric)GETADDRESS(pLib,"SetValueNumeric");
    CommitCaseRecord = (FP_CommitCaseRecord)GETADDRESS(pLib,"CommitCaseRecord");
    CommitNewCase = (FP_CommitNewCase)GETADDRESS(pLib,"CommitNewCase");
    CommitManyCases = (FP_CommitManyCases)GETADDRESS(pLib,"CommitManyCases");
    EndChanges = (FP_EndChanges)GETADDRESS(pLib,"EndChanges");
    IsEndSplit = (FP_IsEndSplit)GETADDRESS(pLib,"IsEndSplit");
    HasProcedure = (FP_HasProcedure)GETADDRESS(pLib,"HasProcedure");
    GetSPSSLowHigh = (FP_GetSPSSLowHigh)GETADDRESS(pLib,"GetSPSSLowHigh");
    GetWeightVar = (FP_GetWeightVar)GETADDRESS(pLib,"GetWeightVar");
    ResetDataPass = (FP_ResetDataPass)GETADDRESS(pLib,"ResetDataPass");
    AllocNewVarsBuffer = (FP_AllocNewVarsBuffer)GETADDRESS(pLib,"AllocNewVarsBuffer");
    SetOneVarNameAndType = (FP_SetOneVarNameAndType)GETADDRESS(pLib,"SetOneVarNameAndType");
    SetXDriveMode = (FP_SetXDriveMode)GETADDRESS(pLib,"SetXDriveMode");

    StartDataStep = (FP_StartDataStep)GETADDRESS(pLib,"StartDataStep");
    EndDataStep = (FP_EndDataStep)GETADDRESS(pLib,"EndDataStep");
    CreateDataset = (FP_CreateDataset)GETADDRESS(pLib,"CreateDataset");
    SetDatasetName = (FP_SetDatasetName)GETADDRESS(pLib,"SetDatasetName");
    GetNewDatasetName = (FP_GetNewDatasetName)GETADDRESS(pLib,"GetNewDatasetName");
    GetActive = (FP_GetActive)GETADDRESS(pLib,"GetActive");
    SetActive = (FP_SetActive)GETADDRESS(pLib,"SetActive");
    CopyDataset = (FP_CopyDataset)GETADDRESS(pLib,"CopyDataset");
    GetSpssDatasets = (FP_GetSpssDatasets)GETADDRESS(pLib,"GetSpssDatasets");
    GetDatastepDatasets = (FP_GetDatastepDatasets)GETADDRESS(pLib,"GetDatastepDatasets");
    FreeStringArray = (FP_FreeStringArray)GETADDRESS(pLib,"FreeStringArray");
    CloseDataset = (FP_CloseDataset)GETADDRESS(pLib,"CloseDataset");
    InsertVariable = (FP_InsertVariable)GETADDRESS(pLib,"InsertVariable");
    DeleteVariable = (FP_DeleteVariable)GETADDRESS(pLib,"DeleteVariable");
    GetVarCountInDS = (FP_GetVarCountInDS)GETADDRESS(pLib,"GetVarCountInDS");
    GetVarNameInDS = (FP_GetVarNameInDS)GETADDRESS(pLib,"GetVarNameInDS");
    SetVarNameInDS = (FP_SetVarNameInDS)GETADDRESS(pLib,"SetVarNameInDS");
    GetVarLabelInDS = (FP_GetVarLabelInDS)GETADDRESS(pLib,"GetVarLabelInDS");
    SetVarLabelInDS = (FP_SetVarLabelInDS)GETADDRESS(pLib,"SetVarLabelInDS");
    GetVarTypeInDS = (FP_GetVarTypeInDS)GETADDRESS(pLib,"GetVarTypeInDS");
    SetVarTypeInDS = (FP_SetVarTypeInDS)GETADDRESS(pLib,"SetVarTypeInDS");
    GetVarFormatInDS = (FP_GetVarFormatInDS)GETADDRESS(pLib,"GetVarFormatInDS");
    SetVarFormatInDS = (FP_SetVarFormatInDS)GETADDRESS(pLib,"SetVarFormatInDS");
    GetVarAlignmentInDS = (FP_GetVarAlignmentInDS)GETADDRESS(pLib,"GetVarAlignmentInDS");
    SetVarAlignmentInDS = (FP_SetVarAlignmentInDS)GETADDRESS(pLib,"SetVarAlignmentInDS");
    GetVarMeasurementLevelInDS = (FP_GetVarMeasurementLevelInDS)GETADDRESS(pLib,"GetVarMeasurementLevelInDS");
    SetVarMeasurementLevelInDS = (FP_SetVarMeasurementLevelInDS)GETADDRESS(pLib,"SetVarMeasurementLevelInDS");
    GetVarNMissingValuesInDS = (FP_GetVarNMissingValuesInDS)GETADDRESS(pLib,"GetVarNMissingValuesInDS");
    GetVarCMissingValuesInDS = (FP_GetVarCMissingValuesInDS)GETADDRESS(pLib,"GetVarCMissingValuesInDS");
    SetVarNMissingValuesInDS = (FP_SetVarNMissingValuesInDS)GETADDRESS(pLib,"SetVarNMissingValuesInDS");
    SetVarCMissingValuesInDS = (FP_SetVarCMissingValuesInDS)GETADDRESS(pLib,"SetVarCMissingValuesInDS");
    GetVarAttributesNameInDS = (FP_GetVarAttributesNameInDS)GETADDRESS(pLib,"GetVarAttributesNameInDS");
    GetVarAttributesInDS = (FP_GetVarAttributesInDS)GETADDRESS(pLib,"GetVarAttributesInDS");
    SetVarAttributesInDS = (FP_SetVarAttributesInDS)GETADDRESS(pLib,"SetVarAttributesInDS");
    DelVarAttributesInDS = (FP_DelVarAttributesInDS)GETADDRESS(pLib,"DelVarAttributesInDS");
    GetVarNValueLabelInDS = (FP_GetVarNValueLabelInDS)GETADDRESS(pLib,"GetVarNValueLabelInDS");
    FreeDoubleArray = (FP_FreeDoubleArray)GETADDRESS(pLib,"FreeDoubleArray");
    GetVarCValueLabelInDS = (FP_GetVarCValueLabelInDS)GETADDRESS(pLib,"GetVarCValueLabelInDS");
    SetVarNValueLabelInDS = (FP_SetVarNValueLabelInDS)GETADDRESS(pLib,"SetVarNValueLabelInDS");
    SetVarCValueLabelInDS = (FP_SetVarCValueLabelInDS)GETADDRESS(pLib,"SetVarCValueLabelInDS");
    DelVarValueLabelInDS = (FP_DelVarValueLabelInDS)GETADDRESS(pLib,"DelVarValueLabelInDS");
    DelVarNValueLabelInDS = (FP_DelVarNValueLabelInDS)GETADDRESS(pLib,"DelVarNValueLabelInDS");
    DelVarCValueLabelInDS = (FP_DelVarCValueLabelInDS)GETADDRESS(pLib,"DelVarCValueLabelInDS");
    InsertCase = (FP_InsertCase)GETADDRESS(pLib,"InsertCase");
    DeleteCase = (FP_DeleteCase)GETADDRESS(pLib,"DeleteCase");
    GetCaseCountInDS = (FP_GetCaseCountInDS)GETADDRESS(pLib,"GetCaseCountInDS");
    GetNCellValue = (FP_GetNCellValue)GETADDRESS(pLib,"GetNCellValue");
    GetCCellValue = (FP_GetCCellValue)GETADDRESS(pLib,"GetCCellValue");
    SetNCellValue = (FP_SetNCellValue)GETADDRESS(pLib,"SetNCellValue");
    SetCCellValue = (FP_SetCCellValue)GETADDRESS(pLib,"SetCCellValue");

    IsUTF8mode = (FP_IsUTF8mode)GETADDRESS(pLib,"IsUTF8mode");
    GetSplitVariableNames = (FP_GetSplitVariableNames)GETADDRESS(pLib,"GetSplitVariableNames");
    GetDataFileAttributeNames = (FP_GetDataFileAttributeNames)GETADDRESS(pLib,"GetDataFileAttributeNames");
    GetDataFileAttributes = (FP_GetDataFileAttributes)GETADDRESS(pLib,"GetDataFileAttributes");
    GetDataFileAttributeNamesInProcDS = (FP_GetDataFileAttributeNamesInProcDS)GETADDRESS(pLib,"GetDataFileAttributeNamesInProcDS");
    GetDataFileAttributesInProcDS = (FP_GetDataFileAttributesInProcDS)GETADDRESS(pLib,"GetDataFileAttributesInProcDS");
    GetMultiResponseSetNames = (FP_GetMultiResponseSetNames)GETADDRESS(pLib,"GetMultiResponseSetNames");
    GetMultiResponseSet = (FP_GetMultiResponseSet)GETADDRESS(pLib,"GetMultiResponseSet");
    GetMultiResponseSetNamesInProcDS = (FP_GetMultiResponseSetNamesInProcDS)GETADDRESS(pLib,"GetMultiResponseSetNamesInProcDS");
    GetMultiResponseSetInProcDS = (FP_GetMultiResponseSetInProcDS)GETADDRESS(pLib,"GetMultiResponseSetInProcDS");
    SetDataFileAttributesInDS = (FP_SetDataFileAttributesInDS)GETADDRESS(pLib,"SetDataFileAttributesInDS");
    SetMultiResponseSetInDS = (FP_SetMultiResponseSetInDS)GETADDRESS(pLib,"SetMultiResponseSetInDS");
    FreeString = (FP_FreeString)GETADDRESS(pLib,"FreeString");
    GetDataFileAttributeNamesInDS = (FP_GetDataFileAttributeNamesInDS)GETADDRESS(pLib,"GetDataFileAttributeNamesInDS");
    GetDataFileAttributesInDS = (FP_GetDataFileAttributesInDS)GETADDRESS(pLib,"GetDataFileAttributesInDS");
    GetMultiResponseSetNamesInDS = (FP_GetMultiResponseSetNamesInDS)GETADDRESS(pLib,"GetMultiResponseSetNamesInDS");
    GetMultiResponseSetInDS = (FP_GetMultiResponseSetInDS)GETADDRESS(pLib,"GetMultiResponseSetInDS");
    GetVarColumnWidthInDS = (FP_GetVarColumnWidthInDS)GETADDRESS(pLib,"GetVarColumnWidthInDS");
    SetVarColumnWidthInDS = (FP_SetVarColumnWidthInDS)GETADDRESS(pLib,"SetVarColumnWidthInDS");
    SaveFileInDS = (FP_SaveFileInDS)GETADDRESS(pLib,"SaveFileInDS");
    DelDataFileAttributesInDS = (FP_DelDataFileAttributesInDS)GETADDRESS(pLib,"DelDataFileAttributesInDS");
    DelMultiResponseSetInDS = (FP_DelMultiResponseSetInDS)GETADDRESS(pLib,"DelMultiResponseSetInDS");
    GetXDriveMode = (FP_GetXDriveMode)GETADDRESS(pLib,"GetXDriveMode");
    GetSPSSLocale = (FP_GetSPSSLocale)GETADDRESS(pLib,"GetSPSSLocale");
    GetCLocale = (FP_GetCLocale)GETADDRESS(pLib,"GetCLocale");
    SetOutputLanguage = (FP_SetOutputLanguage)GETADDRESS(pLib,"SetOutputLanguage");
    GetNestDepth = (FP_GetNestDepth)GETADDRESS(pLib,"GetNestDepth");
    GetOutputLanguage = (FP_GetOutputLanguage)GETADDRESS(pLib,"GetOutputLanguage");
    
    TransCode = (FP_TransCode)GETADDRESS(pLib, "TransCode");

    GetNCellValueCache = (FP_GetNCellValueCache)GETADDRESS(pLib, "GetNCellValueCache");
    GetCCellValueCache = (FP_GetCCellValueCache)GETADDRESS(pLib, "GetCCellValueCache");
    GetRowList = (FP_GetRowList)GETADDRESS(pLib, "GetRowList");
    GetVarTypeInDSCache = (FP_GetVarTypeInDSCache)GETADDRESS(pLib, "GetVarTypeInDSCache");
    ClearDatastepBatch = (FP_ClearDatastepBatch)GETADDRESS(pLib, "ClearDatastepBatch");
    
    GetVariableRole = (FP_GetVariableRole)GETADDRESS(pLib, "GetVariableRole");
    GetVariableRoleInProcDS = (FP_GetVariableRoleInProcDS)GETADDRESS(pLib, "GetVariableRoleInProcDS");
    SetVarRole = (FP_SetVarRole)GETADDRESS(pLib, "SetVarRole");
    GetVarRoleInDS = (FP_GetVarRoleInDS)GETADDRESS(pLib, "GetVarRoleInDS");
    SetVarRoleInDS = (FP_SetVarRoleInDS)GETADDRESS(pLib, "SetVarRoleInDS");
    
    TransportData = (FP_TransportData)GETADDRESS(pLib,"TransportData");
    GetDataFromTempFile = (FP_GetDataFromTempFile)GETADDRESS(pLib,"GetDataFromTempFile");
    SaveDataToTempFile = (FP_SaveDataToTempFile)GETADDRESS(pLib,"SaveDataToTempFile");
    GetSplitEndIndex = (FP_GetSplitEndIndex)GETADDRESS(pLib,"GetSplitEndIndex");
    SetMode = (FP_SetMode)GETADDRESS(pLib,"SetMode");
    GetRowCountInTempFile = (FP_GetRowCountInTempFile)GETADDRESS(pLib, "GetRowCountInTempFile");
    
    GetNCellValueFromCache = (FP_GetNCellValueFromCache)GETADDRESS(pLib,"GetNCellValueFromCache");
    GetCCellValueFromCache = (FP_GetCCellValueFromCache)GETADDRESS(pLib,"GetCCellValueFromCache");
    GetVarInfo = (FP_GetVarInfo)GETADDRESS(pLib,"GetVarInfo");
    SetNCellValueFromCache = (FP_SetNCellValueFromCache)GETADDRESS(pLib,"SetNCellValueFromCache");
    SetCCellValueFromCache = (FP_SetCCellValueFromCache)GETADDRESS(pLib,"SetCCellValueFromCache");
    GetCaseValue = (FP_GetCaseValue)GETADDRESS(pLib,"GetCaseValue");
    SetCasePartValue = (FP_SetCasePartValue)GETADDRESS(pLib,"SetCasePartValue");
    SetCacheInDS = (FP_SetCacheInDS)GETADDRESS(pLib,"SetCacheInDS");
    
    IsDistributedMode = (FP_IsDistributedMode)GETADDRESS(pLib,"IsDistributedMode");
    IsUseOrFilter = (FP_IsUseOrFilter)GETADDRESS(pLib,"IsUseOrFilter");
    
    MoveVariables = (FP_MoveVariables)GETADDRESS(pLib,"MoveVariables");
    
    CreateSocket = (FP_CreateSocket)GETADDRESS(pLib,"CreateSocket");
    AcceptSocket = (FP_AcceptSocket)GETADDRESS(pLib,"AcceptSocket");
    GetTrendsInfoInDS = (FP_GetTrendsInfoInDS)GETADDRESS(pLib,"GetTrendsInfoInDS");
    GetSelectedRowCountInProcDS = (FP_GetSelectedRowCountInProcDS)GETADDRESS(pLib, "GetSelectedRowCountInProcDS");
  }

  //load spssxd.dll
  int LoadLib()
  {

    // The object holding the new PATH env string cannot go out of scope.
    const char* spsshome = NULL;
    char* libPath = NULL;
    char* spssPath = NULL;
    if (getenv("SPSS_HOME")) {
        spsshome = getenv("SPSS_HOME");
    }
    if (NULL == spsshome){
        libPath = new char[strlen(libName)+1];
        strcpy(libPath,libName);
    }
    else{
#ifdef MS_WINDOWS
    libPath = new char[strlen(spsshome) + strlen(libName) + 3];
    strcpy(libPath,spsshome);
    strcat(libPath,"\\");
    strcat(libPath,libName);
#else
    libPath = new char[strlen(spsshome) + strlen(libName) + 6];
    strcpy(libPath,spsshome);
    strcat(libPath,"/lib/");
    strcat(libPath,libName);
    spssPath = new char[strlen(spsshome) + 6];
    strcpy(spssPath,spsshome);
    strcat(spssPath,"/lib/");
#endif
}
    if(NULL == pLib) {

#ifdef MS_WINDOWS
         //find out spssxd module, it will success when spss drive
         pLib = GetModuleHandle(libPath);
         //find out spssxd module failure, load it.
         if(NULL == pLib){
             pLib = LoadLibraryEx(libPath,NULL,LOAD_WITH_ALTERED_SEARCH_PATH);
         }
         // force to load libifcoremd.dll
         if (NULL == pLib1){
           pLib1 = LoadLibrary("libifcoremd.dll");
         }
#else
         int mode;
         #ifdef __MACOSX__
             mode = RTLD_LOCAL;
         #else
             mode = RTLD_NOW | RTLD_GLOBAL;
         #endif
#ifdef __UNX_LINUX__
         char* spsslibPath = NULL;
         spsslibPath = new char[strlen(spssPath) + strlen("libcxaguard.so.5") + 1];
         strcpy(spsslibPath,spssPath);
         strcat(spsslibPath,"libirc.so");
         pLib1 = dlopen(spsslibPath,mode);
         strcpy(spsslibPath,spssPath);
         strcat(spsslibPath,"libimf.so");
         pLib2 = dlopen(spsslibPath,mode);
         strcpy(spsslibPath,spssPath);
         strcat(spsslibPath,"libcxaguard.so.5");
         pLib3 = dlopen(spsslibPath,mode);
#endif
#ifdef __UNX_SUNOS__
         char* spsslibPath = NULL;
         spsslibPath = new char[strlen(spssPath) + strlen("libsunmath.so.1") + 1];
         strcpy(spsslibPath,spssPath);
         strcat(spsslibPath,"libm9x.so.0");
         pLib1 = dlopen(spsslibPath,mode);
         strcpy(spsslibPath,spssPath);
         strcat(spsslibPath,"libsunmath.so.1");
         pLib2 = dlopen(spsslibPath,mode);
         strcpy(spsslibPath,spssPath);
         strcat(spsslibPath,"libfsu.so.1");
         pLib3 = dlopen(spsslibPath,mode);
#endif
         pLib = dlopen(libPath,mode);
#endif
    }
   if (pLib) {
     InitializeFP();
   }
   else {//load failure
       #ifndef MS_WINDOWS
       char *perr = dlerror();
       if(perr) {
           printf("dlopen fails with error: %s.\n",perr);
       }
       #endif
       delete []libPath;
       return LOAD_FAIL;
   }
   delete []libPath;
   return LOAD_SUCCESS;
  }

  //unload spssxd.dll
  void FreeLib()
  {
#ifdef MS_WINDOWS
    FreeLibrary(pLib);
#else
    dlclose(pLib);
#endif

    pLib= NULL;
    IsBackendReady = NULL;
    IsXDriven = NULL;
    SetXNameAndSHome = NULL;
    StartSpss = NULL;
    StopSpss = NULL;
    Submit = NULL;
    QueueCommandPart = NULL;
    PostSpssOutput = NULL;
    GetVariableCount = NULL;
    GetRowCount = NULL;
    GetVariableName = NULL;
    GetVariableLabel = NULL;
    GetVariableType = NULL;
    GetVariableFormat = NULL;
    GetVariableMeasurementLevel = NULL;
    CreateXPathDictionary = NULL;
    RemoveXPathHandle = NULL;
    EvaluateXPath = NULL;
    GetStringListLength = NULL;
    GetStringFromList = NULL;
    RemoveStringList = NULL;
    GetXmlUtf16 = NULL;
    GetImage = NULL;
    GetSetting = NULL;
    GetOMSTagList = NULL;
    GetHandleList = NULL;
    GetFileHandles = NULL;
    GetNumericValue = NULL;
    GetStringValue = NULL;
    NextCase = NULL;
    RemoveCaseCursor = NULL;
    GetVariableFormatType = NULL;
    GetCursorPosition = NULL;
    MakeCaseCursor = NULL;
    GetColumnCountInProcDS = NULL;
    StartProcedure = NULL;
    SplitChange = NULL;
    EndProcedure = NULL;

    StartPivotTable = NULL;
    HidePivotTableTitle = NULL;
    PivotTableCaption = NULL;
    AddDimension = NULL;
    AddNumberCategory = NULL;
    AddStringCategory = NULL;
    AddVarNameCategory = NULL;
    AddVarValueDoubleCategory = NULL;
    AddVarValueStringCategory = NULL;
    SetNumberCell = NULL;
    SetStringCell = NULL;
    SetVarNameCell = NULL;
    SetVarValueDoubleCell = NULL;
    SetVarValueStringCell = NULL;
    AddCellFootnotes = NULL;

    AddProcFootnotes = NULL;
    AddOutlineFootnotes = NULL;
    AddTitleFootnotes = NULL;
    AddDimFootnotes = NULL;
    AddCategoryFootnotes = NULL;


    AddTextBlock = NULL;
    MinDataColumnWidth = NULL;
    MaxDataColumnWidth = NULL;
    SetFormatSpecCoefficient = NULL;
    SetFormatSpecCoefficientSE = NULL;
    SetFormatSpecCoefficientVar = NULL;
    SetFormatSpecCorrelation = NULL;
    SetFormatSpecGeneralStat = NULL;
    SetFormatSpecMean = NULL;
    SetFormatSpecCount = NULL;
    SetFormatSpecPercent = NULL;
    SetFormatSpecPercentNoSign = NULL;
    SetFormatSpecProportion = NULL;
    SetFormatSpecSignificance = NULL;
    SetFormatSpecResidual = NULL;
    SetFormatSpecVariable = NULL;
    SetFormatSpecStdDev = NULL;
    SetFormatSpecDifference = NULL;
    SetFormatSpecSum = NULL;

    CommitCaseRecord = NULL;
    CommitNewCase = NULL;
    CommitManyCases = NULL;
    EndChanges = NULL;

    HasCursor = NULL;
    IsEndSplit = NULL;
    HasProcedure = NULL;
    GetSPSSLowHigh = NULL;
    GetWeightVar = NULL;
    ResetDataPass = NULL;
    AllocNewVarsBuffer = NULL;
    SetOneVarNameAndType = NULL;
    GetRowCountInProcDS = NULL;
    GetVariableCountInProcDS = NULL;
    GetVariableLabelInProcDS = NULL;
    GetVariableMeasurementLevelInProcDS = NULL;
    GetVariableFormatTypeInProcDS = NULL;

    GetVariableNameInProcDS  = NULL;
    GetVariableTypeInProcDS  = NULL;
    GetVarAttributeNamesInProcDS  = NULL;
    GetVarAttributesInProcDS  = NULL;
    GetVarCMissingValuesInProcDS  = NULL;
    GetVarNMissingValuesInProcDS  = NULL;
    SetXDriveMode = NULL;

    StartDataStep = NULL;
    EndDataStep = NULL;
    CreateDataset = NULL;
    SetDatasetName = NULL;
    GetNewDatasetName = NULL;
    GetActive = NULL;
    SetActive = NULL;
    CopyDataset = NULL;
    GetSpssDatasets = NULL;
    GetDatastepDatasets = NULL;
    FreeStringArray = NULL;
    CloseDataset = NULL;
    InsertVariable = NULL;
    DeleteVariable = NULL;
    GetVarCountInDS = NULL;
    GetVarNameInDS = NULL;
    SetVarNameInDS = NULL;
    GetVarLabelInDS = NULL;
    SetVarLabelInDS = NULL;
    GetVarTypeInDS = NULL;
    SetVarTypeInDS = NULL;
    GetVarFormatInDS = NULL;
    SetVarFormatInDS = NULL;
    GetVarAlignmentInDS = NULL;
    SetVarAlignmentInDS = NULL;
    GetVarMeasurementLevelInDS = NULL;
    SetVarMeasurementLevelInDS = NULL;
    GetVarNMissingValuesInDS = NULL;
    GetVarCMissingValuesInDS = NULL;
    SetVarNMissingValuesInDS = NULL;
    SetVarCMissingValuesInDS = NULL;
    GetVarAttributesNameInDS = NULL;
    GetVarAttributesInDS = NULL;
    SetVarAttributesInDS = NULL;
    DelVarAttributesInDS = NULL;
    GetVarNValueLabelInDS = NULL;
    FreeDoubleArray = NULL;
    GetVarCValueLabelInDS = NULL;
    SetVarNValueLabelInDS = NULL;
    SetVarCValueLabelInDS = NULL;
    DelVarValueLabelInDS = NULL;
    DelVarNValueLabelInDS = NULL;
    DelVarCValueLabelInDS = NULL;
    InsertCase = NULL;
    DeleteCase = NULL;
    GetCaseCountInDS = NULL;
    GetNCellValue = NULL;
    GetCCellValue = NULL;
    SetNCellValue = NULL;
    SetCCellValue = NULL;

    IsUTF8mode = NULL;
    GetDataFileAttributeNames = NULL;
    GetDataFileAttributes = NULL;
    GetDataFileAttributeNamesInProcDS = NULL;
    GetDataFileAttributesInProcDS = NULL;
    GetMultiResponseSetNames = NULL;
    GetMultiResponseSet = NULL;
    GetMultiResponseSetNamesInProcDS = NULL;
    GetMultiResponseSetInProcDS = NULL;
    SetDataFileAttributesInDS = NULL;
    SetMultiResponseSetInDS = NULL;
    FreeString = NULL;
    GetDataFileAttributeNamesInDS = NULL;
    GetDataFileAttributesInDS = NULL;
    GetMultiResponseSetNamesInDS = NULL;
    GetMultiResponseSetInDS = NULL;
    GetVarColumnWidthInDS = NULL;
    SetVarColumnWidthInDS = NULL;
    SaveFileInDS = NULL;
    DelDataFileAttributesInDS = NULL;
    DelMultiResponseSetInDS = NULL;
    GetXDriveMode = NULL;
    GetSPSSLocale = NULL;
    GetCLocale = NULL;
    SetOutputLanguage = NULL;
    GetNestDepth = NULL;
    GetOutputLanguage = NULL;
    TransCode = NULL;

    GetNCellValueCache = NULL;
    GetCCellValueCache = NULL;
    GetRowList = NULL;
    GetVarTypeInDSCache = NULL;
    ClearDatastepBatch = NULL;
    
    GetVariableRole = NULL;
    GetVariableRoleInProcDS = NULL;
    SetVarRole = NULL;
    GetVarRoleInDS = NULL;
    SetVarRoleInDS = NULL;
    
    TransportData = NULL;
    GetDataFromTempFile = NULL;
    SaveDataToTempFile = NULL;
    GetSplitEndIndex = NULL;
    SetMode = NULL;
    GetRowCountInTempFile = NULL;
    
    GetNCellValueFromCache = NULL;
    GetCCellValueFromCache = NULL;
    SetNCellValueFromCache = NULL;
    SetCCellValueFromCache = NULL;
    GetVarInfo = NULL;
    GetCaseValue = NULL;
    SetCasePartValue = NULL;
    SetCacheInDS = NULL;
    
    IsDistributedMode = NULL;
    IsUseOrFilter = NULL;
    
    MoveVariables = NULL;
    
    CreateSocket = NULL;
    AcceptSocket = NULL;
    GetTrendsInfoInDS = NULL;
    GetSelectedRowCountInProcDS = NULL;
  }

    //======================================================================
    PyObject *
    GetCaseCountAll_XD(PyObject *self, PyObject *args, const int mode)
    {
        long count;
        int errLevel;
        if(mode == DS_MODE) {
            count = GetRowCount(errLevel);
        } else {
            count = GetRowCountInProcDS(errLevel);
        }

        return Py_BuildValue("li", count, errLevel);
    }

    PyObject *
    GetVariableCountAll_XD(PyObject *self, PyObject *args, const int mode)
    {
        int count;
        int errLevel;

        if(mode == DS_MODE) {
            count = GetVariableCount(errLevel);
        } else {
            count = GetVariableCountInProcDS(errLevel);
        }

        return Py_BuildValue("ii", count, errLevel);
    }

    PyObject *
    GetVariableFormatAll_XD(PyObject *self, PyObject *args, const int mode)
    {
        int index;
        const char* format;
        if (!PyArg_ParseTuple(args, "i", &index)) {
            return NULL;
        }

        int errLevel;
        if(mode == DS_MODE) {
            format = GetVariableFormat(index,errLevel);
        } else {
            format = GetVariableFormat(index,errLevel);
        }
        if(IsUTF8mode()&&(errLevel==0))
        {
            PyObject* pyValue = PyUnicode_Decode(format,strlen(format),"utf-8",NULL);
            PyObject* r = Py_BuildValue("Oi", pyValue, errLevel);
            Py_XDECREF(pyValue);
            return r;
        }
        else
        {
            return Py_BuildValue("si", format, errLevel);
        }
    }

    PyObject *
    GetVariableLabelAll_XD(PyObject *self, PyObject *args, const int mode)
    {
        int index;
        const char* label;
        if (!PyArg_ParseTuple(args, "i", &index)) {
            return NULL;
        }

        int errLevel;

        if(mode == DS_MODE) {
            label = GetVariableLabel(index, errLevel);
        } else {
            label = GetVariableLabelInProcDS(index, errLevel);
        }
        if(IsUTF8mode()&&(errLevel==0))
        {
            PyObject* pyValue = PyUnicode_Decode(label,strlen(label),"utf-8",NULL);
            PyObject* r = Py_BuildValue("Oi", pyValue, errLevel);
            Py_XDECREF(pyValue);
            return r;
        }
        else
        {
            return Py_BuildValue("si", label, errLevel);
        }
    }

    PyObject *
    GetVariableMeasurementLevelAll_XD(PyObject *self, PyObject *args,const int mode)
    {
        int index;
        int measurementLevel;
        if (!PyArg_ParseTuple(args, "i", &index)) {
            return NULL;
        }

        int errLevel;

        if(mode == DS_MODE) {
            measurementLevel = GetVariableMeasurementLevel(index,errLevel);
        } else {
            measurementLevel = GetVariableMeasurementLevelInProcDS(index,errLevel);
        }

        return Py_BuildValue("ii", measurementLevel,errLevel);
    }

    PyObject *
    GetVariableNameAll_XD(PyObject *self, PyObject *args, const int mode)
    {
        int index;
        const char* name;
        if (!PyArg_ParseTuple(args, "i", &index)) {
            return NULL;
        }

        int errLevel;

        if(mode == DS_MODE) {
            name = GetVariableName(index,errLevel);
        } else {
            name = GetVariableNameInProcDS(index,errLevel);
        }
        if(IsUTF8mode()&&(errLevel==0))
        {
            PyObject* pyValue = PyUnicode_Decode(name,strlen(name),"utf-8",NULL);
            PyObject* r = Py_BuildValue("Oi", pyValue,errLevel);
            Py_XDECREF(pyValue);
            return r;
        }
        else
        {
            return Py_BuildValue("si", name,errLevel);
        }
    }

    PyObject *
    GetVariableTypeAll_XD(PyObject *self, PyObject *args,const int mode)
    {
        int index;
        int type;
        if (!PyArg_ParseTuple(args, "i", &index)) {
            return NULL;
        }

        int errLevel;

        if(mode == DS_MODE) {
            type = GetVariableType(index,errLevel);
        } else {
            type = GetVariableTypeInProcDS(index,errLevel);
        }

        return Py_BuildValue("ii", type,errLevel);
    }

    PyObject *
    GetVarCMissingValuesAll_XD( PyObject *self, PyObject *args,const int mode)
    {
        int error = 0 ;
        int varIndex;
        int missingFormat = 0;
        char missingVal1[MISSING_VAL_LEN];
        char missingVal2[MISSING_VAL_LEN];
        char missingVal3[MISSING_VAL_LEN];

        memset(missingVal1,'\0',MISSING_VAL_LEN);
        memset(missingVal2,'\0',MISSING_VAL_LEN);
        memset(missingVal3,'\0',MISSING_VAL_LEN);

        if(!PyArg_ParseTuple(args,"i",&varIndex)) {
            return NULL;
        }

        if(mode == DS_MODE) {
            error = GetVarCMissingValues(varIndex,&missingFormat,missingVal1,missingVal2,missingVal3);
        } else {
            error = GetVarCMissingValuesInProcDS(varIndex,&missingFormat,missingVal1,missingVal2,missingVal3);
        }

        PyObject *result = NULL;

        int utf8mode = IsUTF8mode();
        switch(missingFormat)
        {
            case 0:
            {
                result = Py_BuildValue("iOOOi",missingFormat,Py_None,Py_None,Py_None,error);
            }
            break;
            case 1:
            {
                if(utf8mode)
                {
                    PyObject* pyValue = PyUnicode_Decode(missingVal1,strlen(missingVal1),"utf-8",NULL);
                    result = Py_BuildValue("iOOOi",missingFormat,pyValue,Py_None,Py_None,error);
                    Py_XDECREF(pyValue);
                }
                else
                {
                    result = Py_BuildValue("isOOi",missingFormat,missingVal1,Py_None,Py_None,error);
                }
            }
            break;
            case 2:
            {
                if(utf8mode)
                {
                    PyObject* pyValueOne = PyUnicode_Decode(missingVal1,strlen(missingVal1),"utf-8",NULL);
                    PyObject* pyValueTwo = PyUnicode_Decode(missingVal2,strlen(missingVal2),"utf-8",NULL);
                    result = Py_BuildValue("iOOOi",missingFormat,pyValueOne,pyValueTwo,Py_None,error);
                    Py_XDECREF(pyValueOne);
                    Py_XDECREF(pyValueTwo);
                }
                else
                {
                    result = Py_BuildValue("issOi",missingFormat,missingVal1,missingVal2,Py_None,error);
                }
            }
            break;
            case 3:
            {
                if(utf8mode)
                {
                    PyObject* pyValueOne = PyUnicode_Decode(missingVal1,strlen(missingVal1),"utf-8",NULL);
                    PyObject* pyValueTwo = PyUnicode_Decode(missingVal2,strlen(missingVal2),"utf-8",NULL);
                    PyObject* pyValueThree = PyUnicode_Decode(missingVal3,strlen(missingVal3),"utf-8",NULL);
                    result = Py_BuildValue("iOOOi",missingFormat,pyValueOne,pyValueTwo,pyValueThree,error);
                    Py_XDECREF(pyValueOne);
                    Py_XDECREF(pyValueTwo);
                    Py_XDECREF(pyValueThree);
                }
                else
                {
                    result = Py_BuildValue("isssi",missingFormat,missingVal1,missingVal2,missingVal3,error);
                }
            }
            break;
            default:
            {
            }
            break;
        }

        return result;
    }

    PyObject *
    GetVarNMissingValuesAll_XD(PyObject *self,PyObject *args,const int mode)
    {
        int error = 0;
        int varIndex;
        int missingFormat;
        double missingVal1;
        double missingVal2;
        double missingVal3;
        static const int DISCRETE_TYPE = 0;
        static const int RANGE_TYPE = 1;
        static const int RANGE_DISCRETE_TYPE = 2;
        int format = 0;

        if(!PyArg_ParseTuple(args,"i",&varIndex)) {
            return NULL;
        }

        if(mode == DS_MODE) {
            error = GetVarNMissingValues(varIndex,&missingFormat,&missingVal1,&missingVal2,&missingVal3);
        } else {
            error = GetVarNMissingValuesInProcDS(varIndex,&missingFormat,&missingVal1,&missingVal2,&missingVal3);
        }

        PyObject *result = NULL;

        if(missingFormat == -3) {
            format = RANGE_DISCRETE_TYPE;
        } else if(missingFormat == -2) {
            format = RANGE_TYPE;
        } else {
            format = DISCRETE_TYPE;
        }

        switch(missingFormat)
        {
            case 0:
            {
                result = Py_BuildValue("iOOOi",format,Py_None,Py_None,Py_None,error);
            }
            break;
            case 1:
            {
                result = Py_BuildValue("idOOi",format,missingVal1,Py_None,Py_None,error);
            }
            break;
            case 2:
            case -2:
            {
                result = Py_BuildValue("iddOi",format,missingVal1,missingVal2,Py_None,error);
            }
            break;
            case 3:
            case -3:
            {
                result = Py_BuildValue("idddi",format,missingVal1,missingVal2,missingVal3,error);
            }
            break;
            default:
            {
            }
            break;
        }

        return result;
    }

    PyObject *
    GetVarAttributeNamesAll_XD( PyObject *self, PyObject *args,const int mode)
    {
        int error = 0;
        int varIndex;
        char **names = NULL;
        int numOfNames;

        if(!PyArg_ParseTuple(args,"i",&varIndex)) {
            return NULL;
        }

        if(mode == DS_MODE) {
            error = GetVarAttributeNames(varIndex,&names,&numOfNames);
        } else {
            error = GetVarAttributeNamesInProcDS(varIndex,&names,&numOfNames);
        }

        PyObject *pyNames = NULL;

        if(0 == error) {
            pyNames = PyTuple_New(numOfNames);
            for(int i = 0; i < numOfNames; i++) {
                if(IsUTF8mode()) {
                    PyObject* pyValue = PyUnicode_Decode(names[i],strlen(names[i]),"utf-8",NULL);
                    PyTuple_SetItem(pyNames,i,Py_BuildValue("O",pyValue));
                    Py_XDECREF(pyValue);
                } else {
                    PyTuple_SetItem(pyNames,i,Py_BuildValue("s",names[i]));
                }
            }

            FreeAttributeNames(names,numOfNames);
            PyObject* r = Py_BuildValue("Oi",pyNames,error);
            Py_DECREF(pyNames);

            return r;
        } else {
            return Py_BuildValue("Oi",Py_None,error);
        }
    }

    PyObject *
    GetVarAttributesAll_XD( PyObject *self, PyObject *args, const int mode)
    {
        int error = 0;
        int varIndex;
        char *attrName = NULL;
        char **attrValues = NULL;
        int numOfValues;

        if(!PyArg_ParseTuple(args,"is",&varIndex,&attrName)) {
            return NULL;
        }

        if(mode == DS_MODE) {
            error = GetVarAttributes(varIndex,attrName,&attrValues,&numOfValues);
        } else {
            error = GetVarAttributesInProcDS(varIndex,attrName,&attrValues,&numOfValues);
        }

        PyObject *pyValues = NULL;

        if(0 == error) {
            pyValues = PyTuple_New(numOfValues);
            for(int i = 0; i < numOfValues; i++) {
                if(IsUTF8mode()) {
                    PyObject* pyValue = PyUnicode_Decode(attrValues[i],strlen(attrValues[i]),"utf-8",NULL);
                    PyTuple_SetItem(pyValues,i,Py_BuildValue("O",pyValue));
                    Py_XDECREF(pyValue);
                } else {
                    PyTuple_SetItem(pyValues,i,Py_BuildValue("s",attrValues[i]));
                }
            }

            FreeAttributes(attrValues,numOfValues);
            PyObject* r = Py_BuildValue("Oi",pyValues,error);
            Py_DECREF(pyValues);

            return r;
        } else {
            return Py_BuildValue("Oi",Py_None,error);
        }
    }

    PyObject * GetDataFileAttributeNamesAll_XD( PyObject *self,PyObject *args, const int mode)
    {
        int error = 0;
        char **names = NULL;
        int numOfNames;

        if(mode == DS_MODE) {
            error = GetDataFileAttributeNames(&names,&numOfNames);
        } else {
            error = GetDataFileAttributeNamesInProcDS(&names,&numOfNames);
        }

        PyObject *pyNames = NULL;

        if(0 == error) {
            pyNames = PyTuple_New(numOfNames);
            for(int i = 0; i < numOfNames; i++) {
                if(IsUTF8mode()) {
                    PyObject* pyValue = PyUnicode_Decode(names[i],strlen(names[i]),"utf-8",NULL);
                    PyTuple_SetItem(pyNames,i,Py_BuildValue("O",pyValue));
                    Py_XDECREF(pyValue);
                } else {
                    PyTuple_SetItem(pyNames,i,Py_BuildValue("s",names[i]));
                }
            }

            FreeAttributeNames(names,numOfNames);
            PyObject* r = Py_BuildValue("Oi",pyNames,error);
            Py_DECREF(pyNames);

            return r;
        } else {
            return Py_BuildValue("Oi",Py_None,error);
        }
    }

    PyObject * GetDataFileAttributesAll_XD( PyObject *self,PyObject *args, const int mode)
    {
        int error = 0;
        char *attrName = NULL;
        char **attrValues = NULL;
        int numOfValues;

        if(!PyArg_ParseTuple(args,"s",&attrName)) {
            return NULL;
        }

        if(mode == DS_MODE) {
            error = GetDataFileAttributes(attrName,&attrValues,&numOfValues);
        } else {
            error = GetDataFileAttributesInProcDS(attrName,&attrValues,&numOfValues);
        }

        PyObject *pyValues = NULL;

        if(0 == error) {
            pyValues = PyTuple_New(numOfValues);
            for(int i = 0; i < numOfValues; i++) {
                if(IsUTF8mode()) {
                    PyObject* pyValue = PyUnicode_Decode(attrValues[i],strlen(attrValues[i]),"utf-8",NULL);
                    PyTuple_SetItem(pyValues,i,Py_BuildValue("O",pyValue));
                    Py_XDECREF(pyValue);
                } else {
                    PyTuple_SetItem(pyValues,i,Py_BuildValue("s",attrValues[i]));
                }
            }

            FreeAttributes(attrValues,numOfValues);
            PyObject* r = Py_BuildValue("Oi",pyValues,error);
            Py_DECREF(pyValues);

            return r;
        } else {
            return Py_BuildValue("Oi",Py_None,error);
        }
    }


    PyObject * GetMultiResponseSetNamesAll_XD(PyObject *self, PyObject *args, const int mode)
    {
        char* handle = 0;
        char* context = 0;
        char* expression = 0;
        int errLevel = 0;
        void *result;
        if(DS_MODE == mode) {
            result = GetMultiResponseSetNames(errLevel);
        } else {
            result = GetMultiResponseSetNamesInProcDS(errLevel);
        }
        int size = GetStringListLength(result);

        if ( 0 != errLevel || 0 == size || 0 == result){
            return Py_BuildValue("[i]",errLevel);
        }

        //pass the result to python object
        PyObject* outList = PyList_New(size);
        for( int i = 0; i < size; i++){
            PyObject* item ;
            const char *str = GetStringFromList(result,i);
            if(IsUTF8mode()) {
                PyObject* pyValue = PyUnicode_Decode(str,strlen(str),"utf-8",NULL);
                item = Py_BuildValue("O",pyValue);
                Py_XDECREF(pyValue);
            } else {
                item = Py_BuildValue("s",str);
            }
            PyList_SET_ITEM(outList,i,item);
        }
        RemoveStringList(result);
        PyObject* r = Py_BuildValue("Oi",outList,errLevel);
        Py_DECREF(outList);

        return r;
    }

    PyObject * GetMultiResponseSetAll_XD( PyObject *self,PyObject *args, const int mode)
    {
        int errLevel = 0;
        char *mrsetName = NULL;
        char *mrsetLabel, *mrsetCountedValue;
        void *elemVarNames;
        int mrsetCodeAs,mrsetDataType;

        if(!PyArg_ParseTuple(args,"s",&mrsetName)) {
            return NULL;
        }

        if(mode == DS_MODE) {
            GetMultiResponseSet(mrsetName,&mrsetLabel,mrsetCodeAs,&mrsetCountedValue,mrsetDataType,&elemVarNames,errLevel);
        } else {
            GetMultiResponseSetInProcDS(mrsetName,&mrsetLabel,mrsetCodeAs,&mrsetCountedValue,mrsetDataType,&elemVarNames,errLevel);
        }

        PyObject *pyValues = NULL;

        if(0 == errLevel) {
            int size = GetStringListLength(elemVarNames);
            pyValues = PyList_New(size);

            for(int i = 0; i < size; i++) {
                const char *str = GetStringFromList(elemVarNames,i);
                if(IsUTF8mode()) {
                    PyObject* pyValue = PyUnicode_Decode(str,strlen(str),"utf-8",NULL);
                    PyList_SetItem(pyValues,i,Py_BuildValue("O",pyValue));
                    Py_XDECREF(pyValue);
                } else {
                    PyList_SetItem(pyValues,i,Py_BuildValue("s",str));
                }
            }

            char *codeAs = 0, *dataType = 0;
            if (mrsetCodeAs == 1)
                codeAs = "Categories";
            else if (mrsetCodeAs == 2)
                codeAs = "Dichotomies";
            else
                codeAs = "Unknown";
            if (mrsetDataType == 0)
                dataType = "Numeric";
            else
                dataType = "String";
            PyObject *ret;
            if(IsUTF8mode()) {
                PyObject* pyValueOne = PyUnicode_Decode(mrsetLabel,strlen(mrsetLabel),"utf-8",NULL);
                PyObject* pyValueTwo = PyUnicode_Decode(codeAs,strlen(codeAs),"utf-8",NULL);
                PyObject* pyValueThree = PyUnicode_Decode(mrsetCountedValue,strlen(mrsetCountedValue),"utf-8",NULL);
                PyObject* pyValueFour = PyUnicode_Decode(dataType,strlen(dataType),"utf-8",NULL);
                ret = Py_BuildValue("OOOOOi",pyValueOne,pyValueTwo,pyValueThree,pyValueFour,pyValues,errLevel);
                Py_XDECREF(pyValueOne);
                Py_XDECREF(pyValueTwo);
                Py_XDECREF(pyValueThree);
                Py_XDECREF(pyValueFour);
            } else {
               ret = Py_BuildValue("ssssOi",mrsetLabel,codeAs,mrsetCountedValue,dataType,pyValues,errLevel);
            }

            FreeString(mrsetLabel);
            FreeString(mrsetCountedValue);
            RemoveStringList(elemVarNames);
            if ( pyValues )
                Py_DECREF(pyValues);

            return ret;
        } else {
            return Py_BuildValue("[i]",errLevel);
        }
    }
//========================================================================

  PyObject *
  ext_QueueCommandPart(PyObject *self, PyObject *args)
  {
    char* command;
    int length;
    int errLevel;
    if (!PyArg_ParseTuple(args, "si", &command,&length))
      return NULL;
    errLevel = QueueCommandPart(command,length);
    return Py_BuildValue("i", errLevel);
  }
  PyObject *
  ext_CreateXPathDictionary(PyObject *self, PyObject *args)
  {
    char* handle;
    int result;
    if (!PyArg_ParseTuple(args, "s", &handle))
      return NULL;
    result = CreateXPathDictionary(handle);
    return Py_BuildValue("i", result);
  }
  PyObject *
  ext_RemoveXPathHandle(PyObject *self, PyObject *args)
  {
    char* handle;
    int result;
    if (!PyArg_ParseTuple(args, "s", &handle))
      return NULL;
    result = RemoveXPathHandle(handle);
    return Py_BuildValue("i", result);
  }

  PyObject *
  ext_Submit(PyObject *self, PyObject *args)
  {
    char* command;
    int length;
    int errLevel;
    if (!PyArg_ParseTuple(args, "si", &command,&length)) {
        return NULL;
    }
    SetXDriveMode(1);

    enum XDriveMode {
        kInvalidMode = -1,
        kSPSSMode,
        kPythonMode,
        kOthers
    };

    int currMode=0, originMode=0;
    errLevel = GetXDriveMode(currMode, originMode);
    if ( kPythonMode == originMode ){
        if ( StartReceivePyThread ){
            errLevel = StartReceivePyThread();
        }
    }
    
    errLevel = Submit(command,length);
    if ( 0 == errLevel && GetCLocale){
        //reset the locale to xdlocale.
        char *xdlocale = 0;
        errLevel = GetCLocale(&xdlocale);
        if ( xdlocale ){
            setlocale(LC_ALL, xdlocale);
        }
        FreeString( xdlocale );
    }

    return Py_BuildValue("i", errLevel);
  }

  PyObject *
  ext_GetCaseCount(PyObject *self, PyObject *args)
  {
      return GetCaseCountAll_XD(self,args,DS_MODE);
  }

  PyObject *
  ext_GetVariableCount(PyObject *self, PyObject *args)
  {
      return GetVariableCountAll_XD(self,args,DS_MODE);
  }

  PyObject *
  ext_GetVariableFormat(PyObject *self, PyObject *args)
  {
      return GetVariableFormatAll_XD(self,args,DS_MODE);
  }
  PyObject *
  ext_GetVariableLabel(PyObject *self, PyObject *args)
  {
      return GetVariableLabelAll_XD(self,args,DS_MODE);
  }
  PyObject *
  ext_GetVariableMeasurementLevel(PyObject *self, PyObject *args)
  {
      return GetVariableMeasurementLevelAll_XD(self,args,DS_MODE);
  }
  PyObject *
  ext_GetVariableName(PyObject *self, PyObject *args)
  {
      return GetVariableNameAll_XD(self,args,DS_MODE);
  }
  PyObject *
  ext_GetVariableType(PyObject *self, PyObject *args)
  {
      return GetVariableTypeAll_XD(self,args,DS_MODE);
  }
  PyObject *
  ext_IsXDriven(PyObject *self, PyObject *args)
  {
    int check;
    if (!PyArg_ParseTuple(args, "", &check))
      return NULL;

    int model;
    if(NULL == IsXDriven) {
      model = PY_TRUE;
    } else {
      model = IsXDriven();
    }
    return Py_BuildValue("i", model);
  }
  PyObject *
  ext_PostSpssOutput(PyObject *self, PyObject *args)
  {
    char* command;
    int length;
    int errLevel;
    if (!PyArg_ParseTuple(args, "si", &command,&length))
      return NULL;
    errLevel = PostSpssOutput(command,length);
    return Py_BuildValue("i", errLevel);
  }
  PyObject *
  ext_StartSpss(PyObject *self, PyObject *args)
  {
	  char *commandLine;
    int errLevel;
    if (!PyArg_ParseTuple(args, "s", &commandLine))
      return NULL;
      
    char *language = "-language=python,";
    char *cmdline = new char[strlen(commandLine) + strlen(language) + 1];
    memset(cmdline, '\0', strlen(commandLine) + strlen(language) + 1);
    strcpy(cmdline, language);
    strcat(cmdline, commandLine);

    errLevel = LoadLib();
    if(LOAD_SUCCESS == errLevel) {
      SetXNameAndSHome("PYTHON",getenv("SPSS_HOME"));
      errLevel = StartSpss(cmdline);
    }
    
    delete []cmdline;

    if(0 == errLevel) {
        SetXDriveMode(1);
    }
    return Py_BuildValue("i", errLevel);
  }
  PyObject *
  ext_StopSpss(PyObject *self, PyObject *args)
  {
    StopSpss();
    FreeLib();

    Py_INCREF(Py_None);
    return Py_None;
  }
  PyObject *
  ext_IsBackendReady(PyObject *self, PyObject *args)
  {
    int errLevel;

    if(NULL == IsBackendReady) {
      errLevel = PY_FALSE;
    } else {
      errLevel = IsBackendReady();
    }
    return Py_BuildValue("i", errLevel);
  }
  PyObject *
  ext_EvaluateXPath(PyObject *self, PyObject *args)
  {
    char* handle = 0;
    char* context = 0;
    char* expression = 0;
    if (!PyArg_ParseTuple(args, "sss", &handle, &context, &expression))
      return NULL;

    void* result;
    int errLevel;
    //evaluate xpath with the given context and xpath expression
    result = EvaluateXPath(handle,context,expression,errLevel);
    int size = GetStringListLength(result);

    //if evaluate xpath failure
    if ( 0 != errLevel || 0 == size || 0 == result){
      return Py_BuildValue("[i]",errLevel);
    }

    //evaluate success, pass the result to python object
    PyObject* outList = PyList_New(size+1);
    for( int i=0; i<size; i++){
        const char *str = GetStringFromList(result,i);
        PyObject* item = NULL;
        if(IsUTF8mode())
        {
            PyObject* pyValue = PyUnicode_Decode(str,strlen(str),"utf-8",NULL);
            item = Py_BuildValue("O",pyValue);
            Py_XDECREF(pyValue);
        }
        else
        {
            item = Py_BuildValue("s",str);
        }
        PyList_SET_ITEM(outList,i,item);
    }
    RemoveStringList(result);

    PyObject* errCode = Py_BuildValue("i",errLevel);
    PyList_SET_ITEM(outList,size,errCode);

    return outList;
  }
  PyObject *
  ext_GetXmlUtf16(PyObject *self, PyObject *args)
  {
    char* handle = 0;
    if (!PyArg_ParseTuple(args, "s", &handle))
      return NULL;

    const wchar_t* result;
    int errLevel;
    result = GetXmlUtf16(handle,errLevel);
    PyObject* out = Py_BuildValue("ui",result,errLevel);
    return out;
  }

  PyObject *
  ext_GetImage(PyObject *self, PyObject *args)
  {
      char* handle = 0;
      char* fileName = 0;
      char* imageType = 0;
       int imageSize = 0;

      if (!PyArg_ParseTuple(args, "ss", &handle,&fileName))
          return NULL;

      int errLevel;
      const char* result = GetImage(handle,fileName,imageSize,&imageType,errLevel);

      if ( 0 != errLevel  || 0 == result)
          return Py_BuildValue("[i]",errLevel);


      PyObject* out = Py_BuildValue("s#isi",result,imageSize,imageSize,imageType,errLevel);
      return out;
  }

  PyObject* 
  ext_GetSetting(PyObject*self,PyObject* args)
  {

    char* setting = 0;
    char* option = 0;
 
    int errLevel;

    if (!PyArg_ParseTuple(args, "ss", &setting,&option))
        return NULL;
  
    const char* result = GetSetting(setting,errLevel,option);

    if ( 0 != errLevel || 0 == result)
        return Py_BuildValue("[i]",errLevel);

    PyObject* out = NULL;
    if(IsUTF8mode()&&(errLevel==0))
    {
        PyObject* pyValue = PyUnicode_Decode(result,strlen(result),"utf-8",NULL);
        out = Py_BuildValue("Oi", pyValue,errLevel);
        Py_XDECREF(pyValue);
    }
    else
    {
        out = Py_BuildValue("si", result,errLevel);
    }

    return out;
  }

PyObject*
ext_GetOMSTagList(PyObject *self, PyObject *args)
{
     
    void* result;
    int errLevel;
    result = GetOMSTagList(errLevel);
    int size = GetStringListLength(result);

    //if get handle list failure, or handle list is empty
    if ( 0 != errLevel || 0 == size || 0 == result)
        return Py_BuildValue("[i]",errLevel);

    //else, pass the handle list to python object
    PyObject* outList = PyList_New(size+1);
    for( int i=0; i<size; i++) {
        const char * str = GetStringFromList(result,i);
        PyObject* item = NULL;
        if(IsUTF8mode())
        {
            PyObject* pyValue = PyUnicode_Decode(str,strlen(str),"utf-8",NULL);
            item = Py_BuildValue("O",pyValue);
            Py_XDECREF(pyValue);
        }
        else
        {
            item = Py_BuildValue("s",str);
        }
        PyList_SET_ITEM(outList,i,item);
    }
    RemoveStringList(result);

    PyObject* errCode = Py_BuildValue("i",errLevel);
    PyList_SET_ITEM(outList,size,errCode);

    return outList;
}

  PyObject *
  ext_GetHandleList(PyObject *self, PyObject *args)
  {
    void* result;
    int errLevel;
    result = GetHandleList(errLevel);
    int size = GetStringListLength(result);

    //if get handle list failure, or handle list is empty
    if ( 0 != errLevel || 0 == size || 0 == result)
      return Py_BuildValue("[i]",errLevel);

    //else, pass the handle list to python object
    PyObject* outList = PyList_New(size+1);
    for( int i=0; i<size; i++) {
        const char * str = GetStringFromList(result,i);
        PyObject* item = NULL;
        if(IsUTF8mode())
        {
            PyObject* pyValue = PyUnicode_Decode(str,strlen(str),"utf-8",NULL);
            item = Py_BuildValue("O",pyValue);
            Py_XDECREF(pyValue);
        }
        else
        {
            item = Py_BuildValue("s",str);
        }
        PyList_SET_ITEM(outList,i,item);
    }
    RemoveStringList(result);

    PyObject* errCode = Py_BuildValue("i",errLevel);
    PyList_SET_ITEM(outList,size,errCode);

    return outList;
  }

    PyObject *
  ext_StartProcedure(PyObject *self, PyObject *args)
  {
    char* procName = 0;
	char* omsIdentifier = 0;
    if (!PyArg_ParseTuple(args, "ss", &procName, &omsIdentifier))
      return NULL;

    int errLevel;
    errLevel = StartProcedure(omsIdentifier,procName);
    PyObject* out = Py_BuildValue("i",errLevel);
    return out;
  }
    PyObject *
  ext_SplitChange(PyObject *self, PyObject *args)
  {
    char* procName = 0;
    if (!PyArg_ParseTuple(args, "s", &procName))
      return NULL;

    int errLevel;
    errLevel = SplitChange(procName);
    PyObject* out = Py_BuildValue("i",errLevel);
    return out;
  }

  PyObject *
  ext_EndProcedure(PyObject *self, PyObject *args)
  {
    int errLevel;
    errLevel = EndProcedure();
    PyObject* out = Py_BuildValue("i",errLevel);
    return out;
  }

    int parse2IntegerStar(PyObject *data,
                          int **result,
                          int *size)
    {
        int i;
        int iSize;
        typedef PyObject *(*FP_GetItem)(PyObject*, PYSIZE);
        FP_GetItem getitem = 0;
        //PyObject *(*getitem)(PyObject*, PYSIZE);

        if(PyList_Check(data)) {
            iSize = PyList_Size(data);
            getitem = (FP_GetItem)PyList_GetItem;
        } else if(PyTuple_Check(data)) {
            iSize = PyTuple_Size(data);
            getitem = (FP_GetItem)PyTuple_GetItem;
        } else {
            return PARSE_TUPLE_FAIL;
        }

        if(iSize == 0) {
            return PARSE_TUPLE_FAIL;
        }

        *result = (int*)PyMem_Malloc(iSize*sizeof(int));
        if(!result) {
            return NO_MEMORY;
        }

        for(i = 0; i < iSize; i++) {
            if(!PyArg_Parse((*getitem)(data,i),"i",&(*result)[i])) {
                PyMem_Del(*result);
                return PARSE_TUPLE_FAIL;
            }
        }

        *size = iSize;

        return 0;
    }

    int parse2DoubleStar(PyObject *data,
                          double **result,
                          int *size)
    {
        int i;
        int iSize;
        typedef PyObject *(*FP_GetItem)(PyObject*, PYSIZE);
        FP_GetItem getitem = 0;

        if(PyList_Check(data)) {
            iSize = PyList_Size(data);
            getitem = (FP_GetItem)PyList_GetItem;
        } else if(PyTuple_Check(data)) {
            iSize = PyTuple_Size(data);
            getitem = (FP_GetItem)PyTuple_GetItem;
        } else {
            return PARSE_TUPLE_FAIL;
        }

        if(iSize == 0) {
            return PARSE_TUPLE_FAIL;
        }

        *result = (double*)PyMem_Malloc(iSize*sizeof(double));
        if(!result) {
            return NO_MEMORY;
        }

        for(i = 0; i < iSize; i++) {
            if(!PyArg_Parse((*getitem)(data,i),"d",&(*result)[i])) {
                PyMem_Del(*result);
                return PARSE_TUPLE_FAIL;
            }
        }

        *size = iSize;

        return 0;
    }

    int parse2CharStarStar(PyObject *data,char*** result, int *size)
    {
        int i,j;
        int iSize;
        typedef PyObject *(*FP_GetItem)(PyObject*, PYSIZE);
        FP_GetItem getitem = 0;
        //PyObject *(*getitem)(PyObject*, PYSIZE);

        if(PyList_Check(data)) {
            iSize = PyList_Size(data);
            getitem = (FP_GetItem)PyList_GetItem;
        } else if(PyTuple_Check(data)) {
            iSize = PyTuple_Size(data);
            getitem = (FP_GetItem)PyTuple_GetItem;
        } else {
            return PARSE_TUPLE_FAIL;
        }

        *result = PyMem_New(char*,iSize);
        if(!result) {
            return NO_MEMORY;
        }

        for(i = 0; i < iSize; i++) {
            if(!PyArg_Parse((*getitem)(data,i),"et",IsUTF8mode() ? "utf-8" : Py_FileSystemDefaultEncoding,&((*result)[i]))) {
                for (j = 0; j < i; j++) {
                    PyMem_Free(result[j]);
                }
                PyMem_Del(result);
                return PARSE_TUPLE_FAIL;
            }
        }

        *size = iSize;

        return 0;
    }

  PyObject *
      ext_StartPivotTable(PyObject *self, PyObject *args)
  {
      int errLevel;

      char *outline, *title, *templateTitle;
      bool isSplit;
      if (!PyArg_ParseTuple(args, "sssb", &outline, &title, &templateTitle, &isSplit))
          return NULL;

      errLevel = StartPivotTable(outline, title,templateTitle,isSplit);
      return Py_BuildValue("i",errLevel);
  }
  PyObject *
      ext_HidePivotTableTitle(PyObject *self, PyObject *args)
  {
      int errLevel;

      char *outline, *title, *templateTitle;
      bool isSplit;
      if (!PyArg_ParseTuple(args, "sssb", &outline, &title, &templateTitle, &isSplit))
          return NULL;

      errLevel = HidePivotTableTitle(outline, title,templateTitle,isSplit);
      return Py_BuildValue("i",errLevel);
  }
  PyObject *
      ext_PivotTableCaption(PyObject *self, PyObject *args)
  {
      int errLevel;

      char *outline, *title, *templateTitle, *caption;
      bool isSplit;
      if (!PyArg_ParseTuple(args, "sssbs", &outline, &title, &templateTitle, &isSplit, &caption))
          return NULL;

      errLevel = PivotTableCaption(outline, title,templateTitle,isSplit,caption);
      return Py_BuildValue("i",errLevel);
  }
  PyObject *
      ext_AddDimension(PyObject *self, PyObject *args)
  {
      int errLevel;

      char *outline,*tableName,*templateName;
      bool isSplit;
      char *dimName;
      int place,position;
      bool hideName, hideLabels;
      if (!PyArg_ParseTuple(args, "sssbsiibb", &outline,&tableName,&templateName,&isSplit,
                                               &dimName, &place, &position, &hideName, &hideLabels))
          return NULL;

      errLevel = AddDimension(outline,tableName,templateName,isSplit,dimName, place, position, hideName, hideLabels);
      return Py_BuildValue("i",errLevel);
  }

PyObject*
    ext_SetFormatSpecCoefficient(PyObject *self, PyObject *args)
{
    int errLevel;
    int check;
    if (!PyArg_ParseTuple(args, "", &check))
      return NULL;

    errLevel = SetFormatSpecCoefficient();
    return Py_BuildValue("i",errLevel);
}
PyObject*
    ext_SetFormatSpecCoefficientSE(PyObject *self, PyObject *args)
{
    int errLevel;
    int check;
    if (!PyArg_ParseTuple(args, "", &check))
      return NULL;

    errLevel = SetFormatSpecCoefficientSE();
    return Py_BuildValue("i",errLevel);
}
PyObject*
    ext_SetFormatSpecCoefficientVar(PyObject *self, PyObject *args)
{
    int errLevel;
    int check;
    if (!PyArg_ParseTuple(args, "", &check))
      return NULL;

    errLevel = SetFormatSpecCoefficientVar();
    return Py_BuildValue("i",errLevel);
}
PyObject*
    ext_SetFormatSpecCorrelation(PyObject *self, PyObject *args)
{
    int errLevel;
    int check;
    if (!PyArg_ParseTuple(args, "", &check))
      return NULL;

    errLevel = SetFormatSpecCorrelation();
    return Py_BuildValue("i",errLevel);
}
PyObject*
    ext_SetFormatSpecGeneralStat(PyObject *self, PyObject *args)
{
    int errLevel;
    int check;
    if (!PyArg_ParseTuple(args, "", &check))
      return NULL;

    errLevel = SetFormatSpecGeneralStat();
    return Py_BuildValue("i",errLevel);
}
PyObject*
    ext_SetFormatSpecMean(PyObject *self, PyObject *args)
{
    int errLevel;
    int varIndex;
    if (!PyArg_ParseTuple(args, "i", &varIndex))
      return NULL;

    errLevel = SetFormatSpecMean(varIndex);
    return Py_BuildValue("i",errLevel);
}
PyObject*
    ext_SetFormatSpecCount(PyObject *self, PyObject *args)
{
    int errLevel;
    int check;
    if (!PyArg_ParseTuple(args, "", &check))
      return NULL;

    errLevel = SetFormatSpecCount();
    return Py_BuildValue("i",errLevel);
}
PyObject*
    ext_SetFormatSpecPercent(PyObject *self, PyObject *args)
{
    int errLevel;
    int check;
    if (!PyArg_ParseTuple(args, "", &check))
      return NULL;

    errLevel = SetFormatSpecPercent();
    return Py_BuildValue("i",errLevel);
}
PyObject*
    ext_SetFormatSpecPercentNoSign(PyObject *self, PyObject *args)
{
    int errLevel;
    int check;
    if (!PyArg_ParseTuple(args, "", &check))
      return NULL;

    errLevel = SetFormatSpecPercentNoSign();
    return Py_BuildValue("i",errLevel);
}
PyObject*
    ext_SetFormatSpecProportion(PyObject *self, PyObject *args)
{
    int errLevel;
    int check;
    if (!PyArg_ParseTuple(args, "", &check))
      return NULL;

    errLevel = SetFormatSpecProportion();
    return Py_BuildValue("i",errLevel);
}
PyObject*
    ext_SetFormatSpecSignificance(PyObject *self, PyObject *args)
{
    int errLevel;
    int check;
    if (!PyArg_ParseTuple(args, "", &check))
      return NULL;

    errLevel = SetFormatSpecSignificance();
    return Py_BuildValue("i",errLevel);
}
PyObject*
    ext_SetFormatSpecResidual(PyObject *self, PyObject *args)
{
    int errLevel;
    int check;
    if (!PyArg_ParseTuple(args, "", &check))
      return NULL;

    errLevel = SetFormatSpecResidual();
    return Py_BuildValue("i",errLevel);
}
PyObject*
    ext_SetFormatSpecVariable(PyObject *self, PyObject *args)
{
    int errLevel;
    int varIndex;
    if (!PyArg_ParseTuple(args, "i", &varIndex))
      return NULL;

    errLevel = SetFormatSpecVariable(varIndex);
    return Py_BuildValue("i",errLevel);
}
PyObject*
    ext_SetFormatSpecStdDev(PyObject *self, PyObject *args)
{
    int errLevel;
    int varIndex;
    if (!PyArg_ParseTuple(args, "i", &varIndex))
      return NULL;

    errLevel = SetFormatSpecStdDev(varIndex);
    return Py_BuildValue("i",errLevel);
}
PyObject*
    ext_SetFormatSpecDifference(PyObject *self, PyObject *args)
{
    int errLevel;
    int varIndex;
    if (!PyArg_ParseTuple(args, "i", &varIndex))
      return NULL;

    errLevel = SetFormatSpecDifference(varIndex);
    return Py_BuildValue("i",errLevel);
}
PyObject*
    ext_SetFormatSpecSum(PyObject *self, PyObject *args)
{
    int errLevel;
    int varIndex;
    if (!PyArg_ParseTuple(args, "i", &varIndex))
      return NULL;

    errLevel = SetFormatSpecSum(varIndex);
    return Py_BuildValue("i",errLevel);
}

PyObject *
    ext_AddNumberCategory(PyObject *self, PyObject *args)
{
    int errLevel;

    char *outline,*tableName,*templateName;
    bool isSplit;
    char *dimName;
    int place,position;
    bool hideName, hideLabels;
    double category;

    if (!PyArg_ParseTuple(args, "sssbsiibbd", &outline,&tableName,&templateName,&isSplit,
                                          &dimName, &place, &position, &hideName, &hideLabels,
                                          &category)){
        return NULL;
    }

    errLevel = AddNumberCategory(outline,tableName,templateName,isSplit,
                                 dimName,place, position, hideName, hideLabels,
                                 category);
    return Py_BuildValue("i",errLevel);
}
PyObject *
    ext_AddStringCategory(PyObject *self, PyObject *args)
{
    int errLevel;

    char *outline,*tableName,*templateName;
    bool isSplit;
    char *dimName;
    int place,position;
    bool hideName, hideLabels;
    char *category;

    if (!PyArg_ParseTuple(args, "sssbsiibbs", &outline,&tableName,&templateName,&isSplit,
                                              &dimName, &place, &position, &hideName, &hideLabels,
                                              &category)){
        return NULL;
    }
    errLevel = AddStringCategory(outline,tableName,templateName,isSplit,
                                 dimName,place, position, hideName, hideLabels,
                                 category);
    return Py_BuildValue("i",errLevel);
}
PyObject *
    ext_AddVarNameCategory(PyObject *self, PyObject *args)
{
    int errLevel;

    char *outline,*tableName,*templateName;
    bool isSplit;
    char *dimName;
    int place,position;
    bool hideName, hideLabels;
    int category;

    if (!PyArg_ParseTuple(args, "sssbsiibbi", &outline,&tableName,&templateName,&isSplit,
                                          &dimName, &place, &position, &hideName, &hideLabels,
                                          &category)){
        return NULL;
    }

    errLevel = AddVarNameCategory(outline,tableName,templateName,isSplit,
                                  dimName,place, position, hideName, hideLabels,
                                  category);
    return Py_BuildValue("i",errLevel);
}
PyObject *
    ext_AddVarValueDoubleCategory(PyObject *self, PyObject *args)
{
    int errLevel;

    char *outline,*tableName,*templateName;
    bool isSplit;
    char *dimName;
    int place,position;
    bool hideName, hideLabels;
    int category;
    double d;

    if (!PyArg_ParseTuple(args, "sssbsiibbid", &outline,&tableName,&templateName,&isSplit,
                                           &dimName, &place, &position, &hideName, &hideLabels,
                                           &category,&d)){
        return NULL;
    }

    errLevel = AddVarValueDoubleCategory(outline,tableName,templateName,isSplit,
                                        dimName,place, position, hideName, hideLabels,
                                        category,d);
    return Py_BuildValue("i",errLevel);
}
PyObject *
    ext_AddVarValueStringCategory(PyObject *self, PyObject *args)
{
    int errLevel;

    char *outline,*tableName,*templateName;
    bool isSplit;
    char *dimName;
    int place,position;
    bool hideName, hideLabels;
    int category;
    char *ch;

    if (!PyArg_ParseTuple(args, "sssbsiibbis", &outline,&tableName,&templateName,&isSplit,
                                           &dimName, &place, &position, &hideName, &hideLabels,
                                           &category,&ch)){
        return NULL;
    }

    errLevel = AddVarValueStringCategory(outline,tableName,templateName,isSplit,
                                         dimName,place, position, hideName, hideLabels,
                                         category,ch);
    return Py_BuildValue("i",errLevel);
}

PyObject *
    ext_SetNumberCell(PyObject *self, PyObject *args)
{
    int errLevel;

    char *outline,*tableName,*templateName;
    bool isSplit;
    char *dimName;
    int place,position;
    bool hideName, hideLabels;
    double cell;

    if (!PyArg_ParseTuple(args, "sssbsiibbd", &outline,&tableName,&templateName,&isSplit,
                                          &dimName, &place, &position, &hideName, &hideLabels,
                                          &cell)){
        return NULL;
    }

    errLevel = SetNumberCell(outline,tableName,templateName,isSplit,
                             dimName,place, position, hideName, hideLabels,
                             cell);
    return Py_BuildValue("i",errLevel);
}
PyObject *
    ext_SetStringCell(PyObject *self, PyObject *args)
{
    int errLevel;

    char *outline,*tableName,*templateName;
    bool isSplit;
    char *dimName;
    int place,position;
    bool hideName, hideLabels;
    char *cell;

    if (!PyArg_ParseTuple(args, "sssbsiibbs", &outline,&tableName,&templateName,&isSplit,
                                          &dimName, &place, &position, &hideName, &hideLabels,
                                          &cell)){
        return NULL;
    }

    errLevel = SetStringCell(outline,tableName,templateName,isSplit,
                             dimName,place, position, hideName, hideLabels,
                             cell);
    return Py_BuildValue("i",errLevel);
}
PyObject *
    ext_SetVarNameCell(PyObject *self, PyObject *args)
{
    int errLevel;

    char *outline,*tableName,*templateName;
    bool isSplit;
    char *dimName;
    int place,position;
    bool hideName, hideLabels;
    int cell;

    if (!PyArg_ParseTuple(args, "sssbsiibbi", &outline,&tableName,&templateName,&isSplit,
                                          &dimName, &place, &position, &hideName, &hideLabels,
                                          &cell)){
        return NULL;
    }

    errLevel = SetVarNameCell(outline,tableName,templateName,isSplit,
                              dimName,place, position, hideName, hideLabels,
                              cell);
    return Py_BuildValue("i",errLevel);
}
PyObject *
    ext_SetVarValueDoubleCell(PyObject *self, PyObject *args)
{
    int errLevel;

    char *outline,*tableName,*templateName;
    bool isSplit;
    char *dimName;
    int place,position;
    bool hideName, hideLabels;
    int cell;
    double d;

    if (!PyArg_ParseTuple(args, "sssbsiibbid", &outline,&tableName,&templateName,&isSplit,
                                           &dimName, &place, &position, &hideName, &hideLabels,
                                           &cell,&d)){
        return NULL;
    }

    errLevel = SetVarValueDoubleCell(outline,tableName,templateName,isSplit,
                                     dimName,place, position, hideName, hideLabels,
                                     cell,d);
    return Py_BuildValue("i",errLevel);
}
PyObject *
    ext_SetVarValueStringCell(PyObject *self, PyObject *args)
{
    int errLevel;

    char *outline,*tableName,*templateName;
    bool isSplit;
    char *dimName;
    int place,position;
    bool hideName, hideLabels;
    int cell;
    char *ch;

    if (!PyArg_ParseTuple(args, "sssbsiibbis", &outline,&tableName,&templateName,&isSplit,
                                           &dimName, &place, &position, &hideName, &hideLabels,
                                           &cell,&ch)){
        return NULL;
    }

    errLevel = SetVarValueStringCell(outline,tableName,templateName,isSplit,
                                     dimName,place, position, hideName, hideLabels,
                                     cell,ch);
    return Py_BuildValue("i",errLevel);
}

  PyObject *
      ext_AddCellFootnotes(PyObject *self, PyObject *args)
  {
      int errLevel;

      char *outline,*tableName,*templateName;
      bool isSplit;
      char *dimName;
      int place,position;
      bool hideName, hideLabels;
      char *footnotes;


      if (!PyArg_ParseTuple(args, "sssbsiibbs", &outline,&tableName,&templateName,&isSplit,
                                                &dimName, &place, &position, &hideName, &hideLabels,
                                                &footnotes)){
          return NULL;
      }

      errLevel = AddCellFootnotes(outline,tableName,templateName,isSplit,
                                  dimName,place, position, hideName, hideLabels,
                                  footnotes);
      return Py_BuildValue("i",errLevel);
  }

  PyObject *
      ext_AddProcFootnotes(PyObject *self, PyObject *args)
  {
      int errLevel;
      char *footnotes;

      if (!PyArg_ParseTuple(args, "s", &footnotes)){
          return NULL;
      }

      errLevel = AddProcFootnotes(footnotes);
      return Py_BuildValue("i",errLevel);
  }

  PyObject *
      ext_AddOutlineFootnotes(PyObject *self, PyObject *args)
  {
      int errLevel;

      char *outline,*tableName,*templateName,*footnotes;
      bool isSplit;

      if (!PyArg_ParseTuple(args, "ssssb", &outline,&tableName,&templateName,&footnotes,&isSplit)){
          return NULL;
      }

      errLevel = AddOutlineFootnotes(outline,tableName,templateName,footnotes,isSplit);
      return Py_BuildValue("i",errLevel);
  }

  PyObject *
      ext_AddTitleFootnotes(PyObject *self, PyObject *args)
  {
      int errLevel;

      char *outline,*tableName,*templateName,*footnotes;
      bool isSplit;

      if (!PyArg_ParseTuple(args, "ssssb", &outline,&tableName,&templateName,&footnotes,&isSplit)){
          return NULL;
      }

      errLevel = AddTitleFootnotes(outline,tableName,templateName,footnotes,isSplit);
      return Py_BuildValue("i",errLevel);
  }

  PyObject *
      ext_AddDimFootnotes(PyObject *self, PyObject *args)
  {
      int errLevel;

      char *outline,*tableName,*templateName;
      bool isSplit;
      char *dimName;
      int place,position;
      bool hideName, hideLabels;
      char *footnotes;


      if (!PyArg_ParseTuple(args, "sssbsiibbs", &outline,&tableName,&templateName,&isSplit,
                                                &dimName, &place, &position, &hideName, &hideLabels,
                                                &footnotes)){
          return NULL;
      }

      errLevel = AddDimFootnotes(outline,tableName,templateName,isSplit,
                                  dimName,place, position, hideName, hideLabels,
                                  footnotes);
      return Py_BuildValue("i",errLevel);
  }

  PyObject *
      ext_AddCategoryFootnotes(PyObject *self, PyObject *args)
  {
      int errLevel;

      char *outline,*tableName,*templateName;
      bool isSplit;
      char *dimName;
      int place,position;
      bool hideName, hideLabels;
      char *footnotes;


      if (!PyArg_ParseTuple(args, "sssbsiibbs", &outline,&tableName,&templateName,&isSplit,
                                                &dimName, &place, &position, &hideName, &hideLabels,
                                                &footnotes)){
          return NULL;
      }

      errLevel = AddCategoryFootnotes(outline,tableName,templateName,isSplit,
                                      dimName,place, position, hideName, hideLabels,
                                      footnotes);
      return Py_BuildValue("i",errLevel);
  }


  PyObject *
      ext_AddTextBlock(PyObject *self, PyObject *args)
  {
      int errLevel;

      char *outline,*name,*line;
      int nSkip;

      if (!PyArg_ParseTuple(args, "sssi", &outline,&name,&line,&nSkip)){
          return NULL;
      }
      errLevel = AddTextBlock(outline,name,line,nSkip);
      return Py_BuildValue("i",errLevel);
}

    PyObject * ext_SetXDriveMode(PyObject *self, PyObject *args)
  {
      int errLevel;
      int mode;

      if (!PyArg_ParseTuple(args, "i", &mode)){
          return NULL;
      }
      errLevel = SetXDriveMode(mode);
      return Py_BuildValue("i",errLevel);
}

  typedef struct {
    int                       formatType;
    char                      fName[256+1];  // name as entered
    char                      *label;       //variable label.
    int                       mLevel;
    int                       width;      // Character width of string representation.
    int                       decimals;   // Maximum # decimal places to show.
    int                       type;
    unsigned int              flags;      /* Div flags */
  } SPSS_VariableInfo;

  typedef struct {
      PyObject_HEAD
      int                       varCount; //variable count of data source.
      int                       outCount; //real output case count.
      int                       incMissing;
      SPSS_VariableInfo         *varInfo; //variable information of data source.
      void                      *caseCursor; //no use.
      PyObject                  *converter; //converter object from converter.py.
      PyObject                  *varDict; //the relationship between the real data source index and the output index.
      char                      *accessType;
  } SPSS_ResultObject;

    extern PyTypeObject SPSS_ResultObject_Type;

    int  MakeConverter(SPSS_ResultObject *resultSet, PyObject *conv) {
        int i = 0, j = 0;
        long formatType, errLevel = 0;
        int varCount = resultSet->varCount;

        if (!(resultSet->converter = PyTuple_New(varCount))) {
            errLevel = NO_MEMORY;
            goto error;
        }

        for(i = 0; i < varCount; ++i) {
            PyObject *tmp = NULL, *fun = NULL;
            formatType = resultSet->varInfo[i].formatType;

            tmp = PyInt_FromLong((long)formatType);
            if(!tmp) {
                errLevel = ERROR_PARAMETER;
                break;
            }

            fun = PyObject_GetItem(conv, tmp);

            Py_DECREF(tmp);

            //PyTuple_SET_ITEM(resultSet->converter, i, fun);

            errLevel = PyTuple_SetItem(resultSet->converter, i, fun);
            if(errLevel != 0) {
                break;
            }
        }

      error:
        return errLevel;
    }

    PyObject *
    fetch_row(SPSS_ResultObject *resultSet,int batchMode,int &errLevel)
    {
        int         varCount, isMissing = 0;
        int         i = 0;
        int         realIndex = 0;
        PYSIZE      pos = 0;
        double      dValue = 0;
        char        *ch;

        errLevel = 0;
        varCount = resultSet->varCount;
        PyObject* row = PyTuple_New(resultSet->outCount);
        if(!row) {
            errLevel = NO_MEMORY;
            goto error;
        }

        errLevel = NextCase(); 
       
        if(0 != errLevel) {
            goto error;
        }

        PyObject *key, *value;
        pos = 0;
        realIndex = 0; //real index of datasource variable

        while (PyDict_Next(resultSet->varDict, &pos, &key, &value)) {
            PyArg_Parse(key,"i",&realIndex);
            PyObject *v = NULL, *fun = NULL;
            fun = PyTuple_GET_ITEM(resultSet->converter,realIndex);
            if(0 == resultSet->varInfo[realIndex].type) {
                errLevel = GetNumericValue(realIndex, dValue, isMissing);                    
                if(errLevel) {
                    goto error;
                }

                //isMissing == 2 means this value is System_MissingValue.
                //isMissing == 1 means this values is user missing value.
                if (isMissing == 2 || (isMissing == 1 && resultSet->incMissing == 0)) {
                    Py_INCREF(Py_None);
                    v = Py_None;
                } else {
                    v = PyObject_CallFunction(fun,"d",dValue);
                }
            } else {
                ch = (char*)PyMem_Malloc(resultSet->varInfo[realIndex].type + 1);
                if(!ch) {
                    goto error;
                }
                memset(ch,'\0',resultSet->varInfo[realIndex].type + 1);
                errLevel = GetStringValue(realIndex,ch,resultSet->varInfo[realIndex].type,isMissing);                    
                if(errLevel) {
                    goto error;
                }
                if(isMissing == 1 && resultSet->incMissing == 0) {
                    Py_INCREF(Py_None);
                    v = Py_None;
                } else {
                    if(IsUTF8mode()){
                        PyObject* pyValue = PyUnicode_Decode(ch,resultSet->varInfo[realIndex].type,"utf-8",NULL);
                        v = Py_BuildValue("O", pyValue);
                        Py_XDECREF(pyValue);
                    } else {
                        v = PyObject_CallFunction(fun,"s#",ch,resultSet->varInfo[realIndex].type);
                    }
                }
                PyMem_Free(ch);
            }
            PyObject *indexKey = NULL, *indexValue = NULL;
            PYSIZE startpos = 0;
            int outputIndex = 0; //the position of output tuple
            //record the output value
            int vNum = 0;
            while(PyDict_Next(value, &startpos, &indexKey, &indexValue)) {
                if(!PyArg_Parse(indexValue,"i",&outputIndex)) {
                    errLevel = ERROR_PYTHON;
                    goto error;
                }
                if(vNum > 0) {
                    Py_INCREF(v);
                }
                PyTuple_SET_ITEM(row,outputIndex,v);
                ++vNum;
            }
        }

        return row;

      error:
        Py_XDECREF(row);
        Py_INCREF(Py_None);

        return Py_None;
    }
    
    PyObject *
    fetchdata(SPSS_ResultObject* pResult, int size, int batchMode, int &errLevel)
    {
        int i = 0, totalCase = 0, pos = 0;
        PyObject  *data = NULL;
        bool      noRowCount = false;
        bool      failData = false;   

        if(-1 == size) {
            size = 100;
            noRowCount = true;
        }
        data = PyTuple_New(size);
        if(!data) {
            errLevel = NO_MEMORY;
            return NULL;
        }

        while(true) {
            int tempSize = 0;
            for(i = pos; i < size; ++i) {
                PyObject *rawData = fetch_row((SPSS_ResultObject*)pResult,batchMode,errLevel);
                if(0 == errLevel) {
                    PyTuple_SET_ITEM(data,i,rawData);
                    ++totalCase;
                } 
                else
                {                  
                    //get data fail or no more data.
                    failData = true;
                    break;
                }                  
            }

            if(failData) {
                break;
            }

            if(noRowCount) {
                pos = size;
                size *= 2;
                if(_PyTuple_Resize(&data,size)) {
                    errLevel = ERROR_PYTHON;
                    break;
                }                
            } else {
                break;
            }           
        }

        if(_PyTuple_Resize(&data,totalCase)) {
            errLevel = ERROR_PYTHON;
        }

        return data;
    }

    PyObject *
    ext_fetchone(PyObject *self, PyObject *args)
    {
        int errLevel = 0, size = 0;
        PyObject *pResult = NULL, *dataSet = NULL;

        if(!PyArg_ParseTuple(args,"O",&pResult)) {
            errLevel = ERROR_PARAMETER;
            Py_INCREF(Py_None);
            return Py_BuildValue("(iO)",errLevel,Py_None);
        }
        int batchMode = 0;
              
        PyObject *rawData = fetch_row((SPSS_ResultObject*)pResult,batchMode,errLevel); //0:one case    
       
        if(errLevel == 23 || errLevel == 67)
        {
            rawData = Py_None;
                   
        }
        PyObject *r = Py_BuildValue("(iO)",errLevel,rawData);

        Py_DECREF(rawData);

        return r;
    }

    PyObject *
    ext_fetchmany(PyObject *self, PyObject *args)
    {
        int errLevel = 0, size = 1, i = 0;
        PyObject *pResult = NULL, *dataSet = NULL;

        if (!PyArg_ParseTuple(args, "Oi", &pResult, &size)) {
            errLevel = ERROR_PARAMETER;
            Py_INCREF(Py_None);
            return Py_BuildValue("(iO)",errLevel,Py_None);
        }

        if(size <= 0 ) {
            //just get one case data.
            size = 1;
        }

        dataSet = fetchdata((SPSS_ResultObject*)pResult,size,1,errLevel); //many cases

        PyObject *r = Py_BuildValue("(iO)",errLevel,dataSet);

        Py_DECREF(dataSet);

        return r;
    }

    PyObject *
    ext_fetchall(PyObject *self, PyObject *args)
    {
        int errLevel = 0, i = 0;
        int rowCount = 0, curPos = 0, size = 0;
        PyObject *pResult = NULL, *dataSet = NULL;

        if(!PyArg_ParseTuple(args,"O",&pResult)) {
            errLevel = ERROR_PARAMETER;
            Py_INCREF(Py_None);
            return Py_BuildValue("iO",errLevel,Py_None);
        }

        size = -1;
        dataSet = fetchdata((SPSS_ResultObject*)pResult,size,2,errLevel);  //2:all cases

        if(0 == size) {
            errLevel = NO_MORE_DATA;
        }

        PyObject *r = Py_BuildValue("(iO)",errLevel,dataSet);

        Py_DECREF(dataSet);
        return r;
    }
    
    PyObject *
    ext_NextCase(PyObject *self, PyObject *args)
    {
        return Py_BuildValue("i",NextCase());
    }

    PyObject *
    ext_scroll(PyObject *self, PyObject *args)
    {
        Py_INCREF(Py_None);
        return Py_None;
    }

    PyObject *
    ext_describe(PyObject *self, PyObject *args)
    {
        PyObject *d = NULL;
        PyObject *pInfo = NULL;
        SPSS_ResultObject *pDatasrcInfo = NULL;
        unsigned int i, n;

        if(!PyArg_ParseTuple(args,"O",&pInfo)) {
            return NULL;
        }

        pDatasrcInfo = (SPSS_ResultObject*)pInfo;

        n = pDatasrcInfo->varCount;

        if (!(d = PyTuple_New(n))) {
            return NULL;
        }

        for (i = 0; i < n; i++) {
            PyObject *t;
            if(IsUTF8mode()) {
            PyObject* pyValue = PyUnicode_Decode(pDatasrcInfo->varInfo[i].fName,strlen(pDatasrcInfo->varInfo[i].fName),"utf-8",NULL);
            t = Py_BuildValue("(Oiiiiii)",
                              pyValue,
                              (long) pDatasrcInfo->varInfo[i].type,
                              (long) pDatasrcInfo->varInfo[i].width,
                              (long) pDatasrcInfo->varInfo[i].width,
                              (long) pDatasrcInfo->varInfo[i].width,
                              (long) pDatasrcInfo->varInfo[i].decimals,
                              (long) pDatasrcInfo->varInfo[i].flags);
            Py_XDECREF(pyValue);
            } else {
            t = Py_BuildValue("(siiiiii)",
                              pDatasrcInfo->varInfo[i].fName,
                              (long) pDatasrcInfo->varInfo[i].type,
                              (long) pDatasrcInfo->varInfo[i].width,
                              (long) pDatasrcInfo->varInfo[i].width,
                              (long) pDatasrcInfo->varInfo[i].width,
                              (long) pDatasrcInfo->varInfo[i].decimals,
                              (long) pDatasrcInfo->varInfo[i].flags);
            }

            if (!t) goto error;
            PyTuple_SET_ITEM(d, i, t);
        }
        return d;
      error:
        Py_XDECREF(d);
        return NULL;
    }


    PyObject *
    ext_close(PyObject *self, PyObject *args)
    {
        int errLevel = 0;
        PyObject *pDatasrcInfo = NULL;
        SPSS_ResultObject *pResult = NULL;
                
        if(RemoveCaseCursor) {
            errLevel = RemoveCaseCursor();
            if(errLevel) {
                return Py_BuildValue("i",errLevel);
            }
        }
        Py_INCREF(Py_None);
        return Py_None;
    }

    int MakeVarDict(SPSS_ResultObject *datasrcInfo, PyObject *varTuple)
    {
        int errLevel = 0;
        int len = 0;
        int i = 0;
        PyObject *temp = NULL;

        datasrcInfo->varDict = PyDict_New();

        if(PyTuple_Check(varTuple)) {

            /*
             *  there are two dictionary to save the relationship between output variable and real variable index.
             *  dictionary
             *     |
             *     ->  key:  index
             *        value: dictionary
             *                    |
             *                    -> key: 1,2,3 or 4 ....
             *                    values: output position
             */

            len = PyTuple_Size(varTuple);
            if(0 == len) {
                datasrcInfo->outCount = datasrcInfo->varCount;

                for(i = 0; i < datasrcInfo->varCount; i++) {
                    PyObject *v = Py_BuildValue("i",i);
                    PyObject *key = Py_BuildValue("i",1);
                    PyObject *r = PyDict_New();
                    if(NULL == r) {
                        errLevel = ERROR_PYTHON;
                        goto error;
                    }

                    if(PyDict_SetItem(r,key,v)) {
                        errLevel = ERROR_PYTHON;
                        goto error;
                    } else {
                        Py_DECREF(key);
                    }

                    if(PyDict_SetItem(datasrcInfo->varDict,v,r)) {
                        errLevel = ERROR_PYTHON;
                        goto error;
                    } else {
                        Py_DECREF(r);
                        Py_DECREF(v);
                    }
                }
            } else {
                datasrcInfo->outCount = len;

                int index = 0;
                PyObject *key = NULL;

                for(i = 0; i < len; ++i) {
                    //get the varibale index
                    PyObject *v = PyTuple_GetItem(varTuple, i);
                    if(NULL == v) {
                        errLevel = ERROR_PYTHON;
                        goto error;
                    }
                    if(!PyArg_Parse(v,"i",&index)) {
                        errLevel = ERROR_PYTHON;
                        goto error;
                    }
                    if(index >= datasrcInfo->varCount || index < 0) {
                        errLevel = 1028;
                        goto error;
                    }
                    PyObject *r = PyDict_GetItem(datasrcInfo->varDict,v);
                    PyObject *value = Py_BuildValue("i",i);
                    if(r) {
                        int size = PyDict_Size(r);
                        key = Py_BuildValue("i",++size);
                    } else {
                        r = PyDict_New();
                        if(!r) {
                            errLevel = ERROR_PYTHON;
                            goto error;
                        }
                        key = Py_BuildValue("i",1);
                        if(PyDict_SetItem(datasrcInfo->varDict,v,r)) {
                            errLevel = ERROR_PYTHON;
                            goto error;
                        } else {
                            Py_DECREF(r);
                        }
                    }
                    if(PyDict_SetItem(r,key,value)) {
                        errLevel = ERROR_PYTHON;
                        goto error;
                    } else {
                        Py_DECREF(key);
                        Py_DECREF(value);
                    }
                }
            }
        }

      error:
        if(errLevel != 0) {
            Py_DECREF(datasrcInfo->varDict);
        }

        return errLevel;
    }

    int ConstructDataSrcInfo(PyObject * conv, SPSS_ResultObject *datasrcInfo, PyObject *varTuple)
    {
        int i = 0;
        int varCount = 0;
        int len = 0;
        int errLevel = 0;
        PyObject *result = NULL;
        const char* name = NULL;

        PyObject * varDict = datasrcInfo->varDict;
        int outCount = datasrcInfo->outCount;

        varCount = GetVariableCountInProcDS(errLevel);
        if(errLevel || 0 == varCount) {
            goto error;
        }

        datasrcInfo->varCount = varCount;

        PyMem_Free(datasrcInfo->varInfo);
        datasrcInfo->varInfo = NULL;

        datasrcInfo->varInfo = (SPSS_VariableInfo*)PyMem_Malloc(sizeof(SPSS_VariableInfo) * datasrcInfo->varCount);
        if(!(datasrcInfo->varInfo)) {
            errLevel = NO_MEMORY; //no memory.
            goto error;
        }

      for(i = 0; i < datasrcInfo->varCount; ++i) {
          datasrcInfo->varInfo[i].type = GetVariableTypeInProcDS(i, errLevel);
          if(errLevel) {
              goto error;
          }

          errLevel = GetVariableFormatTypeInProcDS(i,datasrcInfo->varInfo[i].formatType,
                                                   datasrcInfo->varInfo[i].width,
                                                   datasrcInfo->varInfo[i].decimals);
          if(errLevel) {
              goto error;
          }

          name = GetVariableNameInProcDS(i,errLevel);
          if(errLevel) {
              goto error;
          }
          memset(datasrcInfo->varInfo[i].fName,'\0',257);
          strcpy(datasrcInfo->varInfo[i].fName,name);

          datasrcInfo->varInfo[i].mLevel = GetVariableMeasurementLevelInProcDS(i,errLevel);
          if(errLevel) {
              goto error;
          }

          datasrcInfo->varInfo[i].flags = 0;
      }

      Py_DECREF(datasrcInfo->converter);
      errLevel = MakeConverter(datasrcInfo,conv);
      if(errLevel) {
          errLevel = CONVERTER_FAIL; //make converter fail.
          goto error;
      }

      //Py_DECREF(datasrcInfo->varDict);
      errLevel = MakeVarDict(datasrcInfo,varTuple);
      if(errLevel == 0) {
          Py_DECREF(varDict);
      } else {
          datasrcInfo->varDict = varDict;
          datasrcInfo->outCount = outCount;
      }

      error:
          return errLevel;
    }


    PyObject *
    ext_cursor(PyObject *self, PyObject *args) {
        int errLevel = 0, i = 0;
        int varCount = 0;
        int len = 0;
        char * accessType;
        PyObject *conv = NULL, *result = NULL, *varTuple = NULL;
        SPSS_ResultObject *datasrcInfo = NULL;
        const char* name = NULL;

        if(!PyArg_ParseTuple(args,"OOs",&conv, &varTuple,&accessType)) {
            errLevel = ERROR_PARAMETER; //error parameter.
            goto error;
        }


        varCount = GetVariableCount(errLevel);
        if(errLevel || 0 == varCount) {
            goto error;
        }

        datasrcInfo = (SPSS_ResultObject*)PyType_GenericAlloc(&SPSS_ResultObject_Type,0);
        if(!datasrcInfo) {
            errLevel = NO_MEMORY; //no memeory.
            goto error;
        }

        datasrcInfo->incMissing = 0;

        datasrcInfo->varCount = varCount;

        datasrcInfo->varInfo = (SPSS_VariableInfo*)PyMem_Malloc(sizeof(SPSS_VariableInfo) * datasrcInfo->varCount);
        if(!(datasrcInfo->varInfo)) {
            errLevel = NO_MEMORY; //no memory.
            goto error;
        }

        for(i = 0; i < datasrcInfo->varCount; ++i) {
            datasrcInfo->varInfo[i].type = GetVariableType(i, errLevel);
            if(errLevel) {
                goto error;
            }
            errLevel = GetVariableFormatType(i,datasrcInfo->varInfo[i].formatType,
                                             datasrcInfo->varInfo[i].width,
                                             datasrcInfo->varInfo[i].decimals);
            if(errLevel) {
                goto error;
            }

            name = GetVariableName(i,errLevel);
            if(errLevel) {
                goto error;
            }
            memset(datasrcInfo->varInfo[i].fName,'\0',257);
            strcpy(datasrcInfo->varInfo[i].fName,name);

            datasrcInfo->varInfo[i].mLevel = GetVariableMeasurementLevel(i,errLevel);
            if(errLevel) {
                goto error;
            }

            datasrcInfo->varInfo[i].flags = 0;
        }

        errLevel = MakeConverter(datasrcInfo,conv);
        if(errLevel) {
            errLevel = CONVERTER_FAIL; //make converter fail.
            goto error;
        }
        datasrcInfo->varDict = PyDict_New();
        //save the variable list to get.
        len = 0;
        if(PyTuple_Check(varTuple)) {

            /*
             *  there are two dictionary to save the relationship between output variable and real variable index.
             *  dictionary
             *     |
             *     ->  key:  index
             *        value: dictionary
             *                    |
             *                    -> key: 1,2,3 or 4 ....
             *                    values: output position
             */

            len = PyTuple_Size(varTuple);
            if(0 == len) {
                datasrcInfo->outCount = datasrcInfo->varCount;

                for(i = 0; i < datasrcInfo->varCount; i++) {
                    PyObject *v = Py_BuildValue("i",i);
                    PyObject *key = Py_BuildValue("i",1);
                    PyObject *r = PyDict_New();
                    if(NULL == r) {
                        errLevel = ERROR_PYTHON;
                        goto error;
                    }

                    if(PyDict_SetItem(r,key,v)) {
                        errLevel = ERROR_PYTHON;
                        goto error;
                    } else {
                        Py_DECREF(key);
                    }

                    if(PyDict_SetItem(datasrcInfo->varDict,v,r)) {
                        errLevel = ERROR_PYTHON;
                        goto error;
                    } else {
                        Py_DECREF(r);
                        Py_DECREF(v);
                    }
                }
            } else {
                datasrcInfo->outCount = len;

                int index = 0;
                PyObject *key = NULL;

                for(i = 0; i < len; ++i) {
                    //get the varibale index
                    PyObject *v = PyTuple_GetItem(varTuple, i);
                    if(NULL == v) {
                        errLevel = ERROR_PYTHON;
                        goto error;
                    }
                    if(!PyArg_Parse(v,"i",&index)) {
                        errLevel = ERROR_PYTHON;
                        goto error;
                    }
                    if(index >= datasrcInfo->varCount || index < 0) {
                        errLevel = 1028;
                        goto error;
                    }
                    PyObject *r = PyDict_GetItem(datasrcInfo->varDict,v);
                    PyObject *value = Py_BuildValue("i",i);
                    if(r) {
                        int size = PyDict_Size(r);
                        key = Py_BuildValue("i",++size);
                    } else {
                        r = PyDict_New();
                        if(!r) {
                            errLevel = ERROR_PYTHON;
                            goto error;
                        }
                        key = Py_BuildValue("i",1);
                        if(PyDict_SetItem(datasrcInfo->varDict,v,r)) {
                            errLevel = ERROR_PYTHON;
                            goto error;
                        } else {
                            //Py_DECREF(v);
                            Py_DECREF(r);
                        }
                    }
                    if(PyDict_SetItem(r,key,value)) {
                        errLevel = ERROR_PYTHON;
                        goto error;
                    } else {
                        Py_DECREF(key);
                        Py_DECREF(value);
                    }
                }
            }
        }

        errLevel = MakeCaseCursor(accessType);
        if(0 != errLevel) {
            goto error;
        }
        datasrcInfo->accessType = (char*)malloc(strlen(accessType)+1);
        strcpy(datasrcInfo->accessType,accessType);
        datasrcInfo->accessType[strlen(accessType)] = '\0';
        
        //return (PyObject*)datasrcInfo;
        return Py_BuildValue("Oi",(PyObject*)datasrcInfo,errLevel);

      error:
        if(datasrcInfo && datasrcInfo->varInfo) {
            PyMem_Free(datasrcInfo->varInfo);
        }

        Py_INCREF(Py_None);

        return Py_BuildValue("Oi",Py_None,errLevel);
    }

  static char  SPSS_ResultObject__doc__[] =
    "SPSS_ResultObject__doc__";

  static PyObject *
  SPSS_ResultObject_getattr( SPSS_ResultObject *self,
                             char *name)
  {
      PyObject *out = NULL;
      return out;
  }

  static int
  SPSS_ResultObject_setattr(SPSS_ResultObject *self,
                            char *name,
                            PyObject *v)
  {
      return  0;
  }

  static int SPSS_ResultObject_traverse(SPSS_ResultObject *self,
                                        visitproc visit,
                                        void *arg)
  {
      return 0;
  }

  static int SPSS_ResultObject_clear(SPSS_ResultObject *self)
  {
      Py_DECREF(self->converter);
      Py_DECREF(self->varDict);

      return 0;
  }

  static int
  SPSS_ResultObject_Initialize( SPSS_ResultObject *self,
                                PyObject *args,
                                PyObject *kwargs)
  {
      return 0;
  }
  static void
  SPSS_ResultObject_dealloc(SPSS_ResultObject *self)
  {
      SPSS_ResultObject_clear(self);

      PyMem_Free(self->varInfo);
      _PyObject_GC_Del((PyObject *)self);
  }

  static PyObject *
  SPSS_ResultObject_repr(SPSS_ResultObject *self)
  {
      char *buf = "result object";

      return PyString_FromString(buf);
  }

  static char SPSS_ResultObject_describe__doc__[] =
    "Returns the sequence of 7-tuples required by the DB-API for\n \
     the Cursor.description attribute.\n                                   \
    ";

  static PyObject *
  SPSS_ResultObject_describe(SPSS_ResultObject *self,
                             PyObject *args )
  {
      return NULL;
  }

  static PyMethodDef SPSS_ResultObject_methods[] = {
    {
                "describe",
                (PyCFunction)SPSS_ResultObject_describe,
                METH_VARARGS,
                SPSS_ResultObject_describe__doc__
        },
        {NULL, NULL}
  };

  PyTypeObject SPSS_ResultObject_Type = {
    PyObject_HEAD_INIT(NULL)
    0,
    "spss.result",
    sizeof(SPSS_ResultObject),
    0,
    (destructor)SPSS_ResultObject_dealloc, /* tp_dealloc */
    0, /*tp_print*/
    0,//(getattrfunc)SPSS_ResultObject_getattr, /* tp_getattr */
    0,//(setattrfunc)SPSS_ResultObject_setattr, /* tp_setattr */
    0, /*tp_compare*/
    0,//(reprfunc)SPSS_ResultObject_repr, /* tp_repr */

    /* Method suites for standard classes */

    0, /* (PyNumberMethods *) tp_as_number */
    0, /* (PySequenceMethods *) tp_as_sequence */
    0, /* (PyMappingMethods *) tp_as_mapping */

    /* More standard operations (here for binary compatibility) */

    0, /* (hashfunc) tp_hash */
    0, /* (ternaryfunc) tp_call */
    0, /* (reprfunc) tp_str */
    0, /* (getattrofunc) tp_getattro */
    0, /* (setattrofunc) tp_setattro */

    /* Functions to access object as input/output buffer */
    0, /* (PyBufferProcs *) tp_as_buffer */

    /* Flags to define presence of optional/expanded features */
    Py_TPFLAGS_DEFAULT | Py_TPFLAGS_HAVE_GC | Py_TPFLAGS_BASETYPE,
    SPSS_ResultObject__doc__, /* (char *) tp_doc Documentation string */
    /* call function for all accessible objects */
    (traverseproc) SPSS_ResultObject_traverse, /* tp_traverse */

    /* delete references to contained objects */
    (inquiry) SPSS_ResultObject_clear, /* tp_clear */
    /* Assigned meaning in release 2.1 */
    /* rich comparisons */
    0, /* (richcmpfunc) tp_richcompare */

    /* weak reference enabler */
    0, /* (long) tp_weaklistoffset */
    /* Added in release 2.2 */
    /* Iterators */
    0, /* (getiterfunc) tp_iter */
    0, /* (iternextfunc) tp_iternext */

    /* Attribute descriptor and subclassing stuff */
    (struct PyMethodDef *) SPSS_ResultObject_methods, /* tp_methods */
    NULL,//(MyMemberlist(*)) _SPSS_ResultObject_memberlist, /*tp_members */
    0, /* (struct getsetlist *) tp_getset; */
    0, /* (struct _typeobject *) tp_base; */
    0, /* (PyObject *) tp_dict */
    0, /* (descrgetfunc) tp_descr_get */
    0, /* (descrsetfunc) tp_descr_set */
    0, /* (long) tp_dictoffset */
    (initproc)SPSS_ResultObject_Initialize, /* tp_init */
    NULL, /* tp_alloc */
    NULL, /* tp_new */
    NULL, /* tp_free Low-level free-memory routine */
    0, /* (PyObject *) tp_bases */
    0, /* (PyObject *) tp_mro method resolution order */
    0, /* (PyObject *) tp_defined */
  };

    PyObject*
    ext_SetVarName(PyObject* self,
                       PyObject* args)
    {
        int error = 0;
        int i = 0;
        int *varType;
        PyObject *pyName, *pyType;
        char **varName;


        if(!PyArg_ParseTuple(args,"OO",&pyName,&pyType)) {
            return NULL;
        }

        int nameSize = 0;
        error = parse2CharStarStar(pyName,&varName,&nameSize);
        if(error != 0) {
            return Py_BuildValue("i",error);
        }

        int typeSize;
        error = parse2IntegerStar(pyType,&varType,&typeSize);
        if(error != 0) {
            return Py_BuildValue("i",error);
        }

        if(nameSize != typeSize) {
            error = SIZE_NOT_EQUAL;
            return Py_BuildValue("i",error);
        }

        error = SetVarNameAndType(varName,varType,nameSize);

        for(i = 0; i < nameSize; i++) {
            PyMem_Free(varName[i]);
        }
        PyMem_Free(varName);

        PyMem_Free(varType);

        return Py_BuildValue("i",error);
    }

    PyObject*
    ext_SetVarLabel(PyObject* self,
                    PyObject* args)
    {
        int error = 0;
        char *varName;
        char *varLabel;

        if(!PyArg_ParseTuple(args,"ss",&varName,&varLabel)) {
            return NULL;
        }

        error = SetVarLabel(varName,varLabel);

        return Py_BuildValue("i",error);
    }

    PyObject*
    ext_CommitHeader(PyObject* self,
                     PyObject* args)
    {
        int error = 0;

        error = CommitHeader();

        return Py_BuildValue("i",error);
    }

    PyObject*
    ext_SetValueChar(PyObject* self,
                     PyObject* args)
    {
        int error = 0;
        char *varName;
        char *value;

        if(!PyArg_ParseTuple(args,"ss",&varName,&value)) {
            return NULL;
        }

        error = SetValueChar(varName,value,strlen(value));

        return Py_BuildValue("i",error);
    }

    PyObject*
    ext_SetValueNumeric(PyObject* self,
                        PyObject* args)
    {
        int error = 0;
        char *varName;
        double value;

        if(!PyArg_ParseTuple(args,"sd",&varName,&value)) {
            return NULL;
        }

        error = SetValueNumeric(varName,value);

        return Py_BuildValue("i",error);
    }

    PyObject*
    ext_CommitManyCases(PyObject* self,
                         PyObject* args)
    {
        int error = 0;
        PyObject *pyVarName, *pyCaseData,*pyVarType;
        
        if(!PyArg_ParseTuple(args,"OOO",&pyVarName,&pyCaseData,&pyVarType)) 
        {
            return NULL;
        }
        
        int nameSize = 0;
        char **newVarName;
        error = parse2CharStarStar(pyVarName,&newVarName,&nameSize);
        if(error != 0) 
        {
            return Py_BuildValue("i",error);
        }
        
        int typeSize;
        int *newVarType;
        error = parse2IntegerStar(pyVarType,&newVarType,&typeSize);
        if(error != 0) 
        {
            return Py_BuildValue("i",error);
        }
        
        int oneCaseSize = 0;
        for(int i = 0; i < typeSize; i++)
        {
            if(newVarType[i] == 0)                
                oneCaseSize += sizeof(double);
            else
                oneCaseSize += newVarType[i];                
        }
        
        Py_ssize_t caseNumber = PyTuple_GET_SIZE(pyCaseData) ;
        unsigned int totalCaseSize = oneCaseSize*caseNumber;
        void *data = malloc(totalCaseSize);
        memset(data,'\0',totalCaseSize);
        if(data == 0)
        {
            return Py_BuildValue("i",NO_MEMORY);
        }        
        
        char *tmpData = (char*)data;
    
        typedef PyObject *(*FP_GetItem)(PyObject*, PYSIZE);
        FP_GetItem getitem = 0;
        if(PyList_Check(pyCaseData)) {
            getitem = (FP_GetItem)PyList_GetItem;
        } 
        else
        {
            getitem = (FP_GetItem)PyTuple_GetItem;
        } 

        for(Py_ssize_t k = 0; k < caseNumber; k++)
        {
            PyObject* curCase = getitem(pyCaseData, k);   
            FP_GetItem getitemForCase = 0;
            if(PyList_Check(curCase)) 
            {
                getitemForCase = (FP_GetItem)PyList_GetItem;
            } 
            else if (PyTuple_Check(curCase))
            {
                getitemForCase = (FP_GetItem)PyTuple_GetItem;
            }  
            else
            {
                free(data);
                return Py_BuildValue("i",30);
            }        
            for(int j = 0; j < typeSize; j++)
            {
                PyObject *cell = getitemForCase(curCase, j); 
                if(newVarType[j] == 0) 
                {
                    double d;
                    if(!PyArg_Parse(cell,"d",&d)) 
                    {
                        free(data);
                        return Py_BuildValue("i",30);
                    }
                    memcpy(tmpData,&d,sizeof(double));
                    tmpData += sizeof(double);
                }                                
                else
                {
                    char *c;
                    if(!PyArg_Parse(cell,"s",&c)) 
                    {
                        free(data);
                        return Py_BuildValue("i",30);
                    }
                    memcpy(tmpData,c,strlen(c)>newVarType[j]?newVarType[j]:strlen(c));
                    tmpData += newVarType[j];
                }                 
            } 
        }
        
        error = CommitManyCases(newVarName,newVarType,data,nameSize,totalCaseSize,caseNumber);
        free(data);
        PyMem_Free(newVarType);
        return Py_BuildValue("i",error);
    }


    PyObject*
    ext_CommitCaseRecord(PyObject* self,
                         PyObject* args)
    {
        int error = 0;

        error = CommitCaseRecord();

        return Py_BuildValue("i",error);
    }

    PyObject*
    ext_CommitNewCase(PyObject* self,
                      PyObject* args)
    {
        int error = 0;

        error = CommitNewCase();

        return Py_BuildValue("i",error);
    }

    PyObject*
    ext_EndChanges(PyObject* self,
                   PyObject* args)
    {
        int error = 0;

        error = EndChanges();

        return Py_BuildValue("i",error);
    }

    PyObject * ext_SetVarCValueLabel(PyObject *self,
                                     PyObject *args)
    {
        int error = 0;
        char *varName;
        char *value;
        char *label;

        if(!PyArg_ParseTuple(args,"sss",&varName,&value,&label)) {
            return NULL;
        }

        error = SetVarCValueLabel(varName,value,label);

        return Py_BuildValue("i",error);
    }

    PyObject * ext_SetVarNValueLabel(PyObject *self,
                                         PyObject *args)
    {
        int error = 0;
        char *varName;
        double value;
        char *label;

        if(!PyArg_ParseTuple(args,"sds",&varName,&value,&label)) {
            return NULL;
        }

        error = SetVarNValueLabel(varName,value,label);

        return Py_BuildValue("i",error);
    }

    PyObject * ext_SetVarCMissingValues(PyObject *self,
                                        PyObject *args)
    {
        int error = 0;
        char *varName;
        int missingFormat;
        char *missingVal1 = NULL;
        char *missingVal2 = NULL;
        char *missingVal3 = NULL;

        if(!PyArg_ParseTuple(args,"si|sss",&varName,&missingFormat,&missingVal1,&missingVal2,&missingVal3)) {
            return NULL;
        }

        error = SetVarCMissingValues(varName,missingFormat,missingVal1,missingVal2,missingVal3);

        return Py_BuildValue("i",error);
    }

    PyObject * ext_SetVarNMissingValues(PyObject *self,
                                        PyObject *args)
    {
        int error = 0;
        int missingFormat;
        char* varName;
        double missingVal1;
        double missingVal2;
        double missingVal3;

        if(!PyArg_ParseTuple(args,"si|ddd",&varName,&missingFormat,&missingVal1,&missingVal2,&missingVal3)) {
            return NULL;
        }

        error = SetVarNMissingValues(varName,missingFormat,missingVal1,missingVal2,missingVal3);

        return Py_BuildValue("i",error);
    }

    PyObject * ext_SetVarMeasureLevel(PyObject *self,
                                          PyObject *args)
    {
        int error = 0;
        char *varName;
        int measureLevel;

        if(!PyArg_ParseTuple(args,"si",&varName,&measureLevel)) {
            return NULL;
        }

        error = SetVarMeasureLevel(varName,measureLevel);

        return Py_BuildValue("i",error);
    }

    PyObject * ext_SetVarAlignment(PyObject *self,
                                       PyObject *args)
    {
        int error = 0;
        char *varName;
        int aligment;

        if(!PyArg_ParseTuple(args,"si",&varName,&aligment)) {
            return NULL;
        }

        error = SetVarAlignment(varName,aligment);

        return Py_BuildValue("i",error);
    }

    PyObject * ext_SetVarFormat(PyObject *self,
                                     PyObject *args)
    {
        int error = 0;
        char* varName;
        int writeType;
        int writeDec;
        int writeWidth;

        if(!PyArg_ParseTuple(args,"siii",&varName,&writeType,&writeDec,&writeWidth)) {
            return NULL;
        }

        error = SetVarFormat(varName, writeType, writeDec, writeWidth);

        return Py_BuildValue("i",error);
    }

    PyObject * ext_HasCursor(PyObject *self,
                             PyObject *args)
    {
        int error = 0;
        int hasCur;

        error = HasCursor(hasCur);

        return Py_BuildValue("ii",hasCur,error);
    }


    PyObject * ext_IsEndSplit( PyObject *self,
                               PyObject *args)
    {
        int error = 0;
        int endSplit = 0;
        error =IsEndSplit(endSplit);        
        return Py_BuildValue("ii",endSplit,error);
    }

    PyObject *  ext_GetVarCMissingValues( PyObject *self,
                                              PyObject *args)
    {
        return GetVarCMissingValuesAll_XD(self,args,DS_MODE);
    }

    PyObject *  ext_GetVarNMissingValues( PyObject *self,
                                          PyObject *args)
    {
        return GetVarNMissingValuesAll_XD(self,args,DS_MODE);
    }

    PyObject *  ext_GetVarAttributeNames( PyObject *self,
                                          PyObject *args)
    {
        return GetVarAttributeNamesAll_XD(self,args,DS_MODE);
    }


    PyObject *  ext_GetVarAttributes( PyObject *self,
                                      PyObject *args)
    {
        return GetVarAttributesAll_XD(self,args,DS_MODE);
    }

    PyObject *  ext_SetVarAttributes( PyObject *self,
                                      PyObject *args)
    {
        int error = 0;
        int index;
        char *varName;
        char *attrName;
        char *attrValue;

        if(!PyArg_ParseTuple(args,"sssi",&varName,&attrName,&attrValue,&index)) {
            return NULL;
        }

        error = SetVarAttributes(varName,attrName,attrValue,index);

        return Py_BuildValue("i",error);

    }

    PyObject * ext_GetCaseCountInProcDS( PyObject *self,
                                         PyObject *args
                                         )
    {
        return GetCaseCountAll_XD(self,args,PROCDS_MODE);
    }

    PyObject * ext_GetVariableCountInProcDS( PyObject *self,
                                             PyObject *args
                                             )
    {
        return GetVariableCountAll_XD(self,args,PROCDS_MODE);
    }

    PyObject * ext_GetVariableFormatInProcDS( PyObject *self,
                                              PyObject *args
                                              )
    {
        return GetVariableFormatAll_XD(self,args,PROCDS_MODE);
    }

    PyObject * ext_GetVariableLabelInProcDS( PyObject *self,
                                             PyObject *args
                                             )
    {
        return GetVariableLabelAll_XD(self,args,PROCDS_MODE);
    }

    PyObject * ext_GetVariableMeasurementLevelInProcDS( PyObject *self,
                                                        PyObject *args
                                                        )
    {
        return GetVariableMeasurementLevelAll_XD(self,args,PROCDS_MODE);
    }

    PyObject * ext_GetVariableNameInProcDS( PyObject *self,
                                            PyObject *args
                                            )
    {
        return GetVariableNameAll_XD(self,args,PROCDS_MODE);
    }

    PyObject * ext_GetVariableTypeInProcDS( PyObject *self,
                                            PyObject *args
                                            )
    {
        return GetVariableTypeAll_XD(self,args,PROCDS_MODE);
    }

    PyObject *  ext_GetVarCMissingValuesInProcDS( PyObject *self,
                                                  PyObject *args
                                                  )
    {
        return GetVarCMissingValuesAll_XD(self,args,PROCDS_MODE);
    }

    PyObject *  ext_GetVarNMissingValuesInProcDS( PyObject *self,
                                                  PyObject *args
                                                  )
    {
        return GetVarNMissingValuesAll_XD(self,args,PROCDS_MODE);
    }

    PyObject *  ext_GetVarAttributeNamesInProcDS( PyObject *self,
                                                  PyObject *args)
    {
        return GetVarAttributeNamesAll_XD(self,args,PROCDS_MODE);
    }

    PyObject *  ext_GetVarAttributesInProcDS( PyObject *self,
                                              PyObject *args)
    {
        return GetVarAttributesAll_XD(self,args,PROCDS_MODE);
    }

    PyObject *  ext_SetUserMissingInclude( PyObject *self,
                                           PyObject *args)
    {
        int errLevel = 0;
        int incMissing = 0;
        PyObject *pDatasrcInfo = NULL;

        if(!PyArg_ParseTuple(args,"Oi",&pDatasrcInfo,&incMissing)) {
            errLevel = ERROR_PARAMETER;
            Py_INCREF(Py_None);
            return Py_BuildValue("iO",errLevel,Py_None);
        }
        SPSS_ResultObject *datasrc = (SPSS_ResultObject*)pDatasrcInfo;
        datasrc->incMissing = incMissing;

        Py_INCREF(Py_None);
        return Py_None;
    }

    PyObject *  ext_HasProcedure( PyObject *self,
                                  PyObject *args)
    {
        return Py_BuildValue("i",HasProcedure());
    }


    PyObject *  ext_GetSPSSLowHigh( PyObject *self,
                                    PyObject *args)
    {
        int errLevel = 0;

        double low = 0.0, high = 0.0;

        errLevel = GetSPSSLowHigh(&low,&high);

        return Py_BuildValue("ffi",low,high,errLevel);
    }


    PyObject *  ext_GetWeightVar( PyObject *self,
                                  PyObject *args)
    {
        int errLevel = 0;

        const char * varName = GetWeightVar(errLevel);

        if(varName && strlen(varName) > 0) {
            if(IsUTF8mode())
            {
                PyObject* pyValue = PyUnicode_Decode(varName,strlen(varName),"utf-8",NULL);
                PyObject* r = Py_BuildValue("Oi",pyValue,errLevel);
                Py_XDECREF(pyValue);
                return r;
            }
            else
            {
                return Py_BuildValue("si",varName,errLevel);
            }
        } else {
            return Py_BuildValue("Oi",Py_None,errLevel);
        }
    }

    PyObject *  ext_ResetDataPass( PyObject *self,
                                   PyObject *args)
    {         
        return Py_BuildValue("i",ResetDataPass());
    }

    PyObject * ext_ResetDataPassWrite( PyObject *self,
                                       PyObject *args)
    {
        int errLevel = 0;
        PyObject *conv = NULL;
        PyObject *datasrcInfo = NULL;
        PyObject *varTuple = NULL;

        if(!PyArg_ParseTuple(args,"OOO",&conv,&datasrcInfo,&varTuple)) {
            return NULL;
        }

        errLevel = ResetDataPass();
        if(0 == errLevel) {
            errLevel = ConstructDataSrcInfo(conv,(SPSS_ResultObject *)datasrcInfo,varTuple);
        }

        return Py_BuildValue("i",errLevel);
    }


    PyObject *  ext_AllocNewVarsBuffer( PyObject *self,
                                        PyObject *args)
    {
        int errLevel = 0;

        unsigned int size;

        if(!PyArg_ParseTuple(args,"I",&size)) {
            return NULL;
        }

        return Py_BuildValue("i",AllocNewVarsBuffer(size));
    }


    PyObject *  ext_SetOneVarNameAndType( PyObject *self,
                                          PyObject *args)
    {
        int errLevel = 0;
        char * varName;
        int varType;

        if(!PyArg_ParseTuple(args,"si",&varName,&varType)) {
            return NULL;
        }

        return Py_BuildValue("i",SetOneVarNameAndType(varName,varType));
    }

    PyObject *  ext_SetFetchVarList( PyObject *self,
                                     PyObject *args)
    {
        int errLevel = 0;
        PyObject *varTuple = NULL;
        PyObject *datasrc = NULL;
        PyObject *varDict = NULL;
        SPSS_ResultObject *datasrcInfo = NULL;
        int outCount = 0;


        if(!PyArg_ParseTuple(args,"OO",&datasrc,&varTuple)) {
            return NULL;
        }

        datasrcInfo = (SPSS_ResultObject *)datasrc;

        varDict = datasrcInfo->varDict;
        outCount = datasrcInfo->outCount;

        errLevel = MakeVarDict(datasrcInfo,varTuple);
        if(errLevel == 0) {
            Py_DECREF(varDict);
        } else {
            datasrcInfo->varDict = varDict;
            datasrcInfo->outCount = outCount;
        }

        return Py_BuildValue("i",errLevel);
    }

        // data step
        PYINVOKESPSS_API PyObject * ext_StartDataStep(
                                                      PyObject *self,
                                                      PyObject *args)
        {
                return Py_BuildValue("i", StartDataStep());
        }

        PYINVOKESPSS_API PyObject * ext_EndDataStep(
                                                      PyObject *self,
                                                      PyObject *args)
        {
                return Py_BuildValue("i", EndDataStep());
        }

        PYINVOKESPSS_API PyObject * ext_CreateDataset(
                                                      PyObject *self,
                                                      PyObject *args)
        {
            char* dsName ;
            bool isEmpty;
            bool hidden;
            if(!PyArg_ParseTuple(args,"sbb",&dsName,&isEmpty,&hidden)) {
                return NULL;
            }
            return Py_BuildValue("i", CreateDataset(dsName, isEmpty, hidden));
        }

        PYINVOKESPSS_API PyObject * ext_SetDatasetName(
                                                      PyObject *self,
                                                      PyObject *args)
        {
            char* oldName;
            char* newName;
            if (!PyArg_ParseTuple(args, "ss", &oldName, &newName))
                return NULL;
            return Py_BuildValue("i", SetDatasetName(oldName, newName));
        }

        PYINVOKESPSS_API PyObject * ext_GetNewDatasetName(
                                                      PyObject *self,
                                                      PyObject *args)
        {
            char newName[9+sizeof(int)];
            int errLevel = GetNewDatasetName( newName );

            if(IsUTF8mode()&&(errLevel==0))
            {
                PyObject* pyValue = PyUnicode_Decode(newName,strlen(newName),"utf-8",NULL);
                PyObject* r = Py_BuildValue("Oi", pyValue, errLevel);
                Py_XDECREF(pyValue);
                return r;
            }
            else
            {
                return Py_BuildValue("si", newName, errLevel);
            }
        }

        PYINVOKESPSS_API PyObject * ext_GetActive(
                                                   PyObject *self,
                                                   PyObject *args)
        {
            int errLevel;
            const char* acName = GetActive( errLevel );
            if ( 0 != acName )
            {
                if(IsUTF8mode())
                {
                    PyObject* pyValue = PyUnicode_Decode(acName,strlen(acName),"utf-8",NULL);
                    PyObject* r = Py_BuildValue("Oi", pyValue, errLevel);
                    Py_XDECREF(pyValue);
                    return r;
                }
                else
                {
                    return Py_BuildValue("si", acName, errLevel);
                }
            }
            else
                return Py_BuildValue("Oi", Py_None, errLevel);
        }

        PYINVOKESPSS_API PyObject * ext_SetActive(
                                                      PyObject *self,
                                                      PyObject *args)
        {
            char* dsName;
            if (!PyArg_ParseTuple(args, "s", &dsName))
                return NULL;
            return Py_BuildValue("i", SetActive(dsName));
        }

        PYINVOKESPSS_API PyObject * ext_CopyDataset(
                                                      PyObject *self,
                                                      PyObject *args)
        {
            char* oriName;
            char* desName;
            if (!PyArg_ParseTuple(args, "ss", &oriName, &desName))
                return NULL;
            return Py_BuildValue("i", CopyDataset(oriName, desName));
        }

        PYINVOKESPSS_API PyObject * ext_GetSpssDatasets(
                                                      PyObject *self,
                                                      PyObject *args)
        {
            char** dsNames;
            int length;
            int errLevel;
            errLevel = GetSpssDatasets( &dsNames, length );

            if(0 == errLevel) {
                PyObject* pyValues = PyTuple_New(length);
                for(int i = 0; i < length; i++) {
                    if(IsUTF8mode())
                    {
                        PyObject* pyValue = PyUnicode_Decode(dsNames[i],strlen(dsNames[i]),"utf-8",NULL);
                        PyTuple_SetItem(pyValues,i,Py_BuildValue("O",pyValue));
                        Py_XDECREF(pyValue);
                    }
                    else
                    {
                        PyTuple_SetItem(pyValues,i,Py_BuildValue("s",dsNames[i]));
                    }
                }

                FreeStringArray(dsNames, length);
                PyObject* r = Py_BuildValue("Oi",pyValues,errLevel);
                Py_DECREF(pyValues);

                return r;
            } else {
                return Py_BuildValue("Oi",Py_None,errLevel);
            }
        }

        PYINVOKESPSS_API PyObject * ext_GetDatastepDatasets(
                                                      PyObject *self,
                                                      PyObject *args)
        {
            char** dsNames;
            int length;
            int errLevel;
            errLevel = GetDatastepDatasets( &dsNames, length );

            if(0 == errLevel) {
                PyObject* pyValues = PyTuple_New(length);
                for(int i = 0; i < length; i++) {
                    if(IsUTF8mode())
                    {
                        PyObject* pyValue = PyUnicode_Decode(dsNames[i],strlen(dsNames[i]),"utf-8",NULL);
                        PyTuple_SetItem(pyValues,i,Py_BuildValue("O",pyValue));
                        Py_XDECREF(pyValue);
                    }
                    else
                    {
                        PyTuple_SetItem(pyValues,i,Py_BuildValue("s",dsNames[i]));
                    }
                }

                FreeStringArray(dsNames, length);
                PyObject* r = Py_BuildValue("Oi",pyValues,errLevel);
                Py_DECREF(pyValues);

                return r;
            } else {
                return Py_BuildValue("Oi",Py_None,errLevel);
            }
        }

        PYINVOKESPSS_API PyObject * ext_CloseDataset(
                                                      PyObject *self,
                                                      PyObject *args)
        {
            char* dsName;
            if (!PyArg_ParseTuple(args, "s", &dsName))
                return NULL;
            return Py_BuildValue("i", CloseDataset( dsName ));
        }

        PYINVOKESPSS_API PyObject * ext_InsertVariable(
                                                      PyObject *self,
                                                      PyObject *args)
        {
            char* dsName;
            int index;
            char* varName;
            int type;
            if (!PyArg_ParseTuple(args, "sisi", &dsName, &index, &varName, &type))
                return NULL;
            return Py_BuildValue("i", InsertVariable(dsName, index, varName, type));
        }

        PYINVOKESPSS_API PyObject * ext_DeleteVariable(
                                                      PyObject *self,
                                                      PyObject *args)
        {
            char* dsName;
            int index;
            if (!PyArg_ParseTuple(args, "si", &dsName, &index))
                return NULL;
            return Py_BuildValue("i", DeleteVariable(dsName, index));
        }

        PYINVOKESPSS_API PyObject * ext_GetVarCountInDS(
                                                      PyObject *self,
                                                      PyObject *args)
        {
            char* dsName;
            if (!PyArg_ParseTuple(args, "s", &dsName))
                return NULL;
            int errLevel;
            int count = GetVarCountInDS(dsName, errLevel);
            return Py_BuildValue("ii", count, errLevel);
        }

        PYINVOKESPSS_API PyObject * ext_GetVarNameInDS(
                                                      PyObject *self,
                                                      PyObject *args)
        {
            char* dsName;
            int index;
            int errLevel;
            
            if (!PyArg_ParseTuple(args, "si", &dsName, &index))
                return NULL;
            const char* varName = GetVarNameInDS(dsName, index, errLevel);
            if(IsUTF8mode()&&(errLevel==0))
            {
                PyObject* pyValue = PyUnicode_Decode(varName,strlen(varName),"utf-8",NULL);
                PyObject* r = Py_BuildValue("Oi", pyValue, errLevel);
                Py_XDECREF(pyValue);
                return r;
            }
            else
                return Py_BuildValue("si", varName, errLevel);
        }

        PYINVOKESPSS_API PyObject * ext_SetVarNameInDS(
                                                      PyObject *self,
                                                      PyObject *args)
        {
            char* dsName;
            int index;
            char* varName;
            if (!PyArg_ParseTuple(args, "sis", &dsName, &index, &varName))
                return NULL;
            return Py_BuildValue("i", SetVarNameInDS(dsName, index, varName));
        }

        PYINVOKESPSS_API PyObject * ext_GetVarLabelInDS(
                                                      PyObject *self,
                                                      PyObject *args)
        {
            char* dsName;
            int index;
            int errLevel;
            if (!PyArg_ParseTuple(args, "si", &dsName, &index))
                return NULL;
            const char* varLabel = GetVarLabelInDS(dsName, index, errLevel);
            if (!strcmp(varLabel, ""))
                return Py_BuildValue("Oi", Py_None, errLevel);

            if(IsUTF8mode()&&(errLevel==0))
            {
                PyObject* pyValue = PyUnicode_Decode(varLabel,strlen(varLabel),"utf-8",NULL);
                PyObject* r = Py_BuildValue("Oi", pyValue, errLevel);
                Py_XDECREF(pyValue);
                return r;
            }
            else
            {
                return Py_BuildValue("si", varLabel, errLevel);
            }
        }

        PYINVOKESPSS_API PyObject * ext_SetVarLabelInDS(
                                                      PyObject *self,
                                                      PyObject *args)
        {
            char* dsName;
            int index;
            char* varLabel;
            if (!PyArg_ParseTuple(args, "sis", &dsName, &index, &varLabel))
                return NULL;
            return Py_BuildValue("i", SetVarLabelInDS(dsName, index, varLabel));
        }

        PYINVOKESPSS_API PyObject * ext_GetVarTypeInDS(
                                                      PyObject *self,
                                                      PyObject *args)
        {
            char* dsName;
            int index;
            int errLevel;
            if (!PyArg_ParseTuple(args, "si", &dsName, &index))
                return NULL;
            int varType = GetVarTypeInDS(dsName, index, errLevel);
            return Py_BuildValue("ii", varType, errLevel);
        }

        PYINVOKESPSS_API PyObject * ext_SetVarTypeInDS(
                                                      PyObject *self,
                                                      PyObject *args)
        {
            char* dsName;
            int index;
            int varType;
            if (!PyArg_ParseTuple(args, "sii", &dsName, &index, &varType))
                return NULL;
            return Py_BuildValue("i", SetVarTypeInDS(dsName, index, varType));
        }

        PYINVOKESPSS_API PyObject * ext_GetVarFormatInDS(
                                                      PyObject *self,
                                                      PyObject *args)
        {
            char* dsName;
            int index;
            int errLevel;
            if (!PyArg_ParseTuple(args, "si", &dsName, &index))
                return NULL;
            const char* varFormat = GetVarFormatInDS(dsName, index, errLevel);
            if(IsUTF8mode()&&(errLevel==0))
            {
                PyObject* pyValue = PyUnicode_Decode(varFormat,strlen(varFormat),"utf-8",NULL);
                PyObject* r = Py_BuildValue("Oi", pyValue, errLevel);
                Py_XDECREF(pyValue);
                return r;
            }
            else
            {
                return Py_BuildValue("si", varFormat, errLevel);
            }
        }

        PYINVOKESPSS_API PyObject * ext_SetVarFormatInDS(
                                                      PyObject *self,
                                                      PyObject *args)
        {
            char* dsName;
            int index;
            int formatType;
            int formatWidth;
            int formatDecimal;
            if (!PyArg_ParseTuple(args, "siiii", &dsName, &index, &formatType, &formatWidth, &formatDecimal))
                return NULL;
            return Py_BuildValue("i", SetVarFormatInDS(dsName, index, formatType, formatWidth, formatDecimal));
        }

        PYINVOKESPSS_API PyObject * ext_GetVarAlignmentInDS(
                                                      PyObject *self,
                                                      PyObject *args)
        {
            char* dsName;
            int index;
            int errLevel;
            if (!PyArg_ParseTuple(args, "si", &dsName, &index))
                return NULL;
            int varAlignment = GetVarAlignmentInDS(dsName, index, errLevel);
            return Py_BuildValue("ii", varAlignment, errLevel);
        }

        PYINVOKESPSS_API PyObject * ext_SetVarAlignmentInDS(
                                                      PyObject *self,
                                                      PyObject *args)
        {
            char* dsName;
            int index;
            int varAlignment;
            if (!PyArg_ParseTuple(args, "sii", &dsName, &index, &varAlignment))
                return NULL;
            return Py_BuildValue("i", SetVarAlignmentInDS(dsName, index, varAlignment));
        }

        PYINVOKESPSS_API PyObject * ext_GetVarMeasurementLevelInDS(
                                                      PyObject *self,
                                                      PyObject *args)
        {
            char* dsName;
            int index;
            int errLevel;
            if (!PyArg_ParseTuple(args, "si", &dsName, &index))
                return NULL;
            const char* varMeasurement = GetVarMeasurementLevelInDS(dsName, index, errLevel);
            if(IsUTF8mode()&&(errLevel==0))
            {
                PyObject* pyValue = PyUnicode_Decode(varMeasurement,strlen(varMeasurement),"utf-8",NULL);
                PyObject* r = Py_BuildValue("Oi", pyValue, errLevel);
                Py_XDECREF(pyValue);
                return r;
            }
            else
            {
                return Py_BuildValue("si", varMeasurement, errLevel);
            }
        }

        PYINVOKESPSS_API PyObject * ext_SetVarMeasurementLevelInDS(
                                                      PyObject *self,
                                                      PyObject *args)
        {
            char* dsName;
            int index;
            char* varMeasurement;
            if (!PyArg_ParseTuple(args, "sis", &dsName, &index, &varMeasurement))
                return NULL;
            return Py_BuildValue("i", SetVarMeasurementLevelInDS(dsName, index, varMeasurement));
        }

        PYINVOKESPSS_API PyObject * ext_GetVarNMissingValuesInDS(
                                                      PyObject *self,
                                                      PyObject *args)
        {
            char* dsName;
            int index;

            if (!PyArg_ParseTuple(args, "si", &dsName, &index))
                return NULL;

            int missingFormat;
            double missingVal1;
            double missingVal2;
            double missingVal3;

            int errLevel = GetVarNMissingValuesInDS(dsName, index, missingFormat, &missingVal1, &missingVal2, &missingVal3);

            PyObject *pvalue = NULL;

            switch(missingFormat)
            {
                case 0:
                {
                    pvalue = Py_BuildValue("iOOOi",missingFormat,Py_None,Py_None,Py_None,errLevel);
                }
                break;
                case 1:
                {
                    pvalue = Py_BuildValue("idOOi",missingFormat,missingVal1,Py_None,Py_None,errLevel);
                }
                break;
                case 2:
                case -2:
                {
                    pvalue = Py_BuildValue("iddOi",missingFormat,missingVal1,missingVal2,Py_None,errLevel);
                }
                break;
                case 3:
                case -3:
                {
                    pvalue = Py_BuildValue("idddi",missingFormat,missingVal1,missingVal2,missingVal3,errLevel);
                }
                break;
                default:
                {
                }
                break;
            }
            return pvalue;
        }

        PYINVOKESPSS_API PyObject * ext_GetVarCMissingValuesInDS(
                                                      PyObject *self,
                                                      PyObject *args)
        {
            char* dsName;
            int index;

            if (!PyArg_ParseTuple(args, "si", &dsName, &index))
                return NULL;

            int missingFormat;
            char missingVal1[MISSING_VAL_LEN + 1];
            char missingVal2[MISSING_VAL_LEN + 1];
            char missingVal3[MISSING_VAL_LEN + 1];

            memset(missingVal1,'\0',MISSING_VAL_LEN + 1);
            memset(missingVal2,'\0',MISSING_VAL_LEN + 1);
            memset(missingVal3,'\0',MISSING_VAL_LEN + 1);

            int errLevel = GetVarCMissingValuesInDS(dsName, index, missingFormat, missingVal1, missingVal2, missingVal3);

            PyObject *pvalue = NULL;
            int utf8mode = IsUTF8mode();
            switch(missingFormat)
            {
                case 0:
                {
                    pvalue = Py_BuildValue("iOOOi",missingFormat,Py_None,Py_None,Py_None,errLevel);
                }
                break;
                case 1:
                {
                    if(utf8mode&&(errLevel==0))
                    {
                        PyObject* pyValue = PyUnicode_Decode(missingVal1,strlen(missingVal1),"utf-8",NULL);
                        pvalue = Py_BuildValue("iOOOi",missingFormat,pyValue,Py_None,Py_None,errLevel);
                        Py_XDECREF(pyValue);
                    }
                    else
                    {
                        pvalue = Py_BuildValue("isOOi",missingFormat,missingVal1,Py_None,Py_None,errLevel);
                    }
                }
                break;
                case 2:
                {
                    if(utf8mode&&(errLevel==0))
                    {
                        PyObject* pyValueOne = PyUnicode_Decode(missingVal1,strlen(missingVal1),"utf-8",NULL);
                        PyObject* pyValueTwo = PyUnicode_Decode(missingVal2,strlen(missingVal2),"utf-8",NULL);
                        pvalue = Py_BuildValue("iOOOi",missingFormat,pyValueOne,pyValueTwo,Py_None,errLevel);
                        Py_XDECREF(pyValueOne);
                        Py_XDECREF(pyValueTwo);
                    }
                    else
                    {
                        pvalue = Py_BuildValue("issOi",missingFormat,missingVal1,missingVal2,Py_None,errLevel);
                    }                       
                }
                break;
                case 3:
                {
                    if(utf8mode&&(errLevel==0))
                    {
                        PyObject* pyValueOne = PyUnicode_Decode(missingVal1,strlen(missingVal1),"utf-8",NULL);
                        PyObject* pyValueTwo = PyUnicode_Decode(missingVal2,strlen(missingVal2),"utf-8",NULL);
                        PyObject* pyValueThree = PyUnicode_Decode(missingVal3,strlen(missingVal3),"utf-8",NULL);
                        pvalue = Py_BuildValue("iOOOi",missingFormat,pyValueOne,pyValueTwo,pyValueThree,errLevel);
                        Py_XDECREF(pyValueOne);
                        Py_XDECREF(pyValueTwo);
                        Py_XDECREF(pyValueThree);
                    }
                    else
                    {
                        pvalue = Py_BuildValue("isssi",missingFormat,missingVal1,missingVal2,missingVal3,errLevel);
                    }
                }
                break;
                default:
                {
                }
                break;
            }

            return pvalue;
        }

        PYINVOKESPSS_API PyObject * ext_SetVarNMissingValuesInDS(
                                                      PyObject *self,
                                                      PyObject *args)
        {
            char* dsName;
            int index;
            int missingFormat;
            double missingValue1;
            double missingValue2;
            double missingValue3;
            if (!PyArg_ParseTuple(args, "siiddd", &dsName, &index, &missingFormat, &missingValue1, &missingValue2, &missingValue3))
                return NULL;
            return Py_BuildValue("i", SetVarNMissingValuesInDS(dsName, index, missingFormat, missingValue1, missingValue2, missingValue3));
        }

        PYINVOKESPSS_API PyObject * ext_SetVarCMissingValuesInDS(
                                                      PyObject *self,
                                                      PyObject *args)
        {
            char* dsName;
            int index;
            int missingFormat;
            char* missingValue1;
            char* missingValue2;
            char* missingValue3;
            if (!PyArg_ParseTuple(args, "siisss", &dsName, &index, &missingFormat, &missingValue1, &missingValue2, &missingValue3))
                return NULL;
            return Py_BuildValue("i", SetVarCMissingValuesInDS(dsName, index, missingFormat, missingValue1, missingValue2, missingValue3));
        }

        PYINVOKESPSS_API PyObject * ext_GetVarAttributesNameInDS(
                                                      PyObject *self,
                                                      PyObject *args)
        {
            char* dsName;
            int index;
            char** attNames;
            int length;

            if (!PyArg_ParseTuple(args, "si", &dsName, &index))
                return NULL;

            int errLevel = GetVarAttributesNameInDS(dsName, index, &attNames, length);

            if(0 == errLevel) {
                PyObject* pyValues = PyTuple_New(length);
                for(int i = 0; i < length; i++) {
                    if(IsUTF8mode())
                    {
                        PyObject* pyValue = PyUnicode_Decode(attNames[i],strlen(attNames[i]),"utf-8",NULL);
                        PyTuple_SetItem(pyValues,i,Py_BuildValue("O", pyValue));
                        Py_XDECREF(pyValue);
                    }
                    else
                    {
                        PyTuple_SetItem(pyValues,i,Py_BuildValue("s",attNames[i]));
                    }
                }

                FreeStringArray(attNames, length);
                
                PyObject* r = Py_BuildValue("Oi",pyValues,errLevel);
                Py_DECREF(pyValues);

                return r;
            } else {
                return Py_BuildValue("Oi",Py_None,errLevel);
            }
        }

        PYINVOKESPSS_API PyObject * ext_GetVarAttributesInDS(
                                                      PyObject *self,
                                                      PyObject *args)
        {
            char* dsName;
            int index;
            char* attName;
            char** values;
            int length;

            if (!PyArg_ParseTuple(args, "sis", &dsName, &index, &attName))
                return NULL;

            int errLevel = GetVarAttributesInDS(dsName, index, attName, &values, length);

            if(0 == errLevel) {
                PyObject* pyValues = PyTuple_New(length);
                for(int i = 0; i < length; i++) {
                    if(IsUTF8mode())
                    {
                        PyObject* pyValue = PyUnicode_Decode(values[i],strlen(values[i]),"utf-8",NULL);
                        PyTuple_SetItem(pyValues,i, Py_BuildValue("O",pyValue));
                        Py_XDECREF(pyValue);
                    }
                    else
                    {
                        PyTuple_SetItem(pyValues,i,Py_BuildValue("s",values[i]));
                    }
                }

                FreeStringArray(values, length);
                PyObject* r = Py_BuildValue("Oi",pyValues,errLevel);
                Py_DECREF(pyValues);

                return r;
            } else {
                return Py_BuildValue("Oi",Py_None,errLevel);
            }
        }

        PYINVOKESPSS_API PyObject * ext_SetVarAttributesInDS(
                                                      PyObject *self,
                                                      PyObject *args)
        {
            char* dsName;
            int index;
            char* attName;
            char** values;
            int length;
            PyObject* pylist;
            if (!PyArg_ParseTuple(args, "sisO", &dsName, &index, &attName, &pylist))
                return NULL;

            int nameSize = 0;
            int errLevel = parse2CharStarStar(pylist,&values,&length);
            if(errLevel != 0) {
                return Py_BuildValue("i",errLevel);
            }

            errLevel = SetVarAttributesInDS(dsName, index, attName, values, length);
            for(int i = 0; i < length; i++) {
                PyMem_Free(values[i]);
            }
            PyMem_Free(values);

            return Py_BuildValue("i", errLevel);
        }

        PYINVOKESPSS_API PyObject * ext_DelVarAttributesInDS(
                                                      PyObject *self,
                                                      PyObject *args)
        {
            char* dsName;
            int index;
            char* attName;

            if (!PyArg_ParseTuple(args, "sis", &dsName, &index, &attName))
                return NULL;

            return Py_BuildValue("i", DelVarAttributesInDS(dsName, index, attName));
        }

        PYINVOKESPSS_API PyObject * ext_GetVarNValueLabelInDS(
                                                      PyObject *self,
                                                      PyObject *args)
        {
            char* dsName;
            int index;
            double* values;
            char** labels;
            int length;

            if (!PyArg_ParseTuple(args, "si", &dsName, &index))
                return NULL;

            int errLevel = GetVarNValueLabelInDS(dsName, index, &values, &labels, length);

            if(0 == errLevel) {
                PyObject* pyValues = PyDict_New();
                int utf8mode = IsUTF8mode();
                for(int i = 0; i < length; i++) {
                    if(utf8mode)
                    {
                        PyObject* pyValue = PyUnicode_Decode(labels[i],strlen(labels[i]),"utf-8",NULL);
                        PyDict_SetItem(pyValues, Py_BuildValue("d", values[i]), Py_BuildValue("O",pyValue));
                        Py_XDECREF(pyValue);
                    }
                    else
                    {
                        PyDict_SetItem(pyValues, Py_BuildValue("d", values[i]), Py_BuildValue("s",labels[i]));
                    }
                }

                FreeDoubleArray(values, length);
                FreeStringArray(labels, length);
                
                PyObject* r = Py_BuildValue("Oi",pyValues,errLevel);
                Py_DECREF(pyValues);

                return r;
            } else {
                return Py_BuildValue("Oi",Py_None,errLevel);
            }
        }

        PYINVOKESPSS_API PyObject * ext_GetVarCValueLabelInDS(
                                                      PyObject *self,
                                                      PyObject *args)
        {
            char* dsName;
            int index;
            char** values;
            char** labels;
            int length;

            if (!PyArg_ParseTuple(args, "si", &dsName, &index))
                return NULL;

            int errLevel = GetVarCValueLabelInDS(dsName, index, &values, &labels, length);
            int utf8mode = IsUTF8mode();
            if(0 == errLevel) {
                PyObject* pyValues = PyDict_New();
                for(int i = 0; i < length; i++) {
                    if(utf8mode)
                    {
                        PyObject* pyValueOne = PyUnicode_Decode(values[i],strlen(values[i]),"utf-8",NULL);
                        PyObject* pyValueTwo = PyUnicode_Decode(labels[i],strlen(labels[i]),"utf-8",NULL);
                        PyDict_SetItem(pyValues, Py_BuildValue("O", pyValueOne), Py_BuildValue("O", pyValueTwo));
                        Py_XDECREF(pyValueOne);
                        Py_XDECREF(pyValueTwo);
                    }
                    else
                    {
                        PyDict_SetItem(pyValues, Py_BuildValue("s", values[i]),Py_BuildValue("s",labels[i]));
                    }
                }

                FreeStringArray(values, length);
                FreeStringArray(labels, length);
                
                PyObject* r = Py_BuildValue("Oi",pyValues,errLevel);
                Py_DECREF(pyValues);

                return r;
            } else {
                return Py_BuildValue("Oi",Py_None,errLevel);
            }
        }

        PYINVOKESPSS_API PyObject * ext_SetVarNValueLabelInDS(
                                                      PyObject *self,
                                                      PyObject *args)
        {
            char* dsName;
            int error, index;
            double value;
            char* label;

            if (!PyArg_ParseTuple(args, "sids", &dsName, &index, &value, &label))
                return NULL;
                
            error = SetVarNValueLabelInDS(dsName, index, value, label);
            return Py_BuildValue("i", error);
        }
        
        PYINVOKESPSS_API PyObject * ext_SetVarNValueLabelsInDS(
                                                      PyObject *self,
                                                      PyObject *args)
        {
            char* dsName;
            int error, index, valueSize = 0, labelSize = 0, i=0;
            double *values;
            char **labels;
            PyObject *pyValue, *pyLabel;

            if(!PyArg_ParseTuple(args,"siOO", &dsName, &index, &pyValue,&pyLabel)) {
                return NULL;
            }
    
            error = parse2DoubleStar(pyValue,&values,&valueSize);
            if(error != 0) {
                PyMem_Free(values);
                return Py_BuildValue("i",error);
            }

            error = parse2CharStarStar(pyLabel,&labels,&labelSize);
            if(error != 0) {
                for(i = 0; i < labelSize; i++) {
                    PyMem_Free(labels[i]);
                }
                PyMem_Free(labels);
                return Py_BuildValue("i",error);
            }
            
            if (valueSize != labelSize){
                for(i = 0; i < labelSize; i++) {
                    PyMem_Free(labels[i]);
                }
                PyMem_Free(labels);
                PyMem_Free(values);
            }
            assert (valueSize == labelSize);

            for ( i=0; i<valueSize; ++i){
                error = SetVarNValueLabelInDS(dsName, index, values[i], labels[i]);
            }
            
            for(i = 0; i < labelSize; i++) {
                PyMem_Free(labels[i]);
            }
            PyMem_Free(labels);
    
            PyMem_Free(values);

            return Py_BuildValue("i", error);
    
        }

        PYINVOKESPSS_API PyObject * ext_SetVarCValueLabelInDS(
                                                      PyObject *self,
                                                      PyObject *args)
        {
            char* dsName;
            int index;
            char* value;
            char* label;

            if (!PyArg_ParseTuple(args, "siss", &dsName, &index, &value, &label))
                return NULL;

            return Py_BuildValue("i", SetVarCValueLabelInDS(dsName, index, value, label));
        }

        PYINVOKESPSS_API PyObject * ext_SetVarCValueLabelsInDS(
                                                      PyObject *self,
                                                      PyObject *args)
        {
            char* dsName;
            int error, index, valueSize = 0, labelSize = 0, i=0;
            char **values, **labels;
            PyObject *pyValue, *pyLabel;

            if(!PyArg_ParseTuple(args,"siOO", &dsName, &index, &pyValue,&pyLabel)) {
                return NULL;
            }
    
            error = parse2CharStarStar(pyValue,&values,&valueSize);
            if(error != 0) {
                for(i = 0; i < valueSize; i++) {
                    PyMem_Free(values[i]);
                }
                PyMem_Free(values);
                return Py_BuildValue("i",error);
            }

            error = parse2CharStarStar(pyLabel,&labels,&labelSize);
            if(error != 0) {
                for(i = 0; i < labelSize; i++) {
                    PyMem_Free(labels[i]);
                }
                PyMem_Free(labels);
                return Py_BuildValue("i",error);
            }
            
            if (valueSize != labelSize){
                for(i = 0; i < labelSize; i++) {
                    PyMem_Free(labels[i]);
                }
                PyMem_Free(labels);
                for(i = 0; i < valueSize; i++) {
                    PyMem_Free(values[i]);
                }
                PyMem_Free(values);
            }
            assert (valueSize == labelSize);

            for ( i=0; i<valueSize; ++i){
                error = SetVarCValueLabelInDS(dsName, index, values[i], labels[i]);
            }
            
            for(i = 0; i < labelSize; i++) {
                PyMem_Free(labels[i]);
            }
            PyMem_Free(labels);
            for(i = 0; i < valueSize; i++) {
                PyMem_Free(values[i]);
            }
            PyMem_Free(values);

            return Py_BuildValue("i", error);
        }

        PYINVOKESPSS_API PyObject * ext_DelVarValueLabelInDS(
                                                      PyObject *self,
                                                      PyObject *args)
        {
            char* dsName;
            int index;

            if (!PyArg_ParseTuple(args, "si", &dsName, &index))
                return NULL;

            return Py_BuildValue("i", DelVarValueLabelInDS(dsName, index));
        }

        PYINVOKESPSS_API PyObject * ext_DelVarNValueLabelInDS(
                                                      PyObject *self,
                                                      PyObject *args)
        {
            char* dsName;
            int index;
            double delValue;

            if (!PyArg_ParseTuple(args, "sid", &dsName, &index, &delValue))
                return NULL;

            return Py_BuildValue("i", DelVarNValueLabelInDS(dsName, index, delValue));
        }

        PYINVOKESPSS_API PyObject * ext_DelVarCValueLabelInDS(
                                                      PyObject *self,
                                                      PyObject *args)
        {
            char* dsName;
            int index;
            char* delValue;

            if (!PyArg_ParseTuple(args, "sis", &dsName, &index, &delValue))
                return NULL;

            return Py_BuildValue("i", DelVarCValueLabelInDS(dsName, index, delValue));
        }

        PYINVOKESPSS_API PyObject * ext_InsertCase(
                                                      PyObject *self,
                                                      PyObject *args)
        {
            char* dsName;
            int rowIndex;
            if (!PyArg_ParseTuple(args, "si", &dsName, &rowIndex))
                return NULL;
            return Py_BuildValue("i", InsertCase( dsName, rowIndex));
        }

        PYINVOKESPSS_API PyObject * ext_DeleteCase(
                                                      PyObject *self,
                                                      PyObject *args)
        {
            char* dsName;
            int rowIndex;
            if (!PyArg_ParseTuple(args, "si", &dsName, &rowIndex))
                return NULL;
            return Py_BuildValue("i", DeleteCase( dsName, rowIndex));
        }

        PYINVOKESPSS_API PyObject * ext_GetCaseCountInDS(
                                                      PyObject *self,
                                                      PyObject *args)
        {
            char* dsName;
            int errLevel;
            if (!PyArg_ParseTuple(args, "s", &dsName))
                return NULL;
            int count = GetCaseCountInDS(dsName, errLevel);

            return Py_BuildValue("ii", count, errLevel);
        }

        PYINVOKESPSS_API PyObject * ext_GetNCellValue(
                                                      PyObject *self,
                                                      PyObject *args)
        {
            char* dsName;
            int rowIndex;
            int columnIndex;
            int isMissing;
            int errLevel;
            if (!PyArg_ParseTuple(args, "sii", &dsName, &rowIndex, &columnIndex))
                return NULL;
            double value = GetNCellValue(dsName, rowIndex, columnIndex, isMissing, errLevel);

            if (isMissing == 2)
                return Py_BuildValue("Oi", Py_None, errLevel);
            else
                return Py_BuildValue("di", value, errLevel);
        }
        
        PYINVOKESPSS_API PyObject * ext_GetCellsValue(
                                                      PyObject *self,
                                                      PyObject *args)
        {
            char* dsName;
            int rowIndex;
            int varNumber;
            int isMissing;
            int errLevel;
            if (!PyArg_ParseTuple(args, "sii", &dsName, &rowIndex, &varNumber))
                return NULL;
            PyObject *pyValueList = NULL;
            
            pyValueList = PyList_New(varNumber);
            
            for ( int i=0; i<varNumber; ++i){
                int varType = GetVarTypeInDS(dsName, i, errLevel);
                //int varType = 0;
                if (0 == varType){
                    double value = GetNCellValue(dsName, rowIndex, i, isMissing, errLevel);
                
                    if (isMissing == 2){
                        Py_INCREF(Py_None);
                        PyList_SetItem(pyValueList, i, Py_None);
                    }
                    else
                        PyList_SetItem(pyValueList, i, Py_BuildValue("d", value));
                }
                else{
                    const char* value = GetCCellValue(dsName, rowIndex, i, isMissing, errLevel);
                    if (IsUTF8mode()&&(0==errLevel))
                    {
                        PyObject* pyValue = PyUnicode_Decode(value,strlen(value),"utf-8",NULL);
                        PyList_SetItem(pyValueList, i, Py_BuildValue("O", pyValue));
                        Py_XDECREF(pyValue);
                    }
                    else
                        PyList_SetItem(pyValueList, i, Py_BuildValue("s", value));
                }
            }
            
            PyObject* r = Py_BuildValue("Oi", pyValueList, errLevel);
            Py_DECREF(pyValueList);

            return r;
        }
        
        PYINVOKESPSS_API PyObject * ext_GetCellsValueCache(
                                                      PyObject *self,
                                                      PyObject *args)
        {
            char* dsName;
            int rowIndex;
            int varNumber;
            int isMissing;
            int dsChange;
            int errLevel;
            if (!PyArg_ParseTuple(args, "siii", &dsName, &rowIndex, &varNumber, &dsChange))
                return NULL;
            PyObject *pyRowList = NULL;
            
            long rowStart;
            long rowEnd;
            long rowCount;
            int columnIndex = 0;
            long linecount = 0;
            if (dsChange > 0 ){
                ClearDatastepBatch();
            }
            GetRowList(dsName, rowIndex, columnIndex, rowStart, rowEnd, rowCount, errLevel);
            pyRowList = PyList_New(0);
            
            int* varType = new int[varNumber];
            GetVarTypeInDSCache(varType, varNumber, errLevel);
            
            for ( int j=rowStart; j<=rowEnd; ++j ){
                PyObject *pyValueList = NULL;
                pyValueList = PyList_New(varNumber);
                for ( int i=0; i<varNumber; ++i){
                    if (0 == varType[i]){
                        double value = GetNCellValueCache(dsName, j, i, isMissing, errLevel);
                        if (isMissing == 2){
                            Py_INCREF(Py_None);
                            PyList_SetItem(pyValueList, i, Py_None);
                        }
                        else
                            PyList_SetItem(pyValueList, i, Py_BuildValue("d", value));
                    }
                    else{
                        const char* value = GetCCellValueCache(dsName, j, i, isMissing, errLevel);
                        if (IsUTF8mode()&&(0==errLevel))
                        {
                            PyObject* pyValue = PyUnicode_Decode(value,strlen(value),"utf-8",NULL);
                            PyList_SetItem(pyValueList, i, Py_BuildValue("O", pyValue));
                            Py_XDECREF(pyValue);
                        }
                        else
                            PyList_SetItem(pyValueList, i, Py_BuildValue("s", value));
                            
                        FreeString((char*)value);
                    }
                }
                PyList_Append(pyRowList, pyValueList);
                Py_DECREF(pyValueList);
            }
            
            delete []varType;
            varType = 0;
            
            PyObject* r = Py_BuildValue("iiOi", rowStart, rowEnd, pyRowList, errLevel);
            Py_DECREF(pyRowList);

            return r;
        }

        PYINVOKESPSS_API PyObject * ext_GetCCellValue(
                                                      PyObject *self,
                                                      PyObject *args)
        {
            char* dsName;
            int rowIndex;
            int columnIndex;
            int isMissing;
            int errLevel;
            if (!PyArg_ParseTuple(args, "sii", &dsName, &rowIndex, &columnIndex))
                return NULL;
            const char* value = GetCCellValue(dsName, rowIndex, columnIndex, isMissing, errLevel);
            if(IsUTF8mode()&&(errLevel==0))
            {
                PyObject* pyValue = PyUnicode_Decode(value,strlen(value),"utf-8",NULL);
                PyObject* r = Py_BuildValue("Oi", pyValue, errLevel);
                Py_XDECREF(pyValue);
                return r;
            }
            else
            {
                return Py_BuildValue("si", value, errLevel);
            }
        }

        PYINVOKESPSS_API PyObject * ext_SetNCellValue(
                                                      PyObject *self,
                                                      PyObject *args)
        {
            char* dsName;
            int rowIndex;
            int columnIndex;
            double value;
            if (!PyArg_ParseTuple(args, "siid", &dsName, &rowIndex, &columnIndex, &value))
                return NULL;

            return Py_BuildValue("i", SetNCellValue(dsName, rowIndex, columnIndex, value));
        }

        PYINVOKESPSS_API PyObject * ext_SetCCellValue(
                                                      PyObject *self,
                                                      PyObject *args)
        {
            char* dsName;
            int rowIndex;
            int columnIndex;
            char* value;
            if (!PyArg_ParseTuple(args, "siis", &dsName, &rowIndex, &columnIndex, &value))
                return NULL;

            return Py_BuildValue("i", SetCCellValue(dsName, rowIndex, columnIndex, value));
        }

        PYINVOKESPSS_API PyObject * ext_IsUTF8mode( PyObject *self,
                                                    PyObject *args)
        {
            return Py_BuildValue("i",IsUTF8mode());
        }

        PYINVOKESPSS_API PyObject * ext_GetSplitVariableNames( PyObject *self,
                                                               PyObject *args)
        {
            void* result;
            int errLevel;

            result = GetSplitVariableNames(errLevel);
            int size = GetStringListLength(result);

            //if evaluate xpath failure
            if ( 0 != errLevel || 0 == result){
                return Py_BuildValue("[i]",errLevel);
            }

            //pass the result to python object
            int utf8mode = IsUTF8mode();
            PyObject* outList = PyList_New(size+1);
            for( int i=0; i<size; i++){
                const char *str = GetStringFromList(result,i);
                PyObject* item = NULL;
                if(utf8mode)
                {
                    PyObject* pyValue = PyUnicode_Decode(str,strlen(str),"utf-8",NULL);
                    item = Py_BuildValue("O",pyValue);
                    Py_XDECREF(pyValue);
                }
                else
                {
                    item = Py_BuildValue("s",str);
                }
                PyList_SET_ITEM(outList,i,item);
            }
            RemoveStringList(result);

            PyObject* errCode = Py_BuildValue("i",errLevel);
            PyList_SET_ITEM(outList,size,errCode);

            return outList;
        }

    PYINVOKESPSS_API PyObject * ext_GetDataFileAttributeNames( PyObject *self,
                                                               PyObject *args)
    {
        return GetDataFileAttributeNamesAll_XD(self,args,DS_MODE);
    }

    PYINVOKESPSS_API PyObject * ext_GetDataFileAttributes( PyObject *self,
                                                           PyObject *args)
    {
        return GetDataFileAttributesAll_XD(self,args,DS_MODE);
    }

    PYINVOKESPSS_API PyObject * ext_GetDataFileAttributeNamesInProcDS( PyObject *self,
                                                                       PyObject *args)
    {
        return GetDataFileAttributeNamesAll_XD(self,args,PROCDS_MODE);
    }

    PYINVOKESPSS_API PyObject * ext_GetDataFileAttributesInProcDS( PyObject *self,
                                                                   PyObject *args)
    {
        return GetDataFileAttributesAll_XD(self,args,PROCDS_MODE);
    }

    PYINVOKESPSS_API PyObject * ext_GetMultiResponseSetNames(PyObject *self,
                                                             PyObject *args)
    {
        return GetMultiResponseSetNamesAll_XD(self,args,DS_MODE);
    }

    PYINVOKESPSS_API PyObject * ext_GetMultiResponseSet(PyObject *self,
                                                        PyObject *args)
    {
        return GetMultiResponseSetAll_XD(self,args,DS_MODE);
    }

    PYINVOKESPSS_API PyObject * ext_GetMultiResponseSetNamesInProcDS(PyObject *self,
                                                                     PyObject *args)
    {
        return GetMultiResponseSetNamesAll_XD(self,args,PROCDS_MODE);
    }

    PYINVOKESPSS_API PyObject * ext_GetMultiResponseSetInProcDS(PyObject *self,
                                                                PyObject *args)
    {
        return GetMultiResponseSetAll_XD(self,args,PROCDS_MODE);
    }

    PYINVOKESPSS_API PyObject * ext_SetDataFileAttributesInDS(PyObject *self,
                                                              PyObject *args)
    {
        char * dsName, *attrName;
        char **attributes;
        int length;
        int errLevel;
        int size;
        PyObject *attr;
        if(!PyArg_ParseTuple(args,"ssOi",&dsName,&attrName,&attr,&length)) {
            return NULL;
        }

        errLevel = parse2CharStarStar(attr,&attributes, &size);
        if(errLevel != 0) {
            return Py_BuildValue("i",errLevel);
        }

        errLevel = SetDataFileAttributesInDS(dsName,attrName,attributes,length);
        for(int i = 0; i < size; i++) {
            PyMem_Free(attributes[i]);
        }
        PyMem_Free(attributes);

        return Py_BuildValue("i",errLevel);
    }

    PYINVOKESPSS_API PyObject * ext_SetMultiResponseSetInDS(PyObject *self,
                                                            PyObject *args)
    {
        int errLevel = 0;
        char *datasetName,*mrsetName,*mrsetLabel,*mrsetCountedValue;
        char **elemVarNames;
        int mrsetCodeAs,numOfVars,size;
        PyObject *varNames;
        if(!PyArg_ParseTuple(args,"sssisOi",&datasetName,&mrsetName,&mrsetLabel,&mrsetCodeAs,&mrsetCountedValue,&varNames,&numOfVars)) {
            return NULL;
        }

        errLevel = parse2CharStarStar(varNames,&elemVarNames, &size);
        if(errLevel != 0) {
            return Py_BuildValue("i",errLevel);
        }

        SetMultiResponseSetInDS(datasetName,mrsetName,mrsetLabel,mrsetCodeAs,mrsetCountedValue,(const char**)elemVarNames,numOfVars,errLevel);

        for(int i = 0; i < size; i++) {
            PyMem_Free(elemVarNames[i]);
        }
        PyMem_Free(elemVarNames);

        return Py_BuildValue("i",errLevel);
    }

    PYINVOKESPSS_API PyObject * ext_GetDataFileAttributeNamesInDS(PyObject *self,
                                                                  PyObject *args)
    {
        int error = 0;
        char *dsName;
        char **names = NULL;
        int numOfNames;

        if (!PyArg_ParseTuple(args, "s", &dsName))
            return NULL;

        error = GetDataFileAttributeNamesInDS(dsName, &names, &numOfNames);


        PyObject *pyNames = NULL;

        if(0 == error) {
            pyNames = PyTuple_New(numOfNames);
            for(int i = 0; i < numOfNames; i++) {
                if(IsUTF8mode()) {
                    PyObject* pyValue = PyUnicode_Decode(names[i],strlen(names[i]),"utf-8",NULL);
                    PyTuple_SetItem(pyNames,i,Py_BuildValue("O",pyValue));
                    Py_XDECREF(pyValue);
                } else {
                    PyTuple_SetItem(pyNames,i,Py_BuildValue("s",names[i]));
                }
            }

            FreeAttributeNames(names,numOfNames);
            
            PyObject* r = Py_BuildValue("Oi",pyNames,error);
            Py_DECREF(pyNames);

            return r;
        } else {
            return Py_BuildValue("Oi",Py_None,error);
        }
    }

    PYINVOKESPSS_API PyObject * ext_GetDataFileAttributesInDS(PyObject *self,
                                                              PyObject *args)
    {
        int error = 0;
        char *dsName;
        char *attrName = NULL;
        char **attrValues = NULL;
        int numOfValues;

        if(!PyArg_ParseTuple(args,"ss",&dsName, &attrName)) {
            return NULL;
        }

        error = GetDataFileAttributesInDS(dsName,attrName,&attrValues,&numOfValues);

        PyObject *pyValues = NULL;

        if(0 == error) {
            pyValues = PyTuple_New(numOfValues);
            for(int i = 0; i < numOfValues; i++) {
                if(IsUTF8mode()) {
                    PyObject* pyValue = PyUnicode_Decode(attrValues[i],strlen(attrValues[i]),"utf-8",NULL);
                    PyTuple_SetItem(pyValues,i,Py_BuildValue("O",pyValue));
                    Py_XDECREF(pyValue);
                } else {
                    PyTuple_SetItem(pyValues,i,Py_BuildValue("s",attrValues[i]));
                }
            }

            FreeAttributes(attrValues,numOfValues);

            return Py_BuildValue("Oi",pyValues,error);
        } else {
            return Py_BuildValue("Oi",Py_None,error);
        }
    }

    PYINVOKESPSS_API PyObject * ext_GetMultiResponseSetNamesInDS(PyObject *self,
                                                                 PyObject *args)
    {
        char* handle = 0;
        char* context = 0;
        char* expression = 0;
        int errLevel = 0;
        void *result;
        char* dsName;

        if (!PyArg_ParseTuple(args, "s", &dsName))
            return NULL;

        result = GetMultiResponseSetNamesInDS(dsName, errLevel);

        int size = GetStringListLength(result);

        if ( 0 != errLevel || 0 == size || 0 == result){
            return Py_BuildValue("[i]",errLevel);
        }

        //pass the result to python object
        PyObject* outList = PyList_New(size);
        for( int i = 0; i < size; i++){
            PyObject* item;
            const char *str = GetStringFromList(result,i);
            if(IsUTF8mode()) {
                PyObject* pyValue = PyUnicode_Decode(str,strlen(str),"utf-8",NULL);
                item = Py_BuildValue("O", pyValue);
                Py_XDECREF(pyValue);
            } else {
                item = Py_BuildValue("s",str);
            }
            PyList_SET_ITEM(outList,i,item);
        }
        RemoveStringList(result);
        
        PyObject* r = Py_BuildValue("Oi",outList,errLevel);
        Py_DECREF(outList);

        return r;
    }

    PYINVOKESPSS_API PyObject * ext_GetMultiResponseSetInDS(PyObject *self,
                                                            PyObject *args)
    {
        int errLevel = 0;
        char *dsName;
        char *mrsetName = NULL;
        char *mrsetLabel, *mrsetCountedValue;
        void *elemVarNames;
        int mrsetCodeAs,mrsetDataType;

        if(!PyArg_ParseTuple(args,"ss",&dsName, &mrsetName)) {
            return NULL;
        }

        GetMultiResponseSetInDS(dsName,mrsetName,&mrsetLabel,mrsetCodeAs,&mrsetCountedValue,mrsetDataType,&elemVarNames,errLevel);

        PyObject *pyValues = NULL;

        if(0 == errLevel) {
            int size = GetStringListLength(elemVarNames);
            pyValues = PyList_New(size);

            for(int i = 0; i < size; i++) {
                const char *str = GetStringFromList(elemVarNames,i);
                if(IsUTF8mode()) {
                    PyObject* pyValue = PyUnicode_Decode(str,strlen(str),"utf-8",NULL);
                    PyList_SetItem(pyValues,i,Py_BuildValue("O",pyValue));
                    Py_XDECREF(pyValue);
                } else {
                    PyList_SetItem(pyValues,i,Py_BuildValue("s",str));
                }
            }

            char *codeAs = 0, *dataType = 0;
            if (mrsetCodeAs == 1)
                codeAs = "Categories";
            else if (mrsetCodeAs == 2)
                codeAs = "Dichotomies";
            else
                codeAs = "Unknown";
            if (mrsetDataType == 0)
                dataType = "Numeric";
            else
                dataType = "String";
            PyObject *ret;
            if(IsUTF8mode()) {
                PyObject* pyValueOne = PyUnicode_Decode(mrsetLabel,strlen(mrsetLabel),"utf-8",NULL);
                PyObject* pyValueTwo = PyUnicode_Decode(codeAs,strlen(codeAs),"utf-8",NULL);
                PyObject* pyValueThree = PyUnicode_Decode(mrsetCountedValue,strlen(mrsetCountedValue),"utf-8",NULL);
                PyObject* pyValueFour = PyUnicode_Decode(dataType,strlen(dataType),"utf-8",NULL);
                ret = Py_BuildValue("OOOOOi",pyValueOne,pyValueTwo,pyValueThree,pyValueFour,pyValues,errLevel);
                Py_XDECREF(pyValueOne);
                Py_XDECREF(pyValueTwo);
                Py_XDECREF(pyValueThree);
                Py_XDECREF(pyValueFour);
            } else {
               ret = Py_BuildValue("ssssOi",mrsetLabel,codeAs,mrsetCountedValue,dataType,pyValues,errLevel);
            }

            FreeString(mrsetLabel);
            FreeString(mrsetCountedValue);
            RemoveStringList(elemVarNames);
            if ( pyValues )
                Py_DECREF(pyValues);

            return ret;

        } else {
            return Py_BuildValue("[i]",errLevel);
        }
    }

    PYINVOKESPSS_API PyObject * ext_GetVarColumnWidthInDS(
                                                      PyObject *self,
                                                      PyObject *args)
    {
        char* dsName;
        int index;
        int errLevel;
        if (!PyArg_ParseTuple(args, "si", &dsName, &index))
            return NULL;
        int width = GetVarColumnWidthInDS(dsName, index, errLevel);
        return Py_BuildValue("ii", width, errLevel);
    }

    PYINVOKESPSS_API PyObject * ext_SetVarColumnWidthInDS(
                                                  PyObject *self,
                                                  PyObject *args)
    {
        char* dsName;
        int index;
        int width;
        if (!PyArg_ParseTuple(args, "sii", &dsName, &index, &width))
            return NULL;
        return Py_BuildValue("i", SetVarColumnWidthInDS(dsName, index, width));
    }

    PYINVOKESPSS_API PyObject * ext_SaveFileInDS(
                                                  PyObject *self,
                                                  PyObject *args)
    {
        char* dsName;
        char* savePath;
        bool compressed;
        if (!PyArg_ParseTuple(args, "ssb", &dsName, &savePath, &compressed))
            return NULL;
        return Py_BuildValue("i", SaveFileInDS(dsName, savePath, compressed));
    }

    PYINVOKESPSS_API PyObject * ext_DelDataFileAttributesInDS(
                                                  PyObject *self,
                                                  PyObject *args)
    {
        char* dsName;
        char* attrName;
        if (!PyArg_ParseTuple(args, "ss", &dsName, &attrName))
            return NULL;
        return Py_BuildValue("i", DelDataFileAttributesInDS(dsName, attrName));
    }

    PYINVOKESPSS_API PyObject * ext_DelMultiResponseSetInDS(
                                                  PyObject *self,
                                                  PyObject *args)
    {
        char* dsName;
        char* mrsetName;
        if (!PyArg_ParseTuple(args, "ss", &dsName, &mrsetName))
            return NULL;
        return Py_BuildValue("i", DelMultiResponseSetInDS(dsName, mrsetName));
    }

    PYINVOKESPSS_API PyObject * ext_GetXDriveMode(PyObject *self,
                                                  PyObject *args)
    {
        int errLevel = 0;
        int curMode,originMode;
        errLevel = GetXDriveMode(curMode,originMode);

        return Py_BuildValue("iii", curMode,originMode,errLevel);
    }

    PyObject *
    ext_GetSPSSLocale(PyObject *self, PyObject *args)
    {
        int errLevel = 0;
        const char *locale = GetSPSSLocale(errLevel);

        if ( 0 == errLevel && GetCLocale){
            //reset the locale to xdlocale.
            char *xdlocale = 0;
            errLevel = GetCLocale(&xdlocale);
            if ( xdlocale ){
                setlocale(LC_ALL, xdlocale);
            }
            FreeString( xdlocale );
        }

        if(IsUTF8mode()&&(errLevel==0))
        {
            PyObject* pyValue = PyUnicode_Decode(locale,strlen(locale),"utf-8",NULL);
            PyObject* r = Py_BuildValue("Oi", pyValue,errLevel);
            Py_XDECREF(pyValue);
            return r;
        }
        else
        {
            return Py_BuildValue("si", locale,errLevel);
        }
    }
    PyObject *ext_GetCLocale(PyObject *self, PyObject *args)
    {
        int errLevel = 0;
        char *xdlocale = 0;
        errLevel = GetCLocale(&xdlocale);
        PyObject *result = Py_BuildValue("si", xdlocale,errLevel);
        FreeString( xdlocale );
        return result;        
    }

    PyObject *
    ext_SetOutputLanguage(PyObject *self, PyObject *args)
    {
        int errLevel = 0;
        char* language;
        if (!PyArg_ParseTuple(args, "s", &language))
            return NULL;

        SetOutputLanguage(language, errLevel);
        return Py_BuildValue("i", errLevel);
    }
    
    PyObject *
    ext_GetSystemMissingValue(PyObject *self, PyObject *args)
    {
        int errLevel = 0;
        double sysMissingValue;
        sysMissingValue = -DBL_MAX;
        return Py_BuildValue("d", sysMissingValue);
    }

    PyObject * 
    ext_GetNestDepth(PyObject *self, PyObject *args)
    {
        int errLevel = 0;
        int depth;
        errLevel = GetNestDepth(depth);

        return Py_BuildValue("ii", depth, errLevel);
    }
                                                        
    PyObject *
    ext_GetFileHandles(PyObject *self, PyObject *args)
    {
        void* result;
        int errLevel;
        result = GetFileHandles(errLevel);
        int size = GetStringListLength(result);
    
        //if get handle list failure, or handle list is empty
        if ( 0 != errLevel || 0 == size || 0 == result)
          return Py_BuildValue("[i]",errLevel);
    
        //else, pass the handle list to python object
        PyObject* outList = PyList_New(size+1);
        for( int i=0; i<size; i++) {
            const char * str = GetStringFromList(result,i);
            PyObject* item = NULL;
            if(IsUTF8mode())
            {
                PyObject* pyValue = PyUnicode_Decode(str,strlen(str),"utf-8",NULL);
                item = Py_BuildValue("O",pyValue);
                Py_XDECREF(pyValue);
            }
            else
            {
                item = Py_BuildValue("s",str);
            }
            PyList_SET_ITEM(outList,i,item);
        }
        RemoveStringList(result);
    
        PyObject* errCode = Py_BuildValue("i",errLevel);
        PyList_SET_ITEM(outList,size,errCode);
    
        return outList;
    }
    
    PyObject * 
    ext_GetOutputLanguage(PyObject *self, PyObject *args)
    {
        int errLevel = 0;
        const char *olang = GetOutputLanguage(errLevel);

        return Py_BuildValue("si", olang, errLevel);
    }
    
    PyObject * 
    ext_TransCode(PyObject *self, PyObject *args)
    {
        int errLevel = 0;
        char* orig;
        if (!PyArg_ParseTuple(args, "s", &orig)) {
            return NULL;
        }
        
        const char* dest = TransCode(orig, errLevel);
        
        return Py_BuildValue("si", dest, errLevel);
    }

    PYINVOKESPSS_API PyObject * ext_SetDefaultEncoding(
                                                    PyObject *self,
                                                    PyObject *args)
    {
        int errLevel = 0;
        char* PyDefaultEncoding;
        if (!PyArg_ParseTuple(args, "s", &PyDefaultEncoding)) {
            return NULL;
        }
        errLevel = PyUnicode_SetDefaultEncoding(PyDefaultEncoding);

        return Py_BuildValue("i", errLevel);
    }
    
    PyObject *
    GetVariableRoleAll_XD(PyObject *self, PyObject *args,const int mode)
    {
        int index;
        int role;
        if (!PyArg_ParseTuple(args, "i", &index)) {
            return NULL;
        }

        int errLevel;

        if(mode == DS_MODE) {
            role = GetVariableRole(index,errLevel);
        } else {
            role = GetVariableRoleInProcDS(index,errLevel);
        }

        return Py_BuildValue("ii", role,errLevel);
    }
    
    PYINVOKESPSS_API PyObject *
    ext_GetVariableRole(PyObject *self, PyObject *args)
    {
        return GetVariableRoleAll_XD(self,args,DS_MODE);
    }
    
    PYINVOKESPSS_API PyObject *
    ext_GetVariableRoleInProcDS( PyObject *self, PyObject *args)
    {
        return GetVariableRoleAll_XD(self,args,PROCDS_MODE);
    }
    
    PYINVOKESPSS_API PyObject * ext_SetVarRole(PyObject *self, PyObject *args)
    {
        int error = 0;
        char* varName;
        int varRole;

        if(!PyArg_ParseTuple(args,"si",&varName,&varRole)) {
            return NULL;
        }

        error = SetVarRole(varName, varRole);

        return Py_BuildValue("i",error);
    }
    
    PYINVOKESPSS_API PyObject *
    ext_SetVarRoleInDS(PyObject *self, PyObject *args)
    {
        char* dsName;
        int index;
        int varRole;
        if (!PyArg_ParseTuple(args, "sii", &dsName, &index, &varRole))
            return NULL;
        return Py_BuildValue("i", SetVarRoleInDS(dsName, index, varRole));
    }
    
    PYINVOKESPSS_API PyObject *
    ext_GetVarRoleInDS(PyObject *self, PyObject *args)
    {
        char* dsName;
        int index;
        int errLevel;
        if (!PyArg_ParseTuple(args, "si", &dsName, &index))
            return NULL;
        int varRole = GetVarRoleInDS(dsName, index, errLevel);
        return Py_BuildValue("ii", varRole, errLevel);
    }

    PyMODINIT_FUNC
    initPyInvokeSpss(void)
    {
        Py_InitModule("PyInvokeSpss", PyInvokeSpss_methods);
        LoadLib();
        SPSS_ResultObject_Type.ob_type = &PyType_Type;
        SPSS_ResultObject_Type.tp_alloc = PyType_GenericAlloc;
        SPSS_ResultObject_Type.tp_new = PyType_GenericNew;
        SPSS_ResultObject_Type.tp_free = _PyObject_GC_Del;
    }

    PyObject *
    ext_TransportData(PyObject *self, PyObject *args)
    {
        char* varList;
        char* portNum;
        bool needSleep;
        int errLevel;
        if (!PyArg_ParseTuple(args, "ssb", &portNum, &varList, &needSleep)) {
            return NULL;
        }

        errLevel = TransportData(portNum, varList, needSleep);
        PyObject* out = Py_BuildValue("i",errLevel);
        return out;
    }
    
    PyObject *
    ext_GetDataFromTempFile(PyObject *self, PyObject *args)
    {
        char* filename;
        int newVarCount;
        int caseLen;
        int errLevel;
        if (!PyArg_ParseTuple(args, "sii", &filename, &newVarCount, &caseLen)) {
            return NULL;
        }
        errLevel = GetDataFromTempFile(filename, newVarCount, caseLen);
        PyObject* out = Py_BuildValue("i",errLevel);
        return out;
    }
    
    PyObject *
    ext_SaveDataToTempFile(PyObject *self, PyObject *args)
    {
        char* filename;
        PyObject *varTuple = NULL;
        int errLevel = 0;
        
        if (!PyArg_ParseTuple(args, "sO", &filename, &varTuple)) {
            return NULL;
        }
        int len = 0;
        if(PyTuple_Check(varTuple)) {

            len = PyTuple_Size(varTuple);
            int* varIndexes = new int[len];
            int index = 0;
            for (int i=0; i<len; i++)
            {
                PyObject *v = PyTuple_GetItem(varTuple, i);
                if(!PyArg_Parse(v,"i",&index)) {
                        errLevel = ERROR_PYTHON;
                }
                varIndexes[i] = index;
            }
            errLevel = SaveDataToTempFile(filename, varIndexes, len);
            delete []varIndexes;
        }
        return Py_BuildValue("i", errLevel);
    }
    
    PyObject *
    ext_GetSplitEndIndex(PyObject *self, PyObject *args)
    {
        PyObject *indexTuple = NULL;
        int errLevel;
        int size = 0;
        int *splitEndIndexes = 0;
     
        splitEndIndexes = GetSplitEndIndex(size, errLevel);
        indexTuple = PyTuple_New(size);
        if (size > 0) {
            for (int i=0; i<size; i++)
            {
                PyTuple_SET_ITEM(indexTuple,i,Py_BuildValue("i", splitEndIndexes[i]));
            }
            PyObject* r = Py_BuildValue("Oii", indexTuple, size, errLevel);
            Py_DECREF(indexTuple);
            return r;
        } else {
            return Py_BuildValue("Oii",Py_None,size,errLevel);
        }
    }
    
    PyObject*
    ext_SetMode(PyObject* self, PyObject* args)
    {
        int error = 0;
        int mode;
        if (!PyArg_ParseTuple(args, "i", &mode)) {
            return NULL;
        }

        error = SetMode(mode);

        return Py_BuildValue("i",error);
    }
    
    PyObject*
    ext_GetRowCountInTempFile(PyObject* self, PyObject* args)
    {
        int errLevel = 0;
        long result = 0;
        result = GetRowCountInTempFile(errLevel);
        return Py_BuildValue("li", result, errLevel);
    }
    
    PYINVOKESPSS_API PyObject * ext_GetCaseValue(PyObject *self, PyObject *args)
    {
        char* dsName;
        int rowIndex;
        bool isCache;
        int isMissing;
        int errLevel = 0;
        if (!PyArg_ParseTuple(args, "sib", &dsName, &rowIndex, &isCache))
            return NULL;
            
        int* varType = 0;
        int size = 0;
        
        GetVarInfo(dsName, &varType, size);
        PyObject* result = NULL;
        if (size>0)
        {
            int len = 0;
            GetCaseValue(dsName, rowIndex, isCache, len, errLevel);
            if (0 == errLevel){
                PyObject *pyValueList = NULL;
                pyValueList = PyList_New(size);
                for (int i=0; i<size; i++){
                    if(0 == varType[i]){
                        double value = GetNCellValueFromCache(dsName, i, isMissing, errLevel);
                        if(isMissing){
                            Py_INCREF(Py_None);
                            PyList_SetItem(pyValueList, i, Py_None);
                        }
                        else{
                            PyList_SetItem(pyValueList, i, Py_BuildValue("d", value));
                        }
                    }
                    else{
                        const char* value = GetCCellValueFromCache(dsName, i, errLevel);
                        if (IsUTF8mode()&&(0==errLevel)){
                            PyObject* pyValue = PyUnicode_Decode(value,strlen(value),"utf-8",NULL);
                            PyList_SetItem(pyValueList, i, Py_BuildValue("O", pyValue));
                            Py_XDECREF(pyValue);
                        }
                        else{
                            PyList_SetItem(pyValueList, i, Py_BuildValue("s", value));
                        }
                        FreeString((char*)value);
                    }
                }
            
                delete []varType;
                varType = 0;
                
                result = Py_BuildValue("Oi", pyValueList, errLevel);
                Py_DECREF(pyValueList);
            }
        }
        else
        {
            result = Py_BuildValue("Oi", Py_None, errLevel);
        }
        return result;
    }
    
    PYINVOKESPSS_API PyObject * ext_SetCaseValue(PyObject *self, PyObject *args)
    {
        char* dsName;
        int rowIndex;
        PyObject *pyCaseData;
        bool isCache;
        int errLevel = 0;
        if (!PyArg_ParseTuple(args, "siOb", &dsName, &rowIndex, &pyCaseData, &isCache)){
            return NULL;
        }
        int* varType = 0;
        int size = 0;
        int varLen = 0;
        varLen = PyDict_Size(pyCaseData);
        
        if (varLen>0)
        {
            GetVarInfo(dsName, &varType, size);
            if (size>0)
            {
                PyObject *keys = PyDict_Keys(pyCaseData);
                PyObject *values = PyDict_Values(pyCaseData);
                
                int *columnList = new int[varLen];
                
                for (int i=0; i<varLen; i++){
                    PyObject *index = PyList_GetItem(keys, i);
                    int var;
                    PyArg_Parse(index,"i",&var);
                    columnList[i] = var;
                    PyObject *value = PyList_GetItem(values, i);
                    if(0 == varType[var]){
                        double d;
                        if(PyFloat_Check(value))
                        {
                            d = PyFloat_AsDouble(value);
                        }
                        else
                        {
                            if(PyInt_Check(value))
                            {
                                d = (double)PyInt_AS_LONG(value);
                            }
                            else 
                            {
                                if (value == Py_None)
                                {
                                    d = -DBL_MAX;
                                }
                                else
                                {
                                    return Py_BuildValue("i",57);
                                }
                            }
                        }
                        errLevel = SetNCellValueFromCache(dsName, var, d);
                    }                                
                    else{
                        char *c;
                        if(!PyArg_Parse(value,"s",&c)) 
                        {
                            return Py_BuildValue("i",56);
                        }
                        errLevel = SetCCellValueFromCache(dsName, var, c);
                    }
                }
                
                errLevel = SetCasePartValue(dsName, rowIndex, columnList, isCache, varLen);
                delete []columnList;
                columnList = 0;
                Py_DECREF(keys);
                Py_DECREF(values);
            }
            delete []varType;
        }
        
        return Py_BuildValue("i",errLevel);
    }
    
    PYINVOKESPSS_API PyObject * ext_SetCacheInDS(PyObject *self, PyObject *args)
    {
        char* dsName;
        bool isCache;
        int errLevel = 0;
        if (!PyArg_ParseTuple(args, "sb", &dsName, &isCache)){
            return NULL;
        }
        errLevel = SetCacheInDS(dsName, isCache);
        return Py_BuildValue("i",errLevel);
    }
    
    PYINVOKESPSS_API PyObject * ext_IsDistributedMode(PyObject *self, PyObject *args)
    {
        int result;

        if(NULL == IsDistributedMode) {
          result = PY_FALSE;
        } else {
          result = IsDistributedMode();
        }
        return Py_BuildValue("i", result);
    }
    
    PYINVOKESPSS_API PyObject * ext_IsUseOrFilter(PyObject *self, PyObject *args)
    {
        int error = 0;
        int isUse;

        error = IsUseOrFilter(isUse);

        return Py_BuildValue("ii",isUse,error);
    }
    
    PYINVOKESPSS_API PyObject * ext_MoveVariables(PyObject *self, PyObject *args)
    {
        char* dsName;
        int targetIndex;
        PyObject *varTuple = NULL;
        int errLevel = 0;
        
        if (!PyArg_ParseTuple(args, "sOi", &dsName, &varTuple, &targetIndex)) {
            return NULL;
        }
        int len = 0;
        if(PyTuple_Check(varTuple)) {

            len = PyTuple_Size(varTuple);
            int* varIndexes = new int[len];
            int index = 0;
            for (int i=0; i<len; i++)
            {
                PyObject *v = PyTuple_GetItem(varTuple, i);
                if(!PyArg_Parse(v,"i",&index)) {
                        errLevel = ERROR_PYTHON;
                }
                varIndexes[i] = index;
            }
            errLevel = MoveVariables(dsName, varIndexes, len, targetIndex);
            delete []varIndexes;
        }
        return Py_BuildValue("i", errLevel);
    }
    
    PYINVOKESPSS_API PyObject * ext_CreateSocket(PyObject *self, PyObject *args)
    {
        int error = 0;
        int port;
        
        error = CreateSocket(port);
        
        return Py_BuildValue("ii", port, error);
    }
    
    PYINVOKESPSS_API PyObject * ext_AcceptSocket(PyObject *self, PyObject *args)
    {
        int error = 0;
        
        error = AcceptSocket();
        
        return Py_BuildValue("i", error);
    }
    
    PYINVOKESPSS_API PyObject * ext_GetTrendsInfoInDS(PyObject *self, PyObject *args)
    {
        char* dsName;
        int errLevel = 0;
        int size = 0;
        int *TInfo = 0;
        PyObject *trendsInfoTuple = NULL;
        
        if (!PyArg_ParseTuple(args, "s", &dsName)) {
            return NULL;
        }
     
        TInfo = GetTrendsInfoInDS(dsName, size, errLevel);
        trendsInfoTuple = PyTuple_New(size);
        if (size > 0) {
            for (int i=0; i<size; i++)
            {
                PyTuple_SET_ITEM(trendsInfoTuple,i,Py_BuildValue("i", TInfo[i]));
            }
            PyObject* r = Py_BuildValue("Oi", trendsInfoTuple, errLevel);
            Py_DECREF(trendsInfoTuple);
            return r;
        } else {
            return Py_BuildValue("Oi",Py_None,errLevel);
        }
    }

    PYINVOKESPSS_API PyObject * ext_GetSelectedRowCountInProcDS(PyObject *self, PyObject *args)
    {
        int errLevel = 0;
        long result = -1;
        result = GetSelectedRowCountInProcDS(errLevel);
        return Py_BuildValue("li", result, errLevel);
    }
}
