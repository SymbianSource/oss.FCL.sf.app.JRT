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
* Description:  This class contains utilities for starting the JVM.
*
*/


#include <sstream>

#include "commsclientendpoint.h"
#include "commslistener.h"
#include "commsmessage.h"
#include "rtcmessages.h"
#include "coremessages.h"

#include "runtimestarterutils.h"
#include "osthreadsupervisor.h"

#include "logger.h"
#include "exception"
#include "runtimeexception.h"
#include "javacommonutils.h"
#include "bootclasspath.h"
#include "ueiargsparser.h"

#include "threaddumper.h"

namespace java // codescanner::namespace
{
namespace runtime // codescanner::namespace
{

/**
 * This class is for listening the messages from Java Captain related to
 * development fetures.
 */
class DevelopmentFeaturesListener : public java::comms::CommsListener
{
public:
    /**
     * Default contructor of DevelopmentFeaturesListener.
     */
    DevelopmentFeaturesListener();

    /**
     * Destructor of DevelopmentFeaturesListener.
     */
    ~DevelopmentFeaturesListener();

    /**
     * This processes messages only with moduleId PLUGIN_ID_JAVACAPTAIN_CORE_C
     *@see commslistener.h
     */
    void processMessage(CommsMessage& message);
};
} // end namespace runtime
} // end namespace java

using namespace java::runtime;
using namespace java::util;


const wchar_t* const JAVA_CAPTAIN = L"javacaptain";
const int DEVELOPMENT_MODULE_ID = java::comms::PLUGIN_ID_JAVACAPTAIN_CORE_C; // codescanner::constnames

OS_EXPORT
RuntimeStarterUtils::RuntimeStarterUtils(): mComms(0),
        mSupervisor(0),
        mListener(0)
{
    JELOG2(EJavaRuntime);
}

OS_EXPORT RuntimeStarterUtils::~RuntimeStarterUtils()
{
    JELOG2(EJavaRuntime);
    delete mSupervisor;
    mSupervisor = 0;

    delete mListener;
    mListener = 0;

    delete mComms;
    mComms = 0;
}

OS_EXPORT
void RuntimeStarterUtils::startThreadSupervisor(bool tryThreadDumping)
{
    JELOG2(EJavaRuntime);
    // Delete the old one if exists.
    if (mSupervisor)
    {
        delete mSupervisor;
        mSupervisor = 0;
    }
    mSupervisor = new OsThreadSupervisor(tryThreadDumping); // codescanner::nonleavenew
}

OS_EXPORT
void RuntimeStarterUtils::enableDevelopmentFeatures(JvmStarter& jvmStarter,
        bool askForArgs,
        std::wstring *pApplicationArgs)
{
    JELOG2(EJavaRuntime);

    // Try to find if there is already client endpoint available.
    CommsClientEndpoint* comms =
        java::comms::CommsClientEndpoint::find(JAVA_CAPTAIN);

    if (comms == 0)
    {
        // Not found, so creating new one.
        mComms = new CommsClientEndpoint(); // codescanner::nonleavenew
        comms = mComms;
    }

    // Create a listener for receiving development messages from JavaCaptain.
    mListener = new DevelopmentFeaturesListener(); // codescanner::nonleavenew
    comms->registerListener(DEVELOPMENT_MODULE_ID, mListener);

    // If necessary ask more arguments from JC
    if (askForArgs)
    {
        LOG(EJavaRuntime, EInfo, "Getting extra args from Java Captain");

        java::comms::CommsMessage message;
        java::comms::CommsMessage reply;

        // Send a message to Java captain in order to get debug attributes.
        java::captain::setGetExtraArguments(message);

        comms->connect(java::comms::IPC_ADDRESS_JAVA_CAPTAIN_C);
        int status = comms->sendReceive(message, reply, 1);

        if (status == 0)
        {
            std::wstring runtimeArgs;
            std::wstring applicationArgs;
            java::captain::getGetExtraArgumentsAck(reply, runtimeArgs,
                                                   applicationArgs);

            if (pApplicationArgs)
            {
                *pApplicationArgs = applicationArgs;
            }

            addRuntimeArguments(runtimeArgs, jvmStarter);
        }
    }
}

void RuntimeStarterUtils::addRuntimeArguments(const std::wstring& aRuntimeArgs,
        JvmStarter& aJvmStarter)
{
    if (aRuntimeArgs.length() > 0)
    {
        UeiArgsParser parser;
        std::wstring jvmArgs = parser.convertUeiArgsToJvmArgs(aRuntimeArgs);

        std::wstringstream stream;
        stream << jvmArgs; // codescanner::leave

        std::wstring jvmArg;
        while (stream.good())
        {
            stream >> jvmArg; // codescanner::leave
            if (jvmArg.length() > 0)
            {
                LOG1(EJavaRuntime, EInfo, "  Attrib: %S", jvmArg.c_str());
                aJvmStarter.appendRawJvmArgument(jvmArg);
            }
        }
    }
}
OS_EXPORT
void RuntimeStarterUtils::getExtBootClassPath(std::wstring& extendedBootClassPath)
{
    getExtendedBootClassPath(extendedBootClassPath);
}

DevelopmentFeaturesListener::DevelopmentFeaturesListener()
{
    JELOG2(EJavaRuntime);
}

DevelopmentFeaturesListener::~DevelopmentFeaturesListener()
{
    JELOG2(EJavaRuntime);
}

void DevelopmentFeaturesListener::processMessage(CommsMessage& message)
{
    JELOG2(EJavaRuntime);
    if (message.getMessageId() == java::captain::CORE_MSG_ID_DO_THREAD_DUMP)
    {
        WLOG(EJavaRuntime, "Doing THREAD DUMP");
        ThreadDump::doDump();
    }
    else
    {
        ELOG2(EJavaRuntime, "Unknown message sent to development features "
              "listener. messageId = %d, moduleId = %d.",
              message.getMessageId(), message.getModuleId());
    }
}
