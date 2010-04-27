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
* Description:
*
*/


package com.nokia.mj.impl.installer.storagehandler;

import com.nokia.mj.impl.installer.utils.InstallerException;
import com.nokia.mj.impl.installer.utils.Log;
import com.nokia.mj.impl.installer.utils.PlatformUid;
import com.nokia.mj.impl.storage.StorageAttribute;
import com.nokia.mj.impl.storage.StorageEntry;
import com.nokia.mj.impl.storage.StorageFactory;
import com.nokia.mj.impl.storage.StorageNames;
import com.nokia.mj.impl.storage.StorageSession;
import com.nokia.mj.impl.utils.Uid;

import java.util.Vector;

/**
 * OtaStatusHandler takes care of storing OTA status notifications
 * persistently. MIDP spec requires that when OTA status notification
 * sending fails, it must be retried certain number of times after
 * certain time period. For this reason the OtaStatusHandler must
 * store the notifications that need to be sent later to JavaStorage.
 * OtaStatusHandler must have its own JavaStorage session because
 * in installation/uninstallation failure case the storage changes
 * will be rolled back, but OtaStatusHandler must still be able to
 * commit its storage changes.
 *
 * Because JavaStorage limits the number of concurrent sessions to
 * one for each database, the OtaStatusHandler uses a separate
 * JavaOtaStorage database for storing the OTA status notifications.
 *
 * @author Nokia Corporation
 * @version $Rev: 9041 $ $Date: 2009-12-07 18:36:08 +0200 (Mon, 07 Dec 2009) $
 */
public class OtaStatusHandler
{

    /**
     * Return pending OTA status notifications from storage.
     */
    synchronized public OtaStatusNotification[] getNotifications()
    {
        StorageSession session = null;
        try
        {
            OtaStatusNotification[] notifications = null;
            session = openSession();
            StorageEntry[] entries = session.search(
                                         StorageNames.OTA_STATUS_TABLE, new StorageEntry());
            if (entries != null)
            {
                notifications = new OtaStatusNotification[entries.length];
                for (int i = 0; i < notifications.length; i++)
                {
                    StorageEntry entry = entries[i];
                    Uid uid = PlatformUid.createUid
                              (entry.getAttribute(StorageNames.ID).getValue());
                    long creationTime = Long.parseLong
                                        (entry.getAttribute(StorageNames.CREATION_TIME).getValue());
                    int type = Integer.parseInt
                               (entry.getAttribute(StorageNames.TYPE).getValue());
                    int otaCode = Integer.parseInt
                                  (entry.getAttribute(StorageNames.OTA_CODE).getValue());
                    String url = entry.getAttribute(StorageNames.URL).getValue();
                    long latestRetryTime = Long.parseLong
                                           (entry.getAttribute(StorageNames.LATEST_RETRY_TIME).getValue());
                    int retryCount = Integer.parseInt
                                     (entry.getAttribute(StorageNames.RETRY_COUNT).getValue());
                    notifications[i] =
                        new OtaStatusNotification
                    (uid, creationTime, type, otaCode, url,
                     latestRetryTime, retryCount);
                }
            }
            closeSession(session);
            session = null;
            return sortNotifications(notifications);
        }
        catch (Throwable t)
        {
            rollbackSession(session);
            session = null;
            InstallerException.internalError
            ("Error while getting OtaStatusNotifications", t);
        }
        return null;
    }

    /**
     * Create OTA status notification to storage.
     */
    synchronized public void addNotification(OtaStatusNotification aNotification)
    {
        if (aNotification == null)
        {
            return;
        }
        StorageSession session = null;
        try
        {
            session = openSession();
            StorageEntry entry = new StorageEntry();
            entry.addAttribute
            (new StorageAttribute
             (StorageNames.ID, getUidString(aNotification.getUid())));
            entry.addAttribute
            (new StorageAttribute
             (StorageNames.CREATION_TIME, Long.toString(aNotification.getCreationTime())));
            entry.addAttribute
            (new StorageAttribute
             (StorageNames.TYPE, Integer.toString(aNotification.getType()),
              StorageAttribute.INT_TYPE));
            entry.addAttribute
            (new StorageAttribute
             (StorageNames.OTA_CODE, Integer.toString(aNotification.getOtaCode()),
              StorageAttribute.INT_TYPE));
            entry.addAttribute
            (new StorageAttribute
             (StorageNames.URL, aNotification.getUrl()));
            entry.addAttribute
            (new StorageAttribute
             (StorageNames.LATEST_RETRY_TIME,
              Long.toString(aNotification.getLatestRetryTime())));
            entry.addAttribute
            (new StorageAttribute
             (StorageNames.RETRY_COUNT,
              Integer.toString(aNotification.getRetryCount()),
              StorageAttribute.INT_TYPE));
            session.write(StorageNames.OTA_STATUS_TABLE, entry);
            closeSession(session);
            session = null;
        }
        catch (Throwable t)
        {
            rollbackSession(session);
            session = null;
            InstallerException.internalError
            ("Error while adding OtaStatusNotification", t);
        }
    }

    /**
     * Remove OTA status notification from storage. Notification
     * is identified with suite uid and notification creation time.
     */
    synchronized public void removeNotification(OtaStatusNotification aNotification)
    {
        if (aNotification == null)
        {
            return;
        }
        StorageSession session = null;
        try
        {
            session = openSession();
            StorageEntry entry = new StorageEntry();
            entry.addAttribute
            (new StorageAttribute
             (StorageNames.ID, getUidString(aNotification.getUid())));
            entry.addAttribute
            (new StorageAttribute
             (StorageNames.CREATION_TIME, Long.toString(aNotification.getCreationTime())));
            session.remove(StorageNames.OTA_STATUS_TABLE, entry);
            closeSession(session);
            session = null;
        }
        catch (Throwable t)
        {
            rollbackSession(session);
            session = null;
            InstallerException.internalError
            ("Error while removing OtaStatusNotification", t);
        }
    }

    /**
     * Updates latest retry time and retry count from given
     * OTA status notification to storage. Notification
     * is identified with suite uid and notification creation time.
     */
    synchronized public void updateNotification(OtaStatusNotification aNotification)
    {
        if (aNotification == null)
        {
            return;
        }
        StorageSession session = null;
        try
        {
            session = openSession();
            // Create entry with matching fields.
            StorageEntry entryMatch = new StorageEntry();
            entryMatch.addAttribute
            (new StorageAttribute
             (StorageNames.ID, getUidString(aNotification.getUid())));
            entryMatch.addAttribute
            (new StorageAttribute
             (StorageNames.CREATION_TIME, Long.toString(aNotification.getCreationTime())));
            // Create entry with updated fields.
            StorageEntry entryUpdate = new StorageEntry();
            entryUpdate.addAttribute
            (new StorageAttribute
             (StorageNames.LATEST_RETRY_TIME, Long.toString(aNotification.getLatestRetryTime())));
            entryUpdate.addAttribute
            (new StorageAttribute
             (StorageNames.RETRY_COUNT, Integer.toString(aNotification.getRetryCount()),
              StorageAttribute.INT_TYPE));
            // Update entry to storage.
            session.update(StorageNames.OTA_STATUS_TABLE, entryUpdate, entryMatch);
            closeSession(session);
            session = null;
        }
        catch (Throwable t)
        {
            rollbackSession(session);
            session = null;
            InstallerException.internalError
            ("Error while updating OtaStatusNotification", t);
        }
    }

    /**
     * Returns an open StorageSession.
     */
    private StorageSession openSession()
    {
        StorageSession session = StorageFactory.createSession();
        if (session == null)
        {
            InstallerException.internalError("Opening StorageSession failed");
        }
        session.open(StorageNames.JAVA_OTA_DATABASE_NAME);
        return session;
    }

    /**
     * Closes given StorageSession.
     */
    private void closeSession(StorageSession aSession)
    {
        if (aSession == null)
        {
            return;
        }
        aSession.close();
        aSession.destroySession();
    }

    /**
     * Closes given StorageSession.
     */
    private void rollbackSession(StorageSession aSession)
    {
        if (aSession == null)
        {
            return;
        }
        aSession.close();
        aSession.destroySession();
    }

    /**
     * Returns given notifications sorted by Uid and CreationTime.
     */
    private OtaStatusNotification[] sortNotifications(
        OtaStatusNotification[] aNotifications)
    {
        if (aNotifications == null)
        {
            return null;
        }
        // Do insertion sort for the OtaStatusNotifications.
        Vector result = new Vector();
        for (int i = 0; i < aNotifications.length; i++)
        {
            boolean added = false;
            for (int j = 0; j < result.size() && !added; j++)
            {
                if (aNotifications[i].compareTo(
                            (OtaStatusNotification)result.elementAt(j)) > 0)
                {
                    continue;
                }
                result.insertElementAt(aNotifications[i], j);
                added = true;
            }
            if (!added)
            {
                result.addElement(aNotifications[i]);
            }
        }
        // Convert results to OtaStatusNotification array.
        OtaStatusNotification[] notifications =
            new OtaStatusNotification[result.size()];
        result.copyInto(notifications);
        return notifications;
    }

    /**
     * Returns uid string used when storing notification to storage.
     */
    private static String getUidString(Uid aUid)
    {
        String result = null;
        if (aUid == null)
        {
            result = PlatformUid.createUid("0").toString();
        }
        else
        {
            result = aUid.toString();
        }
        return result;
    }
}
