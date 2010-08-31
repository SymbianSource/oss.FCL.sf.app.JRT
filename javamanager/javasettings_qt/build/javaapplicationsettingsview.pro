#
# Copyright (c) 2010 Nokia Corporation and/or its subsidiary(-ies).
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
TEMPLATE = lib
TARGET = javaapplicationsettingsview
CONFIG += qt hb stl plugin mobility
MOBILITY += bearer
LIBS += -ljavastorage -lcmapplsettingsui -ljavaconnectionmanager -ljavautils
TRANSLATIONS = javaruntimeapplicationsettings.ts

INCLUDEPATH += ../../../inc
SOURCES += ../src/javaapplicationsettingsview.cpp ../src/javaapplicationsettingsview_p.cpp ../src/javaapplicationsettings.cpp
HEADERS += ../src/javaapplicationsettingsview.h ../src/javaapplicationsettingsview_p.h ../src/javaapplicationsettings.h

symbian : plugin {
    TARGET.EPOCALLOWDLLDATA = 1
    TARGET.CAPABILITY = ALL -TCB
    PLUGIN_STUB_PATH = /resource/qt/plugins/appsettings
    pluginstub.sources = $${TARGET}.dll
    pluginstub.path = $$PLUGIN_STUB_PATH
    DEPLOYMENT += pluginstub
    qtplugins.path = $$PLUGIN_STUB_PATH
    qtplugins.sources += qmakepluginstubs/$${TARGET}.qtplugin
    for(qtplugin, qtplugins.sources):BLD_INF_RULES.prj_exports += "./$$qtplugin  z:$$qtplugins.path/$$basename(qtplugin)"

    include(../../../build/symbian_uids.pri)

}

# Workaround for RVCT 4.0 QT/Symbian incmpatibility
MMP_RULES -= $$MMP_RULES_DONT_EXPORT_ALL_CLASS_IMPEDIMENTA
