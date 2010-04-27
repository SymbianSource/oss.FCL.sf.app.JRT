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

CONFIGUREJAVA:
	make -C $(TO_BLDINF)/../.. -f subsystem.mk configure

FREEZE :
LIB :
MAKMAKE : CONFIGUREJAVA
RESOURCE :
BLD :
SAVESPACE :
FINAL :
CLEAN :
RELEASABLES :

