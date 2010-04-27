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

TEMPLATE=app
TARGET=javacleaner
CONFIG += omj no_icon stl
CONFIG -= qt

symbian {
    TARGET.EPOCSTACKSIZE = 0x5000
    TARGET.UID3          = 0x2002119A
    TARGET.CAPABILITY    = WriteDeviceData TrustedUI

    SOURCES += ../src.s60/*.cpp   
    
    LIBS += -lapgrfx
}

include(../../../build/omj.pri)
