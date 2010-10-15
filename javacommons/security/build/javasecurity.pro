#
# Copyright (c) 2009-2010 Nokia Corporation and/or its subsidiary(-ies).
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

include(../../../inc/build_defines.pri)

TEMPLATE=lib
TARGET=javasecurity
CONFIG += omj java stl
CONFIG -= qt

DEPENDPATH += ../inc ../src

SOURCES += ../src/utils/javaclassinstantiator.cpp \
            ../src/utils/storagehandler.cpp \
            ../src/utils/securityutils.cpp \
            ../src/utils/javacertstorehandler.cpp \
           ../src/midpauthenticationmodule.cpp \
           ../src/midpauthenticationmoduleimpl.cpp

INCLUDEPATH += ../inc \
                ../utils/inc \
               ../../../inc

LIBS += -ljavacomms -ljavastorage

symbian {
    DEPENDPATH += ../src.s60
    INCLUDEPATH += ../src.s60
    SOURCES += ../src.s60/*.cpp
    LIBS += -llibcrypto \
            -letel \
            -letelmm \
            -lefsrv \
            -lplatformenv \
            -lcertstore \
            -locsp \
            -locsptransport \
            -lx509 \
            -lcentralrepository \
            -lcaf

    contains(PROJECT_DEFINES,RD_JAVA_S60_RELEASE_5_0) {
        INCLUDEPATH += /src/common/generic/security/common/inc
    }

    contains(PROJECT_DEFINES,RD_JAVA_S60_RELEASE_10_1_ONWARDS) {
        CONFIG += qt hb
        TRANSLATIONS = javaapplicationsecuritymessages.ts
    }
}

!symbian {
    DEPENDPATH += ../src.linux
    INCLUDEPATH += ../src.linux
    SOURCES += ../src.linux/*.cpp
    LIBS += -lcrypto
}

include(../../../build/omj.pri)
