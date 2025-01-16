#/***********************************************************************
# * Licensed Materials - Property of IBM 
# *
# * IBM SPSS Products: Statistics Common
# *
# * (C) Copyright IBM Corp. 1989, 2011
# *
# * US Government Users Restricted Rights - Use, duplication or disclosure
# * restricted by GSA ADP Schedule Contract with IBM Corp. 
# ************************************************************************/


"""
This module maintains version information for the IBM SPSS Statistics-Python package.
"""
#Copyright (C) 2008 by SPSS Inc.

# The version of the IBM SPSS Statistics-Python Integration Plug-In.
# You should not change the version number,
# otherwise the IBM SPSS Statistics-Python package may not function properly.
import os
import ConfigParser
xdconfig = ConfigParser.ConfigParser()
xdcfgfile = os.path.join(os.path.dirname(os.path.abspath(__file__)), 'spssxdcfg.ini')
assert os.path.exists(xdcfgfile)
xdconfig.read(xdcfgfile)
version = xdconfig.get('version','SpssxdVersion')
