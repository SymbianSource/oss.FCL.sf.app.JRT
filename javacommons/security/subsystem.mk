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

COMPONENTS = \
	build \
	javaunicertstoreplugin/build

ifdef RD_JAVA_S60_RELEASE_5_0_IAD
SUBSYSTEMS = \
	legacysupport
endif

SYMBIAN_ONLY = \
    javaunicertstoreplugin/build
ifdef RD_JAVA_S60_RELEASE_5_0_IAD
SYMBIAN_ONLY += \
    legacysupport
endif

LINUX_ONLY =  

include ${JAVA_SRC_ROOT}/build/Makefile.subsystem

