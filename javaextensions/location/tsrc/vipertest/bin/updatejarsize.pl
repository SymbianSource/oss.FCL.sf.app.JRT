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

$jarsize = (-s $ARGV[0]);

open (JAD, $ARGV[1]) or die "cannot open jad-file: $!";

$midletjarsize = "MIDlet-Jar-Size";

while (<JAD>) {
    if (/$midletjarsize+/) {
        print $midletjarsize.": $jarsize\n";       
    } else {
        print $_;
    }
}