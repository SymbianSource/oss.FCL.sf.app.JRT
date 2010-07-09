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

include(../../../../inc/build_defines.pri)

symbian {
    TEMPLATE=lib
    TARGET=javainstaller
    DEPENDPATH += ../src.s60/applicationregistrator \
                  ../src.s60/iconconverter \
                  ../src.s60/jadjarmatcher \
                  ../src.s60/utils

    CONFIG += omj java stl
    contains(PROJECT_DEFINES,RD_JAVA_S60_RELEASE_5_0) {
        CONFIG -= qt
    }
    contains(PROJECT_DEFINES,RD_JAVA_S60_RELEASE_9_2) {
        CONFIG -= qt
    }

    TARGET.UID2 = 0x1000008d

    INCLUDEPATH += ../src.s60/applicationregistrator \
                   ../src.s60/iconconverter \
                   ../src.s60/jadjarmatcher \
                   ../src.s60/utils \
                   ../../iconsizenotifplugin/inc

    SOURCES += ../src.s60/applicationregistrator/applicationregistrator.cpp \
               ../src.s60/applicationregistrator/sifnotifier.cpp \
               ../src.s60/applicationregistrator/sifregistrator.cpp \
               ../src.s60/iconconverter/iconconverter.cpp \
               ../src.s60/iconconverter/mifconverter.cpp \
               ../src.s60/jadjarmatcher/jadjarmatcher.cpp \
               ../src.s60/jadjarmatcher/jadjarmatcherscanfrominbox.cpp \
               ../src.s60/utils/filewriter.cpp \
               ../src.s60/utils/fileroots.cpp \
               ../src.s60/utils/sysutil.cpp

    LIBS += -lapgrfx -lbafl -lcentralrepository -lcharconv -lefsrv -lestor \
               -lezip -lfbscli -limageconversion -lmsgs  \
               -lsysutil -lplatformenv -lws32 -lapparc -lcentralrepository \
               -lhal -lcaf -lcafutils -lbitmaptransforms

    contains(PROJECT_DEFINES,RD_JAVA_S60_50_REL2) {
        LIBS += -lmcsmenu
    }

    contains(PROJECT_DEFINES,RD_JAVA_S60_RELEASE_10_1_ONWARDS) {
        CONFIG += hb
        LIBS += -lsifnotification -lxqservice
    }

    MMP_RULES += \
    "$${LITERAL_HASH}include <bldvariant.hrh>" \
    "$${LITERAL_HASH}ifdef SYMBIAN_UNIVERSAL_INSTALL_FRAMEWORK" \
    "LIBRARY scrclient.lib" \
    "$${LITERAL_HASH}endif"
} else {
    CONFIG += omj java javaonly stl
}


include(../../../../build/omj.pri)
