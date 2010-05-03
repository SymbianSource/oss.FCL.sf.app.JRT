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
* Description:
*
*/


package com.nokia.mj.test.rms;
import com.nokia.mj.impl.storage.*;
import com.nokia.mj.impl.comms.*;

public class AppLauncher
{
    private static final int PLUGIN_ID_RTC = 1;
    private static final int RTC_MSG_ID_LAUNCH_APPLICATION_REQ      = 101;
    private static final int RTC_LAUNCH_TYPE_NORMAL_C               = 0;
    private static final int RTC_LAUNCH_OPTIONS_RUNNING_IND_REQ_C   = 1;
    private static final int RTC_LAUNCH_OPTIONS_TERMINATE_IND_REQ_C = 2;

    // blocks until app exits
    public static void runApp(String aMainClass, String aVendor, String aSuite)
    {
        String suiteUid = AppLauncher.getSuiteUid(aSuite, aVendor);
        String uid = AppLauncher.getAppUid(suiteUid, aMainClass);
        launchApp(uid, RTC_LAUNCH_OPTIONS_TERMINATE_IND_REQ_C);
    }

    // block until app is running
    public static void launchApp(String aMainClass, String aVendor, String aSuite)
    {
        String suiteUid = AppLauncher.getSuiteUid(aSuite, aVendor);
        String uid = AppLauncher.getAppUid(suiteUid, aMainClass);
        launchApp(uid, RTC_LAUNCH_OPTIONS_RUNNING_IND_REQ_C);
    }


    private static String getSuiteUid(String aSuite, String aVendor)
    {
        StorageSession session = null;
        String uid = "";
        try
        {
            StorageAttribute attr = new StorageAttribute(StorageNames.PACKAGE_NAME, aSuite);
            StorageEntry query = new StorageEntry();
            query.addAttribute(attr);

            attr = new StorageAttribute(StorageNames.VENDOR, aVendor);
            query.addAttribute(attr);

            attr = new StorageAttribute(StorageNames.ID, "");
            query.addAttribute(attr);

            session = StorageFactory.createSession();
            session.open();
            StorageEntry[] receivedEntries = session.search(StorageNames.APPLICATION_PACKAGE_TABLE, query);
            uid = receivedEntries[0].getAttribute(StorageNames.ID).getValue();
        }
        finally
        {
            if (session != null)
            {
                session.close();
                session.destroySession();
            }
        }
        return uid;
    }

    private static String getAppUid(String aSuiteUid, String aMainClass)
    {
        StorageSession session = null;
        String uid = "";
        try
        {
            StorageAttribute attr = new StorageAttribute(StorageNames.PACKAGE_ID, aSuiteUid);
            StorageEntry query = new StorageEntry();
            query.addAttribute(attr);

            attr = new StorageAttribute(StorageNames.MAIN_CLASS, aMainClass);
            query.addAttribute(attr);

            attr = new StorageAttribute(StorageNames.ID, "");
            query.addAttribute(attr);

            session = StorageFactory.createSession();
            session.open();
            StorageEntry[] receivedEntries = session.search(StorageNames.APPLICATION_TABLE , query);
            uid = receivedEntries[0].getAttribute(StorageNames.ID).getValue();
        }
        finally
        {
            if (session != null)
            {
                session.close();
                session.destroySession();
            }
        }
        return uid;
    }

    private static void launchApp(String aUid, int aLaunchOption)
    {
        CommsMessage msg = new CommsMessage();
        msg.setMessageId(RTC_MSG_ID_LAUNCH_APPLICATION_REQ);
        msg.setModuleId(PLUGIN_ID_RTC);

        msg.write(aUid);
        msg.write(RTC_LAUNCH_TYPE_NORMAL_C);
        msg.write(aLaunchOption);
        msg.write("midp");
        msg.write(""); // application parameters
        msg.write(""); // runtime parameters

        CommsEndpoint comms = CommsEndpoint.find("InstallerJavaCaptain");
        comms.sendReceive(msg, CommsEndpoint.WAIT_FOR_EVER);
    }


}
