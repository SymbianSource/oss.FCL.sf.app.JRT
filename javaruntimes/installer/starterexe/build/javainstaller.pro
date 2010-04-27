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
TARGET=javainstaller
CONFIG += omj no_icon stl
CONFIG -= qt

LIBS += -ljavaruntimestarterutils

symbian {
    TARGET.EPOCSTACKSIZE = 0x5000
    TARGET.EPOCHEAPSIZE = 0x00001000 0x02000000

    TARGET.UID2 = 0x2001843A
    TARGET.UID3 = 0x2001843A

    # MIDP capabilities
    TARGET.CAPABILITY = \
        LocalServices NetworkServices ProtServ WriteDeviceData \
        ReadDeviceData ReadUserData WriteUserData UserEnvironment \
        SwEvent Location MultiMediaDD NetworkControl TrustedUI
}

include(../../../../build/omj.pri)
