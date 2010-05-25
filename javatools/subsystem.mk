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

# Common
COMPONENTS = \
	tckrunner/starter/build \
	tckrunner/runner/build

# Platform specific
ifeq ($(BUILDENV),symbian)

COMPONENTS += \
	javasecuritycustomization/build

ifdef RD_JAVA_S60_RELEASE_5_0_IAD

COMPONENTS += \
	javacleaner/build \
	javaappconverter/build \
	javaapppreconverter/build \
	javaappbackconverter/build \
	javaupdater/build \
	javapostupdater/build \
	certificatesconfigurator/build \
	usersettingsconfigurator/build \
	javarestoreconverter/build 

endif

NONQTSUBSYSTEMS += \
	javacaptain_app/build \
	testcerts

endif

# Declare that release preparation removes subdirectories not in build, except
# listed special cases
REMOVE_NOTBUILT_EXCLUDING = releng 

include ${JAVA_SRC_ROOT}/build/Makefile.subsystem


