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
# Description: Find all paths matching pattern
#
#! /usr/bin/python

import os.path, fnmatch, sys
from optparse import OptionParser

def findDirs(root, pattern, exclude):
    result = []
    for path, dirs, files in os.walk(root):
        if exclude and fnmatch.filter(path.replace("\\", "/").split("/"), exclude):
            continue
        result.extend([os.path.join(path, d) 
                       for d in fnmatch.filter(dirs, pattern)])
    return result

def main():
    parser = OptionParser()
    parser.add_option(
        "-e", "--exclude", dest = "exclude", 
        help = "Exclude paths having this pattern as directory component, e.g. tsrc")
    parser.add_option(
        "-o", "--output", dest = "out", help = "Output file")
    (options, args) = parser.parse_args()

    if len(args) != 2:
        print "Usage: %s [options] <root> <pattern>" % sys.argv[0]
        sys.exit(1)

    root = args[0]
    pattern = args[1]

    dirs = findDirs(root, pattern, options.exclude)
    output = "\n".join(dirs) + "\n"

    if options.out:
        open(options.out, "w").write(output)
        print "Wrote %s" % options.out
    else:
        print output

    
if __name__ == "__main__":
    main()
