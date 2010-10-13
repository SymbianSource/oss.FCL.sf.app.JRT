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
TARGET=javamidp
CONFIG += omj no_icon stl
CONFIG -= qt

include(../../../inc/build_defines.pri)

symbian {

contains(PROJECT_DEFINES,RD_JAVA_UI_QT) {
    TARGET.EPOCSTACKSIZE = 81920
}

!contains(PROJECT_DEFINES,RD_JAVA_UI_QT) {
    TARGET.EPOCSTACKSIZE = 16384
}

    TARGET.UID2 = 0x102033E6
    TARGET.UID3 = 0x102033E6

    TARGET.SID = 0x102033E6
    TARGET.VID = 0x101FB657

    TARGET.CAPABILITY = \
        LocalServices NetworkServices ProtServ WriteDeviceData \
        ReadDeviceData ReadUserData WriteUserData UserEnvironment  \
        SwEvent Location MultiMediaDD NetworkControl TrustedUI
}
# For wins the max heap is 16 MB and for arm it is 64 MB
MMP_RULES += "$${LITERAL_HASH}ifdef WINSCW"
MMP_RULES += "EPOCHEAPSIZE        0x00001000 0x01000000"
MMP_RULES += "$${LITERAL_HASH}else // WINSCW"
MMP_RULES += "EPOCHEAPSIZE        0x00001000 0x04000000"
MMP_RULES += "$${LITERAL_HASH}endif // WINSCW"

include(../../../build/omj.pri)
