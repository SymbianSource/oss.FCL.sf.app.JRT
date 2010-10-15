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
#!/usr/bin/python
#
# This script creates java specific loc zip files from S60 loc zip files
# and also creates a "resources.jar" file where all the java loc files
# are collected.
#
# Usage: extractJavaLocFiles.py <input_dir> <output_dir>
#
# Script reads S60 loc zip files from <input_dir> directory and writes
# java loc zip files and "resources.jar" to <output_dir> directory.
#
# This script uses external "jar" command for handling zip files and
# thus is not optimized for speed.
#

import os, re, shutil, sys, zipfile
import traceback
from optparse import OptionParser

# Base names for java loc files.
JAVA_LOC_FILES = [
    'javafileconnection',
    'javainstallation',
    'javasensor',
    'javassl',
    'javausermessages',
    'javaapplicationsettings',
    ]

# Pattern for loc zip file names.
LOC_ZIP_FILE = re.compile("^\d+\.zip$", re.IGNORECASE)

# Path for java loc files inside RESOURCES_FILE.
RESOURCES_PATH = os.path.join("resources", "com", "nokia", "mj", "impl")

def main():
    parser = OptionParser(
        usage = "Usage: %prog [args] <input_dir> <output_dir> <loc-file.jar>")
    parser.add_option(
        "--files", dest = "files", default = ",".join(JAVA_LOC_FILES),
        help = "list of Java loc files to read from input (default: %s)" % 
        ",".join(JAVA_LOC_FILES))
    (opts, args) = parser.parse_args()

    locFiles = opts.files.split(",")

    try:
        inputDir = args[0]
        outputDir = args[1]
        resourcesFilename = os.path.join(outputDir, args[2])

        print "Processing loc files from %s to %s" % (inputDir, outputDir)
        locFileCount = 0;
        for dirname, _, filenames in os.walk(inputDir):
            for file in filenames:
                inFilename = os.path.join(dirname, file)
                if LOC_ZIP_FILE.match(file) and zipfile.is_zipfile(inFilename):
                    outFilename = os.path.join(outputDir, file.lower())
                    (fileIndex, fileExt) = os.path.splitext(file)
                    print "Extracting %s" % inFilename
                    extractJavaLocFiles(locFiles, fileIndex, inFilename)
                    print "Creating %s" % outFilename
                    os.system("jar cfM " + outFilename + " " + fileIndex)
                    print "Updating %s" % resourcesFilename
                    updateResources(fileIndex, resourcesFilename)
                    # Remove temporary fileIndex directory.
                    if os.path.isdir(fileIndex):
                        shutil.rmtree(fileIndex)
                    locFileCount = locFileCount + 1
        print "Processed %d loc files" % (locFileCount)

    except:
        print "Usage: %s <input_dir> <output_dir>" % sys.argv[0]
        traceback.print_exc()
        sys.exit(1)

def extractJavaLocFiles(locFiles, index, inFilename):
    javaLocFiles = ""
    for javaLocFile in locFiles:
        javaLocFilename = index + "/" + javaLocFile + "_" + index + ".loc"
        javaLocFiles = javaLocFiles + " " + javaLocFilename
    command = "jar xf " + inFilename + javaLocFiles
    os.system(command)

def updateResources(index, resourcesFilename):
    resourcesDir = os.path.join(index, RESOURCES_PATH)
    if not os.path.isdir(resourcesDir):
        os.makedirs(resourcesDir)
    for dirname, _, filenames in os.walk(index):
        for file in filenames:
            os.rename(os.path.join(dirname, file),
                      os.path.join(resourcesDir, file))
    if os.path.isfile(resourcesFilename):
        os.system("jar ufM " + resourcesFilename + " -C " + index + " .")
    else:
        os.system("jar cfM " + resourcesFilename + " -C " + index + " .")

if __name__ == "__main__":
    main()
