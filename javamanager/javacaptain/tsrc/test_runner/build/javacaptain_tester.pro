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
TARGET=javacaptain_tester
CONFIG += omj no_icon stl
CONFIG -= qt

LIBS += -ljavacomms
LIBS += -ljavastorage
LIBS += -lCppUTest

INCLUDEPATH += ../../../../../tools/cpputest/include/CppUTest

symbian {
INCLUDEPATH += ../../../../../tools/cpputest/include/Platforms/Symbian
}
!symbian {
INCLUDEPATH += ../../../../../tools/cpputest/include/Platforms/Gcc
LIBPATH +=     ../../../../../tools/cpputest/lib
}

symbian {
    TARGET.EPOCSTACKSIZE = 0x5000
    TARGET.UID2          = 0
    TARGET.UID3          = 0xE0000200
    TARGET.CAPABILITY    = all -tcb -allfiles -drm
}

include(../../../../../build/omj.pri)
