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
#
# FILE : invokePython.mk
#
# PURPOSE : This file is used to build libInvokePython.so on unix.
#
# USAGE SYNOPSIS:
#       (g)make -f [path]invokePython.mk
#
# PREREQUISITES: Environment variables which must be set prior to the
#       calling of this makefile.
#
#   PYTHON_HOME - must be defined as the installation directory of Python
#
#########################################################################


#   --  ensure all needed environment variables are set before beginning
ifndef PYTHON_HOME
%: forceabort
	@echo "ERROR: PYTHON_HOME not defined"
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
    CFLAGS += -DUNX_LINUX
endif

ifeq ($(MACHINE),Darwin)
    DIRNAME= macosx
    CFLAGS += -DUNX_MACOSX
#   --  Define Mac machine type x86_64, it will be assigned a value only on Mac platform.
    MAC_ARCH_X64= -arch x86_64
endif

INVOKE_PREFIX=libInvokePython310
PYTHON_NAME=python3.10
INVOKE_PYTHON=invokepython310.o


ifeq ($(MACHINE),Darwin)
    INVOKE_NAME=$(INVOKE_PREFIX).dylib
else
    INVOKE_NAME=$(INVOKE_PREFIX).so
endif

#   --  Where your source files are put into
SRC_DIR = ./..

#   --  Where the created files will be put into, such as .o, .so
OUT_DIR = $(SRC_DIR)/$(DIRNAME)


#   --  Pick up the header files
INC_PATH= \
         -I$(PYTHON_HOME)/include/$(PYTHON_NAME) \
         -I$(SRC_DIR)

#   --  Pick up the libraries files
LIB_PATH= \
         -L$(PYTHON_HOME)/lib \
#         -L/space/SunStudio/SUNWspro/lib \
         -L.

#   --  We need the libraries plus
LIBS =  -l$(PYTHON_NAME) -lm -ldl

#   -- Define compile and link options for different platform
ifeq ($(MACHINE), AIX)
	CC=        xlC_r
	CXX=
	LINKCC=    $(CC)
	CFLAGS +=  \
                  -qrtti=all \
                  -qopt=3 \
                  -qstrict \
                  -qlibansi \
                  -qthreaded \
                  -q64
	LFLAGS +=  \
                  -Wl,-brtl \
                  -qrtti=all \
                  -G \
                  -bshared \
                  -q64
endif

ifeq ($(MACHINE), SunOS)
	CC=         CC
	CXX=        g++
	LINKCC=     $(CC)
	CFLAGS +=   -PIC
	CFLAGS +=   -G
	CFLAGS +=   -xarch=v9
	LFLAGS += $(CFLAGS)
#	BFLAGS +=   -x04
endif

ifeq ($(MACHINE),Linux)
    HARDWARE = $(shell uname -i)
    ifeq ($(HARDWARE),ppc64le)
        CC=        xlC_r
        CXX=
        LINKCC=    $(CC)
        CFLAGS +=  \
                      -qrtti=all \
                      -qopt=3 \
                      -qstrict \
                      -qlibansi \
                      -qthreaded \
                      -q64
        LFLAGS +=  \
                      -qrtti=all \
                      -G \
                      -qmkshrobj \
                      -q64
    else
        CC=         g++
        CXX=
        LINKCC=     $(CC)
        CFLAGS +=       -std=c++11 \
                        -fPIC \
                        -O2 \
                        -fexceptions \
                        -pedantic \
                        -Wno-long-long \
                        -D__LINUX__ \
    #                    -DSIZEOF_LONG=4 \
                        -DLING_BIT=64
        LFLAGS +=     -fPIC \
                    -O2 \
                    -fexceptions \
                    -pedantic \
                    -Wl,--export-dynamic \
                    -shared \
                    -Wl,--hash-style=both
    endif

endif

ifeq ($(MACHINE),Darwin)
	CC=         clang++
	CXX=
	LINKCC=     $(CC)
	CFLAGS += -pedantic \
              -Wno-long-long \
              -isysroot /Applications/Xcode.app/Contents/Developer/Platforms/MacOSX.platform/Developer/SDKs/MacOSX.sdk \
              -mmacosx-version-min=10.12 \
              -trigraphs

	LFLAGS += \
              -dynamiclib \
              -pedantic \
              -Wno-long-long \
              -trigraphs \
              -fPIC \
              -Wl,-syslibroot,/Applications/Xcode.app/Contents/Developer/Platforms/MacOSX.platform/Developer/SDKs/MacOSX.sdk \
              -mmacosx-version-min=10.12 \
              -single_module
endif


#   -- Additional flags, which is not required
RM = rm -f
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


#   -- Build libInvokePython.so
.PHONY:all
all:$(INVOKE_NAME)
$(INVOKE_NAME):$(INVOKE_PYTHON)
	$(CC) -o $(OUT_DIR)/$(INVOKE_NAME) $(OUT_DIR)/$(INVOKE_PYTHON) $(LFLAGS) $(MAC_ARCH_X64) $(LIB_PATH) $(LIBS)
	$(RM) $(OUT_DIR)/$(INVOKE_PYTHON)

$(INVOKE_PYTHON): $(SRC_DIR)/invokepython.cpp
	$(CreateDir)
	$(CC) -c $(BFLAGS) $(CFLAGS) $(MAC_ARCH_X64) $(INC_PATH) -o $(OUT_DIR)/$(INVOKE_PYTHON) $(SRC_DIR)/invokepython.cpp


#   -- Clean output files
.PHONY:clean
clean:
	$(RM) $(OUT_DIR)/*.so
	$(RM) $(OUT_DIR)/*.o


.PHONY:export
export:
	cp $(OUT_DIR)/$(INVOKE_NAME) $(SPSS_HOME)/lib
