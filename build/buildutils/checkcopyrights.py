#!/usr/bin/python
#
# Copyright (c) 2009 Nokia Corporation and/or its subsidiary(-ies).
# All rights reserved.
# This component and the accompanying materials are made available
# under the terms of "Eclipse Public License v1.0"
# which accompanies this distribution, and is available
# at the URL "http://www.eclipse.org/legal/epl-v10.html".
#
# Initial Contributors:
# Nokia Corporation - initial contribution.
#
# Contributors:
#
# Description:
#   Checks that all the source files have a proper EPL copyright header.
#   Ignores number of known 3rd party source files and directories.
#
#   Run this script on a clean workarea in order to avoid warnings
#   about exported files and qmake generated files.
#
#   The excluded directories should be checked every now and then
#   so that there won't be other problems (like files without any header).

import sys, os, re


# Specify here the file types to be checked
checkedFileTypes = [".cpp", ".h", ".java", ".py", ".mk"]

# Specify here the directories to be ignored relative to the root
ignoredRootDirs = [
    "javaextensions/bluetooth/bluecove", # Bluecove
    "javacommons/jvms/j9/s60/inc",       # IBM J9
    "javauis/eswt_akn",                  # eSWT checked separately
    "javauis/eswt_qt",                   # eSWT checked separately
    "javauis/tsrc/fute/eswt",            # eSWT checked separately
    "javaextensions/webservices",        # Webservices not delivered
    "tools"                              # Tools not delivered
    ]

# Specify here the directories to be ignored everywhere
ignoredAnyDirs = [
    ".svn",         # SVN directories
    "internal"      # Internal directories not delivered
    ]
 
# Specify here any individual files to be ignored relative to the root
ignoredFiles = [
    "javacommons/jvms/cldc_1.1.1/javasrc/java/util/Timer.java",     # Apache
    "javacommons/jvms/cldc_1.1.1/javasrc/java/util/TimerTask.java", # Apache
    "javacommons/jvms/cldc_1.1.1/tsrc/javasrc/com/nokia/mj/test/java/util/TimerTaskTest.java", # Apache
    "javacommons/jvms/cldc_1.1.1/tsrc/javasrc/com/nokia/mj/test/java/util/TimerTest.java",     # Apache
    "javacommons/utils/inc/convertutf.h",   # Unicode Inc.
    "javacommons/utils/src/convertutf.cpp", # Unicode Inc.
    "javaextensions/ccapi/ccapiext/javasrc/com/innovision/rf/JewelTagConnection.java",  # Innovision
    "javaextensions/ccapi/ccapiext/javasrc/com/sony/felica/Type3TagConnection.java"     # Sony
]


# The copyright texts to be searched for
copyrightText1 = "Nokia Corporation and/or its subsidiary(-ies)"
copyrightText2 = "Eclipse Public License v1.0"


def main():

    root = os.path.abspath(sys.argv[1])
    rootlen = len(root) + 1

    # Preprocess the ignore lists to speed up the actual checking
    rootDirsToIgnore = map(os.path.normpath, ignoredRootDirs)
    filesToIgnore = map(os.path.normpath, ignoredFiles)

    def visitFun(arg, dirname, names):

        # Skip the ignored directories
        if os.path.split(dirname)[1] in ignoredAnyDirs or \
           dirname[rootlen:] in rootDirsToIgnore:
                names[:] = []
                return

        # Check then the files
        for f in names: 
        
            fname = os.path.join(dirname, f)
            
            # Skip directories and ignored files
            if os.path.isdir(fname) or fname[rootlen:] in filesToIgnore:
                continue

            # Skip non-wanted file types
            lastDot = f.rfind(".")
            if lastDot == -1 or not f[lastDot:] in checkedFileTypes:
                continue
            
            # Check if the file contains the copyright texts
            found = False
            try:                       
                file = open(fname)
                
                # Search for the first text
                found = False
                line = file.readline()
                while line != "":
                    if copyrightText1 in line:
                        found = True
                        break;
                    line = file.readline()
                
                # Search the second copyright text as well
                if found:
                    found = False
                    line = file.readline()
                    while line != "":
                        if copyrightText2 in line:
                            found = True
                            break;
                        line = file.readline()

                file.close()
                                    
            except IOError:
                print "Error reading the file " + fname
                
            if not found:
                print fname
                continue
    
    os.path.walk(root, visitFun, None)


if __name__ == "__main__":
    main()
