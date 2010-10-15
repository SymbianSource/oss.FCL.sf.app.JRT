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
# Description:
#

TEMPLATE=lib
TARGET=javacommonutilstest
CONFIG += omj java staticdata stl
CONFIG -= qt

include(../../../../../inc/build_defines.pri)

symbian {
    TARGET.UID3 = 0xE0045056
}

SOURCES += ../../testdll/src/*.cpp

include(../../../../../build/omj.pri)
