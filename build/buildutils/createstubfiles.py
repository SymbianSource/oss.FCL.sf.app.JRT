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

import sys, os
import traceback
from optparse import OptionParser

def main():
    parser = OptionParser(
        usage = "Usage: %prog <component root> <javah files> <javah root>")


    (opts, args) = parser.parse_args()

    try:
        componentRoot = args[0]
        javahFiles = args[1]
        javahRoot = args[2]
        
        # Generate empty jxe.c file if doesn't exist.
        jxeFile = componentRoot + "/jxe.c"
        if not os.path.exists(jxeFile):
            fout = open(jxeFile, 'w')
            content = """
// This prevents warnings of literal treated as \"unsigned long long
// coming from compilation of jxe.c (J9 romized classes).
#ifndef __WINS__
#pragma diag_suppress 1135
#endif

#ifdef _DEBUG
#include \"jxe_debug.c\"
#else
#include \"jxe_release.c\"
#endif
"""
            fout.write(content)
            fout.close()

        # Generate empty jxe_<debug/release>.c files if doesn't exist.
        createUnexistingFile(componentRoot + "/jxe_debug.c")
        createUnexistingFile(componentRoot + "/jxe_release.c")

        # Generate empty lookup.c if doesn't exist.
        createUnexistingFile(componentRoot + "/lookup.cpp")

        # Generate empty javah file if the component has defined such.
        # The javahFiles will contain a comma separated list of java files
        # that should be 'javah'ed'. In here we need to tweak names a little 
        # bit. If the component doesnt havy any files to be 'javah'ed' them
        # the value of javahFiles is ${javah.classnames}
        if not javahFiles.startswith("$"):
            # Split a string to list.
            javahFilesList = javahFiles.split(',')

            # Loop all the files. Convert '.' to '_ and add '.h' extension.
            for item in javahFilesList:
                javahFile = item.strip().replace('.', '_') + ".h"
                
                # Create empty file if doesn't exist.
                createUnexistingFile(javahRoot + "/" + javahFile)
        
    except:
        print "Error in s60init"
        traceback.print_exc()
        sys.exit(1)

def createUnexistingFile(file):
    if not os.path.exists(file):
        fout = open(file, 'w')
        fout.close()
        
        
if __name__ == "__main__":
    main()

