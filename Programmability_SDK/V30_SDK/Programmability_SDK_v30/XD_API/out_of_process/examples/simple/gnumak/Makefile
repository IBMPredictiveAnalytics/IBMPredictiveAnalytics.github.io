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

#   --  ensure all needed environment variables are set before beginning
ifndef SPSS_HOME
%: forceabort
	@echo "ERROR: SPSS_HOME not defined"
	@exit 1
forceabort: ;
endif

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
INC_PATH= -I../../../../include

#   --  Pick up the libraries files
LIB_PATH= -L../../../lib/$(DIRNAME)

#   --  We need the libraries plus 
LIBS = 	-lspssxd_p

#   -- Define compile and link options for different platform
ifeq ($(MACHINE), AIX)
	CC=        xlC_r -q64
	CXX=
	LINKCC=    $(CC)
	CFLAGS +=  
	LFLAGS += -Wl,-brtl
endif

ifeq ($(MACHINE), SunOS)
# if don't use "-lthread", pthread_create() may fail.
LIBS += -lthread
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
        CFLAGS += -DUNX_LINUX
        LFLAGS = -Wl,--no-as-needed \
                 -ldl
        LIBS += -lpthread
    endif
endif

ifeq ($(MACHINE),Darwin)
				 
LIB_PATH = -L../../../lib/$(DIRNAME) \
		   -L"$(SPSS_HOME)"/lib
    		    
	CC=         g++
	CXX=
	LINKCC=     $(CC)
	
	CFLAGS += \
			  -arch x86_64 \
			  -isysroot \
			  /Applications/Xcode.app/Contents/Developer/Platforms/MacOSX.platform/Developer/SDKs/MacOSX.sdk \
			  -DDARWIN \
			  -pedantic \
	          -Wno-long-long \
	          -trigraphs

	LFLAGS += \
			  -arch x86_64 \
			  -isysroot \
			  /Applications/Xcode.app/Contents/Developer/Platforms/MacOSX.platform/Developer/SDKs/MacOSX.sdk \
	          -pedantic \
	          -Wno-long-long \
	          -trigraphs \
	          -fPIC \
			  -lstdc++
	ADD_LFLAGS = \
    		  -dylib_file @executable_path/../lib/libspsspool.dylib:"$(SPSS_HOME)"/lib/libspsspool.dylib \
    		  -dylib_file @executable_path/../lib/libmccomsps.dylib:"$(SPSS_HOME)"/lib/libmccomsps.dylib \
    		  -dylib_file @executable_path/../lib/libscorelibr.dylib:"$(SPSS_HOME)"/common/ext/bin/spss.common/libscorelibr.dylib \
    		  -dylib_file @executable_path/../lib/libGeneralRegressionModelr.dylib:"$(SPSS_HOME)"/common/ext/bin/spss.common/libGeneralRegressionModelr.dylib \
    		  -dylib_file @executable_path/../lib/libdiscriminant-modelr.dylib:"$(SPSS_HOME)"/common/ext/bin/spss.common/libdiscriminant-modelr.dylib
endif

#   -- Additional flags, which is not required
RM = rm -f
VPATH = src include

ifdef DEBUG 
	CFLAGS +=     -g
endif

LFLAGS+= \
	$(ADD_LFLAGS)

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
ifeq ($(MACHINE),Darwin)
	$(CC) $(LFLAGS) -o $(OUT_DIR)/testxdApp $(LIB_PATH) $(LIBS) -bind_at_load $(OUT_DIR)/testxd.o
else
	$(CC) $(LFLAGS) -o $(OUT_DIR)/testxdApp $(LIB_PATH) $(LIBS) $(OUT_DIR)/testxd.o
endif

testxd.o: $(SRC_DIR)/testxd.cpp 
	$(CreateDir)
	$(CC) -c $(BFLAGS) $(CFLAGS) $(INC_PATH) -o $(OUT_DIR)/testxd.o $(SRC_DIR)/testxd.cpp 


#   -- Clean output files
.PHONY:clean
clean:
	rm -fr $(OUT_DIR)
