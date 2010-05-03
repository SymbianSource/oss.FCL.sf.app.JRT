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

TEMPLATE=lib
TARGET=javaregistry
CONFIG += omj stl
CONFIG -= qt


symbian {
    INCLUDEPATH += ../../installedappsregistry/inc \
                   ../../../../../javautils/javastorage/inc \
                   ../../../inc \

    TARGET.UID2 = 0x1000008d
    TARGET.UID3 = 0x101Faaaa
    LIBS += -ljavaregistryclient -lefsrv -lestor -linstalledappsregistry -ljavastorage
}


include(../../../../../build/omj.pri)
