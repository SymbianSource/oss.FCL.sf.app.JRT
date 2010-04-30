#
# Copyright (c) 2010 Nokia Corporation and/or its subsidiary(-ies).
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

TEMPLATE = subdirs

# Export J9 only when available here (not available here in SF builds)
# (use indirection through ./exports.inf for pregenerated bld.inf variation)
exists($${_PRO_FILE_PWD_}/s60/exports.inf): {
  BLD_INF_RULES.prj_extensions += "$${LITERAL_HASH}include \"exports.inf\"" \
}
