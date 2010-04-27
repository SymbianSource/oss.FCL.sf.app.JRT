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
* Description:  Rtc
*
*/

#ifndef RTC_H
#define RTC_H

#include <pthread.h>
#include <map>
#include <set>

#include "logger.h"
#include "scopedlocks.h"
#include "javauid.h"
#include "dynamiclibloader.h"

#include "rtcinterface.h"
#include "rtcplugininterface.h"
#include "rtcruntimeinterface.h"

#include "processmanagementeventsinterface.h"
#include "commslistener.h"
#include "eventconsumerinterface.h"
#include "applicationmanagementeventsinterface.h"

using namespace java::comms;
using java::util::Uid;
using java::util::DynamicLibLoader;

namespace java
{

namespace captain
{

class CoreInterface;
class ApplicationRuntimeEventsInterface;

class Rtc : public RtcInterface,
        public ProcessManagementEventsInterface,
        public CommsListener,
        public EventConsumerInterface,
        public ApplicationManagementEventsInterface
{
public:
    Rtc();
    virtual ~Rtc();

    bool start(CoreInterface* aCore,
               ApplicationRuntimeEventsInterface* aRuntimeEventsDispatcher);
    bool stop();

    // RtcInterface methods
    virtual bool launch(const rtcLaunchInfo& aLaunchInfo);
    virtual bool terminate(const rtcTerminateInfo& aTerminateInfo);

    virtual bool enable(const Uid& aUID);
    virtual bool disable(const Uid& aUID);

    virtual void setGlobalRuntimeArguments(const std::wstring& aArguments);
    virtual std::wstring getGlobalRuntimeArguments();

    virtual void setGlobalApplicationArguments(const std::wstring& aArguments);
    virtual std::wstring getGlobalApplicationArguments();

    // ProcessManagementEventsInterface methods
    virtual void pmcTerminated(const int& pid, const int& exitCode);

    // CommsListener methods
    virtual void processMessage(CommsMessage& aMessage);

    // EventConsumerInterface (needed for listening plugin events)
    virtual void event(const std::string& eventProvider,
                       java::comms::CommsMessage& aMsg);

    // ApplicationManagementEventsInterface methods
    virtual void amAdded(const uids_t& aUids);
    virtual void amUpdated(const uids_t& aUids);
    virtual void amDeleted(const uids_t& aUids);

    // Other public interface methods
    void terminateRtcRuntimes();
    bool routeMessage(CommsMessage& aMessage, const int& aRuntimeAddress);
    void routeMessageToAll(CommsMessage& aMessage);

    void launchPrewarm();
    void stopPrewarm();

private:
    // Internal versions supporting CommsMessage based extra indiation options
    bool launch(const rtcLaunchInfo& aLaunchInfo, const int& aOptions, CommsMessage& aRequester);
    bool terminate(const rtcTerminateInfo& aTerminateInfo, const int& aOptions, CommsMessage& aRequester);
    void sendOperationFailed(CommsMessage& aRequester, const Uid& aUid, const int& aOptions);

    void initialiseRuntimePlugins();
    void clearRuntimePlugins();

    RtcRuntimeInterface* getRtcRuntime(const Uid& aUID);
    RtcRuntimeInterface* getOrCreateRtcRuntime(const rtcLaunchInfo& aLaunchInfo);
    RtcRuntimeInterface* getPrewarmedRtcRuntime(const rtcLaunchInfo& aLaunchInfo);

    void deleteRuntime(const Uid& aUid);

    void clearDisabledList();

    void notifyLaunchers(const Uid& aUid, const int& aStatus, const int& aPid);
    void notifyTerminators(const Uid& aUid, const int& aStatus);

    bool isThereInstalledMidlets() const;

    CoreInterface*      mCore;

    class rtcPluginData
    {
    public:
        // Statically linked plugins
        rtcPluginData(RtcPluginInterface* aInterface)
                :mInterface(aInterface)
        {
        }
        // Dynamically linked plugins
        rtcPluginData(RtcPluginInterface* aInterface,
                      std::auto_ptr<DynamicLibLoader> aDllLoader)
                :mInterface(aInterface)
        {
            mDllLoader = aDllLoader;
        }

        virtual ~rtcPluginData()
        {
            delete mInterface;
        }

        RtcPluginInterface*             mInterface;

    private:
        std::auto_ptr<DynamicLibLoader> mDllLoader;

        rtcPluginData();
        rtcPluginData(const rtcPluginData&);
        rtcPluginData &operator=(const rtcPluginData&);
    };

    typedef std::map<std::string, rtcPluginData*> plugins_t;
    plugins_t           mPlugins;

    typedef std::map<Uid, RtcRuntimeInterface*> runtimes_t;
    runtimes_t          mRuntimes;

    typedef std::set<Uid> disabledUIDs_t;
    disabledUIDs_t      mDisabledUIDs;

    typedef std::multimap<Uid, CommsMessage> requesters_t;
    requesters_t        mLaunchers;
    requesters_t        mTerminators;

    std::wstring        mGlobalRuntimeArguments;
    std::wstring        mGlobalApplicationArguments;

    ApplicationRuntimeEventsInterface* mRuntimeEventsDispatcher;

    bool                mSupportPreWarming;
};

} // namespace captain
} // namespace java

#endif // RTC_H

