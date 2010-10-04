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
TARGET=javaregistry
CONFIG += omj no_icon stl
CONFIG -= qt


symbian {
    INCLUDEPATH += ../inc \
                   ../inc/FromLegacy \
                   ../../../client/inc \
                   ../../../inc \
                   ../../../util/inc

    SOURCES += ../src/*.cpp
    TARGET.UID2 = 0x1000008d
    TARGET.UID3 = 0x10282475
    TARGET.CAPABILITY = ProtServ

    LIBS += -ljavaregistryclient -lefsrv -lestor -ljavastorage -ljavafileutils
}


include(../../../../../build/omj.pri)
