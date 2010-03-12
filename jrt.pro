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
SUBDIRS += javacommons 
SUBDIRS += javaruntimes 
SUBDIRS += javauis 
SUBDIRS += javamanager 
SUBDIRS += javatools 
SUBDIRS += javaextensions 
BLD_INF_RULES.prj_extensions += "$${LITERAL_HASH}include \"jrt_plat/group/bld.inf\""  
BLD_INF_RULES.prj_extensions += "$${LITERAL_HASH}include \"build/bld.inf\""  
BLD_INF_RULES.prj_extensions += "$${LITERAL_HASH}include \"build/stubsis/bld.inf\""  
BLD_INF_RULES.prj_extensions += "prj_extensions" 
