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
	midp



COMPONENTS = \
	installer/starterdll/build \
	installer/starterexe/build \
	jvmargmodifier/default/build \
	jvmargmodifier/file/build \
	starterutils/build

ifdef RD_JAVA_S60_RELEASE_5_0_IAD
	COMPONENTS += starter/build.iad
else
	COMPONENTS += starter/build
endif

starterutils/build: jvmargmodifier/default/build
midp: starterutils/build
installer/starterdll/build: starterutils/build

include ${JAVA_SRC_ROOT}/build/Makefile.subsystem
