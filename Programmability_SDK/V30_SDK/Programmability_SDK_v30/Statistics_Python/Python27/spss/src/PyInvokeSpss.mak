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
#   PyInvokeSpss.mak  - To build Python extension module PyInvokeSpss.pyd
#
# SYNOPSIS
#
#   nmake -f [path]PyInvokeSpss.mak PYTHONVERSION=<pythonversion>
#
# EXAMPLE
#
#   To build 64-bit PyInvokeSpss.pyd.....
#
#   nmake -f PyInvokeSpss.mak
#
# PREREQUISITES:
#
#   Environment variables which must be set prior to the calling of this makefile.
#     PYTHON_HOME - must be defined as the network drive containing the source tree
#
#   Visual Studio 2017 must be installed for building 64-bit extension module
#
# OPTIONS: Environment variables which affect what PyInvokeSpss.mak does
#
#   DEBUG - If defined, CodeView symbols will be included
#
# Copyright (c) 2006, 2022 SPSS Inc. All rights reserved.
#
#########################################################################

#   --  ensure all needed environment variables are set before beginning

!if !defined(PYTHON_HOME)
!       ERROR PYTHON_HOME - must be defined as the network drive containing the source tree (ie PYTHON_HOME=C:\Python25)
!endif

#   --  Specify the extension of the default target
DEFAULT_TARGET=DLL

#   --  Specify the Python version
PYTHON_VERSION = Python310

#   --  Specify the mode of build - Release, or Debug
!if defined(Debug)
MODE = Debug
!elseif defined(DEBUG)
MODE = Debug
!else
MODE = Release
!endif

#   --  Where your source files are put into
SRC_DIR = .

#   --  Pick up the header files
INC_PATH = -I $(PYTHON_HOME)\include \
           -I $(SRC_DIR)

#   --  Pick up the libraries files
LIB_PATH = $(PYTHON_HOME)\libs

#   --  Where the created files will be put into, such as .obj, .res, .dll, .lib
OUT_DIR = $(SRC_DIR)\X64\$(MODE)

VERSIONINF=$(SRC_DIR)\PyInvokeSpss.res

#   --  We need the libraries plus
!if "$(MODE)" == "Debug"
LIBS_LIST = $(PYTHON_VERSION)_d.lib
!else
LIBS_LIST = $(PYTHON_VERSION).lib
!endif

#   -- Define macros
MACROS = -DWIN32 \
         -D_WINDOWS \
         -DPYINVOKESPSS_EXPORTS \
         -D_USRDLL \
         -DHAS_WINDOWS_THREADS=1 \
         -DSTRICT \
         -DMS_WINDOWS=1 \
         -D_STDCALL_SUPPORTED


!if "$(MODE)" == "Debug"
MACROS = $(MACROS) -DDEBUG
!else
MACROS = $(MACROS) -DNDEBUG
!endif

MACROS = $(MACROS) -D_M_AMD64
MACROS = $(MACROS) -D_AMD64_
MACROS = $(MACROS) -U_X86_

#  -- Compile options
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
CPPOPTS = $(CPPOPTS) -Fo$(OUT_DIR)\             # name object file
CPPOPTS = $(CPPOPTS) -Fd$(OUT_DIR)\PyInvokeSpss.pdb     # name pdb file
CPPOPTS = $(CPPOPTS) -Zc:wchar_t

#  -- Link options
LINKOPTS =
LINKOPTS = $(LINKOPTS) -INCREMENTAL:NO                         # override default incremental link
LINKOPTS = $(LINKOPTS) -NOLOGO                                 # Suppresses startup banner
LINKOPTS = $(LINKOPTS) -$(DEFAULT_TARGET)                      # extension of target
LINKOPTS = $(LINKOPTS) -LIBPATH:$(LIB_PATH)                    # override the environmental library path
LINKOPTS = $(LINKOPTS) -SUBSYSTEM:WINDOWS                      # Tells the operating system how to run the .exe file
LINKOPTS = $(LINKOPTS) -OPT:REF                                # Controls LINK optimizations
LINKOPTS = $(LINKOPTS) -OPT:ICF                                # Controls LINK optimizations
!if "$(PYTHONVERSION)" == "3.10"
LINKOPTS = $(LINKOPTS) -EXPORT:PyInit_PyInvokeSpss
!endif
LINKOPTS = $(LINKOPTS) -IMPLIB:$(OUT_DIR)\PyInvokeSpss.lib     # Overrides the default import library name
LINKOPTS = $(LINKOPTS) -PDB:$(OUT_DIR)\PyInvokeSpss.pdb        # Creates a program database (PDB) file
LINKOPTS = $(LINKOPTS) -DEBUG                                  # Creates debugging information
LINKOPTS = $(LINKOPTS) -MANIFEST
LINKOPTS = $(LINKOPTS) -MANIFESTFILE:$(OUT_DIR)\PyInvokeSpss.dll.intermediate.manifest
LINKOPTS = $(LINKOPTS) -MACHINE:X64                            # Specifies the target platform

#  -- Specify the obj files
OBJ_LIST = $(OUT_DIR)\PyInvokeSpss.obj

CC = cl
LD = link

#   --  Do build

default:
  if exist $(OUT_DIR). rd /s/q $(OUT_DIR)
	md $(OUT_DIR)

	$(CC) $(CPPOPTS) $(INC_PATH) $(SRC_DIR)\PyInvokeSpss.cpp
	$(LD) $(LINKOPTS) -OUT:$(OUT_DIR)\PyInvokeSpss.dll $(LIBS_LIST) $(VERSIONINF) $(OBJ_LIST)
	mt.exe /outputresource:"$(OUT_DIR)\PyInvokeSpss.dll;#2" /manifest "$(OUT_DIR)\PyInvokeSpss.dll.intermediate.manifest" /nologo
	copy $(OUT_DIR)\PyInvokeSpss.dll $(OUT_DIR)\PyInvokeSpss.pyd
