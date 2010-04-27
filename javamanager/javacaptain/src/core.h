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
* Description:  Core
*
*/

#ifndef CORE_H
#define CORE_H

#include <list>
#include <map>
#include <string>

//#include "scopedlocks.h"

#include "commslistener.h"
#include "commsserverendpoint.h"

#include "coreinterface.h"
#include "timerserverinterface.h"
#include "tickerproviderinterface.h"
#include "extensionplugininterface.h"
#include "applicationruntimeeventsinterface.h"
#include "applicationmanagementeventsinterface.h"
#include "processmanagementeventsinterface.h"
#include "eventconsumerinterface.h"

#include "rtc.h"
#include "amc.h"
#include "pmc.h"

namespace java
{
namespace util
{
class Monitor;
}

namespace comms
{
class CommsMessage;
}
namespace captain
{

class ExtensionPluginData;
class TimerData;

class Core : public java::comms::CommsListener,
        public java::comms::CommsServerEndpoint,
        public CoreInterface,
        public TimerServerInterface,
        public TickerProviderEventsInterface,
        public ApplicationRuntimeEventsInterface,
        public ApplicationManagementEventsInterface,
        public ProcessManagementEventsInterface,
        public EventConsumerInterface
{
public:
    Core(java::util::Monitor* aMonitor);
    virtual ~Core();

    virtual bool start();
    virtual int stop();

    // CommsServerEndpoint::CommsEndpoint overrides
    virtual void onStart();
    virtual void onExit();

    // CoreInterface methods
    virtual java::comms::CommsEndpoint* getComms();
    virtual TimerServerInterface* getTimerServer();
    virtual RtcInterface* getRtc();
    virtual PmcInterface* getPmc();

    // Experimental
    virtual EventConsumerInterface* getEventDispatcher();
    virtual AmcInterface* getAmc();

    virtual ExtensionPluginInterface* loadExtensionPlugin(const std::string& aPluginName);
    virtual void unloadExtensionPlugin(const std::string& aPluginName);


    // TimerServerInterface methods
    virtual int timerCreateSeconds(const unsigned int& aTimeoutInSeconds,
                                   TimerServerEventsInterface* aTimerEvents);
    virtual int timerCreateJavaTime(const JavaTime& aJavaTime,
                                    TimerServerEventsInterface* aTimerEvents);
    virtual void timerCancel(const int& aTimerId);

    virtual JavaTime& getCurrentJavaTime(JavaTime&) const;
    virtual bool hasExpired(const JavaTime&, const long long& aAccuracyInMs = 100) const;
    virtual bool isLess(const JavaTime& a, const JavaTime& b) const;
    virtual bool isEqual(const JavaTime& a, const JavaTime& b) const;
    virtual bool isMore(const JavaTime& a, const JavaTime& b) const;

    // TickerProviderEventsInterface methods
    virtual void tick();
    void triggerTicker(); // helper

    // ApplicationRuntimeEventsInterface methods
    virtual void arLaunched(const Uid& aUID, const int& aRuntimeCommsAddress);
    virtual void arTerminated(const Uid& aUID, const int& aExitCode);

    // ApplicationManagementEventsInterface methods
    virtual void amAdded(const uids_t& aUIDs);
    virtual void amUpdated(const uids_t& aUIDs);
    virtual void amDeleted(const uids_t& aUIDs);

    // ProcessManagementEventsInterface methods
    virtual void pmcTerminated(const int& pid, const int& exitCode);

    // EventConsumerInterface methods
    virtual void event(const std::string& eventProvider,
                       java::comms::CommsMessage& aMsg);

protected:
    TickerProviderInterface*            mTicker;

private:
    // CommsListener methods
    virtual void processMessage(java::comms::CommsMessage& message);

private:

    void loadConfigExtensionPlugin();
    void unloadExtensionPlugins();

    Rtc                                 mRtc;
    Amc                                 mAmc;
    Pmc                                 mPmc;

    java::util::Monitor*                mMonitor;

    typedef std::list<ExtensionPluginData*> extensionPlugins_t;
    typedef extensionPlugins_t::iterator extensionPluginsIterator;
    extensionPlugins_t                  mExtensionPlugins;
//    java::util::ScopedMutex             mExtensionPluginsMutex;

    typedef std::list<TimerData*> timers_t;
    typedef timers_t::iterator timersIterator;
    timers_t                            mTimers;
//    java::util::ScopedMutex             mTimersMutex;
};

} // namespace captain
} // namespace java

#endif // CORE_H

