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

import socket, sys, os
import traceback

def main():
    try:
        # Assuming key-value pairs like key=value
        inputFile = sys.argv[1]
        outputFile = sys.argv[2]
        
        values = {}
        
        for pair in sys.argv[3:]:
            key, value = pair.split("=", 1)
            values[key] = value
         
        replaceInFile(inputFile, outputFile, values)
        
    except:
        print "Usage: %s inputFile outputFile <key=value> ..." % sys.argv[0]
        print "Replaces all occurrences of <key> with <value> <file>"
        traceback.print_exc()
        sys.exit(1)


def replaceInFile(srcFile, dstFile, values={}):
    # Binary mode => no line feed conversions
    f = open(srcFile, "rb")
    data = f.read()
    f.close()
    
    for key, value in values.items():
        data = data.replace("${%s}" % key, value)
    
    # Binary mode => no line feed conversions
    f = open(dstFile, "wb")
    f.write(data)
    f.close()


if __name__ == "__main__":
    main()
