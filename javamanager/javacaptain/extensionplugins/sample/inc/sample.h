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
* Description:  Sample
*
*/

#ifndef SAMPLE_H
#define SAMPLE_H

#include "javaosheaders.h"

#include "commslistener.h"

#include "extensionplugininterface.h"
#include "eventconsumerinterface.h"
#include "applicationmanagementeventsinterface.h"
#include "applicationruntimeeventsinterface.h"

using namespace java::comms;

namespace java
{
namespace captain
{

class CoreInterface;

OS_NONSHARABLE_CLASS(Sample) :  public ExtensionPluginInterface,
        public EventConsumerInterface,
        public ApplicationManagementEventsInterface,
        public ApplicationRuntimeEventsInterface,
        public CommsListener
{
public:
    Sample();
    virtual ~Sample();

    // PluginInterface
    virtual void startPlugin(CoreInterface* aCore);
    virtual void stopPlugin();

    // ExtensionPluginInterface methods
    virtual EventConsumerInterface* getEventConsumer();
    virtual ApplicationManagementEventsInterface* getApplicationManagementListener();
    virtual ApplicationRuntimeEventsInterface*    getApplicationRuntimeListener();
    virtual java::comms::CommsListener* getCommsListener();

    // EventConsumerInterface
    virtual void event(const std::string& eventProvider,
                       java::comms::CommsMessage& aMsg);

    // ApplicationManagementEventsInterface
    virtual void amAdded(const uids_t& uids);
    virtual void amUpdated(const uids_t& uids);
    virtual void amDeleted(const uids_t& uids);

    // ApplicationRuntimeEventsInterface
    virtual void arLaunched(const Uid& aUID, const int& aRuntimeCommsAddress);
    virtual void arTerminated(const Uid& aUID, const int& aExitCode);

protected:
    // CommsListener methods
    virtual void processMessage(CommsMessage& message);

private:
    CoreInterface* mCore;
};

} // namespace captain
} // namespace java

#endif // SAMPLE_H

