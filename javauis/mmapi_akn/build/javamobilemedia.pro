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

TARGET=javamobilemedia
TEMPLATE=lib
CONFIG += omj java stl
CONFIG -= qt

include(../../../inc/build_defines.pri)

DEFINES += RD_JAVA_VOLUME_CONTROL
DEFINES += RD_JAVA_OMA_DRM_V2

INCLUDEPATH +=  /epoc32/include/mmf/common \              
                ../animated_gif/inc \
                ../baseline/inc \
                ../src_drmv2/inc \
                ../directcontent/inc \
                ../camerasound/inc \
                ../volumekeys/inc \
                ../audiostreaming/inc

SOURCES +=  ../baseline/src/*.cpp \
            ../animated_gif/src/*.cpp \
            ../camerasound/src/*.cpp \
            ../directcontent/src/*.cpp \
            ../src_drmv2/src/*.cpp \
            ../volumekeys/src/*.cpp \
            ../audiostreaming/src/*.cpp
            
contains(PROJECT_DEFINES,RD_JAVA_HTTP_EMC_ENABLED) {
        INCLUDEPATH +=  ../baseline/inc.emc \
        ../audiostreaming/inc.emc
        
        SOURCES += ../baseline/src.emc/*.cpp \
         ../audiostreaming/src.emc/*.cpp
         
        LIBS += -lEnhancedMediaClient \
        -lMetaDataUtility \
        -lapmime\
        -lapgrfx \
				-lapmime \
				-lflogger 
				
}
else{
			  INCLUDEPATH +=  ../baseline/inc.mmf  \
        ../audiostreaming/inc.mmf
        
			  SOURCES += ../baseline/src.mmf/*.cpp \
			  ../audiostreaming/src.mmf/*.cpp
}

contains(PROJECT_DEFINES,RD_JAVA_NGA_ENABLED) {
				INCLUDEPATH +=  ../baseline/inc.nga
        
        SOURCES += ../baseline/src.nga/*.cpp
        
        LIBS += -lmediaclientvideodisplay \
                -lsensrvclient \
                -lsensrvutil \
                -lfbscli
}
else {
				INCLUDEPATH +=  ../baseline/inc.dsa
        
        SOURCES += ../baseline/src.dsa/*.cpp
}

LIBS += -lAknIcon \
        -laknskins \
        -laknskinsrv \
        -lapgrfx \
        -lapmime \
        -lBitmapTransforms \
        -lDRMHelper \
        -lDrmAudioPlayUtility \
        -lImageConversion \
        -lIHL \
        -lMMFControllerFramework \
        -lMediaClientAudio \
        -lMmfStandardCustomCommands \
        -lRemConCoreApi \
        -lRemConInterfaceBase \
        -lbafl \
        -lbitgdi \
        -lcaf \
        -lcafutils \
        -lcentralrepository \
        -lcone \
        -lecam \
        -lecom \
        -leswt \
        -lefsrv \
        -leikcoctl \
        -leikcore \ 
        -lesock \
        -leuser \
        -lfbscli \
        -lflogger \
        -ljavalegacyutils \
        -ljavaremconobserver \
        -lmediaclientvideo \
        -lmidiclient \
        -lws32 \
        -lhxmetadatautil \
        -lprofileeng \
        -laccmonitor


!contains(PROJECT_DEFINES,RD_JAVA_HTTP_EMC_ENABLED) {
    defBlock = "deffile ./~/javamobilemedia_5_0.def"
}

include(../../../build/omj.pri)
