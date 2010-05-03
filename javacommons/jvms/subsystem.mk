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
	nativeportlayer/build \
	j9utils/threaddump/build \
	j9

SYMBIAN_ONLY = nativeportlayer/build
LINUX_ONLY = 

include ${JAVA_SRC_ROOT}/build/Makefile.subsystem
