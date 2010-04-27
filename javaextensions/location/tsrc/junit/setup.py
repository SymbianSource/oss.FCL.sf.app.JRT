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

import os

def setup():
    
    # NPP and BT GPS PSY Cenrep file location
    btpsyfile = "\\epoc32\\RELEASE\\winscw\\UDEB\\Z\\private\\10202be9\\101FE999.txt"
    npppsyfile = "\\epoc32\\RELEASE\\winscw\\UDEB\\Z\\private\\10202be9\\10206915.txt"

    # Path where landmark databases lie in the emulator environment
    lmpath = "\\epoc32\\winscw\\c\\Private\\100012a5\\"

    print "Setup for Location API module tests"

    if (os.path.isfile(btpsyfile)):
        # Remove old temp file if still exists
        if( os.path.isfile(btpsyfile+'.tmp')):
            os.remove(btpsyfile+'.tmp')
        # Rename existing Bluetooth GPS PSY file to temp file
        os.rename(btpsyfile,btpsyfile+'.tmp')
        print "Renamed: " + btpsyfile
    if (os.path.isfile(npppsyfile)):
        # Remove old temp file if still exists
        if( os.path.isfile(npppsyfile+'.tmp')):
            os.remove(npppsyfile+'.tmp')
        # Rename existing NPP PSY file to temp file
        os.rename(npppsyfile,npppsyfile+'.tmp')
        print "Renamed: " + npppsyfile
        
    # Compile test PSY to the emulator environment
    if (os.path.exists('..\\ViperTestPSY\\group\\')):
        os.chdir(r'..\\ViperTestPSY\\group\\')
        os.system(r'bldmake bldfiles')
        os.system(r'abld cleanexport')
        os.system(r'abld build winscw udeb')

    # Remove existing landmark databases from the emulator
    if (os.path.exists(lmpath)):
        print "Deleting landmark databases"
        os.system(r'del '+lmpath+'*.ldb')

    print "Setup is done."

if __name__ == "__main__":
	# If this module was started as the main module (given as the py-file to
	# python.exe) take command line arguments and pass them to replaceFile
	setup()
