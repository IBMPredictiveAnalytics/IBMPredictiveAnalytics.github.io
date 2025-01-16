#/***********************************************************************
# * Licensed Materials - Property of IBM 
# *
# * IBM SPSS Products: Statistics Common
# *
# * (C) Copyright IBM Corp. 1989, 2015
# *
# * US Government Users Restricted Rights - Use, duplication or disclosure
# * restricted by GSA ADP Schedule Contract with IBM Corp. 
# ************************************************************************/

"""Convert a IBM SPSS Statistics variable into a Python data type based on the display format
for the variable."
"""
#Copyright (C) 2008 by SPSS Inc.

import FIELD_TYPE

conversions = {
    FIELD_TYPE.A:str,
    FIELD_TYPE.AHEX:str,
    FIELD_TYPE.COMMA:float,
    FIELD_TYPE.DOLLAR:float,
    FIELD_TYPE.F:float,
    FIELD_TYPE.IB:float,
    FIELD_TYPE.IBHEX:float,
    FIELD_TYPE.P:float,
    FIELD_TYPE.PIB:float,
    FIELD_TYPE.PK:float,
    FIELD_TYPE.RB:float,
    FIELD_TYPE.RBHEX:float,
    FIELD_TYPE.Z:float,
    FIELD_TYPE.N:float,
    FIELD_TYPE.E:float,
    FIELD_TYPE.DATE:float,
    FIELD_TYPE.TIME:float,
    FIELD_TYPE.DATETIME:float,
    FIELD_TYPE.ADATE:float,
    FIELD_TYPE.JDATE:float,
    FIELD_TYPE.DTIME:float,
    FIELD_TYPE.WKDAY:float,
    FIELD_TYPE.MONTH:float,
    FIELD_TYPE.MOYR:float,
    FIELD_TYPE.QYR:float,
    FIELD_TYPE.WKYR:float,
    FIELD_TYPE.PERCENT:float,
    FIELD_TYPE.DOT:float,
    FIELD_TYPE.CCA:float,
    FIELD_TYPE.CCB:float,
    FIELD_TYPE.CCC:float,
    FIELD_TYPE.CCD:float,
    FIELD_TYPE.CCE:float,
    FIELD_TYPE.EDATE:float,
    FIELD_TYPE.SDATE:float,
    FIELD_TYPE.G:float,
    #don't know the store type.
    FIELD_TYPE.LNUMBER:float,
    FIELD_TYPE.LDATE:float,
    FIELD_TYPE.LTIME:float,
    FIELD_TYPE.LCA:float,
    FIELD_TYPE.LCB:float,
    FIELD_TYPE.LCC:float,
    FIELD_TYPE.LCD:float,
    FIELD_TYPE.LCE:float,
    FIELD_TYPE.LCF:float,
    FIELD_TYPE.LCG:float,
    FIELD_TYPE.LCH:float,
    FIELD_TYPE.LCI:float,
    FIELD_TYPE.LCJ:float,
    #Extended date/time formats
    FIELD_TYPE.MTIME:float,
    FIELD_TYPE.YMDHMS:float
    }

import version
__version__ = version.version
