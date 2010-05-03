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
TARGET=javausersettingsconfigurator
CONFIG += omj no_icon stl
CONFIG -= qt

symbian {
    TARGET.EPOCSTACKSIZE = 0x5000
    TARGET.UID2          = 0x20004c45
    TARGET.UID3          = 0x20022E7A
    TARGET.CAPABILITY    = WriteDeviceData TrustedUI

    INCLUDEPATH += ../../javaapppreconverter/src.s60/midletlist.h \
                   ../../../javacommons/gcfprotocols/connectionmanager/inc.s60

    SOURCES += ../src.s60/*.cpp ../../javaapppreconverter/src.s60/midletlist.cpp

    LIBS += -ledbms -ljavastorage -lestor -lmidp2securitypolicyV2 -lcentralrepository -lefsrv -ljavaconnectionmanager
    }

include(../../../build/omj.pri)
