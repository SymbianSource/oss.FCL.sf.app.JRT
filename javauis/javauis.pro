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
# Description: Generated file - do not edit manually
#
TEMPLATE = subdirs
SUBDIRS += eswt_qt/build/eswtqt.pro 
SUBDIRS += lcdui_qt/build/openlcdui.pro 
SUBDIRS += runtimeui_qt/build/javaruntimeui.pro 
SUBDIRS += nokiauiapi_qt/build/nokiauiapiqt.pro 
SUBDIRS += mmapi_qt/build/javamobilemedia.pro 
SUBDIRS += javalegacyutils 
BLD_INF_RULES.prj_extensions += "$${LITERAL_HASH}include \"coreui/build/bld.inf\""  
BLD_INF_RULES.prj_extensions += "prj_extensions" 
