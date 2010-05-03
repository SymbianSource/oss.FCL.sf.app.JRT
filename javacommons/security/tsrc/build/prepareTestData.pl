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

use File::Find;
$cp = $ENV{'OMJ_PP_CP'};
$root_dir = $ENV{'OMJ_PP_ROOT_DIR'};
$jar_dir = $ENV{'OMJ_PP_JAR_DIR'};
@javac = ("javac", "-cp", "$cp", ,"-d", "classes", "myClass.java");
my $current_dir;
sub process_dir {
    if (-d $_)
    {
        $current_dir = $_;
    }
    $current_file = $_;
    @jar = ("jar", "-cf", "$jar_dir$current_dir.jar","-C","classes",".");
    if ($current_file eq "myClass.java")
    {
       system("mkdir","classes");
       system(@javac);
       system(@jar);
    }
}
find(\&process_dir, $root_dir);
