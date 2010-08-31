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
# Description: QT Service application the implements support for
#               starting Java applications using "javaapp:" QUrl
#

TEMPLATE=app
TARGET=javaappscheme
CONFIG += omj no_icon stl service
CONFIG -= gui

symbian {
    TARGET.UID3       = 0x200315F0
    TARGET.CAPABILITY = CAP_APPLICATION
    LIBS += -lxqservice -lxqserviceutil
}

SERVICE.FILE = service_conf.xml
SERVICE.OPTIONS = embeddable
SERVICE.OPTIONS += hidden

HEADERS=../src.s60/serviceapp.h 

include(../../../build/omj.pri)
