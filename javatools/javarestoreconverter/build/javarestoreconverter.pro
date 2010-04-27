#
# Copyright (c) 2009 Nokia Corporation and/or its subsidiary(-ies).
# All rights reserved.
# This component and the accompanying materials are made available
# under the terms of the License "Eclipse Public License v1.0"
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
#    This process need AllFiles capability to be able to
#    find all java applications to be converted. (They are
#    in the data cage of the java runtime process.)


TEMPLATE=app
TARGET=javarestoreconverter
CONFIG += omj no_icon stl
CONFIG -= qt

symbian {
    TARGET.EPOCSTACKSIZE = 0x5000
    TARGET.UID2          = 0
    TARGET.UID3          = 0x2002B3EA
    TARGET.CAPABILITY    = WriteDeviceData AllFiles TrustedUI

    SOURCES += ../src.s60/*.cpp

    LIBS += -lefsrv \
            -lPlatformEnv \
            -ljavacomms \
            -lcharconv \
            -lapparc \
            -ljavastorage \
            -lezip 
}

include(../../../build/omj.pri)
