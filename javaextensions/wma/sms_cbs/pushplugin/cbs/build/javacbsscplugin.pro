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
TARGET=javacbsscplugin
CONFIG += omj staticdata stl
CONFIG -= qt

symbian {
    SOURCES += ../src/*.cpp \
               ../../src/*.cpp \
               ../src.s60/*.cpp 

    LIBS += -lbafl \
            -lcharconv \
            -lcommdb \
            -ljavagcf \
            -lestor \
            -letel \
            -letelmm \
            -lgsmu \
            -ljavafileutils \
            -lsmcm 
}




include(../../../../../../build/omj.pri)
