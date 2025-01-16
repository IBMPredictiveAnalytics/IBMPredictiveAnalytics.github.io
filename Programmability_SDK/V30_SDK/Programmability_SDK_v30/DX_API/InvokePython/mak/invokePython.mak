#/***********************************************************************
# * Licensed Materials - Property of IBM
# *
# * IBM SPSS Products: Statistics Common
# *
# * (C) Copyright IBM Corp. 1989, 2022
# *
# * US Government Users Restricted Rights - Use, duplication or disclosure
# * restricted by GSA ADP Schedule Contract with IBM Corp.
# ************************************************************************/

##########################################################################
# NAME
#
#   invokePython.mak  - To build InvokePython.dll
#
# USAGE SYNOPSIS: Should invoke nmake from the directory of invokePython.mak
#
#   nmake -f [path]invokePython.mak PYTHONVERSION=<python_version>
#
# EXAMPLE
#
#   To build 64-bit InvokePython.dll.....
#
#   nmake -f invokePython.mak
#
# PREREQUISITES:
#
# PREREQUISITES: Environment variables which must be set prior to the
#       calling of this makefile.
#
#   PYTHON_HOME - must be defined as the installation directory of Python
#
#   Visual Studio 2008 must be installed for building 32-bit and 64-bit .dll
#
# OPTIONS: Environment variables which affect what invokePython.mak does
#
#   DEBUG - If defined, CodeView symbols will be included
#
# Copyright (c) 2008 SPSS Inc. All rights reserved.
#
#########################################################################


#   --  ensure all needed environment variables are set before beginning

!if !defined(PYTHON_HOME)
!       ERROR PYTHON_HOME - must be defined as the installation directory of Python (ie PYTHON_HOME=C:\Python310)
!endif

#   --  Specify the extension of the default target
DEFAULT_TARGET=DLL

#   --  Specify the Python version according to platform.
!if "$(PYTHONVERSION)" == "3.10"
PYTHON_VERSION = Python310
OUTPUT_NAME = InvokePython310
!else
PYTHON_VERSION = Python310
OUTPUT_NAME = InvokePython310
!endif

#   --  Specify the mode of build - Release, or Debug
!if defined(Debug)
MODE = Debug
!elseif defined(DEBUG)
MODE = Debug
!else
MODE = Release
!endif

#   --  Where your source files are put into
SRC_DIR = ..\

#   --  Pick up the header files
INC_PATH = -I$(PYTHON_HOME)\include \
           -I$(SRC_DIR)

#   --  Pick up the libraries files
LIB_PATH = $(PYTHON_HOME)\libs

#   --  Where the created files will be put into, such as .obj, .res, .dll, .lib
OUT_DIR = $(SRC_DIR)\X64\$(MODE)

VERSIONINF=$(SRC_DIR)\InvokePython.res

#   --  We need the libraries plus
!if "$(MODE)" == "Debug"
LIBS_LIST = $(PYTHON_VERSION)_d.lib
!else
LIBS_LIST = $(PYTHON_VERSION).lib
!endif

#   -- Define macros
MACROS = -DINVOKEPYTHON_EXPORTS \
         -D_WINDOWS \
         -D_USRDLL \
         -D_WINDLL \
         -D_MBCS

!if "$(MODE)" == "Debug"
MACROS = $(MACROS) -DDEBUG
!else
MACROS = $(MACROS) -DNDEBUG
!endif

MACROS = $(MACROS) -D_M_AMD64
MACROS = $(MACROS) -D_AMD64_
MACROS = $(MACROS) -U_X86_

CPPOPTS = $(MACROS)
CPPOPTS = $(CPPOPTS) -O2                        # maximize speed
CPPOPTS = $(CPPOPTS) -GS-                       # disable security checks
CPPOPTS = $(CPPOPTS) -Zi                        # enable debugging information
CPPOPTS = $(CPPOPTS) -TP                        # compile all files as .cpp
CPPOPTS = $(CPPOPTS) -MD                        # don't link with LIBCMT.LIB
CPPOPTS = $(CPPOPTS) -W3                        # warning level
CPPOPTS = $(CPPOPTS) -nologo                    # suppress copyright message
CPPOPTS = $(CPPOPTS) -c                         # compile only, no link
CPPOPTS = $(CPPOPTS) -Wp64                      # enable 64 bit porting warnings
CPPOPTS = $(CPPOPTS) -EHsc                      # Specifies the model of exception handling
CPPOPTS = $(CPPOPTS) -FD
CPPOPTS = $(CPPOPTS) -Fo$(OUT_DIR)\$(OUTPUT_NAME).obj     # name object file
CPPOPTS = $(CPPOPTS) -Fd$(OUT_DIR)\$(OUTPUT_NAME).pdb     # name pdb file
CPPOPTS = $(CPPOPTS) -Zc:wchar_t

LINKOPTS =
LINKOPTS = $(LINKOPTS) -INCREMENTAL:NO                         # override default incremental link
LINKOPTS = $(LINKOPTS) -NOLOGO                                 # Suppresses startup banner
LINKOPTS = $(LINKOPTS) -$(DEFAULT_TARGET)                      # extension of target
LINKOPTS = $(LINKOPTS) -LIBPATH:$(LIB_PATH)                    # override the environmental library path
LINKOPTS = $(LINKOPTS) -SUBSYSTEM:WINDOWS                      # Tells the operating system how to run the .exe file
LINKOPTS = $(LINKOPTS) -OPT:REF                                # Controls LINK optimizations
LINKOPTS = $(LINKOPTS) -OPT:ICF                                # Controls LINK optimizations
LINKOPTS = $(LINKOPTS) -IMPLIB:$(OUT_DIR)\$(OUTPUT_NAME).lib     # Overrides the default import library name
LINKOPTS = $(LINKOPTS) -PDB:$(OUT_DIR)\$(OUTPUT_NAME).pdb        # Creates a program database (PDB) file
LINKOPTS = $(LINKOPTS) -DEBUG                                  # Creates debugging information
LINKOPTS = $(LINKOPTS) -MANIFEST
LINKOPTS = $(LINKOPTS) -MANIFESTFILE:$(OUT_DIR)\$(OUTPUT_NAME).dll.intermediate.manifest
LINKOPTS = $(LINKOPTS) -MACHINE:X64                            # Specifies the target platform


CC = cl
LD = link

#   --  Do build

default:
  if exist $(OUT_DIR). rd /s/q $(OUT_DIR)
	md $(OUT_DIR)

	$(CC) $(CPPOPTS) $(INC_PATH) $(SRC_DIR)\invokepython.cpp
	$(LD) $(LINKOPTS) -OUT:$(OUT_DIR)\$(OUTPUT_NAME).dll $(LIBS_LIST) $(VERSIONINF) $(OUT_DIR)\$(OUTPUT_NAME).obj
    mt.exe /outputresource:"$(OUT_DIR)\$(OUTPUT_NAME).dll;#2" /manifest "$(OUT_DIR)\$(OUTPUT_NAME).dll.intermediate.manifest" /nologo
