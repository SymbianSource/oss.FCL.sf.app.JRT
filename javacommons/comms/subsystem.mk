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
# Comms has different transport providers, e.g. socket or Symbian client/server.
# Only one transport provider can be used at the time. 
# Selection is done by uncommenting wanted transport provider below.


include $(JAVA_SRC_ROOT)/build/Makefile.defs

ifeq ($(BUILDENV),symbian)
COMPONENTS = \
	ipclib/clientserver/build
#	ipclib/socket/build
endif

ifeq ($(BUILDENV),linux)
COMPONENTS = \
	ipclib/socket/build
endif

COMPONENTS += \
    build

#ipclib/build is included only for exporting purposes!
NONQTSUBSYSTEMS += \
    ipclib/build

include ${JAVA_SRC_ROOT}/build/Makefile.subsystem

