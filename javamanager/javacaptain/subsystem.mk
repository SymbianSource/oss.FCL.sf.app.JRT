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
    extensionplugins/config/build \
    extensionplugins/javacertstore/build \
    extensionplugins/storageserver/build \
    extensionplugins/settingslistener/build \
    $(LINUX_ONLY)


ALL_TARGETS = $(COMPONENTS) $(SUBSYSTEMS) $(NONQTSUBSYSTEMS)

# Filter targets by platform
ifeq ($(BUILDENV),symbian)
ifdef RD_JAVA_S60_RELEASE_5_0_IAD
NONQTSUBSYSTEMS = \
	systemams/build
endif

COMPONENTS += extensionplugins/preinstallerstarter/build
COMPONENTS += extensionplugins/autostarter/build
ifdef SYMBIAN_UNIVERSAL_INSTALL_FRAMEWORK
COMPONENTS += extensionplugins/scrupdater/build
endif
SUBTARGETS = $(filter-out $(LINUX_ONLY), $(ALL_TARGETS))
else
SUBTARGETS = $(filter-out $(SYMBIAN_ONLY), $(ALL_TARGETS))
endif

include ${JAVA_SRC_ROOT}/build/Makefile.subsystem
