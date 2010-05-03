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

TEMPLATE=lib
TARGET=javacaptain_rtc_test1
CONFIG += omj staticdata stl
CONFIG -= qt

LIBS += -ljavacomms

SOURCES += ../src/*.cpp \
            ../../../src/rtcbaseplugin.cpp

symbian {
    TARGET.CAPABILITY    = all -tcb -allfiles -drm
    DEPENDPATH +=  ../inc.s60
    INCLUDEPATH += ../inc.s60
    SOURCES += ../src.s60/*.cpp
    TARGET.UID3 = 0xE0000081
}
include(../../../../../build/omj.pri)
