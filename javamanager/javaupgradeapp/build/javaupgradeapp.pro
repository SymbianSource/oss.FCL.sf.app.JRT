#
# Copyright (c) 2010 Nokia Corporation and/or its subsidiary(-ies).
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
# Description: Helper application for uninstalling a java application and 
#    then installing new application (java or native)
#
# 

TEMPLATE=app
TARGET=javaupgradeapp
CONFIG += omj no_icon stl
CONFIG -= qt

symbian {
    TARGET.UID2          = 0x2000CFEC
    TARGET.UID3          = 0x2002FF64
    TARGET.CAPABILITY = TrustedUI 
    LIBS +=  -lestor -lapparc -lapgrfx
}

include(../../../build/omj.pri)
