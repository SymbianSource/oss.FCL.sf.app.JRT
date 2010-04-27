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
TARGET=midp2cenrep
CONFIG += omj stl
CONFIG -= qt

LIBS += -ljavastorage

symbian {
    INCLUDEPATH += ../inc.s60 \
                   ../../../inc.s60
    
    LIBS += -ljavaconnectionmanager
    
    SOURCES += ../src.s60/*.cpp
}

include(../../../../../build/omj.pri)
