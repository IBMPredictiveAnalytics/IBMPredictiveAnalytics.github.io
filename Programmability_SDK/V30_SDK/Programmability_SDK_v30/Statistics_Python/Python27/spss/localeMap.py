#/***********************************************************************
# * Licensed Materials - Property of IBM 
# *
# * IBM SPSS Products: Statistics Common
# *
# * (C) Copyright IBM Corp. 1989, 2013
# *
# * US Government Users Restricted Rights - Use, duplication or disclosure
# * restricted by GSA ADP Schedule Contract with IBM Corp. 
# ************************************************************************/


"""
Map the IBM SPSS Statistics locale to the Python locale.
"""

import os, sys, locale

localeDict = {
    "English":[
        "en_US.cp1252",
        "en_US.iso-8859-1",
        "en_US",
        "en",
        "en_US.iso88591",
        "en_US.ISO8859-1"
        ],
    "English-US":[
        "en_US.cp1252",
        "en_US.iso-8859-1",
        "en_US",
        "en",
        "en_US.iso88591",
        "en_US.ISO8859-1"
        ],
    "English-UK":[
        "en_UK.cp1252",
        "en_UK.iso-8859-1",
        "en_GB",
        "en_UK",
        "en_GB.ISO8859-1"
        ],
    "English-Canada":[
        "en_CA.cp1252",
        "en_CA.iso-8859-1",
        "en_CA",
        "en_CA.8859-15"
        ],
    "English-Australia":[
        "en_AU.cp1252",
        "en_CA.iso-8859-1",
        "en_AU",
        "EN_AU",
        "en",
        "en_AU.8859-15"
        ],
    "Japanese":[
        "ja_JP.cp932",
        "ja_JP.sjis",
        "ja_JP.PCK",
        "ja_JP.SJIS",
        "Ja_JP.IBM-943",
        "ja_JP.eucjp"
        ],
    "Korean":[
        "ko_KR.cp949",
        "ko_KR.IBM-eucKR",
        "ko_KR.IBM-euckr",
        "ko_KR.euckr",
        "ko_KR",
        "ko"
        ],
    "Chinese-PRC":[
        "zh_CN.cp936",
        "Zh_CN.GBK",
        "zh_CN.GBK",
        "zh_CN.gbk"
        ],
    "Chinese-Taiwan":[
        "zh_TW.cp950",
        "Zh_TW.big5",
        "zh_TW.BIG5",
        "zh_TW.euctw"
        ],
    "Vietnamese":[
        "vi_VN.cp1258",
        "Vi_VN.IBM-1129",
        "Vi_VN",
        "vi_VN"
        ],
    "Thai":[
        "th_TH.cp874",
        "th_TH",
        "Th_TH"
        ],
    "Russian":[
        "ru_RU.cp1251",
        "ru_RU.ISO8859-5",
        "ru_RU.8859-5",
        "ru",
        "ru_RU"
        ],
    "Polish":[
        "pl_PL.cp1250",
        "pl_PL.ISO8859-2",
        "pl_PL.8859-2",
        "pl_PL"
        ],
    "Greek":[
        "el_GR.cp1253",
        "el_GR.ISO8859-7",
        "el_GR.8859-7",
        "el_GR",
        "EL_GR",
        "el"
        ],
    "Hebrew":[
        "iw_IL.cp1255",
        "iw_IL.ISO8859-8",
        "iw_IL.8859-8",
        "iw_IL",
        "he_IL",
        "iw"
        ],
    "Arabic":[
        "ar_SA.cp1256",
        "ar_SA.ISO8859-6",
        "ar_SA.8859-6",
        "ar_SA",
        "ar"
        ],
    "Spanish":[
        "es_ES.cp1252",
        "es_ES.IBM-1252@euro",
        "es_ES.8859-15@euro",
        "es_ES.8859-15",
        "es_ES.ISO8859-15@euro",
        "es_ES.iso885915@euro",
        "es_ES.ISO8859-15",
        "es_ES.ISO8859-1",
        "es_ES",
        "es",
        "es_ES.iso88591"
        ],
    "German":[
        "de_DE.cp1252",
        "de_DE.IBM-1252@euro",
        "de_DE.IBM-1252",
        "de.ISO8859-15@euro",
        "de_DE.ISO8859-15",
        "de_DE.ISO8859-1",
        "de.8859-15",
        "de.8859-15@euro",
        "de.ISO8859-15",
        "de_DE.iso885915@euro",
        "de_DE@euro",
        "de_DE",
        "de",
        "de_DE.iso88591"
        ],
    "French":[
        "fr_FR.cp1252",
        "fr_FR.IBM-1252@euro",
        "fr_FR.IBM-1252",
        "fr_FR.8859-15",
        "fr_FR.ISO8859-15",
        "fr_FR.iso885915@euro",
        "fr_FR@euro",
        "fr_FR",
        "fr",
        "fr_FR.iso88591",
        "fr_FR.ISO8859-1"
        ],
    "French-Canada":[
        "fr_CA.cp1252",
        "fr_CA",
        "fr_CA.ISO8859-1"
        ],
    "Italian":[
        "it_IT.cp1252",
        "it_IT.IBM-1252@euro",
        "it_IT.IBM-1252",
        "it_IT.8859-15",
        "it_IT.ISO8859-15",
        "it_IT.iso885915@euro",
        "it_IT@euro",
        "it_IT",
        "it",
        "it_IT.iso88591"
        ],
    "Portuguese-Brazil":[
        "pt_BR.cp1252",
        "pt_BR.ISO8859-1",
        "pt_BR.iso88591",
        "pt_BR.8859-15",
        "pt_BR.iso885915",
        "pt_BR.windows-1252"
        ]        
    }

def readKeysLocale(spss_locale):
    return [x for x in localeDict.keys() if x.startswith(spss_locale)]

def readValuesLocale(spss_locale):
    return [x for (x, v) in localeDict.items() if [i for i in v if i.startswith(spss_locale)]]
    #return [x for x in localeDict.keys() if x.startswith(spss_locale)]

def SetSPSSLocaleAsPythonLocale(spss_locale):
    spssLocaleFull = spss_locale.split(".")[0].strip().lower()
    spssLocaleSimple = spssLocaleFull.split("_")[0].strip()
    spssEncoding = spss_locale.split(".")[-1].strip().split("-")[-1].strip()

    py_locale = None

    #Try to set python locale with the keys in localeDict -- that is SPSS Client locale
    py_locale_dict = readKeysLocale(spssLocaleFull)
    if [] == py_locale_dict:
        py_locale_dict = readKeysLocale(spssLocaleSimple)
    if [] == py_locale_dict:
        py_locale_dict = readValuesLocale(spssLocaleFull)
    if [] == py_locale_dict:
        py_locale_dict = readValuesLocale(spssLocaleSimple)
    
    for loc in py_locale_dict:
        try:
            #Try spss locale with encoding.
            py_locale = locale.setlocale(locale.LC_CTYPE, loc + "." + spssEncoding)
            break;
        except:
            continue

    if py_locale is None:
        for loc in py_locale_dict:
            try:
                #Try spss locale without encoding.
                py_locale = locale.setlocale(locale.LC_CTYPE, loc)
                break;
            except:
                continue

    if py_locale is None:
        for k in py_locale_dict:
            loc = localeDict[k]
            try:
                #Try spss locale with encoding.
                py_locale = locale.setlocale(locale.LC_CTYPE, loc)
                break;
            except:
                continue

    if py_locale is None:
        for k in py_locale_dict:
            loc = localeDict[k]
            try:
                #Try spss locale without encoding.
                py_locale = locale.setlocale(locale.LC_CTYPE, loc.split(".")[0].strip())
            except:
                continue
        
    if py_locale is None:
        from spssutil import CheckStr
        py_locale_dict = locale.locale_alias
        py_locale_specifies = [x for x in py_locale_dict.keys() if x.startswith(CheckStr(spssLocaleSimple))]

        for loc in py_locale_specifies:
            try:
                #Try to set python locale to spss locale with spss encoding.
                py_locale = locale.setlocale(locale.LC_CTYPE, loc + "." + spssEncoding)
                break;
            except:
                continue
        if py_locale is None:
            for loc in py_locale_specifies:
                try:
                    #Try to set python locale to spss locale without spss encoding.
                    py_locale = locale.setlocale(locale.LC_CTYPE, loc)
                    break;
                except:
                    continue


__all__ = ["SetSPSSLocaleAsPythonLocale"]

import version
__version__ = version.version


