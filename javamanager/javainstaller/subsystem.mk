#
# Copyright (c) 2009-2010 Nokia Corporation and/or its subsidiary(-ies).
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
	installerui/build \
	installer/build \

NONQTSUBSYSTEMS = \
	appinstuiplugin/build

SYMBIAN_ONLY = \
	appinstuiplugin/build

ifdef SYMBIAN_UNIVERSAL_INSTALL_FRAMEWORK    
    NONQTSUBSYSTEMS += javasifplugin/build
    SYMBIAN_ONLY += javasifplugin/build
else
ifndef RD_JAVA_S60_RELEASE_5_0_IAD
    NONQTSUBSYSTEMS += iconsizenotifplugin/build
    SYMBIAN_ONLY += iconsizenotifplugin/build
endif
endif
    
LINUX_ONLY =

include ${JAVA_SRC_ROOT}/build/Makefile.subsystem
