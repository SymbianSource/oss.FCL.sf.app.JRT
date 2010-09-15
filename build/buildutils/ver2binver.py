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
# This script changes a JRT version to a binary version usable as 
# Symbian mmp-file version or qmake's version.
# - each version part is cut to three digits, and then to 8 bits
# - qmake's version is taken as is, mmp's version is mangled as by qmake (the
#   resulting version is the same in binaries).
# - major version is always 10

import sys, re 


USAGE = "usage: %s [qmake|mmp] 1.2.3\n" % sys.argv[0]

def main():
    try:
        if len(sys.argv) != 3:
            raise Exception(USAGE)

        mode, javaversion = sys.argv[1:]

        if mode != "qmake" and mode != "mmp":
            raise Exception(USAGE)

        # Match version
        parts = re.match(r"^(\d+)\.(\d+)\.(\d+)$", javaversion)
        if not parts:
            raise Exception(USAGE)

        # Change to integer
        parts = [int(p) for p in parts.groups()]

        # Modulo 1000, 256
        parts = [(p % 1000) % 256 for p in parts]

        # Print according to type
        if mode == "qmake":
            print "%d.%d.%d" % (10, parts[1], parts[2])
        else:
            print "%d.%d" % (10, (parts[1] << 8) + parts[2])

    
    except Exception, e:
        sys.stderr.write(e.__str__())
        sys.exit(1)


if __name__ == "__main__":
    main()


