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
# Description: Simple application for testing Sif API 
#              (install, uninstall, get component info)
#

TEMPLATE=app
TARGET=testsifapi
CONFIG += omj no_icon stl
CONFIG -= qt

symbian {
    TARGET.UID2          = 0
    TARGET.UID3          = 0xE0E5E262
    TARGET.CAPABILITY    = all -tcb -drm


    LIBS +=  -lestor -lapparc -lapgrfx -lefsrv -lsif

}

include(../../../../../build/omj.pri)
