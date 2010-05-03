/*
* Copyright (c) 2008 Nokia Corporation and/or its subsidiary(-ies).
* All rights reserved.
* This component and the accompanying materials are made available
* under the terms of "Eclipse Public License v1.0"
* which accompanies this distribution, and is available
* at the URL "http://www.eclipse.org/legal/epl-v10.html".
*
* Initial Contributors:
* Nokia Corporation - initial contribution.
*
* Contributors:
*
* Description:  core messages
*
*/

#ifndef COREMESSAGES_H
#define COREMESSAGES_H

#include "comms.h"

using namespace java::comms;

namespace java
{
namespace captain
{
// Message IDs of all core services
const int CORE_MSG_ID_STOP_JAVACAPTAIN  = JAVACAPTAIN_MESSAGE_ID_RANGE_START_C + 30; // 130
const int CORE_MSG_ID_DO_THREAD_DUMP    = JAVACAPTAIN_MESSAGE_ID_RANGE_START_C + 31; // 131
const int CORE_MSG_ID_START_PREWARM     = JAVACAPTAIN_MESSAGE_ID_RANGE_START_C + 32; // 133
const int CORE_MSG_ID_STOP_PREWARM      = JAVACAPTAIN_MESSAGE_ID_RANGE_START_C + 33; // 133

} // namespace captain
} // namespace java

#endif // COREMESSAGES_H

