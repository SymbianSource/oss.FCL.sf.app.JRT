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
include(../../../../inc/build_defines.pri)

TARGET=javahttp
TEMPLATE=lib
CONFIG += omj java stl
CONFIG -= qt

symbian {

    LIBS += -leuser -lhttp -lecom -lbafl -linetprotutil -lhttpfiltercommon -lx509 -lx500 -lcrypto -lesock -lcommdb -lcentralrepository -lwebutils

    contains(PROJECT_DEFINES,RD_JAVA_S60_RELEASE_5_0) {
        LIBS += -lcommdb
    }
    contains(PROJECT_DEFINES,RD_JAVA_S60_RELEASE_9_2_ONWARDS) {
        LIBS += -lextendedconnpref -lnetmeta
    }
}

include(../../../../build/omj.pri)
