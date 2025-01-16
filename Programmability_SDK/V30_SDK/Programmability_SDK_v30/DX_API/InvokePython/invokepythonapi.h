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
 * NAME
 *     invokepythonapi.h
 *
 * DESCRIPTION
 *     Read the config file when spss drive external language.
 *
 *--------------------------------------------------------
 *
 * Copyright (c) 2006 SPSS Inc. All rights reserved.
 */



#ifndef INVOKERPYTHON__H
#define INVOKERPYTHON__H


#ifdef MS_WINDOWS

  #ifdef INVOKEPYTHON_EXPORTS
  #define INVOKEPYTHON_API __declspec(dllexport)
  #else
  #define INVOKEPYTHON_API __declspec(dllimport)
  #endif

#else
  #define INVOKEPYTHON_API
#endif


#ifdef __cplusplus
extern "C" {
#endif

  typedef int (*FPPostSpssOutput)(const char* text, int length);

  /**
   * initialize the X environment.
   */
  INVOKEPYTHON_API int   Init_Embedded_Python(int argc, char **argv);

  /**
   * execute the X language command.
   */
  INVOKEPYTHON_API int   Execute_Python(char *cmd);

  /**
   * redirection the output.
   */
  INVOKEPYTHON_API void  PostSpssOutput(FPPostSpssOutput fpSpssOutput);

  /**
   * acquire the library lock. 
   */
  INVOKEPYTHON_API int  GetLibraryLock();

  /**
   * release the library lock.
   */
  INVOKEPYTHON_API int  ReleaseLibraryLock();

  /**
   * execute the python code file and redirects all output to log file.
   */
  INVOKEPYTHON_API int  Execute_Python_Ex(char *scriptFileName, char *logFileName);

  /**
   * check if python initialized
   */
  INVOKEPYTHON_API int  Python_IsInitialized();

#ifdef __cplusplus
}
#endif //__cplusplus

#endif //INVOKERPYTHON__H
