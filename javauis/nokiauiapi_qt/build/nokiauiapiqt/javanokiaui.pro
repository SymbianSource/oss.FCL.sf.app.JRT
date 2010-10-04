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

TEMPLATE=lib
TARGET=javanokiaui
CONFIG += omj java stl

symbian {
    
    CONFIG +=hb
   
    INCLUDEPATH += ../../inc ../../../../inc

    HEADERS += ../../inc/CSoftNotification.h ../../inc/autorelease.h ../../inc/CGlobalindicators.h
    SOURCES += ../../src/os.cpp ../../src/CSoftNotification.cpp ../../src/CGlobalIndicators.cpp
   
    LIBS += -lcone
    LIBS += -ltouchfeedback
    LIBS += -lHbCore
    LIBS += -lbafl
    LIBS += -lws32
    LIBS += -lapparc
    LIBS += -lapgrfx
    LIBS += -ljavastorage
    LIBS += -llibstdcpp
    LIBS += -ljavautils
}

include(../../../../build/omj.pri)
