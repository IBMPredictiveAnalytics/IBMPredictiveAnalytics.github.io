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

#!/usr/bin/env python
"""
File : setup.py

Purpose : This file is used to build spss-python package

Usage Synopsis:

Creating spss-python source package:
  python [path]setup.py sdist --force-manifest

Creating spss-python package installer for Win32 platform:
  python [path]setup.py bdist_wininst

Building spss-python binary package, not installing it:
  python [path]setup.py build

Building spss-python binary package, and installing it:
  python [path]setup.py install

Building PyInvokeSpss extension module:
  python [path]setup.py build_ext [Options]

Options for 'build_ext' command:
  --build-lib (-b)     directory for compiled extension modules
  --build-temp (-t)    directory for temporary files (build by-products)
  --inplace (-i)       ignore build-lib and put compiled extensions into the
                       source directory alongside your pure Python modules
  --include-dirs (-I)  list of directories to search for header files
  --define (-D)        C preprocessor macros to define
  --undef (-U)         C preprocessor macros to undefine

Default Location of Output files:
  -- Binary package:
     [path]build/lib.platformInfo-pythonversion/spss, if not install the package
     <Python_home>/Lib/site-packages/spss, if install the package
  -- Source package:
     [path]dist
  -- Win32 installer:
     [path]dist

"""

#Copyright (C) 2008 by SPSS Inc.

import os
import sys
from distutils.core import Extension, setup
from distutils.sysconfig import get_python_lib

EXTRA_LINK_ARGS = EXTRA_COMPILE_ARGS = LIBRARY_DIRS = []
if sys.platform == 'win32':
    pass
elif sys.platform.lower().find('aix') > -1:
    os.environ['CXX'] = ''
    if "PYTHON_HOME" in os.environ:
        EXTRA_LINK_ARGS = ['-brtl -G',
                           '-L'+os.environ['PYTHON_HOME']+'/lib'
                          ]
    else:
        raise "PYTHON_HOME must be set before run setup."
elif sys.platform.lower().find('sunos5') > -1:
    os.environ['CC'] = 'CC'
    os.environ['CXX'] = 'CC'
    os.environ['CFLAGS'] = '-xarch=v9 -mt -xO2'
    os.environ['LINKCC'] = os.environ['CC']
    EXTRA_LINK_ARGS = ['-lCrun']
elif sys.platform.lower().find('darwin') > -1:
    if os.uname()[2].split('.')[0] == '13':
        os.environ['CC'] = 'clang'
        os.environ['CXX'] = 'clang++'
        os.environ['CFLAGS'] = '-arch x86_64 -isysroot /Applications/Xcode.app/Contents/Developer/Platforms/MacOSX.platform/Developer/SDKs/MacOSX10.8.sdk -mmacosx-version-min=10.8 -fno-strict-aliasing -fno-common -dynamic'
        os.environ['LINKCC'] = os.environ['CC']
        os.environ['LDSHARED'] = '-g -bundle -undefined dynamic_lookup'
        EXTRA_LINK_ARGS = ['-arch x86_64','-lstdc++','-Wl,-syslibroot,/Applications/Xcode.app/Contents/Developer/Platforms/MacOSX.platform/Developer/SDKs/MacOSX10.8.sdk -mmacosx-version-min=10.8']
    else:
        os.environ['CC'] = 'gcc'
        os.environ['CXX'] = 'gcc'
        os.environ['LINKCC'] = os.environ['CC']
        EXTRA_LINK_ARGS = ['-arch x86_64','-lstdc++']
elif sys.platform.lower().find('linux') > -1:
    EXTRA_LINK_ARGS = ['-Wl,--hash-style=both']
elif sys.platform.lower().find('hp-ux11') > -1:
    os.environ['CC'] = 'aCC'
    os.environ['CXX'] = 'aCC'
    os.environ['LINKCC'] = os.environ['CC']
    LIBRARY_DIRS = [ '/usr/lib/hpux64',
            os.environ['PYTHON_HOME']+'/lib']
    EXTRA_LINK_ARGS = [ '-mt',
                        '-lCsup',
                        '-ldl',
                        '-ldld', '-lc', '-lm', '-lrt']

def setup_package():
    old_path = os.getcwd()
    local_path = os.path.dirname(os.path.abspath(sys.argv[0]))
    os.chdir(local_path)
    sys.path.insert(0,local_path)
    sys.path.insert(0,os.path.join(local_path,'spss'))

    try:
        from version import version as spss_plugin_version
        major_version = "".join(spss_plugin_version.split('.')[:2])

        #Define the basic information of the package
        package_name = 'spss'
        package_install_path = package_name
        package_version = spss_plugin_version
        package_description = 'A python interface to IBM SPSS Statistics API'
        package_author = 'spss'
        package_author_email = ''
        package_url = 'http://www.ibm.com/developerworks/spssdevcentral'
        package_license = ''

        package_sources = [package_name + '/src/PyInvokeSpss.cpp']

        if sys.platform == 'win32':
            package_macros = [('PYINVOKESPSS_EXPORTS', None)]
        elif sys.platform == 'darwin':
            package_macros = [('__PYTHON27__', None),('__MACOSX__', None)]
        elif sys.platform == 'sunos5':
            package_macros = [('__PYTHON27__', None),('__UNX_SUNOS__', None)]
        elif sys.platform == 'aix6' or sys.platform == 'aix7':
            package_macros = [('__PYTHON27__', None)]
        elif sys.platform.lower().find('linux') > -1 :
            package_macros = [('__PYTHON27__', None),('__UNX_LINUX__', None)]
        elif sys.platform == 'hp-ux11':
            package_macros = [('__PYTHON27__', None),('__HPUX64__', None)]
        else :
            package_macros = []

        #Define the extension module 'PyInvokeSpss'
        ext = Extension(name = package_install_path + '.PyInvokeSpss',
                        sources = package_sources,
                        define_macros = package_macros,
                        extra_compile_args = EXTRA_COMPILE_ARGS,
                        extra_link_args =  EXTRA_LINK_ARGS,
                        library_dirs = LIBRARY_DIRS,
                       )
        #Define additional files which need to be installed into the package
        data = {package_install_path: ['spssxdcfg.ini',
                                       'lang/*/spsspy.properties'
                                      ]
                }

        #Execute package setup for Python plug-in.
        if 'win32' == sys.platform and '64 bit' in sys.version:
            #win64 does not build PyInvokeSpss.pyd
            setup(
                name = package_install_path,
                version = package_version,
                description = package_description,
                author = package_author,
                author_email = package_author_email,
                url = package_url,
                license = package_license,
                package_dir = {package_install_path:package_name},
                packages = [package_install_path],
                package_data = data
                #data_files = additional_files
                )
        else:
            setup(
                name = package_install_path,
                version = package_version,
                description = package_description,
                author = package_author,
                author_email = package_author_email,
                url = package_url,
                license = package_license,
                package_dir = {package_install_path:package_name},
                packages = [package_install_path],
                ext_modules=[ext],
                package_data = data
                #data_files = additional_files
                )


    finally:
        del sys.path[0]
        os.chdir(old_path)

if __name__ == "__main__":
    setup_package()
