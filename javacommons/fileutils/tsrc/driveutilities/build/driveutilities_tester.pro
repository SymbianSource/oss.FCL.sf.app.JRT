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

TARGET=driveutilities_tester
TEMPLATE=app
CONFIG += omj no_icon stl
CONFIG -= qt

LIBS += -lCppUTest -ljavafileutils -ljavacomms

INCLUDEPATH += ../../../src
INCLUDEPATH += ../../../../../tools/cpputest/include/CppUTest

symbian {
INCLUDEPATH += ../../../../../tools/cpputest/include/Platforms/Symbian
}
!symbian {
INCLUDEPATH += ../../../../../tools/cpputest/include/Platforms/Gcc
LIBPATH +=     ../../../../../tools/cpputest/lib
}

include(../../../../../build/omj.pri)
