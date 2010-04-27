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
TARGET=javapostupdater
CONFIG += omj no_icon stl
CONFIG -= qt

symbian {
    TARGET.EPOCSTACKSIZE = 0x5000
    TARGET.EPOCHEAPSIZE  = 0x4000 0x1000000
    TARGET.UID3          = 0x20022EF6
    TARGET.CAPABILITY    = WriteDeviceData PowerMgmt TrustedUI
    
    LIBS += -lapgrfx -lswinstcli -lcentralrepository
    }

include(../../../build/omj.pri)
