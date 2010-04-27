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
import subprocess
import shutil
import re
import types
import zipfile

from os.path import abspath

"""
#ifdef __JAVA_API_ENHANCEMENTS_SECURITY
#if defined (CHAPI_SUPPORT)
#ifdef __JAVA_API_ENHANCEMENTS_SECURITY
#ifdef RD_JAVA_VOLUME_CONTROL
#ifdef BLUETOOTH11
#ifdef BLUETOOTH10
#ifdef MIDP20BUILD
#ifdef TESTBUILD
#ifdef LCDUI_DEBUG
#ifdef TRACE_GRAPHICS
#ifdef GRAPHICS_BUFFER_DRAWPIXELS
#ifndef XDEBUG
#ifdef DEBUG
#ifdef RD_JAVA_OMA_DRM_V2
#ifdef _APIEXT_SECURITY_
#ifdef BLUETOOTH_MASTER_SUPPORT
#ifdef RD_JAVA_VMPORT_LAYER_IN_USE
#ifdef RD_JAVA_LC


"""

def main():
    try:
        source = sys.argv[1]
        destination = sys.argv[2]
        print "source = "+source+", destination = "+destination
        javaCpp(source, destination)
    except:
        print "Error during javaCpp "
        traceback.print_exc()
        sys.exit(-1)


def javaCpp(sourceDirs, destinationDir):

    # Find all .java-files to preprocess. If any two or more files represent
    # the same class, discard the ones specified first using jsourcetree.
    dirs = sourceDirs.strip().split(":")
    #dirs = ["/s60/mw/java/javalegacy/j2me/components/lcduib/src/epoc/classes",
    #        "/s60/mw/java/javaextensions/supplements/nokialcdui/javasrc"
    #       ]
    allJavas = {}
    for d in dirs:
        # Get all .java-files under sourcetree
        print "DIR = "+d
        javas = getJavaFiles(d)

        for file in javas:
            allJavas[file] = os.path.abspath(os.path.join(d, file))

    for file in allJavas.keys():
        # File name for preprocessed Java file
        cppFile = os.path.join(destinationDir, file)

        # File name to original Java file
        javaFile = allJavas[file]

        if not uptodate(javaFile, cppFile):
            doCpp(javaFile, cppFile)


def getJavaFiles(javasDir):
    startDir = os.path.abspath(javasDir)

    files = []
    suffix = ".java"

    def callBack(arg, dirname, files):
        # Remove startDir-part from dirname
        dirname = dirname[len(startDir) + 1:]
        for file in files:
            if file.endswith(suffix):
                # replace '\' with '/' and strip the class name.
                res = os.path.join(dirname, file)
                arg.append(res)
            
    os.path.walk(startDir, callBack, files)
    
    return files


def doCpp(
        srcFile, 
        dstFile):

    macros = [
              'JDEBUG(aMessage)=',
              'JDEBUG_STATIC(aMessage)=',
              'JPRINT(aMessage)=',
              'JASSERT(aAssertion)=',
              'JASSERT_STATIC(aAssertion)=',
              'JASSERT_TXT(aAssertion,aMessage)=',
              'JASSERT_TXT_STATIC(aAssertion,aMessage)=',
              'JSOURCEDEBUG(aSourceCode)=',
              'RD_JAVA_OMJ',
              'RD_JAVA_OMA_DRM_V2',
              'RD_JAVA_VOLUME_CONTROL'
              ]

    if not os.path.exists(dstFile):
        makeDirs(dstFile, isFile = True)

    # Use cpp defined on environment CPPCMD, as the cpp/gcc varies between symsee, abld, sbs, 
    # linux, ...
    if "CPPCMD" in os.environ:
        cmd = os.environ["CPPCMD"]

    else:
        cmd = " ".join([
            'gcc',                      # Generic GCC
            '-E',                       # Preprocess only
            '-undef',                   # Do not predefine any macros
            '-P',                       # Do not generate #line directives
            '-x', 'assembler-with-cpp', # Language as C++
            '-w',                       # No warnings
            '-traditional-cpp'])        # Use traditional precompilation

    cmd = cmd + " " + " ".join(
        ["-D" + m for m in macros] +
        [srcFile, "-o", dstFile])

    doExecute(cmd)

def doExecute(
        cmd, 
        *args, 
        **kwd):
    """Any arguments are passed directly to subprocess.call(). """


    # Construct string to be displayed on screen
    """
    msg = ""
    if kwd.has_key('cwd'):
        msg = "[" + kwd['cwd'] + "] "
    if type(cmd) == types.StringType:
        msg += cmd
    elif type(cmd) == types.ListType:    
        msg += " ".join(cmd)   
    else:
        raise Error, "Invalid type %s for cmd argument" % type(cmd)
    sys.stderr.write("cmd = %s\n" % msg)
    """
    kwd['args'] = cmd    

    process = subprocess.Popen(*args, **kwd)
    return

def uptodate(source, target):

    if not os.path.exists(target):
        return False

    try:
        fileTimeSrc = os.path.getmtime(source)
        fileTimeDst = os.path.getmtime(target)
        if fileTimeSrc > fileTimeDst :
            result = False
        else:
            result = True
        
    except:
        # One or more files don't exist
        result = False

    return result

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

if __name__ == "__main__":
    main()
