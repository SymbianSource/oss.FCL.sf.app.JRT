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
# Description: Generated file - do not edit manually
#
TEMPLATE = subdirs
SUBDIRS += preinstaller/build/javapreinstaller.pro 
SUBDIRS += debugapi/build/debugapi.pro 
SUBDIRS += javalauncher/build/javalauncher.pro 
SUBDIRS += javaupgradeapp/build/javaupgradeapp.pro 
SUBDIRS += javaappscheme/build/javaappscheme.pro 
SUBDIRS += javaqtrequest/build/javaqtrequest.pro 
SUBDIRS += javasettings_qt/build/javaapplicationsettingsview.pro 
SUBDIRS += javacaptain 
SUBDIRS += javainstaller 
SUBDIRS += javaregistry 
SUBDIRS += javabackup 
SUBDIRS += javasettings 
BLD_INF_RULES.prj_extensions += "$${LITERAL_HASH}include \"javarecognizer/build/bld.inf\""  
BLD_INF_RULES.prj_extensions += "prj_extensions" 
