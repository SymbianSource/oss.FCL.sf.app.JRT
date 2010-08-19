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

TARGET=javaamms
TEMPLATE=lib
CONFIG += omj java stl
CONFIG -= qt

include(../../../inc/build_defines.pri)

DEFINES += RD_JAVA_VOLUME_CONTROL
DEFINES += RD_JAVA_OMA_DRM_V2
DEFINES += __JAVA_JSR234_TUNER


INCLUDEPATH +=  ../ammscontrol/inc \
                ../ammscontrol/audio3D/inc \
                ../ammscontrol/audioeffect/inc \
                ../mmacontrol/inc \
                ../module/inc \
                ../src_tuner/native/external_include \
                ../src_tuner/native/inc \
                ../ammscontrol/inc 


SOURCES +=  ../ammscontrol/src/*.cpp \
            ../ammscontrol/audio3D/src/*.cpp \
            ../ammscontrol/audioeffect/src/*.cpp \
            ../jni/src/*.cpp \
            ../mmacontrol/src/*.cpp \
            ../module/src/*.cpp
          
contains(PROJECT_DEFINES,RD_JAVA_HTTP_EMC_ENABLED) {
        INCLUDEPATH +=  ../mmacontrol/inc.emc \
        
        SOURCES += ../mmacontrol/src.emc/*.cpp
         
         
        LIBS += -lEnhancedMediaClient \
        -lmmfdevsound
}
else{
			 INCLUDEPATH += ../mmacontrol/inc.mmf \
        
			  SOURCES += ../mmacontrol/src.mmf/*.cpp
			  
}

LIBS += -lAudioEqualizerEffect \
        -lAudioEqualizerUtility \
        -lCustomCommandUtility \
        -lDistanceAttenuationEffect \
        -lDopplerBase \
        -lEnvironmentalReverbEffect \
        -lEnvironmentalReverbUtility \
        -lListenerDopplerEffect \
        -lListenerLocationEffect \
        -lListenerOrientationEffect \
        -lLocationBase \
        -lMediaClientAudio \
        -lOrientationBase \
        -lRoomLevelEffect \
        -lSourceDopplerEffect \
        -lSourceLocationEffect \
        -lStereoWideningEffect \
        -lStereoWideningUtility \
        -lbafl \
        -lcentralrepository \
        -lestor \
        -leuser \
        -lflogger \
        -ljavalegacyutils \
        -ljavamobilemedia \
        -lmidiclient \
        -lmmfcontrollerframework \
        -lmmfstandardcustomcommands \
        -laudiooutputrouting \
        -laccmonitor

include(../../../build/omj.pri)
