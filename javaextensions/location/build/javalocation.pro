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

TARGET=javalocation
TEMPLATE=lib
CONFIG += omj java stl
CONFIG -= qt

include(../../../inc/build_defines.pri)

symbian {
    INCLUDEPATH += ../common/inc \
               ../orientation/inc \
               ../landmarks/inc

    SOURCES += ../common/src/*.cpp \
               ../orientation/src/*.cpp \
               ../landmarks/src/*.cpp

contains(PROJECT_DEFINES,RD_JAVA_PROXIMITY_LISTENER_ENABLED){
    INCLUDEPATH += ../position/inc
    SOURCES += ../position/src/*.cpp

    LIBS += -llbt \
            -llbtcontainer \
            -llbtlogger \
            -llbtmanager \
            -llbtserverlogicbase \
            -llbtstrategypluginapi \
            -lltmvmtdetapi
}
else{
    INCLUDEPATH += ../position/inc/cdelaystatechangetimer.h \
                   ../position/inc/clocationprovider.h \
                   ../position/inc/cpositioner.h \
                   ../position/inc/cpositionerbase.h \
                   ../position/inc/ctimeouttimer.h \
                   ../position/inc/ctrackingpositioner.h

    SOURCES += ../position/src/cdelaystatechangetimer.cpp \
               ../position/src/clocationprovider.cpp \
               ../position/src/cpositioner.cpp \
               ../position/src/cpositionerbase.cpp \
               ../position/src/ctimeouttimer.cpp \
               ../position/src/ctrackingpositioner.cpp \
               ../position/src/locationproviderimpl.cpp \
               ../position/src/positioner.cpp
}

    LIBS += -lbafl \
            -lefsrv \
            -leuser \
            -lsensrvutil \
            -lsensrvclient \
            -leposlandmarks \
            -leposlmdbmanlib \
            -leposlmsearchlib \
            -llbs
}

include(../../../build/omj.pri)
