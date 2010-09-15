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

include(../../../inc/build_defines.pri)

TARGET=javasatsa
TEMPLATE=lib
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


 			INCLUDEPATH +=  ../framework/inc	\
                            ../pki/inc	\
                            ../apdu/inc	\
 							../../inc	\
 							../../../inc \



 			SOURCES +=  ../pki/src.s60/*.cpp \
                        ../crypto/src/*.cpp \
                        ../apdu/src/*.cpp	\
                        ../apdu/src.s60/*.cpp   \
                        ../framework/src/satsajnitools.cpp \


 			LIBS +=    -ljavastorage	\
 								 -lCommonUI \
 								 -lrandom \
 								 -lx509	\
								 -lx500	\
								 -llibcrypto \
								 -lasn1	\
								 -lasnpkcs	\
								 -lpkcs10		\
								 -lcrypto	\
 								 -lbafl \
								 -lcertstore \
								 -lcharconv \
								 -lcms \
								 -leiksrv \
								 -lctframework \
								 -lcustomapi \
								 -lefsrv \
								 -letel \
								 -letelmm \
								 -leuser \
								 -lflogger \
								 -lhash \
								 -lWimClient	\
								 -lestlib	\
								 -lcryptography \


contains(PROJECT_DEFINES,RD_JAVA_S60_RELEASE_5_0) {
        LIBS +=    -laknnotify \
        					 -lctsecdlgs \
    }
contains(PROJECT_DEFINES,RD_JAVA_S60_RELEASE_9_2) {
        LIBS +=    -laknnotify \
        					 -lctsecdlgs \
    }
		
 }

include(../../../build/omj.pri)
