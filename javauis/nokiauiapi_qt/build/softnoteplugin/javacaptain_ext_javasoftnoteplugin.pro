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
TARGET=javacaptain_ext_javasoftnoteplugin
CONFIG += omj stl 

symbian {
    
    CONFIG +=hb
    
	INCLUDEPATH += ../../softnoteplugin/inc ../../../../inc
                
 	HEADERS += ../../softnoteplugin/inc/javasoftnotehandler.h
  	SOURCES += ../../softnoteplugin/src.s60/lookup.cpp ../../softnoteplugin/src/javasoftnotehandler.cpp
    
    LIBS += -lHbCore
    LIBS += -ljavastorage
    LIBS += -llibstdcpp
    LIBS += -ljavautils
    LIBS += -lbafl
}
    
include(../../../../build/omj.pri)