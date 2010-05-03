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

TEMPLATE=app
TARGET=javalauncher
CONFIG += omj no_icon stl
CONFIG -= qt

symbian {
    TARGET.UID2          = 0x2000CFEC
    TARGET.UID3          = 0x2001E262
    TARGET.CAPABILITY = TrustedUI WriteDeviceData PowerMgmt NetworkControl
    LIBS +=  -lestor -lapparc -ljavacomms -lbafl -ljavastorage -ljavaruntimestarterutils
}

include(../../../build/omj.pri)
