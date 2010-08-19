/*
* Copyright (c) 2010 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  Definition of comms messages used for reporting chages in system
*                    settings.
*
*/

#ifndef SETTINGSCHANGEEVENTSPROVIDERMESSAGES_H
#define SETTINGSCHANGEEVENTSPROVIDERMESSAGES_H

#include "commsmessage.h"
#include "logger.h"

namespace java
{
namespace captain
{

using namespace java::util;
using namespace java::comms;

const int SETTINGS_CHANGE_EVENT_MESSAGE_ID_C = 1;

const char* const SETTINGS_CHANGE_EVENT_PROVIDER = "settingslistener";

typedef enum
{
    UNDEFINED_CHANGE  = 0x00,
    MIDP_CLASS_PATH_CHANGE = 0x01,

} SettingsChangeEventType_t;


inline void setSettingsChangeEventMessageParams(CommsMessage& aMessage,
        const int& aChangeEventType)
{
    aMessage.setMessageId(SETTINGS_CHANGE_EVENT_MESSAGE_ID_C);
    aMessage << aChangeEventType;
}

inline void getSettingsChangeEventMessageParams(CommsMessage& aMessage,
        int& aChangeEventType)
{
    if (aMessage.getMessageId() == SETTINGS_CHANGE_EVENT_MESSAGE_ID_C)
    {
        aMessage >> aChangeEventType;
    }
    else
    {
        ELOG2(EJavaCaptain, "Received Settings Change Event with  wrong MessageId!: %d should be %d",
              aMessage.getMessageId(), SETTINGS_CHANGE_EVENT_MESSAGE_ID_C);
        aChangeEventType = UNDEFINED_CHANGE;
    }
}

} // namespace captain
} // namespace java

#endif // SETTINGSCHANGEEVENTSPROVIDERMESSAGES_H
