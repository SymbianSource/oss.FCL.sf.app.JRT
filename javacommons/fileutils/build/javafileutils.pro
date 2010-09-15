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

TARGET=javafileutils
TEMPLATE=lib
CONFIG += omj java staticdata stl
CONFIG -= qt

symbian : LIBS += -lefsrv -lplatformenv -lcaf

LIBS+= -ljavautils -ljavacomms

include(../../../build/omj.pri)
