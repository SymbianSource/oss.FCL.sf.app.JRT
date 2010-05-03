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
TARGET=javaappbackconverter
CONFIG += omj no_icon stl
CONFIG -= qt

symbian {
    TARGET.EPOCSTACKSIZE = 0x5000
    TARGET.EPOCHEAPSIZE  = 0x4000 0x500000
    TARGET.UID3          = 0x20022D90
    TARGET.CAPABILITY    = WriteDeviceData PowerMgmt

    INCLUDEPATH += ../../javaapppreconverter/src.s60
    
    SOURCES += ../src.s60/*.cpp  ../../javaapppreconverter/src.s60/midletlist.cpp
    
    LIBS += -lefsrv \
            -lapgrfx \
            -lestor \
            -lflogger \
            -laknnotify \
            -lcentralrepository 
}


include(../../../build/omj.pri)
