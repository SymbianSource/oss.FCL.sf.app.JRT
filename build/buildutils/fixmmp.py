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
#

import sys, os, re
import traceback
from optparse import OptionParser

TMP_RE = re.compile(r"^SYSTEMINCLUDE\s+(.+[\\/])?tmp$\n", re.MULTILINE)
DRIVE_RE = re.compile("(SYSTEMINCLUDE\s+|SOURCEPATH\s+)[a-zA-Z]:")
RESOURCE_RE = re.compile(r"\nSTART RESOURCE.*?\nEND\n", re.DOTALL)
TIME_RE = re.compile(r"(// Generated by qmake .*? on): .*")

def main():
    parser = OptionParser(
        usage = "Usage: %prog [args] <input mmp> <output mmp>")

    parser.add_option(
        "--driveletter", dest = "driveletter", action = "store_true", default = False)
    parser.add_option(
        "--tmp-dirs", dest = "tmpDirs", action = "store_true", default = False)
    parser.add_option(
        "--resource", dest = "resource", action = "store_true", default = False)
    parser.add_option(
        "--datetime", dest = "datetime", action = "store_true", default = False)
    parser.add_option(
        "--mocpath", dest = "mocpath", action = "store_true", default = False)
    parser.add_option(
        "--header", dest = "header")

    (opts, args) = parser.parse_args()

    try:
        inputFile = args[0]
        outputFile = args[1]
        
        # Read mmp file
        fin = open(inputFile)
        mmp = fin.read()
        fin.close()

        # Remove /tmp directories from system includes
        if opts.tmpDirs:
            mmp = TMP_RE.sub("", mmp)

        # Remove driveletter from systeminclude and sourcepath
        if opts.driveletter:
            mmp = DRIVE_RE.sub("\\1", mmp)

        # Remove resource block
        if opts.resource:
            mmp = RESOURCE_RE.sub("", mmp)

        # Remove changing time
        if opts.datetime:
            mmp = TIME_RE.sub("\\1: (date)", mmp)

        # Fix MOC path (in bld.inf)
        if opts.mocpath:
            mmp = mmp.replace(r"/epoc32/tools/qt/moc.exe", 
                              r"$(EPOCROOT)epoc32/tools/qt/moc.exe")

        # Add header
        if opts.header:
            headerText = open(opts.header).read()
            mmp = headerText + mmp

        # Write mmp file
        open(outputFile, "w").write(mmp)
        
    except:
        print "Usage: %s inputFile outputFile" % sys.argv[0]
        traceback.print_exc()
        sys.exit(1)

        
if __name__ == "__main__":
    main()
