##########################################################################

#/***********************************************************************
# * Licensed Materials - Property of IBM 
# *
# * IBM SPSS Products: Statistics Common
# *
# * (C) Copyright IBM Corp. 1989, 2021
# *
# * US Government Users Restricted Rights - Use, duplication or disclosure
# * restricted by GSA ADP Schedule Contract with IBM Corp. 
# ************************************************************************/

#
# FILE : Make
#
# PURPOSE : This file is used to build testxdApp on unix.
#
# USAGE SYNOPSIS:
#       (g)make -f [path]Makefile
#	if you need 64 bit lib on linux,use
#       (g)make -f [path]Makefile
#                                                                  
# Copyright (c) 2005 SPSS Inc. All rights reserved.
#
#########################################################################


MACHINE = $(shell uname)

#   --  Define DIRNAME for different platform
ifeq ($(MACHINE),AIX)
    DIRNAME= aix64
endif

ifeq ($(MACHINE), SunOS)
    DIRNAME= solaris_sparc64
endif

ifeq ($(MACHINE),Linux)
    HARDWARE = $(shell uname -i)
    ifeq ($(HARDWARE),s390x)
        DIRNAME= zlinux64
    else
        ifeq ($(HARDWARE),ppc64le)
            DIRNAME= plinux64
        else
            DIRNAME= lintel64
        endif
    endif
endif

ifeq ($(MACHINE),Darwin)
	DIRNAME= macosx
endif

#   --  Where your source files are put into
SRC_DIR = ./..

#   --  Where the created files will be put into, such as .o, .so
OUT_DIR = $(SRC_DIR)/$(DIRNAME)


#   --  Pick up the header files
INC_PATH= -I.


#   -- Define compile and link options for different platform
ifeq ($(MACHINE), AIX)
	CC=        xlC_r -q64
	CXX=
	LINKCC=    $(CC)
	CFLAGS += -qrtti=all 
	LFLAGS += \
               -Wl,-brtl \
               -qrtti=all
endif

ifeq ($(MACHINE), SunOS)
	CC=         CC
	CXX=        g++
	LINKCC=     $(CC)
	CFLAGS += \
			   -c \
			   -xarch=v9
	LFLAGS = -ldl \
			 -xarch=v9
endif

ifeq ($(MACHINE),Linux)
    HARDWARE = $(shell uname -i)
    ifeq ($(HARDWARE),ppc64le)
        CC=        xlC_r -q64
        CXX=
        LINKCC=    $(CC)
        CFLAGS += \
                  -qrtti
        LFLAGS += \
                  -qrtti \
                  -Wl,--export-dynamic \
                  -Wl,--hash-style=both
    else
        CC=         g++
        CXX=        
        LINKCC=     $(CC)
        CFLAGS +=   
        LFLAGS = -Wl,--no-as-needed \
                                  -ldl
    endif
endif

ifeq ($(MACHINE),Darwin)

	CC=         g++
	CXX=
	LINKCC=     $(CC)
	CFLAGS += \
			  -DDARWIN \
			  -pedantic \
	          -Wno-long-long \
	          -trigraphs

	LFLAGS += \
	          -pedantic \
	          -Wno-long-long \
	          -trigraphs \
	          -fPIC \
	          -Wl,-syslibroot,/Applications/Xcode.app/Contents/Developer/Platforms/MacOSX.platform/Developer/SDKs/MacOSX.sdk
endif

#   -- Additional flags, which is not required
RM = rm
VPATH = src include

ifdef DEBUG 
	CFLAGS +=     -g
endif


#   -- Create directory $(OUT_DIR) if it doesn't already exist.
define CreateDir
if [ ! -d $(OUT_DIR) ]; then \
   (umask 002; set -x; mkdir $(OUT_DIR) ); \
fi
endef


#   -- Build testxdApp
.PHONY:all
all:testxdApp
testxdApp:testxd.o 
	$(CC) $(LFLAGS) -o $(OUT_DIR)/testxdApp -g $(OUT_DIR)/testxd.o 

testxd.o:$(SRC_DIR)/testxd.cpp $(SRC_DIR)/testxd.h
	$(CreateDir)
	$(CC) $(CFLAGS) -c -o $(OUT_DIR)/testxd.o -g $(SRC_DIR)/testxd.cpp



#   -- Clean output files
.PHONY:clean
clean:
	$(RM) -fr $(OUT_DIR)
