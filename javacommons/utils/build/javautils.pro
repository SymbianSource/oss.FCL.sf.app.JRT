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
TARGET=javautils
CONFIG += omj java staticdata stl
CONFIG -= qt

include(../../../inc/build_defines.pri)

contains(PROJECT_DEFINES,RD_JAVA_UI_QT) {
  CONFIG += qt
  QT = core
}

SOURCES += ../src/*.cpp

symbian {
    INCLUDEPATH += ../inc.s60 ../functionserver/inc.s60
                   

    SOURCES += ../functionserver/src/*.cpp \
               ../functionserver/src.s60/*.cpp \
               ../src.s60/*.cpp \

      LIBS += -lflogger -lplatformver -lsysversioninfo -lsysutil -lbafl \
              -lefsrv -ljavaenvinfo -lhal -llibcrypto -lCommonEngine -lplatformenv

    !contains(PROJECT_DEFINES,RD_JAVA_UI_QT) {
        LIBS += -lavkon
    }
}

!symbian:debug {
  DEFINES += LOGGING_ON NGJAVA_LOGGER_ON NGJAVA_HEAVY_LOGGER_ON
}

!symbian {
    INCLUDEPATH += ../inc.linux
    SOURCES += ../src.linux/javaoslayer.cpp \
               ../src.linux/properties.cpp \
               ../src.linux/formatternative.cpp

    LIBS +=  -ldl -lcrypto
}

include(../../../build/omj.pri)
