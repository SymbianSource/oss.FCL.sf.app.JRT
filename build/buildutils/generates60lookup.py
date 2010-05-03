#! /usr/bin/python
#
# Copyright (c) 2009,2010 Nokia Corporation and/or its subsidiary(-ies).
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

import sys, traceback, os, re, os.path
from optparse import OptionParser

# JNIEXPORT void JNICALL Java_com_...... (JNIEnv *, ....);
RE_JNIDECL = re.compile(r"JNIEXPORT\s+\w+\s+JNICALL\s+(\w+)\s+ \([\s\w,\*]+\);")

def getJniFunctions(javah):
    ''' Reads all .h files from hDir and returns the header file names and 
        JNI functions found on the file '''

    if os.path.isdir(javah):
        # Find all headers in javah
        headers = [os.path.join(javah, h) 
                   for h in os.listdir(javah) if h.endswith(".h")]

    elif os.path.isfile(javah):
        headers = [javah]

    else:
        raise Exception("Could not find %s" % javah)

    # Find all JNI functions in header files
    jniFunctions = []
    for headerFile in headers:
        content = open(headerFile).read()
        jniFunctions.extend(RE_JNIDECL.findall(content))

    return [os.path.basename(h) for h in headers], jniFunctions

def writeLookup(lookupFile, headers, jniFunctions, jxeLookup):
    ''' Writes the lookupfile, generated from given headers and jni functions '''

    # File header
    content = r'''/*
 * Description: Automatically generated JNI lookup file. Do not modify manually.
 */

#include "javasymbianoslayer.h"
typedef void (*TFunc)();
'''

    if jxeLookup:
        # Add the J9GetJXE getter as part of JNI functions
        jniFunctions.append("J9GetJXE")

        content = content + r'''
#include "j9comp.h"
extern "C" {
    extern const U_64 j9Jar[];

    void* J9GetJXE() {
        return const_cast<void*>(reinterpret_cast<const void*>(j9Jar));
    }
}
'''

    # Header includes
    content = content + "\n".join(['#include "%s"' % h for h in headers])

    # Sorted lookup table
    content = content + "\nconst FuncTable funcTable[] = {\n"
    content = content + ",\n".join(['   { "%s", (unsigned int) %s}' % 
                                    (jni, jni) for jni in sorted(jniFunctions)])
    content = content + "\n};\n"

    # Jnilookup method
    content = content + r'''
IMPORT_C TFunc jni_lookup(const char* name);
EXPORT_C TFunc jni_lookup(const char* name) {
    return (TFunc)findMethod(name, funcTable, sizeof(funcTable)/sizeof(FuncTable));
}
'''

    # Write file
    open(lookupFile, "w").write(content)


def main():
    parser = OptionParser(usage = "%prog [options] javah-file-or-dir lookup-source.cpp")
    parser.add_option("--nojxe", dest = "jxeLookup",
                      help = "Do not generate JXE lookup code",
                      action = "store_false", default = True)
    (options, args) = parser.parse_args()
    try:
        javah, lookupFile = args

        headers, jniFunctions = getJniFunctions(javah)
        writeLookup(lookupFile, headers, jniFunctions, options.jxeLookup)
        
    except:
        print "Error during lookup file generation!"
        traceback.print_exc()
        sys.exit(-1)

if __name__ == "__main__":
    main()
