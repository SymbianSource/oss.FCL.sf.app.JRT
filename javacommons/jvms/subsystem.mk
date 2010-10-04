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
	j9utils/threaddump/build

# Component j9 is needed only if the package hasn't been split
ifndef RD_JAVA_PKG_SPLIT
   COMPONENTS += j9
endif    

SYMBIAN_ONLY = nativeportlayer/build
LINUX_ONLY = 

# Declare that release preparation removes subdirectories not in build, except
# listed special cases
REMOVE_NOTBUILT_EXCLUDING = 


include ${JAVA_SRC_ROOT}/build/Makefile.subsystem
