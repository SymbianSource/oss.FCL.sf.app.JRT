#
# Copyright (c) 2010 Nokia Corporation and/or its subsidiary(-ies).
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
# Description: This application is executed by the native installer
#              after a new version of Jrt has been installed from
#              .sisx package on top of older Jrt version (e.g. when
#              updating Jrt 2.1 to Jrt 2.2 via IAD).
#              This application restarts Java Captain so that new
#              version of the Jrt dlls is taken into use.
#              All running MIDlets exit when Java Captain closes.
#

TEMPLATE=app
TARGET=javafinishjrtinstall
CONFIG += omj no_icon stl
CONFIG -= qt

symbian {
    TARGET.EPOCSTACKSIZE = 0x5000
    TARGET.UID3          = 0x20031E4D
    TARGET.CAPABILITY    = PowerMgmt

    SOURCES += ../src.s60/*.cpp

    LIBS += -ljavautils -ljavacomms
    }

include(../../../build/omj.pri)
