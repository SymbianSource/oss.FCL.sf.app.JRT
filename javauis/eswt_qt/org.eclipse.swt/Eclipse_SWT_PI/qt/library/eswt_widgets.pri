#*******************************************************************************
# Copyright (c) 2008, 2010 Nokia Corporation and/or its subsidiary(-ies).
# All rights reserved. This program and the accompanying materials
# are made available under the terms of the Eclipse Public License v1.0
# which accompanies this distribution, and is available at
# http://www.eclipse.org/legal/epl-v10.html
#
# Contributors:
#     Nokia Corporation - initial implementation
#*******************************************************************************

QT += core \
      gui \
      svg

DEFINES += SWTQT_LIBRARY

INCLUDEPATH += $$PWD
DEPENDPATH += $$PWD

HEADERS += \
    eventcallback.h \
    slotcallback.h \
    swtlog.h \
    jniutils.h \
    swt.h \
    swterror.h \
    autorelease.h \
    listdatamodel.h \
    qcaptionedwidget.h \
    qswttimeedit.h \
    qtasktipbar.h \
    swtapplication.h \
    qswttabwidget.h \
    execexceptionhandler.h \
    swtfontcache.h \
    swtbufferflushevent.h \
    swtexport.h

SOURCES += \
    eventcallback.cpp \
    slotcallback.cpp \
    os.cpp \
    swtlog.cpp \
    jniutils.cpp \
    listdatamodel.cpp \
    qcaptionedwidget.cpp \
    qswttimeedit.cpp \
    qtasktipbar.cpp \
    swtapplication.cpp \
    qswttabwidget.cpp \
    swtfontcache.cpp

##### Symbian specific items ######
symbian {
    CONFIG += mobility
    MOBILITY += bearer

    CONFIG +=hb
    HEADERS += \
        swts60.h \
        swtmobiledevice.h \
        swtflipwatch.h

    SOURCES += \
        swts60.cpp \
        swtmobiledevice.cpp \
        swtflipwatch.cpp

    LIBS += \
        -leikcore \
        -lapparc \
        -lefsrv \
        -lcone \
        -lavkon \
        -leikcoctl \
        -lapgrfx \
        -lHWRMVibraClient \
        -lHWRMLightClient \
        -lxqservice \
        -lxqtelephonyservice \
        -lQtContacts \
        -lhal \
        -lws32 \
        -lgdi \
        -lHbCore \
        -lfbscli
}
