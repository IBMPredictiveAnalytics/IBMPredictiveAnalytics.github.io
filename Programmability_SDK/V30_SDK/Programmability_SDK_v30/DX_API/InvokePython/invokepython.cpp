/************************************************************************
** Licensed Materials - Property of IBM 
**
** IBM SPSS Products: Statistics Common
**
** (C) Copyright IBM Corp. 1989, 2020
**
** US Government Users Restricted Rights - Use, duplication or disclosure
** restricted by GSA ADP Schedule Contract with IBM Corp. 
************************************************************************/

/**
 * NAME
 *     invokepython.cpp
 *
 * DESCRIPTION
 *     -- invokepythonapi.h
 *     Read the config file when spss drive external language.
 *
 *--------------------------------------------------------
 *
 * Copyright (c) 2006 SPSS Inc. All rights reserved.
 */


#include <Python.h>
#include <pythonrun.h>
#include "invokepythonapi.h"

#include <string>
#include <iostream>
#include <fstream>
#include <cassert>

#include "dxcallback.h"

#ifdef _WINDOWS
  #include <windows.h>
#else
  #include "IBM_SPSS_Copyright.h"
#endif

#if UNX_LINUX || UNX_MACOSX
#include <pthread.h>
#include <stdio.h>
#endif

#ifdef __cplusplus
extern "C" {
#endif

#ifdef _WINDOWS

HANDLE hMutex = NULL;

// dll entry point for Windows
BOOL WINAPI DllMain (HINSTANCE hinstDLL, DWORD fdwReason, LPVOID lpvReserved)
{
    if (fdwReason == DLL_PROCESS_ATTACH) {
        // Create a mutex with no initial owner.
        hMutex = CreateMutex(NULL, // default security attributes
                            FALSE, // initially not owned
                            NULL); // unnamed mutex

        if (hMutex == NULL) {
            return FALSE;
        }
        else {
            return TRUE;
        }
    }
    else if (fdwReason == DLL_PROCESS_DETACH) {
        if (hMutex != NULL) {
            CloseHandle(hMutex);
        }
        return TRUE;
    }
    return TRUE;
}

#endif

#if UNX_LINUX || UNX_MACOSX

pthread_mutex_t invokePythonMutex;

__attribute__((constructor))
void initMutex()
{
    if(pthread_mutex_init(&invokePythonMutex, NULL))
    {
        fprintf(stderr, "\nFailed to initialize the mutex.");
    }
}

__attribute__((destructor))
void destroyMutex()
{
    if(pthread_mutex_destroy(&invokePythonMutex))
    {
        fprintf(stderr, "\nFailed to destroy the mutex.");
    }
}

#endif

// below are DX call back functions

static DXCallBackHandle DxHandle = 0;

int SPSS_IsUTF8mode()
{
    assert(DxHandle);
    return DxHandle->IsUTF8mode();
}

const char* SPSS_GetDXVersion()
{
    assert(DxHandle);
    return DxHandle->GetDXVersion();
}

const char* SPSS_GetDXTempDir()
{
    assert(DxHandle);
    return DxHandle->GetDXTempDir();
}

int SPSS_GetNestLevel()
{
    assert(DxHandle);
    return DxHandle->GetNestLevel();
}

void SPSS_Trace(const char* msg)
{
    assert(DxHandle);
    DxHandle->Trace(msg);
}

//acquire the library lock.
INVOKEPYTHON_API int  GetLibraryLock() {
#ifdef _WINDOWS

    DWORD dwWaitResult;

    // Request ownership of mutex.
    dwWaitResult = WaitForSingleObject(
                    hMutex,     // handle to mutex
                    INFINITE);  // infinite wait

    if (dwWaitResult != WAIT_OBJECT_0 && dwWaitResult != WAIT_ABANDONED) {
        fprintf(stderr, "\nFailed to lock the mutex.");
        return 0; // false
    }
#endif

#if UNX_LINUX || UNX_MACOSX
    if(pthread_mutex_lock(&invokePythonMutex))
    {
        fprintf(stderr, "\nFailed to lock the mutex.");
        return 0; // false
    }
#endif
    return 1; // true
}

//release the library lock.
INVOKEPYTHON_API int  ReleaseLibraryLock() {
#ifdef _WINDOWS
    if(0 == ReleaseMutex(hMutex))
    {
        fprintf(stderr, "\nFailed to unlock the mutex.");
        return 0; // false
    }
#endif

#if UNX_LINUX || UNX_MACOSX
    if(pthread_mutex_unlock(&invokePythonMutex))
    {
        fprintf(stderr, "\nFailed to unlock the mutex.");
        return 0; // false
    }
#endif
    return 1; // true
}

//execute the X lanaguage command.
INVOKEPYTHON_API int  Execute_Python(char *cmd) {

    int errlvl=0;
    if (cmd != 0) {
        SPSS_Trace(cmd);
        errlvl=PyRun_SimpleString(cmd);
    }

    return errlvl;
}

//check if python initialized
INVOKEPYTHON_API int  Python_IsInitialized()
{
	return Py_IsInitialized();
}

//redirect the output.
INVOKEPYTHON_API void PostSpssOutput(FPPostSpssOutput fpSpssOutput) {
}

//initialize the X language environment.
INVOKEPYTHON_API int Init_Embedded_Python(int argc, char **argv)
{
    if (! Py_IsInitialized()) {
        Py_Initialize();
    }

    return 0;
}

//execute the python code file and redirects all output to log file.
//returns 0 on success, -1 on failure and -2 when scriptLogFile can not be opened.
INVOKEPYTHON_API int  Execute_Python_Ex(char *scriptFileName, char *logFileName) {

    int errlvl=0;
    if (scriptFileName != 0 && logFileName != 0) {
        std::string scriptCodeFile = std::string(scriptFileName);
        std::string scriptLogFile = std::string(logFileName);
        std::string::size_type idx;
        while((idx = scriptCodeFile.find_first_of("\\")) != std::string::npos) {
            scriptCodeFile.replace(idx,1,"/");
        }

        while((idx = scriptLogFile.find_first_of("\\")) != std::string::npos) {
            scriptLogFile.replace(idx,1,"/");
        }

        std::fstream fin;
        fin.open(scriptLogFile.c_str(),std::ios::in);
        if( !fin.is_open() )
        {
            fprintf(stderr, "\nScript log file can not be opened.");
            fin.close();
            return -2;
        }
        fin.close();

        std::string cmdAll("");
        cmdAll = std::string("exec '''\nimport traceback, sys\nwop = open(\"");
        cmdAll.append(scriptLogFile);
        cmdAll.append("\", 'w')\noldstdout=sys.stdout\noldstderr=sys.stderr\nsys.stdout=wop\nsys.stderr=wop\ntry:\n    f = open(\"");
        cmdAll.append(scriptCodeFile);
        cmdAll.append("\")\n    exec f\nexcept Exception, e:\n    info = sys.exc_info()\n    print info[0]\n    print info[1]\n    traceback.print_tb(info[2])\nwop.flush()\nsys.stdout=oldstdout\nsys.stderr=oldstderr\n''' in globals().copy(), locals().copy()");

        const int iLen = cmdAll.size() + 1;
        char* cmdFinal = new char[iLen];
        strcpy(cmdFinal, cmdAll.c_str());

        errlvl=PyRun_SimpleString(cmdFinal);

        delete [] cmdFinal;
    }

    return errlvl;
}

// 
INVOKEPYTHON_API void setup_dxcallback(void* dxhandle)
{
    DxHandle = (DXCallBackHandle)dxhandle;
}

//initialize the X language environment.
INVOKEPYTHON_API int init_embedded_x(int argc, char **argv)
{
    SPSS_Trace("init_embedded_x");
    int err = Init_Embedded_Python(argc, argv);

    return err;
}

//execute the X lanaguage command.
INVOKEPYTHON_API int  execute_x(char* cmd)
{
    int err = 0;

    std::string codePage("utf-8");
    if(!SPSS_IsUTF8mode()){
        if (getenv("PYTHON_LOCALE"))
            codePage = std::string(getenv("PYTHON_LOCALE"));
        else
            codePage = std::string("");
    }
    std::string singleCmd = "#coding: ";
    singleCmd.append(codePage).append("\n");
    singleCmd.append(cmd);
    
    int curnest = SPSS_GetNestLevel();
    if(curnest)
    {
        char buffer[8];
        sprintf(buffer, "%d", curnest) ;
        std::string depth( buffer );
        std::string codeVar("spss_code_temp_");
        codeVar.append(depth);

        std::string nestCmd(codeVar);
        nestCmd.append(" = compile(\"\"\"\n");    
        while(true)
        {
            std::size_t pos = singleCmd.find("\"\"\"");
            if(pos != std::string::npos)
            {
                singleCmd.replace(pos,3,"\\\"\\\"\\\"");
            }
            else
            {
                break;
            }
        }
        nestCmd.append(singleCmd);
        nestCmd.append("\"\"\",\"\",\"exec\")\n");
        nestCmd.append("exec(\"theDict={}\")\n");    
        nestCmd.append("exec(");
        nestCmd.append(codeVar);
        nestCmd.append(") in globals(),theDict\n");
        nestCmd.append("del ");
        nestCmd.append(codeVar);

        err = Execute_Python((char*)nestCmd.c_str());
    }
    else
    {
        err = Execute_Python((char*)singleCmd.c_str());
    }
    return err;
}

INVOKEPYTHON_API int  stop_embedded_x()
{
    SPSS_Trace("stop_embedded_x");
    Py_Finalize();
    return 0;
}

INVOKEPYTHON_API int  pre_action()
{
    int err = 0;

    int curnest = SPSS_GetNestLevel();
    
    std::string script_preaction(
        "from spss import preaction\n"
        "preaction.start(");
    if(curnest)
    {
        char buffer[8];
        sprintf(buffer, "%d", curnest) ;
        std::string depth( buffer );
        std::string tempdir( SPSS_GetDXTempDir() );
    #if _WINDOWS
        std::string::size_type idx;
        while((idx = tempdir.find_first_of("\\")) != std::string::npos) {
            tempdir.replace(idx,1,"/");
        }
    #endif
        script_preaction.append(depth);
        script_preaction.append(",\"");
        script_preaction.append(tempdir);
        script_preaction.append("\"");
    }   
    script_preaction.append(")");
    
    err = Execute_Python((char*)script_preaction.c_str());

    return err;
}

INVOKEPYTHON_API int  post_action()
{
    char* script_postaction = 
        (char *)"from spss import postaction\n"
        "postaction.end()";
    return Execute_Python(script_postaction);
}

#ifdef __cplusplus
}
#endif
