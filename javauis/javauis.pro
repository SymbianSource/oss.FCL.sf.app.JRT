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
SUBDIRS += nokiasound/build/javanokiasound.pro 
SUBDIRS += eswt_qt/build 
SUBDIRS += coreui_qt/build/javacoreui.pro 
SUBDIRS += lcdui_qt/build/openlcdui.pro 
SUBDIRS += runtimeui_qt/build/javaruntimeui.pro 
SUBDIRS += mmapi_qt/build/javamobilemedia.pro 
SUBDIRS += amms_qt/build/javaamms.pro 
SUBDIRS += m3g_qt/build/javam3g.pro 
SUBDIRS += nokiauiapi_qt/build 
BLD_INF_RULES.prj_extensions += "$${LITERAL_HASH}include \"coreui/build/bld.inf\""  
BLD_INF_RULES.prj_extensions += "prj_extensions" 
