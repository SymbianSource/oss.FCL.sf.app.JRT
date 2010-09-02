#
# Copyright (c) 2009 Nokia Corporation and/or its subsidiary(-ies).
# All rights reserved.
# This component and the accompanying materials are made available
# under the terms of the License "Eclipse Public License v1.0"
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

TARGET=javawmamms
TEMPLATE=lib
CONFIG += omj java stl
CONFIG -= qt

symbian {
    INCLUDEPATH += ../inc.s60 \
                   ../../inc 
    
    SOURCES += ../src/*.cpp \
               ../src.s60/*.cpp 
    
     LIBS += -lapmime \
             -lplatformenv \
             -lcharconv \
             -lCommonUI \ 
             -lefsrv \
             -lestor \
             -ljavammsscplugin \
             -lmsgs \
             -lmmsappadapter \
             -lmmsserversettings \
             -ljavagcf
}




include(../../../../build/omj.pri)
