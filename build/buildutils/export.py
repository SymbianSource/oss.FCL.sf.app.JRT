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
import os.path
import sys
#import subprocess
import shutil
import re
import types
import zipfile

from os.path import abspath

class BuildError(Exception):
    pass

def main():
    tmpExtension = ".cpp"
    try:
        args = sys.argv[1:]

        # Get macro definitios
        macros = [a[2:] for a in args if a.startswith("-D")]
        args = [a for a in args if not a.startswith("-D")]

        opcode = args[0]
        fileName = args[1]

        exports = readExports(fileName, fileName+tmpExtension, macros)
        for src, dst in exports:
            isZipFile = False
            if src.startswith("___zip___"):
                isZipFile = True
                src = src[9:]

            if not os.path.exists(src):
                print "Exported file "+src+" doesn't exist"
            else:    
                if opcode.lower() == "clean":
                    if isZipFile:
                        handleZipCommand(src, dst, doClean = True)
                    else:
                        delFile(dst)
                else:
                    if isZipFile:
                        handleZipCommand(src, dst, doClean = False)
                    else:
                        updateFile(dst, src)
    except:
        print "Error during export "
        traceback.print_exc()
        sys.exit(-1)

    delFile(fileName+tmpExtension, False)

def readExports(fileName, cppedFile, macros):

    if not os.path.exists(fileName):
        print "No %s, nothing to export" % fileName
        return ()

    # Preprocess bld.inf        

    args = [
        '-undef',
        '-nostdinc']

    if os.name=="nt":
        args = [
            '-undef',
            '-traditional', 
            '-+', 
            '-nostdinc']

    cppFile(
        fileName,
        cppedFile, 
        args,
        macros)

    lines = readFile(cppedFile)

    EXPORTS = {}

    bldInf = {}

    # Keeps track of which inf-file's contents inside the preprocessed
    # bld.inf-file are being processed
    infFile = None

    section = None

    # Read in bld.inf contents
    for line in [l.strip() for l in lines if len(l.strip()) > 0]:
        if line.startswith("#"):
            # Comment line indicates which inf-file any subsequent
            # lines were included from

            # Parse inf-file name
            infFile = re.search(r'^# \d+ "(.+)"', line).group(1)

            # Translate backslashes to slashes, remove dupes
            infFile = abspath(re.sub(r"\\+", "/", infFile))
        elif line.lower().startswith("prj_"):
            section = line.lower()[4:]
        else:
            if not section:
                raise BuildError, \
                    "Data outside of data section in bld.inf: %s" % line

            # Collect lines by section (prj_exports etc), each entry is a
            # tuple of the actual data line and the name of the inf-file
            # where it originated from
            bldInf.setdefault(
                section, []).append((line.strip(), infFile))

    # Collect exports to a separate list
    for export, infFile in bldInf.get("exports", []):
        isZipFile = False
        if export.strip().lower().startswith(":zip"):
            isZipFile = True
            export = export[5:]
        try:
            src, dst = export.split()
        except:
            raise BuildError, \
                'Malformed line "%s" in %s' % (export, infFile)

        # Map source path through current inf file path
        if not src.startswith("\\") and not src.startswith("/"):
            src = os.path.join(os.path.split(infFile)[0], src)

        if dst.startswith("\\") or dst.startswith("/"):
            # Destination path is absolute, use it as is
            pass
        elif dst.startswith("|"):
            # Destination path is relative to the inf-file it was specified
            # in 
            dst = os.path.join(os.path.dirname(infFile), dst[1:])

        src, dst = map(abspath, [src, dst])    
        if isZipFile:
            src = "___zip___"+src
        EXPORTS[(src, dst)] = None

    return EXPORTS.keys()

def handleZipCommand(zipfl, dirToUnzip, doClean = False):
    zfobj = zipfile.ZipFile(zipfl)
    if doClean:
        print("Deleting files extracted from %s:" % (zipfl))
    else:
        print("Extracting files from %s:" % (zipfl))
    for name in zfobj.namelist():
        #Not handling cases where zip file contains empty dirs
        if not name.endswith('/'):
            print("    " + os.path.join(dirToUnzip,name))
            destFile = os.path.join(dirToUnzip, name)
            if doClean:
                delFile(destFile, doPrint = False)
            else:
                makeDirs(destFile, isFile = True)
                if uptodate(destFile, zipfl):
                    print("        Already extracted...")
                    continue
                outfile = open(destFile, 'wb')
                outfile.write(zfobj.read(name))
                outfile.close()



def updateFile(dst, src):
    if not uptodate(dst, src):
        print("Copying %s to %s" % (src, dst))
        shutil.copy(src, dst)

def oldestTime(files):
    oldestTime = None
    oldestFile = None
    try:
        for f in files:
            fileTime = os.path.getmtime(f)
            if oldestTime == None or fileTime < oldestTime:
                oldestTime = fileTime
                oldestFile = f
    except:
        # One or more files don't exist
        return 0

    #print "oldest file is %s with %d" % (oldestFile, oldestTime)
    return oldestTime    

def newestTime(files):
    newest = 0
    newestFile = None

    for f in files:
        try:
            fileTime = os.path.getmtime(f)
        except:
            print("Expected to find file " + f)
        else:    
            if fileTime > newest:
                newestFile = f
                newest = fileTime

    #print "newest file is %s with %d" % (newestFile, newest)
    return newest        

def uptodate(targets, sources):

    # Make non-list arguments lists
    if not isinstance(targets, list):
        targets = [targets]
    if not isinstance(sources, list):
        sources = [sources]

    if oldestTime(targets) < newestTime(sources):
        # One or more target files don't exist or are older than one or
        # more source files, rebuild is needed
        result = False
    else:
        # Rebuild is not needed
        result = True
    

    # Create missing directories
    if result == False:
        for f in targets:
            dirName = os.path.dirname(f)
            if not os.path.exists(dirName):
                os.makedirs(dirName)

    return result


def cppFile(
        srcFile, 
        dstFile, 
        args, 
        macros = []):

    """ Preprocesses a file with CPP using given arguments. 

    By default dstFile is altered only if running CPP would change the file's
    contents. 

    Returns true if dstFile was altered. 
    """

    # Target file doesn't exist or preprocessing was forced

    # Create any missing directories for destination file
    makeDirs(dstFile, isFile = True)

    for m in macros:
        if len(m) == 0:
            raise BuildError, "empty macro definition"
        if m == "NOKIA_SYMBIAN_BLD":
            args.extend([
                "-D__GNUC__=4",
                "-I " + abspath("\\epoc32\\include\\"),
                "-imacros " + abspath("\\epoc32\\include\\platform_paths.hrh")])

    cmd = ['cpp'] + \
        args + \
        ["-D" + m for m in macros] +\
        [srcFile, \
        dstFile]

    print " ".join(cmd)

    execute(cmd, False)
    return True


def makeDirs(directory, isFile = False):
    """ Creates a directory if it doesn't exist. 

    Doesn't do anything if the directory already exists. 

    If isFile is True, the last element of the directory path is assumed to be
    a file and is discarded from the path to be created. """

    if isFile:
        directory = os.path.dirname(directory)

    try:
        os.makedirs(directory)
    except:
        pass

def readFile(f):
    """ Reads contents of a file to a list and returns it. """

    f = open(f, 'r')
    contents = f.readlines()
    f.close()

    return contents

def delFile(file, doPrint = True):
    """ Deletes a file.

    Fails if the file appears to be a directory. """

    if os.path.exists(file):
        if os.path.isfile(file):
            if doPrint:
                print("Deleting file " + file)
            os.remove(file)
        else:    
            print('Can not delete file "%s", is a directory' % (file))
    
def execute(
        cmd, 
        printMsg = True, 
        haltOnError = True, 
        input = None, 
        grabOutput = False,
        *args, 
        **kwd):
    """ Executes external command, keeps track of time spent outside. 

    Any arguments are passed directly to subprocess.call(). """


    if printMsg:
        # Construct string to be displayed on screen
        msg = ""
        if kwd.has_key('cwd'):
            msg = "[" + kwd['cwd'] + "] "
        if type(cmd) == types.StringType:
            msg += cmd
        elif type(cmd) == types.ListType:    
            msg += " ".join(cmd)   
        else:
            raise Error, "Invalid type %s for cmd argument" % type(cmd)
        print msg

    kwd['args'] = cmd    


    """
    if input:
        # Input data for command was specified, pipe it
        kwd['stdin'] = subprocess.PIPE
    if grabOutput:
        # Command output should be grabbed
        kwd['stdout'] = subprocess.PIPE
        kwd['stderr'] = subprocess.PIPE
        
    process = subprocess.Popen(*args, **kwd)

    if input or grabOutput:
        stdout, stderr = process.communicate(input)
    else:
        process.wait()
        
    result = process.returncode
    """
    cmd2 = " ".join(cmd)
    result = os.system(" ".join(cmd))
    if not result == 0 and haltOnError == True:
        print 'Error running external command "' + \
            (type(cmd) == types.StringType and cmd or ' '.join(cmd)) + \
            '", return code ' + str(result)

    return result

        
if __name__ == "__main__":
    main()
