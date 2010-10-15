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
# Description:
#
#!/usr/bin/python
#
# This script creates java specific loc zip files from S60 loc zip files
# and also creates a "resources.jar" file where all the java loc files
# are collected.
#
# Usage: extractJavaLocFiles_qt.py <input_dir> <output_dir> <resources_jar>
#
# Script reads S60 loc zip files from <input_dir> directory and writes
# "resources.jar" to <output_dir> directory.
#
# This script uses external "jar" command for handling zip files and
# thus is not optimized for speed.
#

import os, re, shutil, sys, glob, codecs
import traceback
import xml.etree.ElementTree as et
from optparse import OptionParser

# Base names for java loc files.
JAVA_LOC_FILES = [
    'javafileconnection',
    'javainstallation',
    'javasensor',
    'javassl',
    'javausermessages',
    'javaapplicationsettings',
    # loc files for qt
    'common_errors',
    'javaapplicationinstaller',
    'javaapplicationinstallererrors',
    'javaapplicationsecuritymessages',
    'javapim',
    'javaruntimeapplicationsettings',
    'javaruntimecertificatemanagement',
    'javauitoolkits',
    ]

# Path for java loc files inside RESOURCES_FILE.
RESOURCES_PATH = os.path.join("resources", "com", "nokia", "mj", "impl")

def main():
    parser = OptionParser(
        usage = "Usage: %prog [args] <input_dir> <output_dir> <resources_jar>")
    (opts, args) = parser.parse_args()

    try:
        inputDir = args[0]
        outputDir = args[1]
        resourcesFilename = os.path.join(outputDir, args[2])

        # Use temporary directory
        tmpDir = "tmpdir"
        if os.path.exists(tmpDir):
            shutil.rmtree(tmpDir)
        locDir = os.path.join(tmpDir, RESOURCES_PATH)
        os.makedirs(locDir)

        print "Processing loc files from %s to %s" % (inputDir, resourcesFilename)
        locFileCount = 0;

        # Go through all Java loc file names, converting the .ts to .loc
        for name in JAVA_LOC_FILES:

            # Find all existing ts-files
            for ts in glob.glob("%s/%s_*.ts" % (inputDir, name)):
                path, filename = os.path.split(ts)
                base, ext = os.path.splitext(filename)
                loc = os.path.join(locDir, base + ".loc")
                print "Converting %s" % ts
                writeLoc(loc, readTs(ts))
                locFileCount = locFileCount + 1

        # Update the target jar with loc-files
        if os.path.isfile(resourcesFilename):
            os.system("jar ufM " + resourcesFilename + " -C " + tmpDir + " .")
        else:
            os.system("jar cfM " + resourcesFilename + " -C " + tmpDir + " .")

        print "Processed %d loc files" % (locFileCount)
        shutil.rmtree(tmpDir)

    except:
        print "Usage: %s <input_dir> <output_dir> <resources_jar>" % sys.argv[0]
        traceback.print_exc()
        sys.exit(1)

def readTs(filename):
    messages = []
    tree = et.parse(filename).getroot()
    for message in tree.find("context").findall("message"):
        id = message.get("id")
        translation = message.find("translation")

        # Sanity check - we have no good plurality support for qt-localisation
        if translation.find("numerusform") != None:
            print "WARNING at %s / %s: numerus form (qt plurality) is not supported" % (filename, id)

        numerusforms = translation.findall("numerusform")
        lengthvariant = translation.find("lengthvariant")
        if lengthvariant != None:
            text = lengthvariant.text
        elif len(numerusforms) > 0:
            text = numerusforms[0].text
        else:
            text = translation.text

        # If text is not found, this is unlocalised engineering English file
        if text == None:
            text = message.find("source").text

        # Escape characters
        text = text.replace("\\", "\\\\")
        text = text.replace("\n", "\\n")
        text = text.replace("\t", "\\t")
        text = text.replace("\"", "\\\"")
        text = text.replace("'", "\\'")

        messages.append((id, text))

    return messages

def writeLoc(filename, messages):
    loc = codecs.open(filename, "w", "UTF-8")
    loc.write(u"CHARACTER_SET UTF8\n")
    for id, message in messages:
        loc.write(u'#define %s "%s"\n' % (id, message))
    loc.close()

if __name__ == "__main__":
    main()
