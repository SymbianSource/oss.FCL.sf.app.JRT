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

import re,sys, os.path, subprocess

# Pattern for .pkg file descriptions
RE_FILE = re.compile('^"([^"]+)"-"([^"]+)"', re.MULTILINE)

def main():

    if len(sys.argv) < 2 or len(sys.argv) > 3:
        print "Usage: compressmapfiles.py <pkgfile> [<mapfiles.zip>]"
        return 1

    pkgfile = sys.argv[1]

    mapfilezip = os.path.abspath("mapfiles.zip")
    if len(sys.argv) > 2:
        mapfilezip = os.path.abspath(sys.argv[2])

    # Read files in .pkg
    pkg = open(pkgfile).read()

    files = [match[0] for match in RE_FILE.findall(pkg)]

    # Find mapfiles
    mapfiles = [file + ".map" for file in files]
    mapfiles = [mapfile for mapfile in mapfiles if os.path.exists(mapfile)]

    if not mapfiles:
        return

    dir = os.path.dirname(mapfiles[0])
    mapfiles = [os.path.basename(file) for file in mapfiles]

    # Write mapfiles without path
    cmd = ["zip", "-@", mapfilezip]
    process = subprocess.Popen(args = cmd, stdin = subprocess.PIPE, cwd = dir)
    stdout, stderr = process.communicate(input = "\n".join(mapfiles))

    if process.returncode == 0 or process.returncode == 12:
        return 0
    return process.returncode


if __name__ == "__main__":
    main()
    
