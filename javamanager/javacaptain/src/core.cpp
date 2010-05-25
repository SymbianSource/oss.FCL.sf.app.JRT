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

#include <string>
#include <iostream>
#include <algorithm>

#ifdef __SYMBIAN32__
#include "e32std.h"
#else
#include <sys/types.h>
#include <sys/wait.h>
#include "signalhandlermessages.h"
#endif /* __SYMBIAN32__ */

#include "logger.h"
#include "comms.h"
#include "commsmessage.h"
#include "libraryloaderexception.h"
#include "javacommonutils.h"
#include "monitor.h"

#include "core.h"
#include "coremessages.h"
#include "extensionplugininterface.h"
#include "extensionplugindata.h"

#include "booteventprovider.h"
#include "mmceventprovider.h"

#include "timerdata.h"
#include "tickerprovider.h"

using namespace java::comms;

namespace java
{
namespace captain
{

//using java::util::ScopedLock;

Core::Core(java::util::Monitor* aMonitor)
        :mTicker(NULL),
        mMonitor(aMonitor)
{
    JELOG4(EJavaCaptain, EInfoHeavyLoad);
}

Core::~Core()
{
    JELOG4(EJavaCaptain, EInfoHeavyLoad);
    mMonitor = 0;
    mTicker = 0;
}

bool Core::start()
{
    JELOG4(EJavaCaptain, EInfoHeavyLoad);
    registerDefaultListener(this);
    CommsServerEndpoint::start(IPC_ADDRESS_JAVA_CAPTAIN_C);

    return true;
}

int Core::stop()
{
    JELOG4(EJavaCaptain, EInfoHeavyLoad);
    unregisterDefaultListener(this);
    return CommsServerEndpoint::stop();
}

void Core::onStart()
{
    JELOG4(EJavaCaptain, EInfoHeavyLoad);
    mTicker = new TickerProvider(this);
    mPmc.start(this, this);
    mAmc.start(this, this);
    mRtc.start(this, this);
    loadConfigExtensionPlugin();
}

void Core::onExit()
{
    JELOG4(EJavaCaptain, EInfoHeavyLoad);
    unloadExtensionPlugins();
    mRtc.stop();
    mAmc.stop();
    mPmc.stop();
    delete mTicker;
    for (timersIterator iter = mTimers.begin() ; iter != mTimers.end() ; ++iter)
    {
        delete(*iter);
    }
    mTimers.clear();

    mMonitor->notify();
}

RtcInterface* Core::getRtc()
{
    JELOG4(EJavaCaptain, EInfoHeavyLoad);
    return &mRtc;
}

PmcInterface* Core::getPmc()
{
    JELOG4(EJavaCaptain, EInfoHeavyLoad);
    return &mPmc;
}

AmcInterface* Core::getAmc()
{
    JELOG4(EJavaCaptain, EInfoHeavyLoad);
    return &mAmc;
}

TimerServerInterface* Core::getTimerServer()
{
    JELOG4(EJavaCaptain, EInfoHeavyLoad);
    return this;
}

CommsEndpoint* Core::getComms()
{
    JELOG4(EJavaCaptain, EInfoHeavyLoad);
    return this;
}

ExtensionPluginInterface* Core::loadExtensionPlugin(const std::string& aPluginName)
{
    JELOG4(EJavaCaptain, EInfoHeavyLoad);
    LOG1(EJavaCaptain, EInfo, "Core::loadExtensionPlugin(%s)", aPluginName.c_str());

    extensionPluginsIterator iter = mExtensionPlugins.begin();
    for (; iter != mExtensionPlugins.end() ; ++iter)
    {
        if ((*iter)->getName() == aPluginName)
        {
            break;
        }
    }

    if (iter != mExtensionPlugins.end())
    {
        (*iter)->addLoadCount();
        return (*iter)->mExtensionPlugin;
    }

    ExtensionPluginInterface* interface = NULL;

    if (aPluginName == "boot")
    {
        interface = new BootEventProvider();
        mExtensionPlugins.push_back(new ExtensionPluginData(interface, aPluginName));
        // needs to be pushed in to the collection before starting due to dependencies
        interface->startPlugin(this);
    }
    else if (aPluginName == "mmc")
    {
        interface = new MmcEventProvider();
        mExtensionPlugins.push_back(new ExtensionPluginData(interface, aPluginName));
        // needs to be pushed in to the collection before starting due to dependencies
        interface->startPlugin(this);
    }
    else
    {
        try
        {
            std::string libraryName = "javacaptain_ext_" +  aPluginName;

            std::auto_ptr<java::util::DynamicLibLoader>
            loader(new java::util::DynamicLibLoader(libraryName.c_str()));

            java::captain::ExtensionPluginInterface*(*GetExtensionPluginFunc)() =
                (java::captain::ExtensionPluginInterface*(*)())
                loader->getFunction("getExtensionPlugin");

            if (GetExtensionPluginFunc)
            {
                interface = GetExtensionPluginFunc();
                mExtensionPlugins.push_back(new ExtensionPluginDataDynamic(interface, loader, aPluginName));
                // needs to be pushed in to the collection before starting due to dependencies
                interface->startPlugin(this);
            }
        }
        catch (java::util::LibraryLoaderException& ex)
        {
            ELOG2(EJavaCaptain, "Core::loadExtensionPlugin(%s) failed due: %s",
                  aPluginName.c_str(), ex.toString().c_str());
        }
    }

    return interface;
}

void Core::unloadExtensionPlugin(const std::string& aPluginName)
{
    JELOG4(EJavaCaptain, EInfoHeavyLoad);
    LOG1(EJavaCaptain, EInfo, "Core::unloadExtensionPlugin(%s)", aPluginName.c_str());

    extensionPluginsIterator iter = mExtensionPlugins.begin();
    for (; iter != mExtensionPlugins.end() ; ++iter)
    {
        if ((*iter)->getName() == aPluginName)
        {
            break;
        }
    }

    if (iter != mExtensionPlugins.end())
    {
        if (0 == (*iter)->decLoadCount())
        {
            // Stopped via destructor while still in the collection
            delete(*iter);
            mExtensionPlugins.erase(iter);
        }
    }
}

EventConsumerInterface* Core::getEventDispatcher()
{
    JELOG4(EJavaCaptain, EInfoHeavyLoad);
    return this;
}

// TimerServerInterface methods
int Core::timerCreateSeconds(const unsigned int& aTimeoutInSeconds,
                             TimerServerEventsInterface* aTimerEvents)
{
    JELOG4(EJavaCaptain, EInfoHeavyLoad);

    int timerId = timerCreateJavaTime(JavaTime(mTicker->getCurrentJavaTime() +
                                      aTimeoutInSeconds * 1000LL),
                                      aTimerEvents);
    LOG2(EJavaCaptain, EInfoHeavyLoad, "timerCreateSeconds(%d) -> timerId %d",
         aTimeoutInSeconds, timerId);
    return timerId;
}

bool compareTimerDatas(TimerData*& a, TimerData*& b)
{
    return a->getTimeout().getTime() < b->getTimeout().getTime();
}

int Core::timerCreateJavaTime(const JavaTime& aJavaTime,
                              TimerServerEventsInterface* aTimerEvents)
{
    JELOG4(EJavaCaptain, EInfoHeavyLoad);

    TimerData* td = new TimerData(aJavaTime, aTimerEvents);
    mTimers.push_back(td);
    mTimers.sort(compareTimerDatas);

    triggerTicker();

    LOG1(EJavaCaptain, EInfo, "timerCreateJavaTime(%#x)", td);

    return (int)td;
}

void Core::timerCancel(const int& aTimerId)
{
    JELOG4(EJavaCaptain, EInfoHeavyLoad);
    LOG1(EJavaCaptain, EInfo, "timerCancel(%#x)", aTimerId);

    for (timersIterator iter = mTimers.begin() ;
            iter != mTimers.end() ; ++iter)
    {
        if ((int)(*iter) == aTimerId)
        {
            delete(*iter);
            mTimers.erase(iter);
            break;
        }
    }
    triggerTicker();
}

JavaTime& Core::getCurrentJavaTime(JavaTime& aJt) const
{
    aJt.setTime(mTicker->getCurrentJavaTime());
    return aJt;
}

bool Core::hasExpired(const JavaTime& aJt, const long long& aAccuracyInMs) const
{
    JavaTime currentTime;
    // Within aAccuracyTime is considered expired to due OS limitations
    return isMore(getCurrentJavaTime(currentTime) + aAccuracyInMs, aJt);
}

bool Core::isLess(const JavaTime& a, const JavaTime& b) const
{
    return a.getTime() < b.getTime();
}

bool Core::isEqual(const JavaTime& a, const JavaTime& b) const
{
    return a.getTime() == b.getTime();
}

bool Core::isMore(const JavaTime& a, const JavaTime& b) const
{
    return !isLess(a, b);
}

void Core::triggerTicker()
{
    JELOG4(EJavaCaptain, EInfoHeavyLoad);
    if (mTimers.size() > 0)
    {
        long long nextTickAt = mTicker->getNextTickAt();
        if (!nextTickAt || isMore(JavaTime(nextTickAt), (*mTimers.begin())->getTimeout()))
        {
            mTicker->nextTickAt((*mTimers.begin())->getTimeout().getTime());
        }
    }
    else
    {
        mTicker->cancel();
    }
}

// TickerEventsInterface methods
void Core::tick()
{
    JELOG4(EJavaCaptain, EInfoHeavyLoad);

    JavaTime currentTime = JavaTime(mTicker->getCurrentJavaTime());

    for (timersIterator iter = mTimers.begin() ;
            iter != mTimers.end() ; /*iter++*/)
    {
        if (hasExpired((*iter)->getTimeout()))
        {
            (*iter)->timeoutReached();
            delete(*iter);
            iter = mTimers.erase(iter);
        }
        else
        {
            ++iter;
        }
    }
    triggerTicker();
}

// Helper macro, a template would be better but I prefer macros for 'clarity'
#define DISTR_EVENTS(A, B) \
    for(extensionPluginsIterator iter = mExtensionPlugins.begin(); \
        iter != mExtensionPlugins.end(); ++iter) { \
        A##EventsInterface* listener = \
        (*iter)->mExtensionPlugin->get##A##Listener(); \
        if(listener) listener->B; }

// ApplicationRuntimeEventsInterface methods
void Core::arLaunched(const Uid& aUID, const int& aRuntimeCommsAddress)
{
    mAmc.arLaunched(aUID, aRuntimeCommsAddress);
    DISTR_EVENTS(ApplicationRuntime, arLaunched(aUID, aRuntimeCommsAddress))
}
void Core::arTerminated(const Uid& aUID, const int& aExitCode)
{
    mAmc.arTerminated(aUID, aExitCode);
    DISTR_EVENTS(ApplicationRuntime, arTerminated(aUID, aExitCode))
}

// ApplicationManagementEventsInterface methods
void Core::amAdded(const uids_t& aUIDs)
{
    mRtc.amAdded(aUIDs);
    LOG1(EJavaCaptain, EInfo, "amAdded event dispatcher %d uids", aUIDs.size());
    DISTR_EVENTS(ApplicationManagement, amAdded(aUIDs))
}
void Core::amUpdated(const uids_t& aUIDs)
{
    mRtc.amUpdated(aUIDs);
    LOG1(EJavaCaptain, EInfo, "amUpdated event dispatcher %d uids", aUIDs.size());
    DISTR_EVENTS(ApplicationManagement, amUpdated(aUIDs))
}
void Core::amDeleted(const uids_t& aUIDs)
{
    mRtc.amDeleted(aUIDs);
    LOG1(EJavaCaptain, EInfo, "amDeleted event dispatcher %d uids", aUIDs.size());
    DISTR_EVENTS(ApplicationManagement, amDeleted(aUIDs))
}

// ProcessManagementEventsInterface methods
void Core::pmcTerminated(const int& pid, const int& exitCode)
{
    LOG2(EJavaCaptain, EInfo, "pmcTerminated event dispatcher pid=%d, exitCode=%d", pid, exitCode);
    mRtc.pmcTerminated(pid, exitCode);
    DISTR_EVENTS(ProcessManagement, pmcTerminated(pid, exitCode))
}

// EventConsumerInterface methods
void Core::event(const std::string& eventProvider,
                 java::comms::CommsMessage& aMsg)
{
    for (extensionPluginsIterator iter = mExtensionPlugins.begin();
            iter != mExtensionPlugins.end();
            ++iter)
    {
        EventConsumerInterface* consumer = (*iter)->mExtensionPlugin->getEventConsumer();
        if (consumer)
        {
            consumer->event(eventProvider, aMsg);
            aMsg.begin();
        }
    }
    // Rtc needs to listen boot events because of prewarm
    mRtc.event(eventProvider, aMsg);
    aMsg.begin();
}

// Default messsage handler, means that the receiver module was not loaded yet
void Core::processMessage(CommsMessage& aMessage)
{
    JELOG4(EJavaCaptain, EInfoHeavyLoad);

    switch (aMessage.getModuleId())
    {
    case PLUGIN_ID_JAVACAPTAIN_CORE_C:
        switch (aMessage.getMessageId())
        {
        case CORE_MSG_ID_STOP_JAVACAPTAIN:
            if (aMessage.hasPermission(STOP_APPLICATION))
            {
                WLOG(EJavaCaptain, "STOP message received");
                mRtc.terminateRtcRuntimes();
                mMonitor->notify();
            }
            break;
        case CORE_MSG_ID_DO_THREAD_DUMP:
            if (aMessage.hasPermission(LAUNCH_APPLICATION))
            {
                WLOG(EJavaCaptain, "DO_THREAD_DUMP message reveived");
                mRtc.routeMessageToAll(aMessage);
            }
            break;

        case CORE_MSG_ID_START_PREWARM:
            if (aMessage.hasPermission(LAUNCH_APPLICATION))
            {
                WLOG(EJavaCaptain, "START prewarm message received");
                mRtc.launchPrewarm();
            }
            break;

        case CORE_MSG_ID_STOP_PREWARM:
            if (aMessage.hasPermission(STOP_APPLICATION))
            {
                WLOG(EJavaCaptain, "STOP prewarm message received");
                mRtc.stopPrewarm();
            }
            break;

        case CORE_MSG_ID_GET_PREWARM:
            if (aMessage.hasPermission(STOP_APPLICATION))
            {
                int isSupported = (int) mRtc.isPrewarmSupported();
                WLOG1(EJavaCaptain, "GET prewarm message received (%d)", isSupported);
                CommsMessage reply;
                reply.replyTo(aMessage);
                reply << isSupported;
                send(reply);
            }
            break;

        default:
            break;
        }
        break;

#ifndef __SYMBIAN32__
    case PLUGIN_ID_SIGNAL_C:
        LOG(EJavaCaptain, EInfo, "SIGNAL message received");
        switch (aMessage.getMessageId())
        {
        case SIG_MSG_ID_SIGCHLD:
        {
            int pid = 0;
            int status = 0;
            getSignalChildMessage(aMessage, pid, status);
            pmcTerminated(pid, status);
        }
        break;

        case SIG_MSG_ID_SIGALRM:
            tick();
            break;

        case SIG_MSG_ID_SIGINT:
            LOG(EJavaCaptain, EInfo, "STOP (SIGINT)message received");
            mRtc.terminateRtcRuntimes();
            mMonitor->notify();
            break;
        }

        break;
#endif /* __SYMBIAN32__ */

    default:
    {
        std::string extensionPluginName = "ondemand_" + java::util::JavaCommonUtils::intToString(aMessage.getModuleId());
        ExtensionPluginInterface* interface = loadExtensionPlugin(extensionPluginName.c_str());
        if (interface)
        {
            java::comms::CommsListener* commsListener =  interface->getCommsListener();
            if (commsListener)
            {
                commsListener->processMessage(aMessage);
            }
        }
        else
        {
            WLOG1(EJavaCaptain, "JavaCaptain::ProcessMessage, sender    = %d\n", aMessage.getSender());
            WLOG1(EJavaCaptain, "JavaCaptain::ProcessMessage, messageId = %d\n", aMessage.getMessageId());
            WLOG1(EJavaCaptain, "JavaCaptain::ProcessMessage, moduleId  = %d\n", aMessage.getModuleId());
        }
    }
    }
}

void Core::loadConfigExtensionPlugin()
{
    JELOG4(EJavaCaptain, EInfoHeavyLoad);
    loadExtensionPlugin("config");
// Let's notify the Starter that the boot may proceed
#ifdef __SYMBIAN32__
    RProcess::Rendezvous(KErrNone);
#endif
}

void Core::unloadExtensionPlugins()
{
    JELOG4(EJavaCaptain, EInfoHeavyLoad);

    ExtensionPluginData* plugin = NULL;

    do
    {
        plugin = NULL;
        {
            extensionPluginsIterator iter = mExtensionPlugins.begin();
            if (iter != mExtensionPlugins.end())
            {
                plugin = (*iter);
                mExtensionPlugins.erase(iter);
            }
        }
        if (plugin)
        {
            delete plugin;
        }
    }
    while (mExtensionPlugins.size() > 0);
}

} // namespace captain
} // namespace java

