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
*     test the functions of spssxd api iteratively. spssxd library will be load
*     and unload statically.
*
* COPYRIGHT
*     Copyright 2005 by SPSS Inc.  All rights reserved.
*
*/

#include <string>
#include <vector>
#include <iostream>
#include <cassert>
#include "spssxd.h"

#ifdef MS_WINDOWS
#include <stdio.h>
#include<fstream>
#endif

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
    // For example, "c:\\Progra~1\\IBM\\SPSS\\Statistics\\Samples\\" for windows
    // Or, "/usr/local/StatisticsServer/samples/" for UNIX
    std::string filepath = "C:\\Program Files\\IBM\\SPSS\\Statistics\\Samples\\English\\";

    //create the full path of data file and push it into fList.
#ifdef UNX_LINUX
    fList[0] = (filepath+"demo.sav");
    fList[1] = (filepath+"anorectic.sav");
    fList[2] = (filepath+"carpet.sav");
    fList[3] = (filepath+"Employee data.sav");
    fList[4] = (filepath+"guttman.sav");
    fList[5] = (filepath+"judges.sav");
    fList[6] = (filepath+"kinship_dat.sav");
    fList[7] = (filepath+"kinship_ini.sav");
    fList[8] = (filepath+"kinship_var.sav");
    fList[9] = (filepath+"ozone.sav");
    fList[10] = (filepath+"screws.sav");
    fList[11] = (filepath+"tv-survey.sav");
    fList[12] = (filepath+"verd1985.sav");
    fList[13] = (filepath+"breakfast.sav");
    fList[14] = (filepath+"catalog.sav");
#else
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
#endif
}

//test XPath and general functions
void XPathAndGeneralFunctions(const char* datafile)
{
    int err = 0;

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

    for( int x=0; x< varnum; x++){
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
    for (int x=0; x<size; x++){
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
    err = MakeCaseCursor();
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

//test pivot table functions
void DataStepFunctions()
{
	int err = 0;
	//const char* cmd = 
	StartDataStep();

	const char* dsname = "example";
	err = CreateDataset(dsname);

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
	for (int i=0; i<length; i++)
	{
		types[i] = GetVarTypeInDS(dsname, i, err);
	}

	for ( int i = GetCaseCountInDS(dsname,err)-1; i>=0; i-=2)
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

int main(int argc, char* argv[])
{
    int iPosition = 0;
    int count = 1;//the count of iterations
    if (argc >1)
        count = atoi(argv[1]);

#ifdef UNX_LINUX
    std::vector<std::string> fList(100);//data file list
#else
    std::vector<std::string> fList;//data file list
#endif
    filelist(fList);
#ifdef MS_WINDOWS
    std::string tempfile = "c:/temp/result.txt";
    std::string tempresult("-out ");
    tempresult = tempresult + tempfile;
    int error = StartSpss(tempresult.c_str());
#else
    int error = StartSpss("");
#endif
    assert( 0 == error );

    for ( int i=0; i<count; i++)
    {
        int drive = IsXDriven();
        assert( 1 == drive );
        int ready = IsBackendReady();
        assert( 1 == ready );

        int err = 0;

        //todo: add your test code here

        //getfile
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
        StartProcedure("proc1");
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
    return 0;
}
