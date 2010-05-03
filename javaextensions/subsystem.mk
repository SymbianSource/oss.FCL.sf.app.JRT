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
	globalindicators/build \
	webservices/build \
	satsa/build \
	location/build \
	sensor/build

SYMBIAN_ONLY = pim wma bluetooth satsa/build location/build sensor/build iapinfo/build mobinfo/build globalindicators/build comm/build

LINUX_ONLY =


SUBSYSTEMS = \
    midppush \
    bluetooth \
    datagram \
    wma \
    pim

ifdef RD_JAVA_MIDPRMS_DB
    SUBSYSTEMS += midprms_db
else
    COMPONENTS += midprms/build
endif

include ${JAVA_SRC_ROOT}/build/Makefile.subsystem
