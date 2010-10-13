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
# Description: Java Captain extension plugin for Comms Module tests
#

TARGET=javacaptain_ext_ondemand_101
TEMPLATE=lib
CONFIG += omj stl
CONFIG -= qt

LIBS += -ljavacomms

SOURCES += ../src/*.cpp

symbian {
    SOURCES += ../src.s60/lookup.cpp
}
include(../../../../../build/omj.pri)
