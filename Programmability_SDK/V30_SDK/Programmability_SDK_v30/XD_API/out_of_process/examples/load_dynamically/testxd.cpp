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
* testxd.cpp -
*     test start and stop IBM SPSS Statistics iteratively. spssxd library will be load and unload
*     dynamically.
*
* COPYRIGHT
*     Copyright 2005 by SPSS Inc.  All rights reserved.
*
*/

#include <string>
#include <vector>
#include <iostream>
#include <cassert>
#include "testxd.h"
#include "wchar.h"

#ifdef MS_WINDOWS
#include <windows.h>
#include <stdio.h>
#include<fstream>
#else
#include <dlfcn.h>
#endif

#ifdef MS_WINDOWS
#define LIBHANDLE        HMODULE
#define GETADDRESS       GetProcAddress
#define LIBNAME          "spssxd_p.dll"
#else
	#define LIBHANDLE        void*
	#define GETADDRESS       dlsym
	#ifdef HPUX
	#define LIBNAME			 "libspssxd_p.sl"
	#else
		#ifdef DARWIN
		#define LIBNAME			 "libspssxd_p.dylib"
		#else
		#define LIBNAME          "libspssxd_p.so"
		#endif
	#endif
#endif

LIBHANDLE pLib = NULL;
const char libName[] = LIBNAME;
const int LOAD_FAIL = 1011;  
const int LOAD_SUCCESS = 0;


//declare the function pointer.
static FP_IsBackendReady                    IsBackendReady =               NULL;
static FP_IsXDriven                         IsXDriven =                    NULL;
static FP_StartSpss                         StartSpss =                    NULL;
static FP_StopSpss                          StopSpss =                     NULL;
static FP_Submit                            Submit =                       NULL;
static FP_QueueCommandPart                  QueueCommandPart =             NULL;
static FP_PostSpssOutput                    PostSpssOutput =               NULL;
static FP_GetVariableCount                  GetVariableCount =             NULL;
static FP_GetRowCount                       GetRowCount =                  NULL;
static FP_GetVariableName                   GetVariableName =              NULL;
static FP_GetVariableLabel                  GetVariableLabel =             NULL;
static FP_GetVariableType                   GetVariableType =              NULL;
static FP_GetVariableFormat                 GetVariableFormat =            NULL;
static FP_GetVariableMeasurementLevel       GetVariableMeasurementLevel =  NULL;
static FP_CreateXPathDictionary             CreateXPathDictionary =        NULL;
static FP_RemoveXPathHandle                 RemoveXPathHandle =            NULL;
static FP_EvaluateXPath                     EvaluateXPath =                NULL;
static FP_GetStringListLength               GetStringListLength =          NULL;
static FP_GetStringFromList                 GetStringFromList =            NULL;
static FP_RemoveStringList                  RemoveStringList =             NULL;
static FP_GetXmlUtf16                       GetXmlUtf16 =                  NULL;
static FP_GetHandleList                     GetHandleList =                NULL;
static FP_GetNumericValue                   GetNumericValue =              NULL;
static FP_GetStringValue                    GetStringValue =               NULL;
static FP_NextCase                          NextCase =                     NULL;
static FP_RemoveCaseCursor                  RemoveCaseCursor =             NULL;
static FP_GetVariableFormatType             GetVariableFormatType =        NULL;
static FP_GetCursorPosition                 GetCursorPosition =            NULL;
static FP_MakeCaseCursor                    MakeCaseCursor =               NULL;
static FP_StartPivotTable                 StartPivotTable = NULL;                                                  
static FP_AddDimension                    AddDimension = NULL;                                                  
static FP_AddStringCategory               AddStringCategory = NULL;                                                  
static FP_SetFormatSpecCount              SetFormatSpecCount = NULL;                                                  
static FP_SetNumberCell                   SetNumberCell = NULL;                                                  
static FP_AddCellFootnotes                AddCellFootnotes = NULL;                                                  
static FP_AddTextBlock                    AddTextBlock = NULL;                                                  
static FP_StartProcedure                  StartProcedure = NULL;                                                  
static FP_EndProcedure                    EndProcedure = NULL;     

//data step
static FP_StartDataStep						StartDataStep =					NULL;                                             
static FP_CreateDataset						CreateDataset =					NULL;
static FP_GetCaseCountInDS					GetCaseCountInDS =				NULL;
static FP_InsertCase						InsertCase =					NULL;
static FP_DeleteCase						DeleteCase =					NULL;
static FP_GetVarTypeInDS					GetVarTypeInDS =				NULL;
static FP_GetNCellValue						GetNCellValue =					NULL;
static FP_GetCCellValue						GetCCellValue =					NULL;
static FP_SetNCellValue						SetNCellValue =					NULL;
static FP_SetCCellValue						SetCCellValue =					NULL;
static FP_EndDataStep						EndDataStep =					NULL;
//end data step

//Initialize the function pointer  
void InitializeFP() 
{
    IsBackendReady = (FP_IsBackendReady)GETADDRESS(pLib,"IsBackendReady");
    IsXDriven = (FP_IsXDriven)GETADDRESS(pLib,"IsXDriven");
    StartSpss = (FP_StartSpss)GETADDRESS(pLib,"StartSpss");
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
    GetHandleList = (FP_GetHandleList)GETADDRESS(pLib,"GetHandleList");

    GetNumericValue = (FP_GetNumericValue)GETADDRESS(pLib,"GetNumericValue");
    GetStringValue = (FP_GetStringValue)GETADDRESS(pLib,"GetStringValue");
    NextCase = (FP_NextCase)GETADDRESS(pLib,"NextCase");
    RemoveCaseCursor = (FP_RemoveCaseCursor)GETADDRESS(pLib,"RemoveCaseCursor");  
    GetVariableFormatType = (FP_GetVariableFormatType)GETADDRESS(pLib,"GetVariableFormatType"); 
    GetCursorPosition = (FP_GetCursorPosition)GETADDRESS(pLib,"GetCursorPosition"); 
    MakeCaseCursor = (FP_MakeCaseCursor)GETADDRESS(pLib,"MakeCaseCursor");

    StartPivotTable = (FP_StartPivotTable)GETADDRESS(pLib,"StartPivotTable");
    AddDimension = (FP_AddDimension)GETADDRESS(pLib,"AddDimension");
    AddStringCategory = (FP_AddStringCategory)GETADDRESS(pLib,"AddStringCategory");
    SetFormatSpecCount = (FP_SetFormatSpecCount)GETADDRESS(pLib,"SetFormatSpecCount");
    SetNumberCell = (FP_SetNumberCell)GETADDRESS(pLib,"SetNumberCell");
    AddCellFootnotes = (FP_AddCellFootnotes)GETADDRESS(pLib,"AddCellFootnotes");
    AddTextBlock = (FP_AddTextBlock)GETADDRESS(pLib,"AddTextBlock");
    StartProcedure = (FP_StartProcedure)GETADDRESS(pLib,"StartProcedure");
    EndProcedure = (FP_EndProcedure)GETADDRESS(pLib,"EndProcedure");
	
	//data step
	StartDataStep =	(FP_StartDataStep)GETADDRESS(pLib,"StartDataStep");
	CreateDataset =	(FP_CreateDataset)GETADDRESS(pLib,"CreateDataset"); 
	GetCaseCountInDS = (FP_GetCaseCountInDS)GETADDRESS(pLib,"GetCaseCountInDS"); 
	InsertCase = (FP_InsertCase)GETADDRESS(pLib,"InsertCase"); 
	DeleteCase = (FP_DeleteCase)GETADDRESS(pLib,"DeleteCase"); 
	GetVarTypeInDS = (FP_GetVarTypeInDS)GETADDRESS(pLib,"GetVarTypeInDS"); 
	GetNCellValue =	(FP_GetNCellValue)GETADDRESS(pLib,"GetNCellValue"); 
	GetCCellValue =	(FP_GetCCellValue)GETADDRESS(pLib,"GetCCellValue"); 
	SetNCellValue =	(FP_SetNCellValue)GETADDRESS(pLib,"SetNCellValue"); 
	SetCCellValue =	(FP_SetCCellValue)GETADDRESS(pLib,"SetCCellValue"); 
	EndDataStep = (FP_EndDataStep)GETADDRESS(pLib,"EndDataStep"); 
	//end data step
}

int LoadLib()
{

    if(NULL == pLib){
#ifdef MS_WINDOWS
        //find out spssxd module, it will success when spss drive
        pLib = GetModuleHandle(libName);
        //find out spssxd module failure, load it.
        if(NULL == pLib)
            pLib = LoadLibrary(libName);
#else
        pLib = dlopen(libName,RTLD_NOW | RTLD_GLOBAL);

#endif
    }
    if (pLib) {
        InitializeFP();
    }
    else {//load failure
        return LOAD_FAIL;
    }
    return LOAD_SUCCESS;
}

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
    GetHandleList = NULL;
    GetNumericValue = NULL;
    GetStringValue = NULL;
    NextCase = NULL;
    RemoveCaseCursor = NULL;
    GetVariableFormatType = NULL;
    GetCursorPosition = NULL;
    MakeCaseCursor = NULL;
    StartPivotTable = NULL;                                                  
    AddDimension = NULL;                                                  
    AddStringCategory = NULL;                                                  
    SetFormatSpecCount = NULL;                                                  
    SetNumberCell = NULL;                                                  
    AddCellFootnotes = NULL;                                                  
    AddTextBlock = NULL;                                                  
    StartProcedure = NULL;                                                  
    EndProcedure = NULL;                                                  
}

//for data source.
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

//open data file with the given filename
int getfile(const char* filename)
{
    std::string cmd("get file=\'");
    cmd += filename;
    cmd += "\'.";
    return Submit(cmd.c_str(),(int)cmd.size());
}

// add your data file here. please give the full path of your data file.
void filelist(std::vector<std::string>& fList)
{
    // Correct the path to data files to point to the installation directory of IBM SPSS Statistics
    // For example, "c:\\Progra~1\\IBM\\SPSS\\Statistics\\samples\\" for windows
    // Or, "/usr/local/Statistics/samples/" for UNIX
    std::string filepath = "C:\\Program Files\\IBM\\SPSS\\Statistics\\Samples\\English\\";

    //create the full path of data file and push it into fList.
    fList.push_back(filepath+"demo.sav");
    fList.push_back(filepath+"anorectic.sav");
    fList.push_back(filepath+"carpet.sav");
    fList.push_back(filepath+"Employee data.sav");
    fList.push_back(filepath+"guttman.sav");
    fList.push_back(filepath+"judges.sav");
    fList.push_back(filepath+"kinship_dat.sav");
    fList.push_back(filepath+"kinship_ini.sav");
    fList.push_back(filepath+"kinship_var.sav");
    fList.push_back(filepath+"ozone.sav");
    fList.push_back(filepath+"screws.sav");
    fList.push_back(filepath+"tv-survey.sav");
    fList.push_back(filepath+"verd1985.sav");
    fList.push_back(filepath+"breakfast.sav");
    fList.push_back(filepath+"catalog.sav");
}

//test XPath and general functions
void XPathAndGeneralFunctions(const char* datafile)
{
    int err = 0;
	int x = 0;

    //test create xpath dictionary
    err = CreateXPathDictionary(datafile);
    assert( err == 0 );
    if (err !=0 ){
        std::cout << "create xpath error: handle=>" << datafile << " err=" << err << std::endl;
    }
    //test get variable count
    int varnum = GetVariableCount(err);
    //test get cases count
    int casenum = GetRowCount(err);

    for( x=0; x< varnum; x++){
        //test get variable name
        const char* name = GetVariableName(x,err);
        assert( err == 0 );
        //test get variable type
        int type = GetVariableType(x,err);
        assert( err == 0 );
        //test get variable label
        const char* label = GetVariableLabel(x,err);
        assert( err == 0 );
        //test get variable format
        const char* format = GetVariableFormat(x,err);
        assert( err == 0 );
        //test get variable measurement level
        int measure = GetVariableMeasurementLevel(x,err);
        assert( err == 0 );
    }

    //test evaluate xpath
    void* result = EvaluateXPath(datafile,"dictionary","variable/@name",err);
    assert( err == 0 );
    int size = GetStringListLength(result);
    assert( size > 0 );
    if(size==0 || result==0 || err != 0)
        std::cout << "evaluate xpath error: handle=>" << datafile << " err=" << err << std::endl;

    std::string namestr(" ");
    for (x=0; x<size; x++){
        namestr += GetStringFromList(result,x);
        namestr += " ";
    }
    std::cout << "namestr=>" << namestr << std::endl;

    //test delete handle
    err = RemoveXPathHandle(datafile);
    assert( err == 0 );
    if( err != 0 )
        std::cout << "delete handle error: handle=>" << datafile << " err=" << err << std::endl;

    //test Queue command part
    err = QueueCommandPart("list all.",9);
    assert( err == 0 );
    err = QueueCommandPart("disp dict.",10);
    assert( err == 0 );

    //test post IBM SPSS Statistics output
    err = PostSpssOutput("this is a test for xd api.",26);
    assert( err == 0 );

    //test submit
    std::string cmd("freq var=");
    cmd += namestr;
    cmd += ".";
    err = Submit(cmd.c_str(),(int)cmd.size());
    assert( 0 == err || 2 == err );
}

//test data source
void DataSourceFunctions()
{
    int err = 0;
    err = MakeCaseCursor("R");
    assert(err == 0);

    std::vector<SPSS_VariableInfo> varVector;

    int varCount = GetVariableCount(err);
    assert(err == 0);

    for(int j = 0; j <varCount; ++j) {
        SPSS_VariableInfo varInfo;
        varInfo.type = GetVariableType(j, err);
        assert(err == 0);

        err = GetVariableFormatType(j,varInfo.formatType,varInfo.width,varInfo.decimals);
        assert(err == 0);

        const char *name = GetVariableName(j,err);
        assert(err == 0);
        memset(varInfo.fName,'\0',257);
        strcpy(varInfo.fName,name);

        varInfo.mLevel = GetVariableMeasurementLevel(j,err);
        assert(err == 0);

        varVector.push_back(varInfo);
    }

    int rowCount = GetRowCount(err);
    assert(err == 0);

    int curPos = 0;
    int isMissing = 0;
    double dValue = 0.0;
    char* ch = NULL;

    for(int k = 0; k < rowCount; ++k) {
        err = NextCase();
        assert(err == 0);

        for(int m = 0; m < varVector.size(); ++m) {
            if(0 == varVector[m].type) {
                err = GetNumericValue(m, dValue, isMissing);
                assert(err == 0);
                std::cout << dValue << "    ";
            } else {
                ch = new char[varVector[m].type + 1];
                memset(ch,'\0',varVector[m].type + 1);
                err = GetStringValue(m,ch,varVector[m].type,isMissing);
                assert(err == 0);
                std::cout << ch << "    ";
                delete [] ch;
                ch = NULL;
            }
        }
        std::cout << std::endl;
        err = GetCursorPosition(curPos);
        assert(err == 0);
        std::cout << "the current cursor index is " << curPos << std::endl;
    }

    RemoveCaseCursor();
}

//test pivot table functions
void PivotTableFunctions()
{
    //Create a structure of a pivot table.
    int err = StartPivotTable("outline","table","table",false);
    assert( err == 0 );

    //Add row dimensions.
    AddDimension("outline","table","table",false,"job",0,1,false,false);
    AddDimension("outline","table","table",false,"gender",0,1,false,false);
    //Add column dimension.
    AddDimension("outline","table","table",false,"level",1,1,false,false);

    //Add string categories to row dimensions.
    AddStringCategory("outline","table","table",false,"job",0,1,false,false,"Clerical");
    AddStringCategory("outline","table","table",false,"gender",0,1,false,false,"Male");
    //Add string categories to column dimension.
    AddStringCategory("outline","table","table",false,"level",1,1,false,false,"8");

    //Set numeric cell value.
    SetFormatSpecCount();
    SetNumberCell("outline","table","table",false,"level",1,1,false,false,1);
    //Set cell footnotes.
    AddCellFootnotes("outline","table","table",false,"level",1,1,false,false,"cell footnotes");

    AddStringCategory("outline","table","table",false,"job",0,1,false,false,"Clerical");
    AddStringCategory("outline","table","table",false,"gender",0,1,false,false,"FeMale");
    AddStringCategory("outline","table","table",false,"level",1,1,false,false,"12");
    SetFormatSpecCount();
    SetNumberCell("outline","table","table",false,"level",1,1,false,false,2);

    AddStringCategory("outline","table","table",false,"job",0,1,false,false,"Custodial");
    AddStringCategory("outline","table","table",false,"gender",0,1,false,false,"Male");
    AddStringCategory("outline","table","table",false,"level",1,1,false,false,"12");
    SetFormatSpecCount();
    SetNumberCell("outline","table","table",false,"level",1,1,false,false,3);

    AddStringCategory("outline","table","table",false,"job",0,1,false,false,"Custodial");
    AddStringCategory("outline","table","table",false,"gender",0,1,false,false,"FeMale");
    AddStringCategory("outline","table","table",false,"level",1,1,false,false,"8");
    SetFormatSpecCount();
    SetNumberCell("outline","table","table",false,"level",1,1,false,false,4);

    //Add text block
    AddTextBlock("outline2","myText","hello",1);
    AddTextBlock("outline2","myText","world",1);
    AddTextBlock("outline2","myText","test",2);

}

//test data step functions
void DataStepFunctions()
{
	int err = 0;
	int i = 0;
	//const char* cmd = 
	StartDataStep();

	const char* dsname = "example";
	err = CreateDataset(dsname, false);

	int iTemp = GetCaseCountInDS(dsname, err);
	for (--iTemp; iTemp>=0; iTemp-=2)
	{
		err = InsertCase(dsname, iTemp);
	}

	iTemp = GetCaseCountInDS(dsname, err);
	for (--iTemp; iTemp>=0; iTemp-=2)
	{
		err = DeleteCase(dsname, iTemp);
	}

	const int length = 4;
	int types[length];
	for (i=0; i<length; i++)
	{
		types[i] = GetVarTypeInDS(dsname, i, err);
	}

	for (i = GetCaseCountInDS(dsname,err)-1; i>=0; i-=2)
	{
		for (int j=0; j<length; ++j)
		{
			int isMissing;
			if (types[j] == 0)
			{
				err = SetNCellValue(dsname, i, j, 88);
				std::cout<< GetNCellValue(dsname, i, j, isMissing, err) << ",";
			}
			else
			{
				err = SetCCellValue(dsname, i, j, "xx");
				std::cout<< GetCCellValue(dsname, i, j, isMissing, err) << ",";
			}
		}
		std::cout <<std::endl;
	}
	EndDataStep();
}

//main
int main(int argc, char* argv[])
{
    int iPosition = 0;
    int count = 1; //the count of iterations
    if (argc >1)
        count = atoi(argv[1]);

    std::vector<std::string> fList; //data file list
    filelist(fList);

    int load = LoadLib(); //load spssxd.dll dynamically
    if ( load != 0 )
    {
        std::cout << "load failure, exitcode = " << load << std::endl;
        return -1;
    }

#ifdef MS_WINDOWS
    std::string tempfile = "c:/temp/result.txt";
    std::string tempresult("-out ");
    tempresult = tempresult + tempfile;
    int err = StartSpss(tempresult.c_str());
#else
    int err = StartSpss("");
#endif
    assert( 0 == err );

    for (int i=0; i<count; i++)
    {
        int ready = IsBackendReady();
        assert( 1 == ready );

        //todo: add your test code here

        //test getfile
        int index=i%(int)fList.size();
        err = getfile(fList[index].c_str());
        assert ( 0 == err );
        if (err !=0 ){
            std::cout << "get file error: filename=>" << fList[index] << " err=" << err << std::endl;
        }

        //test XPath and general functions
        XPathAndGeneralFunctions(fList[index].c_str());
#ifdef MS_WINDOWS
        Displayresult(tempfile.c_str(), iPosition);
#endif

        //Start a user procedure.
        StartProcedure("proc1", "");
#ifdef MS_WINDOWS
        Displayresult(tempfile.c_str(), iPosition);
#endif

        //test data source functions
        DataSourceFunctions();
#ifdef MS_WINDOWS
        Displayresult(tempfile.c_str(), iPosition);
#endif

        //test pivot table functions
        PivotTableFunctions();
#ifdef MS_WINDOWS
        Displayresult(tempfile.c_str(), iPosition);
#endif

        //End the user procedure.
        EndProcedure();
#ifdef MS_WINDOWS
        Displayresult(tempfile.c_str(), iPosition);
#endif
		//test data step functions
		DataStepFunctions();
#ifdef MS_WINDOWS
        Displayresult(tempfile.c_str(), iPosition);
#endif
    }
    
    StopSpss();
#ifdef MS_WINDOWS
        Displayresult(tempfile.c_str(), iPosition);
#endif
    FreeLib(); //unload spssxd.dll dynamiclly
    return 0;
}

#ifdef MS_WINDOWS
void Displayresult(const char *filename, int &iPosition)
{
	std::ifstream fin(filename);

	std::string result;

	fin.seekg(iPosition, std::ios::beg);

	while(fin.peek()!=EOF)
	{
		getline(fin,result,'\n');
		std::cout << result <<std::endl;
	}

	iPosition = fin.tellg();

	fin.close();

	remove(filename);
}
#endif
