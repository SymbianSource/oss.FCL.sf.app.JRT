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
CONFIG += omj java

QT += core

symbian {
    
    CONFIG +=hb
   
    INCLUDEPATH += ../../inc ../../../../inc

    HEADERS += ../../inc/CSoftNotification.h ../../inc/autorelease.h
    SOURCES += ../../src/os.cpp ../../src/CSoftNotification.cpp
   
    LIBS += -lcone
    LIBS += -ltouchfeedback
    LIBS += -lHbCore
    LIBS += -lbafl
    LIBS += -lws32
    LIBS += -lapparc
    LIBS += -lapgrfx  
}

include(../../../../build/omj.pri)
