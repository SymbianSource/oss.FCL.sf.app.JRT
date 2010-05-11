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
#   Checks that all the java source files declare a package and that 
#   the directory within a java source file is located corresponds
#   properly to the package.

import sys, os, re


def main():

    files = []
    
    # Create a reg exp matching to "package x.y.z;" with whitespace ignored
    regex = re.compile("\\s*package\\s*([\\w.]*);.*", re.IGNORECASE)

    def visitFun(arg, dirname, names):

        # Skip SVN directories
        if dirname.find("\\.svn") != -1:
            return names
            
        for f in names:
            if not f.endswith(".java"):
                continue
                
            try:
                fname = dirname + "\\" + f            
                file = open(fname)

                package = None
                line = file.readline()
                while line != "":
                    result = regex.match(line)
                    if result != None:
                        package = result.group(1)
                        break;                    
                    line = file.readline()
                
                if package != None:
                    expectedDir = package.replace(".", "\\");
                    if not dirname.endswith(expectedDir):
                        print "Wrong directory:", fname + ", package", package
                else:
                    print "Package statement missing:", fname
                

                file.close()
                                    
            except IOError:
                print "Error reading the file " + fname
                
    os.path.walk(sys.argv[1], visitFun, files)


if __name__ == "__main__":
    main()
