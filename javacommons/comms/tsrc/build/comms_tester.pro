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
TARGET=comms_tester
CONFIG += omj no_icon stl
CONFIG -= qt

LIBS += -lCppUTest -ljavacomms -ljavaipc

INCLUDEPATH += ../../../../tools/cpputest/include/CppUTest

symbian {
INCLUDEPATH += ../../../../tools/cpputest/include/Platforms/Symbian

TARGET.UID2 = 0x20004c45
TARGET.UID3 = 0xE0000031
TARGET.CAPABILITY = all -tcb -allfiles -drm
# Sometimes it makes sense to run tests with reduced capa set
# Removing capa below will fail corresponding permission check test
# -TrustedUI -WriteDeviceData -PowerMgmt -NetworkControl
}

!symbian {
INCLUDEPATH += ../../../../tools/cpputest/include/Platforms/Gcc
LIBPATH +=     ../../../../tools/cpputest/lib
}

include(../../../../build/omj.pri)
