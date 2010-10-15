#
# Copyright (c) 2009, 2010 Nokia Corporation and/or its subsidiary(-ies).
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
# Description: Makefile for Qt based components and subsystems
#

SUBSYSTEMS   += eswt_qt/build nokiauiapi_qt/build
COMPONENTS   += coreui_qt/build lcdui_qt/build runtimeui_qt/build mmapi_qt/build amms_qt/build m3g_qt/build m2g_qt/build

SYMBIAN_ONLY += coreui_qt/build mmapi_qt/build amms_qt/build m3g_qt/build m2g_qt/build

# Build order dependency
coreui_qt/build lcdui_qt/build runtimeui_qt/build nokiauiapi_qt/build mmapi_qt/build amms_qt/build m3g_qt/build m2g_qt/build: eswt_qt/build
