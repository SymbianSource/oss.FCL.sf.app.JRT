#
# Copyright (c) 2010 Nokia Corporation and/or its subsidiary(-ies).
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
# Description: Check Java source code for inconsistencies. Includes following checks
#              - is package defined and matching with file path
#
#! /usr/bin/python

import os, fnmatch, re, sys
from optparse import OptionParser

RE_PACKAGE = re.compile(r"package\s+([\w.]+)\s*;")


def getPackage(f):
    ''' Return the package of Java source file, or None if not defined '''
    fin = file(f, "r")
    for l in fin.readlines():
        match = RE_PACKAGE.search(l)
        if match:
            return match.group(1)

    return None

def checkPackage(f, package):
    ''' Check if package is correct in file path. Return True if incorrect. '''

    if not package:
        print "%s(0): error: no package defined" % f
        return True

    dir = os.path.dirname(f).replace("\\", "/")
    if not dir.endswith(package.replace(".", "/")):
        print "%s(0): error: package \"%s\" does not match filepath" % (f, package)
        return True

    return False

def checkHierarchy(root, exclude = None):
    failures = False
    for path, dirs, files in os.walk(root):
        if exclude and exclude in path.replace("\\", "/").split("/"):
            continue

        filenames = [os.path.join(path, f) for f in fnmatch.filter(files, "*.java")]
        for f in filenames:
            result = checkPackage(f, getPackage(f))
            failures = failures or result

    return failures
        
def main():
    parser = OptionParser()
    parser.add_option("--exclude", dest = "exclude", 
                      help = "Exclude paths having this directory component, e.g. tsrc")
    (options, args) = parser.parse_args()

    failures = False
    for root in args:
        failures = failures or checkHierarchy(root, options.exclude)

    sys.exit(failures and 1 or 0)

if __name__ == "__main__":
    main()
