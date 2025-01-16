/************************************************************************
** IBM Confidential
**
** OCO Source Materials
**
** IBM SPSS Products: Statistics Common
**
** (C) Copyright IBM Corp. 1989, 2017
**
** The source code for this program is not published or otherwise divested of its trade secrets, 
** irrespective of what has been deposited with the U.S. Copyright Office.
************************************************************************/

/**
 * NAME
 *     spssxd.h
 *
 * DESCRIPTION
 *     Present a callable interface to the IBM(R) SPSS(R) Statistics backend.
 *     The interface is only primitive types.
 *
 *--------------------------------------------------------
 *
 */

// DOxygen comments are of the form /**...*/ or ///...


/** \mainpage XD API

\section intro Introduction

The XD API is a collection of C/C++ functions that enable external programs
to control the IBM SPSS Statistics backend ("X-Drives").  These functions are implemented
in part with facilities already present, drawing heavily from the 
IBM SPSS Statistics Batch Facility, a small executable program providing command line control of
the IBM SPSS Statistics backend.

The IBM SPSS Statistics backend is the part of the application that handles
statistical computations and data management. The backend is controlled by
the IBM SPSS Statistics command language.

Once started, the IBM SPSS Statistics backend performs a 'Fetch/Execute' cycle continually
alternating between a phase of getting the next command from its input stream and
a phase of executing the command obtained.

One of the facilities of the IBM SPSS Statistics backend pertinant to an understanding of
the XD interface is the IBM SPSS Statistics Macro Processor.  Using the Define command,
macro programs are created that can convert one or more input symbols
into any number of additional input tokens. Such a macro program results
in a string of tokens which is then 'pushed back' onto the input stream
and then fetched and executed ahead of any other input.

An external program controlling the IBM SPSS Statistics backend plays a role similar to the macro processor. Once
started with the XD API StartSpss function, the backend waits for 'requests' from
the external program through the XD API functions.

The main work-horses of the XD interface are the
QueueCommandPart and the Submit functions.
QueueCommandPart takes a string
argument and queues it as a line of IBM SPSS Statistics command input -- but it does not execute it.
Submit takes a string argument, queues it as a line of IBM SPSS Statistics command input on
the backend input stream, and then waits for the backend to process all the queued lines.
The lines ending in '.' indicate the end of an IBM SPSS Statistics command. After the completion
of the queued commands, the Submit function's return value indicates the
maximum IBM SPSS Statistics 'error level' from any command executed.

Each of the XD API functions is a request of some sort to the IBM SPSS Statistics
backend. The two main types are 'actions requests' (for example, Submit)
and 'information requests' (for example, GetRowCount).
Usually the return value of an action request is an error code and the
return value of an information request is the information requested.

This XD API documentation is part of a software development kit (SDK) for the
Programmability Extension. For more information and tools to use with the Programmability Extension, 
visit the SPSS community at http://www.ibm.com/developerworks/spssdevcentral/.

\subpage notices "Legal Notices"

*/

/*! \page notices
\htmlinclude legalnotices.htm
*/

#ifndef __SPSSXD_H__
#define __SPSSXD_H__

#ifdef SPSSXD_EXPORTS
# define SPSSXD_API __declspec(dllexport)
#else
# if MS_WINDOWS
#   define SPSSXD_API __declspec(dllimport)
# else
#   define SPSSXD_API
# endif
#endif

class SpssAdapter;
class SpssXDSmb;

extern "C" {
/** IsBackendReady is used to determine if the IBM SPSS Statistics backend is ready to
  receive commands. It is of interest only when an external program is controlling
  the backend.
  The return value is always true when IBM SPSS Statistics is controlling the backend.
  \code
     if(!IsBackendReady()){
        StartSpss();
      }
  \endcode
  \return true=The IBM SPSS Statistics backend is running. \n
          false=The IBM SPSS Statistics backend is not running
*/
SPSSXD_API bool IsBackendReady();

/** IsXDriven is used to determine the mode of operation. If IBM SPSS Statistics is not
    the main program, the mode is 'External', or 'XDriven'. It is necessary to
    use this function in situations where code is used both from the IBM SPSS Statistics
    command line, and from a stand-alone program. For example, StopSpss() is not
    effective when !IsXDriven().
    \code
       if(IsXDriven()){
         StopSpss();
       }
    \endcode
    \return true=IBM SPSS Statistics is not the main program \n
            false=IBM SPSS Statistics is the main program
*/
SPSSXD_API bool IsXDriven();

/** The XD API uses an SpssAdapter and an SpssXDSmb to communicate
    with IBM SPSS Statistics. When IBM SPSS Statistics is the main program, it provides these with SetUpFromSpss.
    We do not expect other applications will use this function.
    It is exported for IBM SPSS Statistics to use.
    \return 0=Success
*/
SPSSXD_API int SetUpFromSpss( SpssAdapter* anAdapter, SpssXDSmb* anSmb);

/** Start the IBM SPSS Statistics backend using parameters. These are a subset of the parameters
    available on the IBM SPSS Statistics Batch Facility command line.
  \param text the parameters.
  \code
  -type text|html|xhtml|webreport|oxml|sxml|sav

  -type text produces space separated plain text output. Tab separated output is available
             with the -t switch.
  -type html produces a single HTML output file. If there are charts or classification trees in the ouput
             these will be embedded in the HTML file.
  -type xhtml produces HTML3.2 or earlier output. If there are charts or classification trees in the ouput
             these will be saved as portable network graphic (PNG) files and referenced in the
             HTML.
  -type webreport produces an interactive HTML5 output. If there are charts or classification trees in the ouput
             these will be embedded in the HTML file.
  -type oxml produces XML output, which conforms to the SpssOutputXml.xsd schema.
  -type sxml produces output in a format suitable for publising to the Web using
             an application such as SmartViewer Web Server.
  -type sav  produces ouput in an IBM SPSS Statistics-format data file. This is a binary format and may
             contain only one pivot table or multiple tables of the same structure.


    -drop<item>  Supresses output for a given autoscriptname.
    -keep<item>  Keeps output for a given autoscriptname.
    -pb          Inserts page breaks between output from different procedures and/or output
                 items.
    -t           Use tabs for column separators
    -n <num_of_chars>    number of columns perline of output
    -rs <row_separator>  The row separator.
    -cs <col_separator>  The column separator
    -LC_ALL <locale> Set the localle. See your operating system for the possible values.
    -language <lang> de en us fr it ja ko pl zh_tw. Sets the language for error messages
                     and other output.
    -encoding <encod> Sets the subdirectory where the language encodings are stored.
  \endcode
  \return The return code. \n
          0=Success \n 
          17=IBM SPSS Statistics backend is not ready

*/
SPSSXD_API int StartSpss(const char* commandline = 0);

/** StopSpss causes the IBM SPSS Statistics backend to shutdown when it was
    started from an external application. It is a no-op when IBM SPSS Statistics
    is the main program.
    \code
       if(IsXDriven()){
         StopSpss();
      }
    \endcode
*/
SPSSXD_API void StopSpss();

/** Queue a line of an IBM SPSS Statistics command and execute all of the currently
    queued lines. This will include multiple commands if previously
    queued lines end with a command terminator. The backend needs to
    be started before calling this function.
    There are multiple ways in which
    this function may fail. First, there may not be a current connection
    to the IBM SPSS Statistics backend. However, the most likely
    cause of failure is a problem with one of the submitted commands.
    Each error in running an IBM SPSS Statistics command results in a non-zero return
    code.
    
    When submitting syntax that includes a BEGIN PROGRAM - END PROGRAM block, each of the lines in the block (including BEGIN PROGRAM)
    must be queued using the QueueCommandPart function. The END PROGRAM statement, however, can be submitted with the Submit function. In addition,
    note that you cannot submit an empty BEGIN PROGRAM - END PROGRAM block. The block must contain at least one statement in the associated language (Python or R).
  \param command The is the content of the line.
  \param length The length of the line in bytes.
  \code
     int err = 0;
     if(!IsBackendReady()){
        err = StartSpss();
      }
     if( 0 == err ){
        Submit("show locale.",12);
     }
     if(IsXDriven()){
       StopSpss();
     }
  \endcode
  \return 0=Success \n
          1=Comment \n 
          2=Warning \n 
          3=Serious \n 
          4=Fatal \n
          5=Catastrophic \n
          17=IBM SPSS Statistics backend is not ready
*/
SPSSXD_API int Submit(const char* command, int length);

/** Queue a line of IBM SPSS Statistics command text but do not execute it.
    The backend needs to be started before calling this function.
    Each command string queued is treated as a line of IBM SPSS Statistics input.
    There is an implicit 'new-line' character at the end of each queued line.
    Other new-line characters are not permitted.
    If the line ends with a command terminator, the next queued line will
    start a new IBM SPSS Statistics command.
    
    When submitting syntax that includes a BEGIN PROGRAM - END PROGRAM block, each of the lines in the block (including BEGIN PROGRAM)
    must be queued using the QueueCommandPart function. The END PROGRAM statement, however, can be submitted with the Submit function. In addition,
    note that you cannot submit an empty BEGIN PROGRAM - END PROGRAM block. The block must contain at least one statement in the associated language (Python or R).
  \param line The content of the line.
  \param length is the length of the line in bytes.
  \code
     int err = 0;
     if(!IsBackendReady()){
        err = StartSpss();
      }
     if( 0 == err ){
        QueueCommandPart("show all.",9);
     }
  \endcode
  \return     The return code. \n
              0=Success \n
              1=Comment \n
              2=Warning \n
              3=Serious \n
              4=Fatal \n
              5=Catastrophic \n
              17=IBM SPSS Statistics backend is not ready \n
              98=Can't submit in data step
*/
SPSSXD_API int QueueCommandPart(const char* line, int length);

/** Adds a line to the IBM SPSS Statistics log. The IBM SPSS Statistics backend needs to be started before calling this function.
  \param text is the content of the line.
  \param length is the length of the line in bytes.
  \code
     int err = 0;
     if(!IsBackendReady()){
        err = StartSpss();
      }
     if( 0 == err ){
        PostSpssOutput("sample output.",12);
     }
  \endcode
  \return   The return code. \n
            0=Success \n
            17=IBM SPSS Statistics backend is not ready
*/
SPSSXD_API int PostSpssOutput(const char* text, int length);

/**  GetVariableCount returns the number of variables in the IBM SPSS Statistics active file.
    The backend needs to be started and there needs to be an active datasource before
    calling this function. This value may be zero because in certain stages of processing data sources,
    no variables (and hence no cases) may exist. This is the state when the IBM SPSS Statistics backend first starts.
  \param errCode Returned value indicates error.
                  0=No error, 9=No data source, 17=IBM SPSS Statistics backend is not ready
  \code
     int err = 0;
     if(!IsBackendReady()){
        err = StartSpss();
     }
     if( 0 == err ){
        const char *cmd = "Get file = 'demo.sav'.";
        Submit(cmd, strlen(cmd));
        unsigned varNum = GetVariableCount(err);
     }
     if(IsXDriven()){
       StopSpss();
     }
  \endcode
  \return n>0 The number of variables.
*/
SPSSXD_API unsigned GetVariableCount(int& errCode);

/** GetRowCount returns the number of cases in the IBM SPSS Statistics active file.
    The backend needs to be started and there needs to be an active data source before calling this function.
    The value -1 indicates the case count is 'unknown'.
    This is the value prior to an actual pass of data from a data source.
    The value is non-negative for data in .sav files.
    The -1 value occurs before a pass of the data from a source other than a .sav file.
  \param errCode Returned value indicates error Code.
                  0=No error, 9=No data source, 17=IBM SPSS Statistics backend is not ready
  \code
     int err = 0;
     if(!IsBackendReady()){
        err = StartSpss();
      }
     if( 0 == err ){
        const char *cmd = "Get file = 'demo.sav'.";
        Submit(cmd, strlen(cmd));
        long rowNum = GetRowCount(err);
     }
     if(IsXDriven()){
       StopSpss();
     }
  \endcode
  \return n>0 The number of cases.
*/
SPSSXD_API long GetRowCount(int& errCode);

/** GetVariableName returns the name of the indexed variable in the current active file.
  The backend needs to be started and there needs to be an active datasource before calling this function.
  The index value must be within bounds of the variable count.
  \param index The zero based index of the variable.
  \param errCode Returned value indicates error level. \n
                  0=No error \n
                  9=No data source \n
                  10=Invalid index \n
                  17=IBM SPSS Statistics backend is not ready
  \code
     int err = 0;
     if(!IsBackendReady()){
        err = StartSpss();
      }
     if( 0 == err ){
        const char *cmd = "Get file = 'demo.sav'.";
        Submit(cmd, strlen(cmd));
        const char* varName = GetVariableName(0,err);
     }
     if(IsXDriven()){
       StopSpss();
     }
  \endcode
  \return ""=failure or no active file, otherwise the variable name.
*/
SPSSXD_API const char* GetVariableName(int index,int& errCode);

/** GetVariableLabel returns the label of the indexed variable in the current active file.
  The IBM SPSS Statistics backend needs to be started and there needs to be an active data source before calling this function.
  The index value must be within bounds of the variable count.
  \param index The zero based index of the variable.
  \param errCode Returned value indicates error code. \n
                  0=No error \n
                  9=No data source \n
                  10=Invalid index \n
                  17=IBM SPSS Statistics backend is not ready
  \code
     int err = 0;
     if(!IsBackendReady()){
        err = StartSpss();
      }
     if( 0 == err ){
        const char *cmd = "Get file = 'demo.sav'.";
        Submit(cmd, strlen(cmd));
        const char* varLabel = GetVariableLabel(0,err);
     }
     if(IsXDriven()){
       StopSpss();
     }
  \endcode
  \return if No Error,  the variable label.
*/
SPSSXD_API const char* GetVariableLabel(int index,int& errCode);

/** GetVariableType returns the type of the indexed variable in the current active file.
  The backend needs to be started and there needs to be an active datasource before calling this function.
  The index value must be within bounds of the variable count.
  \param index The zero based index of the variable.
  \param errCode Returned value indicates error code. \n
                  0=No error \n
                  9=No data source \n
                  10=Invalid index \n
                  17=IBM SPSS Statistics backend is not ready
  \code
     int err = 0;
     if(!IsBackendReady()){
        err = StartSpss();
      }
     if( 0 == err ){
        const char *cmd = "Get file = 'demo.sav'.";
        Submit(cmd, strlen(cmd));
        int varType = GetVariableType(0,err);
     }
     if(IsXDriven()){
       StopSpss();
     }
  \endcode
  \return If No Error, the type of the variable.
          A variable's type is zero for numeric variables and the length in
          bytes for string variables.
*/
SPSSXD_API int GetVariableType(int index,int& errCode);

/** GetVariableFormat returns the print format of the indexed variable.
    The IBM SPSS Statistics backend needs to be started and there needs to be an active
    data source before calling this function.
    The index value must be within bounds of the variable count.
    \param index The zero based index of the variable.
    \param errCode Returned value indicates error code. \n
                  0=No error \n
                  9=No data source \n
                  10=Invalid index \n
                  17=IBM SPSS Statistics backend is not ready
    \code
       int err = 0;
       if(!IsBackendReady()){
          err = StartSpss();
        }
       if( 0 == err ){
          const char *cmd = "Get file = 'demo.sav'.";
          Submit(cmd, strlen(cmd));
          const char* varFormat = GetVariableFormat(0,err);
       }
       if(IsXDriven()){
         StopSpss();
       }
    \endcode
    \return If No Error, a string representing the variable's print format.
*/
SPSSXD_API const char* GetVariableFormat(int index,int& errCode);

/** GetVariableMeasurementLevel returns the measurement level of the indexed variable in the current active file.
    Measurement level is an optional field that describes IBM SPSS Statistics variables. The backend needs to be started and there
    needs to be an active datasource before calling this function.
    The index value must be within bounds of the variable count.
    \param index The 0 based index of the variable.
    \param errCode Returned value indicates the error code. \n
                   0=No error \n
                   9=No data source \n
                   10=Invalid index \n
                   17=IBM SPSS Statistics backend is not ready
    \code
       int err = 0;
       if(!IsBackendReady()){
          err = StartSpss();
       }
       if( 0 == err ){
          const char *cmd = "Get file = 'demo.sav'.";
          Submit(cmd, strlen(cmd));
          int varMeasurementLevel = GetVariableMeasurementLevel(0,err);
       }
       if(IsXDriven()){
         StopSpss();
       }
    \endcode
    \return 1 = unknown, 2 = nominal, 3 = ordinal, 4 = scale

 */
SPSSXD_API int GetVariableMeasurementLevel(int index,int& errCode);

/** CreateXPathDictionary creates an XPath DOM from the current active file dictionary and
    adds it to the XML workspace with the given handle. This requires potentially large amounts of memory.
    It is the caller's responsibility to free the memory by removing the object using the same handle.
    The backend needs to be started before calling this function.
  \param handle is the case-insensitive name for the object added.
  \code
     int err = 0;
     if(!IsBackendReady()){
        err = StartSpss();
      }
     if( 0 == err ){
        const char *cmd = "Get file = 'demo.sav'.";
        Submit(cmd, strlen(cmd));
        CreateXPathDictionary("theHandle");
     }
     if(IsXDriven()){
       StopSpss();
     }
  \endcode
  \return  The return code. \n 
           0=Success \n 
           9=No data source \n
           17=IBM SPSS Statistics backend is not ready
*/
SPSSXD_API int CreateXPathDictionary(const char* handle);

/** RemoveXPathHandle remove an object from XML workspace, freeing the storage.
  The backend needs to be started before calling this function.
  There needs to be an XPATH DOM with the given handle in XML workspace
  before calling this function.
  \param handle is the case-insensitive name of object to remove.
  \code
     RemoveXPathHandle("theHandle");
  \endcode
  \sa
     EvaluateXPath
  \return   The return code. \n
            0=success \n 
            17=IBM SPSS Statistics backend is not ready
*/
SPSSXD_API int RemoveXPathHandle(const char* handle);

/** EvaluateXPath evaluates the XPath expression in the given context.
    The backend needs to be started before calling this function.
    The handle must be a valid name of an XPath DOM in the
    XML workspace.
  \param handle is a case-insensitive name of object to search.
  \param context is the XPath context for expression.
  \param expression is the expression to evaluate.
  \param errCode returned value indicating errors. \n
  0=No error \n
  9=No data source \n
  17=IBM SPSS Statistics backend is not ready
  \code
     int err = 0;
     if(!IsBackendReady()){
        err = StartSpss();
      }
     if( 0 == err ){
        const char *cmd = "Get file = 'demo.sav'.";
        Submit(cmd, strlen(cmd));
        CreateXPathDictionary("theHandle");
        void* result = EvaluateXPath("theHandle","/dictionary","variable[1]/@name",err);
        int len = GetStringListLength(result);
        if( len > 0 ){
            const char* str = GetStringFromList(result,0);
        }
        RemoveStringList(result);
        RemoveXPathHandle("theHandle");
     }
     if(IsXDriven()){
       StopSpss();
     }
  \endcode
  \return listHandle for retreiving results.
*/
SPSSXD_API void* EvaluateXPath(const char* handle,
                               const char* context,
                               const char* expression,
                               int& errCode);

/** GetStringListLength returns the length of the list found with EvaluateXPath.
    The listHandle pointer must have been returned from EvaluateXPath, but the list may be empty.
  \param listHandle the pointer returned from EvaluateXPath
  \sa   EvaluateXPath
  \return The number of strings in the list.
  */
SPSSXD_API int GetStringListLength(void* listHandle);

/** GetStringFromList gets a single string from the list returned from EvaluateXPath.
    The listHandle pointer must have been returned from EvaluateXPath and the
    index must be a valid zero based index in the defined list.
  \param listHandle The pointer returned from EvaluateXPath
  \param index The 0 based index of the desired string.
  \sa     EvaluateXPath
  \return The indexed string. The returned value is empty if the input is invalid.
  */
SPSSXD_API const char* GetStringFromList(void* listHandle, int index);

/** RemoveStringList deletes the storage associated with the string list.
  The indicated listHandle must be valid at the time this function is called.
  \param listHandle The pointer returned from EvaluateXPath
  \sa     EvaluateXPath
*/
SPSSXD_API void RemoveStringList(void* listHandle);

/** GetXmlUtf16 gets XML from XML workspace, and returns an Utf16 representation.
  \param handle a case-insensitive name of XML object in the XML workspace.
  \param errCode returned value indicating errors. \n
  0=No error \n
  17=IBM SPSS Statistics backend is not ready
  \code
     int err = 0;
     if(!IsBackendReady()){
        err = StartSpss();
      }
     if( 0 == err ){
        const char *cmd = "Get file = 'demo.sav'.";
        Submit(cmd, strlen(cmd));
        CreateXPathDictionary("theHandle");
        const wchar_t* xml = GetXmlUtf16("theHandle",err);
        RemoveXPathHandle("theHandle");
     }
     if(IsXDriven()){
       StopSpss();
     }
  \endcode
  \return String value of the result.
*/
SPSSXD_API const wchar_t* GetXmlUtf16(const char* handle,
                                            int& errCode);
/** GetHandleList gets the list of handles of the objects currently in
    the XML workspace. The structure of this list is the same as returned
    from EvaluateXPath so the functions, GetStringListLength and GetStringFromList
    may be used to obtain the string value of the handles. Use RemoveStringList
    to free the storage relating to this list.
    \param errLevel This is the return code. \n
                     0=Success \n
                    17=IBM SPSS Statistics backend is not ready

    \code
     int err = 0;
     if(!IsBackendReady()){
        err = StartSpss();
      }
     if( 0 == err ){
        const char *cmd = "Get file = 'demo.sav'.";
        Submit(cmd, strlen(cmd));
        CreateXPathDictionary("theHandle1");
        CreateXPathDictionary("theHandle2");
        void* result = GetHandleList(err);
        int len = GetStringListLength(result);
        if( len > 0 ){
            const char* str = GetStringFromList(result,0);
        }
        RemoveStringList(result);
     }
     if(IsXDriven()){
       StopSpss();
     }
    \endcode
    \return         This value must be used when retrieving items in the list.
                    The list may be empty.
 *
 */
SPSSXD_API void* GetHandleList(int& errLevel);

/**   Returns the value and missing status for the indexed variable.
      \param varIndex   The index of a numeric variable
      \param result     The location that will receive the result.
      \param isMissing  The location that will receive the missing status.
      \sa
        MakeCaseCursor
      \return           The return code. \n
                         0=success \n
                        10=Invalid index \n
                        23=No more data \n
                        24=Not a numeric variable index
                        111=Invalid cursor position, need to call NextCase first.

*
*/
SPSSXD_API int GetNumericValue(unsigned varIndex, double& result, int& isMissing);

/**   Returns the string value for the indexed variable.
      bufferLength must at least be the size of the string variable.
      It is an error if the index is not of a string variable.
      \param varIndex   The index of a string variable
      \param result     The location that will receive the result.
      \param bufferLength The size of the buffer supplied for the result.
      \param isMissing  The location that will receive the missing status.
      \sa
          MakeCaseCursor
      \return           The return code. \n
                         0=Success \n
                        10=Invalid index \n
                        23=No more data \n
                        24=Not a string variable index \n
                        111=Invalid cursor position, need to call NextCase first.
*
*/
SPSSXD_API int GetStringValue(unsigned varIndex, char* &result, int bufferLength, int &isMissing);

/**   Moves the cursor to the next case. The result indicates the end of data, or success.
      The cursor becomes invalid once the end of data is reached.
      \sa
          MakeCaseCursor
      \return     The return code. \n
                  0=Success \n
                  9=No data source \n
                  17=IBM SPSS Statistics backend is not ready \n
                  23=No more data
*
*/
SPSSXD_API int NextCase();

SPSSXD_API void* NextCasePtr(int &caseSize,int &errLevel);

/**   Frees the storage for a case. It is an error if the cursor is not valid.
      \sa
          MakeCaseCursor
      \return The return code. \n
              0=success \n
             17=IBM SPSS Statistics backend is not ready
 *
 */
SPSSXD_API int RemoveCaseCursor();

/**   GetVariableFormatType returns the format type, width, and decimal of the indexed variable in the current active file.
      The IBM SPSS Statistics backend needs to be started and there needs to be an active data source before calling this function.
      The index value must be within bounds of the variable count.
      \param varIndex   The index of a variable
      \param formatType    The location that will receive the format type.
      \param formatWidth   The location that will receive the format width.
      \param formatDecimal The location that will receive the number of decimals.
      \return              The return code. \n
                           0=Success \n
                           9=No data source \n
                           10=Invalid index \n
                           17=IBM SPSS Statistics backend is not ready \n
                           23=No more data
 *
 */
SPSSXD_API int GetVariableFormatType(int varIndex, int& formatType, int& formatWidth, int& formatDecimal);

/** Get the row index of the current cursor. The row index is the number of times NextCase has been called
    after MakeCursor was called to construct the cursor.
    \param curPos the location to receive the resulting current position.
    \sa
        MakeCaseCursor
    \return The return code. \n
            0=No error \n
            60=No cursor is running \n
            17=IBM SPSS Statistics backend is not ready
 *
 */
SPSSXD_API int GetCursorPosition(int& curPos);

    /**  Gets user missing values of the indexed string variable.
       \param index variable index.
       \param missingFormat format code.
       \param v1 the first missing value.A caller should allocate 9 bytes memory.
       \param v2 the second missing value.A caller should allocate 9 bytes memory.
       \param v3 the third missing value.A caller should allocate 9 bytes memory.
       \sa
           GetVarNMissingValues
       \return The return code. \n
               0=No error \n
               9=No data source \n
               10=Invalid index \n
               56=Expects string variable
    */
    SPSSXD_API int GetVarCMissingValues(const int index, int *missingFormat,char *v1, char *v2, char *v3);


    /**  Gets user missing values for the indexed numeric variable in the current active file.
       \param index variable index.
       \param missingFormat format code.
       \param v1 the first missing value.
       \param v2 the second missing value.
       \param v3 the third missing value.
       \code
           void func()
           {
               int errLevel = 0;
               errLevel = StartSpss();

               char *cmd1 = "get file = 'Employee data.sav'.";
               errLevel = Submit(cmd1,strlen(cmd1));

               unsigned varCount = GetVariableCount(errLevel);

               for(int i = 0; i < varCount; i++) {
                   int varType = GetVariableType(i,errLevel);

                   if(varType != 0 && varType < 8) {
                        const int CMISSING_SIZE = 9;
                        char v1[CMISSING_SIZE],v2[CMISSING_SIZE],v3[CMISSING_SIZE];
                        int missingFormat;
                        memset(v1,0,CMISSING_SIZE);
                        memset(v2,0,CMISSING_SIZE);
                        memset(v3,0,CMISSING_SIZE);
                        errLevel = GetVarCMissingValues(i,&missingFormat,v1, v2, v3);
                    }
                    if(varType == 0) {
                        double v1,v2,v3;
                        int missingFormat;
                        errLevel = GetVarNMissingValues(i,&missingFormat,&v1, &v2, &v3);
                    }
                }

                if(IsXDriven()){
                  StopSpss();
                }
            }
       \endcode
       \return The return code. \n
               0=No error
    */
    SPSSXD_API int GetVarNMissingValues(const int index,int *missingFormat,double *v1, double *v2, double *v3);

    /** Defines a connection from the SMB to an iterator over the currently active datasource.
        There can only be one cursor at a time. It is the caller's responsibility to ensure this
        condition.
        \code
        int errLevel = 0;
        errLevel = StartSpss();

        char *cmd1 = "GET FILE='Employee data.sav'.";
        errLevel = Submit(cmd1,strlen(cmd1));

        int hasCursor = 0;
        errLevel = HasCursor(hasCursor);

        errLevel = MakeCaseCursor("w");

        errLevel = HasCursor(hasCursor);

        int varCount = GetVariableCount(errLevel);

        int *varType = new int[varCount];
        for(int i = 0; i < varCount; i++) {
            varType[i] = GetVariableType(i,errLevel);
        }

        int rowCount = GetRowCount(errLevel);
        int curPos = 0;
        double dResult;
        char *cResult;
        int isMissing;
        for(int i = 0; i < rowCount; i++) {
            errLevel = NextCase();

            errLevel = GetCursorPosition(curPos);
            std::cout << "cursor position is " << curPos << std::endl;

            for(int j = 0; j < varCount; j++) {
                if(varType[j] == 0) {
                    errLevel = GetNumericValue(j,dResult,isMissing);
                    std::cout << dResult << "    ";
                } else {
                    cResult = new char[varType[j] + 1];
                    memset(cResult,'\0',varType[j] + 1);
                    errLevel = GetStringValue(j,cResult,varType[j] + 1,isMissing);
                    std::cout << cResult << "    ";
                }
            }
            std::cout << std::endl;
        }

        errLevel = RemoveCaseCursor();

        if(IsXDriven()){
          StopSpss();
        }
        \endcode
        \return The return code. \n
                0=No error \n
                9=No data source \n
                17=IBM SPSS Statistics backend is not ready
    */
    SPSSXD_API int  MakeCaseCursor(const char *accessType = "R");

    /** Checks whether there is a running cursor. o=no running cursor, 1=there is a running cursor.
        \sa
            MakeCaseCursor
        \return The return code. \n
                0=No error
    */
    SPSSXD_API int HasCursor(int &hasCur);

/**  Creates a pivot table structure.
     *
     * \param outLine   The outline title for the pivot table.
     * \param title     The title for the pivot table.
     * \param templateName  The OMS table subtype for the pivot table.
     * \param isSplit   Indicates whether or not to enable split file processing for the pivot table.
     *
       \code
          int err = 0;
          if(!IsBackendReady()){
             err = StartSpss();
          }
          if( 0 == err ){
             const char *cmd = "Get file = 'demo.sav'.";
             Submit(cmd, strlen(cmd));
             StartProcedure("proc1");
             StartPivotTable("outline","title","mytitle",false);
             EndProcedure();
          }
           if(IsXDriven()){
             StopSpss();
           }
       \endcode
       \return          The return code. \n
                        0=No error \n
                        17=IBM SPSS Statistics backend is not ready \n
                        65=No procedure
*/
  SPSSXD_API int StartPivotTable(const char* outLine,
                                  const char* title,
                                  const char* templateName,
                                  bool isSplit);

/**  Adds outline footnotes for the pivot table. This function has been deprecated.
     *
     * \param outLine   The outline title for the pivot table.
     * \param title     The title for the pivot table.
     * \param templateName  The OMS table subtype for the pivot table.
     * \param footnotes The outline footnotes for the pivot table.
     * \param isSplit   Indicates whether or not to enable split file processing for the pivot table.
     *
       \code
          int err = 0;
          if(!IsBackendReady()){
             err = StartSpss();
          }
          if( 0 == err ){
             const char *cmd = "Get file = 'demo.sav'.";
             Submit(cmd, strlen(cmd));
             StartProcedure("proc1");
             StartPivotTable("outline","title","mytitle",false);
             AddOutlineFootnotes("outline","title","mytitle","myfootnotes",false);
             EndProcedure();
          }
          if(IsXDriven()){
            StopSpss();
          }
       \endcode
       \return          The return code. \n
                        0=No error \n
                        17=IBM SPSS Statistics backend is not ready \n
                        65=No procedure
*/
  SPSSXD_API int AddOutlineFootnotes(const char* outLine,
                                  const char* title,
                                  const char* templateName,
                                  const char *footnotes,
                                  bool isSplit);

/**  Adds string footnotes for the procedure.
     *
     * \param footnotes The string footnotes for the procedure.
     *
       \code
          int err = 0;
          if(!IsBackendReady()){
             err = StartSpss();
          }
          if( 0 == err ){
             const char *cmd = "Get file = 'demo.sav'.";
             Submit(cmd, strlen(cmd));
             StartProcedure("proc1");
             AddProcFootnotes("myfootnotes");
             EndProcedure();
          }
          if(IsXDriven()){
            StopSpss();
          }
       \endcode
       \return          The return code. \n
                        0=No error \n
                        17=IBM SPSS Statistics backend is not ready \n
                        65=No procedure
*/
  SPSSXD_API int AddProcFootnotes(const char *footnotes);

/**  Adds title footnotes for the pivot table.
     *
     * \param outLine   The outline title for the pivot table.
     * \param title     The title for the pivot table.
     * \param templateName  The OMS table subtype for the pivot table.
     * \param footnotes The title footnotes for the pivot table.
     * \param isSplit   Indicates whether or not to enable split file processing for the pivot table.
     *
       \code
          int err = 0;
          if(!IsBackendReady()){
             err = StartSpss();
          }
          if( 0 == err ){
             const char *cmd = "Get file = 'demo.sav'.";
             Submit(cmd, strlen(cmd));
             StartProcedure("proc1");
             StartPivotTable("outline","title","mytitle",false);
             AddTitleFootnotes("outline","title","mytitle","myfootnotes",false);
             EndProcedure();
          }
          if(IsXDriven()){
            StopSpss();
          }
       \endcode
       \return          The return code. \n
                        0=No error \n
                        17=IBM SPSS Statistics backend is not ready \n
                        65=No procedure
*/
  SPSSXD_API int AddTitleFootnotes(const char* outLine,
                                  const char* title,
                                  const char* templateName,
                                  const char *footnotes,
                                  bool isSplit);

/**  Hides the title of the pivot table.
     *
     * \param outLine   The outline title for the pivot table.
     * \param title     The title for the pivot table.
     * \param templateName  The OMS table subtype for the pivot table.
     * \param isSplit   Indicates whether or not to enable split file processing for the pivot table.
     *
       \code
          int err = 0;
          if(!IsBackendReady()){
             err = StartSpss();
          }
          if( 0 == err ){
             const char *cmd = "Get file = 'demo.sav'.";
             Submit(cmd, strlen(cmd));
             StartProcedure("proc1");
             StartPivotTable("outline","title","mytitle",false);
             HidePivotTableTitle("outline","title","mytitle",false);
             EndProcedure();
          }
          if(IsXDriven()){
            StopSpss();
          }
       \endcode
       \return          The return code. \n
                        0=No error \n
                        17=IBM SPSS Statistics backend is not ready \n
                        65=No procedure
*/
  SPSSXD_API int HidePivotTableTitle(const char* outLine,
                                      const char* title,
                                      const char* templateName,
                                      bool isSplit);

/**  Adds a caption to the pivot table.
     *
     * \param outLine   The outline title for the pivot table.
     * \param title     The title for the pivot table.
     * \param templateName  The OMS table subtype for the pivot table.
     * \param isSplit   Indicates whether or not to enable split file processing for the pivot table.
     * \param caption   The caption of the pivot table.
     *
       \code
          int err = 0;
          if(!IsBackendReady()){
             err = StartSpss();
          }
          if( 0 == err ){
             const char *cmd = "Get file = 'demo.sav'.";
             Submit(cmd, strlen(cmd));
             StartProcedure("proc1");
             StartPivotTable("outline","title","mytitle",false);
             PivotTableCaption("outline","title","mytitle",false,"my caption");
             EndProcedure();
          }
          if(IsXDriven()){
            StopSpss();
          }
       \endcode
       \return          The return code. \n
                        0=No error \n
                        17=IBM SPSS Statistics backend is not ready \n
                        65=No procedure
*/
  SPSSXD_API int PivotTableCaption(const char* outLine,
                                    const char* title,
                                    const char* templateName,
                                    bool isSplit,
                                    const char* caption);

/**  Adds caption footnotes for the pivot table.
     *
     * \param outLine   The outline title for the pivot table.
     * \param title     The title for the pivot table.
     * \param templateName  The OMS table subtype for the pivot table.
     * \param isSplit   Indicates whether or not to enable split file processing for the pivot table.
     * \param caption   The caption of the pivot table.
     * \param footnotes The caption footnotes.
     *
       \code
          int err = 0;
          if(!IsBackendReady()){
             err = StartSpss();
          }
          if( 0 == err ){
             const char *cmd = "Get file = 'demo.sav'.";
             Submit(cmd, strlen(cmd));
             StartProcedure("proc1");
             StartPivotTable("outline","title","mytitle",false);
             PivotTableCaption("outline","title","mytitle",false,"my caption");
             AddCaptionFootnotes("outline","title","mytitle",false,"my caption","myfootnotes");
             EndProcedure();
          }
          if(IsXDriven()){
            StopSpss();
          }
       \endcode
       \return          The return code. \n
                        0=No error \n
                        17=IBM SPSS Statistics backend is not ready \n
                        65=No procedure
*/
  SPSSXD_API int AddCaptionFootnotes(const char* outLine,
                                    const char* title,
                                    const char* templateName,
                                    bool isSplit,
                                    const char* caption,
                                    const char *footnotes);

/**  Sets the minimum column width for the pivot table.
     *
     * \param outLine   The outline title for the pivot table.
     * \param title     The title for the pivot table.
     * \param templateName  The OMS table subtype for the pivot table.
     * \param isSplit   Indicates whether or not to enable split file processing for the pivot table.
     * \param nMinInPoints  The minimum column width.
     *
       \code
          int err = 0;
          if(!IsBackendReady()){
             err = StartSpss();
          }
          if( 0 == err ){
             const char *cmd = "Get file = 'demo.sav'.";
             Submit(cmd, strlen(cmd));
             StartProcedure("proc1");
             StartPivotTable("outline","title","mytitle",false);
             MinDataColumnWidth("outline","title","mytitle",false,10);
             EndProcedure();
          }
          if(IsXDriven()){
            StopSpss();
          }
       \endcode
       \return          The return code. \n
                        0=No error \n
                        17=IBM SPSS Statistics backend is not ready \n
                        65=No procedure
*/
  SPSSXD_API int  MinDataColumnWidth(const char* outLine,
                                     const char* title,
                                     const char* templateName,
                                     bool isSplit,
                                     int nMinInPoints);

/**  Sets the maximum column width for the pivot table.
     *
     * \param outLine   The outline title for the pivot table.
     * \param title     The title for the pivot table.
     * \param templateName  The OMS table subtype for the pivot table.
     * \param isSplit   Indicates whether or not to enable split file processing for the pivot table.
     * \param nMaxInPoints  The maximum column width.
     *
       \code
          int err = 0;
          if(!IsBackendReady()){
             err = StartSpss();
          }
          if( 0 == err ){
             const char *cmd = "Get file = 'demo.sav'.";
             Submit(cmd, strlen(cmd));
             StartProcedure("proc1");
             StartPivotTable("outline","title","mytitle",false);
             MaxDataColumnWidth("outline","title","mytitle",false,20);
             EndProcedure();
          }
          if(IsXDriven()){
            StopSpss();
          }
       \endcode
       \return          The return code. \n
                        0=No error \n
                        17=IBM SPSS Statistics backend is not ready \n
                        65=No procedure
*/
  SPSSXD_API int MaxDataColumnWidth(const char* outLine,
                                     const char* title,
                                     const char* templateName,
                                     bool isSplit,
                                     int nMaxInPoints);

/**  Add a dimension to the pivot table.
     *
     * \param outLine   The outline title for the pivot table.
     * \param title     The title for the pivot table.
     * \param templateName  The OMS table subtype for the pivot table.
     * \param isSplit   Indicates whether or not to enable split file processing for the pivot table.
     * \param dimName   The name of the dimension.
     * \param place     The placement of the dimension. \n
                            0 = row \n
                            1 = column \n
                            2 = layer 
     * \param position  The position of the dimension. Starts from 1. The lowest number is the inner dimension.
     * \param hideName  Specifies whether the dimension name is hidden.
     * \param hideLabels    specifies whether the dimension labels are hidden.
     *
       \code
          int err = 0;
          if(!IsBackendReady()){
             err = StartSpss();
          }
          if( 0 == err ){
             const char *cmd = "Get file = 'demo.sav'.";
             Submit(cmd, strlen(cmd));
             StartProcedure("proc1");
             StartPivotTable("outline","title","mytitle",false);
             AddDimension("outline","title","mytitle",false,"rowdim",0,1,false,false);
             AddDimension("outline","title","mytitle",false,"coldim",1,1,false,false);
             EndProcedure();
          }
          if(IsXDriven()){
            StopSpss();
          }
       \endcode
       \return          The return code. \n
                        0=No error \n
                        17=IBM SPSS Statistics backend is not ready \n
                        65=No procedure
*/
  SPSSXD_API int  AddDimension(const char* outLine,
                               const char* title,
                               const char* templateName,
                               bool isSplit,
                               const char* dimName,
                               int place,
                               int position,
                               bool hideName,
                               bool hideLabels);

/**  Add dimension footnotes to the pivot table.
     *
     * \param outLine   The outline title for the pivot table.
     * \param title     The title for the pivot table.
     * \param templateName  The OMS table subtype for the pivot table.
     * \param isSplit   Indicates whether or not to enable split file processing for the pivot table.
     * \param dimName   The name of the dimension.
     * \param place     The placement of the dimension. \n
                            0 = row \n
                            1 = column \n
                            2 = layer
     * \param position  The position of the dimension. Starts from 1. The lowest number is the inner dimension.
     * \param hideName  Specifies whether the dimension name is hidden.
     * \param hideLabels    specifies whether the dimension labels are hidden.
     * \param footnotes     The dimension footnotes.
     *
       \code
          int err = 0;
          if(!IsBackendReady()){
             err = StartSpss();
          }
          if( 0 == err ){
             const char *cmd = "Get file = 'demo.sav'.";
             Submit(cmd, strlen(cmd));
             StartProcedure("proc1");
             StartPivotTable("outline","title","mytitle",false);
             AddDimension("outline","title","mytitle",false,"rowdim",0,1,false,false);
             AddDimension("outline","title","mytitle",false,"coldim",1,1,false,false);
             AddDimFootnotes("outline","title","mytitle",false,"coldim",1,1,false,false,"myfootnotes");
             EndProcedure();
          }
          if(IsXDriven()){
            StopSpss();
          }
       \endcode
       \return          The return code. \n
                        0=No error \n
                        17=IBM SPSS Statistics backend is not ready \n
                        65=No procedure
*/
  SPSSXD_API int  AddDimFootnotes(const char* outLine,
                               const char* title,
                               const char* templateName,
                               bool isSplit,
                               const char* dimName,
                               int place,
                               int position,
                               bool hideName,
                               bool hideLabels,
                               const char *footnotes);

/**  Sets the numeric formatSpec to Coefficient.
     *
       \code
          SetFormatSpecCoefficient();
       \endcode
       \sa
          SetNumberCell AddNumberCategory
       \return          The return code. \n
                        0=No error \n
                        17=IBM SPSS Statistics backend is not ready
*/
  SPSSXD_API int SetFormatSpecCoefficient();

/**  Sets the numeric formatSpec to CoefficientSE.
     *
       \code
          SetFormatSpecCoefficientSE();
       \endcode
       \sa
          SetNumberCell AddNumberCategory
       \return          The return code. \n
                        0=No error \n
                        17=IBM SPSS Statistics backend is not ready
*/
  SPSSXD_API int SetFormatSpecCoefficientSE();

/**  Sets the numeric formatSpec to CoefficientVar.
     *
       \code
          SetFormatSpecCoefficientVar();
       \endcode
       \sa
          SetNumberCell AddNumberCategory
       \return          The return code. \n
                        0=No error \n
                        17=IBM SPSS Statistics backend is not ready
*/
  SPSSXD_API int SetFormatSpecCoefficientVar();

/**  Sets the numeric formatSpec to Correlation.
     *
       \code
          SetFormatSpecCorrelation();
       \endcode
       \sa
          SetNumberCell AddNumberCategory
       \return          The return code. \n
                        0=No error \n
                        17=IBM SPSS Statistics backend is not ready
*/
  SPSSXD_API int SetFormatSpecCorrelation();

/**  Sets the numeric formatSpec to GeneralStat.
     *
       \code
          SetFormatSpecGeneralStat();
       \endcode
       \sa
          SetNumberCell AddNumberCategory
       \return          The return code. \n
                        0= No error \n
                        17=IBM SPSS Statistics backend is not ready
*/
  SPSSXD_API int SetFormatSpecGeneralStat();

/**  Sets the numeric formatSpec to Mean.
     *
     * \param varIndex  The index of a variable whose format is used to determine details of the Mean format
       \code
          SetFormatSpecMean(0);
       \endcode
       \sa
          SetNumberCell AddNumberCategory
       \return          The return code. \n
                        0=No error \n
                        17=IBM SPSS Statistics backend is not ready
*/
  SPSSXD_API int SetFormatSpecMean(int varIndex);

/**  Sets the numeric formatSpec to Count.
     *
       \code
          SetFormatSpecCount();
       \endcode
       \sa
          SetNumberCell AddNumberCategory
       \return          The return code. \n
                        0=No error \n
                        17=IBM SPSS Statistics backend is not ready
*/
  SPSSXD_API int SetFormatSpecCount();

/**  Sets the numeric formatSpec to Percent.
     *
       \code
          SetFormatSpecPercent();
       \endcode
       \sa
          SetNumberCell AddNumberCategory
       \return          The return code. \n
                        0=No error \n
                        17=IBM SPSS Statistics backend is not ready
*/
  SPSSXD_API int SetFormatSpecPercent();

/**  Sets the numeric formatSpec to PercentNoSign.
     *
       \code
          SetFormatSpecPercentNoSign();
       \endcode
       \sa
          SetNumberCell AddNumberCategory
       \return          The return code. \n
                        0=No error \n
                        17=IBM SPSS Statistics backend is not ready
*/
  SPSSXD_API int SetFormatSpecPercentNoSign();

/**  Sets the numeric formatSpec to Proportion.
     *
       \code
          SetFormatSpecProportion();
       \endcode
       \sa
          SetNumberCell AddNumberCategory
       \return          The return code. \n
                        0=No error \n
                        17=IBM SPSS Statistics backend is not ready
*/
  SPSSXD_API int SetFormatSpecProportion();

/**  Sets the numeric formatSpec to Significance.
     *
       \code
          SetFormatSpecSignificance();
       \endcode
       \sa
          SetNumberCell AddNumberCategory
       \return          The return code. \n
                        0=No error \n
                        17=IBM SPSS Statistics backend is not ready
*/
  SPSSXD_API int SetFormatSpecSignificance();

/**  Sets the numeric formatSpec to Residual.
     *
       \code
          SetFormatSpecResidual();
       \endcode
       \sa
          SetNumberCell AddNumberCategory
       \return          The return code. \n
                        0=No error \n
                        17=IBM SPSS Statistics backend is not ready
*/
  SPSSXD_API int SetFormatSpecResidual();

/**  Sets the numeric formatSpec to Variable.
     *
     * \param varIndex  The index of a variable whose format is used to determine details of the Variable format.
       \code
          SetFormatSpecVariable(0);
       \endcode
       \sa
          SetNumberCell AddNumberCategory
       \return          The return code. \n
                        0=No error \n
                        17=IBM SPSS Statistics backend is not ready
*/
  SPSSXD_API int SetFormatSpecVariable(int varIndex);

/**  Sets the numeric formatSpec to StdDev.
     *
     * \param varIndex  The index of a variable whose format is used to determine details of the StdDev format
       \code
          SetFormatSpecStdDev(0);
       \endcode
       \sa
          SetNumberCell AddNumberCategory
       \return          The return code. \n
                        0=No error \n
                        17=IBM SPSS Statistics backend is not ready
*/
  SPSSXD_API int SetFormatSpecStdDev(int varIndex);

/**  Sets the numeric formatSpec to Difference.
     *
     * \param varIndex  The index of a variable whose format is used to determine details of the Sum format
       \code
          SetFormatSpecDifference(0);
       \endcode
       \sa
          SetNumberCell AddNumberCategory
       \return          The return code. \n
                        0=No error \n
                        17=IBM SPSS Statistics backend is not ready
*/
  SPSSXD_API int SetFormatSpecDifference(int varIndex);

/**  Sets the numeric formatSpec to Sum.
     *
     * \param varIndex  The index of a variable whose format is used to determine details of the Sum format
       \code
          SetFormatSpecSum(0);
       \endcode
       \sa
          SetNumberCell AddNumberCategory
       \return          The return code. \n
                        0=No error \n
                        17=IBM SPSS Statistics backend is not ready
*/
  SPSSXD_API int SetFormatSpecSum(int varIndex);

/**  Add a numeric category to the pivot table.
     A SetFormatSpec... function must be called prior to calling this function.
     *
     * \param outLine   The outline title for the pivot table.
     * \param title     The title for the pivot table.
     * \param templateName  The OMS table subtype for the pivot table.
     * \param isSplit   Indicates whether or not to enable split file processing for the pivot table.
     * \param dimName   The name of the dimension.
     * \param place     The placement of the dimension. \n
                            0 = row \n
                            1 = column \n
                            2 = layer
     * \param position  The position of the dimension. Starts from 1. The lowest number is the inner dimension.
     * \param hideName  Specifies whether the dimension name is hidden.
     * \param hideLabels    Specifies whether the dimension labels are hidden.
     * \param category  The category name.
                        A double value with special formatSpec(set by a SetFormatSpec... function ).
     *
       \code
          int err = 0;
          if(!IsBackendReady()){
             err = StartSpss();
          }
          if( 0 == err ){
             const char *cmd = "Get file = 'demo.sav'.";
             Submit(cmd, strlen(cmd));

             StartProcedure("proc1");
             StartPivotTable("outline","title","mytitle",false);
             AddDimension("outline","title","mytitle",false,"rowdim",0,1,false,false);
             AddDimension("outline","title","mytitle",false,"coldim",1,1,false,false);
             SetFormatSpecCount();
             AddNumberCategory("outline","title","mytitle",false,"rowdim",0,1,false,false,2);
             AddStringCategory("outline","title","mytitle",false,"coldim",1,1,false,false,"a");
             SetStringCell("outline","title","mytitle",false,"coldim",1,1,false,false,"b");
             EndProcedure();
          }
          if(IsXDriven()){
            StopSpss();
          }
       \endcode
       \return          The return code. \n
                        0=No error \n
                        17=IBM SPSS Statistics backend is not ready \n
                        65=No procedure                       
*/
  SPSSXD_API int AddNumberCategory(const char* outLine,
                                   const char* title,
                                   const char* templateName,
                                   bool isSplit,
                                   const char* dimName,
                                   int place,
                                   int position,
                                   bool hideName,
                                   bool hideLabels,
                                   double category);

/**  Add a string category to the pivot table.
     The string will not be translated.
     *
     * \param outLine   The outline title for the pivot table.
     * \param title     The title for the pivot table.
     * \param templateName  The OMS table subtype for the pivot table.
     * \param isSplit   Indicates whether or not to enable split file processing for the pivot table.
     * \param dimName   The name of the dimension.
     * \param place     The placement of the dimension. \n
                            0 = row \n
                            1 = column \n
                            2 = layer
     * \param position  The position of the dimension. Starts from 1. The lowest number is the inner dimension.
     * \param hideName  Specifies whether the dimension name is hidden.
     * \param hideLabels    Specifies whether the dimension labels are hidden.
     * \param category  The category name.
     *
       \sa
          AddNumberCategory
       \return          The return code. \n
                        0=No error \n
                        17=IBM SPSS Statistics backend is not ready \n
                        65=No procedure 
*/
  SPSSXD_API int AddStringCategory(const char* outLine,
                                   const char* title,
                                   const char* templateName,
                                   bool isSplit,
                                   const char* dimName,
                                   int place,
                                   int position,
                                   bool hideName,
                                   bool hideLabels,
                                   const char* category);

/**  Add a variable name category to the pivot table. The category name will be the variable name or label.
     *
     * \param outLine   The outline title for the pivot table.
     * \param title     The title for the pivot table.
     * \param templateName  The OMS table subtype for the pivot table.
     * \param isSplit   Indicates whether or not to enable split file processing for the pivot table.
     * \param dimName   The name of the dimension.
     * \param place     The placement of the dimension. \n
                            0 = row \n
                            1 = column \n
                            2 = layer
     * \param position  The position of the dimension. Starts from 1. The lowest number is the inner dimension.
     * \param hideName  Specifies whether the dimension name is hidden.
     * \param hideLabels    Specifies whether the dimension labels are hidden.
     * \param category  A variable index. The category name will be indexed variable's name or label.
     *
       \code
          int err = 0;
          if(!IsBackendReady()){
             err = StartSpss();
          }
          if( 0 == err ){
             const char *cmd = "Get file = 'Employee data.sav'.";
             Submit(cmd, strlen(cmd));

             StartProcedure("proc1");
             StartPivotTable("outline","title","mytitle",false);
             AddDimension("outline","title","mytitle",false,"rowdim",0,1,false,false);
             AddDimension("outline","title","mytitle",false,"coldim",1,1,false,false);
             AddVarNameCategory("outline","title","mytitle",false,"rowdim",0,1,false,false,1);
             AddVarValueDoubleCategory("outline","title","mytitle",false,"coldim",1,1,false,false,3,2);
             AddVarValueStringCategory("outline","title","mytitle",false,"coldim",1,1,false,false,1,"f");
             EndProcedure();
          }
          if(IsXDriven()){
            StopSpss();
          }
       \endcode
       \return          The return code. \n
                        0=No error \n
                        9=No data source \n
                        10=Invalid index \n
                        17=IBM SPSS Statistics backend is not ready \n
                        65=No procedure
*/
  SPSSXD_API int AddVarNameCategory(const char* outLine,
                                    const char* title,
                                    const char* templateName,
                                    bool isSplit,
                                    const char* dimName,
                                    int place,
                                    int position,
                                    bool hideName,
                                    bool hideLabels,
                                    int category);

/**  Add a numeric variable value category to the pivot table.
     The category name will be the variable value or value label.
     *
     * \param outLine   The outline title for the pivot table.
     * \param title     The title for the pivot table.
     * \param templateName  The OMS table subtype for the pivot table.
     * \param isSplit   Indicates whether or not to enable split file processing for the pivot table.
     * \param dimName   The name of the dimension.
     * \param place     The placement of the dimension. \n
                            0 = row \n
                            1 = column \n
                            2 = layer
     * \param position  The position of the dimension. Starts from 1. The lowest number is the inner dimension.
     * \param hideName  Specifies whether the dimension name is hidden.
     * \param hideLabels    Specifies whether the dimension labels are hidden.
     * \param category  A variable index. The category name will be indexed variable's name or label.
     * \param d         A double value. It should be one of the values of the variable.
     *
       \sa
          AddVarNameCategory
       \return          The return code. \n
                        0=No error \n
                        9=No data source \n
                        10=Invalid index \n
                        17=IBM SPSS Statistics backend is not ready \n
                        65=No procedure
*/
  SPSSXD_API int AddVarValueDoubleCategory(const char* outLine,
                                           const char* title,
                                           const char* templateName,
                                           bool isSplit,
                                           const char* dimName,
                                           int place,
                                           int position,
                                           bool hideName,
                                           bool hideLabels,
                                           int category,
                                           double d);

/**  Add a string variable value category to the pivot table.
     The category name will be the variable value or value label.
     *
     * \param outLine   The outline title for the pivot table.
     * \param title     The title for the pivot table.
     * \param templateName  The OMS table subtype for the pivot table.
     * \param isSplit   Indicates whether or not to enable split file processing for the pivot table.
     * \param dimName   The name of the dimension.
     * \param place     The placement of the dimension. \n
                            0 = row \n
                            1 = column \n
                            2 = layer
     * \param position  The position of the dimension. Starts from 1. The lowest number is the inner dimension.
     * \param hideName  Specifies whether the dimension name is hidden.
     * \param hideLabels    Specifies whether the dimension labels are hidden.
     * \param category  A variable index. The category name will be indexed variable's name or label.
     * \param ch        A string. It should be one of the values of the variable.
     *
       \sa
          AddVarNameCategory
       \return          The return code. \n
                        0=No error \n
                        9=No data source \n
                        10=Invalid index \n
                        17=IBM SPSS Statistics backend is not ready \n
                        65=No procedure
*/
  SPSSXD_API int AddVarValueStringCategory(const char* outLine,
                                           const char* title,
                                           const char* templateName,
                                           bool isSplit,
                                           const char* dimName,
                                           int place,
                                           int position,
                                           bool hideName,
                                           bool hideLabels,
                                           int category,
                                           const char* ch);

/**  Add string footnotes to the current category for the pivot table.
     *
     * \param outLine   The outline title for the pivot table.
     * \param title     The title for the pivot table.
     * \param templateName  The OMS table subtype for the pivot table.
     * \param isSplit   Indicates whether or not to enable split file processing for the pivot table.
     * \param dimName   The name of the dimension.
     * \param place     The placement of the dimension. \n
                            0 = row \n
                            1 = column \n
                            2 = layer
     * \param position  The position of the dimension. Starts from 1. The lowest number is the inner dimension.
     * \param hideName  Specifies whether the dimension name is hidden.
     * \param hideLabels    Specifies whether the dimension labels are hidden.
     * \param footnotes     The string footnotes for the current category.
     *
       \code
          int err = 0;
          if(!IsBackendReady()){
             err = StartSpss();
          }
          if( 0 == err ){
             const char *cmd = "Get file = 'demo.sav'.";
             Submit(cmd, strlen(cmd));

             StartProcedure("proc1");
             StartPivotTable("outline","title","mytitle",false);
             AddDimension("outline","title","mytitle",false,"rowdim",0,1,false,false);
             AddDimension("outline","title","mytitle",false,"coldim",1,1,false,false);
             SetFormatSpecCount();
             AddNumberCategory("outline","title","mytitle",false,"rowdim",0,1,false,false,2);
             AddStringCategory("outline","title","mytitle",false,"coldim",1,1,false,false,"a");
             AddCategoryFootnotes("outline","title","mytitle",false,"coldim",1,1,false,false,"myfootnotes");
             SetStringCell("outline","title","mytitle",false,"coldim",1,1,false,false,"b");
             EndProcedure();
          }
          if(IsXDriven()){
            StopSpss();
          }
       \endcode
       \return          The return code. \n
                        0=No error \n
                        17=IBM SPSS Statistics backend is not ready \n
                        65=No procedure
*/
  SPSSXD_API int  AddCategoryFootnotes(const char* outLine,
                                       const char* title,
                                       const char* templateName,
                                       bool isSplit,
                                       const char* dimName,
                                       int place,
                                       int position,
                                       bool hideName,
                                       bool hideLabels,
                                       const char* footnotes);

/**  Adds a numeric cell to the pivot table. One of the SetFormatSpec... functions must be called
     prior to calling this function.. The category must be added before calling this function.
     *
     * \param outLine   The outline title for the pivot table.
     * \param title     The title for the pivot table.
     * \param templateName  The OMS table subtype for the pivot table.
     * \param isSplit   Indicates whether or not to enable split file processing for the pivot table.
     * \param dimName   The name of the dimension.
     * \param place     The placement of the dimension. \n
                            0 = row \n
                            1 = column \n
                            2 = layer
     * \param position  The position of the dimension. Starts from 1. The lowest number is the inner dimension.
     * \param hideName  Specifies whether the dimension name is hidden.
     * \param hideLabels    Specifies whether the dimension labels are hidden.
     * \param cellVal   The cell value.
                        A double value with special formatSpec(set by one of SetFormatSpec... functions ).
     *
       \code
          int err = 0;
          if(!IsBackendReady()){
             err = StartSpss();
          }
          if( 0 == err ){
             const char *cmd = "Get file = 'Employee data.sav'.";
             Submit(cmd, strlen(cmd));

             StartProcedure("proc1");
             StartPivotTable("outline","title","mytitle",false);

             AddDimension("outline","title","mytitle",false,"rowdim",0,1,false,false);
             AddDimension("outline","title","mytitle",false,"coldim",1,1,false,false);

             AddStringCategory("outline","title","mytitle",false,"rowdim",0,1,false,false,"1");

             AddStringCategory("outline","title","mytitle",false,"coldim",1,1,false,false,"cat-1");
             SetFormatSpecCount();
             SetNumberCell("outline","title","mytitle",false,"coldim",1,1,false,false,1);

             AddStringCategory("outline","title","mytitle",false,"coldim",1,1,false,false,"cat-2");
             SetStringCell("outline","title","mytitle",false,"coldim",1,1,false,false,"cell-2");
             
             AddStringCategory("outline","title","mytitle",false,"coldim",1,1,false,false,"cat-3");
             SetDateCell("outline","title","mytitle",false,"coldim",1,1,false,false,"15-03-2009 00:00:00");

             AddStringCategory("outline","title","mytitle",false,"coldim",1,1,false,false,"varName");
             SetVarNameCell("outline","title","mytitle",false,"coldim",1,1,false,false,1);

             AddStringCategory("outline","title","mytitle",false,"coldim",1,1,false,false,"Is Female?");
             SetVarValueDoubleCell("outline","title","mytitle",false,"coldim",1,1,false,false,3,1);

             AddStringCategory("outline","title","mytitle",false,"coldim",1,1,false,false,"Is Male?");
             SetVarValueStringCell("outline","title","mytitle",false,"coldim",1,1,false,false,1,"m");

             EndProcedure();
          }
          if(IsXDriven()){
            StopSpss();
          }
       \endcode
       \return          The return code. \n
                        0=No error \n
                        17=IBM SPSS Statistics backend is not ready. \n
                        65=No procedure
*/
  SPSSXD_API int SetNumberCell(const char* outLine,
                               const char* title,
                               const char* templateName,
                               bool isSplit,
                               const char* dimName,
                               int place,
                               int position,
                               bool hideName,
                               bool hideLabels,
                               double cellVal);

/**  Adds a string cell to the pivot table. The string will not be translated.
     The category must be added before calling this function.
     *
     * \param outLine   The outline title for the pivot table.
     * \param title     The title for the pivot table.
     * \param templateName  The OMS table subtype for the pivot table.
     * \param isSplit   Indicates whether or not to enable split file processing for the pivot table.
     * \param dimName   The name of the dimension.
     * \param place     The placement of the dimension. \n
                            0 = row \n
                            1 = column \n
                            2 = layer
     * \param position  The position of the dimension. Starts from 1. The lowest number is the inner dimension.
     * \param hideName  Specifies whether the dimension name is hidden.
     * \param hideLabels    Specifies whether the dimension labels are hidden.
     * \param cellVal   The cell value.
     *
       \sa
          SetNumberCell
       \return          The return code. \n
                        0=No error \n
                        17=IBM SPSS Statistics backend is not ready. \n
                        65=No procedure
*/
  SPSSXD_API int SetStringCell(const char* outLine,
                               const char* title,
                               const char* templateName,
                               bool isSplit,
                               const char* dimName,
                               int place,
                               int position,
                               bool hideName,
                               bool hideLabels,
                               const char* cellVal);

/**  Adds a variable name cell to the pivot table. The cell value will be the variable name or label.
     The category must be added before calling this function.
     *
     * \param outLine   The outline title for the pivot table.
     * \param title     The title for the pivot table.
     * \param templateName  The OMS table subtype for the pivot table.
     * \param isSplit   Indicates whether or not to enable split file processing for the pivot table.
     * \param dimName   The name of the dimension.
     * \param place     The placement of the dimension. \n
                            0 = row \n
                            1 = column \n
                            2 = layer
     * \param position  The position of the dimension. Starts from 1. The lowest number is the inner dimension.
     * \param hideName  Specifies whether the dimension name is hidden.
     * \param hideLabels    Specifies whether the dimension labels are hidden.
     * \param cellVal   A variable index. The cell value will be the variable name or label.
     *
       \sa
          SetNumberCell
       \return          The return code. \n
                        0=No error \n
                        9=No data source \n
                        10=Invalid index \n
                        17=IBM SPSS Statistics backend is not ready \n
                        65=No procedure
*/
  SPSSXD_API int SetVarNameCell(const char* outLine,
                               const char* title,
                               const char* templateName,
                               bool isSplit,
                               const char* dimName,
                               int place,
                               int position,
                               bool hideName,
                               bool hideLabels,
                               int cellVal);

/**  Adds a numeric variable value cell to the pivot table. The cell value will be the variable value or value label.
     The category must be added before calling this function.
     *
     * \param outLine   The outline title for the pivot table.
     * \param title     The title for the pivot table.
     * \param templateName  The OMS table subtype for the pivot table.
     * \param isSplit   Indicates whether or not to enable split file processing for the pivot table.
     * \param dimName   The name of the dimension.
     * \param place     The placement of the dimension. \n
                            0 = row \n
                            1 = column \n
                            2 = layer
     * \param position  The position of the dimension. Starts from 1. The lowest number is the inner dimension.
     * \param hideName  Specifies whether the dimension name is hidden.
     * \param hideLabels    Specifies whether the dimension labels are hidden.
     * \param cellVal   A variable index. The cell value will be the variable value or value label.
     * \param d         A double value. It should be one of values of the variable.
     *
       \sa
          SetNumberCell
       \return          The return code. \n
                        0=No error \n
                        9=No data source \n
                        10=Invalid index \n
                        17=IBM SPSS Statistics backend is not ready \n
                        65=No procedure
*/
  SPSSXD_API int SetVarValueDoubleCell(const char* outLine,
                                       const char* title,
                                       const char* templateName,
                                       bool isSplit,
                                       const char* dimName,
                                       int place,
                                       int position,
                                       bool hideName,
                                       bool hideLabels,
                                       int cellVal,
                                       double d);

/**  Add a variable string value cell to the pivot table. The cell value will be the variable value or value label.
     The category must be added before calling this function.
     *
     * \param outLine   The outline title for the pivot table.
     * \param title     The title for the pivot table.
     * \param templateName  The OMS table subtype for the pivot table.
     * \param isSplit   Indicates whether or not to enable split file processing for the pivot table.
     * \param dimName   The name of the dimension.
     * \param place     The placement of the dimension. \n
                            0 = row \n
                            1 = column \n
                            2 = layer
     * \param position  The position of the dimension. Starts from 1. The lowest number is the inner dimension.
     * \param hideName  Specifies whether the dimension name is hidden.
     * \param hideLabels    Specifies whether the dimension labels are hidden.
     * \param cellVal   A variable index. The cell value will be the variable value or value label.
     * \param ch        A string. It should be one of value labels of the variable.
     *
       \sa
          SetNumberCell
       \return          The return code. \n
                        0=No error \n
                        9=No data source \n
                        10=Invalid index \n
                        17=IBM SPSS Statistics backend is not ready \n
                        65=No procedure
*/
  SPSSXD_API int SetVarValueStringCell(const char* outLine,
                                       const char* title,
                                       const char* templateName,
                                       bool isSplit,
                                       const char* dimName,
                                       int place,
                                       int position,
                                       bool hideName,
                                       bool hideLabels,
                                       int cellVal,
                                       const char* ch);

/**  Adds footnotes to a pivot table cell which is indicated by a list of categories.
     Categories must exist prior to calling this function.
     *
     * \param outLine   The outline title for the pivot table.
     * \param title     The title for the pivot table.
     * \param templateName  The OMS table subtype for the pivot table.
     * \param isSplit   Indicates whether or not to enable split file processing for the pivot table.
     * \param dimName   The name of the dimension.
     * \param place     The placement of the dimension. \n
                            0 = row \n
                            1 = column \n
                            2 = layer
     * \param position  The position of the dimension. Starts from 1. The lowest number is the inner dimension.
     * \param hideName  Specifies whether the dimension name is hidden.
     * \param hideLabels    Specifies whether the dimension labels are hidden.
     * \param footnotes The footnote text to be added.
     *
       \code
          int err = 0;
          if(!IsBackendReady()){
             err = StartSpss();
          }
          if( 0 == err ){
             const char *cmd = "Get file = 'demo.sav'.";
             Submit(cmd, strlen(cmd));

             StartProcedure("proc1");
             StartPivotTable("outline","title","mytitle",false);
             AddDimension("outline","title","mytitle",false,"rowdim",0,1,false,false);
             AddDimension("outline","title","mytitle",false,"coldim",1,1,false,false);
             SetFormatSpecCount();
             AddNumberCategory("outline","title","mytitle",false,"rowdim",0,1,false,false,2);
             AddStringCategory("outline","title","mytitle",false,"coldim",1,1,false,false,"a");
             SetStringCell("outline","title","mytitle",false,"coldim",1,1,false,false,"b");
             AddCellFootnotes("outline","title","mytitle",false,"coldim",1,1,false,false,"cell footnotes");
             EndProcedure();
          }
          if(IsXDriven()){
            StopSpss();
          }
       \endcode
       \return          The return code. \n
                        0=No error \n
                        9=No data source \n
                        10=Invalid index \n
                        17=IBM SPSS Statistics backend is not ready \n
                        65=No procedure
*/
  SPSSXD_API int  AddCellFootnotes(const char* outLine,
                                   const char* title,
                                   const char* templateName,
                                   bool isSplit,
                                   const char* dimName,
                                   int place,
                                   int position,
                                   bool hideName,
                                   bool hideLabels,
                                   const char* footnotes);

/**  Add a text block to the outline.
     *
     * \param outLine   The outline title for the pivot table.
     * \param name      The name for the text block.
     * \param line      The content of the text block.
     * \param nSkip     The number of new lines to skip.
     *
       \code
          int err = 0;
          if(!IsBackendReady()){
             err = StartSpss();
          }
          if( 0 == err ){
             const char *cmd = "Get file = 'demo.sav'.";
             Submit(cmd, strlen(cmd));

             StartProcedure("proc1");
             AddTextBlock("outline","test","hello");
             EndProcedure();
          }
          if(IsXDriven()){
            StopSpss();
          }
       \endcode
       \return          The return code. \n
                        0=No error \n
                        17=IBM SPSS Statistics backend is not ready \n
                        65=No procedure
*/
  SPSSXD_API int  AddTextBlock(const char* outLine,
                               const char* name,
                               const char* line,
                               int nSkip=1);

/**  Applies a split change. Use IsEndSplit to detect the end of a split and userSplitChange to start the new split.
     *
     * \param procName  The name of the procedure.
       \code
          int err = 0;
          if(!IsBackendReady()){
             err = StartSpss();
          }
          if( 0 == err ){
             const char *cmd = "Get file = 'demo.sav'.";
             Submit(cmd, strlen(cmd));

             StartProcedure("proc1");
             SplitChange("proc1");
             EndProcedure();
          }
          if(IsXDriven()){
            StopSpss();
          }
       \endcode
       \return          The return code. \n
                        0=No error \n
                        17=IBM SPSS Statistics backend is not ready \n
                        65=No procedure
*/
  SPSSXD_API int SplitChange(const char* procName);

  /**Retrieve the value of the line width of IBM SPSS Statistics output.
   *
   * \param errLevel    The error code returned from the IBM SPSS Statistics backend. 0 = no error.
   * 
     \code
          int err = 0;
          int spssOutputWidth = 0;
          if(!IsBackendReady()){
             err = StartSpss();
          }
          if( 0 == err ){
             const char *cmd = "Get file = 'demo.sav'.";
             Submit(cmd, strlen(cmd));

             spssOutputWidth = GetSpssOutputWidth(err);
          } 
          if(IsXDriven()){
            StopSpss();
          }
     \endcode
     \return          The value of the line width of IBM SPSS Statistics output.
   */
  SPSSXD_API int GetSpssOutputWidth(int& errLevel);
  
  /**Retrieve the list of the split variables names in a split sav file.
   *
   * \param errLevel    The error code returned from the IBM SPSS Statistics backend. \n
                        0=No error \n
                        9=No data source
   * 
     \code
          int err = 0;
          if(!IsBackendReady()){
             err = StartSpss();
          }
          if( 0 == err ){
             const char *cmd = "Get file = 'Employee data.sav'.";
             Submit(cmd, strlen(cmd));
             //Notes: gender must be one of categorical variables in Employee data.sav.
             Submit("SPLIT FILE SEPARATE BY gender.",30);
             void *result = GetSplitVariableNames(err);
             int len = GetStringListLength(result);
             if( len > 0 ){
                 const char* str = GetStringFromList(result,0);
             }
             RemoveStringList(result);
          }
          if(IsXDriven()){
            StopSpss();
          }
     \endcode
     \return          The value of the line width of IBM SPSS Statistics output.
   */
  SPSSXD_API void*  GetSplitVariableNames(int& errLevel);

  /** Starts a user procedure. There can only be one procedure at a time. After you finish the user procedure, you should end ,
      you should call the EndProcedure(...).
      \param procName the procedure name.
      \code
          int err = 0;
          if(!IsBackendReady()){
             err = StartSpss();
          }
          if( 0 == err ){
              StartProcedure("proc1");
              EndProcedure();
          }
          if(IsXDriven()){
            StopSpss();
          }
      \endcode
      \sa
         AddTextBlock
      \return The return code. \n
              0=No error \n
              17=IBM SPSS Statistics backend is not ready.
   */
  SPSSXD_API int StartProcedure(const char* procName, const char* translatedName = 0);


  /** Ends a user procedure. After you finish the user procedure, you should end it.
      *
      \sa
         AddTextBlock
      \return The return code. \n
              0=No error \n
              17=IBM SPSS Statistics backend is not ready.
   */
  SPSSXD_API int EndProcedure();

  /** Adds a new variable name and of the specified type into current active data source.
      \param varName variable name array, each item is a null-terminated variable name.
      \param varType variable type array, each type is a variable type 0 means numeric, a positive value means string.
      \param numOfVar number of variables.
      \code
          void func()
          {
              int errLevel = 0;
              errLevel = StartSpss();

              char *cmd1 = "GET FILE='demo.sav'.";
              errLevel = Submit(cmd1,strlen(cmd1));

              errLevel = MakeCaseCursor("w");

              char *varName[] = {"t1","t2"};
              int varType[] = {0,8};
              unsigned int numOfVar = 2;
              errLevel = SetVarNameAndType(varName,varType,numOfVar);

              char *varLabel_0 = "wwq1";
              errLevel = SetVarLabel(varName[0],varLabel_0) ;

              char *varLabel_1 = "wwq2";
              errLevel = SetVarLabel(varName[1],varLabel_1) ;

              double value_0 = 10.0;
              char *valueLable_0 = "aaa";
              errLevel = SetVarNValueLabel(varName[0],value_0,valueLable_0);

              char *value_1 = "wwq";
              char *valueLable_1 = "bbb";
              errLevel = SetVarCValueLabel(varName[1],value_1,valueLable_1);

              int missingFormat_0 = -3;
              double missingVal1_0 = 10.0; //lower limits
              double missingVal2_0 = 20.0; //upper limits
              double missingVal3_0 = 100.0; //discrete value
              errLevel = SetVarNMissingValues(varName[0],missingFormat_0,missingVal1_0,missingVal2_0,missingVal3_0);

              int missingFormat_1 = 3;
              char *missingVal1_1 = "abc"; //discrete value
              char *missingVal2_1 = "def"; //discrete value
              char *missingVal3_1 = "g"; //discrete value
              errLevel = SetVarCMissingValues(varName[1],missingFormat_1,missingVal1_1,missingVal2_1,missingVal3_1);

              int alignment_0 = 0; //left
              errLevel = SetVarAlignment(varName[0],alignment_0);

              int alignment_1 = 2; //right
              errLevel = SetVarAlignment(varName[1],alignment_1);

              int measureLevel_0 = 2; //nominal
              errLevel = SetVarMeasureLevel(varName[0],measureLevel_0);

              int measureLevel_1 = 3; //ordinal
              errLevel = SetVarMeasureLevel(varName[1],measureLevel_1);

              int formatType_0 = 5; //float
              int formatWid_0 = 8;
              int formatDec_0 = 2;
              errLevel = SetVarFormat(varName[0],formatType_0,formatWid_0,formatDec_0);
              int formatType_1 = 1; //String
              int formatWid_1 = 20;
              int formatDec_1 = 0;
              errLevel = SetVarFormat(varName[1],formatType_1,formatWid_1,formatDec_1);
              
              int varRole_0 = 2;
              errLevel = SetVarRole(varName[0],varRole_0);
              
              int varRole_1 = 3;
              errLevel = SetVarRole(varName[1],varRole_1);

              errLevel = CommitHeader();

              errLevel = RemoveCaseCursor();

              if(IsXDriven()){
                StopSpss();
              }
          }
      \endcode
      \return The return code. \n
              0=No error \n
              9=No data source \n
              17=IBM SPSS Statistics backend is not ready \n
              25=Duplicate variable name \n
              27=Invalid variable name \n
              55=Can't call this function again before calling RemoveCaseCursor() \n
              60=No cursor is running
  */
  SPSSXD_API int SetVarNameAndType( char *varName[],
                                    const int *varType,
                                    const unsigned int numOfVar);

  /** Sets the label of the named variable.
      \param varName a null-terminated variable name.
      \param varLabel a null-terminated variable label.
      \sa
           SetVarNameAndType
      \return The return code. \n
              0=No error \n
              9=No data source \n
              17=IBM SPSS Statistics backend is not ready \n
              27=Invalid variable name \n
              60=No cursor is running
  */
  SPSSXD_API int SetVarLabel(const char *varName,
                             const char *varLabel) ;

  /** Changes or adds a value label for the specified value of a short string
      variable. The label should be a null-terminated string not exceeding 60
      bytes in length.
      \param varName a null-terminated variable name.
      \param value the null-terminated value to be labeled.
      \param label the null-terminated label.
      \sa
           SetVarNameAndType
      \return The return code. \n
              0=No error \n
              9=No data source \n
              17=IBM SPSS Statistics backend is not ready \n
              27=Invalid variable name \n
              60=No cursor is running
   */
  SPSSXD_API int SetVarCValueLabel(const char *varName,
                                   const char *value,
                                   const char *label);

  /** Changes or adds a value label for the specified value of a numeric
      variable. The label should be a null-terminated string not exceeding 60
      bytes in length.
      \param varName a null-terminated variable name.
      \param value a double value.
      \param label a null-terminated label value.
      \sa
           SetVarNameAndType
      \return The return code. \n
              0=No error \n
              9=No data source \n
              17=IBM SPSS Statistics backend is not ready \n
              27=invalid variable name \n
              60=No cursor is running
  */
  SPSSXD_API int SetVarNValueLabel( const char *varName,
                                    const double value,
                                    const char *label);

  /** Sets missing values for a short string variable. The argument missingFormat
      must be set to a value in the range 0-3 to indicate the number of missing
      values supplied. When fewer than three missing values are to be defined, the
      redundant arguments must still be present, although their values are not
      inspected. For example, if missingFormat is 2, missingVal3 is unused. The
      supplied missing values must be nullterminated and not longer than the length
      of the variable unless the excess length is made up of blanks,which are ignored.
      If the missing value is shorter than the length of the variable, trailing blanks
      are assumed.
      \param varName a null-terminated variable name.
      \param missingFormat Missing format code.
      \param missingVal1 First missing value.
      \param missingVal2 Second missing value.
      \param missingVal3 Third missing value.
      \sa
           SetVarNameAndType
      \return The return code. \n
              0=No error \n
              9=No data source \n
              17=IBM SPSS Statistics backend is not ready \n
              27=Invalid variable name \n
              57=Numeric variable. \n
              59=The length of missing value should be less than 9 \n
              60=No cursor is running
   */
  SPSSXD_API int SetVarCMissingValues(const char *varName,
                                      const int missingFormat,
                                      const char *missingVal1,
                                      const char *missingVal2,
                                      const char *missingVal3);

    /** Sets missing values for a numeric variable. The interpretation of the
        arguments missingVal1, missingVal2, and missingVal3 depends on the value of
        missingFormat. If missingFormat is set to -2, missingVal1 and missingVal2
        are taken as the upper and lower limits, respectively, of the range, and
        missingVal3 is ignored. If missingFormat is -3, missingval1 and missingVal2
        are taken as limits of the range and missingVal3 is taken as the discrete
        missing value. If missingFormat is neither of the above, it must be in the range 0-3,
        indicating the number of discrete missing values present. For example, if
        missingFormat is 2, missingVal1 and missingVal2 are taken as two discrete missing
        values and missingVal3 is ignored.
        \param varName a null-terminated variable name.
        \param missingFormat Missing format code.
        \param missingVal1 First missing value.
        \param missingVal2 Second missing value.
        \param missingVal3 Third missing value.
        \sa
             SetVarNameAndType
        \return The return code. \n
                0=No error \n
                9=No data source \n
                17=IBM SPSS Statistics backend is not ready \n
                27=Invalid variable name \n
                60=No cursor is running
    */
    SPSSXD_API int SetVarNMissingValues(const char *varName,
                                        const int missingFormat,
                                        const double missingVal1,
                                        const double missingVal2,
                                        const double missingVal3);

    /** Sets the value of the measurement level attribute of a variable.
      \param varName a null-terminated variable name.
      \param measureLevel measurement level setting.
                          2=nominal, \n
                          3=ordinal, \n
                          4=scale.
      \sa
           SetVarNameAndType
      \return The return code. \n
              0=No error \n
              9=No data source \n
              17=IBM SPSS Statistics backend is not ready \n
              27=Invalid variable name \n
              60=No cursor is running
    */
    SPSSXD_API int SetVarMeasureLevel(const char * varName,
                                      const int measureLevel);

    /** Sets the alignment of a variable for the IBM SPSS Statistics Data Editor.
        \param varName a null-terminated variable name.
        \param alignment alignment setting. 0=left, 1=right, 2=center.
        \sa
             SetVarNameAndType
        \return The return code. \n
                0=No error \n
                9=No data source \n
                17=IBM SPSS Statistics backend is not ready \n
                27=Invalid variable name \n
                60=No cursor is running
    */
    SPSSXD_API int SetVarAlignment(const char * varName,
                                    const int alignment);

    /** Sets the write format of the named variable.
        \param varName a null-terminated variable name.
        \param formatType a format type code.
                          A = 1
                          AHEX = 2
                          COMMA = 3
                          DOLLAR = 4
                          F = 5
                          IB = 6
                          PIBHEX = 7
                          P = 8
                          PIB = 9
                          PK = 10
                          RB = 11
                          RBHEX = 12
                          Z=15
                          N = 16
                          E = 17
                          DATE=20
                          TIME = 21
                          DATETIME = 22
                          ADATE = 23
                          JDATE = 24
                          DTIME = 25
                          WKDAY = 26
                          MONTH = 27
                          MOYR = 28
                          QYR = 29
                          WKYR = 30
                          PERCENT = 31
                          DOT = 32
                          CCA = 33
                          CCB = 34
                          CCC = 35
                          CCD = 36
                          CCE = 37
                          EDATE = 38
                          SDATE = 39
                          G = 40
                          LNUMBER = 41
                          LDATE = 42
                          LTIME = 43
                          LCA = 44
                          LCB = 45
                          LCC = 46
                          LCD = 47
                          LCE = 48
                          LCF = 49
                          LCG = 50
                          LCH = 51
                          LCI = 52
                          LCJ = 53
        \param formatWid a format width.
        \param formatDec the number of digits after the decimal.
        \sa
             SetVarNameAndType
        \return The return code. \n
                0=No error \n
                9=No data source \n
                17=IBM SPSS Statistics backend is not ready \n
                27=Invalid variable name \n
                60=No cursor is running
    */
    SPSSXD_API int SetVarFormat(const char *varName,
                                const int formatType,
                                const int formatWid,
                                const int formatDec);

    /** For cursor write mode, after committing the data for the current case,
        call this function before setting data for the next case.
        \sa
             SetVarNameAndType
        \return The return code. \n
                0=No error \n
                9=No data source \n
                17=IBM SPSS Statistics backend is not ready \n
                27=Invalid variable name \n
                60=No cursor is running
    */
    SPSSXD_API int CommitHeader();

    /** Sets the value of a string variable for the current case. The current case is
        not written until CommitCaseRecord is called.
        \param varName a null-terminated variable name.
        \param value a value that is not null-terminated.
        \param length the length of value.
        \code
            void func()
            {
                int errLevel = 0;
                errLevel = StartSpss();

                char *cmd1 = "GET FILE='demo.sav'.";
                errLevel = Submit(cmd1,strlen(cmd1));

                errLevel = MakeCaseCursor("w");

                char *varName[] = {"t1","t2"};
                int varType[] = {0,20};
                unsigned int numOfVar = 2;
                errLevel = SetVarNameAndType(varName,varType,numOfVar);

                errLevel = CommitHeader();
                int caseCount = GetRowCount(errLevel);

                char *cValue = "ABC";
                for(int i = 0; i < caseCount; i++) {
                    errLevel = NextCase();
                    errLevel = SetValueNumeric(varName[0],1.1*i);
                    errLevel = SetValueChar(varName[1],cValue,strlen(cValue));
                    errLevel = CommitCaseRecord();
                }

                errLevel = RemoveCaseCursor();

                if(IsXDriven()){
                  StopSpss();
                }
            }
        \endcode
        \return The return code. \n
                0=No error \n
                17=IBM SPSS Statistics backend is not ready \n
                23=End of the data \n
                60=No cursor is running
    */
    SPSSXD_API int SetValueChar(const char *varName,
                                const char* value,
                                const int length);

    /** Sets the value of a numeric variable for the current case. The current case
        is not written out to the data file until spssCommitCaseRecord is called.
        \param varName a null-terminated variable name.
        \param value a value.
        \sa
            SetValueChar
        \return The return code. \n
                0=No error \n
                9=No data source \n
                17=IBM SPSS Statistics backend is not ready \n
                23=End of data \n
                27=Invalid variable name \n
                60=No cursor is running
        */
    SPSSXD_API int SetValueNumeric(const char *varName,
                                   const double value);

    /** For cursor write mode, commits the data for the current case (row) into to the active data source.
        \sa
            SetValueChar
        \return The return code. \n
                0=No error \n
                9=No data source \n
                17=IBM SPSS Statistics backend is not ready \n
                60=No cursor is running
    */
    SPSSXD_API int CommitCaseRecord();

    /** Write a case to the active data source for the Append mode.
        \code
            void func()
            {
                int errLevel = 0;
                errLevel = StartSpss();

                char *cmd1 = "GET FILE='demo.sav'.";
                errLevel = Submit(cmd1,strlen(cmd1));

                errLevel = MakeCaseCursor("a");
                int caseCount = 10;

                for(int i = 0; i < caseCount; i++) {
                    errLevel = SetValueNumeric("mpg",1.1*i);
                    errLevel = SetValueNumeric("engine",2.2*i);
                    errLevel = CommitNewCase();
                }

                errLevel = EndChanges();

                errLevel = RemoveCaseCursor();

                if(IsXDriven()){
                  StopSpss();
                }
            }
        \endcode
        \return The return code. \n
                0=No error \n
                9=No data source \n
                17=IBM SPSS Statistics backend is not ready \n
                60=No cursor is running
    */
    SPSSXD_API int CommitNewCase();

    /** For cursor append mode, commits the data for the current case (row) into the active data source.
        \sa
            CommitNewCase
       \return The return code. \n
               0=No error \n
               9=No data source \n
               17=IBM SPSS Statistics backend is not ready \n
               60=No cursor is running
    */
    SPSSXD_API int EndChanges();

    /** Checks the end of the split.
       \param endSplit  It will take the result.
                        0 means not at the end of the split,
                        1 means at the end of the split.
       \code
           void func()
           {
               int errLevel = 0;
               errLevel = StartSpss();

               char *cmd1 = "GET FILE='Employee data.sav'.";
               errLevel = Submit(cmd1,strlen(cmd1));

               char *cmd2 = "SORT CASES BY GENDER.";
               errLevel = Submit(cmd2,strlen(cmd2));

               char *cmd3 = "SPLIT FILE LAYERED BY GENDER.";
               errLevel = Submit(cmd3,strlen(cmd3));
               long numCases = GetRowCount(errLevel);

               errLevel = MakeCaseCursor();

               int i = 0;
               while( i < numCases ) {
                   errLevel = NextCase();
                   if(errLevel == 23) {
                       break;
                   }
                   ++i;
                   int split = 0 ;
                   errLevel = IsEndSplit(split);
                   if(split) {
                       std::cout << "Found split end. New split begins at case: " << i << std::endl;
                   }
               }

               errLevel = RemoveCaseCursor();

               if(IsXDriven()){
                 StopSpss();
               }
           }
       \endcode
       \return The return code. \n
               0=No error \n
               17=IBM SPSS Statistics backend is not ready \n
               60=No cursor is running
    */
    SPSSXD_API int IsEndSplit(int & endSplit);

    /** Gets the attribute names. After calling this function,
        FreeAttrNames should be called to free the memory for the names.
       \param index  variable index.
       \param name   store the value of the name.
       \param numOfNames  the number of names.
       \code
            int errLevel = 0;
            if(!IsBackendReady()){
               errLevel = StartSpss();
            }
            if ( 0 == errLevel ){
               std::string cmd("Data List free /x.");
               Submit(cmd.c_str(),cmd.size());
               cmd = "Begin data.";
               QueueCommandPart(cmd.c_str(),cmd.size());
               cmd = "1";
               QueueCommandPart(cmd.c_str(),cmd.size());
               cmd = "2";
               QueueCommandPart(cmd.c_str(),cmd.size());
               cmd = "3";
               QueueCommandPart(cmd.c_str(),cmd.size());
               cmd = "End data.";
               Submit(cmd.c_str(),cmd.size());
               cmd = "Variable Attributes VARIABLES=x ATTRIBUTE=DemographicVars ('1').";
               Submit(cmd.c_str(),cmd.size());

               int varIndex = 0;
               int numOfNames = 0;
               char **name;
               errLevel = GetVarAttributeNames(varIndex, &name, &numOfNames);
               for(int i = 0; i < numOfNames; i++) {
                   int numOfAttr = 0;
                   char **attr;
                   errLevel = GetVarAttributes(varIndex, name[i], &attr, &numOfAttr);
                   for(int j = 0; j < numOfAttr; j++) {
                        std::cout << name[i] << ": " << attr[j] << std::endl;
                   }
                   FreeAttributes(attr,numOfAttr);
               }
               FreeAttributeNames(name,numOfNames);

               if(IsXDriven()){
                 StopSpss();
               }
          }
       \endcode
       \return The return code. \n
               0=No error \n
               9=No data source \n
               10=Invalid index  \n
               17=IBM SPSS Statistics backend is not ready
     */
    SPSSXD_API int GetVarAttributeNames(const int index, char ***name, int *numOfNames);

    /** Gets the names of any datafile attributes for the active dataset.
        After calling this function,
        FreeAttrNames should be called to free the memory for the names.
       \param name   The names of any datafile attributes.
       \param numOfNames  The number of names.
       \code
            int errLevel = 0;
            if(!IsBackendReady()){
               errLevel = StartSpss();
            }
            if ( 0 == errLevel ){
               std::string cmd("Data List free /x.");
               Submit(cmd.c_str(),cmd.size());
               cmd = "Begin data.";
               QueueCommandPart(cmd.c_str(),cmd.size());
               cmd = "1";
               QueueCommandPart(cmd.c_str(),cmd.size());
               cmd = "2";
               QueueCommandPart(cmd.c_str(),cmd.size());
               cmd = "3";
               QueueCommandPart(cmd.c_str(),cmd.size());
               cmd = "End data.";
               Submit(cmd.c_str(),cmd.size());
               cmd = "DATAFILE ATTRIBUTE  ATTRIBUTE=OriginalVersion ('1').";
               Submit(cmd.c_str(),cmd.size());

               int numOfNames = 0;
               char **name;
               errLevel = GetDataFileAttributeNames(&name, &numOfNames);
               for(int i = 0; i < numOfNames; i++) {
                   int numOfAttr = 0;
                   char **attr;
                   errLevel = GetDataFileAttributes(name[i], &attr, &numOfAttr);
                   for(int j = 0; j < numOfAttr; j++) {
                        std::cout << name[i] << ": " << attr[j] << std::endl;
                   }
                   FreeAttributes(attr,numOfAttr);
               }
               FreeAttributeNames(name,numOfNames);

              if(IsXDriven()){
                StopSpss();
              }
          }
       \endcode
       \return The return code. \n
               0=No error \n
               9=No data source \n
               17=IBM SPSS Statistics backend is not ready
    */
    SPSSXD_API int GetDataFileAttributeNames(char ***name, int *numOfNames);

    /** Free the memory of name.
       \param name  its memory will be freed.
       \param numOfNames  the number of names.
       \sa
           GetVarAttributeNames
       \return The return code. \n
              0=No error
     */
    SPSSXD_API int FreeAttributeNames(char **name, const int numOfNames);

    /** GetVarAttributes returns the custom attribute of the indexed variable. After calling this function,
        call FreeAttributes to free memory.
       \param index  variable index.
       \param attrName  attribute name.
       \param attr  attribute value
       \param numOfAttr number of values.
       \sa
           GetVarAttributeNames
       \return The return code. \n
               0=No error \n
               9=No data source \n
               10=Invalid index  \n
               17=IBM SPSS Statistics backend is not ready
     */
    SPSSXD_API int GetVarAttributes(const int index, const char *attrName, char ***attr, int *numOfAttr);

    /** Gets the values associated with a specified datafile attribute in the active dataset.
        After calling this function,
        call FreeAttributes to free memory.
       \param attrName  The name of the attribute to retrieve.
       \param attr  The value(s) associated with the attribute.
       \param numOfAttr The number of values associated with the attribute. A given datafile attribute can have an array of values.
       \sa
           GetDataFileAttributeNames
       \return The return code. \n
               0=No error \n
               9=No data source \n
               17=IBM SPSS Statistics backend is not ready
    */
    SPSSXD_API int GetDataFileAttributes(const char *attrName, char ***attr, int *numOfAttr);

    /** Free the memory associated with an attribute.
       \param attr The attribute.
       \param numOfAttr The number of values associated with the attribute.
       \sa
           GetVarAttributeNames
       \return The return code. \n
               0=No error
     */
    SPSSXD_API int FreeAttributes(char **attr, const int numOfAttr);

    /** Sets the custom attribute of a variable.
       \param varName  variable name.
       \param attrName attribute name.
       \param value    attribute value
       \param index    attribute index to save.
       \code
           void func()
           {
               int errLevel = 0;
               errLevel = StartSpss();

               char *cmd1 = "get file = 'Employee data.sav'.";
               errLevel = Submit(cmd1,strlen(cmd1));

               errLevel = MakeCaseCursor("w");

               int varsBuffer = 100;
               errLevel = AllocNewVarsBuffer(varsBuffer);

               char *varNames[] = {"t1","t2","t3"};
               int varTypes[] = {0,20,20};
               const int numOfVar = 3;
               errLevel = SetVarNameAndType(varNames,varTypes,numOfVar);
               char *attrNames[] = {"attr1","attr2","attr3"};
               char *values[] = {"v1","v2","v3"};
               const int attrIndex = 0;
               for(int i = 0; i < numOfVar; i++) {
                   errLevel = SetVarAttributes(varNames[i],attrNames[i],values[i],attrIndex);
               }

               errLevel = CommitHeader();

               errLevel = RemoveCaseCursor();

               if(IsXDriven()){
                 StopSpss();
               }
           }
       \endcode
       \return The return code. \n
               0=No error \n
               10=Invalid index \n
               17=IBM SPSS Statistics backend is not ready \n
               27=Invalid variable name \n
               58=Invalid attribute name \n
               60=No cursor is running
     */
    SPSSXD_API int SetVarAttributes(const char *varName,const  char *attrName,const char *value,const int index);

    /** GetRowCountInProcDS returns the number of cases in the current IBM SPSS Statistics procedure data source.
        There needs to be a procedure data source before calling this function.
        The value -1 indicates the case count is 'unknown'. This is the value prior
        to an actual pass of data from a data source. The value is non-negative
        for data in .sav files.  The -1 value occurs before a pass of the data
        from a source other than a .sav file.
        \param  errCode Returned value indicates error Code. \n
                0=No error  \n
                17=IBM SPSS Statistics backend is not ready \n
                60=No cursor is running
        \sa
            GetVariableCountInProcDS
        \return n>0 The number of cases.
    */
    SPSSXD_API long GetRowCountInProcDS(int& errCode);

    /**  GetVariableCountInProcDS returns the number of variables in the procedure data source.
         There needs to be an active procedure data source before calling this function.
         This value may be zero because in certain stages of processing data sources, no variables
         (and hence no cases) may exist. This is the state when the IBM SPSS Statistics backend first starts.
         \param errCode Returned value indicates error. \n
                0=No error  \n
                17=IBM SPSS Statistics backend is not ready \n
                60=No cursor is running
         \code
             void func()
             {
                  int errLevel = 0;
                  errLevel = StartSpss();

                  char *cmd1 = "get file = 'Employee data.sav'.";
                  errLevel = Submit(cmd1,strlen(cmd1));

                  errLevel = MakeCaseCursor();

                  long rowCount = GetRowCountInProcDS(errLevel);

                  unsigned varCount = GetVariableCountInProcDS(errLevel);

                  for(int i = 0; i < varCount; i++) {
                      int varType = GetVariableTypeInProcDS(i,errLevel);

                      const char *format = GetVariableFormatInProcDS(i,errLevel);

                      const char *varLabel = GetVariableLabelInProcDS(i,errLevel);

                      int measureLevel = GetVariableMeasurementLevelInProcDS(i,errLevel);

                      const char *varName = GetVariableNameInProcDS(i,errLevel);
                      
                      int varRole = GetVariableRoleInProcDS(i, errLevel);

                      char **name;
                      int numOfNames;
                      errLevel = GetVarAttributeNamesInProcDS(i, &name, &numOfNames);
                      for(int j = 0; j < numOfNames; j++) {
                          char **attr;
                          int numOfAttr;
                          errLevel = GetVarAttributesInProcDS(j, name[j], &attr, &numOfAttr);
                          FreeAttributes(attr,numOfAttr);
                      }
                      FreeAttributeNames(name,numOfNames);
                      char **fileAttrNames;
                      int numOfFileAttrNames;
                      errLevel = GetDataFileAttributeNamesInProcDS(&fileAttrNames,&numOfFileAttrNames);
                      if(errLevel !=0 ) {
                          char **fileAttr;
                          int numOfFileAttr;
                          for(int k = 0; k < numOfFileAttrNames; k++) {
                              GetDataFileAttributesInProcDS(fileAttrNames[k],&fileAttr,&numOfFileAttr);
                              FreeAttributes(fileAttr,numOfFileAttr);
                          }
                      }
                      FreeAttributeNames(fileAttrNames,numOfFileAttrNames);
                      if(varType != 0 && varType < 8) {
                          const int CMISSING_SIZE = 9;
                          char v1[CMISSING_SIZE],v2[CMISSING_SIZE],v3[CMISSING_SIZE];
                          int missingFormat;
                          memset(v1,0,CMISSING_SIZE);
                          memset(v2,0,CMISSING_SIZE);
                          memset(v3,0,CMISSING_SIZE);
                          errLevel = GetVarCMissingValuesInProcDS(i,&missingFormat,v1, v2, v3);
                      }
                      if(varType == 0) {
                          double v1,v2,v3;
                          int missingFormat;
                          errLevel = GetVarNMissingValuesInProcDS(i,&missingFormat,&v1, &v2, &v3);
                      }

                      int formatType,formatWidth,formatDecimal;
                      errLevel = GetVariableFormatTypeInProcDS(i, formatType, formatWidth, formatDecimal);
                 }


                 errLevel = RemoveCaseCursor();

                 if(IsXDriven()){
                   StopSpss();
                 }
             }
         \endcode
         \return n>0 The number of variables.
    */
    SPSSXD_API unsigned GetVariableCountInProcDS(int& errCode);

    /** GetVariableFormatInProcDS returns the print format of the indexed variable from procedure data source.
        There needs to be a procedure data source before calling this function.
        The index value must be within bounds of the variable count.
        \param index The zero based index of the variable.
        \param errCode Returned value indicates error code. \n
                0=No error  \n
                10=Invalid index  \n
                17=IBM SPSS Statistics backend is not ready \n
                60=No cursor is running
        \sa
            GetVariableCountInProcDS
        \return if No Error, a string representing the variable's print format.
    */
    SPSSXD_API const char* GetVariableFormatInProcDS(int index,int& errCode);

    /** GetVariableLabelInProcDS returns the label of the indexed variable in the procedure data source.
        There needs to be an active procedure data source before calling this function.
        The index value must be within bounds of the variable count.
        \param index The zero based index of the variable.
        \param errCode Returned value indicates error code. \n
                0=No error  \n
                10=Invalid index  \n
                17=IBM SPSS Statistics backend is not ready \n
                60=No cursor is running
        \sa
            GetVariableCountInProcDS
        \return if No Error,  the variable label.
    */
    SPSSXD_API const char* GetVariableLabelInProcDS(int index,int& errCode);

    /** GetVariableMeasurementLevelInProcDS returns the measurement level of the indexed
        variable from the procedure data source. Measurement level is an optional field
        that describes IBM SPSS Statistics variables. There needs to be an active procedure data source
        before calling this function. The index value must be within bounds of the variable count.
        \param index The 0 based index of the variable.
        \param errCode Returned value indicates the error code. \n
                0=No error  \n
                10=Invalid index  \n
                17=IBM SPSS Statistics backend is not ready \n
                60=No cursor is running
        \sa
            GetVariableCountInProcDS
        \return 1 = unknown, 2 = nominal, 3 = ordinal, 4 = scale
    */
    SPSSXD_API int GetVariableMeasurementLevelInProcDS(int index,int& errCode);

    /** GetVariableNameInProcDS returns the name of the indexed variable in the procedure data source.
        There needs to be an active procedure data source before calling this function.
        The index value must be within bounds of the variable count.
        \param index The zero based index of the requested variable name.
        \param errCode Returned value indicates error level. \n
                0=No error  \n
                10=Invalid index  \n
                17=IBM SPSS Statistics backend is not ready \n
                60=No cursor is running
        \sa
            GetVariableCountInProcDS
        \return ""=failure or no active file, otherwise the variable name.
    */
    SPSSXD_API const char* GetVariableNameInProcDS(int index,int& errCode);

    /** GetVariableTypeInProcDS returns the type of the indexed variable in the procedure data source.
        There needs to be an active datasource before calling this function.
        The index value must be within bounds of the variable count.
        \param index The zero based index of the variable.
        \param errCode Returned value indicates error code. \n
                0=No error  \n
                10=Invalid index  \n
                17=IBM SPSS Statistics backend is not ready \n
                60=No cursor is running
        \sa
            GetVariableCountInProcDS
        \return if No Error, the type of the variable.
        A variable's type is zero for numeric variables and the length in
        bytes for string variables.
    */
    SPSSXD_API int GetVariableTypeInProcDS(int index,int& errCode);

     /** GetVarAttributeNamesInProcDS returns the attribute names in the procedure data source.
         After calling this function, FreeAttrNames should be called to free the memory for the names.
         \param index variable index.
         \param name store the value of the name.
         \param numOfNames the number of names.
         \sa
            GetVariableCountInProcDS
         \return The return code. \n
               0=No error \n
               10=Invalid index  \n
               17=IBM SPSS Statistics backend is not ready \n
               60=No cursor is running
     */
    SPSSXD_API int GetVarAttributeNamesInProcDS(const int index, char ***name, int *numOfNames);

    /** Gets the names of any datafile attributes associated with the current procedure data source.
        \param name The names of any datafile attributes.
        \param numOfNames The number of datafile attributes.
        \sa
            GetVarAttributeNamesInProcDS
        \return The return code. \n
               0=no error \n
               17=IBM SPSS Statistics backend is not ready \n
               60=no cursor is running
    */
    SPSSXD_API int GetDataFileAttributeNamesInProcDS(char ***name, int *numOfNames);

    /** GetVarAttributesInProcDS returns the custom attribute of the indexed variable in the procedure data source.
        After calling this function, call FreeAttributes to free memory.
        \param index variable index.
        \param attrName attribute name.
        \param attr attribute value
        \param numOfAttr number of values.
        \sa
            GetVariableCountInProcDS
        \return The return code. \n
               0=No error \n
               10=Invalid index  \n
               17=IBM SPSS Statistics backend is not ready \n
               60=No cursor is running
     */
    SPSSXD_API int GetVarAttributesInProcDS(const int index, const char *attrName, char ***attr, int *numOfAttr);

    /** Gets the values associated with a specified datafile attribute in the current procedure data source.
        \param attrName The name of the attribute to retrieve.
        \param attr The value(s) associated with the attribute.
        \param numOfAttr The number of values associated with the attribute. A given datafile attribute can have an array of values. 
        \sa
            GetVariableCountInProcDS
        \return The return code. \n
               0=No error \n
               17=IBM SPSS Statistics backend is not ready \n
               60=No cursor is running
     */
    SPSSXD_API int GetDataFileAttributesInProcDS(const char *attrName, char ***attr, int *numOfAttr);

     /** GetVarCMissingValuesInProcDS returns user missing values of string variable in procedure
         data source.
         \param index variable index.
         \param missingFormat format code.
         \param v1 the first missing value. A caller should allocate 9 bytes memory.
         \param v2 the second missing value. A caller should allocate 9 bytes memory.
         \param v3 the third missing value. A caller should allocate 9 bytes memory.
         \sa
            GetVariableCountInProcDS
         \return The return code. \n
               0=No error \n
               10=Invalid index  \n
               17=IBM SPSS Statistics backend is not ready \n
               57=Numeric variable \n
               60=No cursor is running
     */
    SPSSXD_API int GetVarCMissingValuesInProcDS(const int index, int *missingFormat,char * v1, char *v2, char *v3);

    /**  GetVarNMissingValuesInProcDS returns user missing values of the indexed numeric variable
         in the procedure data source.
         \param index variable index.
         \param missingFormat format code.
         \param v1 the first missing value.
         \param v2 the second missing value.
         \param v3 the third missing value.
         \sa
            GetVariableCountInProcDS
         \return The return code. \n
               0=No error \n
               10=Invalid index \n
               17=IBM SPSS Statistics backend is not ready \n
               57=Expects numeric variable \n
               60=No cursor is running
    */
    SPSSXD_API int GetVarNMissingValuesInProcDS(const int index,int *missingFormat,double *v1, double *v2, double *v3);

/**   GetVariableFormatType returns the format type, width, and decimal of the indexed variable in the current active file.
      The IBM SPSS Statistics backend needs to be started and there needs to be an active data source before calling this function.
      The index value must be within bounds of the variable count.
      \param varIndex   The index of a variable
      \param formatType    The location that will receive the format type.
      \param formatWidth   The location that will receive the format width.
      \param formatDecimal The location that will receive the number of decimals.
      \sa
           GetVariableCountInProcDS
      \return              The return code. \n
                            0=Success \n
                            10=Invalid index \n
                            17=IBM SPSS Statistics backend is not ready \n
                            23=No more data \n
                            60=No cursor is running
    */
    SPSSXD_API int GetVariableFormatTypeInProcDS(int varIndex, int& formatType, int& formatWidth, int& formatDecimal);

    /** HasProcedure returns an integer indicating whether or not there is a running BpProcedure (user procedure).
        \code
            void func()
            {
                int errLevel = 0;
                errLevel = StartSpss();

                char *cmd1 = "get file = 'Employee data.sav'.";
                errLevel = Submit(cmd1,strlen(cmd1));

                int isRunning = HasProcedure();
                errLevel = StartProcedure("procname");
                isRunning = HasProcedure();
                errLevel = EndProcedure();

                if(IsXDriven()){
                  StopSpss();
                }
            }
        \endcode
        \return a value indicating the running procedure state. \n
                0=No running procedure \n
                1=A running procedure
     */
    SPSSXD_API int HasProcedure();

    /** GetWeightVar returns a name of weight variable. If no weight variable is defined, it returns a null string.
        \code
            void func()
            {
                int errLevel = 0;
                errLevel = StartSpss();

                char *cmd1 = "get file = 'Employee data.sav'.";
                errLevel = Submit(cmd1,strlen(cmd1));

                char *cmd2 = "weight by id";
                errLevel = Submit(cmd2,strlen(cmd2));

                const char *varName = GetWeightVar(errLevel);

                if(IsXDriven()){
                  StopSpss();
                }
            }
        \endcode
        \param errLevel This is the returned error code.
                        0=Success \n
                        9=No data source \n
                        17=IBM SPSS Statistics backend is not ready
        \return a value indicating weight variable name.
     */
    SPSSXD_API const char* GetWeightVar(int &errLevel);

    /** GetSPSSLowHigh returns the values that IBM SPSS Statistics is using for LO and HI.
        \param low take the LO value.
        \param high take the HI value.
        \code
            void func()
            {
                int errLevel = 0;
                errLevel = StartSpss();

                double low, high;
                errLevel = GetSPSSLowHigh(&low, &high);

                if(IsXDriven()){
                  StopSpss();
                }
            }
        \endcode
        \return The return code. \n
                 0=No error \n
                 17=IBM SPSS Statistics backend is not ready
     */
    SPSSXD_API int GetSPSSLowHigh(double *low, double *high);


    /** Resets the current cursor, and moves it to the beginning position.
        \code
            void func()
            {
                int errLevel = 0;
                errLevel = StartSpss();

                char *cmd1 = "get file = 'Employee data.sav'.";
                errLevel = Submit(cmd1,strlen(cmd1));

                errLevel = MakeCaseCursor("w");
                errLevel = AllocNewVarsBuffer(100);
                errLevel = SetOneVarNameAndType("t1",0);
                errLevel = CommitHeader();
                errLevel = ResetDataPass();
                errLevel = NextCase();
                errLevel = RemoveCaseCursor();
                if(IsXDriven()){
                  StopSpss();
                }
            }
        \endcode
        \return The return code. \n
                 0=No error \n
                 17=IBM SPSS Statistics backend is not ready
     */
    SPSSXD_API int ResetDataPass();

    /** Allocate a buffer for adding new variables with a cursor.
        \param size The buffer size.
        \code
            void func()
            {
                int errLevel = 0;

                errLevel = StartSpss();

                char *cmd1 = "get file = 'Employee data.sav'.";
                errLevel = Submit(cmd1,strlen(cmd1));

                errLevel = MakeCaseCursor("w");
                errLevel = AllocNewVarsBuffer(100);
                errLevel = SetOneVarNameAndType("t1",0);
                errLevel = SetOneVarNameAndType("t2",20);
                errLevel = CommitHeader();
                errLevel = RemoveCaseCursor();
                if(IsXDriven()){
                  StopSpss();
                }
            }
        \endcode
        \return The return code. \n
               0=No error \n
               17=IBM SPSS Statistics backend is not ready \n
               50=Invalid size of variables buffer \n
               51=Maximum vars_added is already set \n
               60=No cursor is running
     */
    SPSSXD_API int AllocNewVarsBuffer(unsigned int  size);

    /** Adds a new  variable to the procedure data source.
      \param varName variable name.
      \param varType variable type. 0 means numeric, greater than 0 means string.
      \sa
          AllocNewVarsBuffer
      \return The return code. \n
               0=No error \n
               17=IBM SPSS Statistics backend is not ready \n
               25=Duplicated variable name \n
               27=Invalid variable name \n
               52=Maximum vars_added is not set \n
               60=No cursor is running \n
               62=Invalid variable type
     */
    SPSSXD_API int SetOneVarNameAndType(const char *varName,int varType);

    /** Gets the value labels from the active data source for a numeric variable. You must call
        FreeNValueLabels to free memory.
        \param index variable index.
        \param values save value as double array.
        \param labels save labels as string array.
        \param numOfValueLabels the number of value-label pairs.
        \code
             void func()
             {
                  int errLevel = 0;
                  errLevel = StartSpss();

                  char *cmd1 = "get file = 'Employee data.sav'.";
                  errLevel = Submit(cmd1,strlen(cmd1));

                  unsigned varCount = GetVariableCount(errLevel);

                  for(int i = 0; i < varCount; i++) {
                      int varType = GetVariableType(i,errLevel);

                      if(varType == 0) {
                          double *values = NULL;
                          char **labels = NULL;
                          int numOfValues = 0;

                          errLevel = GetNValueLabels(i,&values,&labels,&numOfValues);
                          for(int k = 0; k < numOfValues; k++) {
                               std::cout << values[k] << " = " << labels[k] << std::endl;
                          }
                          errLevel = FreeNValueLabels(values,labels,numOfValues);
                      }
                  }

                  if(IsXDriven()){
                    StopSpss();
                  }
              }
        \endcode
        \return The return code. \n
               0=No error \n
               9=No data source \n
               10=Invalid index \n
               17=IBM SPSS Statistics backend is not ready \n
               57=Expects numeric variable
     */
    SPSSXD_API int GetNValueLabels(int index,double **values, char ***labels, int *numOfValueLabels);

    /** Free the memory for the double and string arrays after calling GetNValueLabels().
        \param values double array to save the double values.
        \param labels string array to save the string labels.
        \param numOfValueLabels the size of array.
        \sa
            GetNValueLabels
        \return The return code. \n
                0=No error
     */
    SPSSXD_API int FreeNValueLabels(double *values, char **labels, int numOfValueLabels);

    /** Gets the value labels from the active data source for a string variable. You must call
        FreeCValueLabels to free memory.
        \param index variable index.
        \param values save values as string array.
        \param labels save labels as string array.
        \param numOfValueLabels the number of value-labels pairs.
        \code
            void func()
            {
                int errLevel = 0;
                errLevel = StartSpss();

                char *cmd1 = "get file = 'Employee data.sav'.";
                errLevel = Submit(cmd1,strlen(cmd1));

                unsigned varCount = GetVariableCount(errLevel);

                for(int i = 0; i < varCount; i++) {
                    int varType = GetVariableType(i,errLevel);

                    if(varType != 0) {
                        char **values = NULL;
                        char **labels = NULL;
                        int numOfValues;
                        errLevel = GetCValueLabels(i,&values,&labels,&numOfValues);

                        for(int k = 0; k < numOfValues; k++) {
                        std::cout << values[k] << " = " << labels[k] << std::endl;
                        }
                        errLevel = FreeCValueLabels(values,labels,numOfValues);
                   }
               }

               if(IsXDriven()){
                 StopSpss();
               }
           }
        \endcode
        \return The return code. \n
               0=No error \n
               9=No data source \n
               10=Invalid index \n
               17=IBM SPSS Statistics backend is not ready \n
               56=Expects string variable
     */
    SPSSXD_API int GetCValueLabels(int index,char ***values, char ***labels, int *numOfValueLabels);

    /** Free the memory for the string arrays after calling GetCValueLabels().
        \param values string array to save the string values.
        \param labels string array to save the string labels.
        \param numOfValueLabels the size of array.
        \sa
            GetCValueLabels
        \return The return code. \n
                0=No error
     */
    SPSSXD_API int FreeCValueLabels(char **values, char **labels, int numOfValueLabels);

    /** Checks if "cache" syntax should be added for Clementine.
        \param needCache store the return value.
        \code
            void func()
            {
                int errLevel = 0;
                errLevel = StartSpss();

                int cache = 0;
                errLevel = IsNeedCache(cache);

                char *cmd1 = "GET FILE='demo.sav'.";
                errLevel = Submit(cmd1,strlen(cmd1));

                errLevel = IsNeedCache(cache);

                char *cmd2 = "SORT CASES BY age (A) .";
                errLevel = Submit(cmd2,strlen(cmd2));

                errLevel = IsNeedCache(cache);

                if(IsXDriven()){
                  StopSpss();
                }
            }
        \endcode
        \return The return code. \n
                0=No error \n
                17=IBM SPSS Statistics backend is not ready
     */
    SPSSXD_API int IsNeedCache(int &needCache);

    /** Gets the IBM SPSS Statistics system missing value.
        \param sysMissing store the system missing value.
        \code
            void func()
            {
                int errLevel = 0;
                errLevel = StartSpss();

                double sysMissing = 0.0;
                errLevel = GetSystemMissingValue(sysMissing);
                std::cout << "system missing value = " << sysMissing << std::endl;

                if(IsXDriven()){
                  StopSpss();
                }
            }
        \endcode
        \return The return code. \n
                0=No error \n
                17=IBM SPSS Statistics backend is not ready
     */
    SPSSXD_API int GetSystemMissingValue(double &sysMissing);

    /** Validates a potential IBM SPSS Statistics variable name
        \param varName a null-terminated variable name.
        \code
            void func()
            {
                char *varName = "var1";

                int result = ValidateVarName(varName);

            }
        \endcode
        \return The return code. \n
                0=Well formed ordinary variable name \n
                1=Well formed scratch variable name \n
                2=Well formed system variable name \n
                3=Invalid length \n
                4=Contains invalid char or imbedded blank \n
                5=Is a reserved word \n
                6=Invalid first character \n
                7=Invalid last character \n
                100=Warning for underscore char at end of varname
     */
    SPSSXD_API int ValidateVarName(const char *varName);

    /** Specify the mode in which the IBM SPSS Statistics backend is to be driven.
        \param mode 0 if the backend is being driven by IBM SPSS Statistics, 1 if the backend is being driven by Python, and 2 for Other. The default is 2.
        \code
            void func()
            {
                int errLevel = 0;
                errLevel = StartSpss();
                errLevel = SetXDriveMode(1);
            }
        \endcode
        \return The return code.
                0=No error  \n
                17=IBM SPSS Statistics backend is not ready \n
                85=Invalid mode
     */
    SPSSXD_API int SetXDriveMode(int mode);

    /** Return the mode in which the IBM SPSS Statistics backend is being driven.
        \param currMode  The current mode
        \param originMode The mode at the start of the session
        \return The return code. \n
                0=No error  \n
                17=IBM SPSS Statistics backend is not ready
     */
    SPSSXD_API int GetXDriveMode(int &currMode,int &originMode);
    
    /** Starts a data step. Within a data step you can create new datasets and manage multiple existing datasets. You create
        new datasets or access existing ones using the CreateDataset function. Data steps are closed with EndDataStep. \n \n
        Within a data step you cannot create a cursor, a pivot table, or a text block, and you cannot call
        the StartProcedure function or the Submit function. \n \n 
        You cannot start a data step if there are pending transformations or if a user procedure exists. You cannot start a data step if 
        there is an existing one. In that case, close the existing one with EndDataStep and call StartDataStep to start a new data step.
        \code
            void func()
            {
                int errLevel = 0;
                const char* cmd ="GET FILE='demo.sav'.";
            
                errLevel = StartSpss();
                errLevel = Submit(cmd, strlen(cmd));
                StartDataStep();                
                errLevel = CreateDataset("*");                
                EndDataStep();                
                StopSpss();
            }
        \endcode
        \return The return code. \n
                        0=No error \n
                        17=IBM SPSS Statistics backend is not ready \n
                        94=Cannot start a data step with pending transformations \n
                        92=A data step is running \n
                        32=A user procedure is running
    */
    SPSSXD_API int StartDataStep();
    
    /** Ends the current data step and closes any dataset objects created with CreateDataset. 
        Any datasets created during the data step will be available.
        \code
            void func()
            {
                int errLevel = 0;
                const char* cmd ="GET FILE='demo.sav'.";
            
                errLevel = StartSpss();
                errLevel = Submit(cmd, strlen(cmd));
                StartDataStep();                
                errLevel = CreateDataset("dataset");                
                EndDataStep();                
                StopSpss();
            }
        \endcode
        \return The return code. \n
                      0=No error \n
                      17=IBM SPSS Statistics backend is not ready
    */                    
    SPSSXD_API int EndDataStep();
    
    /** Creates a dataset object for use within the current data step. Using this function, you can create a new empty dataset or
        establish a connection to an existing dataset in the current session. Dataset objects are implicitly closed by the EndDataStep 
        function and are thus unavailable outside of the data step in which they were created. You can explicitly close a dataset 
        object with the CloseDataset function, which also has the effect of closing the associated dataset. When driving IBM SPSS Statistics
        from an external processor, the StopSPSS function will close any datasets opened during the session. 
        \param name    A string specifying a dataset name. Cannot be NULL. If isEmpty=false, the following rules apply: 
        if an asterisk ("*") or a blank ("") is specified for the name, the dataset object will be associated with the active dataset 
        (if the active dataset does not have a name, then one will be generated); 
        otherwise the dataset object will be associated with the specified dataset (if there is no dataset with the specified name, then the 
        dataset object is associated with the active dataset and the active dataset is renamed to the specified name). If isEmpty=true, 
        the following rules apply: if an asterisk ("*") is specified for the name, a new active dataset will be created with an automatically 
        generated name (you can get the name from the GetActive function); if the name is blank (""), a new dataset is created with an automatically generated name, but it will not be 
        the active dataset; otherwise, a new dataset will be created with the specified name but it will not be the active dataset.  
        \param isEmpty If true, then create a dataset object associated with a new empty data source; otherwise
                        create a dataset object associated with an existing data source. 
        \param hidden If true, then created dataset is hidden. Otherwise, the created dataset is visible.

        \code 
                    void func()
                    {
                        int errLevel = 0;
    
                        std::vector<char*> cmds;
                        cmds.push_back("GET FILE='demo.sav'.");
                        cmds.push_back("dataset name ds1.");
                        cmds.push_back("GET FILE='demo.sav'.");
                        cmds.push_back("dataset name ds2.");
                    
                        errLevel = StartSpss();
                    
                        for (std::vector<char*>::iterator cmd = cmds.begin(); cmd != cmds.end(); ++cmd) 
                        {
                            QueueCommandPart(*cmd, strlen(*cmd));
                        }
                        errLevel = Submit("exe.",4);
                        
                        StartDataStep();
                        
                        // Create a dataset object associated with the dataset named ds1.
                        errLevel = CreateDataset("ds1");
                    
                        // Create a dataset object associated with the active dataset.
                        errLevel = CreateDataset("*");
                    
                        // Create a dataset object associated with the active dataset by explicitly specifying its name.
                        errLevel = CreateDataset("ds2");
                    
                        // Create a dataset object associated with the active dataset and rename the dataset to ds3.
                        errLevel = CreateDataset("ds3");
                    
                        // Create a new inactive and empty dataset named ds4.
                        errLevel = CreateDataset("ds4", true);
                            
                        // Create a new active and empty dataset and assign a name to it automatically.
                        errLevel = CreateDataset("*", true);
                        
                        EndDataStep();
                        
                        StopSpss();
                    }
        \endcode        
        \return    The return code. \n
                        0=No error \n
                        17=IBM SPSS Statistics backend is not ready \n
                        86=No active dataset \n
                        89=Function cannot be called outside of a data step or user procedure \n
                        90=Cannot create an active dataset in a user procedure \n
                        91=A dataset with the specified name already exists \n
                        99=Can't create dataset
    */
    SPSSXD_API int CreateDataset(const char *name = "*",
                                 const bool isEmpty = false,
                                 const bool hidden = false);
    
    /** Rename a dataset accessed within a data step.
        \param oldname     The current name of the dataset.
        \param newname     The new name for the dataset. Cannot be "" or "*".
        \code
            void func()
            {
                int errLevel = 0;
                const char* cmd ="GET FILE='demo.sav'.";
            
                errLevel = StartSpss();
                errLevel = Submit(cmd, strlen(cmd));    
                StartDataStep();
                
                errLevel = CreateDataset("*");
                const char* oldname = GetActive( errLevel);
                const char* newname = "dscars";
                errLevel = SetDatasetName(oldname, newname);
               
                EndDataStep();    
                StopSpss();
            }
        \endcode 
        \return        The return code. \n
                        0=No error \n
                        17=IBM SPSS Statistics backend is not ready \n
                        87=Invalid dataset name \n
                        91=A dataset with the specified name already exists \n
                        89=Function cannot be called outside of a data step or user procedure
    */            
    SPSSXD_API int SetDatasetName(const char *oldname,
                                  const char *newname);
    
    /** Generate a dataset name.
        \param dsName  The generated dataset name. At least 10 bytes of memory are allocated for the name.
        \code 
            void func()
            {
                int errLevel = 0;
                StartSpss();
                
                char dsname[10];
                
                errLevel = GetNewDatasetName(dsname);
            
                std::cout<< dsname << std::endl;
                   
                StopSpss();
            }
        \endcode
        \return        The return code. \n
                        0=No error \n
                        17=IBM SPSS Statistics backend is not ready
    */
    SPSSXD_API int  GetNewDatasetName(char* dsName);    
                 
    /** Gets the active dataset name.
        \param errLevel  The return code. \n
                            0=No error \n
                            17=IBM SPSS Statistics backend is not ready \n
                            86=No active dataset \n
                            93=The active dataset is not associated with a dataset object in the data step
        \code
            void func()
            {
                int errLevel = 0;
                const char* cmd ="GET FILE='demo.sav'.";
            
                errLevel = StartSpss();
                errLevel = Submit(cmd, strlen(cmd));    
                StartDataStep();
                
                errLevel = CreateDataset("*");
                const char* activeName = GetActive( errLevel);
                std::cout<< activeName << std::endl;   
            
                EndDataStep();    
                StopSpss();
            }
        \endcode
        \return        The name of the active dataset. If the active dataset has no name, then "*" is returned.
    */
    SPSSXD_API const char* GetActive(int& errLevel);
    
    /** Sets the active dataset. This function can only be used within a data step.
        \param name    The name of the dataset to be set to active. This must be a dataset associated with a dataset object 
        in the current data step.
        \code
            void func()
            {
                int errLevel = 0;
    
                StartSpss();
                StartDataStep();
                
                const char* dsName = "newds";
                errLevel = CreateDataset(dsName);
                errLevel = SetActive(dsName);
                
                EndDataStep();    
                StopSpss();
            }
        \endcode
        \return        The return code. \n
                            0=No error \n
                            17=IBM SPSS Statistics backend is not ready \n
                            9=No data source \n
                            88=Function cannot be used outside of a data step \n
                            87=Invalid dataset name
    */
    SPSSXD_API int SetActive(const char *name);
    
    /** Creates a deep copy of a specified dataset.
        \param oriDs   The name of the dataset to be copied. Must be the name of a dataset associated with a dataset object in the
                       current data step. You can specify "*" to make a copy of the active dataset.
        \param desDs   The name of the new dataset. It cannot be the name of an existing dataset. If "*" is specified, the 
                       copy becomes the active dataset with a name that is automatically generated. 
        \code
            void func()
            {
                int errLevel = 0;
                const char* cmd ="GET FILE='demo.sav'.";
                
                StartSpss();
                Submit(cmd, strlen(cmd));    
                StartDataStep();
                
                const char* orids = "orids";
                const char* desds = "desds";
                errLevel = CreateDataset(orids);
                errLevel = CopyDataset(orids, desds);
                
                EndDataStep();    
                StopSpss();
            }
        \endcode
        \return        The return code. \n
                            0=No error \n 
                            17=IBM SPSS Statistics backend is not ready \n
                            86=No active dataset \n
                            87=Invalid dataset name \n
                            91=A dataset with the specified name already exists \n
                            89=Function cannot be called outside of a data step or user procedure \n
                            90=Cannot create an active dataset in a user procedure \n 
    */
    SPSSXD_API int CopyDataset( const char* oriDs, 
                                const char* desDs);
                                
    /** Gets the names of all defined datasets.(including those datasets which are just a name defined by DATASET DECLARE) If the active 
		dataset is unnamed, then "*" will be returned. After calling this function, FreeStringArray should be called to free the memory
		for the names.
        \param nameList    The dataset names.
        \param length      The number of names in the returned list.
        \code 
            void func()
            {
                int errLevel = 0;
                const char* cmd ="GET FILE='demo.sav'.";
                
                StartSpss();
                Submit(cmd, strlen(cmd));    
                StartDataStep();
                
                const char* blkds = "blkds";
                const char* cpyds = "cpyds";
                errLevel = CreateDataset(blkds, true);
                errLevel = CopyDataset(blkds, cpyds);
                
                char** names;
                int len;
                errLevel = GetSpssDatasets(&names, len);
                for(int i = 0; i < len; ++i)
                {
                    std::cout<< names[i] <<std::endl;
                }
                FreeStringArray(names, len);
            
                EndDataStep();    
                StopSpss();
            }
        \endcode
        \return            The return code. \n
                            0=No error \n
                            17=IBM SPSS Statistics backend is not ready
    */                        
    SPSSXD_API int GetSpssDatasets( char*** nameList,
                                    int& length);
    
    /** Gets the names of all opened datasets. (not including those datasets which just is a name defined by DATASET DECLARE.) 
	    If the active dataset is unnamed, then "*" will be returned. After calling this function, FreeStringArray should be 
		called to free the memory for the names.
        \param nameList    The dataset names.
        \param length      The number of names in the returned list.
        \code 
            void func()
            {
                int errLevel = 0;
                const char* cmd ="GET FILE='demo.sav'.";
                
                StartSpss();
                Submit(cmd, strlen(cmd));    
                StartDataStep();
                
                const char* blkds = "blkds";
                const char* cpyds = "cpyds";
                errLevel = CreateDataset(blkds, true);
                errLevel = CopyDataset(blkds, cpyds);
                
                char** names;
                int len;
                errLevel = GetOpenedSpssDatasets(&names, len);
                for(int i = 0; i < len; ++i)
                {
                    std::cout<< names[i] <<std::endl;
                }
                FreeStringArray(names, len);
            
                EndDataStep();    
                StopSpss();
            }
        \endcode
        \return            The return code. \n
                            0=No error \n
                            17=IBM SPSS Statistics backend is not ready
    */                        
    SPSSXD_API int GetOpenedSpssDatasets( char*** nameList,
                                    int& length);
    /** Gets the names of all datasets associated with dataset objects in the current data step, i.e. those objects created 
        with the CreateDataset function. After calling this function, FreeStringArray should be called to free the memory for the names.
        \param nameList    The dataset names.
        \param length      The number of names in the returned list.
        \code
            void func()
            {
                int errLevel = 0;
                const char* cmd ="GET FILE='demo.sav'.";
                
                StartSpss();
                Submit(cmd, strlen(cmd));    
                StartDataStep();
                
                const char* blkds = "blkds";
                const char* cpyds = "cpyds";
                errLevel = CreateDataset(blkds, true);
                errLevel = CopyDataset(blkds, cpyds);
                
                char** names;
                int len;
                errLevel = GetDatastepDatasets(&names, len);
                for(int i = 0; i < len; ++i)
                {
                    std::cout<< names[i] <<std::endl;
                }
                FreeStringArray(names, len);
            
                EndDataStep();    
                StopSpss();
            }
        \endcode
        \return            The return code. \n
                            0=No error \n
                            17=IBM SPSS Statistics backend is not ready
    */
    SPSSXD_API int GetDatastepDatasets( char*** nameList,
                                int& length);
    
    /** Frees the memory associated with a 1-dimension char*.
        \param array   The char** object
        \param length  The length of the string array
        \return        The return code. \n
                        0=No error 
    */                                                        
    SPSSXD_API int FreeStringArray(char **array, const int length);

    /** Frees the memory associated with a char*.
        \param str     The char* 
        \return        The return code. \n
                        0=No error 
    */                                                        
    SPSSXD_API int FreeString(char *str);     

    /** Closes a dataset object created by the CreateDataset function as well as the associated IBM SPSS Statistics dataset.
        If the dataset object is associated with the active dataset, the association with the dataset's name is broken and 
        the active dataset remains active but has no name.
        \param name    The dataset name. You can specify "*" for the active dataset.
        \code 
            void func()
            {
                int errLevel = 0;
                const char* cmd ="GET FILE='demo.sav'.";
            
                StartSpss();
                Submit(cmd, strlen(cmd));    
                StartDataStep();
                
                errLevel = CreateDataset("*");
                errLevel = CloseDataset("*");
            
                EndDataStep();    
                StopSpss();
            }
        \endcode
        \return        The return code. \n
                            0=No error \n 
                            17=IBM SPSS Statistics backend is not ready \n
                            89=Function cannot be called outside of a data step or user procedure\n
                            87=Invalid dataset name \n
                            86=No active dataset
    */
    SPSSXD_API int CloseDataset(const char* name);
   
   /** Inserts a variable into a specified dataset at a specified position.
       \param dsName  The dataset name. Must be the name of a dataset associated with a dataset object in the
                               current data step.
       \param index   The position at which to insert the variable (0-based).
       \param varName The variable name.
       \param type    The variable type--0 for numeric and an integer equal to the defined length for a string variable (maximum of 32767 bytes).
       \code 
            void func()
            {
                int errLevel = 0;
                const char* cmd ="GET FILE='demo.sav'.";
            
                StartSpss();
                Submit(cmd, strlen(cmd));    
                StartDataStep();
                
                const char* dsname = "example";
                errLevel = CreateDataset(dsname);
                errLevel = InsertVariable(dsname,9,"price", 0);
                errLevel = InsertVariable(dsname,0,"id", 0);
                
                EndDataStep();    
                StopSpss();
            }
       \endcode
       \return        The return code. \n
                            0=No error \n 
                            17=IBM SPSS Statistics backend is not ready \n
                            89=Function cannot be called outside of a data step or user procedure \n
                            87=Invalid dataset name \n
                            10=Invalid index \n
                            25=Duplicate variable name \n
                            26=Invalid variable type \n
                            27=Invalid variable name
    */
    SPSSXD_API int InsertVariable(  const char* dsName,
                                    const int index,
                                    const char* varName,
                                    const int type);
        
    /** Deletes the variable at a specified position in a specified dataset. 
           \param dsName  The dataset name. Must be the name of a dataset associated with a dataset object in the
                            current data step.
           \param index   The index position of the variable (0-based).
           \code
                void func()
                {
                    int errLevel = 0;
                    const char* cmd ="GET FILE='demo.sav'.";
                
                    StartSpss();
                    Submit(cmd, strlen(cmd));    
                    StartDataStep();
                    
                    const char* dsname = "example";
                    errLevel = CreateDataset(dsname);
                    
                    errLevel = DeleteVariable(dsname,0); // delete variable age
                    errLevel = DeleteVariable(dsname,7); // delete variable employ
                    
                    EndDataStep();    
                    StopSpss();
                }
           \endcode
           \return        The return code. \n
                           0=No error \n
                           17=IBM SPSS Statistics backend is not ready \n
                           89=Function cannot be called outside of a data step or user procedure \n
                           87=Invalid dataset name \n
                           10=Invalid index \n
    */                            
    SPSSXD_API int DeleteVariable(const char* dsName,
                                  const int index);
    
    /** Gets the number of variables in the specified dataset.
        \param dsName      The dataset name. Must be the name of a dataset associated with a dataset object in the
                             current data step.
        \param errLevel    The return code. \n
                                0=No error \n
                                17 =IBM SPSS Statistics backend is not ready \n
                                89 =Function cannot be called outside of a data step or user procedure \n
                                87 =Invalid dataset name \n
        \code 
            void func()
            {
                int errLevel = 0;
                const char* cmd ="GET FILE='demo.sav'.";
            
                StartSpss();
                Submit(cmd, strlen(cmd));    
                StartDataStep();
                
                const char* dsname = "example";
                errLevel = CreateDataset(dsname);
                std::cout << GetVarCountInDS(dsname, errLevel) << std::endl;
                
                EndDataStep();    
                StopSpss(); 
            }
        \endcode
        \return            The number of variables.
    */
    SPSSXD_API unsigned GetVarCountInDS(const char* dsName,
                                        int& errLevel);
    
    /** Gets the name of the variable at a specified position in a specified dataset.
        \param dsName      The dataset name. Must be the name of a dataset associated with a dataset object in the
                             current data step.  
        \param index       The index position of the variable (0-based).                    
        \param errLevel    The return code. \n
                                0=No error \n
                                17=IBM SPSS Statistics backend is not ready \n
                                89=Function cannot be called outside of a data step or user procedure\n
                                87=Invalid dataset name \n
                                10=Invalid index
        \code
            void func()
            {
                int errLevel = 0;
                const char* cmd ="GET FILE='demo.sav'.";
            
                StartSpss();
                Submit(cmd, strlen(cmd));    
                StartDataStep();
                
                const char* dsname = "example";
                errLevel = CreateDataset(dsname);
                for (int i =0; i < GetVarCountInDS(dsname,errLevel); ++i)
                { 
                    std::cout << GetVarNameInDS(dsname, i, errLevel) << std::endl;
                }
                
                EndDataStep();    
                StopSpss(); 
            }
        \endcode
        \return     The variable name.
    */
    SPSSXD_API const char* GetVarNameInDS(const char* dsName,
                                          const int index,
                                          int& errLevel);
    
     /** Sets the name of the variable at a specified position in a specified dataset.
        \param dsName      The dataset name. Must be the name of a dataset associated with a dataset object in the
                             current data step. 
        \param index       The index position of the variable (0-based).
        \param varName     The variable name.
        \code 
            void func()
            {
                int errLevel = 0;
                const char* cmd ="GET FILE='demo.sav'.";
            
                StartSpss();
                Submit(cmd, strlen(cmd));    
                StartDataStep();
                
                const char* dsname = "example";
                errLevel = CreateDataset(dsname);
                
                for (int i =0; i < GetVarCountInDS(dsname,errLevel); ++i)
                { 
                    char prename[12] = "ds";
                    const char* varname = GetVarNameInDS(dsname, i, errLevel);
                    errLevel = SetVarNameInDS(dsname, i, strcat(prename, varname));
                    std::cout << GetVarNameInDS(dsname, i, errLevel) << std::endl;
                }
                
                EndDataStep();    
                StopSpss(); 
            }    
        \endcode
        \return            The return code. \n
                                0=No error \n
                                17=IBM SPSS Statistics backend is not ready \n
                                89=Function cannot be called outside of a data step or user procedure \n
                                87=Invalid dataset name \n
                                10=Invalid index \n
                                27=Invalid variable name \n
                                25=Duplicate variable name \n
    */                                      
    SPSSXD_API int SetVarNameInDS(const char* dsName,
                                  const int index,
                                  const char* varName);
    
    /** Gets the label for the variable at a specified position in a specified dataset.
        \param dsName      The dataset name. Must be the name of a dataset associated with a dataset object in the
                             current data step.
        \param index       The index position of the variable (0-based).                   
        \param errLevel    The return code. \n
                                0=No error \n
                                17 =IBM SPSS Statistics backend is not ready \n
                                89 =Function cannot be called outside of a data step or user procedure \n
                                87 =Invalid dataset name \n
                                10 =Invalid index \n
                                27 =Invalid variable name
        \code 
            void func()
            {
                int errLevel = 0;
                const char* cmd ="GET FILE='demo.sav'.";
            
                StartSpss();
                Submit(cmd, strlen(cmd));    
                StartDataStep();
                
                const char* dsname = "example";
                errLevel = CreateDataset(dsname);
                for (int i =0; i < GetVarCountInDS(dsname,errLevel); ++i)
                { 
                    std::cout << GetVarLabelInDS(dsname, i, errLevel) << std::endl;
                }
                
                EndDataStep();    
                StopSpss(); 
            }
        \endcode
         \return            The variable label.
    */                              
    SPSSXD_API const char* GetVarLabelInDS(const char* dsName,
                                          const int index,
                                          int& errLevel);
    
    /** Sets the label for the variable at a specified position in a specified dataset.
        \param dsName      The dataset name. Must be the name of a dataset associated with a dataset object in the
                             current data step.
        \param index       The index position of the variable (0-based).
        \param varLabel    The variable label.
        \code 
            void func()
            {
                int errLevel = 0;
                const char* cmd ="GET FILE='demo.sav'.";
            
                StartSpss();
                Submit(cmd, strlen(cmd));    
                StartDataStep();
                
                const char* dsname = "example";
                errLevel = CreateDataset(dsname);
                
                for (int i =0; i < GetVarCountInDS(dsname,errLevel); ++i)
                { 
                    char prelabel[50] = "ds ";
                    const char* varlabel = GetVarLabelInDS(dsname, i, errLevel);
                    errLevel = SetVarLabelInDS(dsname, i, strcat(prelabel, varlabel));
                    std::cout << GetVarLabelInDS(dsname, i, errLevel) << std::endl;
                }
                
                EndDataStep();    
                StopSpss(); 
            }      
        \endcode
        \return            The return code. \n
                                0=No error \n
                                17=IBM SPSS Statistics backend is not ready \n
                                89=Function cannot be called outside of a data step or user procedure \n
                                87=Invalid dataset name \n
                                10=Invalid index \n
    */                                      
    SPSSXD_API int SetVarLabelInDS(const char* dsName,
                                  const int index,
                                  const char* varLabel);
    
    /** Gets the variable type (numeric or string) of the variable at a specified position in a specified dataset.
        \param dsName      The dataset name. Must be the name of a dataset associated with a dataset object in the
                             current data step.
        \param index       The index position of the variable (0-based).                    
        \param errLevel    The return code. \n
                                0=No error \n
                                17 =IBM SPSS Statistics backend is not ready \n
                                89 =Function cannot be called outside of a data step or user procedure  \n
                                87 =Invalid dataset name \n
                                10 =Invalid index
        \code 
            void func()
            {
                int errLevel = 0;
                const char* cmd ="GET FILE='demo.sav'.";
            
                StartSpss();
                Submit(cmd, strlen(cmd));    
                StartDataStep();
                
                const char* dsname = "example";
                errLevel = CreateDataset(dsname);
                for (int i =0; i < GetVarCountInDS(dsname,errLevel); ++i)
                { 
                    std::cout << GetVarTypeInDS(dsname, i, errLevel) << std::endl;
                }
                
                EndDataStep();    
                StopSpss(); 
            }
        \endcode
        \return           The variable type. For numeric variables, the type is 0. For string variables, the type is an integer equal to
                          the defined length (maximum of 32767 bytes).
    */                                  
    SPSSXD_API int GetVarTypeInDS(const char* dsName,
                                  const int index,
                                  int& errLevel);
        
    /** Sets the variable type (numeric or string) for the variable at a specified position in a specified dataset.
        \param dsName      The dataset name. Must be the name of a dataset associated with a dataset object in the
                                          current data step.  
        \param index       The index position of the variable (0-based).     
        \param varType     The variable type--0 for numeric and an integer equal to the defined length for a string variable (maximum of 32767 bytes). 
        \code 
            void func()
            {
                int errLevel = 0;
                const char* cmd ="GET FILE='demo.sav'.";
            
                StartSpss();
                Submit(cmd, strlen(cmd));    
                StartDataStep();
                
                const char* dsname = "example";
                errLevel = CreateDataset(dsname);
                
                errLevel = InsertVariable(dsname, 9, "mark", 8);
                errLevel = SetVarTypeInDS(dsname, 9, 16); 
                
                EndDataStep();    
                StopSpss(); 
            } 
        \endcode     
        \return            The return code. \n
                                0=No error \n
                                17=IBM SPSS Statistics backend is not ready \n
                                89=Function cannot be called outside of a data step or user procedure \n
                                87=Invalid dataset name \n
                                10=Invalid index \n
                                26=Invalid variable type \n
    */                          
    SPSSXD_API int SetVarTypeInDS(const char* dsName,
                                  const int index,
                                  const int varType);
   
    /** Gets the display format of the variable at a specified position in a specified dataset.
        \param dsName      The dataset name. Must be the name of a dataset associated with a dataset object in the 
                             current data step.
        \param index       The index position of the variable (0-based).                   
        \param errLevel    The return code. \n
                                0=No error \n
                                17 =IBM SPSS Statistics backend is not ready \n
                                89 =Function cannot be called outside of a data step or user procedure \n
                                87 =Invalid dataset name \n
                                10 =Invalid index
        \code 
            void func()
            {
                int errLevel = 0;
                const char* cmd ="GET FILE='demo.sav'.";
            
                StartSpss();
                Submit(cmd, strlen(cmd));    
                StartDataStep();
                
                const char* dsname = "example";
                errLevel = CreateDataset(dsname);
                for (int i =0; i < GetVarCountInDS(dsname,errLevel); ++i)
                { 
                    std::cout << GetVarFormatInDS(dsname, i, errLevel) << std::endl;
                }
                
                EndDataStep();    
                StopSpss(); 
            }
        \endcode
        \return            The variable format.
    */                                       
    SPSSXD_API const char* GetVarFormatInDS( const char* dsName,
                                              const int index,
                                              int& errLevel);
    
    /** Sets the display format of the variable at a specified position in a specified dataset.
        and may destroy corresponded case values.
        \param dsName          The dataset name. Must be the name of a dataset associated with a dataset object in the
                                              current data step.    
        \param index           The index position of the variable (0-based).      
        \param formatType      The type code for the format. For instance, 'F' for standard numeric or 'A' for string.
        \param formatWidth     The defined width, which must include any decimal digits and the decimal indicator. 
        \param formatDecimal   The number of digits after the decimal, for numeric formats.    
        \code
            void func()
            {
                int errLevel = 0;
                const char* cmd ="GET FILE='demo.sav'.";
            
                StartSpss();
                Submit(cmd, strlen(cmd));    
                StartDataStep();
                
                const char* dsname = "example";
                errLevel = CreateDataset(dsname);
                
                errLevel = InsertVariable(dsname, 9, "mark", 8);
                errLevel = SetVarFormatInDS(dsname, 9, 1, 16, 0); 
                
                EndDataStep();    
                StopSpss(); 
            }   
        \endcode
        \return            The return code. \n
                                0=No error \n
                                17=IBM SPSS Statistics backend is not ready \n
                                89=Function cannot be called outside of a data step or user procedure \n
                                87=Invalid dataset name \n
                                10=Invalid index
    */                                              
    SPSSXD_API int SetVarFormatInDS(  const char* dsName,
                                      const int index,
                                      const int formatType,
                                      const int formatWidth,
                                      const int formatDecimal);
    
    /** Gets the alignment of the variable at a specified position in a specified dataset.
        \param dsName      The dataset name. Must be the name of a dataset associated with a dataset object in the
                                          current data step.    
        \param index       The index position of the variable (0-based).                    
        \param errLevel    The return code. \n
                                0=No error \n
                                17 =IBM SPSS Statistics backend is not ready \n
                                89 =Function cannot be called outside of a data step or user procedure \n
                                87 =Invalid dataset name \n
                                10 =Invalid index \n
        \code 
            void func()
            {
                int errLevel = 0;
                const char* cmd ="GET FILE='demo.sav'.";
            
                StartSpss();
                Submit(cmd, strlen(cmd));    
                StartDataStep();
                
                const char* dsname = "example";
                errLevel = CreateDataset(dsname);
                for (int i =0; i < GetVarCountInDS(dsname,errLevel); ++i)
                { 
                    std::cout << GetVarAlignmentInDS(dsname, i, errLevel) << std::endl;
                }
                
                EndDataStep();    
                StopSpss(); 
            }
        \endcode
        \return           The variable alignment: 0=left, 1=right, 2=center.
    */                                  
    SPSSXD_API int GetVarAlignmentInDS(const char* dsName,
                                  const int index,
                                  int& errLevel);
    
    /** Sets the alignment for the variable at a specified position in a specified dataset.
        \param dsName          The dataset name. Must be the name of a dataset associated with a dataset object in the
                                              current data step.   
        \param index           The index position of the variable (0-based).      
        \param varAlignment    The variable alignment: 0=left, 1=right, 2=center.
        \code
            void func()
            {
                int errLevel = 0;
                const char* cmd ="GET FILE='demo.sav'.";
            
                StartSpss();
                Submit(cmd, strlen(cmd));    
                StartDataStep();
                
                const char* dsname = "example";
                errLevel = CreateDataset(dsname);
                
                errLevel = SetVarAlignmentInDS(dsname, 0, 0); 
                errLevel = SetVarAlignmentInDS(dsname, 1, 1); 
                errLevel = SetVarAlignmentInDS(dsname, 2, 2); 
                
                EndDataStep();    
                StopSpss(); 
            }  
        \endcode
        \return            the return code.
                                0=No error \n
                                17=IBM SPSS Statistics backend is not ready \n
                                89=Function cannot be called outside of a data step or user procedure \n
                                87=Invalid dataset name \n
                                10=Invalid index \n
                                35=Invalid alignment
    */                                    
    SPSSXD_API int SetVarAlignmentInDS(const char* dsName,
                                      const int index,
                                      const int varAlignment);
    
    /** Gets the measurement level of the variable at a specified position in a specified dataset.
        \param dsName      The dataset name. Must be the name of a dataset associated with a dataset object in the
                       current data step.   
        \param index       The index position of the variable (0-based).                    
        \param errLevel    The return code. \n
                                0=No error \n
                                17 =IBM SPSS Statistics backend is not ready. \n
                                89 =Function cannot be called outside of a data step or user procedure. \n
                                87 =Invalid dataset name. \n
                                10 =Invalid index
        \code
            void func()
            {
                int errLevel = 0;
                const char* cmd ="GET FILE='demo.sav'.";
            
                StartSpss();
                Submit(cmd, strlen(cmd));    
                StartDataStep();
                
                const char* dsname = "example";
                errLevel = CreateDataset(dsname);
                for (int i =0; i < GetVarCountInDS(dsname,errLevel); ++i)
                { 
                    std::cout << GetVarMeasurementLevelInDS(dsname, i, errLevel) << std::endl;
                }
                
                EndDataStep();    
                StopSpss(); 
            }
        \endcode
        \return            The measurement level of the variable: NOMINAL, ORDINAL, SCALE or UNKNOWN.
    */                                  
    SPSSXD_API const char* GetVarMeasurementLevelInDS(const char* dsName,
                                                      const int index,
                                                      int& errLevel);
    
    /** Sets the measurement level of the variable at a specified position in a specified dataset.
        \param dsName      The dataset name. Must be the name of a dataset associated with a dataset object in the
                       current data step.   
        \param index       The index position of the variable (0-based).      
        \param varMeasure  The measurement level of the variable: NOMINAL, ORDINAL or SCALE.
        \code
            void func()
            {
                int errLevel = 0;
                const char* cmd ="GET FILE='demo.sav'.";
            
                StartSpss();
                Submit(cmd, strlen(cmd));    
                StartDataStep();
                
                const char* dsname = "example";
                errLevel = CreateDataset(dsname);
                const char* measureValue[3] = {"NOMINAL", "ORDINAL", "SCALE"};
                for (int i =0; i < GetVarCountInDS(dsname,errLevel); ++i)
                { 
                    errLevel = SetVarMeasurementLevelInDS(dsname, i, measureValue[i % 3]);
                }
                
                EndDataStep();    
                StopSpss(); 
            }    
        \endcode
        \return            The return code. \n
                                0=No error \n
                                17=IBM SPSS Statistics backend is not ready \n
                                89=Function cannot be called outside of a data step or user procedure \n
                                87=Invalid dataset name \n
                                10=Invalid index \n
                                34=Invalid measurement level
    */                                                   
    SPSSXD_API int SetVarMeasurementLevelInDS(const char* dsName,
                                              const int index,
                                              const char* varMeasure);
    
    /** Gets the missing values for a numeric variable at a specified position in a specified dataset.
        \param dsName          The dataset name. Must be the name of a dataset associated with a dataset object in the
                       current data step.   
        \param index           The index position of the variable (0-based).                    
        \param missingFormat   The missing value type: \n
        -3 for a range of values and a single discrete value \n
        -2 for a range of values \n
        0 for no missing values \n
        1 for a single discrete missing value \n
        2 for two discrete missing values \n
        3 for three discrete missing values
        \param missingValue1   For missing value types -3 and -2, this is the start of the range. For missing value types 1,2 and 3 this
                               is the first discrete missing value.
        \param missingValue2   For missing value types -3 and -2, this is the end of the range. For missing value types 2 and 3 this
                               is the second discrete missing value.
        \param missingValue3   For missing value type -3 this is the single discrete missing value. For missing value type 3 this 
                               is the third discrete missing value.
        \code 
            void func()
            {
                int errLevel = 0;
                const char* cmd ="GET FILE='demo.sav'.";
            
                StartSpss();
                Submit(cmd, strlen(cmd));    
                StartDataStep();
                
                const char* dsname = "example";
                errLevel = CreateDataset(dsname);
                for (int i =0; i < GetVarCountInDS(dsname,errLevel); ++i)
                { 
                    int missingformat;
                    double d1,d2,d3;
                    errLevel = GetVarNMissingValuesInDS(dsname, i, missingformat, &d1, &d2, &d3);
                    if ( errLevel == 0 )
                        std::cout << "missing format=" << missingformat \
                                <<" d1=" << d1 << " d2=" <<d2 << " d3=" << d3 << std::endl;
                }
                
                EndDataStep();    
                StopSpss(); 
            }
        \endcode
        \return            The return code. \n
                                0=No error \n
                                17=IBM SPSS Statistics backend is not ready \n
                                56=Cannot use this function with a string variable \n
                                89=Function cannot be called outside of a data step or user procedure \n
                                87=Invalid dataset name \n
                                10=Invalid index
    */                                          
    SPSSXD_API int GetVarNMissingValuesInDS(const char* dsName,
                                          const int index,
                                          int& missingFormat,
                                          double* missingValue1,
                                          double* missingValue2,
                                          double* missingValue3);
    
    /** Gets the missing values for a string variable at a specified position in a specified dataset.
        \param dsName          The dataset name. Must be the name of a dataset associated with a dataset object in the
                       current data step.   
        \param index           The index position of the variable (0-based).                    
        \param missingFormat   The missing value type: \n
        0 for no missing values \n
        1 for a single discrete missing value \n
        2 for two discrete missing values \n
        3 for three discrete missing values
        \param missingValue1   The first missing value, if any.  
        \param missingValue2   The second missing value, if any.
        \param missingValue3   The third missing value, if any.
        \code 
            void func()
            {
                int errLevel = 0;
                const char* cmd ="GET FILE='Employee data.sav'.";
            
                StartSpss();
                Submit(cmd, strlen(cmd));    
                StartDataStep();
                
                const char* dsname = "example";
                errLevel = CreateDataset(dsname);
                const int CMISSING_SIZE = 9;                        
                for (int i =0; i < GetVarCountInDS(dsname,errLevel); ++i)
                { 
                    int varType = GetVarTypeInDS(dsname, i, errLevel);
            
                    if(varType != 0 ) {
                        int missingformat;
                        char c1[CMISSING_SIZE],c2[CMISSING_SIZE],c3[CMISSING_SIZE];
                        errLevel = GetVarCMissingValuesInDS(dsname, i, missingformat, c1, c2, c3);
                        if ( errLevel == 0 )
                            std::cout << "missing format=" << missingformat 
                                      << " c1=" << c1 << " c2=" << c2 << " c3=" << c3 << std::endl;
                    }
                }
                
                EndDataStep();    
                StopSpss(); 
            }
        \endcode
        \return            The return code. \n
                                0=No error \n
                                17=IBM SPSS Statistics backend is not ready \n
                                57=Cannot use this function with a numeric variable  \n
                                89=Function cannot be called outside of a data step or user procedure \n
                                87=Invalid dataset name \n
                                10=Invalid index
    */                                              
    SPSSXD_API int GetVarCMissingValuesInDS(  const char* dsName,
                                              const int index,
                                              int& missingFormat,
                                              char* missingValue1,
                                              char* missingValue2,
                                              char* missingValue3);
        
    /** Sets the missing values for the numeric variable at a specified position in a specified dataset.
        \param dsName          The dataset name. Must be the name of a dataset associated with a dataset object in the
                       current data step.   
        \param index           The index position of the variable (0-based).      
        \param missingFormat   The missing value type: \n
        -3 for a range of values and a single discrete value \n
        -2 for a range of values \n
        0 for no missing values \n
        1 for a single discrete missing value \n
        2 for two discrete missing values \n
        3 for three discrete missing values
        \param missingValue1   For missing value types -3 and -2, this is the start of the range. For missing value types 1,2 and 3 this
                               is the first discrete missing value.
        \param missingValue2   For missing value types -3 and -2, this is the end of the range. For missing value types 2 and 3 this
                               is the second discrete missing value.
        \param missingValue3   For missing value type -3 this is the single discrete missing value. For missing value type 3 this 
                               is the third discrete missing value.
        \code 
            void func()
            {
                int errLevel = 0;
                const char* cmd ="GET FILE='demo.sav'.";
            
                StartSpss();
                Submit(cmd, strlen(cmd));    
                StartDataStep();
                
                const char* dsname = "example";
                errLevel = CreateDataset(dsname);
                
                errLevel = SetVarNMissingValuesInDS(dsname, 0, 3, 0, 100, 300);  
                
                EndDataStep();    
                StopSpss(); 
            }   
        \endcode
        \return            the return code.
                                0=No error \n
                                17=IBM SPSS Statistics backend is not ready \n
                                56=Cannot use this function with a string variable \n
                                89=Function cannot be called outside of a data step or user procedure \n
                                87=Invalid dataset name \n
                                10=Invalid index
    */                                               
    SPSSXD_API int SetVarNMissingValuesInDS(const char* dsName,
                                            const int index,
                                            const int missingFormat,
                                            const double missingValue1,
                                            const double missingValue2,
                                            const double missingValue3);
    
    /** Sets the missing values for the string variable at a specified position in a specified dataset.    
        \param dsName          The dataset name. Must be the name of a dataset associated with a dataset object in the
                       current data step.   
        \param index           The index position of the variable (0-based).      
        \param missingFormat   The missing value type: \n
        0 for no missing values \n
        1 for a single discrete missing value \n
        2 for two discrete missing values \n
        3 for three discrete missing values
        \param missingValue1   The first missing value, if any.  
        \param missingValue2   The second missing value, if any.
        \param missingValue3   The third missing value, if any.    
        \code
            void func()
            {
                int errLevel = 0;

                StartSpss();  
                StartDataStep();
                
                const char* dsname = "example";
                errLevel = CreateDataset(dsname, true);
                errLevel = InsertVariable(dsname, 0, "s1", 8);
                
                errLevel = SetVarCMissingValuesInDS(dsname, 0, 3, "tes1", "test2", "test3");  
                
                EndDataStep();    
                StopSpss(); 
            }
        \endcode
        \return            The return code. \n
                                0=No error \n
                                17=IBM SPSS Statistics backend is not ready \n
                                57=Cannot use this function with a numeric variable \n
                                89=Function cannot be called outside of a data step or user procedure \n
                                87=Invalid dataset name \n
                                10=Invalid index
    */                                        
    SPSSXD_API int SetVarCMissingValuesInDS(const char* dsName,
                                            const int index,
                                            const int missingFormat,
                                            const char* missingValue1,
                                            const char* missingValue2,
                                            const char* missingValue3);
    
    /** Gets the names of any variable attributes for the variable with a specified position in a specified dataset.
        After calling this function, FreeStringArray should be called to free the memory for the names.
        \param dsName      The dataset name. Must be the name of a dataset associated with a dataset object in the
                       current data step.   
        \param index       The index position of the variable (0-based).                    
        \param names       The attribute names.
        \param length      The number of names in the returned list.  
        \return            The return code. \n
                                0=No error \n
                                17=IBM SPSS Statistics backend is not ready \n
                                89=Function cannot be called outside of a data step or user procedure \n
                                87=Invalid dataset name \n
                                10=Invalid index 
        \sa
            GetVarAttributesInDS
        \return            the return code.
                                0=No error \n
                                17=IBM SPSS Statistics backend is not ready \n
                                89=Not dataset workspace \n
                                87=Invalid dataset name \n
                                10=Invalid index
    */                                            
    SPSSXD_API int GetVarAttributesNameInDS(const char* dsName,
                                            const int index,
                                            char*** names,
                                            int& length);
    
    /** Gets the values associated with a particular variable attribute for the variable at a specified position in a given dataset.
        After calling this function, FreeStringArray should be called to free the memory for the values.
        \param dsName      The dataset name. Must be the name of a dataset associated with a dataset object in the
                       current data step.   
        \param index       The index position of the variable (0-based).     
        \param attName     The attribute name.          
        \param values      The returned attribute values.
        \param length      The number of returned attribute values (a given variable attribute can have an array of values).
        \code 
            int errLevel = 0;
            if(!IsBackendReady()){
               errLevel = StartSpss();
            }
            if ( 0 == errLevel ){
                std::string cmd("Data List free /x.");
                Submit(cmd.c_str(),cmd.size());
                cmd = "Begin data.";
                QueueCommandPart(cmd.c_str(),cmd.size());
                cmd = "1";
                QueueCommandPart(cmd.c_str(),cmd.size());
                cmd = "2";
                QueueCommandPart(cmd.c_str(),cmd.size());
                cmd = "3";
                QueueCommandPart(cmd.c_str(),cmd.size());
                cmd = "End data.";
                Submit(cmd.c_str(),cmd.size());
                cmd = "Variable Attributes VARIABLES=x ATTRIBUTE=DemographicVars ('1').";
                Submit(cmd.c_str(),cmd.size());

                StartDataStep();
                
                errLevel = CreateDataset( "*" , false);
                const char *dsname = GetActive(errLevel);
                
                int varIndex = 0;
                char** attrname;
                int attrlen;
                errLevel = GetVarAttributesNameInDS(dsname, varIndex, &attrname, attrlen);  
                if(attrlen>0)
                {
                    char** values;
                    int valen;
                    errLevel = GetVarAttributesInDS(dsname, 0, attrname[0], &values, valen); 
                    std::cout << attrname[0] << ":  " << values[0] << std::endl;
                    FreeStringArray(values,valen);
                    FreeStringArray(attrname,attrlen);
                }
                    
                EndDataStep();    
                StopSpss(); 
            }
        \endcode
        \return            The return code. \n
                                0=No error \n
                                17=IBM SPSS Statistics backend is not ready \n
                                89=Function cannot be called outside of a data step or user procedure \n
                                87=Invalid dataset name \n
                                10=Invalid index
    */                                            
    SPSSXD_API int GetVarAttributesInDS(const char* dsName,
                                        const int index,
                                        const char* attName,
                                        char*** values,
                                        int& length);
    
    /** Sets the value(s) associated with a variable attribute for the variable at a specified position in a specified dataset.
        \param dsName          The dataset name. Must be the name of a dataset associated with a dataset object in the
                       current data step.   
        \param index           The index position of the variable (0-based).      
        \param attrName        Attribute name
        \param attributes      Attribute value(s).  
        \param length          The number of values associated with the attribute (a given variable attribute can have an array of values).
        \code
            void func()
            {
                int errLevel = 0;
                const char* cmd ="GET FILE='demo.sav'.";
            
                StartSpss();
                Submit(cmd, strlen(cmd));    
                StartDataStep();
                
                const char* dsname = "example";
                errLevel = CreateDataset(dsname);
                
                const char* attrname = "test";
                char* attvalue[] = {"a","g","e"};
                errLevel = SetVarAttributesInDS(dsname, 0, attrname,attvalue, 3 );
                    
                EndDataStep();    
                StopSpss(); 
            }
        \endcode
        \return            The return code. \n
                                0=No error \n
                                17=IBM SPSS Statistics backend is not ready \n
                                89=Function cannot be called outside of a data step or user procedure \n
                                87=Invalid dataset name \n
                                10=Invalid index
    */                                        
    SPSSXD_API int SetVarAttributesInDS(const char* dsName,
                                          const int index,
                                          const char* attrName,
                                          char** attributes,
                                          const int length);
    
    /** Deletes a particular variable attribute from the variable at a specified position in a specified dataset.
        \param dsName          The dataset name. Must be the name of a dataset associated with a dataset object in the
                                              current data step.   
        \param index           The index position of the variable (0-based).      
        \param attrName        Name of the attribute to delete.
        \code
            void func()
            {
                int errLevel = 0;
                const char* cmd ="GET FILE='demo.sav'.";
            
                StartSpss();
                Submit(cmd, strlen(cmd));    
                StartDataStep();
                
                const char* dsname = "example";
                errLevel = CreateDataset(dsname);
                
                const char* attrname = "test";
                char* attvalue[] = {"a","g","e"};
                errLevel = SetVarAttributesInDS(dsname, 0, attrname,attvalue, 3 );
                
                errLevel = DelVarAttributesInDS(dsname, 0, attrname );
                    
                EndDataStep();    
                StopSpss(); 
            }
        \endcode
        \return            The return code. \n
                                0=No error \n
                                17=IBM SPSS Statistics backend is not ready \n
                                89=Function cannot be called outside of a data step or user procedure \n
                                87=Invalid dataset name \n
                                10=Invalid index \n
                                96=Specified attribute does not exist
    */                                          
    SPSSXD_API int DelVarAttributesInDS(const char* dsName,
                                          const int index,
                                          const char* attrName);
    

    /** Sets the value(s) associated with a datafile attribute for a specified dataset.
        \param dsName          The dataset name. Must be the name of a dataset associated with a dataset object in the
                       current data step.   
        \param attrName        Attribute name.
        \param attributes      Attribute value(s).  
        \param length          The number of values associated with the attribute (a given datafile attribute can have an array of values).
        \code
            void func()
            {
                int errLevel = 0;
                const char* cmd ="GET FILE='demo.sav'.";
            
                StartSpss();
                Submit(cmd, strlen(cmd));    
                StartDataStep();
                
                const char* dsname = "example";
                errLevel = CreateDataset(dsname);
                
                const char* attrname = "test";
                char* attvalue[] = {"a","g","e"};
                errLevel = SetDataFileAttributesInDS(dsname, attrname,attvalue, 3 );
                    
                EndDataStep();    
                StopSpss(); 
            }
        \endcode
        \return            The return code. \n
                                0=No error \n
                                17=IBM SPSS Statistics backend is not ready \n
                                89=Function cannot be called outside of a data step or user procedure \n
                                87=Invalid dataset name \n
    */                                        
    SPSSXD_API int SetDataFileAttributesInDS(const char* dsName,
                                             const char* attrName,
                                             char** attributes,
                                             const int length);



    /** Gets the value labels for the numeric variable at a specified position in a specified dataset.
        After calling this function, FreeDoubleArray should be called to free the memory for the values
        and FreeStringArray to free the labels.
        \param dsName      The dataset name. Must be the name of a dataset associated with a dataset object in the
                       current data step.   
        \param index       The index position of the variable (0-based).     
        \param values      The values for which value labels are defined.         
        \param labels      The associated value labels.
        \param num         The number of value-label pairs.  
        \return            The return code. \n
                                0=No error \n
                                17=IBM SPSS Statistics backend is not ready \n
                                56=Cannot use this function with a string variable \n
                                89=Function cannot be called outside of a data step or user procedure \n
                                87=Invalid dataset name. \n
                                10=Invalid index
    */                                              
    SPSSXD_API int GetVarNValueLabelInDS( const char* dsName,
                                          const int index,
                                          double** values,
                                          char*** labels,
                                          int& num);
    
    /** Frees the memory associated with a double* object.
        \param array   The double* object
        \param length  The number of elements in the 1-dimension array.
        \return        The return code. \n
                            0=No error
    */                                      
    SPSSXD_API int FreeDoubleArray(double* array, int length);
    
    /** Gets the value labels for the string variable at a specified position in a specified dataset.
        After calling this function, FreeStringArray should be called to free the memory for the values
        and labels.
        \param dsName      The dataset name. Must be the name of a dataset associated with a dataset object in the
                       current data step.   
        \param index       The index position of the variable (0-based).     
        \param values      The values for which value labels are defined.         
        \param labels      The associated value labels.
        \param num         The number of value-label pairs.
        \code 
            void func()
            {
                int errLevel = 0;
                const char* cmd ="GET FILE='demo.sav'.";
            
                StartSpss();
                Submit(cmd, strlen(cmd));    
                StartDataStep();
                
                const char* dsname = "example";
                errLevel = CreateDataset(dsname);
                
                for (int i =0; i < GetVarCountInDS(dsname,errLevel); ++i)
                { 
                    int varType = GetVarTypeInDS(dsname, i, errLevel);
            
                    if ( varType == 0) 
                    {
                        double* values;
                        char**  labels;
                        int num;
                        errLevel = GetVarNValueLabelInDS(dsname, i, &values, &labels, num);  
                        if (num > 0)
                        {
                            std::cout << GetVarNameInDS(dsname, i, errLevel) << std::endl;
                            for (int j = 0; j < num; ++j)
                            {
                                std::cout << "value:\t" << values[j] << "\tlabel:\t" << labels[j] << std::endl;
                            }
                            FreeDoubleArray(values, num);
                            FreeStringArray(labels, num);
            
                        }
                    }
                    else
                    {
                        char**  values;
                        char**  labels;
                        int num;
                        errLevel = GetVarCValueLabelInDS(dsname, i, &values, &labels, num);  
                        if (num > 0)
                        {
                            std::cout << GetVarNameInDS(dsname, i, errLevel) << std::endl;
                            for (int j = 0; j < num; ++j)
                            {
                                std::cout << "value:\t" << values[j] << "\tlabel:\t" << labels[j] << std::endl;
                            }
                            FreeStringArray(values, num);
                            FreeStringArray(labels, num);
            
                        }
                    }
                           
                }
                    
                EndDataStep();    
                StopSpss(); 
            }
        \endcode
        \return            The return code. \n
                                0=No error \n
                                17=IBM SPSS Statistics backend is not ready \n
                                57=Cannot use this function with a numeric variable \n
                                89=Function cannot be called outside of a data step or user procedure \n
                                87=Invalid dataset name \n
                                10=Invalid index
    */    
    SPSSXD_API int GetVarCValueLabelInDS( const char* dsName,
                                          const int index,
                                          char*** values,
                                          char*** labels,
                                          int& num);
    
    /** Adds one value label for the numeric variable at a specified position in a specified dataset.
        \param dsName      The dataset name. Must be the name of a dataset associated with a dataset object in the
                       current data step.   
        \param index       The index position of the variable (0-based).     
        \param value       The value for which a label is being set.         
        \param label       The associated label.
        \code
            void func()
            {
                int errLevel = 0;
                const char* cmd ="GET FILE='demo.sav'.";
            
                StartSpss();
                Submit(cmd, strlen(cmd));    
                StartDataStep();
                
                const char* dsname = "example";
                errLevel = CreateDataset(dsname);
            
                errLevel = SetVarNValueLabelInDS(dsname, 0, 10, "low");
                errLevel = SetVarNValueLabelInDS(dsname, 0, 100, "mid");
                errLevel = SetVarNValueLabelInDS(dsname, 0, 300, "hig");
                                    
                EndDataStep();    
                StopSpss(); 
            }
        \endcode
        \return            The return code. \n
                                0=No error \n
                                17=IBM SPSS Statistics backend is not ready \n
                                56=Cannot use this function with a string variable \n
                                89=Function cannot be called outside of a data step or user procedure \n
                                87=Invalid dataset name \n
                                10=Invalid index
    */                                          
    SPSSXD_API int SetVarNValueLabelInDS(const char* dsName,
                                          const int index,
                                          const double value,
                                          const char* label);
    
    /** Adds one value label for the string variable at a specified position in a specified dataset.
        \param dsName      The dataset name. Must be the name of a dataset associated with a dataset object in the
                       current data step.   
        \param index       The index position of the variable (0-based).     
        \param value       The value for which a label is being set.         
        \param label       The associated label.
        \code 
            void func()
            {
                int errLevel = 0;
                const char* cmd ="GET FILE='Employee data.sav'.";
            
                StartSpss();
                Submit(cmd, strlen(cmd));    
                StartDataStep();
                
                const char* dsname = "example";
                errLevel = CreateDataset(dsname);
            
                errLevel = SetVarCValueLabelInDS(dsname, 1, "f", "lucy");
                errLevel = SetVarCValueLabelInDS(dsname, 1, "m", "jack");
                        
                EndDataStep();    
                StopSpss(); 
            }
        \endcode
        \return            The return code. \n
                                0=No error \n
                                17=IBM SPSS Statistics backend is not ready \n
                                57=Cannot use this function with a numeric variable \n
                                89=Function cannot be called outside of a data step or user procedure \n
                                87=Invalid dataset name \n
                                10=Invalid index
    */                                      
    SPSSXD_API int SetVarCValueLabelInDS(const char* dsName,
                                          const int index,
                                          const char* value,
                                          const char* label);
                                          
    /** Deletes all value labels for the variable at a specified position in a specified dataset.
        \param dsName      The dataset name. Must be the name of a dataset associated with a dataset object in the
                       current data step.   
        \param index       The index position of the variable (0-based).
        \code 
            void func()
            {
                int errLevel = 0;
                const char* cmd ="GET FILE='demo.sav'.";
            
                StartSpss();
                Submit(cmd, strlen(cmd));    
                StartDataStep();
                
                const char* dsname = "example";
                errLevel = CreateDataset(dsname);
            
                errLevel = DelVarValueLabelInDS(dsname, 6);
                
                EndDataStep();    
                StopSpss(); 
            }  
        \endcode
        \return            The return code. \n
                                0=No error \n
                                17=IBM SPSS Statistics backend is not ready \n
                                89=Function cannot be called outside of a data step or user procedure \n
                                87=Invalid dataset name \n
                                10=Invalid index
    */                                      
    SPSSXD_API int DelVarValueLabelInDS(const char* dsName,
                                          const int index);
    
    /** Deletes a particular value label for the numeric variable at a specified position in a specified dataset.
        \param dsName      The dataset name. Must be the name of a dataset associated with a dataset object in the
                       current data step.   
        \param index       The index position of the variable (0-based).     
        \param delValue    The value for which the value label will be removed.
        \return            The return code. \n
                                0=No error \n
                                17=IBM SPSS Statistics backend is not ready \n
                                56=Cannot use this function with a string variable \n
                                89=Function cannot be called outside of a data step or user procedure \n
                                87=Invalid dataset name \n
                                10=Invalid index
    */                                      
    SPSSXD_API int DelVarNValueLabelInDS(const char* dsName,
                                          const int index,
                                          const double delValue);
    
    /** Deletes a particular value label for the string variable at a specified position in a specified dataset.
        \param dsName      The dataset name. Must be the name of a dataset associated with a dataset object in the
                       current data step.   
        \param index       The index position of the variable (0-based).     
        \param delValue    The value for which the value label will be removed.
        \code 
            void func()
            {
                int errLevel = 0;
                const char* cmd ="GET FILE='demo.sav'.";
            
                StartSpss();
                Submit(cmd, strlen(cmd));    
                StartDataStep();
                
                const char* dsname = "example";
                errLevel = CreateDataset(dsname);
            
                errLevel = DelVarValueLabelInDS(dsname, 6);
                    
                for (int i =0; i < GetVarCountInDS(dsname,errLevel); ++i)
                { 
                    int varType = GetVarTypeInDS(dsname, i, errLevel);
            
                    if ( varType == 0) 
                    {
                        double* values;
                        char**  labels;
                        int num;
                        errLevel = GetVarNValueLabelInDS(dsname, i, &values, &labels, num);  
                        if (num > 0)
                        {
                            std::cout << GetVarNameInDS(dsname, i, errLevel) << std::endl;
                            for (int j = 0; j < num; ++j)
                            {
                                errLevel = DelVarNValueLabelInDS(dsname, i, values[j]);
                            }
                            FreeDoubleArray(values, num);
                            FreeStringArray(labels, num);
            
                        }
                    }
                    else
                    {
                        char**  values;
                        char**  labels;
                        int num;
                        errLevel = GetVarCValueLabelInDS(dsname, i, &values, &labels, num);  
                        if (num > 0)
                        {
                            std::cout << GetVarNameInDS(dsname, i, errLevel) << std::endl;
                            for (int j = 0; j < num; ++j)
                            {
                                errLevel = DelVarCValueLabelInDS(dsname, i, values[j]);
                            }
                            FreeStringArray(values, num);
                            FreeStringArray(labels, num);
            
                        }
                    }
                           
                }
                    
                EndDataStep();    
                StopSpss(); 
            }
        \endcode
        \return            The return code. \n
                                0=No error \n
                                17=IBM SPSS Statistics backend is not ready \n
                                57=Cannot use this function with a numeric variable \n
                                89=Function cannot be called outside of a data step or user procedure \n
                                87=Invalid dataset name \n
                                10=Invalid index
    */                                      
    SPSSXD_API int DelVarCValueLabelInDS(const char* dsName,
                                         const int index,
                                         const char* delValue);
    
    /** Inserts an empty case at a specified position in a specified dataset.    
        \param dsName      The dataset name. Must be the name of a dataset associated with a dataset object in the
                       current data step.  
        \param rowIndex    The position at which the case is inserted (0-based).
        \code
            void func()
            {
                int errLevel = 0;
                const char* cmd ="GET FILE='demo.sav'.";
            
                StartSpss();
                Submit(cmd, strlen(cmd));    
                StartDataStep();
                
                const char* dsname = "example";
                errLevel = CreateDataset(dsname);
                
                int i = GetCaseCountInDS(dsname,errLevel);
                for ( --i; i >=0 ; i-=2)
                {
                    errLevel = InsertCase(dsname, i);
                }
                
                EndDataStep();    
                StopSpss(); 
            } 
        \endcode
        \return            The return code.  \n  
                                0=No error \n
                                17=IBM SPSS Statistics backend is not ready \n
                                89=Function cannot be called outside of a data step or user procedure \n
                                87=Invalid dataset name \n
                                97=Invalid row index
    */                         
    SPSSXD_API int InsertCase(const char* dsName,
                              const long rowIndex);
    
    /** Deletes the case at a specified position in a specified dataset.    
        \param dsName      The dataset name. Must be the name of a dataset associated with a dataset object in the
                       current data step.  
        \param rowIndex    The position of the case to be deleted (0-based).    
        \code
            void func()
            {
                int errLevel = 0;
                const char* cmd ="GET FILE='demo.sav'.";
            
                StartSpss();
                Submit(cmd, strlen(cmd));    
                StartDataStep();
                
                const char* dsname = "example";
                errLevel = CreateDataset(dsname);
                
                int i = GetCaseCountInDS(dsname,errLevel);
                for ( --i; i >=0 ; i-=2)
                {
                    errLevel = DeleteCase(dsname, i);
                }
                
                EndDataStep();    
                StopSpss(); 
            }
        \endcode
        \return            The return code.   \n 
                                0=No error \n
                                17=IBM SPSS Statistics backend is not ready \n
                                89=Function cannot be called outside of a data step or user procedure \n
                                87=Invalid dataset name \n
                                97=Invalid row index
    */                              
    SPSSXD_API int DeleteCase(const char* dsName,
                              const long rowIndex);
    
    /** Gets the number of rows in the specified dataset.
        \param dsName      The dataset name. Must be the name of a dataset associated with a dataset object in the
                       current data step.                              
        \param errLevel    The return code. \n
                                0=No error \n
                                17 =IBM SPSS Statistics backend is not ready \n
                                89 =Function cannot be called outside of a data step or user procedure \n
                                87 =Invalid dataset name 
        \code
            void func()
            {
                int errLevel = 0;
                const char* cmd ="GET FILE='demo.sav'.";
            
                StartSpss();
                Submit(cmd, strlen(cmd));    
                StartDataStep();
                
                const char* dsname = "example";
                errLevel = CreateDataset(dsname);
                
                std::cout<< GetCaseCountInDS(dsname,errLevel) << std::endl;
                    
                EndDataStep();    
                StopSpss(); 
            }
        \endcode
        \return            The number of rows.
    */                              
    SPSSXD_API long GetCaseCountInDS(const char* dsName,
                                    int& errLevel );
    
    /** Gets a numeric value (associated with a numeric variable) from a specified row and column of a specified dataset.
        \param dsName      The dataset name. Must be the name of a dataset associated with a dataset object in the
                       current data step.    
        \param rowIndex    The row position (0-based).       
        \param columnIndex The column position (0-based).      
        \param isMissing    Returned parameter specifying whether the retrieved value is missing: 1 for user missing,
                            2 for system missing, and 0 for not missing.                    
        \param errLevel    The return code. \n
                                0=No error \n 
                                17=IBM SPSS Statistics backend is not ready \n
                                89=Function cannot be called outside of a data step or user procedure \n
                                87=Invalid dataset name \n
                                56=Cannot use this function to retrieve the value of a string variable \n
                                10=Invalid column index \n
                                97=Invalid row index
        \return            A cell value.
    */                                  
    SPSSXD_API double GetNCellValue(const char* dsName,
                                    const long rowIndex,
                                    const int columnIndex,
                                    int& isMissing,
                                    int& errLevel );
    
    /** Gets a string value (associated with a string variable) from a specified row and column of a specified dataset.
        \param dsName      The dataset name. Must be the name of a dataset associated with a dataset object in the
                       current data step.    
        \param rowIndex    The row position (0-based).       
        \param columnIndex The column position (0-based).    
        \param isMissing    Returned parameter specifying whether the retrieved value is missing: 1 for user missing, 
                           0 for not missing.
        \param errLevel    the return code.
                                0=No error \n
                                17 =IBM SPSS Statistics backend is not ready \n
                                89 =Not dataset workspace \n
                                87 =Invalid dataset name \n
                                57 =NUMERIC_VARIABLE \n
                                10 =Invalid index \n
                                97 =Invalid row index
        \code
            void func()
            {
                int errLevel = 0;
                const char* cmd ="GET FILE='Employee data.sav'.";
            
                StartSpss();
                Submit(cmd, strlen(cmd));    
                StartDataStep();
                
                const char* dsname = "example";
                errLevel = CreateDataset(dsname);
                
                const int length = 4;
                int types[length];
                for (int i=0; i< length; i++)
                    types[i] = GetVarTypeInDS(dsname, i, errLevel);
            
                for ( int i = GetCaseCountInDS(dsname,errLevel) - 1; i >=0 ; i-=2)
                {
                    for (int j = 0; j < length; ++j)
                    {
                        int isMissing;
                        if ( types[j] == 0 ) 
                        {
                            std::cout<< GetNCellValue(dsname, i, j, isMissing, errLevel) << ",";
                        }
                        else
                        {
                            std::cout<< GetCCellValue(dsname, i, j, isMissing, errLevel) << ",";
                        }
                    }
                    std::cout << std::endl;
                }
                    
                EndDataStep();    
                StopSpss(); 
            }                       
        \endcode
        \return            A cell value.
    */                              
    SPSSXD_API const char* GetCCellValue(const char* dsName,
                                        const long rowIndex,
                                        const int columnIndex,
                                        int& isMissing,
                                        int& errLevel );
    
    /** Sets a numeric value (associated with a numeric variable) in a specified row and column of a specified dataset.
        \param dsName      The dataset name. Must be the name of a dataset associated with a dataset object in the
                       current data step.    
        \param rowIndex    The row position (0-based).       
        \param columnIndex The column position (0-based).    
        \param value       A double value.                       
        \return            The return code. \n
                                0=No error \n
                                17=IBM SPSS Statistics backend is not ready \n
                                89=Function cannot be called outside of a data step or user procedure \n
                                87=Invalid dataset name \n
                                56=Cannot use this function to set the value of a string variable \n
                                10=Invalid column index \n
                                97=Invalid row index \n
    */                                  
    SPSSXD_API int SetNCellValue(const char* dsName,
                                const long rowIndex,
                                const int columnIndex,
                                  const double value );
    
    /** Sets a string value (associated with a string variable) in a specified row and column of a specified dataset.
        \param dsName      The dataset name. Must be the name of a dataset associated with a dataset object in the
                       current data step.    
        \param rowIndex    The row position (0-based).       
        \param columnIndex The column position (0-based).    
        \param value       A string value.    
        \code 
            void func()
            {
                int errLevel = 0;
                const char* cmd ="GET FILE='Employee data.sav'.";
            
                StartSpss();
                Submit(cmd, strlen(cmd));    
                StartDataStep();
                
                const char* dsname = "example";
                errLevel = CreateDataset(dsname);
                
                const int length = 4;
                int types[length];
                for (int i=0; i< length; i++)
                    types[i] = GetVarTypeInDS(dsname, i, errLevel);
            
                for ( int i = 0; i < length ; ++i)
                {
                    errLevel = InsertCase(dsname, i);
                    for (int j = 0; j < length; ++j)
                    {
                        if ( types[j] == 0 ) 
                        {
                            errLevel = SetNCellValue(dsname, i, j, 88);
                        }
                        else
                        {
                            errLevel = SetCCellValue(dsname, i, j, "xx");
                        }
                    }
                }
                    
                EndDataStep();    
                StopSpss(); 
            }                
        \endcode
        \return            The return code. \n
                                0=No error \n
                                17=IBM SPSS Statistics backend is not ready \n
                                89=Function cannot be called outside of a data step or user procedure \n
                                87=Invalid dataset name \n
                                57=Cannot use this function to set the value of a numeric variable \n
                                10=Invalid column index \n
                                97=Invalid row index
    */                              
    SPSSXD_API int SetCCellValue(const char* dsName,
                                const long rowIndex,
                                const int columnIndex,
                                const char* value );

    /** IsUTF8mode is used to determine if the Unicode mode is on.
        \return            The return code. \n
                           0=Unicode is off \n
                           1=Unicode is on
     */
    SPSSXD_API int IsUTF8mode();

    /** Gets the names of any multiple response sets defined for the active dataset. 
        The backend needs to be started before calling this function.
        RemoveStringList must be called to delete memory used by the returned void pointer.
      \param errLevel   The returned er=ror level. \n
                        0=No error \n
                        9=No data source \n
                        17=IBM SPSS Statistics backend is not ready
      \code 
           int err = 0;
           if(!IsBackendReady()){
              err = StartSpss();
            }
           if( 0 == err ){
               std::string cmd("DATA LIST /Newspaper 1 TV 3 Web 5.");
               Submit(cmd.c_str(),cmd.size());
               cmd = "Begin data.";
               QueueCommandPart(cmd.c_str(),cmd.size());
               cmd = "1 1 1";
               QueueCommandPart(cmd.c_str(),cmd.size());
               cmd = "1 1 1";
               QueueCommandPart(cmd.c_str(),cmd.size());
               cmd = "1 1 1";
               QueueCommandPart(cmd.c_str(),cmd.size());
               cmd = "End data.";
               Submit(cmd.c_str(),cmd.size());
               cmd = "MRSETS /MDGROUP NAME=$mltnews LABEL='News sources' VARIABLES=Newspaper TV Web VALUE=1.";
               Submit(cmd.c_str(),cmd.size());
               
               void *result = GetMultiResponseSetNames(err);
               int len = GetStringListLength(result);

               for (int i = 0; i < len; ++i ){
                   const char *mrsetName = GetStringFromList(result,i);

                   char *mrsetLabel, *mrsetCountedValue; 
                   int mrsetCodeAs, mrsetDataType;
                   void *elemVarNames;
                   GetMultiResponseSet(mrsetName,&mrsetLabel,
                                       mrsetCodeAs, &mrsetCountedValue,
                                       mrsetDataType, &elemVarNames, 
                                       err);

                   std::cout << "multiple response set is: " << std::endl;
                   std::cout << "mrsetName = " << mrsetName << std::endl;
                   std::cout << "mrsetLabel = " << mrsetLabel << std::endl;
                   std::cout << "mrsetCodeAs = " << mrsetCodeAs << std::endl;
                   std::cout << "mrsetDataType = " << mrsetDataType << std::endl;
                   
                   int numof = GetStringListLength(elemVarNames);
                   for (int j = 0; j < numof; ++j ){
                       const char *elemVar = GetStringFromList(elemVarNames,j);
                       std::cout << "elemVarName = " << elemVar << std::endl;
                   }
                   FreeString(mrsetLabel);
                   FreeString(mrsetCountedValue);
                   RemoveStringList(elemVarNames);
               }
               RemoveStringList(result);
               
               cmd = "MRSETS  /DISPLAY NAME=[$mltnews].";
               Submit(cmd.c_str(),cmd.size());
           }
      \endcode
      \return   A list of the names of any multiple response sets.
     */

    SPSSXD_API void* GetMultiResponseSetNames(int &errLevel);

    /** Gets the details for a specified multiple response set associated with the active dataset. 
       The backend needs to be started before calling this function.
       FreeString must be called to delete memory used by mrsetLabel and mrsetCountedValue.  
       RemoveStringList must be called to delete memory used by elemVarNames.
      \param mrsetName      The name of the specified multiple response set (input).
      \param mrsetLabel     The label, if any, of the specified multiple response set (output).
      \param mrsetCodeAs    The variable coding of the specified multiple response set (output). \n
                            1 = Categories \n
                            2 = Dichotomies
      \param mrsetCountedValue  The counted value of the specified multiple 
                                response set (output). Only applies to multiple dichotomy sets. If mrsetCodeAs is 1 (Categories), 
                                mrsetCountedValue will always be an empty string.
      \param mrsetDataType  The data type of the elementary variables in the 
                            specified multiple response set (output).
                            Zero for numeric variables or the length in bytes for 
                            string variables.
      \param elemVarNames   The names of the elementary variables in the specified multiple 
                            response set (output).
      \param errLevel   The returned error level. \n
                        0=No error \n
                        9=No data source \n
                        17=IBM SPSS Statistics backend is not ready
      \sa GetMultiResponseSetNames
     */
    SPSSXD_API void  GetMultiResponseSet(const char *mrsetName, 
                                         char **mrsetLabel, 
                                         int &mrsetCodeAs,
                                         char **mrsetCountedValue,
                                         int &mrsetDataType,
                                         void **elemVarNames, 
                                         int &errLevel);

   /** Sets a multiple response set for a specified dataset.
       The backend needs to be started before calling this function.
      \param datasetName    The dataset name. Must be the name of a dataset associated with a dataset object in the
                       current data step.
      \param mrsetName      The name of the multiple response set. The name is a string of maximum length 63 bytes
                                 that must follow IBM SPSS Statistics variable naming conventions.
      \param mrsetLabel     The label, if any, for the multiple response set.
      \param mrsetCodeAs    The variable coding for the multiple response set. \n
                            1   = Categories \n
                            2   = Dichotomies
      \param mrsetCountedValue  The counted value for the multiple response set (only applies to multiple dichotomy sets). 
                                If mrsetCodeAs is 1 (Categories), 
                                mrsetCountedValue should be an empty string.
      \param elemVarNames   The list of the names of the elementary variables in the multiple response set.
      \param numOfVars      The number of elementary variables.
      \param errLevel   The returned error level. \n
                        0=No error \n
                        17=IBM SPSS Statistics backend is not ready \n
                        100 =Invalid multiple response set definition \n
                        101 =Invalid value for multiple response set variable coding \n
                        102 =Invalid multiple response set name
      \code
           int errLevel = 0;
           if(!IsBackendReady()){
              errLevel = StartSpss();
            }
           if( 0 == errLevel ){
               const char* cmd ="GET FILE='demo.sav'.";
               Submit(cmd, strlen(cmd));    
               StartDataStep();
               
               const char* dsname = "example";
               errLevel = CreateDataset(dsname);
               
               errLevel = SetVarNameInDS(dsname, 0, "a");
               std::cout << GetVarNameInDS(dsname, 0, errLevel) << std::endl;
               errLevel = SetVarNameInDS(dsname, 1, "b");
               std::cout << GetVarNameInDS(dsname, 1, errLevel) << std::endl;
               errLevel = SetVarNameInDS(dsname, 2, "c");
               std::cout << GetVarNameInDS(dsname, 2, errLevel) << std::endl;
               
               const char *mrsetName = "$mrset1";
               const char *mrsetLabel = "myMrset1";
               const char *mrsetCountedValue = "";
               int mrsetCodeAs = 1;
               int numOfVars = 2;
               char **elemVarNames;
               elemVarNames = new char*[2];
               elemVarNames[0] = new char[2];
               elemVarNames[1] = new char[2];
               strcpy(elemVarNames[0],"a");
               strcpy(elemVarNames[1],"b");
               
               SetMultiResponseSetInDS(dsname,mrsetName,mrsetLabel,mrsetCodeAs,mrsetCountedValue,
                                    (const char**)elemVarNames,numOfVars,errLevel);
   
               delete []elemVarNames[0];
               delete []elemVarNames[1];
               delete []elemVarNames;
               EndDataStep();    
               StopSpss(); 
           }
      \endcode
    */                                                                          
    SPSSXD_API void SetMultiResponseSetInDS(const char *datasetName, 
                                            const char *mrsetName,
                                            const char *mrsetLabel,
                                            const int mrsetCodeAs,
                                            const char *mrsetCountedValue,
                                            const char **elemVarNames,
                                            const int numOfVars,
                                            int &errLevel);

    /** Gets the names of any multiple response sets associated with the current procedure data source. 
        The backend needs to be started and there needs to be a procedure data source before calling this function.
        RemoveStringList must be called to delete memory used by the returned void pointer.
      \param errLevel   The returned error level. \n
                        0=No error \n
                        17=IBM SPSS Statistics backend is not ready \n
                        60=No cursor is running
      \code 
           int err = 0;
           if(!IsBackendReady()){
              err = StartSpss();
            }
           if( 0 == err ){
               std::string cmd("DATA LIST /Newspaper 1 TV 3 Web 5.");
               Submit(cmd.c_str(),cmd.size());
               cmd = "Begin data.";
               QueueCommandPart(cmd.c_str(),cmd.size());
               cmd = "1 1 1";
               QueueCommandPart(cmd.c_str(),cmd.size());
               cmd = "1 1 1";
               QueueCommandPart(cmd.c_str(),cmd.size());
               cmd = "1 1 1";
               QueueCommandPart(cmd.c_str(),cmd.size());
               cmd = "End data.";
               Submit(cmd.c_str(),cmd.size());
               cmd = "MRSETS /MDGROUP NAME=$mltnews LABEL='News sources' VARIABLES=Newspaper TV Web VALUE=1.";
               Submit(cmd.c_str(),cmd.size());
               
               err = MakeCaseCursor();
               void *result = GetMultiResponseSetNamesInProcDS(err);
               int len = GetStringListLength(result);

               for (int i = 0; i < len; ++i ){
                   const char *mrsetName = GetStringFromList(result,i);

                   char *mrsetLabel, *mrsetCountedValue; 
                   int mrsetCodeAs, mrsetDataType;
                   void *elemVarNames;
                   GetMultiResponseSetInProcDS(mrsetName,&mrsetLabel,
                                               mrsetCodeAs, &mrsetCountedValue,
                                               mrsetDataType, &elemVarNames, 
                                               err);

                   std::cout << "multiple response set is: " << std::endl;
                   std::cout << "mrsetName = " << mrsetName << std::endl;
                   std::cout << "mrsetLabel = " << mrsetLabel << std::endl;
                   std::cout << "mrsetCodeAs = " << mrsetCodeAs << std::endl;
                   std::cout << "mrsetDataType = " << mrsetDataType << std::endl;
                   
                   int numof = GetStringListLength(elemVarNames);
                   for (int j = 0; j < numof; ++j ){
                       const char *elemVar = GetStringFromList(elemVarNames,j);
                       std::cout << "elemVarName = " << elemVar << std::endl;
                   }
                   FreeString(mrsetLabel);
                   FreeString(mrsetCountedValue);
                   RemoveStringList(elemVarNames);
               }
               RemoveStringList(result);
               RemoveCaseCursor();
               
               cmd = "MRSETS  /DISPLAY NAME=[$mltnews].";
               Submit(cmd.c_str(),cmd.size());
           }
      \endcode
      \return   A list of the names of any multiple response sets.
     */
    SPSSXD_API void* GetMultiResponseSetNamesInProcDS(int &errLevel);

   /** Gets the details for a specified multiple response set associated with the current procedure data source. 
       The backend needs to be started and there needs to be a procedure data source before calling this function.
       FreeString must be called to delete memory used by mrsetLabel and mrsetCountedValue.
       RemoveStringList must be called to delete memory used by elemVarNames.
      \param mrsetName      The name of the specified multiple response set (input).
      \param mrsetLabel     The label, if any, for the specified multiple response set (output).
      \param mrsetCodeAs    The variable coding of the specified multiple response set (output). \n
                            1   = Categories \n
                            2   = Dichotomies
      \param mrsetCountedValue  The counted value of the specified multiple 
                                response set (output). Only applies to multiple dichotomy sets. If mrsetCodeAs is 1 (Categories), 
                                mrsetCountedValue will always be an empty string. 
      \param mrsetDataType  The type of the elementary variables in the 
                            specified multiple response set (output).
                            Zero for numeric variables or the length in bytes for 
                            string variables.
      \param elemVarNames   The names of the elementary variables in the specified multiple 
                            response set (output).
      \param errLevel   The returned error level. \n
                        0=No error \n
                        17=IBM SPSS Statistics backend is not ready \n
                        60=No cursor is running

      \sa GetMultiResponseSetNamesInProcDS
     */
    SPSSXD_API void  GetMultiResponseSetInProcDS(const char *mrsetName, 
                                                 char **mrsetLabel, 
                                                 int &mrsetCodeAs,
                                                 char **mrsetCountedValue,
                                                 int &mrsetDataType,
                                                 void **elemVarNames, 
                                                 int &errLevel);
 
    /** Gets the names of any datafile attributes for a specified dataset.
        After calling this function,
        FreeAttrNames should be called to free the memory for the names.
       \param dsName        The name of the dataset (input). Must be the name of a dataset associated with a dataset object in the
                       current data step.
       \param name          The names of any datafile attributes (output).
       \param numOfNames    The number of datafile attributes (output).
       \code
            int errLevel = 0;
            if(!IsBackendReady()){
               errLevel = StartSpss();
            }
            if ( 0 == errLevel ){
                std::string cmd("Data List free /x.");
                Submit(cmd.c_str(),cmd.size());
                cmd = "Begin data.";
                QueueCommandPart(cmd.c_str(),cmd.size());
                cmd = "1";
                QueueCommandPart(cmd.c_str(),cmd.size());
                cmd = "2";
                QueueCommandPart(cmd.c_str(),cmd.size());
                cmd = "3";
                QueueCommandPart(cmd.c_str(),cmd.size());
                cmd = "End data.";
                Submit(cmd.c_str(),cmd.size());
                cmd = "DATAFILE ATTRIBUTE  ATTRIBUTE=OriginalVersion ('1').";
                Submit(cmd.c_str(),cmd.size());

                StartDataStep();
                errLevel = CreateDataset("*", false);
                const char* dsname = GetActive(errLevel);
                
                int numOfNames = 0;
                char **name;
                errLevel = GetDataFileAttributeNamesInDS(dsname,&name,&numOfNames);
                for(int i = 0; i < numOfNames; i++) {
                    int numOfAttr = 0;
                    char **attr;
                    errLevel = GetDataFileAttributesInDS(dsname, name[i], &attr, &numOfAttr);
                    for(int j = 0; j < numOfAttr; j++) {
                         std::cout << name[i] << ": " << attr[j] << std::endl;
                    }
                    FreeAttributes(attr,numOfAttr);
                }
                FreeAttributeNames(name,numOfNames);
                EndDataStep();    
            }
            StopSpss();
       \endcode
       \return The return code. \n
               0=No error \n
               17=IBM SPSS Statistics backend is not ready \n
               87=Invalid dataset name \n
               89=Function cannot be called outside of a data step or user procedure
    */
    SPSSXD_API int GetDataFileAttributeNamesInDS(const char* dsName, 
                                             char ***name, 
                                             int *numOfNames);

    /** Gets the values associated with a specified datafile attribute for a specified dataset.
        After calling this function, call FreeAttributes to free the memory associated with the attribute.
       \param dsName    The dataset name (input). Must be the name of a dataset associated with a dataset object in the
                       current data step.
       \param attrName  The name of the attribute to retrieve (input).
       \param attr      The value(s) associated with the attribute (output).
       \param numOfAttr The number of values associated with the attribute (output). A given datafile attribute can have an array of values. 
       \sa
           GetDataFileAttributeNamesInDS
       \return The return code. \n
               0=No error \n
               17=IBM SPSS Statistics backend is not ready \n
               87=Invalid dataset name \n
               89=Function cannot be called outside of a data step or user procedure
    */
    SPSSXD_API int GetDataFileAttributesInDS(const char* dsName, 
                                         const char *attrName, 
                                         char ***attr, 
                                         int *numOfAttr);

    /** Gets the names of any multiple response sets associated with a specified dataset.
        The backend needs to be started before calling this function.
        RemoveStringList must be called to delete memory used by the returned void pointer.
       \param dsName    The dataset name (input). Must be the name of a dataset associated with a dataset object in the
                       current data step.
       \param errLevel  The returned error level. \n
                        0=No error \n
                        17=IBM SPSS Statistics backend is not ready \n
                        87=Invalid dataset name \n
                        89=Function cannot be called outside of a data step or user procedure
       \code 
           int errLevel = 0;
           if(!IsBackendReady()){
              errLevel = StartSpss();
            }
           if( 0 == errLevel ){
               std::string cmd("DATA LIST /Newspaper 1 TV 3 Web 5.");
               Submit(cmd.c_str(),cmd.size());
               cmd = "Begin data.";
               QueueCommandPart(cmd.c_str(),cmd.size());
               cmd = "1 1 1";
               QueueCommandPart(cmd.c_str(),cmd.size());
               cmd = "1 1 1";
               QueueCommandPart(cmd.c_str(),cmd.size());
               cmd = "1 1 1";
               QueueCommandPart(cmd.c_str(),cmd.size());
               cmd = "End data.";
               Submit(cmd.c_str(),cmd.size());
               cmd = "MRSETS /MDGROUP NAME=$mltnews LABEL='News sources' VARIABLES=Newspaper TV Web VALUE=1.";
               Submit(cmd.c_str(),cmd.size());
               
               StartDataStep();
               errLevel = CreateDataset("*", false);
               const char* dsname = GetActive(errLevel);

               void *result = GetMultiResponseSetNamesInDS(dsname, errLevel);
               int len = GetStringListLength(result);

               for (int i = 0; i < len; ++i ){
                   const char *mrsetName = GetStringFromList(result,i);

                   char *mrsetLabel, *mrsetCountedValue; 
                   int mrsetCodeAs, mrsetDataType;
                   void *elemVarNames;
                   GetMultiResponseSetInDS(dsname, mrsetName,&mrsetLabel,
                                           mrsetCodeAs, &mrsetCountedValue,
                                           mrsetDataType, &elemVarNames, 
                                           errLevel);

                   std::cout << "mrsetName = " << mrsetName << std::endl;
                   std::cout << "mrsetLabel = " << mrsetLabel << std::endl;
                   std::cout << "mrsetCodeAs = " << mrsetCodeAs << std::endl;
                   std::cout << "mrsetDataType = " << mrsetDataType << std::endl;
                   
                   int numof = GetStringListLength(elemVarNames);
                   for (int j = 0; j < numof; ++j ){
                       const char *elemVar = GetStringFromList(elemVarNames,j);
                       std::cout << "elemVarName = " << elemVar << std::endl;
                   }
                   FreeString(mrsetLabel);
                   FreeString(mrsetCountedValue);
                   RemoveStringList(elemVarNames);
               }
               RemoveStringList(result);
               EndDataStep();
               
               cmd = "MRSETS  /DISPLAY NAME=[$mltnews].";
               Submit(cmd.c_str(),cmd.size());
           }
           StopSpss();
      \endcode
      \return   A list of the names of any multiple response sets.
     */
    SPSSXD_API void* GetMultiResponseSetNamesInDS(const char* dsName, int &errLevel);

   /** Gets the details for a specified multiple response set associated with a specified dataset. 
       The backend needs to be started before calling this function.
       FreeString must be called to delete memory used by mrsetLabel and mrsetCountedValue.
       RemoveStringList must be called to delete memory used by elemVarNames.
      \param dsName         The dataset name (input). Must be the name of a dataset associated with a dataset object in the
                       current data step.
      \param mrsetName      The name of the specified multiple response set (input).
      \param mrsetLabel     The label, if any, for the specified multiple response set (output).
      \param mrsetCodeAs    The variable coding of the specified multiple response set (output). \n
                            1   = Categories \n
                            2   = Dichotomies
      \param mrsetCountedValue  The counted value of the specified multiple 
                                response set (output). Only applies to multiple dichotomy sets. If mrsetCodeAs is 1 (Categories), 
                                mrsetCountedValue will always be an empty string.
      \param mrsetDataType  The type of the elementary variables in the 
                            specified multiple response set (output).
                            Zero for numeric variables or the length in bytes for 
                            string variables.
      \param elemVarNames   The names of the elementary variables in the specified multiple 
                            response set (output).
      \param errLevel   The returned error level. \n
                        0=No error \n
                        17=IBM SPSS Statistics backend is not ready \n
                        87=Invalid dataset name \n
                        89=Function cannot be called outside of a data step or user procedure

      \sa GetMultiResponseSetNamesInDS
     */
    SPSSXD_API void  GetMultiResponseSetInDS(const char* dsName, 
                                         const char *mrsetName, 
                                         char **mrsetLabel, 
                                         int &mrsetCodeAs,
                                         char **mrsetCountedValue,
                                         int &mrsetDataType,
                                         void **elemVarNames, 
                                         int &errLevel);    

    /** Gets the display width for the variable at a specified position in a specified dataset.     
      \param dsName     The dataset name. Must be the name of a dataset associated with a dataset object in the
                       current data step.
      \param index      The index position of the variable (0-based).
      \param errLevel   The returned error level. \n
                        0=No error \n
                        10=Invalid index \n
                        17=IBM SPSS Statistics backend is not ready \n
                        87=Invalid dataset name \n
                        89=Function cannot be called outside of a data step or user procedure
      \code
           int errLevel = 0;
           if(!IsBackendReady()){
              errLevel = StartSpss();
            }
           if( 0 == errLevel ){
               std::string cmd("get file = 'demo.sav'.");
               Submit(cmd.c_str(),cmd.size());
               StartDataStep();
               errLevel = CreateDataset("*", false);
               const char* dsname = GetActive(errLevel);
               
               int varColumnWidth = GetVarColumnWidthInDS( dsname, 0, errLevel);
               std::cout << "The returned variable column width is " << varColumnWidth << std::endl;
               EndDataStep();
           }
           StopSpss();
      \endcode
      \return   The returned variable column width.
    */                                
    SPSSXD_API int GetVarColumnWidthInDS(const char* dsName,
                                          const int index,
                                          int& errLevel);   

    /** Sets the display width for the variable at a specified position in a specified dataset.    
      \param dsName     The dataset name. Must be the name of a dataset associated with a dataset object in the
                       current data step.
      \param index      The index position of the variable (0-based).
      \param width      The column width.
      \code
           int errLevel = 0;
           if(!IsBackendReady()){
              errLevel = StartSpss();
            }
           if( 0 == errLevel ){
               std::string cmd("get file = 'demo.sav'.");
               Submit(cmd.c_str(),cmd.size());
               StartDataStep();
               errLevel = CreateDataset("*", false);
               const char* dsname = GetActive(errLevel);
               
               std::cout << "The old width = " << GetVarColumnWidthInDS( dsname, 0, errLevel) << std::endl;
               int varColumnWidth = 50;
               errLevel = SetVarColumnWidthInDS(dsname, 0, varColumnWidth);
               std::cout << "The new width = " << GetVarColumnWidthInDS( dsname, 0, errLevel) << std::endl;
               EndDataStep();
           }
           StopSpss();
      \endcode
      \return   The return code. \n
                        0=No error \n
                        10=Invalid index \n
                        17=IBM SPSS Statistics backend is not ready \n
                        87=Invalid dataset name \n
                        89=Function cannot be called outside of a data step or user procedure \n
                        103=Invalid column width (must be a positive integer) \n
                        108=The variable column width is too long
    */                                
    SPSSXD_API int SetVarColumnWidthInDS(const char* dsName,
                                          const int index,
                                          const int width);

    /** Deletes a specified datafile attribute from a specified dataset.
       \param dsName        The dataset name. Must be the name of a dataset associated with a dataset object in the
                       current data step.
       \param attrName      The name of the attribute to delete.
       \code
            int errLevel = 0;
            if(!IsBackendReady()){
               errLevel = StartSpss();
            }
            if ( 0 == errLevel ){
                std::string cmd("Data List free /x.");
                Submit(cmd.c_str(),cmd.size());
                cmd = "Begin data.";
                QueueCommandPart(cmd.c_str(),cmd.size());
                cmd = "1";
                QueueCommandPart(cmd.c_str(),cmd.size());
                cmd = "2";
                QueueCommandPart(cmd.c_str(),cmd.size());
                cmd = "3";
                QueueCommandPart(cmd.c_str(),cmd.size());
                cmd = "End data.";
                Submit(cmd.c_str(),cmd.size());
                cmd = "DATAFILE ATTRIBUTE  ATTRIBUTE=OriginalVersion ('1').";
                Submit(cmd.c_str(),cmd.size());

                StartDataStep();
                errLevel = CreateDataset("*", false);
                const char* dsname = GetActive(errLevel);
                
                DelDataFileAttributesInDS( dsname, "OriginalVersion" );
                
                //check if the datafile attribute has been removed.
                int numOfNames = 0;
                char **name;
                errLevel = GetDataFileAttributeNamesInDS(dsname,&name,&numOfNames);
                for(int i = 0; i < numOfNames; i++) {
                    int numOfAttr = 0;
                    char **attr;
                    errLevel = GetDataFileAttributesInDS(dsname, name[i], &attr, &numOfAttr);
                    for(int j = 0; j < numOfAttr; j++) {
                         std::cout << name[i] << ": " << attr[j] << std::endl;
                    }
                    FreeAttributes(attr,numOfAttr);
                }
                FreeAttributeNames(name,numOfNames);
                EndDataStep();    
            }
            StopSpss();
       \endcode
       \return The return code. \n
               0=No error \n
               17=IBM SPSS Statistics backend is not ready \n
               96=The given attribute name does not exist
    */
    SPSSXD_API int DelDataFileAttributesInDS(const char* dsName, 
                                             const char *attrName);

    /** Deletes a specified multiple response set from a specified dataset.
       \param dsName    The dataset name. Must be the name of a dataset associated with a dataset object in the
                       current data step
       \param mrsetName The name of the multiple response set.
       \code 
           int errLevel = 0;
           if(!IsBackendReady()){
              errLevel = StartSpss();
            }
           if( 0 == errLevel ){
               std::string cmd("DATA LIST /Newspaper 1 TV 3 Web 5.");
               Submit(cmd.c_str(),cmd.size());
               cmd = "Begin data.";
               QueueCommandPart(cmd.c_str(),cmd.size());
               cmd = "1 1 1";
               QueueCommandPart(cmd.c_str(),cmd.size());
               cmd = "1 1 1";
               QueueCommandPart(cmd.c_str(),cmd.size());
               cmd = "1 1 1";
               QueueCommandPart(cmd.c_str(),cmd.size());
               cmd = "End data.";
               Submit(cmd.c_str(),cmd.size());
               cmd = "MRSETS /MDGROUP NAME=$mltnews LABEL='News sources' VARIABLES=Newspaper TV Web VALUE=1.";
               Submit(cmd.c_str(),cmd.size());
               
               StartDataStep();
               errLevel = CreateDataset("*", false);
               const char* dsname = GetActive(errLevel);

               errLevel = DelMultiResponseSetInDS( dsname, "$mltnews" );
               
               //check if the multiple response set has been removed.
               void *result = GetMultiResponseSetNamesInDS(dsname, errLevel);
               int len = GetStringListLength(result);

               for (int i = 0; i < len; ++i ){
                   const char *mrsetName = GetStringFromList(result,i);

                   char *mrsetLabel, *mrsetCountedValue; 
                   int mrsetCodeAs, mrsetDataType;
                   void *elemVarNames;
                   GetMultiResponseSetInDS(dsname, mrsetName,&mrsetLabel,
                                           mrsetCodeAs, &mrsetCountedValue,
                                           mrsetDataType, &elemVarNames, 
                                           errLevel);

                   std::cout << "mrsetName = " << mrsetName << std::endl;
                   std::cout << "mrsetLabel = " << mrsetLabel << std::endl;
                   std::cout << "mrsetCodeAs = " << mrsetCodeAs << std::endl;
                   std::cout << "mrsetDataType = " << mrsetDataType << std::endl;
                   
                   int numof = GetStringListLength(elemVarNames);
                   for (int j = 0; j < numof; ++j ){
                       const char *elemVar = GetStringFromList(elemVarNames,j);
                       std::cout << "elemVarName = " << elemVar << std::endl;
                   }
                   FreeString(mrsetLabel);
                   FreeString(mrsetCountedValue);
                   RemoveStringList(elemVarNames);
               }
               RemoveStringList(result);
               EndDataStep();
               
               cmd = "MRSETS  /DISPLAY NAME=[$mltnews].";
               Submit(cmd.c_str(),cmd.size());
           }
           StopSpss();
      \endcode
      \return The return code. \n
               0=No error \n
               17=IBM SPSS Statistics backend is not ready \n
               106=The specified multiple response set does not exist
     */
    SPSSXD_API int DelMultiResponseSetInDS(const char* dsName, 
                                           const char *mrsetName);    

    /** GetGraphic displays an R-style graphic in the IBM SPSS Statistics Viewer. The supported formats 
    for the graphic file are: png, jpg and bmp.
        \code
            void func()
            {
                int errLevel = 0;
                errLevel = StartSpss();
                
                const char *GraphicName = "image.png"
                GetGraphic(GraphicName);

                if(IsXDriven()){
                  StopSpss();
                }
            }
        \endcode
        \param filePath A string specifying the path to a graphic file.
    */
    SPSSXD_API void GetGraphic(const char* filePath);
    
    /** GetSPSSLocale returns a string specifying the locale of the IBM SPSS Statistics processor.
        \code
            void func()
            {
                int errLevel = 0;
                errLevel = StartSpss();
                
                const char *localeName = GetSPSSLocale(errLevel);

                if(IsXDriven()){
                  StopSpss();
                }
            }
        \endcode
        \param errLevel The return code. \n
                        0=Success \n
                        17=IBM SPSS Statistics backend is not ready
        \return A string specifying the locale of the IBM SPSS Statistics processor.
    */
    SPSSXD_API const char* GetSPSSLocale(int &errLevel);
    
    /** GetOutputLanguage returns a string specifying the output language of the IBM SPSS Statistics processor.
        \code
            void func()
            {
                int errLevel = 0;
                errLevel = StartSpss();
                
                const char *olangName = GetOutputLanguage(errLevel);

                if(IsXDriven()){
                  StopSpss();
                }
            }
        \endcode
        \param errLevel The return code. \n
                        0   = Success \n
                        17 = IBM SPSS Statistics backend is not ready
        \return A string specifying the output language of the IBM SPSS Statistics processor. The list of languages: \n
                        English \n
                        French \n
                        German \n
                        Italian \n
                        Japanese \n
                        Korean \n
                        Polish \n
                        Russian \n
                        SChinese \n
                        Spanish \n
                        TChinese \n
                        BPortugu
    */
    SPSSXD_API const char* GetOutputLanguage(int& errLevel);

    /** SetOutputLanguage sets the output language of the IBM SPSS Statistics processor.
        \code
            void func()
            {
                int errLevel = 0;
                errLevel = StartSpss();
                
                SetOutputLanguage("German", errLevel);

                if(IsXDriven()){
                  StopSpss();
                }
            }
        \endcode
        \param lang     The string specifying the language. The list of languages: \n
                        English \n
                        French \n
                        German \n
                        Italian \n
                        Japanese \n
                        Korean \n
                        Polish \n
                        Russian \n
                        SChinese \n
                        Spanish \n
                        TChinese \n
                        BPortugu
        \param errLevel The return code. \n
                        0=Success \n
                        17=IBM SPSS Statistics backend is not ready
    */
    SPSSXD_API void SetOutputLanguage(const char *lang, int &errLevel);
    
    /** Return the depth to which the IBM SPSS Statistics backend is being driven.
        \param depth  The nesting depth
        \return The return code. \n
                0=No error  \n
                17=IBM SPSS Statistics backend is not ready
     */
    SPSSXD_API int GetNestDepth(int& depth);    

    /** Return the host application name and version. Before call this API, SetHostAppNameAndVersion(...) should
        be called. Otherwise, it will return empty string. 
        After calling this function, FreeString should be called to free the memory for the hostname and hostver.
        \code
            void func()
            {
                int errLevel = 0;
                errLevel = StartSpss();
                char *hostname = "IBM SPSS Statistics";
                char *hostver = "18.0.0.0";
                errLevel = SetHostAppNameAndVersion(hostname, hostver);
                
                char *name,*version;
                errLevel = GetHostAppNameAndVersion(&name, &version);
                std::cout << "The name = " << name << std::endl;
                std::cout << "The version = " << version << std::endl;
                FreeString(name);
                FreeString(version);

                if(IsXDriven()){
                  StopSpss();
                }
            }
        \endcode
        \param hostname The host application name
        \param hostver  The string host application version
        \return The return code. \n
                0=No error  \n
                17=IBM SPSS Statistics backend is not ready
     */
    SPSSXD_API int GetHostAppNameAndVersion(char **hostname, char **hostver);   

    /** Return the host application copyright. Before call this API, SetHostAppCopyright() should
        be called. Otherwise, it will return empty string.
        After calling this function, FreeString should be called to free the memory for the copyright.
        \code
            void func()
            {
                int errLevel = 0;
                errLevel = StartSpss();
                const char *theCopyright = "the copyright.";
                errLevel = SetHostAppCopyright(theCopyright);
                
                char *copyright;
                errLevel = GetHostAppCopyright(&copyright);
                std::cout << "The copyright = " << copyright << std::endl;
                FreeString(copyright);

                if(IsXDriven()){
                  StopSpss();
                }
            }
        \endcode
        \param copyright The host application copyright
        \return The return code. \n
                0=No error  \n
                17=IBM SPSS Statistics backend is not ready
     */
    SPSSXD_API int GetHostAppCopyright(char **copyright);

    /** Set the host application name and version.
        \code
            void func()
            {
                int errLevel = 0;
                errLevel = StartSpss();
                char *hostname = "IBM SPSS Statistics";
                char *hostver = "18.0.0.0";
                errLevel = SetHostAppNameAndVersion(hostname, hostver);
                
                char *name,*version;
                errLevel = GetHostAppNameAndVersion(&name, &version);
                std::cout << "The name = " << name << std::endl;
                std::cout << "The version = " << version << std::endl;
                FreeString(name);
                FreeString(version);

                if(IsXDriven()){
                  StopSpss();
                }
            }
        \endcode
        \param hostname The host application name
        \param hostver  The string host application version
        \return The return code. \n
                0=No error  \n
                17=IBM SPSS Statistics backend is not ready
     */
    SPSSXD_API int SetHostAppNameAndVersion(const char *hostname, const char *hostver);

    /** Set the host application copyright.
        \code
            void func()
            {
                int errLevel = 0;
                errLevel = StartSpss();
                
                const char *copyright = "the copyright.";
                errLevel = SetHostAppCopyright(copyright);
                
                char *check;
                errLevel = GetHostAppCopyright(&check);
                std::cout << "The copyright = " << check << std::endl;
                FreeString(check);

                if(IsXDriven()){
                  StopSpss();
                }
            }
        \param copyright The host application copyright. Should be shorter than 64.
        \endcode
        \return The return code. \n
                0=No error
     */
    SPSSXD_API int SetHostAppCopyright(const char *copyright);

    /** Get the defined file handles.
        \code
            void func()
            {
                int errLevel = 0;
                errLevel = StartSpss();

                const char *cmd1 = "FILE HANDLE demo1 /NAME='/demo.sav'.";
                Submit(cmd1,strlen(cmd1));
            
                const char *cmd2 = "FILE HANDLE demo2 /NAME='/demo.sav'.";
                Submit(cmd2,strlen(cmd2));
            
                void* result = GetFileHandles(errLevel);
                int size = GetStringListLength(result);
            
            	//if get handle list failure, or handle list is empty
            	if ( 0 != errLevel || 0 == size || 0 == result)
            		return -1;
            
            	for( int i=0; i<size; i+=3) {
            		const char * str = GetStringFromList(result,i);
            		std::cout << "file handle = " << str << std::endl;
            		const char * str1 = GetStringFromList(result,i+1);
            		std::cout << "file name = " << str1 << std::endl;
            		const char * str2 = GetStringFromList(result,i+2);
            		std::cout << "file encoding = " << str2 << std::endl;
            	}
                RemoveStringList(result);

                if(IsXDriven()){
                  StopSpss();
            }
        \endcode
        \param errLevel This is the return code. \n
                        0=Success \n
                        17=IBM SPSS Statistics backend is not ready
        \return The file handles. 
    */
    SPSSXD_API void* GetFileHandles(int& errLevel);
	
  /** GetImage gets image from  workspace, and returns images representation.
      \param handle a case-insensitive name of XML object in the XML workspace.
      \param fileName an image file name which you want to get from XML workspace.
      \param imageSize returned value indicating the size of image.
      \param imageType returned value indicating the type of image:'PNG','JPG','EMF','BMP','VML'.
      \param errCode returned value indicating errors. \n
        0=No error \n
        17=IBM SPSS Statistics backend is not ready
        116=Invalid handle or filename
        \code

    int errCode = 0;
    int imageSize =0;
    char* imageType = 0;

    if(!IsBackendReady()){
        errCode = StartSpss();
    }
    if( 0 == errCode ){
    const char *cmd = "Get file = 'Employee data.sav'.
                                        OMS SELECT CHARTS 
                                        /DESTINATION FORMAT=OXML 
                                        XMLWORKSPACE ='theHandle' 
                                        IMAGES =YES  
                                        CHARTFORMAT =IMAGE 
                                        IMAGEFORMAT =JPG. 
                                        FREQUENCIES VARIABLES=educ 
                                        /BARCHART FREQ 
                                        /ORDER=ANALYSIS. 
                                        OMSEND.";

     Submit(cmd, strlen(cmd));
     const char* image = GetImage("theHandle","000.jpg",imageSize,imageType,errCode);
     RemoveXPathHandle("theHandle");
    }
    if(IsXDriven()){
        StopSpss();
    }
    \endcode
    \return char* value of the result.
  */
	SPSSXD_API const char* GetImage(const char* handle,
		const char* fileName,
		int& imageSize,
		char** imageType,
		int& errcode);

    /**  Adds a Date cell to the pivot table. The Date will be translated.
         The category must be added before calling this function.
         *
         * \param outLine   The outline title for the pivot table.
         * \param title     The title for the pivot table.
         * \param templateName  The OMS table subtype for the pivot table.
         * \param isSplit   Indicates whether or not to enable split file processing for the pivot table.
         * \param dimName   The name of the dimension.
         * \param place     The placement of the dimension. \n
                                0 = row \n
                                1 = column \n
                                2 = layer
         * \param position  The position of the dimension. Starts from 1. The lowest number is the inner dimension.
         * \param hideName  Specifies whether the dimension name is hidden.
         * \param hideLabels    Specifies whether the dimension labels are hidden.
         * \param cellVal   The cell value. Only support "dd-mm-yyyy hh:MM:ss"
         *
           \sa
              SetNumberCell
           \return          The return code. \n
                            0=No error \n
                            17=IBM SPSS Statistics backend is not ready. \n
                            65=No procedure
    */
    SPSSXD_API int SetDateCell(const char* outLine,
                               const char* title,
                               const char* templateName,
                               bool isSplit,
                               const char* dimName,
                               int place,
                               int position,
                               bool hideName,
                               bool hideLabels,
                               const char* cellVal);
                               
    SPSSXD_API const char* TransCode(const char* orig, int& errLevel);
    
    /** Returns a list of tags associated with any active OMS requests.
        * \param errLevel   take the error when to call this function.
                            0=No error \n
                            17=IBM SPSS Statistics backend is not ready.\n
        * \return A list of strings specifying the tags for any active OMS requests. 
                           The structure of this list is the same as returned from the GetHandleList function, 
                           so the functions GetStringListLength and GetStringFromList may be used to obtain 
                           the string value of the handles. RemoveStringList can be used to free the storage 
                           relating to this list.\n

            \code
              int err = 0;
              if(!IsBackendReady()){
              err = StartSpss();
              }
              if( 0 == err ){
                  const char *cmd = "Get file = 'Employee data.sav'.
                  OMS
                  /SELECT TABLES
                  /IF COMMANDS=['Frequencies'] SUBTYPES=['Frequencies']
                  /DESTINATION FORMAT=OXML IMAGES=NO
                  OUTFILE='./temp.xml'
                  /TAG='tag1'.";
                  Submit(cmd, strlen(cmd));
                  void* result = GetOMSTagList(err);
                  int len = GetStringListLength(result);
              if( len > 0 ){
                 const char* str = GetStringFromList(result,0);
              }
              RemoveStringList(result);
              }
              if(IsXDriven()){
                 StopSpss();
              }
         \endcode
    */
    SPSSXD_API void* GetOMSTagList(int& errLevel);
    
    /** The GetSetting function returns the value of an options setting, as set from the SET command. 
       * \param setting    A string specifying the setting to retrieve. The possible values of setting 
                                          are the names of the SET subcommands,  the param can be given either 
                                          in upper case or in lower case for example, "WORKSPACE", "OLANG", "XVERSION"\n.
       * \param errLevel take the error when to call this function.
                                           0=No error \n
                                           17=IBM SPSS Statistics backend is not ready.\n
                                           117=Invalid command.
       * \param option  A string specifying an option associated with the value of the setting 
                                           argument. Currently, this only applies to the MIOUTPUT subcommand of SET,  
                                           for which there is a separate setting for each of the keywords OBSERVED, IMPUTED, 
                                           POOLED, and DIAGNOSTICS. When setting equals MIOUTPUT, option can be set to 
                                           any of those four keywords to obtain the associated value of the keyword - "Yes" or 
                                           "No". By default, option is NULL.\n
        *\return Returns the value of the specified setting, as a string.
        *\remark GetSetting does not support retrieving the value of the MTINDEX subcommand of the SET command. 

         \code
              int errCode = 0;
              if(!IsBackendReady()){
                    errCode = StartSpss();
              }
              if( 0 == errCode ){
                 std::string  aSetting= "OLANG";
                  const char* currentSetting = GetSetting(aSeting.c_str(),errCode);
              }
              if(IsXDriven()){
                   StopSpss();
              }
      \endcode
    */

    SPSSXD_API const char* GetSetting(const char* setting, int &errLevel, const char* option = 0);

    /** To specifies whether temp files are encrypted. By default, temp files are not encrypted.
       * \param encrypted A boolean - 'true' to encrypt temp files.
          \return     The return code. \n
                            0=No error \n
                            17=IBM SPSS Statistics backend is not ready. \n
                            65=No procedure
    */
    SPSSXD_API int EncryptTempFiles(bool encrypted);
    
    /** GetVariableRole returns the role of the indexed variable in the current active file.
  The backend needs to be started and there needs to be an active datasource before calling this function.
  The index value must be within bounds of the variable count.
    \param index The zero based index of the variable.
    \param errCode Returned value indicates error code. \n
                  0=No error \n
                  9=No data source \n
                  10=Invalid index \n
                  17=IBM SPSS Statistics backend is not ready
    \code
        int err = 0;
        if(!IsBackendReady()){
           err = StartSpss();
         }
        if( 0 == err ){
           const char *cmd = "Get file = 'demo.sav'.";
           Submit(cmd, strlen(cmd));
           int varRole = GetVariableRole(0,err);
        }
        if(IsXDriven()){
          StopSpss();
        }
    \endcode
    \return If No Error, the role of the variable.
    */
    SPSSXD_API int GetVariableRole(int index,int& errCode);
    
    /** GetVariableRoleInProcDS returns the role of the indexed variable in the procedure data source.
        There needs to be an active datasource before calling this function.
        The index value must be within bounds of the variable count.
        \param index The zero based index of the variable.
        \param errCode Returned value indicates error code. \n
                0=No error  \n
                10=Invalid index  \n
                17=IBM SPSS Statistics backend is not ready \n
                60=No cursor is running
        \sa
            GetVariableCountInProcDS
        \return if No Error, the role of the variable.
    */
    SPSSXD_API int GetVariableRoleInProcDS(int index,int& errCode);
    
    /** Sets the value of the role attribute of a variable.
      \param varName a null-terminated variable role.
      \param varRole variable role setting.
                 0 = InputRole \n
                 1 = TargetRole \n 
                 2 = BothRoles \n 
                 3 = NoneRole \n 
                 4 = PartitionRole \n
                 5 = SplitRole \n
                 6=  FrequencyRole
      \sa
           SetVarNameAndType
      \return The return code. \n
              0=No error \n
              9=No data source \n
              17=IBM SPSS Statistics backend is not ready \n
              118=Invalid variable role \n
              60=No cursor is running
    */
    SPSSXD_API int SetVarRole(const char * varName,
                              const int varRole);
    
    /** Gets the variable role (InputRole, TargetRole, BothRoles, NoneRole, PartitionRole and SplitRole) of the variable at a specified position in a specified dataset.
        \param dsName      The dataset name. Must be the name of a dataset associated with a dataset object in the
                             current data step.
        \param index       The index position of the variable (0-based).                    
        \param errLevel    The return code. \n
                                0=No error \n
                                17 =IBM SPSS Statistics backend is not ready \n
                                89 =Function cannot be called outside of a data step or user procedure  \n
                                87 =Invalid dataset name \n
                                10 =Invalid index
        \code 
            void func()
            {
                int errLevel = 0;
                const char* cmd ="GET FILE='demo.sav'.";
            
                StartSpss();
                Submit(cmd, strlen(cmd));    
                StartDataStep();
                
                const char* dsname = "example";
                errLevel = CreateDataset(dsname);
                for (int i =0; i < GetVarCountInDS(dsname,errLevel); ++i)
                { 
                    std::cout << GetVarRoleInDS(dsname, i, errLevel) << std::endl;
                }
                
                EndDataStep();    
                StopSpss(); 
            }
        \endcode
        \return The variable role. \n
                 0 = InputRole \n
                 1 = TargetRole \n 
                 2 = BothRoles \n 
                 3 = NoneRole \n 
                 4 = PartitionRole \n
                 5 = SplitRole
    */                                  
    SPSSXD_API int GetVarRoleInDS(const char* dsName,
                                  const int index,
                                  int& errLevel);
                                  
    /** Sets the variable role (InputRole, TargetRole, BothRoles, NoneRole, PartitionRole, SplitRole and FrequencyRole) for the variable at a specified position in a specified dataset.
        \param dsName      The dataset name. Must be the name of a dataset associated with a dataset object in the
                                          current data step.  
        \param index       The index position of the variable (0-based).     
        \param varRole     The variable type--0 for numeric and an integer equal to the defined length for a string variable (maximum of 32767 bytes). 
        \code 
            void func()
            {
                int errLevel = 0;
                const char* cmd ="GET FILE='demo.sav'.";
            
                StartSpss();
                Submit(cmd, strlen(cmd));    
                StartDataStep();
                
                const char* dsname = "example";
                errLevel = CreateDataset(dsname);
                
                errLevel = InsertVariable(dsname, 9, "mark", 8);
                errLevel = SetVarRoleInDS(dsname, 9, 2); 
                
                EndDataStep();    
                StopSpss(); 
            } 
        \endcode     
        \return            The return code. \n
                                0=No error \n
                                17=IBM SPSS Statistics backend is not ready \n
                                89=Function cannot be called outside of a data step or user procedure \n
                                87=Invalid dataset name \n
                                10=Invalid index \n
                                118=Invalid variable role \n
    */                          
    SPSSXD_API int SetVarRoleInDS(const char* dsName,
                                  const int index,
                                  const int varRole);
                                  
    SPSSXD_API int SetGraphicsLabel(const char* displaylabel,
                                    const char* invariantdisplaylabel);  

    SPSSXD_API int SaveDataToTempFile(const char* filename,
                                      int varIndexes[], 
                                      int keepCount);
    
    SPSSXD_API int GetDataFromTempFile(const char* filename, 
                                       int newVarCount, 
                                       int caseLen);
                                       
    SPSSXD_API int* GetSplitEndIndex(int& size, int& errLevel);

    SPSSXD_API int SetMode(int mode = 0);
    
    SPSSXD_API long GetRowCountInTempFile(int& errLevel);
    
    /** Sets value in a specified row of a specified dataset.
        \param dsName      The dataset name. Must be the name of a dataset associated with a dataset object in the
                       current data step.    
        \param rowIndex    The row position (0-based).       
        \param value       The pointer to case value.  
        \param bufferSize  The buffer size.
        \return            The return code. \n
                                0=No error \n
                                17=IBM SPSS Statistics backend is not ready \n
                                89=Function cannot be called outside of a data step or user procedure \n
                                87=Invalid dataset name \n
                                97=Invalid row index \n
    */                                  
    SPSSXD_API int SetCaseValue(const char* dsName,
                                const long rowIndex,
                                bool isCache,
                                void* value,
                                int bufferSize);
                                 
    /** Gets value from a specified row of a specified dataset.
        \param dsName      The dataset name. Must be the name of a dataset associated with a dataset object in the
                       current data step.    
        \param rowIndex    The row position (0-based).
        \param errLevel    The return code. \n
                                0=No error \n 
                                17=IBM SPSS Statistics backend is not ready \n
                                89=Function cannot be called outside of a data step or user procedure \n
                                87=Invalid dataset name \n
                                97=Invalid row index
        \return            A case value.
    */                                  
    SPSSXD_API void* GetCaseValue(const char* dsName,
                                  const long rowIndex,
                                  bool isCache,
                                  int& len,
                                  int& errLevel );
                                  
    /** Get row length of a specified dataset.
        \param dsName      The dataset name. Must be the name of a dataset associated with a dataset object in the
                       current data step.    
        \param errLevel    The return code. \n
                                0=No error \n 
                                17=IBM SPSS Statistics backend is not ready \n
                                89=Function cannot be called outside of a data step or user procedure \n
                                87=Invalid dataset name
        \return            The case length.
    */              
    SPSSXD_API int GetCaseLength(const char* dsName,
                                 int& errLevel );
    
    /** Get OBS index of a specified variable.
        \param dsName      The dataset name. Must be the name of a dataset associated with a dataset object in the
                       current data step.    
        \param errLevel    The return code. \n
                                0=No error \n 
                                17=IBM SPSS Statistics backend is not ready \n
                                89=Function cannot be called outside of a data step or user procedure \n
                                87=Invalid dataset name
        \return            The internal index of data corresponding to variable.
    */     
    SPSSXD_API int GetVarObsIndex(const char* dsName,
                                  const int columnIndex,
                                  int& errLevel );
    
    SPSSXD_API int SetCacheInDS(const char* dsName,
                                bool isCache);
    
    /** IsDistributedMode is used to determine if IBM SPSS Statistics is in distributed mode.
      The returned value is always false when an external program is controlling the SPSS Statistics backend.
      \return true= SPSS Statistics is in distributed mode. \n
              false= SPSS Statistics is not in distributed mode.
    */
    SPSSXD_API bool IsDistributedMode();
    
    /** Returns the length of the xml response.
      @return length of the xml response.
    */
    SPSSXD_API int GetXmlUtf16Length();
    
    SPSSXD_API int IsUseOrFilter(int &isUse);
    
    /** Move variables to a specified position in a specified dataset.
       \param dsName  The dataset name. Must be the name of a dataset associated with a dataset object in the
                               current data step.
       \param varIndexes The list of variables positions which need to be moved.
       \param keepCount The variable count of need to be moved.
       \param index   The position at which to move the variable (0-based).
       \code
       \endcode
       \return        The return code. \n
                            0=No error \n 
                            17=IBM SPSS Statistics backend is not ready \n
                            89=Function cannot be called outside of a data step or user procedure \n
                            87=Invalid dataset name \n
                            10=Invalid index
    */
    SPSSXD_API int MoveVariables(  const char* dsName,
                                   int varIndexes[],
                                   const int keepCount,
                                   const int targetIndex);
    
    SPSSXD_API std::string GetOutputTreeData(int &length);

    SPSSXD_API int* GetTrendsInfoInDS( const char* dsName, int& size, int& errLevel );

    /** GetSelectedRowCountInProcDS returns the number of selected cases in the current IBM SPSS Statistics procedure data source for New UI.
        \param  errCode Returned value indicates error Code. \n
                0=No error  \n
                17=IBM SPSS Statistics backend is not ready \n
                60=No cursor is running
        \return n>0 The number of selected cases.
    */
    SPSSXD_API long GetSelectedRowCountInProcDS(int& errCode);
}

#endif
