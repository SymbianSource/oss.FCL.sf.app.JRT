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
#! /usr/bin/python

import sys
import traceback
import os



def main():
    tmpExtension = ".cpp"
    try:
        odcFileName  = sys.argv[1]
        javaPeerDll  = sys.argv[2]
        classesDir   = sys.argv[3]
        sysPropsFile = sys.argv[4]

        genOdcFile(odcFileName, 
                   javaPeerDll,
                   getClasses(classesDir),
                   getProperties(sysPropsFile))

    except:
        print "Error during ODC generation!"
        traceback.print_exc()
        sys.exit(-1)

def getProperties(sysPropsFile):

    contents = []
    try:
        f = open(sysPropsFile, 'r')
        contents = f.readlines()
        f.close()
    except IOError:
        pass                           
    return contents


def getClasses(classesDir):
    startDir = os.path.abspath(classesDir)

    files = []
    suffix = ".class"

    def callBack(arg, dirname, files):
        # Remove startDir-part from dirname
        dirname = dirname[len(startDir) + 1:]
        for file in files:
            if file.endswith(suffix):
                # replace '\' with '/' and strip the class name.
                res = os.path.join(dirname, file).replace("\\","/").rsplit("/",1)[0]
                arg.append(res)
            
    os.path.walk(startDir, callBack, files)
    
    return files

def genOdcFile( odcFileName,
                javaPeerDll, 
                classes, 
                properties = []):

    # If a list of classes was given instead of packages, strip class names and
    # remove duplicates
    classes = list(set([c for c in classes]))

    lines = [    
        "[container]",
        "name=" + javaPeerDll,
        "type=JXESL",
        "",
        "[packages]"]
    lines.extend([p.replace(".", "/") for p in sorted(classes)])    
    lines.append("")
    lines.append("[properties]")
    
    # Append system properties, but only if there are classes
    # (classes may be missing if component sources has been excluded
    # from release, which means that properties should not be written
    # either)
    if classes:
        for property in properties:
            property = property.strip()
            if not property:
                continue
            lines.append("-D" + property)
    lines.append("")

    f = open(odcFileName, 'w')
    f.write("\n".join(lines))
    f.close()

        
if __name__ == "__main__":
    main()
