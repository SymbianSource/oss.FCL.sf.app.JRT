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
	javalcdui/build

NONQTSUBSYSTEMS = \
	build \
	lcdgd/build \
	lcdgr/build \
	lcduiphysicswrapper/build \
	lcdui/build

include ${JAVA_SRC_ROOT}/build/Makefile.subsystem
