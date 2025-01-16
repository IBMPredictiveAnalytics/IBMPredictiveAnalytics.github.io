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

int main(int argc, char* argv[])
{
    // Start IBM SPSS Statistics backend.
    int error = StartSpss("");
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

    return 0;
}
