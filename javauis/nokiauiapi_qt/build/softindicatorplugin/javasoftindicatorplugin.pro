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

TEMPLATE=lib
TARGET=javasoftindicatorplugin
CONFIG += plugin 
QT += core
symbian : plugin {
    
        CONFIG +=hb
                
        INCLUDEPATH += ../../softindicatorplugin/inc ../../../../inc
                
        HEADERS += ../../softindicatorplugin/inc/javasoftindicator.h ../../softindicatorplugin/inc/javasoftindicatorplugin.h
        SOURCES += ../../softindicatorplugin/src/javasoftindicator.cpp ../../softindicatorplugin/src/javasoftindicatorplugin.cpp
                
        TARGET.EPOCALLOWDLLDATA = 1
        TARGET.CAPABILITY = ALL -TCB

        PLUGIN_STUB_PATH = /resource/plugins/indicators
        pluginstub.sources = $${TARGET}.dll
        pluginstub.path = $$PLUGIN_STUB_PATH
    
        DEPLOYMENT += pluginstub
    
        qtplugins.path = $$PLUGIN_STUB_PATH
        qtplugins.sources += qmakepluginstubs/$${TARGET}.qtplugin
                
        for(qtplugin, qtplugins.sources):BLD_INF_RULES.prj_exports += "./$$qtplugin  z:$$qtplugins.path/$$basename(qtplugin)"    
                
        LIBS += -lHbCore
        LIBS += -lxqservice
        LIBS += -lws32
        LIBS += -lapparc
        LIBS += -lapgrfx
        LIBS += -lefsrv
                
    
    include(../../../../build/symbian_uids.pri)
}

