#!/usr/bin/python
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
#  This utility is used to find out the size in disk of files 
#  written to ROM images (or compiled). Script finds out Symbian 
#  build enviornment binary file paths based on iby-file(s) or 
#  mmp-files, and prints out file sizes.
#
#  Run the script with option --help to see the parameters

import os.path, fnmatch, re, subprocess, glob
from optparse import OptionParser
from itertools import chain

RE_MMP_TARGET = re.compile(r"\s*TARGET\s+(\S+)", re.IGNORECASE)
RE_DEFINE = re.compile(r"^define\s*(\S+)\s+(\S+)\s*$", re.IGNORECASE)
RE_IBYTARGET = re.compile(r"^(?:file|data)=(\S+)\s+\S+\s*$", re.IGNORECASE)

# Base defines used in IBY files
BASE_DEFINES = [("ABI_DIR", r"\epoc32\release\armv5"),
                ("BUILD_DIR", r"urel"),
                ("DATAZ_", r"\epoc32\data\z"),
                ("RESOURCE_FILES_DIR", r"resource"),
                ("ZRESOURCE", r"DATAZ_\resource"),
                ("ZSYSTEM", r"DATAZ_\system"),
                ("ZPRIVATE", r"DATAZ_\private")]

def flatten(listOfLists):
    return list(chain(*listOfLists))

def cppFile(source, cppDefines = None, include = None):
    ''' Preprocess source with epoc32/include, and return preprocessed lines '''
    includePaths = [r"/epoc32/include"]

    if cppDefines:
        cppDefines = " ".join(["-D%s" % d for d in cppDefines])
    else:
        cppDefines = ""
    if include:
        path, filename = os.path.split(include)
        includePaths.append(path)

        include = "-include " + include
        
    else:
        include = ""

    drive, tail = os.path.splitdrive(source)
    command = r'cpp %s %s %s "%s"' % (" ".join(["-I %s" % i for i in includePaths]),
                                      cppDefines, include, source)
    process = subprocess.Popen(command, 
                               shell = True,
                               cwd = "%s\\" % drive,
                               stdout = subprocess.PIPE)
    for line in process.stdout.readlines():
        yield line

def replaceDefines(line, defines):
    ''' Replace the macro definitions recursively '''
    oldLine = None
    while oldLine != line:
        oldLine = line
        for search, replace in defines:
            line = line.replace(search, replace)
    return line

def parseIby(lines):
    ''' Parse IBY file lines, using defines '''
    defines = BASE_DEFINES[:]
    for l in lines:
        defineMatch = RE_DEFINE.match(l)
        if defineMatch:
            search, replace = defineMatch.groups()
            defines.append((search, replace))
        else:
            yield replaceDefines(l, defines)
            
def getIbySources(ibyfile, cppDefines = None, include = None):
    ''' Get IBY file source files'''
    drive, tail = os.path.splitdrive(ibyfile)
    for l in parseIby(cppFile(ibyfile, cppDefines, include)):
        match = RE_IBYTARGET.match(l)
        if match:
            yield drive + match.group(1)

def findFiles(root, pattern):
    ''' Find files recursively '''
    for path, dirs, files in os.walk(root):
        for f in fnmatch.filter(files, pattern):
            yield os.path.join(path, f)

def getMmpTarget(mmpfile):
    ''' Get the target binary name from mmp-file '''
    for l in open(mmpfile, "r").readlines():
        match = RE_MMP_TARGET.search(l)
        if match:
            return match.group(1)
    return None

def getMmpTargets(mmpRoot):
    ''' Get the target binary paths of all mmp-files found under root '''
    drive, tail = os.path.splitdrive(mmpRoot)

    totalSize = 0
    for mmp in findFiles(mmpRoot, "*.mmp"):
        target = getMmpTarget(mmp)
        if not target:
            continue
        binPath = drive + "/epoc32/release/armv5/urel/" + target
        if os.path.exists(binPath):
            yield binPath

def getSizes(binaries):
    ''' Get the sizes of given files, return list of tuples (size|None, file) '''
    for binPath in binaries:
        if not os.path.exists(binPath):
            yield None, binPath
        else:
            yield os.path.getsize(binPath), binPath

def printSizes(sizes):
    ''' Print file sizes and a total '''
    totalSize = 0
    for size, binPath in sizes:
        if size:
            totalSize = totalSize + size

        print "%s\t%s" % (size, binPath)

    print "%d\tTotal" % totalSize

def main():
    parser = OptionParser()
    parser.add_option(
        "--mmproot", dest = "mmproot",
        help = "root for searching mmp-files (whose target is used for filenames)")
    parser.add_option(
        "--ibyfile", dest = "ibyfile",
        help = "ibyfile(s) which are processed to find out target file paths")
    parser.add_option(
        "--sortname", dest = "sortname", action = "store_true", default = False,
        help = "sort files by name")
    parser.add_option(
        "--sortsize", dest = "sortsize", action = "store_true", default = False,
        help = "sort files by size")
    parser.add_option(
        "--cppdefines", dest = "cppdefines",
        help = "cpp defines for iby, e.g. --cppdefines __JAVA,__JAVA_MIDP20")
    parser.add_option(
        "--include", dest = "include",
        help = "Include file for iby cpp, e.g. --include /epoc32/include/config/ncp52/bldpublic.hrh")
    (options, args) = parser.parse_args()

    if options.mmproot:
        binaries = getMmpTargets(options.mmproot)

    elif options.ibyfile:
        cppdefines = options.cppdefines and options.cppdefines.split(",")

        ibys = glob.glob(options.ibyfile)
        binaries = flatten([getIbySources(iby, cppdefines, options.include) 
                            for iby in ibys])
        
    sizes = getSizes(binaries)

    if options.sortsize:
        sizes = sorted(sizes)
    if options.sortname:
        sizes = sorted(sizes, cmp=lambda x,y: cmp(x[1], y[1]))

    printSizes(sizes)

if __name__ == "__main__":
    main()
