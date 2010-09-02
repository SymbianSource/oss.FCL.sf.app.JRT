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

TEMPLATE=app
TARGET=testjavautils
CONFIG += omj no_icon stl
CONFIG -= qt

LIBS += -lCppUTest

INCLUDEPATH += ../../../../../tools/cpputest/include/CppUTest

INCLUDEPATH += ../../../../../tools/cpputest/include/Platforms/Symbian
INCLUDEPATH += ../testcoreui/src
TARGET.CAPABILITY = all -tcb

#SOURCES += ../../src/SymbianAllTests.cpp
SOURCES += ../../src/AllTests.cpp
SOURCES += ../../src/uidtests.cpp
SOURCES += ../../src/testbase64.cpp
SOURCES += ../../src/testlanguageresource.cpp
SOURCES += ../../src/testoslayer.cpp

include(../../../../../build/omj.pri)
