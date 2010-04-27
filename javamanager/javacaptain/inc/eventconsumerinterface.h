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
* Description:  EventConsumerInterface
*
*/

#ifndef EVENTCONSUMERINTERFACE_H
#define EVENTCONSUMERINTERFACE_H

#include <string>

namespace java
{
namespace comms
{
class CommsMessage;
}

namespace captain
{

class EventConsumerInterface
{
public:
    virtual ~EventConsumerInterface() {};

    /**
     * Fired when an event happens
     *
     * @param aEvent contains event information
     *
     */
    virtual void event(const std::string& eventProvider,
                       java::comms::CommsMessage& aMsg) = 0;
};

} // namespace captain
} // namespace java

#endif // EVENTCONSUMERINTERFACE_H
