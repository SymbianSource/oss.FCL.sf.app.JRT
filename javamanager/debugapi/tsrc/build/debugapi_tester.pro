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
TARGET=javadebugapi_tester
CONFIG += omj no_icon stl
CONFIG -= qt

LIBS += -ljavadebugapi -ljavacomms -lflogger
LIBS += -lCppUTest

INCLUDEPATH += ../../../../tools/cpputest/include/CppUTest
INCLUDEPATH += ../../../../tools/cpputest/include/Platforms/Symbian

INCLUDEPATH += ../inc.s60 \
               ../../src.s60

SOURCES += ../src.s60/*.cpp \
           ../../src.s60/applauncher.cpp \
           ../../src.s60/installoperation.cpp \
           ../../src.s60/appinstaller.cpp \
           ../../src.s60/appremover.cpp \
           ../../src.s60/diagnosticimpl.cpp \

# SDK Debug Agent uids
TARGET.UID2 = 0x10274A0D
TARGET.UID3 = 0x10208DDD

TARGET.CAPABILITY = all -tcb -allfiles -drm
#-TrustedUI -NetworkControl -PowerMgmt


include(../../../../build/omj.pri)
