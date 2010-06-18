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

javasidchecker/build : javaregistry

include ${JAVA_SRC_ROOT}/build/Makefile.subsystem
