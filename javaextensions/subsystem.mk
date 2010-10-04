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
	iapinfo/build \
	mobinfo/build \
	comm/build \
	satsa/build \
	location/build \
	sensor/build

# Component webservices is needed only if the package hasn't been split
ifndef RD_JAVA_PKG_SPLIT
   COMPONENTS += webservices/build
endif        	

ifeq ($(UI),AVKON)
# Broadcast stub is available for now only on AVKON environments (where also MMAPI exists)
COMPONENTS += \
	broadcast_stub/build
endif

SYMBIAN_ONLY = pim wma bluetooth satsa/build location/build sensor/build iapinfo/build mobinfo/build comm/build broadcast_stub/build ccapi

LINUX_ONLY =

SKIP_IN_SF = ccapi

SUBSYSTEMS = \
    midppush \
    bluetooth \
    datagram \
    wma \
    pim

ifdef RD_JAVA_S60_RELEASE_9_2_ONWARDS
    COMPONENTS += centralrepository/build
endif

ifdef RD_JAVA_MIDPRMS_DB
    SUBSYSTEMS += midprms_db
else
    COMPONENTS += midprms/build
endif

# Include old globalindicator only TB 9.2 and older releases
ifndef RD_JAVA_S60_RELEASE_10_1_ONWARDS
    COMPONENTS += globalindicators/build
    SYMBIAN_ONLY += globalindicators/build
endif

# Include CCAPI JSR 257 and CCAPI extensions only on 9.2 java PS2
ifdef RD_JAVA_S60_RELEASE_9_2
    SUBSYSTEMS += ccapi		                  
endif

# Declare that release preparation removes subdirectories not in build, except
# listed special cases
REMOVE_NOTBUILT_EXCLUDING = inc

include ${JAVA_SRC_ROOT}/build/Makefile.subsystem
