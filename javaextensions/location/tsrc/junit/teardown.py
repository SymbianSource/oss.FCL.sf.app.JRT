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

    if (os.path.isfile(btpsyfile+'.tmp')):
        # Recover Bluetooth GPS PSY file from temp file
        os.rename(btpsyfile+'.tmp',btpsyfile)
        print "Recovered: " + btpsyfile
    if (os.path.isfile(npppsyfile+'.tmp')):
        # Recover NPP PSY file from temp file
        os.rename(npppsyfile+'.tmp',npppsyfile)
        print "Recovered: " + npppsyfile

if __name__ == "__main__":
	# If this module was started as the main module (given as the py-file to
	# python.exe) take command line arguments and pass them to replaceFile
	setup()