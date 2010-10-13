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
TARGET=midp2userpreferences
CONFIG += omj stl
CONFIG -= qt

INCLUDEPATH += /epoc32/include/j2me/midp2/data \
               ../../../../../inc/j2me/midp2/security \
               ../../../../../inc/j2me/midp2/runtime \
               ../../../../../inc/j2me/midp2/dbv2 \
               ../../storageutils/inc

SOURCES += ../../storageutils/src/*.cpp \
           ../src/*.cpp

LIBS += -ljavastorage
include(../../../../../build/omj.pri)
