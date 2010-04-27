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
* Description:  ExtensionPluginInterface
*
*/

#ifndef EXTENSIONPLUGININTERFACE_H
#define EXTENSIONPLUGININTERFACE_H

namespace java
{

namespace comms
{
class CommsListener;
} // namespace comms

namespace captain
{

class CoreInterface;
class EventConsumerInterface;
class ApplicationManagementEventsInterface;
class ApplicationRuntimeEventsInterface;
class ProcessManagementEventsInterface;

class ExtensionPluginInterface
{
public:
    virtual ~ExtensionPluginInterface() {};

    virtual void startPlugin(CoreInterface* /* aCore*/) {}

    virtual EventConsumerInterface*                 getEventConsumer()
    {
        return (EventConsumerInterface*)0;
    }
    virtual ApplicationManagementEventsInterface*   getApplicationManagementListener()
    {
        return (ApplicationManagementEventsInterface*)0;
    }
    virtual ApplicationRuntimeEventsInterface*      getApplicationRuntimeListener()
    {
        return (ApplicationRuntimeEventsInterface*)0;
    }
    virtual ProcessManagementEventsInterface*       getProcessManagementListener()
    {
        return (ProcessManagementEventsInterface*)0;
    }
    virtual java::comms::CommsListener*             getCommsListener()
    {
        return (java::comms::CommsListener*)0;
    }

    virtual void stopPlugin() {}
};

} // namespace captain
} // namespace java

#endif // EXTENSIONPLUGININTERFACE_H

