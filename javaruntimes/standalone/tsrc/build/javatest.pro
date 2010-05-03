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
TARGET=javastandalone
CONFIG += omj no_icon stl
CONFIG -= qt

LIBS += -ljavastarter

symbian {

    TARGET.EPOCSTACKSIZE = 16384
    TARGET.EPOCHEAPSIZE = 0x00001000 0x02000000
    TARGET.UID2 = 0x100039CE
    TARGET.UID3 = 0x2002703C

    TARGET.SID = 0x2002703D

    TARGET.CAPABILITY = \
        LocalServices NetworkServices ProtServ WriteDeviceData \
        ReadDeviceData ReadUserData WriteUserData UserEnvironment  \
        SwEvent Location MultiMediaDD NetworkControl TrustedUI
    # S60 resources for javainstaller

    MMP_RULES += "START RESOURCE ../data/javastandalone_reg.rss"
    MMP_RULES += "TARGETPATH /private/10003a3f/apps"
    MMP_RULES += "END //RESOURCE"
}

include(../../../../build/omj.pri)
