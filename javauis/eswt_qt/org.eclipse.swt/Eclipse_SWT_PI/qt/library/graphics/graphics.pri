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

INCLUDEPATH += $$PWD \
               $$PWD/qt
               
symbian {
INCLUDEPATH += /sf/mw/qt/src/gui/painting
}

DEPENDPATH += $$PWD \
              $$PWD/qt

HEADERS += $$PWD/qt/surfaceimpl.h \
    $$PWD/qt/pixmap.h \
    $$PWD/qt/imageloaderimpl.h \
    $$PWD/qt/imagedataimpl.h \
    $$PWD/qt/palettedataimpl.h \
    $$PWD/qt/graphicscontextimpl.h \
    $$PWD/qt/bufferimpl.h \
    $$PWD/qt/targetobserver.h \
    $$PWD/qt/windowsurfaceimpl.h \
    $$PWD/gfxlog.h \
    $$PWD/graphics.h \
    $$PWD/gfxutils.h

SOURCES += $$PWD/qt/surfaceimpl.cpp \
    $$PWD/qt/windowsurfaceimpl.cpp \
    $$PWD/qt/pixmap.cpp \
    $$PWD/qt/imageloaderimpl.cpp \
    $$PWD/qt/imagedataimpl.cpp \
    $$PWD/qt/palettedataimpl.cpp \
    $$PWD/qt/graphicscontextimpl.cpp \
    $$PWD/qt/bufferimpl.cpp \
    $$PWD/qt/targetobserver.cpp \
    $$PWD/graphicsfactory.cpp \
    $$PWD/gfxos.cpp \
    $$PWD/gfxutils.cpp

symbian {
LIBS += -llibEGL
}
