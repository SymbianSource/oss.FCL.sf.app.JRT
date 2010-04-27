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

import sys, re, os, os.path, fnmatch

# This utility reads a compilation log file, and writes last 50 lines up to error 
# to an output tail file. If no errors, then file is not written

RE_ERROR = re.compile(r"fatal error|Build FAILED|make(\.exe)?(\[[^]]+\])?: \*\*\* .+ (Error [0-9]+|Stop\.)|.+[0-9]+: error:|status exit='failed'|<error>.*</error>|<center>[1-9]*</center></td><td><center>High</center>")

def errortail(logfile, tailfile, tailLines = 50):
    tail = []
    for line in open(logfile).readlines():
        tail.append(line.rstrip())
        if (len(tail) > tailLines):
            del tail[0]
        if RE_ERROR.search(line):
            tail = tail[-50:]
            open(tailfile, "w").write("\n".join(tail))
            return

def main():
    if len(sys.argv) != 3:
        print "Usage: %s <build log file> <tail output file>" % sys.argv[0]
        sys.exit(1)
    errortail(sys.argv[1], sys.argv[2])

if __name__ == "__main__":
    main()
