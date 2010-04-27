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

#include "comms.h"
#include "commsendpoint.h"
#include "commsmessage.h"
#include "logger.h"

#include "coreinterface.h"
#include "rtcinterface.h"

#include "sample.h"

#ifdef __SYMBIAN32__
java::captain::ExtensionPluginInterface* getExtensionPlugin()
{
#else
extern "C" java::captain::ExtensionPluginInterface* getExtensionPlugin()
{
#endif
    return new java::captain::Sample();
}

namespace java
{
namespace captain
{

Sample::Sample() : mCore(0)
{
    JELOG2(EJavaCaptain);
}

Sample::~Sample()
{
    JELOG2(EJavaCaptain);
}

void Sample::startPlugin(CoreInterface* core)
{
    JELOG2(EJavaCaptain);
    mCore = core;
    mCore->getComms()->registerListener(PLUGIN_ID_SAMPLE_C, this);
}

void Sample::stopPlugin()
{
    JELOG2(EJavaCaptain);
    mCore->getComms()->unregisterListener(PLUGIN_ID_SAMPLE_C, this);
    mCore = 0;
}

EventConsumerInterface* Sample::getEventConsumer()
{
    JELOG2(EJavaCaptain);
    return this;
}

ApplicationManagementEventsInterface* Sample::getApplicationManagementListener()
{
    JELOG2(EJavaCaptain);
    return this;
}

ApplicationRuntimeEventsInterface* Sample::getApplicationRuntimeListener()
{
    JELOG2(EJavaCaptain);
    return this;
}
java::comms::CommsListener* Sample::getCommsListener()
{
    JELOG2(EJavaCaptain);
    return this;
}

// EventConsumerInterface
void Sample::event(const std::string& eventProvider,
                   java::comms::CommsMessage& aMsg)
{
    LOG1(EJavaCaptain, EInfo, "Sample::event(), aEvent.event = %s", eventProvider.c_str());
}

// ApplicationManagementEventsInterface
void Sample::amAdded(const uids_t& uids)
{
    JELOG2(EJavaCaptain);
    LOG1(EJavaCaptain, EInfo, "Sample::amAdded, %d uids", uids.size());
}

void Sample::amUpdated(const uids_t& uids)
{
    JELOG2(EJavaCaptain);
    LOG1(EJavaCaptain, EInfo, "Sample::amUpdated, %d uids", uids.size());
}

void Sample::amDeleted(const uids_t& uids)
{
    JELOG2(EJavaCaptain);
    LOG1(EJavaCaptain, EInfo, "Sample::amDeleted, %d uids", uids.size());
}

// ApplicationRuntimeEventsInterface
void Sample::arLaunched(const Uid& aUID, const int& aRuntimeCommsAddress)
{
    JELOG2(EJavaCaptain);
    std::wstring wuidi = aUID.toString();
    std::string suidi(wuidi.begin(), wuidi.end());
    LOG2(EJavaCaptain, EInfo, "Sample::arLaunched(%s, %d)", suidi.c_str(), aRuntimeCommsAddress);
}

void Sample::arTerminated(const Uid& aUID, const int& /*aExitCode*/)
{
    JELOG2(EJavaCaptain);
    std::wstring wuidi = aUID.toString();
    std::string suidi(wuidi.begin(), wuidi.end());
    LOG1(EJavaCaptain, EInfo, "Sample::arTerminated(%s, %d)", suidi.c_str());
}

// CommsListener methods
void Sample::processMessage(CommsMessage&/* message*/)
{
    JELOG2(EJavaCaptain);
}

} // namespace captain
} // namespace java

