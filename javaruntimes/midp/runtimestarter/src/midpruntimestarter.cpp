/*
* Copyright (c) 2009 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  This class provides container for message.
*
*/


#include <string>
#include <string.h>
#include <algorithm>
#include <iostream>
#include <fstream>
#include <unistd.h>

#include "midpruntimestarter.h"
#include "applicationinfosetter.h"
#include "runtimeexception.h"
#include "runtimestarterutils.h"
#include "midpruntimearguments.h"

#include "dynamiclibloader.h"
#include "javainifileutils.h"
#include "javacommonutils.h"
#include "javaoslayer.h"
#include "logger.h"

#include "commsmessage.h"
#include "rtcmessages.h"

#include "pushcontrollerstarter.h"

#include "javastoragenames.h"

#include "javacoreui.h"
#include "javacoreuiparams.h"

#include "com_nokia_mj_impl_rt_midp_MemoryLogger.h"

using namespace java::runtime;
using namespace java::util;
using namespace java::push;
using namespace java::storage;
using namespace java::captain;
using namespace java::ui;

const wchar_t* const RUNTIME_MAIN_CLASS = L"com.nokia.mj.impl.rt.midp.Main";
const wchar_t* const TRUE_WSTR          = L"true";


MidpRuntimeStarter::MidpRuntimeStarter(): mMidletInfo(new MidletInfo()), // codescanner::nonleavenew
        mRuntimeState(Constructed)
{
    JELOG2(EJavaRuntime);
}

MidpRuntimeStarter::~MidpRuntimeStarter()
{
    JELOG2(EJavaRuntime);
    if (mPushLib.get())
    {
        mPushLib->closeLib();
    }
}

int MidpRuntimeStarter::start(int argc, char *argv[])
{
    JELOG2(EJavaRuntime);

    // Create instance of RuntimeStarterUtils for thread supervisioning.
    std::auto_ptr<RuntimeStarterUtils> starterUtils(new RuntimeStarterUtils()); // codescanner::nonleavenew
    starterUtils->startThreadSupervisor();

    // Parse the args received from Captain.
    parseArgs(argc, argv);

    initComms();

    // Some push plugins needs to get the UID of the MIDlet and the root
    // path. Setting MidpRuntimeStarter as a service provider.
    setApplicationInfoProvider(*this);

    // Check if push command was receieved.
    if (mMidletInfo->mPushStart)
    {
        // In order ot serve ApplicationInfo we need to solve the
        // MIDlet suite UID.
        std::auto_ptr<JavaStorage> storage(JavaStorage::createInstance());
        storage->open();
        getMIDletSuiteUidFromStorage(*storage.get());
        storage->close();
        storage.reset(0);

        handlePushStart();
        // Handle case where during the push listening we receive a close cmd.
        if (mRuntimeState == Closing)
        {
            closePush();
            return 0;
        }
    }
    // Load the core UI
    std::auto_ptr<java::util::DynamicLibLoader> coreUiLoader;
    startCoreUi(coreUiLoader);

    // Create starter for starting the JVM
    std::auto_ptr<JvmStarter>
    jvm(JvmStarter::getJvmStarterInstance(JvmStarter::CLDC,
                                          L"Midp"));

    // Don't know the class path in pre-warm state.
    if (!mMidletInfo->mPreWarmStart)
    {
        jvm->appendClassPath(mMidletInfo->mClassPath);
        jvm->appendApplicationArgument(L"-uid");
        jvm->appendApplicationArgument(mMidletInfo->mMIDletUid.toString());

        // Get the heap size recorded from the previous runs.
        int heapSize = getHeapSize();
        if (heapSize > com_nokia_mj_impl_rt_midp_MemoryLogger_MAX_OLD_SPACE)
        {
            heapSize = com_nokia_mj_impl_rt_midp_MemoryLogger_MAX_OLD_SPACE;
        }

        if (heapSize > com_nokia_mj_impl_rt_midp_MemoryLogger_DEFAULT_OLD_SPACE)
        {
            jvm->overrideOldHeapSize(heapSize / 1024);
        }
    }

    jvm->setMainClass(RUNTIME_MAIN_CLASS);
    jvm->enableThreadDumping();
    jvm->appendSystemProperty(L"-Dcom.nokia.rt.port=midp");
#ifndef RD_JAVA_UI_QT
    jvm->appendSystemProperty(L"-Dcom.nokia.legacy.support=LegacySymbian");
#endif // RD_JAVA_UI_QT

    // Provide access to this object by Java peer via delivering a pointer
    // to this object.
    jvm->appendApplicationArgument(L"-handle");
    MidpStarterInternalSupport* internalSupport = this;
    int handle = reinterpret_cast<int>(internalSupport);
    jvm->appendApplicationArgument(JavaCommonUtils::intToWstring(handle));

    // If the intention is to go to prewarmed state pass the info to Java peer.
    if (mMidletInfo->mPreWarmStart)
    {
        jvm->appendApplicationArgument(L"-prewarm");
        // Captain needs a pid in order to identify the prewarmed runtime.
        jvm->appendApplicationArgument(JavaCommonUtils::intToWstring(getpid()));

        // Starting with lower old space in pre warm case.
        jvm->overrideOldHeapSize(36);
    }

    // If the requested to go to back ground, pass the info to Java peer.
    if (mMidletInfo->mBackGroundRequested)
    {
        jvm->appendApplicationArgument(L"-background");
        jvm->appendApplicationArgument(TRUE_WSTR);
    }

    // If the autoinvocation happened pass the info to Java peer.
    if (mMidletInfo->mAutoInvocationRequested)
    {
        jvm->appendApplicationArgument(L"-autoinvocation");
        jvm->appendApplicationArgument(TRUE_WSTR);
        jvm->appendApplicationArgument(L"-autoInvocationAdditional");
        jvm->appendApplicationArgument(mPushAdditionalInfo);
    }

    // Enable thread dumping. If the captain informed about debug mode
    // or arguments for MIDlet set the arguments provided by the captain.
    std::wstring midletArgs;
    starterUtils->enableDevelopmentFeatures(*jvm.get(),
                                            mMidletInfo->mDebugRequested || mMidletInfo->mMIDletHasArgs,
                                            &midletArgs);

    if (midletArgs.length() == 0)
    {
        // If MIDlet was waiting for push or was pre-warmed,
        // the arguments can be already in midlet info
        midletArgs = mMidletInfo->mMIDletArgs;
    }

    if (midletArgs.length() > 0)
    {
        // Pass the arguments to 'Java side' in encoded form.
        // Encoding is done to prevent security risks.
        std::wstring encodedArgs = L"-Dcom.nokia.mid.cmdline=";

        encodedArgs.append(encodeArgs(midletArgs));

        jvm->appendSystemProperty(encodedArgs);
        // remember the args
        mMidletInfo->mMIDletArgs = encodedArgs;
    }

    // When starting this property has always value "1"
    jvm->appendSystemProperty(L"-Dcom.nokia.mid.cmdline.instance=1");

    // There might be extensions available. Solving if those exist.
    std::wstring extendedBootClassPath;
    // This call is platform dependent.
    starterUtils->getExtBootClassPath(extendedBootClassPath);
    if (extendedBootClassPath.length() > 0)
    {
        // Append the extensions to bootclasspath.
        jvm->appendBootClassPath(extendedBootClassPath);

        // Provide the bootclasspath also as a system propery
        // for solving the protected and restricted packages.
        std::wstring addOnList(L"-Dcom.nokia.mj.addon.list=");
        addOnList += extendedBootClassPath;
        jvm->appendSystemProperty(addOnList);
    }

    mRuntimeState = Active;

    // Start the JVM.
    int status = jvm->startJvm();

    if (mComms.get())
    {
        mComms->disconnect();
    }
    CoreUi::releaseUi(coreUiLoader);

    return status;
}


void MidpRuntimeStarter::parseArgs(int argc, char* argv[])
{
    JELOG2(EJavaRuntime);

    bool appUidFound = false;

    // Loop through the arguments. First one can be skipped, because it
    // contains the name of the executable.
    for (int index = 1 ; index < argc; index++)
    {
        const char* key = argv[index];
        const char* value = 0;

        // Checking if there is value available.
        if (index+1 < argc)
        {
            value = argv[index+1];
        }
        LOG2(EJavaRuntime, EInfo,"MidpRuntimeStarter::parseArgs(). "
             "Handling key %s with value %s",
             key,
             value==0?"<No Value>":value);

        if (strcmp(key, APP_UID_ARGUMENT) == 0)
        {
            if (value)
            {
                mMidletInfo->mMIDletUid =
                    Uid(java::util::JavaCommonUtils::utf8ToWstring(value));

                appUidFound = true;
                index++;
            }
        }

        else if (strcmp(key, PREWARM_ARGUMENT) == 0)
        {
            mMidletInfo->mPreWarmStart = true;
            LOG1(EJavaRuntime, EInfo,"PREWARM_ARGUMENT = %d ",  mMidletInfo->mPreWarmStart);
        }

        else if (strcmp(key, PUSH_ARGUMENT) == 0)
        {
            mMidletInfo->mPushStart = true;
            LOG1(EJavaRuntime, EInfo,"PUSH_ARGUMENT = %d ",  mMidletInfo->mPushStart);
        }

        else if (strcmp(key, AUTO_INVOCATION_ARGUMENT) == 0)
        {
            mMidletInfo->mAutoInvocationRequested = true;
            LOG1(EJavaRuntime, EInfo,"AUTO_INVOCATION_ARGUMENT = %d ",  mMidletInfo->mAutoInvocationRequested);
        }

        else if (strcmp(key, DEBUG_ARGUMENT) == 0)
        {
            mMidletInfo->mDebugRequested = true;
            LOG1(EJavaRuntime, EInfo,"DEBUG_ARGUMENT = %d ",  mMidletInfo->mDebugRequested);
        }

        else if (strcmp(key, EXTRA_ARGUMENTS) == 0)
        {
            mMidletInfo->mMIDletHasArgs = true;
            LOG1(EJavaRuntime, EInfo,"EXTRA_ARGUMENTS = %d ",  mMidletInfo->mMIDletHasArgs);
        }

        else if (strcmp(key, BG_START_ARGUMENT) == 0)
        {
            mMidletInfo->mBackGroundRequested = true;
            LOG1(EJavaRuntime, EInfo,"BG_START_ARGUMENT = %d ",  mMidletInfo->mBackGroundRequested);
        }

        else
        {
            std::string errorStr("Unknown argument received: ");
            errorStr.append(key);
            throw RuntimeException(errorStr, __FILE__, __FUNCTION__, __LINE__);
        }
    }

    if (appUidFound == false && mMidletInfo->mPreWarmStart == false)
    {
        std::string errorStr("Argument ");
        errorStr.append(APP_UID_ARGUMENT);
        errorStr.append(" not defined!");
        throw RuntimeException(errorStr, __FILE__, __FUNCTION__, __LINE__);
    }
}

void MidpRuntimeStarter::startCoreUi(std::auto_ptr<java::util::DynamicLibLoader>& coreUiLoader)
{
    CoreUi& coreUi = CoreUi::getUiInstance(coreUiLoader);

    // Create the default UI only if not going into pre-warmed state.
    if (!mMidletInfo->mPreWarmStart)
    {

        // Open a session to JavaStorage.
        std::auto_ptr<JavaStorage> javaStorage(JavaStorage::createInstance());
        javaStorage->open();

        CoreUiParams uiParams;

        // Get the MIDlet suite UID from storage.
        if (mMidletInfo->mMIDletSuiteUid.toString() == L"")
        {
            getMIDletSuiteUidFromStorage(*javaStorage.get());
        }

        // Check if the MIDlet has defined the Nokia-MIDlet-App-Orientation
        // JAD attribute.
        std::auto_ptr<std::wstring> appOrientation
        (getMidletAttributeFromStorage(*javaStorage.get(),
                                       L"Nokia-MIDlet-App-Orientation"));
        if (appOrientation.get() != 0)
        {

            std::transform(appOrientation->begin(), appOrientation->end(),
                           appOrientation->begin(), tolower);
            if (*appOrientation == L"portrait")
            {
                uiParams.setOrientation(PORTRAIT);
            }
            else if (*appOrientation == L"landscape")
            {
                uiParams.setOrientation(LANDSCAPE);
            }
            else
            {
                WLOG1(EJavaRuntime, "appOrientation contained unknown value: %S",
                      appOrientation->c_str());
            }
        }
        else
        {
            LOG(EJavaRuntime, EInfo, "Nokia-MIDlet-App-Orientation not defined");
        }

        // Check if the MIDlet has defined the MIDlet-Splash-Screen-Image
        // JAD attribute.
        std::auto_ptr<std::wstring> splashScreen
        (getMidletAttributeFromStorage(*javaStorage.get(),
                                       L"MIDlet-Splash-Screen-Image"));
        if (splashScreen.get() != 0)
        {
            std::transform(splashScreen->begin(), splashScreen->end(),
                           splashScreen->begin(), tolower);
            if (*splashScreen == L"suppress")
            {
                // If MIDlet-Splash-Screen-Image JAD attribute is suppress then
                // we start the UI into background.
                LOG(EJavaRuntime, EInfo, "MIDlet-Splash-Screen-Image is suppress");
                uiParams.setScreenMode(NO_START_SCREEN);
                uiParams.setBackgroundStart(true);
            }
            else
            {
                // If MIDlet-Splash-Screen-Image JAD attribute is not suppress then
                // we need to solve the root path of the MIDlet and provide that
                // to the coreUI.
                uiParams.setScreenMode(MIDLET_DEFINED_SCREEN);
                LOG1(EJavaRuntime, EInfo, "MIDlet-Splash-Screen-Image, setPath to %S",
                     mMidletInfo->mMIDletRootPath.c_str());
            }
        }
        else
        {
            LOG(EJavaRuntime, EInfo, "MIDlet-Splash-Screen-Image not defined");
            uiParams.setScreenMode(DEFAULT_START_SCREEN);
        }

        getRootPath();
        uiParams.setImagePath(mMidletInfo->mMIDletRootPath);

        // If there was a background start requst pass the info to coreUi.
        // Also the autoinvocation start puts the screen into BG.
        if (mMidletInfo->mBackGroundRequested || mMidletInfo->mAutoInvocationRequested)
        {
            uiParams.setBackgroundStart(true);
        }

        // Start the coreUI.
        JavaOsLayer::startUpTrace("Starting CoreUI", -1, -1);
        coreUi.start(mMidletInfo->mMIDletUid, &uiParams);
        JavaOsLayer::startUpTrace("CoreUI started", -1, -1);

        getMIDletSuiteInfoFromStorage(javaStorage.get(), JAR_PATH,
                                      mMidletInfo->mClassPath);
    }

}

void MidpRuntimeStarter::
ApplicationStateChangeRequest(ApplicationState state)
{
    JELOG2(EJavaRuntime);
    LOG1(EJavaRuntime, EInfo, "ApplicationStateChangeRequest: %d", state);
    switch (state)
    {
    case START_APPLICATION:
    case CONTINUE_APPLICATION_STARTUP:
        doStateChange(Start);
        break;

    case CLOSE_APPLICATION:
        doStateChange(Stop);
        break;

    default:
        std::string errorStr("Illegal state ");
        errorStr.append(JavaCommonUtils::intToString(state));
        throw RuntimeException(errorStr, __FILE__, __FUNCTION__, __LINE__);
    }
}

void MidpRuntimeStarter::doStateChange(StateChangeRequest request)
{
    JELOG2(EJavaRuntime);
    LOG2(EJavaRuntime, EInfo, "MidpRuntimeStarter::doStateChange. Request: %d,"
         " current state: %d", request, mRuntimeState);
    ScopedLock lock(mProcessesMutex); // Making the method thread safe
    switch (request)
    {
    case Start:
        // Check if we are still waiting start command.
        if (mRuntimeState == PushListen)
        {
            mRuntimeState = Active;
            //Allow application startUp.
            mMonitor->notify();
        }
        break;

    case Stop:
        // Check if we are still waiting start command.
        if (mRuntimeState == PushListen)
        {
            //Allow application to close.
            mRuntimeState = Closing;
            mMonitor->notify();
        }
        break;

    default:
        std::string errorStr("Illegal request ");
        errorStr.append(JavaCommonUtils::intToString(request));
        throw RuntimeException(errorStr, __FILE__, __FUNCTION__, __LINE__);
    }

}
void MidpRuntimeStarter::closeRuntimeInd()
{
    JELOG2(EJavaRuntime);
    LOG(EJavaRuntime, EInfo, "Starter got close indication from JVM");
    if (mMidletInfo->mPushStart)
    {
        closePush();
    }
}

void MidpRuntimeStarter::setUids(const Uid& midletUid, const Uid& midletSuiteUid)
{
    JELOG2(EJavaRuntime);
    mMidletInfo->mMIDletUid = midletUid;
    mMidletInfo->mMIDletSuiteUid = midletSuiteUid;
    LOG2(EJavaRuntime, EInfo, "Setting UID set during pre-warm start: m=%S, s=%S",
         mMidletInfo->mMIDletUid.toString().c_str(),
         mMidletInfo->mMIDletSuiteUid.toString().c_str());
}

int MidpRuntimeStarter::getHeapSize() const
{
    JELOG2(EJavaRuntime);
    // No need to do charcter conversion in here.
    std::string root(mMidletInfo->mClassPath.begin(),
                     mMidletInfo->mClassPath.end());
    int heapSize = -1;

    // The file is stored into same location where the jar file is put.
    // replace the 'xxx.jar' with 'heap' and open the file.

    size_t pos = root.find_last_of("/\\");
    if (pos != std::string::npos)
    {
        root.erase(pos+1);
        root += "heap";
        LOG1(EJavaRuntime, EInfo, "Heap size from file %s.", root.c_str());
        try
        {
            std::ifstream heapFile;
            heapFile.open(root.c_str(), std::ifstream::in);
            heapFile >> heapSize;
            heapFile.close();
            LOG1(EJavaRuntime, EInfo, "  heap: %d.", heapSize);
        }
        catch (std::exception& e)
        {
            LOG2(EJavaRuntime, EInfo, "Not able to read from file %s. Error %s",
                 root.c_str(), e.what());
        }
    }
    return heapSize;
}

std::wstring MidpRuntimeStarter::encodeArgs(const std::wstring& str)
{
    // Modify the places where the following characters are used to prevent
    // possible security problems when this string is passed as an command line
    // system property parameter to JVM
    const std::wstring specials(L"= -%");

    std::string::size_type idx = str.find_first_of(specials);
    std::string::size_type cur = 0;

    std::wstring res;
    std::string convBuf;

    while (idx != std::string::npos)
    {
        // Add all characters up to and including the current special char to
        // final result string
        if (idx >= cur)
        {
            res.append(str.substr(cur, (idx - cur) + 1));
        }

        // Encode all special characters 'X' in same way.
        // "X" -> "X%"
        res.append(L"%");

        cur = idx + 1;
        idx = str.find_first_of(specials, cur);
    }

    // Add characters after last special character if any
    res.append(str.substr(cur, str.length() - cur));

    return res;
}

void MidpRuntimeStarter::getMIDletSuiteUidFromStorage(java::storage::JavaStorage& storage)
{
    JELOG2(EJavaRuntime);

    // Get the MIDlet suite UID.
    JavaStorageApplicationEntry_t midletEntries;

    // Reading the MIDlet specific attributes from APPLICATION_TABLE.
    // MIDlet UID is a key.
    storage.read(APPLICATION_TABLE, mMidletInfo->mMIDletUid,
                 midletEntries);
    // Storing UID of the MIDlet suite.
    mMidletInfo->mMIDletSuiteUid = Uid(getDbValue(midletEntries, PACKAGE_ID));
}

void MidpRuntimeStarter::getMIDletSuiteInfoFromStorage(JavaStorage* storageConnection,
        const std::wstring& key,
        std::wstring& value) const
{
    JELOG2(EJavaRuntime);

    // If the JavaStorage connection is created temporarily in this method, auto_ptr is used to
    // ensure closing and destroyng the JavaStorage connection. Don't use javaStorage ptr to any
    // other purposes - instead use storageConnection.
    std::auto_ptr<JavaStorage> javaStorage;
    if (storageConnection == 0)
    {
        javaStorage.reset(JavaStorage::createInstance());
        storageConnection = javaStorage.get();
        javaStorage->open();
    }

    JavaStorageApplicationEntry_t midletSuiteEntries;

    // Reading the MIDlet suite specific attributes from
    // APPLICATION_PACKAGE_TABLE. MIDlet suite UID is a key.
    storageConnection->read(APPLICATION_PACKAGE_TABLE,
                            mMidletInfo->mMIDletSuiteUid,
                            midletSuiteEntries);
    value = getDbValue(midletSuiteEntries, key);
}

std::wstring*
MidpRuntimeStarter::getMidletAttributeFromStorage(JavaStorage& storage,
        const std::wstring& searchKey)
const
{
    JELOG2(EJavaRuntime);

    JavaStorageEntry findPattern;
    JavaStorageApplicationEntry_t findPatterns;
    JavaStorageApplicationList_t foundEntries;

    findPattern.setEntry(ID, mMidletInfo->mMIDletSuiteUid.toString(),
                         JavaStorageEntry::STRING);
    findPatterns.insert(findPattern);

    findPattern.setEntry(NAME, searchKey, JavaStorageEntry::STRING);
    findPatterns.insert(findPattern);

    findPattern.setEntry(VALUE,L"");
    findPatterns.insert(findPattern);

    storage.search(APPLICATION_PACKAGE_ATTRIBUTES_TABLE, findPatterns, foundEntries);

    JavaStorageApplicationList_t::iterator iter = foundEntries.begin();

    if (iter != foundEntries.end())
    {
        JavaStorageEntry findPatternForValue;
        findPatternForValue.setEntry(VALUE, L"");
        JavaStorageApplicationEntry_t::iterator valueIter = iter->find(findPatternForValue);
        if (valueIter != iter->end())
        {
            return new std::wstring(valueIter->entryValue()); // codescanner::nonleavenew
        }
        else
        {
            std::string errorStr("MIDLET with UID: ");
            std::wstring uid = mMidletInfo->mMIDletUid.toString();
            errorStr += std::string(uid.begin(), uid.end());
            errorStr += ". Error reading from storage: ";
            errorStr += std::string(searchKey.begin(), searchKey.end());
            throw RuntimeException(errorStr, __FILE__, __FUNCTION__, __LINE__);
        }
    }
    return 0;
}



std::wstring MidpRuntimeStarter::getDbValue(
    const java::storage::JavaStorageApplicationEntry_t& entry,
    const std::wstring& key) const
{
    JELOG2(EJavaRuntime);
    JavaStorageEntry findPattern;
    std::wstring empty;

    findPattern.setEntry(key, empty);

    // Get attribute from read attributes.
    JavaStorageApplicationEntry_t::const_iterator findIterator =
        entry.find(findPattern);

    if (findIterator != entry.end())
    {
        return findIterator->entryValue();
    }
    else
    {
        std::string errorStr("MIDLET with UID: ");
        std::wstring uid = mMidletInfo->mMIDletUid.toString();
        errorStr += std::string(uid.begin(), uid.end());
        errorStr += ". Not able to find attribute ";
        errorStr += std::string(key.begin(), key.end());
        throw RuntimeException(errorStr, __FILE__, __FUNCTION__, __LINE__);
    }
}

void MidpRuntimeStarter::handlePushStart()
{
    JELOG2(EJavaRuntime);
    mMonitor.reset(Monitor::createMonitor());
    mRuntimeState = PushListen;
    PushControllerStarter::getPushControllerStarter(mPushLib).
    startListen(mMidletInfo->mMIDletUid, this);
    mMonitor->wait();
}

void MidpRuntimeStarter::closePush()
{
    // Received a close message from Java peer.
    PushControllerStarter::getPushControllerStarter(mPushLib).close();
}

void MidpRuntimeStarter::
startMidletRequestFromPush(const std::wstring& pushAdditionalInfo)
{
    JELOG2(EJavaRuntime);
    mPushAdditionalInfo = pushAdditionalInfo;

    // This can be considered to be auto invocation. Setting flag on
    // to be delivered to Java runtime.
    mMidletInfo->mAutoInvocationRequested = true;

    doStateChange(Start);
}

void MidpRuntimeStarter::closeRuntimeRequestFromPush()
{
    JELOG2(EJavaRuntime);
    doStateChange(Stop);
}

void MidpRuntimeStarter::initComms()
{
    JELOG2(EJavaRuntime);
    mComms.reset(new CommsClientEndpoint(L"javacaptain")); // codescanner::nonleavenew
    mComms->registerDefaultListener(this);
    int result = mComms->connect(IPC_ADDRESS_JAVA_CAPTAIN_C);
    if (result != 0)
    {
        std::string errorStr("Connection to JavaCaptain failed: Reason = ");
        errorStr.append(JavaCommonUtils::intToString(result));
        throw RuntimeException(errorStr, __FILE__, __FUNCTION__, __LINE__);
    }
    CommsMessage msg;
    msg.setModuleId(PLUGIN_ID_RTC_C);
    setApplicationRunningIndParams(msg, mMidletInfo->mMIDletUid, 0);
    mComms->send(msg);
}

void MidpRuntimeStarter::processMessage(java::comms::CommsMessage& message)
{
    JELOG2(EJavaRuntime);
    int messageId = message.getMessageId();

    // RTC_MSG_ID_LAUNCH_APPLICATION_REQ and
    // RTC_MSG_ID_TERMINATE_APPLICATION_REQ are received from the Java Captain
    // when we are listening push connection and the VM is not started.
    // When the MIDlet is running normally, these two messages are handled
    // by Java peer.
    // RTC_MSG_ID_ADD_PUSH_CONNECTION_IND is always handled here.

    switch (messageId)
    {
    case RTC_MSG_ID_LAUNCH_APPLICATION_REQ:
    {
        LOG(EJavaRuntime, EInfo, "RTC_MSG_ID_LAUNCH_APPLICATION_REQ");
        // Start the push MIDlet that is pre started to listen mode.

        Uid          uid;
        int          type;
        int          options;
        std::string  rtc;
        std::wstring midletArgs;
        std::wstring runtimeArguments;

        getLaunchApplicationReqParams(message, uid, type, options, rtc,
                                      midletArgs,
                                      runtimeArguments);
        if (type == RTC_LAUNCH_TYPE_AUTO_INVOCATION_C)
        {
            mMidletInfo->mAutoInvocationRequested = true;
        }

        // The arguments will be passed to midlet when JVM is started
        mMidletInfo->mMIDletArgs = midletArgs;

        ApplicationStateChangeRequest(START_APPLICATION);
    }
    break;

    case RTC_MSG_ID_TERMINATE_APPLICATION_REQ:
        LOG(EJavaRuntime, EInfo, "RTC_MSG_ID_TERMINATE_APPLICATION_REQ");

        // Stop the push MIDlet that is pre started to listen mode.
        ApplicationStateChangeRequest(CLOSE_APPLICATION);
        break;

    case RTC_MSG_ID_ADD_PUSH_CONNECTION_IND:
    {
        LOG(EJavaRuntime, EInfo, "RTC_MSG_ID_ADD_PUSH_CONNECTION_IND");
        Uid uidFromMessage;
        getUpdatePushReqParams(message, uidFromMessage);
        if (uidFromMessage == mMidletInfo->mMIDletUid)
        {

            PushControllerStarter::getPushControllerStarter(mPushLib).
            updatePushRegs(mMidletInfo->mMIDletUid, this);
        }
        else
        {
            ELOG2(EJavaRuntime, "getUpdatePushReqParams: wrong UID!: "
                  "%S should be %S",
                  uidFromMessage.toString().c_str(),
                  mMidletInfo->mMIDletUid.toString().c_str());
        }
    }
    break;

    default:
        ELOG1(EJavaRuntime, "Unknown message sent to Runtime %d",
              messageId);
        break;
    }
}

const std::wstring& MidpRuntimeStarter::getRootPath() const
{
    JELOG2(EJavaRuntime);
    // If the root path is not yet cached, read it from storage.
    if (mMidletInfo->mMIDletRootPath.length() == 0)
    {
        getMIDletSuiteInfoFromStorage(0, ROOT_PATH,
                                      mMidletInfo->mMIDletRootPath);
    }
    return mMidletInfo->mMIDletRootPath;
}

const java::util::Uid& MidpRuntimeStarter::getUid() const
{
    return mMidletInfo->mMIDletUid;
}
