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
	javabackup

    
COMPONENTS = \
    preinstaller/build \
    debugapi/build \
    javalauncher/build

    
NONQTSUBSYSTEMS = \
	javasidchecker/build \
	javarecognizer/build \
    javaappschemeplugin/build

SYMBIAN_ONLY = \
	javaregistry \
	javalauncher/build \
	javabackup \
	javasidchecker/build \
	javarecognizer/build \
	preinstaller/build \
    debugapi/build \
    javaappschemeplugin/build

ifdef RD_JAVA_APPLICATION_SETTINGS_QT
COMPONENTS += \
    javasettings/appsettingsview_qt/build
SYMBIAN_ONLY += \
    javasettings/appsettingsview_qt/build
else
ifndef RD_JAVA_S60_RELEASE_5_0_IAD
NONQTSUBSYSTEMS += \
	javasettings/appmngrplugin/build
SYMBIAN_ONLY += \
	javasettings/appmngrplugin/build
endif
endif

LINUX_ONLY = 

javasidchecker/build : javaregistry

include ${JAVA_SRC_ROOT}/build/Makefile.subsystem
