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
TARGET=javam3g
CONFIG += omj java staticdata stl
CONFIG -= qt

INCLUDEPATH += ../inc
INCLUDEPATH += ../src/jni

SOURCES += ../src/*.cpp
         
DEFINES += M3G_TARGET_SYMBIAN
DEFINES += M3G_NATIVE_LOADER

LIBS += -lcone \
        -lezlib \
        -lfbscli \
        -ljavalegacyutils \
#ifdef RD_JAVA_NGA_ENABLED
        -llibegl \
        -llibglesv1_cm \
#else        
        -llibGLES_CM \
#endif        
        -lm3gcore

include(../../../build/omj.pri)
