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

TEMPLATE=lib
TARGET=javabluetoothcommons
CONFIG += omj java stl

contains(PROJECT_DEFINES,RD_JAVA_S60_RELEASE_5_0) {
    CONFIG -= qt
}
contains(PROJECT_DEFINES,RD_JAVA_S60_RELEASE_9_2) {
    CONFIG -= qt
}
contains(PROJECT_DEFINES,RD_JAVA_S60_RELEASE_10_1_ONWARDS) {
    CONFIG += hb
}


symbian {	
    SOURCES += ../src/*.cpp 
    SOURCES += ../src.s60/*.cpp
    SOURCES += ../bluetoothplatformcontrol/src/*.cpp 
    SOURCES += ../bluetoothplatformcontrol/src.s60/*.cpp   
}

LIBS +=  -lsdpdatabase -lbtengsettings -lbluetooth -lbtmanclient \
         -lbtdevice -ljavacomms -ljavafileutils -lesock

include(../../../../build/omj.pri)


