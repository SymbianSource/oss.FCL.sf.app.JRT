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
TARGET=javabluetooth
CONFIG += omj java stl
CONFIG -= qt

symbian {
    INCLUDEPATH += ../inc
    INCLUDEPATH += ../../bluetoothcommons/bluetoothplatformcontrol/inc
    INCLUDEPATH += ../../bluetoothcommons/bluetoothplatformcontrol/inc.s60
    INCLUDEPATH += ../../bluetoothcommons/inc.s60
    INCLUDEPATH += ../../bluetoothcommons/inc
    INCLUDEPATH += ../../bluetoothplugins/inc.s60
    INCLUDEPATH += ../../bluetoothplugins/inc
    INCLUDEPATH += ../../bluetoothplugins/btspppushplugin/inc.s60
    INCLUDEPATH += ../../bluetoothplugins/btspppushplugin/inc

    SOURCES += ../src.s60/*.cpp
}

LIBS += -lesock -lbluetooth -lsdpagent -lsdpdatabase -ljavagcf
LIBS += -ljavabluetoothcommons -ljavabluecove -ljavabtl2capscplugin -ljavabtsppscplugin -ljavabtgoepscplugin
//LIBS += -lbtmanclient -lbtdevice

include(../../../../build/omj.pri)
