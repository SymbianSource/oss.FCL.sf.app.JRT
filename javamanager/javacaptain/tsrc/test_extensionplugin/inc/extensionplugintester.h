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
* Description:  ExtensionPluginTester
*
*/

#ifndef EXTENSIONPLUGINTESTER_H
#define EXTENSIONPLUGINTESTER_H

#include "javaosheaders.h"

#include <map>

#include "commslistener.h"

#include "timerserverinterface.h"
#include "eventconsumerinterface.h"
#include "extensionplugininterface.h"
#include "applicationruntimeeventsinterface.h"
#include "applicationmanagementeventsinterface.h"

namespace java
{
namespace comms
{
class CommsMessage;
}

namespace captain
{

class CoreInterface;

OS_NONSHARABLE_CLASS(ExtensionPluginTester) : public ExtensionPluginInterface,
        public TimerServerEventsInterface,
        public EventConsumerInterface,
        public ApplicationManagementEventsInterface,
        public ApplicationRuntimeEventsInterface,
        public java::comms::CommsListener
{
public:
    ExtensionPluginTester();
    virtual ~ExtensionPluginTester();

    // PluginInterface
    virtual void startPlugin(CoreInterface* aCore);
    virtual void stopPlugin();

    // ExtensionPluginInterface methods
    virtual EventConsumerInterface* getEventConsumer();
    virtual ApplicationManagementEventsInterface* getApplicationManagementListener();
    virtual ApplicationRuntimeEventsInterface*    getApplicationRuntimeListener();
    virtual java::comms::CommsListener* getCommsListener();

    // TimerServerEventsInterface
    virtual void timerTimeout(const int& aTimerId);

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

    // CommsListener
    virtual void processMessage(CommsMessage& message);

private:
    CoreInterface*  mCore;
    bool            mPush1Installed;
//    int             mTimeoutRequester;
    typedef std::map<const int, int>  timeoutRequesters;
    timeoutRequesters mTimeoutRequesters;
    int mEventTimerId;
};

} // namespace captain
} // namespace java

#endif // EXTENSIONPLUGINTESTER_H

