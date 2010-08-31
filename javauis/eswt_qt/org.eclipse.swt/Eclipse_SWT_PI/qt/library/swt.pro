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

SWTLIBNAME = eswtqt
SWTGENERATEDDIR = $$PWD/generated/$$SWTLIBNAME

TEMPLATE = lib
TARGET = $$SWTLIBNAME

MOC_DIR= $$SWTGENERATEDDIR
include(eswt_widgets.pri)
include(./graphics/graphics.pri)

unix {
    INCLUDEPATH += $(JAVA_HOME)/include \
        $(JAVA_HOME)/include/linux
}

win32 {
    INCLUDEPATH += $(JAVA_HOME)/include \
        $(JAVA_HOME)/include/win32

    # Required for win32, can be replaced with an auto-generate
    DEF_FILE = swt.def
}
