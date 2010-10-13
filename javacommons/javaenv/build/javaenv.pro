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
TARGET=javaenvinfo
CONFIG += omj stl
CONFIG -= qt

symbian {
    TARGET.UID2 = 0x1000008d
    TARGET.UID3 = 0x2001B2A5
    SOURCES += ../src.s60/*.cpp
    LIBS += -lplatformEnv -lefsrv
}

include(../../../build/omj.pri)
