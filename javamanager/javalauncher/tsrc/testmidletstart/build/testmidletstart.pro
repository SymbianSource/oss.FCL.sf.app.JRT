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
TARGET=testmidletstart
CONFIG += omj no_icon stl
CONFIG -= qt

symbian {
    TARGET.UID2          = 0
    TARGET.UID3          = 0x20E1E262
    TARGET.CAPABILITY    = WriteDeviceData


    LIBS +=  -lestor -lapparc -lapgrfx -lefsrv

}

include(../../../../../build/omj.pri)
