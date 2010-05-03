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

TARGET=javawma
TEMPLATE=lib
CONFIG += omj java stl
CONFIG -= qt

symbian {
    INCLUDEPATH += ../../inc \
                   ../inc.s60 
    
    SOURCES += ../src/*.cpp \
               ../src.s60/*.cpp 
               
    LIBS += -ljavacbsscplugin \
            -lcharconv \
            -lefsrv \
            -lesock \
            -lestor \
            -lgsmu \
            -lsmcm \
            -ljavasmsscplugin \
            -lsmsu \
            -ljavagcf
}

include(../../../../build/omj.pri)
