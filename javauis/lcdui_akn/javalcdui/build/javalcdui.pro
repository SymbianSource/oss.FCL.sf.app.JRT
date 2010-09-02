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
TARGET=javalcdui
CONFIG += omj java stl
CONFIG -= qt

INCLUDEPATH += ../inc.nokialcdui
INCLUDEPATH += ../../lcdui/inc

SOURCES += ../src.nokialcdui/*.cpp
SOURCES += ../src/*.cpp
         
LIBS += -lapgrfx \
        -lbafl \
        -lestor \
        -letext \
        -lapparc \
        -lavkon \
        -lbitgdi \
        -lcone \
        -leikcore \
        -leiksrvc \
        -leswt \
        -lfbscli \
        -ltouchfeedback \
        -ljavacoreui \
        -ljavalegacyutils \
        -lflogger \
        -lgdi \
        -lws32 \
        -lhwrmlightclient \
        -lgfxtrans \
        -lws32 \
        -lCentralRepository \ 
#ifdef RD_JAVA_NGA_ENABLED
	-llibegl_sw \
	-llibglesv1_cm
#endif // RD_JAVA_NGA_ENABLED

include(../../../../build/omj.pri)
