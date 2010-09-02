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
	build/java \
	build/native

SYMBIAN_ONLY = build/native

# Make dependency to those.
build/java build/native: $(JAVA_SRC_ROOT)/tools
include ${JAVA_SRC_ROOT}/build/Makefile.subsystem
