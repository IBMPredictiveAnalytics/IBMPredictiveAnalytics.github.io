#!/bin/sh

#/***********************************************************************
# * Licensed Materials - Property of IBM 
# *
# * IBM SPSS Products: Statistics Common
# *
# * (C) Copyright IBM Corp. 1989, 2023
# *
# * US Government Users Restricted Rights - Use, duplication or disclosure
# * restricted by GSA ADP Schedule Contract with IBM Corp. 
# ************************************************************************/

PLATFORM=`uname`

if [ "${SPSS_HOME}" = "" ]; then
   # Exit with error if SPSS_HOME is not set at this point
   echo "* ERROR(`basename $0`): Cannot locate STATISTICSB executable" 1>&2
   exit 1
fi

if [ "${PLATFORM}" != "Darwin" ]; then
   . ${SPSS_HOME}/bin/statsenv.sh
else
   export DYLD_LIBRARY_PATH="${SPSS_HOME}"/lib:"${SPSS_HOME}"/common/ext/bin/spss.common:"${SPSS_HOME}"/ext/bin/spss.common:"${SPSS_HOME}"/JRE/lib
fi

./testxdApp $1

PLATFORM=`uname`
