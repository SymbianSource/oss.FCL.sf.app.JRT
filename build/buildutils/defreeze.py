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

import re, sys, glob, os.path
from optparse import OptionParser

# When defreezing, these entries can exist, but only at the beginning of the exports. 
# Anything after these will be defrozen.
ALLOWEDENTRIES = set(['_Z10jni_lookupPKc', '_Z13findDllMethodPKc'])

# Pattern for entries
RE_ENTRY = re.compile(r"^\s+([^ ]+) +@ +\d+ +NONAME( +ABSENT)?( *;.*)?$")

# Transient exports which will be marked as absent.
# See http://www.codesourcery.com/public/cxx-abi/abi.html#mangling for detailed 
# de-mangling instructions
TRANSIENTS = set([
    # TI = typeinfo structures
    '_ZTISi', # Si == ::std::basic_istream<char,  std::char_traits<char> >
    '_ZTISo', # So == ::std::basic_ostream<char,  std::char_traits<char> >
    '_ZTISd', # Sd == ::std::basic_iostream<char, std::char_traits<char> >
    '_ZTISt13basic_istreamIwSt11char_traitsIwEE',
    '_ZTISt13basic_ostreamIwSt11char_traitsIwEE',
    '_ZTISt14basic_iostreamIwSt11char_traitsIwEE',
    '_ZTISt15basic_streambufIwSt11char_traitsIwEE',
    '_ZTISt15basic_streambufIcSt11char_traitsIcEE',
    '_ZTISt13basic_filebufIcSt11char_traitsIcEE',
    '_ZTISt15basic_stringbufIcSt11char_traitsIcESaIcEE',
    '_ZTISt15basic_stringbufIwSt11char_traitsIwESaIwEE',
    '_ZTISt18basic_stringstreamIcSt11char_traitsIcESaIcEE',
    '_ZTISt18basic_stringstreamIwSt11char_traitsIwESaIwEE',
    '_ZTISt19basic_ostringstreamIcSt11char_traitsIcESaIcEE',
    '_ZTISt19basic_ostringstreamIwSt11char_traitsIwESaIwEE',
    '_ZTISt19basic_istringstreamIcSt11char_traitsIcESaIcEE',
    '_ZTISt19basic_istringstreamIwSt11char_traitsIwESaIwEE',

    # TV = virtual tables
    '_ZTVSi',
    '_ZTVSo',
    '_ZTVSd',
    '_ZTVSt13basic_istreamIwSt11char_traitsIwEE',
    '_ZTVSt13basic_ostreamIwSt11char_traitsIwEE',
    '_ZTVSt14basic_iostreamIwSt11char_traitsIwEE',
    '_ZTVSt15basic_streambufIwSt11char_traitsIwEE',
    '_ZTVSt15basic_streambufIcSt11char_traitsIcEE',
    '_ZTVSt13basic_filebufIcSt11char_traitsIcEE',
    '_ZTVSt15basic_stringbufIcSt11char_traitsIcESaIcEE',
    '_ZTVSt15basic_stringbufIwSt11char_traitsIwESaIwEE',
    '_ZTVSt18basic_stringstreamIcSt11char_traitsIcESaIcEE',
    '_ZTVSt18basic_stringstreamIwSt11char_traitsIwESaIwEE',
    '_ZTVSt19basic_ostringstreamIcSt11char_traitsIcESaIcEE',
    '_ZTVSt19basic_ostringstreamIwSt11char_traitsIwESaIwEE',
    '_ZTVSt19basic_istringstreamIcSt11char_traitsIcESaIcEE',
    '_ZTVSt19basic_istringstreamIwSt11char_traitsIwESaIwEE',

    # VTT structure (construction vtable index)
    '_ZTTSd',
    '_ZTTSi',
    '_ZTTSo',
    '_ZTTSt13basic_istreamIwSt11char_traitsIwEE',
    '_ZTTSt13basic_ostreamIwSt11char_traitsIwEE',
    '_ZTTSt14basic_ifstreamIcSt11char_traitsIcEE',
    '_ZTTSt14basic_iostreamIwSt11char_traitsIwEE',
    '_ZTTSt14basic_ofstreamIcSt11char_traitsIcEE',
    '_ZTTSt18basic_stringstreamIcSt11char_traitsIcESaIcEE',
    '_ZTTSt18basic_stringstreamIwSt11char_traitsIwESaIwEE',
    '_ZTTSt19basic_ostringstreamIcSt11char_traitsIcESaIcEE',
    '_ZTTSt19basic_ostringstreamIwSt11char_traitsIwESaIwEE',
    '_ZTTSt19basic_istringstreamIcSt11char_traitsIcESaIcEE',
    '_ZTTSt19basic_istringstreamIwSt11char_traitsIwESaIwEE'
])

# These are produced only by 9.2, not by 5.0
TRANSIENTS_92 = set([
    '_ZTTSd',
    '_ZTTSi',
    '_ZTTSo',
    '_ZTTSt13basic_istreamIwSt11char_traitsIwEE',
    '_ZTTSt13basic_ostreamIwSt11char_traitsIwEE',
    '_ZTTSt14basic_ifstreamIcSt11char_traitsIcEE',
    '_ZTTSt14basic_iostreamIwSt11char_traitsIwEE',
    '_ZTTSt14basic_ofstreamIcSt11char_traitsIcEE'
])

# Only produced by stl4 - not by stl5
TRANSIENTS_STL4 = set([
    '_ZTISi',
    '_ZTISt13basic_istreamIwSt11char_traitsIwEE',
    '_ZTVSi',
    '_ZTVSt13basic_istreamIwSt11char_traitsIwEE'
])


# Only produced by stl5
TRANSIENTS_STL5 = set([
    '_ZTISt15basic_streambufIcSt11char_traitsIcEE',
    '_ZTISt15basic_stringbufIcSt11char_traitsIcESaIcEE',
    '_ZTISt15basic_stringbufIwSt11char_traitsIwESaIwEE',
    '_ZTISt18basic_stringstreamIcSt11char_traitsIcESaIcEE',
    '_ZTISt18basic_stringstreamIwSt11char_traitsIwESaIwEE',
    '_ZTISt19basic_ostringstreamIcSt11char_traitsIcESaIcEE',
    '_ZTISt19basic_ostringstreamIwSt11char_traitsIwESaIwEE',
    '_ZTISt19basic_istringstreamIcSt11char_traitsIcESaIcEE',
    '_ZTISt19basic_istringstreamIwSt11char_traitsIwESaIwEE',

    '_ZTTSt18basic_stringstreamIcSt11char_traitsIcESaIcEE',
    '_ZTTSt18basic_stringstreamIwSt11char_traitsIwESaIwEE',
    '_ZTTSt19basic_ostringstreamIcSt11char_traitsIcESaIcEE',
    '_ZTTSt19basic_ostringstreamIwSt11char_traitsIwESaIwEE',
    '_ZTTSt19basic_istringstreamIcSt11char_traitsIcESaIcEE',
    '_ZTTSt19basic_istringstreamIwSt11char_traitsIwESaIwEE'

    '_ZTVSt15basic_streambufIcSt11char_traitsIcEE',
    '_ZTVSt15basic_stringbufIcSt11char_traitsIcESaIcEE',
    '_ZTVSt15basic_stringbufIwSt11char_traitsIwESaIwEE',
    '_ZTVSt18basic_stringstreamIcSt11char_traitsIcESaIcEE',
    '_ZTVSt18basic_stringstreamIwSt11char_traitsIwESaIwEE',
    '_ZTVSt19basic_ostringstreamIcSt11char_traitsIcESaIcEE',
    '_ZTVSt19basic_ostringstreamIwSt11char_traitsIwESaIwEE',
    '_ZTVSt19basic_istringstreamIcSt11char_traitsIcESaIcEE',
    '_ZTVSt19basic_istringstreamIwSt11char_traitsIwESaIwEE'
])

TRANSIENTS_SPECIFIC = TRANSIENTS_92 | TRANSIENTS_STL4 | TRANSIENTS_STL5
TRANSIENTS_COMMON = TRANSIENTS - TRANSIENTS_SPECIFIC

def readExports(file):
    lines = [l.rstrip() for l in open(file).readlines()]
    if lines.pop(0) != "EXPORTS":
        raise Exception("File %s is not a deffile" % file)

    exports = []
    
    for l in lines:
        # Skip empty lines
        if len(l.strip()) == 0:
            continue

        # Read lines
        match = RE_ENTRY.match(l)
        if not match:
            raise Exception("Unrecognized line on file %s: %s" % (file, l))

        (export, absent, comment) = match.groups()
        exports.append((export, absent, comment))

        
    return exports

def movelastTransients(exports):
    return  [e for e in exports if e[0] not in TRANSIENTS] + \
            [e for e in exports if e[0] in TRANSIENTS]

def removeSpecific(exports):
    return  [e for e in exports if e[0] not in TRANSIENTS_SPECIFIC]

def absentTransients(exports):
    newExports = []
    for e in exports:
        if e[0] in TRANSIENTS:
            e = (e[0], " ABSENT", e[2])
        newExports.append(e)
    return newExports

def removeTransients(exports):
    return  [e for e in exports if e[0] not in TRANSIENTS]

def defreeze(exports):
    defrozen = []
    for export, absent, comment in exports:
        if export in ALLOWEDENTRIES:
            defrozen.append((export, absent, comment))
        else:
            break
    return defrozen

def generate(exports):
    output = "EXPORTS\n"

    for index in range(len(exports)):
        (export, absent, comment) = exports[index]
        output = output + "\t%s @ %i NONAME%s%s\n" % (
            export, index + 1, absent or "", comment or "")
    return output + "\n"

def list(exports):
    return "\n    ".join(["'%s'," % e[0] for e in exports])


def main():
    parser = OptionParser()
    parser.add_option("-f", dest = "outputFile")
    parser.add_option("-d", dest = "outputDir")
    parser.add_option("-o", dest = "overwrite",
                      action = "store_true", default = False)
    parser.add_option("--reorder", dest = "reorder",
                      action = "store_true", default = False,
                      help = "reorder transient exports to last")
    parser.add_option("--removeSpecific", dest = "removeSpecific",
                      action = "store_true", default = False,
                      help = "remove exports which are specific to some environment")
    parser.add_option("--removeTransients", dest = "removeTransients",
                      action = "store_true", default = False,
                      help = "remove all exports which are transients")
    parser.add_option("--absentify", dest = "absentify",
                      action = "store_true", default = False,
                      help = "make absent transients exports")
    parser.add_option("--defreeze", dest = "defreeze",
                      action = "store_true", default = False,
                      help = "defreeze all except configured exports")
    parser.add_option("--list", dest = "list",
                      action = "store_true", default = False)
    (options, args) = parser.parse_args()

    # Read
    for file in sum([glob.glob(arg) for arg in args], []):
        exports = readExports(file)

        # Modify
        if options.reorder:
            exports = movelastTransients(exports)
        if options.removeSpecific:
            exports = removeSpecific(exports)
        if options.absentify:
            exports = absentTransients(exports)
        if options.defreeze:
            exports = defreeze(exports)
        if options.removeTransients:
            exports = removeTransients(exports)

        # Generate / list
        if options.list:
            output = list(exports)
        else:
            output = generate(exports)

        # Write / print
        if options.outputFile:
            open(options.outputFile, "w").write(output)
        elif options.outputDir:
            open(os.path.join(options.outputDir, os.path.basename(file)), 
                 "w").write(output)
        elif options.overwrite:
            open(file, "w").write(output)
            print "Modified %s" % file
        else:
            print output        

if __name__ == "__main__":
    main()


