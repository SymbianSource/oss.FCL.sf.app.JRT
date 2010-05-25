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
#   Checks that all the source files have a proper copyright header. 
#   Ignores number of known 3rd party source files and directories.
#
#    Run this script on a clean workarea in order to avoid warnings
#    about exported files and qmake generated files.
#
#    The excluded directories should be checked every now and then
#    so that there won't be other problems (like files without any
#    header).

import sys, os, re

# Specify here the file types to be checked
checkedFileTypes = [".cpp", ".h", ".java", ".py", ".mk"]

# Include here the directories to be ignored
ignoredDirectories = [
    "javaextensions\\bluetooth\\bluecove",  # Bluecove
    "javacommons\\jvms\\j9\\s60\\inc",      # IBM J9
    "javauis\\eswt_akn",                    # eSWT checked separately
    "javauis\\eswt_qt",                     # eSWT checked separately
    "javauis\\tsrc\\fute\\eswt\\",          # eSWT checked separately
    "javaextensions\\webservices",          # Webservices not delivered
    "jrt\\tools",                           # Tools not delivered
    "\\internal"                            # Internal directories not delivered
    ]

# Include here any individual files to be ignored
ignoredFiles = [
    "javacommons\\jvms\\cldc_1.1.1\\javasrc\\java\\util\\Timer.java",       # Apache license
    "javacommons\\jvms\\cldc_1.1.1\\javasrc\\java\\util\\TimerTask.java",   # Apache license
    "javacommons\\jvms\\cldc_1.1.1\\tsrc\\javasrc\\com\\nokia\\mj\\test\\java\\util\\TimerTaskTest.java",   # Apache license
    "javacommons\\jvms\\cldc_1.1.1\\tsrc\\javasrc\\com\\nokia\\mj\\test\\java\\util\\TimerTest.java",       # Apache license
    "javacommons\\utils\\inc\\convertutf.h",                                # Unicode Inc.
    "javacommons\\utils\\src\\convertutf.cpp"                               # Unicode Inc.
]

# The copyright text to be checked
copyrightText = "Nokia Corporation and/or its subsidiary(-ies)"


def main():
    
    def visitFun(arg, dirname, names):

        # Skip SVN directories
        if dirname.find("\\.svn") != -1:
            return
        
        # Skip all the directories to be ignored 
        for dir in ignoredDirectories:
            if dirname.find(dir) != -1:
                names[:] = []
                # print "Ignoring directory", dirname
                return

        # Check then the files
        for f in names: 
        
            fname = dirname + "\\" + f
            
            # Skip directories
            if os.path.isdir(fname):
                continue
        
            # Skip ignored files
            found = False
            for file in ignoredFiles:
                if fname.find(file) != -1:
                    # print "Ignoring file", fname
                    found = True
                    break
            if found:
                continue
            
            # Skip non-wanted file types
            lastDot = f.rfind(".")
            if lastDot == -1 or not f[lastDot:] in checkedFileTypes:
                # print "Ignoring file", f
                continue

            # Check if the file contains the copyright text
            try:                       
                file = open(fname)
                found = False
                line = file.readline()
                while line != "":
                    if copyrightText in line:
                        found = True
                        break;
                    line = file.readline()                
                file.close()
                                    
            except IOError:
                print "Error reading the file " + fname
                
            if not found:
                print fname
                continue
                
    os.path.walk(sys.argv[1], visitFun, None)


if __name__ == "__main__":
    main()
