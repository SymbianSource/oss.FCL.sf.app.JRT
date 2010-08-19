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
SUBDIRS += javalegacyutils 
SUBDIRS += remconobserver_akn/build/javaremconobserver.pro 
SUBDIRS += eswt_akn/org.eclipse.ercp.swt.s60/native/build/eswt.pro 
SUBDIRS += lcdui_akn 
SUBDIRS += mmapi_akn/build/javamobilemedia.pro 
SUBDIRS += amms_akn/build/javaamms.pro 
SUBDIRS += coreui_akn/build/javacoreui.pro 
SUBDIRS += eswt_akn/eswtdirectcontent/build/eswtdirectcontent.pro 
SUBDIRS += m2g_akn/build/javam2g.pro 
SUBDIRS += m3g_akn/build/javam3g.pro 
SUBDIRS += runtimeui_akn/build/javaruntimeui.pro 
SUBDIRS += softnotification_akn/build/javasoftnotification.pro 
BLD_INF_RULES.prj_extensions += "$${LITERAL_HASH}include \"coreui/build/bld.inf\""  
BLD_INF_RULES.prj_extensions += "$${LITERAL_HASH}include \"eswt_akn/build/bld.inf\""  
BLD_INF_RULES.prj_extensions += "$${LITERAL_HASH}include \"eswt_akn/eswtapifacade/group/bld.inf\""  
BLD_INF_RULES.prj_extensions += "$${LITERAL_HASH}include \"eswt_akn/eswtphysics/group/bld.inf\""  
BLD_INF_RULES.prj_extensions += "prj_extensions" 
