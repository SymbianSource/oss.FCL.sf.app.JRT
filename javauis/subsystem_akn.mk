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
# Description: Makefile for Avkon based components and subsystems
#

COMPONENTS += \
	amms_akn/build \
	coreui_akn/build \
	eswt_akn/eswtdirectcontent/build \
	eswt_akn/org.eclipse.ercp.swt.s60/native/build \
	m2g_akn/build \
	m3g_akn/build \
	mmapi_akn/build \
	remconobserver_akn/build \
	runtimeui_akn/build \
	softnotification_akn/build

NONQTSUBSYSTEMS += \
	eswt_akn/build \
	eswt_akn/eswtapifacade/group \
	eswt_akn/eswtphysics/group

SUBSYSTEMS += \
	lcdui_akn

ifdef RD_JAVA_S60_RELEASE_5_0_IAD
    SUBSYSTEMS += ptivariation_akn
endif


#
# Additional dependencies between the components
#
eswt_akn/org.eclipse.ercp.swt.s60/native/build : javalegacyutils remconobserver_akn/build eswt_akn/eswtapifacade/group eswt_akn/eswtphysics/group
lcdui_akn : eswt_akn/org.eclipse.ercp.swt.s60/native/build
eswt_akn/eswtdirectcontent/build : mmapi_akn/build javalegacyutils remconobserver_akn/build eswt_akn/org.eclipse.ercp.swt.s60/native/build
mmapi_akn/build : lcdui_akn
amms_akn/build : mmapi_akn/build
m3g_akn/build javam2g/build mmapi_akn/build : lcdui_akn
m2g_akn/build mmapi_akn/build : eswt_akn/org.eclipse.ercp.swt.s60/native/build
