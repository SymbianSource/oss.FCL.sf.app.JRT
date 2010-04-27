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

QT += core gui

INCLUDEPATH += ../inc
INCLUDEPATH += ../src/jni
INCLUDEPATH += ../../../../eSWT/org.eclipse.swt/Eclipse_SWT_PI/qt/library/graphics

SOURCES += ../src/*.cpp
         
DEFINES += M3G_TARGET_SYMBIAN
DEFINES += M3G_NATIVE_LOADER

LIBS += -lcone \
        -lezlib \
#        -lfbscli \
        -llibgles_cm \
        -lm3gcore \
        -leswt-qt

include(../../../build/omj.pri)
