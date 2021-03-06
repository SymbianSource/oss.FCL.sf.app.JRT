#
# Copyright (c) 2010 Nokia Corporation and/or its subsidiary(-ies).
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

# Include the test frameworks.
SUBSYSTEMS = \
	$(JAVA_SRC_ROOT)/tools

COMPONENTS = \
	java_api/build \
	storageclient/build

# Make dependency to those.
java_api/build storageclient/build: $(JAVA_SRC_ROOT)/tools
include ${JAVA_SRC_ROOT}/build/Makefile.subsystem
