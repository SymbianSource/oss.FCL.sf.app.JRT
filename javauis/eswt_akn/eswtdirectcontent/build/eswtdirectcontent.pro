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
TARGET=eswtdirectcontent
CONFIG += omj java stl
CONFIG -= qt

INCLUDEPATH +=  ../native/inc \
                ../../org.eclipse.ercp.swt.s60/native/inc

SOURCES += ../native/src/dccontrol.cpp \
           ../native/src/swtdccontrol.cpp \
           ../native/src/swtdcobserver.cpp
         
# Symbian version is created from Qt version 'major . (minor << 8) + patch'
VERSION = 12.74.121 # "12.74.121 --> 12.19065 on symbian"

LIBS += -lavkon \
        -lbafl \
        -lbitgdi \
        -lcone \
        -legul \ 
        -leikcore \
        -leikcoctl \
        -lestlib \
        -leuser \
        -lfbscli \
        -lgdi \
        -lws32 \
        -lapparc \
        -leiksrvc \
        -lapgrfx \
        -lflogger \
        -leswt

include(../../../../build/omj.pri)
