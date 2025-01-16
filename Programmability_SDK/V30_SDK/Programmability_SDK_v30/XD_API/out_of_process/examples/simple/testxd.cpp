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
 *     A simple example about how to use SPSSXD API from C.
 * 
 * COPYRIGHT
 *     Copyright 2005 by SPSS Inc.  All rights reserved.
 * 
 */

#include <string>
#include <iostream>
#include "spssxd.h"

#ifdef MS_WINDOWS
#include <stdio.h>
#include<fstream>
#endif

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
    // Start IBM SPSS Statistics backend.
#ifdef MS_WINDOWS
    std::string tempfile = "c:/temp/result.txt";
    std::string tempresult("-out ");
    tempresult = tempresult + tempfile;
    int error = StartSpss(tempresult.c_str());
#else
    int error = StartSpss("");
#endif
    if(error != 0)
    {
        std::cout << "Start IBM SPSS Statistics failure! error level = " << error << std::endl;
        return -1;
    }

    // The full path of data file.
    std::string filename;
    if (argc >1)
        // The full path of data file from commandLine argument.
        filename = argv[1];
    else
        // Default full path of data file
        filename = "C:\\Program Files\\IBM\\SPSS\\Statistics\\Samples\\English\\demo.sav";

    // Open a data file
    std::string cmd("get file=\'");
    cmd += filename;
    cmd += "\'.";
    Submit(cmd.c_str(),(int)cmd.length());

    // Execute a list command
    cmd = "list /cases=3.";
    Submit(cmd.c_str(),(int)cmd.length());
    
    // Stop IBM SPSS Statistics backend.
    StopSpss();
#ifdef MS_WINDOWS
    Displayresult(tempfile.c_str(), iPosition);
#endif

    return 0;
}
