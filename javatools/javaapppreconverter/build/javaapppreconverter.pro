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
#    This process needs AllFiles capability to be able to read
#    the icon files of all midlets and to store midlet data
#    to the data cage of the javaappbackconverter process


TEMPLATE=app
TARGET=javaapppreconverter
CONFIG += omj no_icon stl
CONFIG -= qt

symbian {
    TARGET.EPOCSTACKSIZE = 0x5000
    TARGET.EPOCHEAPSIZE  = 0x4000 0x500000
    TARGET.UID3          = 0x20022D8F
    TARGET.CAPABILITY    = ReadDeviceData WriteDeviceData PowerMgmt AllFiles

    SOURCES += ../src.s60/*.cpp

    INCLUDEPATH += ../legacy/inc

    LIBS += -lefsrv \
            -lapgrfx \
            -lapparc \
            -lmidp2dbv2 \
            -ledbms \
            -lestor \
            -lflogger
}


include(../../../build/omj.pri)
