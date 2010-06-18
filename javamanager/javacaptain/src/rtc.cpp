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

#include <string>
#include <memory>
#include <exception>

#include "commsendpoint.h"
#include "commsmessage.h"
#include "logger.h"
#include "javacommonutils.h"
#include "libraryloaderexception.h"
#include "javaoslayer.h"

#include "rtc.h"
#include "coreinterface.h"
#include "rtcmessages.h"
#include "pmcmessages.h"
#include "applicationruntimeeventsinterface.h"
#include "booteventprovidermessages.h"
#include "javastorageentry.h"
#include "javastorage.h"
#include "javastoragenames.h"

#include "midprtcplugin.h"
#ifdef RD_JAVA_CAPTAIN_TESTRUNTIME
#include "testrtcplugin.h"
#endif

namespace java
{
namespace captain
{

Rtc::Rtc() : mCore(0), mRuntimeEventsDispatcher(0), mSupportPreWarming(false)
{
    JELOG4(EJavaCaptain, EInfoHeavyLoad);
#if defined(RD_JAVA_PREWARM) && !defined (__WINS__)
    ILOG(EJavaCaptain, "Rtc::Rtc() - Prewarm support turned on");
    mSupportPreWarming = true;
#endif // RD_JAVA_PREWARM
}

Rtc::~Rtc()
{
    JELOG4(EJavaCaptain, EInfoHeavyLoad);
}

bool Rtc::start(CoreInterface* aCore,
                ApplicationRuntimeEventsInterface* aRuntimeEventsDispatcher)
{
    JELOG4(EJavaCaptain, EInfoHeavyLoad);

    mCore = aCore;
    mCore->getComms()->registerListener(PLUGIN_ID_RTC_C, this);
    mCore->getComms()->registerListener(PLUGIN_ID_PMC_C, this);
    mRuntimeEventsDispatcher = aRuntimeEventsDispatcher;
    initialiseRuntimePlugins();
    return true;
}

bool Rtc::stop()
{
    JELOG4(EJavaCaptain, EInfoHeavyLoad);

    if (mCore)
    {
        mCore->getComms()->unregisterListener(PLUGIN_ID_RTC_C, this);
        mCore->getComms()->unregisterListener(PLUGIN_ID_PMC_C, this);
    }

    terminateRtcRuntimes();
    clearDisabledList();
    clearRuntimePlugins();

    return true;
}

void Rtc::terminateRtcRuntimes()
{
    JELOG4(EJavaCaptain, EInfoHeavyLoad);

    // Delete the objects
    for (runtimes_t::iterator iter = mRuntimes.begin() ;
            iter != mRuntimes.end(); ++iter)
    {
        delete iter->second;
    }
    // Delete the pairs from map
    mRuntimes.clear();
}

void Rtc::stopPrewarm()
{
    JELOG4(EJavaCaptain, EInfoHeavyLoad);

    mSupportPreWarming = false;
    // Try to find prewarmed instance
    runtimes_t::iterator iter = mRuntimes.find(PREWARM_UID);
    if (iter != mRuntimes.end())
    {
        ILOG(EJavaCaptain, "Rtc::stopPrewarm() - prewarm instance found, deleting...");
        delete iter->second;
        mRuntimes.erase(iter);
    }
}

void Rtc::clearDisabledList()
{
    JELOG4(EJavaCaptain, EInfoHeavyLoad);
    mDisabledUIDs.clear();
}

// RtcInterface methods
bool Rtc::launch(const rtcLaunchInfo& aLaunchInfo)
{
    JELOG4(EJavaCaptain, EInfoHeavyLoad);
    if (mSupportPreWarming &&
            aLaunchInfo.mLaunchType == RTC_LAUNCH_TYPE_PREWARM_C &&
            mRuntimes.find(PREWARM_UID) != mRuntimes.end())
    {
        ILOG(EJavaCaptain, "Rtc::launch() - Prewarmed instance found, not creating new");
        return true;
    }
    int options = RTC_LAUNCH_OPTIONS_NONE_C;
    CommsMessage dummy;
    return launch(aLaunchInfo, options, dummy);
}

bool Rtc::terminate(const rtcTerminateInfo& aTerminateInfo)
{
    JELOG4(EJavaCaptain, EInfoHeavyLoad);
    int options = RTC_TERMINATE_OPTIONS_NONE_C;
    CommsMessage dummy;
    return terminate(aTerminateInfo, options, dummy);
}

bool Rtc::launch(const rtcLaunchInfo& aLaunchInfo, const int& aOptions, CommsMessage& aRequester)
{
    JELOG4(EJavaCaptain, EInfoHeavyLoad);

    LOG1WSTR(EJavaCaptain, EInfo, "Rtc::launch(uid=%s)", aLaunchInfo.mUID.toString());
    LOG3(EJavaCaptain, EInfo, "Rtc::launch(runtime=%s, type=%d, options=%d)",
         aLaunchInfo.mRtcType.c_str(), aLaunchInfo.mLaunchType, aOptions);

    bool launchSuccess = false;

    if (aOptions & RTC_LAUNCH_OPTIONS_RUNNING_IND_REQ_C)
    {
        mLaunchers.insert(std::make_pair(aLaunchInfo.mUID, aRequester));
    }
    if (aOptions & RTC_LAUNCH_OPTIONS_TERMINATE_IND_REQ_C)
    {
        mTerminators.insert(std::make_pair(aLaunchInfo.mUID, aRequester));
    }

    if (mDisabledUIDs.find(aLaunchInfo.mUID) != mDisabledUIDs.end())
    {
        notifyLaunchers(aLaunchInfo.mUID, -1, 0);
        notifyTerminators(aLaunchInfo.mUID, -1);
        launchSuccess = false;
    }
    else
    {
        rtcLaunchInfo finalLaunchInfo(aLaunchInfo.mUID,
                                      aLaunchInfo.mLaunchType, aLaunchInfo.mRtcType,
                                      (!aLaunchInfo.mApplicationArguments.empty() ? aLaunchInfo.mApplicationArguments
                                       : getGlobalApplicationArguments()),
                                      (!aLaunchInfo.mRuntimeArguments.empty() ? aLaunchInfo.mRuntimeArguments
                                       : getGlobalRuntimeArguments()));

        RtcRuntimeInterface* runtime = getOrCreateRtcRuntime(finalLaunchInfo);

        if ((0 == runtime) || (false == runtime->launch(finalLaunchInfo)))
        {
            deleteRuntime(finalLaunchInfo.mUID);
            notifyLaunchers(finalLaunchInfo.mUID, -1, 0);
            notifyTerminators(finalLaunchInfo.mUID, -1);
            launchSuccess = false;
        }
        else
        {
            if (runtime->isRunning())
            {
                notifyLaunchers(finalLaunchInfo.mUID, 0, runtime->pid());
            }
            launchSuccess = true;
        }
    }

    return launchSuccess;
}

bool Rtc::terminate(const rtcTerminateInfo& aTerminateInfo, const int& aOptions, CommsMessage& aRequester)
{
    JELOG4(EJavaCaptain, EInfoHeavyLoad);

    LOG1WSTR(EJavaCaptain, EInfo, "Rtc::terminate(uid=%s)", aTerminateInfo.mUID.toString());
    LOG1(EJavaCaptain, EInfo, "Rtc::terminate(options=%d)", aOptions);

    bool terminateSuccess = false;

    if (aOptions & RTC_TERMINATE_OPTIONS_TERMINATE_IND_REQ_C)
    {
        mTerminators.insert(std::make_pair(aTerminateInfo.mUID, aRequester));
    }

    RtcRuntimeInterface* runtime = getRtcRuntime(aTerminateInfo.mUID);

    if (0 == runtime ||
            true == runtime->terminate(aTerminateInfo))
    {
        notifyTerminators(aTerminateInfo.mUID, 0);
        mRuntimeEventsDispatcher->arTerminated(aTerminateInfo.mUID, 0);
        terminateSuccess = true;
    }

    return terminateSuccess;
}

void Rtc::notifyLaunchers(const Uid& aUid, const int& aStatus, const int& aPid)
{
    JELOG4(EJavaCaptain, EInfoHeavyLoad);

    requesters_t::iterator iter = mLaunchers.find(aUid);

    if (iter != mLaunchers.end())
    {
        CommsMessage msg;
        setApplicationRunningIndParams(msg, aUid, aStatus, aPid);
        CommsEndpoint* comms = mCore->getComms();
        while (comms && iter != mLaunchers.end())
        {
            msg.setModuleId(iter->second.getModuleId());
            msg.setReceiver(iter->second.getSender());
            msg.setMessageRef(iter->second.getMessageRef());
            LOG1(EJavaCaptain, EInfo, "informing launcher %d", msg.getReceiver());
            comms->send(msg);
            mLaunchers.erase(iter);
            iter = mLaunchers.find(aUid);
        }
    }
}

void Rtc::notifyTerminators(const Uid& aUid, const int& aStatus)
{
    JELOG4(EJavaCaptain, EInfoHeavyLoad);

    requesters_t::iterator iter = mTerminators.find(aUid);

    if (iter != mTerminators.end())
    {
        CommsMessage msg;
        setApplicationTerminatedIndParams(msg, aUid, aStatus);
        CommsEndpoint* comms = mCore->getComms();
        while (comms && iter != mTerminators.end())
        {
            msg.setModuleId(iter->second.getModuleId());
            msg.setReceiver(iter->second.getSender());
            msg.setMessageRef(iter->second.getMessageRef());
            LOG1(EJavaCaptain, EInfo, "informing terminator %d", msg.getReceiver());
            comms->send(msg);
            mTerminators.erase(iter);
            iter = mTerminators.find(aUid);
        }
    }
}

bool Rtc::enable(const Uid& aUID)
{
    JELOG4(EJavaCaptain, EInfoHeavyLoad);
    LOG1WSTR(EJavaCaptain, EInfo, "enable(%s)", aUID.toString());

    mDisabledUIDs.erase(aUID);

    return true;
}

bool Rtc::disable(const Uid& aUID)
{
    JELOG4(EJavaCaptain, EInfoHeavyLoad);
    LOG1WSTR(EJavaCaptain, EInfo, "disable(%s)", aUID.toString());

    mDisabledUIDs.insert(aUID);

    return true;
}

void Rtc::setGlobalRuntimeArguments(const std::wstring& aArguments)
{
    mGlobalRuntimeArguments = aArguments;
}

std::wstring Rtc::getGlobalRuntimeArguments()
{
    return mGlobalRuntimeArguments;
}

void Rtc::setGlobalApplicationArguments(const std::wstring& aArguments)
{
    mGlobalApplicationArguments = aArguments;
}

std::wstring Rtc::getGlobalApplicationArguments()
{
    return mGlobalApplicationArguments;
}

void Rtc::pmcTerminated(const int& pid, const int& exitCode)
{
    JELOG4(EJavaCaptain, EInfoHeavyLoad);

    for (runtimes_t::iterator iter = mRuntimes.begin() ;
            iter != mRuntimes.end(); ++iter)
    {
        if (iter->second->pid() == pid)
        {
            iter->second->pmcTerminated(pid, exitCode);

            Uid uid = iter->second->uid();
            // iter value might not be valid anymore after the following callbacks!
            mRuntimeEventsDispatcher->arTerminated(uid, exitCode);
            notifyLaunchers(uid, -1, pid);
            notifyTerminators(uid, 0);
            break;
        }
    }
}

void Rtc::processMessage(CommsMessage& aMessage)
{
    JELOG4(EJavaCaptain, EInfoHeavyLoad);

    try
    {
        switch (aMessage.getMessageId())
        {
        case RTC_MSG_ID_LAUNCH_APPLICATION_REQ:
        {
            java::util::JavaOsLayer::startUpTrace("CAPTAIN: Launch req", -1, -1);
            Uid uid;
            int type = 0;
            int options = 0;
            std::string rtc;
            std::wstring applicationArguments, runtimeArguments;
            getLaunchApplicationReqParams(aMessage, uid, type, options, rtc,
                                          applicationArguments, runtimeArguments);

            if (runtimeArguments.length() != 0 &&
                    !aMessage.hasPermission(LAUNCH_APPLICATION))
            {
                WLOG(EJavaCaptain, "LAUNCH_APP failed: not allowed to modify runtime arguments");
                sendOperationFailed(aMessage, uid, options);
            }
            else
            {
                launch(rtcLaunchInfo(uid, type, rtc, applicationArguments, runtimeArguments),
                       options, aMessage);
            }
        }
        break;

        case RTC_MSG_ID_TERMINATE_APPLICATION_REQ:
        {
            Uid uid;
            int options = 0;
            getTerminateApplicationReqParams(aMessage, uid, options);

            if (aMessage.hasPermission(STOP_APPLICATION))
            {
                terminate(uid, options, aMessage);
            }
            else
            {
                WLOG(EJavaCaptain, "STOP_APP failed: not allowed");
                sendOperationFailed(aMessage, uid, options);
            }
        }
        break;

        case RTC_MSG_ID_APPLICATION_RUNNING_IND:
        {
            Uid uid;
            int status = 0;
            getApplicationRunningIndParams(aMessage, uid, status);
            runtimes_t::iterator rtIter = mRuntimes.find(uid);

            if (rtIter != mRuntimes.end())
            {
                const int senderAddress = aMessage.getSender();
                rtIter->second->runningInd(senderAddress, status);
                mRuntimeEventsDispatcher->arLaunched(uid, senderAddress);
                notifyLaunchers(uid, 0, rtIter->second->pid());
            }
        }
        break;

        case RTC_MSG_ID_APPLICATION_TERMINATED_IND:
        {
            Uid uid;
            int status = 0;
            getApplicationTerminatedIndParams(aMessage, uid, status);
            runtimes_t::iterator rtIter = mRuntimes.find(uid);

            if (rtIter != mRuntimes.end())
            {
                rtIter->second->terminatedInd(status);
            }
        }
        break;

        case RTC_MSG_ID_SET_EXTRA_ARGUMENTS:
            if (aMessage.hasPermission(LAUNCH_APPLICATION))
            {
                getSetExtraArguments(aMessage, mGlobalRuntimeArguments,
                                     mGlobalApplicationArguments);
            }
            else
            {
                WLOG(EJavaCaptain, "SET_EXTRA_ARGUMENTS failed: not allowed");
            }
            break;

        case RTC_MSG_ID_GET_EXTRA_ARGUMENTS:
            if (!routeMessage(aMessage, aMessage.getSender()))
            {
                LOG(EJavaCaptain, EWarning,
                    "Rtc::ProcessMessage: Get extra args msg is not from one of the runtimes");

                // handler was not one of the runtimes then reply directly:
                CommsMessage message;
                message.replyTo(aMessage);
                setGetExtraArgumentsAck(message, mGlobalRuntimeArguments, mGlobalApplicationArguments);
                mCore->getComms()->send(message);
            }
            break;

        case PMC_MSG_ID_PROCESS_INITIALISED_IND:
        {
            int pid = 0;
            getProcessInitialisedIndParams(aMessage, pid);

            for (runtimes_t::iterator iter = mRuntimes.begin();
                    iter != mRuntimes.end(); ++iter)
            {
                if (iter->second->pid() == pid)
                {
                    iter->second->runningInd(aMessage.getSender(), 0);
                    break;
                }
            }
        }
        break;

        default:
            LOG1(EJavaCaptain, EInfo, "Rtc::ProcessMessage, unknown messageId = %d",
                 aMessage.getMessageId());
            break;
        }
    }

    catch (std::exception& e)
    {
        LOG(EJavaCaptain, EInfo, "Rtc::ProcessMessage: unhandled exception");
    }
}


bool Rtc::routeMessage(CommsMessage& aMessage, const int& aRuntimeAddress)
{
    JELOG4(EJavaCaptain, EInfoHeavyLoad);

    for (runtimes_t::iterator iter = mRuntimes.begin();
            iter != mRuntimes.end(); ++iter)
    {
        if (iter->second->runtimeAddress() == aRuntimeAddress)
        {
            iter->second->processMessage(aMessage);
            return true; // handler found
        }
    }

    return false; // handler not found
}

void Rtc::event(const std::string& eventProvider,
                java::comms::CommsMessage& /* aMsg */)
{
    ILOG(EJavaCaptain, "+Rtc::event()");
    if (BOOT_EVENT_PROVIDER == eventProvider)
    {
        if (mSupportPreWarming && isThereInstalledMidlets())
        {
            ILOG(EJavaCaptain, "Rtc::event() - Boot event received, Starting prewarm VM since found Installed MIDlets");
            launchPrewarm(); //New prewarm VM instance is will be created only if it did not exist already
        }
    }
}

// ApplicationManagementEventsInterface methods
void Rtc::amAdded(const uids_t& /* aUids */)
{
    ILOG(EJavaCaptain, "+Rtc::amAdded()");
    // we know there is now installed MIDlets
    if (mSupportPreWarming)
    {
        ILOG(EJavaCaptain, "Rtc::amAdded() - MIDlets installed and prewarm is on -> try to start prewarm VM if not already running");
        launchPrewarm(); //New prewarm VM instance is will be created only if it did not exist already
    }
}

void Rtc::amUpdated(const uids_t& /* aUids */)
{
    ILOG(EJavaCaptain, "+Rtc::amUpdated()");
    // we know there is now installed MIDlets
    if (mSupportPreWarming)
    {
        ILOG(EJavaCaptain, "Rtc::amUpdated() - MIDlet updated and prewarm is on -> try to start prewarm VM if not already running ");
        launchPrewarm(); //New prewarm VM instance is will be created only if it did not exist already
    }
}

void Rtc::amDeleted(const uids_t& /* aUids */)
{
    ILOG(EJavaCaptain, "+Rtc::amDeleted()");
    // Check if prewarm can be turned off
    if (mSupportPreWarming && (!isThereInstalledMidlets()))
    {
        ILOG(EJavaCaptain,  "Rtc::amDeleted() - No MIDlets installed -> Stopping prewarm VM...");
        stopPrewarm();
    }
}

void Rtc::routeMessageToAll(CommsMessage& message)
{
    JELOG4(EJavaCaptain, EInfoHeavyLoad);

    for (runtimes_t::iterator iter = mRuntimes.begin();
            iter != mRuntimes.end(); ++iter)
    {
        iter->second->processMessage(message);
    }
}

void Rtc::sendOperationFailed(CommsMessage& aRequester, const Uid& aUid, const int& aOptions)
{
    if (aOptions != 0)
    {
        CommsMessage reply;
        reply.replyTo(aRequester);
        setApplicationTerminatedIndParams(reply, aUid, -1);
        mCore->getComms()->send(reply);
    }
}

void Rtc::initialiseRuntimePlugins()
{
    JELOG4(EJavaCaptain, EInfoHeavyLoad);

    mPlugins.insert(std::make_pair("midp", new rtcPluginData(getMidpRtcPlugin())));

#ifdef RD_JAVA_CAPTAIN_TESTRUNTIME
    mPlugins.insert(std::make_pair("test", new rtcPluginData(getTestRtcPlugin())));
#endif
}

void Rtc::clearRuntimePlugins()
{
    JELOG4(EJavaCaptain, EInfoHeavyLoad);

    // Delete the objects
    for (plugins_t::iterator iter = mPlugins.begin() ;
            iter != mPlugins.end(); ++iter)
    {
        delete iter->second;
    }
    // Delete the pairs from map
    mPlugins.clear();
}

RtcRuntimeInterface* Rtc::getRtcRuntime(const Uid& aUid)
{
    JELOG4(EJavaCaptain, EInfoHeavyLoad);

    RtcRuntimeInterface* runtime = 0;
    runtimes_t::iterator rtIter = mRuntimes.find(aUid);

    if (rtIter != mRuntimes.end())
    {
        runtime = rtIter->second;
    }

    return runtime;
}

RtcRuntimeInterface* Rtc::getOrCreateRtcRuntime(const rtcLaunchInfo& aLaunchInfo)
{
    JELOG4(EJavaCaptain, EInfoHeavyLoad);

    RtcRuntimeInterface* runtime = 0;
    runtimes_t::iterator rtIter = mRuntimes.find(aLaunchInfo.mUID);

    if (rtIter != mRuntimes.end() &&
            -1 == rtIter->second->pid())
    {
        delete rtIter->second;
        mRuntimes.erase(rtIter);
        rtIter = mRuntimes.end();
    }

    if (rtIter == mRuntimes.end())
    {
        runtime = getPrewarmedRtcRuntime(aLaunchInfo);
        if (runtime == 0)
        {
            RtcPluginInterface* rtcInterface = 0;
            plugins_t::iterator pluginIter = mPlugins.find(aLaunchInfo.mRtcType);

            if (pluginIter != mPlugins.end())
            {
                rtcInterface = pluginIter->second->mInterface;
            }
            else
            {
                try
                {
                    std::string pluginName = "javacaptain_rtc_" + aLaunchInfo.mRtcType;

                    std::auto_ptr<java::util::DynamicLibLoader>
                    loader(new java::util::DynamicLibLoader(pluginName.c_str()));

                    java::captain::RtcPluginInterface*(*getRtcPluginFunc)() =
                        (java::captain::RtcPluginInterface*(*)())
                        loader->getFunction("getRtcPlugin");

                    if (getRtcPluginFunc)
                    {
                        rtcInterface = getRtcPluginFunc();
                        if (rtcInterface)
                        {
                            mPlugins.insert(std::make_pair(aLaunchInfo.mRtcType,
                                                           new rtcPluginData(rtcInterface, loader)));
                        }
                    }
                }
                catch (java::util::LibraryLoaderException& ex)
                {
                    LOG2(EJavaCaptain, EError, "Rtc::getRtcPlugin(%s) failed due: %s",
                         aLaunchInfo.mRtcType.c_str(), ex.toString().c_str());
                }
            }

            if (rtcInterface)
            {
                runtime = rtcInterface->createNewRuntimeInstance(aLaunchInfo.mUID, mCore);
                if (runtime)
                {
                    mRuntimes.insert(std::make_pair(aLaunchInfo.mUID, runtime));
                }
            }
        }
    }
    else
    {
        runtime = rtIter->second;
    }

    return runtime;
}

RtcRuntimeInterface* Rtc::getPrewarmedRtcRuntime(const rtcLaunchInfo& aLaunchInfo)
{
    JELOG2(EJavaCaptain);
    RtcRuntimeInterface* runtime = 0;

    if (mSupportPreWarming &&
            aLaunchInfo.mRtcType == "midp" &&
            aLaunchInfo.mLaunchType == RTC_LAUNCH_TYPE_NORMAL_C)
    {
        // Use prewarming only for normal launches and leave out push,
        // debug, auto invocation etc.
        bool createNewPrewarmProcess = false;

        // Then check if there exists prewarmed instance.
        runtimes_t::iterator rtIter = mRuntimes.find(PREWARM_UID);
        if (rtIter != mRuntimes.end())
        {
            if (rtIter->second->isRunning())
            {
                // Prewarmed instance found, using it
                ILOG(EJavaCaptain,  "Rtc::getPrewarmedRtcRuntime() -Prewarmed instance found, launching within");
                runtime = rtIter->second;

                // Removed/add runtime instance with a correct UID to collection
                mRuntimes.erase(rtIter);
                mRuntimes.insert(std::make_pair(aLaunchInfo.mUID, runtime));
                ILOG(EJavaCaptain, "Rtc::getPrewarmedRtcRuntime() - Asking for new prewarm instance - previous was taken into use");
                createNewPrewarmProcess = true;
            }
            else
            {
                if (-1 == rtIter->second->pid())
                {
                    ILOG(EJavaCaptain, "Rtc::getPrewarmedRtcRuntime() - Prewarmed instance found but it has been deleted.");
                    // Remove the previous instance.
                    delete rtIter->second;
                    mRuntimes.erase(rtIter);
                    createNewPrewarmProcess = true;
                }
                else
                {
                    ILOG(EJavaCaptain, "Rtc::getPrewarmedRtcRuntime() - Prewarmed instance found but it is not ready yet");
                }
            }
        }
        else
        {
            ILOG(EJavaCaptain, "Rtc::getPrewarmedRtcRuntime() - New prewarm instance - didn't exist at all!");
            createNewPrewarmProcess = true;
        }
        if (createNewPrewarmProcess)
        {
            // Create new Prewarmed process.
            launchPrewarm();
        }
    }
    return runtime;
}

void Rtc::launchPrewarm()
{
    mSupportPreWarming = true;
    launch(rtcLaunchInfo(PREWARM_UID, RTC_LAUNCH_TYPE_PREWARM_C,
                         RTC_LAUNCH_RUNTIME_MIDP_C));
}

bool Rtc::isPrewarmSupported()
{
    return mSupportPreWarming;
}

void Rtc::deleteRuntime(const Uid& aUid)
{
    JELOG4(EJavaCaptain, EInfoHeavyLoad);

    runtimes_t::iterator rtIter = mRuntimes.find(aUid);
    if (rtIter != mRuntimes.end())
    {
        delete rtIter->second;
        mRuntimes.erase(rtIter);
    }
}

// Checks from Javastorage if there are installed MIDlets
bool Rtc::isThereInstalledMidlets() const
{
    bool result = false;
    using namespace java::storage;

    JavaStorageEntry idAttribute;
    idAttribute.setEntry(ID, L"");

    JavaStorageApplicationEntry_t findSuiteQuery;
    findSuiteQuery.insert(idAttribute);

    JavaStorageApplicationList_t foundMidletSuites;
    std::auto_ptr<JavaStorage> jsAptr(JavaStorage::createInstance());

    try
    {
        ILOG(EJavaCaptain, "Rtc::isThereInstalledMidlets() - searching for installed suites");
        jsAptr->open(JAVA_DATABASE_NAME);
        jsAptr->search(APPLICATION_PACKAGE_TABLE, findSuiteQuery, foundMidletSuites);
    }
    catch (JavaStorageException& jse)
    {
        ELOG1(EJavaCaptain, "Rtc::isThereInstalledMidlets() - Failed to search from JavaStorage: %s ",
              jse.toString().c_str());
    }
    findSuiteQuery.clear();

    if (foundMidletSuites.size() > 0)
    {
        ILOG(EJavaCaptain, "Rtc::isThereInstalledMidlets() - found installed suites");
        result = true;
    }
    foundMidletSuites.clear();

    // Close connection
    try
    {
        jsAptr->close();
    }
    catch (JavaStorageException& jse)
    {
        ELOG1(EJavaCaptain, "Rtc::isThereInstalledMidlets() - JavaStorage close failed: %s ",
              jse.toString().c_str());
    }

    return result;
}


} // namespace captain
} // namespace java

