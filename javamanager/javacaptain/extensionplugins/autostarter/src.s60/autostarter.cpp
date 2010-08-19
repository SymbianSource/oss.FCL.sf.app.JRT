/*
* Copyright (c) 2010 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  autostarter is Java Captain Symbian plugin that starts
*               auto start Java applications after they have been installed
*               and when the device boots.
*
*/

#include <apgcli.h> // for RApaLsSession
#include <e32base.h>
#include <f32file.h>
#include <javastorage.h>
#include <javastorageentry.h>
#include <javastoragenames.h>
#include <memory>

#include "commsendpoint.h"
#include "coreinterface.h"
#include "javaprocessconstants.h"
#include "javasymbianoslayer.h"
#include "javauid.h"
#include "javauids.h"
#include "logger.h"
#include "preinstallerstartermessages.h"
#include "rtcinterface.h"
#include "rtcmessages.h"

#include "autostarter.h"


using namespace java::storage;
using namespace java::util;


/**
 * Return pointer to ExtensionPluginInterface implementation for this
 * extension dll
 */
java::captain::ExtensionPluginInterface* getExtensionPlugin()
{
    return new java::captain::AutoStarter();
}

namespace java  // codescanner::namespace
{
namespace captain  // codescanner::namespace
{

/**
 * Empty contructor
 */
AutoStarter::AutoStarter() : mCore(0)
{
}

/**
 * Empty destructor
 */
AutoStarter::~AutoStarter()
{
}

/**
 * Implement PluginInterface method
 */
void AutoStarter::startPlugin(CoreInterface* core)
{
    LOG(EJavaCaptain, EInfo, "AutoStarter plugin started");

    mCore = core;
}

/**
 * Implement PluginInterface method
 */
void AutoStarter::stopPlugin()
{
    mCore = 0;
}

/**
 * Implement ExtensionPluginInterface method
 */
EventConsumerInterface* AutoStarter::getEventConsumer()
{
    return this;
}

/**
 * Handle Java Captain events sent by Boot event provider or
 * preinstaller starter
 *
 * Implement EventConsumerInterface method
 */
void AutoStarter::event(const std::string& eventProvider,
                       java::comms::CommsMessage& aMsg)
{
    LOG1(EJavaCaptain, EInfo, "AutoStarter::event received, id %d", aMsg.getMessageId());

    if (eventProvider == PREINSTALLER_EVENT_PROVIDER)
    {
        int exitStatus = 0;
        getPreinstallerExitedMessageParams(aMsg, exitStatus);
        LOG1(
            EJavaCaptain,
            EInfo,
            "AutoStarter::event() preinstaller exited event received (status=%d)",
            exitStatus);

            // Start auto-start MIDlets
            TRAPD(err, checkMIDletsToBeStartedL())
            if (KErrNone != err)
            {
                ELOG1(EJavaCaptain, "AutoStarter::checkMIDletsToBeStartedL: leaved (%d)", err);
            }
    }
}

/**
 *
 *
 */
void AutoStarter::checkMIDletsToBeStartedL()
{
    // Find all MIDlets with Nokia-MIDlet-auto-start
    // APPLICATION_TABLE, if (AUTORUN == AUTOSTART_TRUE) || (AUTORUN == AUTOSTART_ONCE),
    // call startMIDletL(ID)

    LOG(EJavaCaptain, EInfo, "AutoStarter::checkMIDletsToBeStartedL called");

    std::auto_ptr<JavaStorage> js(JavaStorage::createInstance());
    try
    {
        js->open(JAVA_DATABASE_NAME);

        // In Java Storage there is 'APPLICATION_TABLE' table that contains
        // the Uid of the application in ID column. The value in column AUTORUN is
        // one of the following AUTOSTART_FALSE (0), AUTOSTART_TRUE (1) or
        // AUTOSTART_ONCE (2)

        JavaStorageEntry attribute;
        JavaStorageApplicationEntry_t findPattern;
        JavaStorageApplicationList_t  foundEntries;

        // Get all IDs from APPLICATION_TABLE where AUTORUN is AUTOSTART_TRUE
        attribute.setEntry(AUTORUN, L"1");
        findPattern.insert(attribute);
        attribute.setEntry(ID, L"");
        findPattern.insert(attribute);

        js->search(APPLICATION_TABLE, findPattern, foundEntries);
        findPattern.clear();

        // Iterate through all found applications
        JavaStorageApplicationList_t::const_iterator appIterator;
        for (appIterator = foundEntries.begin(); appIterator != foundEntries.end(); ++appIterator)
        {
            std::wstring applicationId = appIterator->begin()->entryValue();

            LOG1WSTR(EJavaCaptain, EInfo,
                "AutoStarter::checkMIDletsToBeStartedL : going to start MIDlet, uid is %s",
                applicationId);

            startMIDletL(applicationId);
        }
        foundEntries.clear();


        // Get all IDs from APPLICATION_TABLE where AUTORUN is AUTOSTART_ONCE
        attribute.setEntry(AUTORUN, L"2");
        findPattern.insert(attribute);
        attribute.setEntry(ID, L"");
        findPattern.insert(attribute);

        js->search(APPLICATION_TABLE, findPattern, foundEntries);
        findPattern.clear();

        // Iterate through all found applications
        for (appIterator = foundEntries.begin(); appIterator != foundEntries.end(); ++appIterator)
        {
            std::wstring applicationId = appIterator->begin()->entryValue();

            LOG1WSTR(EJavaCaptain, EInfo,
                "AutoStarter::checkMIDletsToBeStartedL : going to start MIDlet (once), uid is %s",
                applicationId);

            if (startMIDletL(applicationId))
            {
                // after starting MIDlet must set AUTORUN value to AUTOSTART_FALSE
                JavaStorageApplicationEntry_t matchEntries;

                JavaStorageApplicationEntry_t updateEntries;
                JavaStorageEntry attribute;

                attribute.setEntry(ID, applicationId);
                matchEntries.insert(attribute);

                attribute.setEntry(AUTORUN, L"0");
                updateEntries.insert(attribute);

                js->update(APPLICATION_TABLE, updateEntries, matchEntries);
            }
        }
        foundEntries.clear();
    }
    catch (JavaStorageException& e)
    {
        ELOG1(EJavaCaptain,
            "AutoStarter::checkMIDletsToBeStartedL: Java Storage exception %s", e.what());
    }

    try
    {
        js->close();
    }
    catch (JavaStorageException& e2)
    {
        WLOG1(EJavaCaptain,
            "AutoStarter::checkMIDletsToBeStartedL: Java Storage exception "
            "when closing storage %s", e2.what());
    }
}


/**
 *
 *  @return true if launch succeeded, false is starting failed
 */
bool AutoStarter::startMIDletL(const std::wstring& aUid)
{
    // Try to check whether the MIDlet is present (or whether it has been
    // installed to a removable media that is not present now)
    if (!isMIDletPresent(aUid))
    {
        return false;
    }

    // Start MIDlet
    rtcLaunchInfo launchInfo(aUid);

    bool launchSuccess = mCore->getRtc()->launch(launchInfo);
    if (!launchSuccess)
    {
        ELOG(EJavaCaptain, "AutoStarter::startMIDletL: launching MIDlet failed");
    }

    return launchSuccess;
}


/**
 * Try to check whether the MIDlet is present (or whether it has been
 * installed to a removable media that is not present now)
 *
 * @return false if it is certain that the MIDlet is not present, true otherwise
 */
bool AutoStarter::isMIDletPresent(const std::wstring& aUid)
{
    RApaLsSession apaSession;
    TInt err = apaSession.Connect();
    if (KErrNone != err)
    {
        // Cannot check presence from AppArc, try to start the MIDlet anyway
        ELOG1(EJavaCaptain,
              "AutoStarter::isMIDletPresent: RApaLsSession Connect error %d", err);
        return true;
    }
    CleanupClosePushL(apaSession);

    TUid appUid;
    Uid javaUid(aUid);
    err = uidToTUid(javaUid, appUid);
    if (KErrNone != err)
    {
        WLOG1(EJavaCaptain,
            "AutoStarter::isMIDletPresent: Cannot convert %S to TUid", aUid.c_str());
        CleanupStack::PopAndDestroy(&apaSession); // apaSession
        return true;
    }

    TUid appTypeUid;
    err = apaSession.GetAppType(appTypeUid, appUid);
    if (KErrNone != err)
    {
        if (KErrNotFound == err)
        {
            // The application is not present
            WLOG1(EJavaCaptain,
                "AutoStarter::isMIDletPresent: trying to start MIDlet %S that is not present",
                aUid.c_str());
            CleanupStack::PopAndDestroy(&apaSession); // apaSession
            return false;
        }

        // Cannot check presence from AppArc, try to start the MIDlet anyway
        ELOG1(EJavaCaptain,
            "AutoStarter::isMIDletPresent: RApaLsSession GetAppType error %d", err);
        CleanupStack::PopAndDestroy(&apaSession); // apaSession
        return true;
    }
    else if (appTypeUid.iUid != KMidletApplicationTypeUid)
    {
        // The application is present but it is NOT a MIDlet
        WLOG1(EJavaCaptain,
            "AutoStarter::isMIDletPresent: tried to start application %S that is not MIDlet",
            aUid.c_str());
        CleanupStack::PopAndDestroy(&apaSession); // apaSession
        return false;
    }

    CleanupStack::PopAndDestroy(&apaSession); // apaSession
    return true;
}


} // namespace captain
} // namespace java
