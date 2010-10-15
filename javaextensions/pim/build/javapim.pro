#
# Copyright (c) 2009-2010 Nokia Corporation and/or its subsidiary(-ies).
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

include(../../../inc/build_defines.pri)

TARGET=javapim
TEMPLATE=lib
CONFIG += omj java stl
CONFIG -= qt

symbian {
    
    INCLUDEPATH += ../framework/inc.s60 \
                   ../framework/inc \
                   ../common/inc \
                   ../common/inc.s60 \
                   ../cntadapter/inc.s60 \
                   ../inc.s60 \
                   ../jni/inc \
                   ../agnadapter/inc.s60 \
                   ../versit/inc.s60 \
                   ../javapimloc/src.s60
                    
   
    SOURCES += ../jni/src/*.cpp \
               ../framework/src.s60/*.cpp \            
               ../common/src.s60/*.cpp \
               ../cntadapter/src.s60/*.cpp \
               ../agnadapter/src.s60/*.cpp \
               ../versit/src.s60/*.cpp \
               ../javapimloc/src.s60/*.cpp
               
    
    
    LIBS += -lbafl \
            -lbitmaptransforms \
            -lcaleninterimutils2 \
            -lcalinterimapi \
            -lcntmodel \
            -lefsrv \
            -lestor \
            -lfbscli \
            -limageconversion \
            -ltzclient \
            -lvcal \
            -lversit \
            -lvcard \
            -lefsrv \
            -lCommonEngine \
            -lplatformenv
            
            
    contains(PROJECT_DEFINES,RD_JAVA_S60_RELEASE_10_1_ONWARDS) {
        CONFIG += qt hb
        TRANSLATIONS = javapim.ts
    }

# Resource files
MMP_RULES += "SOURCEPATH ../javapimloc/data"
MMP_RULES += "START RESOURCE pimcm.rss"
MMP_RULES += "TARGETPATH /resource/java"
MMP_RULES += "HEADER"
MMP_RULES += "LANGUAGE_IDS"
MMP_RULES += "END //RESOURCE"
MMP_RULES += "START RESOURCE pimevent.rss"
MMP_RULES += "TARGETPATH /resource/java"
MMP_RULES += "HEADER"
MMP_RULES += "LANGUAGE_IDS"
MMP_RULES += "END //RESOURCE"
MMP_RULES += "START RESOURCE pimtodo.rss"
MMP_RULES += "TARGETPATH /resource/java"
MMP_RULES += "HEADER"
MMP_RULES += "LANGUAGE_IDS"
MMP_RULES += "END //RESOURCE"

}


include(../../../build/omj.pri)
