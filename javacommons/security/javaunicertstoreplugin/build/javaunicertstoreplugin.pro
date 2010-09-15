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
TARGET=javaunicertstoreplugin
CONFIG += omj stl
CONFIG -= qt

contains(PROJECT_DEFINES,RD_JAVA_S60_RELEASE_10_1_ONWARDS) {
    CONFIG += qt hb
    SOURCES += ../src.s60/confirmquery.cpp
    HEADERS += ../src.s60/confirmquery.h
}
symbian {
    TARGET.UID2 = 0x10009D8D
    TARGET.UID3 = 0x200213A3
    TARGET.TARGETTYPE=PLUGIN
    TARGET.CAPABILITY = CAP_ECOM_PLUGIN
    MMP_RULES += "START RESOURCE 200213A3.rss"
    MMP_RULES += "TARGETPATH /resource/plugins"   
    MMP_RULES += "TARGET javaunicertstoreplugin.rsc"
    MMP_RULES += "END"
    MMP_RULES += "START RESOURCE 200213A3_iad.rss"
    MMP_RULES += "TARGETPATH /resource/java/iad"   
    MMP_RULES += "TARGET javaunicertstoreplugin.rsc"
    MMP_RULES += "END"
    MMP_RULES += "START RESOURCE ../data/javausermessages.rss"
    MMP_RULES += "HEADER"
    MMP_RULES += "TARGETPATH /resource/java"
    MMP_RULES += "LANGUAGE_IDS"
    MMP_RULES += "END"
    DEPENDPATH += ../src.s60
    INCLUDEPATH += ../src.s60 ../../../../inc
    SOURCES += ../src.s60/javacertstoreimpl.cpp \
               ../src.s60/javacertstoretoken.cpp \
               ../src.s60/javacertstoretokentype.cpp \
               ../src.s60/main.cpp
    LIBS += -lctframework \
            -lefsrv \
            -lestor \
            -lx509 \
            -ljavacomms \
            -lCommonEngine
    !contains(PROJECT_DEFINES,RD_JAVA_S60_RELEASE_10_1_ONWARDS) {
        LIBS += -laknnotify
    }
}

include(../../../../build/omj.pri)
