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
	jvms \
	comms \
	javastorage \
	security \
	gcfprotocols 

COMPONENTS = \
	utils/build \
	gcfbase/build \
	connectionmanager/build \
  fileutils/build

ifeq ($(BUILDENV),symbian)
    COMPONENTS += javaenv/build
endif


# Define component/subsystem dependencies
utils/build: jvms
javastorage security: utils/build comms
security: javastorage fileutils/build
gcfbase/build: security
gcfprotocols: gcfbase/build security connectionmanager/build
fileutils/build: comms

ifdef RD_JAVA_S60_RELEASE_5_0_IAD
COMPONENTS += \
	connectionmanager/legacysupport/apnsettings/build

SYMBIAN_ONLY = connectionmanager/legacysupport/apnsettings/build
endif

include ${JAVA_SRC_ROOT}/build/Makefile.subsystem
