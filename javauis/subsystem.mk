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
# Description: Makefile for the javauis collection
#

include $(JAVA_SRC_ROOT)/build/Makefile.defs


#
# Common components
#
NONQTSUBSYSTEMS += \
	coreui/build


#
# Common legacy utilities
#
SUBSYSTEMS += javalegacyutils

SYMBIAN_ONLY += javalegacyutils


#
# Then either Avkon or Qt based components and subsystems
#
ifeq ($(UI),AVKON)
include subsystem_akn.mk
else
include subsystem_qt.mk
endif


include ${JAVA_SRC_ROOT}/build/Makefile.subsystem
