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
SUBDIRS += tckrunner/starter/build/tckrunner.pro 
SUBDIRS += tckrunner/runner/build/tckrunner.pro 
SUBDIRS += javacontrolpanel/starter/build/javacontrolpanel.pro 
SUBDIRS += javacontrolpanel/controlpanel/build/javacontrolpanel.pro 
SUBDIRS += javasecuritycustomization/build/javasecuritycustomization.pro 
SUBDIRS += javafinishjrtinstall/build/javafinishjrtinstall.pro 
BLD_INF_RULES.prj_extensions += "$${LITERAL_HASH}include \"testcerts/bld.inf\""  
BLD_INF_RULES.prj_extensions += "prj_extensions" 
