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
TARGET=javastorage
CONFIG += omj java stl
CONFIG -= qt

SOURCES += ../src/client/*.cpp \
           ../src/*.cpp

LIBS += -ljavacomms

symbian {
    DEPENDPATH += ../src.s60
    INCLUDEPATH += ../inc.s60
    SOURCES += ../src.s60/client/*.cpp
    LIBS += -lsqldb
}

!symbian {
    DEPENDPATH += ../inc.linux ../src.linux ../src.linux/client
    INCLUDEPATH += ../inc.linux ../src.linux ../src.linux/client

    SOURCES += ../src.linux/client/*.cpp
    LIBS += -ljavacomms
}


include(../../../build/omj.pri)
