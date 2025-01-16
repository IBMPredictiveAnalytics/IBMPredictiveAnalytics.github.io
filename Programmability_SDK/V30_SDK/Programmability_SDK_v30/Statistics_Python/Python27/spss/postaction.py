#/***********************************************************************
# * Licensed Materials - Property of IBM 
# *
# * IBM SPSS Products: Statistics Common
# *
# * (C) Copyright IBM Corp. 1989, 2014
# *
# * US Government Users Restricted Rights - Use, duplication or disclosure
# * restricted by GSA ADP Schedule Contract with IBM Corp. 
# ************************************************************************/

""" Adjust the enviroment after running "END PROGRAM" syntax

This module displays python output in IBM SPSS Statistics
and restores the stdout and stderr of python, setting the python output tempfile
"""

#Copyright (C) 2008 by SPSS Inc.

def end():
    global oldstdout,oldstderr
    import preaction,spss,sys,os
    import PyInvokeSpss
    from cursors import PythonCursors

    myCursor = spss.HasCursor()
    if myCursor == 1:
        for cur in PythonCursors.values():
            cur.close()
        cursorwarning="Warning: An open Cursor was detected while exiting a program block. The Cursor has been closed."
        PyInvokeSpss.PostSpssOutput(cursorwarning,len(cursorwarning))

    if PyInvokeSpss.HasProcedure() == 1:
        PyInvokeSpss.EndProcedure()

    #Set the width of output line
    width = 255

    n = len(preaction.wop)
    if n < 1:
        return
    
    preaction.wop[-1].stream.flush()
    preaction.rop[-1].stream.seek(preaction.rop[-1].stream.tell())
    k=preaction.rop[-1].stream.read().split("\n")
    if not k[len(k)-1]:
        del k[len(k)-1]   
    for item in k:
        if not item:
            spss.PostOutput(item)
        else:
            length = len(item)
            loop = length/width
            rm=length%width
            for i in range(loop):
                spss.PostOutput(item[i*width:(i+1)*width])
            if rm > 0:
                spss.PostOutput(item[-rm:])
    preaction.wop[-1].stream.seek(0,0)
    #preaction.wop[-1].truncate(0)
    preaction.wop[-1].stream.close()
    preaction.rop[-1].stream.close()
    del preaction.wop[-1]
    del preaction.rop[-1]


    if len(preaction.wop) > 0:
        sys.stdout = preaction.wop[-1]
        sys.stderr = preaction.wop[-1]
    else:
        sys.stdout = preaction.oldstdout
        sys.stderr = preaction.oldstderr
        for tmpfile in preaction.filelist:
            if os.path.exists(tmpfile):
                os.remove(tmpfile)
       

import version
__version__ = version.version
