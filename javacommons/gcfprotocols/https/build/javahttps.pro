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

TARGET=javahttps
TEMPLATE=lib
CONFIG += omj java stl
CONFIG -= qt

symbian {
	INCLUDEPATH += ../../http/inc.s60
	LIBS += -leuser -lhttp -lecom -lbafl -linetprotutil -lhttpfiltercommon -lx509 -lx500 -lcrypto -ljavahttp
}



include(../../../../build/omj.pri)
