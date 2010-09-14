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

TARGET=javaiapinfo
TEMPLATE=lib
CONFIG += omj java stl
CONFIG -= qt

symbian {
    INCLUDES += ../inc.s60
    SOURCES += ../src.s60/javacommdb.cpp \
        ../src.s60/iapinfoimpl.cpp \
        ../src.s60/javaapncontrol.cpp \
        ../src.s60/stringbuffer.cpp \
        ../src.s60/iapinfosession.cpp
    LIBS +=   -leuser -lcommsdat  -lcommdb -lagentdialog -lcmmanager
}

include(../../../build/omj.pri)
