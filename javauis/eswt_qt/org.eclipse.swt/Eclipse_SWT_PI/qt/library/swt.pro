#******************************************************************************
# Copyright (c) 2009, 2010 Nokia Corporation and/or its subsidiary(-ies).
# All rights reserved. This program and the accompanying materials
# are made available under the terms of the Eclipse Public License v1.0
# which accompanies this distribution, and is available at
# http://www.eclipse.org/legal/epl-v10.html
#
# Contributors:
#     Nokia Corporation - initial implementation
#******************************************************************************/

TEMPLATE = lib
TARGET = eswtqt

QT += core \
      gui \
      webkit

MOC_DIR= ./generated
include(eswt_widgets.pri)
include(./graphics/graphics.pri)

unix {
    INCLUDEPATH +=$(JAVA_HOME)/include \
        $(JAVA_HOME)/include/linux
}

win32 {
    INCLUDEPATH += $(JAVA_HOME)/include \
        $(JAVA_HOME)/include/win32

        # Required for win32, can be replaced with an auto-generate
        DEF_FILE = swt.def
}

symbian {
    TARGET.UID =0x101FD687
    TARGET.CAPABILITY ="ALL -TCB"

    # Temporarily allow WSD, to be removed later if possible
    TARGET.EPOCALLOWDLLDATA = 1

    # Prepend to includepath to avoid problems with Qt's overriding files
    INCLUDEPATH = \epoc32\include \
                  \epoc32\include\stdapis \
                  \epoc32\include\osextensions\stdapis\sys \
                  \epoc32\include\osextensions\stdapis\stlport \
                  \epoc32\include\osextensions\stdapis\stlport\stl \
                  $$INCLUDEPATH

    INCLUDEPATH += \epoc32\include\middleware \
                   \epoc32\include\domain\middleware

    # J9 VM specific definitions
    DEFINES+=J9EPOC32

    # Std cpp requirements
    LIBS += -llibc
    LIBS += -leuser
    LIBS += -llibstdcpp
    
    DEFINES+= MACRO_WCHAR_T_DECLARED
    QMAKE_CXXFLAGS.CW += -wchar_t on

    # jni lookup
    SOURCES += $$PWD/generated/jni_lookup.cpp
}
