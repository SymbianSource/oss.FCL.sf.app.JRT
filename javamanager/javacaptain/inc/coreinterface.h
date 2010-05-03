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
* Description:  CoreInterface
*
*/

#ifndef COREINTERFACE_H
#define COREINTERFACE_H

#include <string>

namespace java
{

namespace comms
{
class CommsEndpoint;
}

namespace captain
{

class TimerServerInterface;
class RtcInterface;
class PmcInterface;
class AmcInterface;
class ExtensionPluginInterface;
class EventProviderInterface;
class EventConsumerInterface;

class CoreInterface
{
public:
    virtual ~CoreInterface() {}

    virtual java::comms::CommsEndpoint* getComms()          = 0;
    virtual TimerServerInterface* getTimerServer()          = 0;
    virtual RtcInterface* getRtc()                          = 0;
    virtual PmcInterface* getPmc()                          = 0;
    virtual EventConsumerInterface* getEventDispatcher()    = 0;

    // Experimental interfaces, do not use for production code yet
    virtual AmcInterface* getAmc()                          = 0;

    virtual ExtensionPluginInterface* loadExtensionPlugin(const std::string& aExtensionPluginName) = 0;
    virtual void unloadExtensionPlugin(const std::string& aExtensionPluginName) = 0;
};

} // namespace captain
} // namespace java

#endif // COREINTERFACE_H
