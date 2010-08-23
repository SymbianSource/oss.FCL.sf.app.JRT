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
TARGET=javacaptain
CONFIG += omj no_icon stl
CONFIG -= qt

INCLUDEPATH += ../rtcplugins/midp/inc \
               ../extensionplugins/boot/inc \
               ../extensionplugins/mmc/inc

DEPENDPATH +=  ../rtcplugins/midp/inc \
               ../extensionplugins/boot/inc \
               ../extensionplugins/mmc/inc

SOURCES += ../src/*.cpp \
           ../rtcplugins/midp/src/*.cpp \
           ../extensionplugins/boot/src/*.cpp \
           ../extensionplugins/mmc/src/*.cpp

LIBS +=  -ljavacomms -ljavaipc -ljavafileutils -ljavastorage

symbian {
    LIBS += -lefsrv
    TARGET.EPOCHEAPSIZE  = 0x2800 0x100000
    TARGET.EPOCSTACKSIZE = 0x2000
    TARGET.UID2          = 0x200211DC
    TARGET.UID3          = 0x200211DC
    TARGET.CAPABILITY    = all -tcb -allfiles -drm
    DEPENDPATH +=  ../inc.s60
    INCLUDEPATH += ../inc.s60
    SOURCES += ../src.s60/*.cpp

}

!symbian {
    LIBS +=   -lpthread -lrt
    DEPENDPATH +=  ../inc.linux
    INCLUDEPATH += ..inc.linux
    SOURCES += ../src.linux/*.cpp
}

include(../../../build/omj.pri)
