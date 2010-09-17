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

package com.nokia.mj.impl.push;

import com.nokia.mj.impl.storage.StorageAttribute;
import com.nokia.mj.impl.storage.StorageEntry;
import com.nokia.mj.impl.storage.StorageNames;
import com.nokia.mj.impl.storage.StorageSession;
import com.nokia.mj.impl.utils.Uid;

/**
 * Utility methods for handling push registrations.
 */
public final class PushUtil
{
    /**
     * Returns push registrations for specified application or
     * application suite.
     *
     * @param aSession StorageSesion to be used for fetching
     * the push registrations
     * @param aUid either application or application suite Uid
     * @return array of push registrations
     */
    public static PushRegInfo[] getPushRegs(StorageSession aSession, Uid aUid)
    {
        // Find push registrations using application uid.
        PushRegInfo[] pushRegs = getPushRegsForAppUid(aSession, aUid);
        if (pushRegs.length > 0)
        {
            return pushRegs;
        }

        // Push registrations not found, so find application uids using
        // suite uid and then push registrations using application uids.
        Uid[] appUids = getAppUids(aSession, aUid);
        PushRegInfo[][] pushRegArray = new PushRegInfo[appUids.length][];
        int pushRegCount = 0;
        for (int i = 0; i < appUids.length; i++)
        {
            pushRegArray[i] = getPushRegs(aSession, appUids[i]);
            pushRegCount += pushRegArray[i].length;
        }
        pushRegs = new PushRegInfo[pushRegCount];
        int pushRegsIndex = 0;
        for (int i = 0; i < pushRegArray.length; i++)
        {
            for (int j = 0; j < pushRegArray[i].length; j++)
            {
                pushRegs[pushRegsIndex++] = pushRegArray[i][j];
            }
        }
        return pushRegs;
    }

    /**
     * Returns push registrations for specified application.
     *
     * @param aSession StorageSesion to be used for fetching
     * the push registrations
     * @param aUid application Uid
     * @return array of push registrations
     */
    private static PushRegInfo[] getPushRegsForAppUid(StorageSession aSession, Uid aUid)
    {
        PushRegInfo[] pushRegs = new PushRegInfo[0];
        StorageEntry query = new StorageEntry();
        query.addAttribute(
            new StorageAttribute(StorageNames.ID, aUid.getStringValue()));
        StorageEntry[] entries = aSession.search(
            StorageNames.PUSH_REGISTRATIONS_TABLE, query);
        if (entries != null && entries.length > 0)
        {
            pushRegs = new PushRegInfo[entries.length];
            for (int i = 0; i < entries.length; i++)
            {
                Uid uid = Uid.createUid(
                    entries[i].getAttribute(StorageNames.ID).getValue());
                String name = entries[i].getAttribute(StorageNames.NAME).getValue();
                String url = entries[i].getAttribute(StorageNames.URL).getValue();
                String filter = entries[i].getAttribute(StorageNames.FILTER).getValue();
                int regType = Integer.parseInt(
                    entries[i].getAttribute(StorageNames.REGISTRATION_TYPE).getValue());
                pushRegs[i] = new PushRegInfo(uid, name, url, filter, regType);
            }
        }
        return pushRegs;
    }

    /**
     * Returns application Uids for specified application suite.
     *
     * @param aSession StorageSesion to be used for fetching the Uids
     * @param aSuiteUid application suite Uid
     * @return array of application Uids
     */
    private static Uid[] getAppUids(StorageSession aSession, Uid aUid)
    {
        Uid[] uids = new Uid[0];
        StorageEntry query = new StorageEntry();
        query.addAttribute(new StorageAttribute(
                               StorageNames.PACKAGE_ID, aUid.getStringValue()));
        query.addAttribute(new StorageAttribute(StorageNames.ID, ""));
        StorageEntry[] entries =
            aSession.search(StorageNames.APPLICATION_TABLE, query);
        if (entries != null && entries.length > 0)
        {
            uids = new Uid[entries.length];
            for (int i = 0; i < entries.length; i++)
            {
                uids[i] = Uid.createUid(
                    entries[i].getAttribute(StorageNames.ID).getValue());
            }
        }
        return uids;
    }

    /**
     * PushRegInfo contains information of one push registration.
     */
    public static final class PushRegInfo
    {
        /** Dynamic push registration. */
        public static final int DYNAMIC = 0;
        /** Static push registration. */
        public static final int STATIC = 1;

        /** Application Uid. */
        private Uid iUid = null;
        /** Application name. */
        private String iName = null;
        /** Push registration URL */
        private String iUrl = null;
        /** Filter for incoming messages/connections. */
        private String iFilter = null;
        /** Registration type. Either DYNAMIC or STATIC. */
        private int iRegType = DYNAMIC;

        PushRegInfo(
            Uid aUid, String aName, String aUrl, String aFilter, int aRegType)
        {
            iUid = aUid;
            iName =  aName;
            iUrl = aUrl;
            iFilter = aFilter;
            iRegType = aRegType;
        }

        /** Returns application Uid. */
        public Uid getUid()
        {
            return iUid;
        }

        /** Returns application name. */
        public String getName()
        {
            return iName;
        }

        /** Returns push registration URL */
        public String getUrl()
        {
            return iUrl;
        }

        /** Returns filter for incoming messages/connections. */
        public String getFilter()
        {
            return iFilter;
        }

        /** Returns registration type. Either DYNAMIC or STATIC. */
        public int getRegType()
        {
            return iRegType;
        }
    }
}
