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
	test_extensionplugin/build \
	test_runner/build \
	test_rtcplugin1/build \
	test_rtcplugin2/build \
	test_runtime/build \
	$(SYMBIAN_ONLY) \
	$(LINUX_ONLY)

SYMBIAN_ONLY =
LINUX_ONLY = app_launcher/build

ALL_TARGETS = $(COMPONENTS) $(SUBSYSTEMS)

# Filter targets by platform
ifeq ($(BUILDENV),symbian)
SUBTARGETS = $(filter-out $(LINUX_ONLY), $(ALL_TARGETS))
else
SUBTARGETS = $(filter-out $(SYMBIAN_ONLY), $(ALL_TARGETS))
endif

include ${JAVA_SRC_ROOT}/build/Makefile.subsystem
