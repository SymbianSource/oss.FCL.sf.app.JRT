#!/usr/bin/python
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
# Description: This script checks the build logs for warnings and categorizes
# them into various categories. Returns 1 if there are BAD warnings. 
#

import sys
from optparse import OptionParser


class WarningHandler:

    # Counters for various warnings
    totalWarnings = 0
    badOnes = 0
    deprecatedWarnings = 0
    compilerWarnings = 0
    linkerWarnings = 0
    postlinkerWarnings = 0
    flmWarnings = 0

    # Constants for matching warnings related to deprecation
    deprecatedStart = "warning: preprocessor #warning directive"
    deprecatedSecondLine = "warning: #warning This header file has been deprecated. Will be removed in one of the next SDK releases."
    deprecatedOptionalThirdLine = "warning: (included from:"
    deprecatedOptionalRest = "warning:  "

    # This list includes strings from which the BAD warnings can be recognized.
    # Note that these must be in lower case!
    badWarnings = [
        "#111-d", "#1166-d", "#117-d", "#128-d", "#1293-d", "#1441-d", "#170-d", 
        "#174-d", "#175-d", "#185-d", "#186-d", "#223-d", "#231-d", "#257-d", 
        "#284-d", "#368-d", "#414-d", "#430-d", "#47-d", "#514-d", "#546-d",
        "#68-d", "#69-d", "#830-d", "#940-d", "c2874w", "c4127", "c4355", "c4530",
        "c4702", "c4786", "lnk4049", "#836-d", "a1495e", "l6318w" ]

        
    def IsBadWarning(self, lowerCasedLine):
        for bad in self.badWarnings:
            if bad in lowerCasedLine:
                return True
        return False
        
    
    def PrintResults(self):
        print "\n-------------------------------\n"
        print "Total warnings:", self.totalWarnings
        print "Bad ones:      ", self.badOnes
        print ""
        print "Details:"
        print "  FLM warnings:            ", self.flmWarnings
        print "  Use of deprecated api:   ", self.deprecatedWarnings
        print "  Other compiler warnings: ", self.compilerWarnings
        print "  Linker warnings:         ", self.linkerWarnings
        print "  Post-linker warnings:    ", self.postlinkerWarnings

    

class PrintSettings:
    """Class parsing and maintaining the printing settings related to warnings"""
  
    printDeprecatedWarnings = False
    printCompilerWarnings = False
    printLinkerWarnings = False
    printFlmWarnings = False
 
    def parseArgs(self):
        parser = OptionParser(
            usage = "python -u %prog [options] <logfile>",
            description = "Analyzes warnings from the given log file. " + \
                "By default prints only BAD warnings and statistics.")
        parser.add_option("--printall", dest="printall", action="store_true", 
            default=False, help="Prints all the warnings")
        parser.add_option("--pc", dest="printcompilerwarnings", action="store_true", 
            default=False, help="Prints compiler warnings")
        parser.add_option("--pl", dest="printlinkerwarnings", action="store_true", 
            default=False, help="Prints linker warnings")
        parser.add_option("--pd", dest="printdeprecatedwarnings", action="store_true", 
            default=False, help="Prints deprecation warnings")
        parser.add_option("--pf", dest="printflmwarnings", action="store_true", 
            default=False, help="Prints FLM warnings")
        (opts, args) = parser.parse_args()

        if len(args) != 1:
            parser.print_help()
            sys.exit(-1)
            
        if opts.printall:
            self.printDeprecatedWarnings = True
            self.printCompilerWarnings = True
            self.printLinkerWarnings = True
            self.printFlmWarnings = True
        else:
            self.printDeprecatedWarnings = opts.printdeprecatedwarnings
            self.printCompilerWarnings = opts.printcompilerwarnings
            self.printLinkerWarnings = opts.printlinkerwarnings
            self.printFlmWarnings = opts.printflmwarnings
        
        return args[0]

        
 
# Calculates the number of various warnings from the given file containing 
# the console output from a Raptor build. 
#
# BAD warnings are those warnings that are considered so important that builds
# should have always zero of them. The related log message strings are defined
# in the variable badWarnings above.
#
# The warnings are further categorized as deprecated API warnings, compiler
# warnings, linker, and post-linker warnings.
#
def main():

    # Parse command line options and log file name.
    settings = PrintSettings()
    path = settings.parseArgs()

    # Read in the lines
    f = open(path)
    lines = f.readlines()
    f.close()
    
    # Initialize the warning handler
    wh = WarningHandler()
    
    # Initial state for the state machine, see the method stateMachine for
    # information about the other states
    state = 0
    underCompilation = ""
    
    # Check all the lines for possible warnings
    for line in lines:
    
        # Reset the target always when having line of form " xyz"
        # Works also with empty lines as the newline is counted in
        if line[0] == ' ' and line[1] != ' ':
            underCompilation = line
            state = 1;
            continue

        state = stateMachine(state, line, underCompilation, settings, wh)

    # Done with the parsing, just dump the statistics
    wh.PrintResults()
    
    if wh.badOnes > 0:
        sys.exit(1)
    else:
        sys.exit(0)
        

# The state machine function doing the actual work
def stateMachine(state, line, underCompilation, settings, wh):

    # Looking for any warning related to the current target
    if state == 1:
    
        # Check first for the start of a multiline deprecation warning
        if wh.deprecatedStart in line:
            if settings.printDeprecatedWarnings:
                print underCompilation,
                print line,
            wh.deprecatedWarnings += 1
            wh.totalWarnings += 1
            return 2
            
        else:
        
            # Then check for all the other warnings
            lowerCasedLine = line.lower()
            if "warning:" in lowerCasedLine:
                wh.totalWarnings += 1

                # Check for bad warnings
                isBad = wh.IsBadWarning(lowerCasedLine)
                if isBad:
                    wh.badOnes += 1                    

                # Categorize and print the warning (BAD warnings are printed always)
                if ("mwldsym2.exe: warning" in lowerCasedLine):
                    wh.linkerWarnings += 1
                    if settings.printLinkerWarnings or isBad:
                        print underCompilation,
                        print line,
                elif ("elf2e32: warning:" in lowerCasedLine):
                    wh.postlinkerWarnings += 1
                    if settings.printLinkerWarnings or isBad:
                        print underCompilation,
                        print line,
                elif ("makedef warning:" in lowerCasedLine):
                    wh.postlinkerWarnings += 1
                    if settings.printLinkerWarnings:
                        print underCompilation,
                        print line,
                    return 5   # find the rest of the lines
                elif ("warning: overriding commands for target" in lowerCasedLine or
                       "warning: ignoring old commands for target" in lowerCasedLine):
                    wh.flmWarnings += 1
                    if settings.printFlmWarnings or isBad:
                        print underCompilation,
                        print line,                    
                elif (": warning:" in lowerCasedLine):
                    wh.compilerWarnings += 1
                    if settings.printCompilerWarnings or isBad:
                        print underCompilation,
                        print line,
                    
            return 1
    
    # Looking for the second line of the multiline deprecation warning
    if state == 2:
        if wh.deprecatedSecondLine in line:
            if settings.printDeprecatedWarnings:
                print line,
            return 3
        else:
            print "Missing second line"
            return 1           
    
    # Looking for the optional third line of the multiline deprecation warning
    if state == 3:
        if wh.deprecatedOptionalThirdLine in line:
            if settings.printDeprecatedWarnings:
                print line,
            return 4
        else:
            # Hmm... went one line too far -> need to check the current line again
            # but now in the state 1
            return stateMachine(1, line, underCompilation, settings, wh)            

    # Looking for the optional trailing lines of the multiline deprecation warning
    if state == 4:
        if wh.deprecatedOptionalRest in line:
            if settings.printDeprecatedWarnings:
                print line,
            return 4
        else:
            # Hmm... went one line too far -> need to check the current line again
            # but now in the state 1           
            return stateMachine(1, line, underCompilation, settings, wh)
                    
    # Looking for MAKEDEF detailed information lines
    if state == 5:
        if settings.printLinkerWarnings:
            print line,
        return 1

                            

if __name__ == "__main__":
    main()
