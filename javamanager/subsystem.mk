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

include $(JAVA_SRC_ROOT)/build/Makefile.defs

SUBSYSTEMS = \
	javacaptain \
	javainstaller \
	javaregistry \
	javabackup \
	javasettings 

COMPONENTS = \
	preinstaller/build \
	debugapi/build \
	javalauncher/build \
	javaupgradeapp/build 
    
NONQTSUBSYSTEMS = \
	javarecognizer/build 

SYMBIAN_ONLY = \
	javaregistry \
	javalauncher/build \
	javaupgradeapp/build \
	javabackup \
	javarecognizer/build \
	preinstaller/build \
	debugapi/build 

ifndef RD_JAVA_S60_RELEASE_10_1_ONWARDS    
NONQTSUBSYSTEMS += javasidchecker/build javaappschemeplugin/build 
SYMBIAN_ONLY += javasidchecker/build javaappschemeplugin/build 
javasidchecker/build : javaregistry
else
COMPONENTS += javaappscheme/build javaqtrequest/build 
SYMBIAN_ONLY += javaappscheme/build javaqtrequest/build 
endif

ifdef RD_JAVA_APPLICATION_SETTINGS_QT
COMPONENTS += javasettings_qt/build 
SYMBIAN_ONLY += javasettings_qt/build 
endif

# Declare that release preparation removes subdirectories not in build, except
# listed special cases
REMOVE_NOTBUILT_EXCLUDING = inc

include ${JAVA_SRC_ROOT}/build/Makefile.subsystem
