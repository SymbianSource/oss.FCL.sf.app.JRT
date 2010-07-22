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
                ../ammscontrol/inc \
                ../../mmapi_qt/baseline/inc \
                ../../mmapi_qt/utils/inc


SOURCES +=  ../ammscontrol/src/*.cpp \
            ../ammscontrol/audio3D/src/*.cpp \
            ../ammscontrol/audioeffect/src/*.cpp \
            ../jni/src/*.cpp \
            ../mmacontrol/src/*.cpp \
            ../module/src/*.cpp \
            ../../mmapi_qt/utils/src/*.cpp
          
contains(PROJECT_DEFINES,RD_JAVA_HTTP_EMC_ENABLED) {
        INCLUDEPATH +=  ../../mmapi_qt/baseline/inc.emc \
        ../mmacontrol/inc.emc \
        
        SOURCES += ../mmacontrol/src.emc/*.cpp
         
         
        LIBS += -lEnhancedMediaClient \
        -lmmfdevsound
}
else{
			 INCLUDEPATH +=  ../../mmapi_qt/baseline/inc.mmf \
        ../mmacontrol/inc.mmf \
        
			  SOURCES += ../mmacontrol/src.mmf/*.cpp
			  
}

contains(PROJECT_DEFINES,RD_JAVA_NGA_ENABLED) {
				INCLUDEPATH +=  ../../mmapi_qt/baseline/inc.nga
}
else {
				INCLUDEPATH +=  ../../mmapi_qt/baseline/inc.dsa
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
        -ljavamobilemedia \
        -lmidiclient \
        -lmmfcontrollerframework \
        -lmmfstandardcustomcommands \
        -laudiooutputrouting \
        -laccmonitor

include(../../../build/omj.pri)
