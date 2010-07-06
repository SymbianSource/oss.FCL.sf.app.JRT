/*
* Copyright (c) 2008-2010 Nokia Corporation and/or its subsidiary(-ies).
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
import com.nokia.mj.impl.utils.Attribute;
import com.nokia.mj.impl.utils.Tokenizer;
import com.nokia.mj.impl.utils.Uid;
import com.nokia.mj.impl.utils.Version;

import java.util.Enumeration;
import java.util.Hashtable;
import java.util.Vector;

/**
 * StorageHandler offers JavaStorage services for JavaInstaller.
 */
public class StorageHandler
{

    /** StorageSession instance. */
    private StorageSession iSession = null;
    private boolean iTransactionExists = false;

    /**
     * Default constructor.
     */
    public StorageHandler()
    {
        try
        {
            iSession = StorageFactory.createSession();
            iSession.open();
        }
        catch (Throwable t)
        {
            InstallerException.internalError
            ("Error while constructing StorageHandler", t);
        }
    }

    /**
     * Closes this object.
     */
    public void close()
    {
        if (iSession != null)
        {
            try
            {
                iSession.close();
                iSession.destroySession();
                iSession = null;
            }
            catch (Throwable t)
            {
                InstallerException.internalError
                ("Error while closing StorageHandler", t);
            }
        }
    }

    /**
     * Start a new Storage session for storage update.
     * This method creates a new JavaStorage transaction.
     * @throws InstallerException if session opening fails.
     */
    public void startSession()
    {
        try
        {
            checkSession();
            iSession.startTransaction();
            iTransactionExists = true;
        }
        catch (Throwable t)
        {
            InstallerException.internalError
            ("Error while starting Storage session", t);
        }
    }

    /**
     * Commit a successful session.
     * @throws InstallerException if committing session fails.
     */
    public void commitSession()
    {
        try
        {
            checkSession();
            checkTransaction();
            iSession.commitTransaction();
            iTransactionExists = false;
            close();
        }
        catch (Throwable t)
        {
            InstallerException.internalError
            ("Error while committing Storage session", t);
        }
    }

    /**
     * Rollback a session.
     * @throws InstallerException if session rollback fails.
     */
    public void rollbackSession()
    {
        try
        {
            checkSession();
            checkTransaction();
            iSession.rollbackTransaction();
            iTransactionExists = false;
            close();
        }
        catch (Throwable t)
        {
            InstallerException.internalError
            ("Error while rolling back Storage session", t);
        }
    }

    /**
     * Returns instance of StorageSession, or null if session
     * does not exist.
     */
    public StorageSession getSession()
    {
        return iSession;
    }

    /**
     * Get string which contains listing of all entries
     * from given storage table.
     */
    public String getStorageEntriesString(String aTableName)
    {
        StringBuffer result = new StringBuffer();
        try
        {
            result.append("Table ").append(aTableName).append("\n");
            StorageEntry query = new StorageEntry();
            StorageEntry[] entries = iSession.search(aTableName, query);
            if (entries == null)
            {
                return null;
            }
            for (int i = 0; i < entries.length; i++)
            {
                StorageEntry entry = entries[i];
                result.append(entry.toString()).append("\n");
            }
        }
        catch (Throwable t)
        {
            InstallerException.internalError
            ("Error while getting storage table " + aTableName, t);
        }
        return result.toString();
    }

    /**
     * Get uids for all existing suites.
     */
    public Uid[] getSuiteUids()
    {
        try
        {
            checkSession();
            StorageEntry query = new StorageEntry();
            query.addAttribute(new StorageAttribute(StorageNames.ID, ""));
            StorageEntry[] entries = iSession.search(
                                         StorageNames.APPLICATION_PACKAGE_TABLE, query);
            if (entries == null)
            {
                return null;
            }
            Uid[] uids = new Uid[entries.length];
            for (int i = 0; i < entries.length; i++)
            {
                StorageEntry entry = entries[i];
                uids[i] = PlatformUid.createUid
                          (entry.getAttribute(StorageNames.ID).getValue());
            }
            return uids;
        }
        catch (Throwable t)
        {
            InstallerException.internalError
            ("Error while getting suite uids", t);
        }
        return null;
    }

    /**
     * Get suite uid. Given uid parameter can either be an
     * application uid or suite uid. If it is a suite uid,
     * itself is returned. If no matching suite is found,
     * returns null.
     */
    public Uid getSuiteUid(Uid aUid)
    {
        try
        {
            checkSession();
            // Check if aUid is an application uid.
            StorageEntry query = new StorageEntry();
            query.addAttribute(new StorageAttribute
                               (StorageNames.ID, aUid.getStringValue()));
            query.addAttribute(new StorageAttribute(
                                   StorageNames.PACKAGE_ID, ""));
            StorageEntry[] entries = iSession.search(
                                         StorageNames.APPLICATION_TABLE, query);
            if (entries != null && entries.length > 0)
            {
                // Uid is an application uid, return suite uid column.
                return PlatformUid.createUid
                       (entries[0].getAttribute(StorageNames.PACKAGE_ID).getValue());
            }
            // Check if aUid is a suite uid.
            query = new StorageEntry();
            query.addAttribute(new StorageAttribute
                               (StorageNames.ID, aUid.getStringValue()));
            entries = iSession.search(
                          StorageNames.APPLICATION_PACKAGE_TABLE, query);
            if (entries != null && entries.length > 0)
            {
                // Uid is a suite uid, return itself.
                return aUid;
            }
        }
        catch (Throwable t)
        {
            InstallerException.internalError
            ("Error while getting suite uid", t);
        }
        return null;
    }

    /**
     * Get application uids that belong to given suite.
     * Note that this method does not guarantee the order in which
     * application uids are returned.
     * If no matching suite is found, returns null.
     */
    public Uid[] getAppUids(Uid aUid)
    {
        try
        {
            checkSession();
            Uid[] uids = null;
            StorageEntry query = new StorageEntry();
            query.addAttribute(new StorageAttribute
                               (StorageNames.PACKAGE_ID, aUid.getStringValue()));
            query.addAttribute(new StorageAttribute(StorageNames.ID, ""));
            StorageEntry[] entries = iSession.search(
                                         StorageNames.APPLICATION_TABLE, query);
            if (entries != null && entries.length > 0)
            {
                uids = new Uid[entries.length];
                for (int i = 0; i < entries.length; i++)
                {
                    uids[i] = PlatformUid.createUid
                              (entries[i].getAttribute(StorageNames.ID).getValue());
                }
            }
            return uids;
        }
        catch (Throwable t)
        {
            InstallerException.internalError
            ("Error while getting application uids", t);
        }
        return null;
    }

    /**
     * Reads suite info from JavaStorage. Retrieves info basing
     * on uid field in the given SuiteInfo. If uid field is not present,
     * then info is fetched using Name and Vendor fields.
     * If matching suite is not found from storage, returns false.
     */
    public boolean readSuiteInfo(SuiteInfo aSuiteInfo)
    {
        try
        {
            checkSession();
            if (aSuiteInfo == null)
            {
                return false;
            }
            boolean result = false;
            if (aSuiteInfo.getUid() == null)
            {
                // Fetch suite info basing on name and vendor.
                if (aSuiteInfo.getName() == null ||
                        aSuiteInfo.getVendor() == null)
                {
                    Log.logError
                    ("StorageHandler.readSuiteInfo: uid or name " +
                     "and vendor not found from SuiteInfo");
                    return result;
                }
                StorageEntry query = new StorageEntry();
                query.addAttribute(new StorageAttribute
                                   (StorageNames.PACKAGE_NAME,
                                    aSuiteInfo.getName()));
                query.addAttribute(new StorageAttribute
                                   (StorageNames.VENDOR,
                                    aSuiteInfo.getVendor()));
                query.addAttribute(new StorageAttribute(StorageNames.ID, ""));
                StorageEntry[] entries = iSession.search(
                                             StorageNames.APPLICATION_PACKAGE_TABLE, query);
                if (entries != null && entries.length > 0)
                {
                    // Found matching suite.
                    aSuiteInfo.setUid(PlatformUid.createUid
                                      (entries[0].getAttribute(
                                           StorageNames.ID).getValue()));
                }
            }
            if (aSuiteInfo.getUid() != null)
            {
                // Fetch suite info basing on uid.
                result = readSuiteInfoByUid(aSuiteInfo);
            }
            return result;
        }
        catch (Throwable t)
        {
            InstallerException.internalError
            ("Error while reading suite info", t);
        }
        return false;
    }

    /**
     * Writes suite info to JavaStorage.
     */
    public void writeSuiteInfo(SuiteInfo aSuiteInfo)
    {
        try
        {
            checkSession();
            checkTransaction();
            // Write StorageNames.APPLICATION_PACKAGE_TABLE table.
            StorageEntry entry = new StorageEntry();
            entry.addAttribute
            (new StorageAttribute
             (StorageNames.ID, aSuiteInfo.getUid().getStringValue()));
            entry.addAttribute
            (new StorageAttribute
             (StorageNames.PACKAGE_NAME, aSuiteInfo.getName()));
            entry.addAttribute
            (new StorageAttribute
             (StorageNames.VENDOR, aSuiteInfo.getVendor()));
            entry.addAttribute
            (new StorageAttribute
             (StorageNames.VERSION, aSuiteInfo.getVersion().toString()));
            entry.addAttribute
            (new StorageAttribute
             (StorageNames.ROOT_PATH, aSuiteInfo.getRootDir()));
            entry.addAttribute
            (new StorageAttribute
             (StorageNames.MEDIA_ID, Integer.toString(aSuiteInfo.getMediaId()),
              StorageAttribute.INT_TYPE));
            entry.addAttribute
            (new StorageAttribute
             (StorageNames.INITIAL_SIZE, Integer.toString(aSuiteInfo.getInitialSize()),
              StorageAttribute.INT_TYPE));
            entry.addAttribute
            (new StorageAttribute
             (StorageNames.JAD_PATH, aSuiteInfo.getJadPath()));
            entry.addAttribute
            (new StorageAttribute
             (StorageNames.JAR_PATH, aSuiteInfo.getJarPath()));
            entry.addAttribute
            (new StorageAttribute
             (StorageNames.JAD_URL, aSuiteInfo.getJadUrl()));
            entry.addAttribute
            (new StorageAttribute
             (StorageNames.JAR_URL, aSuiteInfo.getJarUrl()));
            entry.addAttribute
            (new StorageAttribute
             (StorageNames.ACCESS_POINT, aSuiteInfo.getAccessPoint()));
            entry.addAttribute
            (new StorageAttribute
             (StorageNames.CONTENT_INFO, Integer.toString(aSuiteInfo.getContentInfo()),
              StorageAttribute.INT_TYPE));
            entry.addAttribute
            (new StorageAttribute
             (StorageNames.CONTENT_ID, aSuiteInfo.getContentId()));
            iSession.write(StorageNames.APPLICATION_PACKAGE_TABLE, entry);
            // Write StorageNames.APPLICATION_PACKAGE_ATTRIBUTES_TABLE table.
            writeAttributes(aSuiteInfo.getUid(), aSuiteInfo.getAttributes());
            // Write StorageNames.APPLICATION_TABLE table.
            Vector apps = aSuiteInfo.getApplications();
            if (apps != null)
            {
                for (int i = 0; i < apps.size(); i++)
                {
                    writeAppInfo(aSuiteInfo.getUid(),
                                 (ApplicationInfo)apps.elementAt(i));
                }
            }
            // Update StorageNames.MIDP_PACKAGE_TABLE table.
            // This table is shared with security framework
            // so we must have a special handling for it.
            // Create a new entry to be inserted or updated.
            entry = new StorageEntry();
            entry.addAttribute
            (new StorageAttribute
             (StorageNames.ID, aSuiteInfo.getUid().getStringValue()));
            entry.addAttribute
            (new StorageAttribute
             (StorageNames.ON_SCREEN_KEYPAD,
              Integer.toString(aSuiteInfo.getOnScreenKeypad()),
              StorageAttribute.INT_TYPE));
            // Check if an entry already exists.
            StorageEntry matchEntry = new StorageEntry();
            matchEntry.addAttribute(new StorageAttribute(StorageNames.ID,
                                    aSuiteInfo.getUid().getStringValue()));
            StorageEntry[] entries = iSession.search(
                                         StorageNames.MIDP_PACKAGE_TABLE, matchEntry);
            if (entries != null && entries.length > 0)
            {
                // Entry exists, update it.
                iSession.update(StorageNames.MIDP_PACKAGE_TABLE, entry, matchEntry);
            }
            else
            {
                // Entry does not exist, create it.
                iSession.write(StorageNames.MIDP_PACKAGE_TABLE, entry);
            }
            // Update StorageNames.PREINSTALL_TABLE table.
            writePreinstallState(aSuiteInfo);
        }
        catch (Throwable t)
        {
            InstallerException.internalError
            ("Error while writing suite info", t);
        }
    }

    /**
     * Updates changed parts of suite info to JavaStorage.
     */
    public void updateSuiteInfo(SuiteInfo aOldSuiteInfo, SuiteInfo aNewSuiteInfo)
    {
        try
        {
            checkSession();
            checkTransaction();
            removeSuiteInfo(aOldSuiteInfo, true);
            writeSuiteInfo(aNewSuiteInfo);
        }
        catch (Throwable t)
        {
            InstallerException.internalError
            ("Error while updating suite info", t);
        }
    }

    /**
     * Removes info from JavaStorage for given suite uid.
     */
    public void removeSuiteInfo(SuiteInfo aSuiteInfo)
    {
        try
        {
            removeSuiteInfo(aSuiteInfo, false);
        }
        catch (Throwable t)
        {
            InstallerException.internalError
            ("Error while removing suite info", t);
        }
    }

    /**
     * Allocates a free uid which is not in use in storage.
     *
     * @param aPreviousUid if null allocates a random uid,
     * if not null allocates next free uid
     * @return a free uid, or null if uid cannot be allocated
     */
    public Uid allocateUid(Uid aPreviousUid)
    {
        try
        {
            checkSession();
            Uid uid = PlatformUid.generateUid(aPreviousUid, null);
            if (uidInUse(uid))
            {
                Uid startUid = uid;
                do
                {
                    uid = PlatformUid.generateUid(uid, null);
                    if (uid.equals(startUid))
                    {
                        InstallerException.internalError
                        ("No more free uids.");
                    }
                }
                while (uidInUse(uid));
            }
            return uid;
        }
        catch (Throwable t)
        {
            InstallerException.internalError
            ("Error while allocating uid", t);
        }
        return null;
    }

    /**
     * Allocates a free uid which is not in use in storage.
     *
     * @param aUidSeed seed used for allocating uid
     * @return a free uid, or null if uid cannot be allocated
     */
    public Uid allocateUid(String aUidSeed)
    {
        Uid uid = PlatformUid.generateUid(null, aUidSeed);
        if (uidInUse(uid))
        {
            uid = allocateUid(uid);
        }
        return uid;
    }

    /**
     * Removes all data from all JavaStorage tables.
     * @return true if removal was successful, false if removal failed.
     * @throws StorageException if JavaStorage throws it.
     */
    public static boolean removeAllData()
    {
        boolean result = true;
        // Remove all data from JavaStorage tables.
        String[] tableNames =
        {
            StorageNames.APPLICATION_PACKAGE_TABLE,
            StorageNames.APPLICATION_TABLE,
            StorageNames.APPLICATION_PACKAGE_ATTRIBUTES_TABLE,
            StorageNames.MIDP_PACKAGE_TABLE,
            StorageNames.MIDP_PERMISSIONS_TABLE,
            StorageNames.MIDP_FUNC_GRP_SETTINGS_TABLE,
            StorageNames.PUSH_REGISTRATIONS_TABLE,
            StorageNames.ALARM_REGISTRATIONS_TABLE,
            StorageNames.RUNTIME_SETTINGS_TABLE,
            StorageNames.PREINSTALL_TABLE,
        };
        StorageSession session = StorageFactory.createSession();
        session.open();
        session.startTransaction();
        for (int i = 0; i < tableNames.length; i++)
        {
            if (session.remove(tableNames[i], new StorageEntry()) == -1)
            {
                Log.logError("Removing data from JavaStorage table " +
                             tableNames[i] + " failed");
                result = false;
            }
            else
            {
                Log.log("Removed data from JavaStorage table " + tableNames[i]);
            }
        }
        session.commitTransaction();
        session.close();
        session.destroySession();
        // Remove all data from JavaOtaStorage tables.
        String tableName = StorageNames.OTA_STATUS_TABLE;
        session = StorageFactory.createSession();
        session.open(StorageNames.JAVA_OTA_DATABASE_NAME);
        if (session.remove(tableName, new StorageEntry()) == -1)
        {
            Log.logError("Removing data from JavaOtaStorage table " +
                         tableName + " failed");
            result = false;
        }
        else
        {
            Log.log("Removed data from JavaOtaStorage table " + tableName);
        }
        session.close();
        session.destroySession();
        return result;
    }

    /**
     * Returns storage attribute value from given entry, or null
     * if attribute does not exist.
     */
    static String getAttributeValue(StorageEntry aEntry, String aName)
    {
        if (aEntry == null)
        {
            return null;
        }
        StorageAttribute attr = aEntry.getAttribute(aName);
        if (attr != null)
        {
            return attr.getValue();
        }
        return null;
    }

    /**
     * Returns true if given uid is already in use in storage.
     */
    private boolean uidInUse(Uid aUid)
    {
        checkSession();
        StorageEntry query = new StorageEntry();
        query.addAttribute(new StorageAttribute(
                               StorageNames.ID, aUid.getStringValue()));
        // Check if aUid is a suite uid.
        StorageEntry[] entries = iSession.search(
                                     StorageNames.APPLICATION_PACKAGE_TABLE, query);
        if (entries != null && entries.length > 0)
        {
            return true;
        }
        // Check if aUid is an application uid.
        entries = iSession.search(StorageNames.APPLICATION_TABLE, query);
        if (entries != null && entries.length > 0)
        {
            return true;
        }
        return false;
    }

    /**
     * Reads suite info from JavaStorage. Retrieves info basing
     * on uid field in the given SuiteInfo.
     * If matching suite is not found from storage, returns false.
     */
    private boolean readSuiteInfoByUid(SuiteInfo aSuiteInfo)
    {
        if (aSuiteInfo == null)
        {
            return false;
        }
        boolean result = false;
        // Read StorageNames.APPLICATION_PACKAGE_TABLE table.
        StorageEntry query = new StorageEntry();
        query.addAttribute(new StorageAttribute(StorageNames.ID,
                                                aSuiteInfo.getUid().getStringValue()));
        StorageEntry[] entries = iSession.search(
                                     StorageNames.APPLICATION_PACKAGE_TABLE, query);
        if (entries != null && entries.length > 0)
        {
            result = true;
            // Fill suite info.
            StorageEntry entry = entries[0];
            aSuiteInfo.setName(getAttributeValue(entry, StorageNames.PACKAGE_NAME));
            aSuiteInfo.setVendor(getAttributeValue(entry, StorageNames.VENDOR));
            aSuiteInfo.setVersion(Version.getVersion(
                                      getAttributeValue(entry, StorageNames.VERSION)));
            aSuiteInfo.setRootDir(getAttributeValue(entry, StorageNames.ROOT_PATH));
            aSuiteInfo.setMediaId(Integer.parseInt(
                                      getAttributeValue(entry, StorageNames.MEDIA_ID)));
            aSuiteInfo.setInitialSize(Integer.parseInt(
                                          getAttributeValue(entry, StorageNames.INITIAL_SIZE)));
            aSuiteInfo.setJadPath(getAttributeValue(entry, StorageNames.JAD_PATH));
            aSuiteInfo.setJarPath(getAttributeValue(entry, StorageNames.JAR_PATH));
            aSuiteInfo.setJadUrl(getAttributeValue(entry, StorageNames.JAD_URL));
            aSuiteInfo.setJarUrl(getAttributeValue(entry, StorageNames.JAR_URL));
            aSuiteInfo.setAccessPoint(getAttributeValue(entry, StorageNames.ACCESS_POINT));
            aSuiteInfo.setContentInfo(Integer.parseInt(
                                          getAttributeValue(entry, StorageNames.CONTENT_INFO)));
            aSuiteInfo.setContentId(getAttributeValue(entry, StorageNames.CONTENT_ID));
            // Read attributes.
            aSuiteInfo.setAttributes(readAttributes(aSuiteInfo.getUid()));
            // Check from attributes if suite is trusted.
            // This check does not work if the MIDlet-Jar-RSA-SHA1
            // attribute is specified in Manifest for untrusted
            // application: in upgrade the existing application
            // is assumed to be trusted! Trust info must be initialized
            // from Security framework after SuiteInfo has been read.
            if (aSuiteInfo.getAttribute("MIDlet-Jar-RSA-SHA1") != null)
            {
                aSuiteInfo.setTrusted(true);
            }
            // Read application uids.
            Uid[] appUids = getAppUids(aSuiteInfo.getUid());
            if (appUids != null)
            {
                for (int i = 0; i < appUids.length; i++)
                {
                    ApplicationInfo appInfo = readAppInfo(appUids[i]);
                    aSuiteInfo.addApplication(appInfo);
                }
            }
            sortApps(aSuiteInfo);
        }

        if (result)
        {
            // Read StorageNames.MIDP_PACKAGE_TABLE table.
            result = false;
            query = new StorageEntry();
            query.addAttribute(new StorageAttribute(StorageNames.ID,
                                                    aSuiteInfo.getUid().getStringValue()));
            query.addAttribute(new StorageAttribute(
                                   StorageNames.ON_SCREEN_KEYPAD, ""));
            entries = iSession.search(StorageNames.MIDP_PACKAGE_TABLE, query);
            if (entries != null && entries.length > 0)
            {
                result = true;
                // Fill suite info.
                StorageEntry entry = entries[0];
                aSuiteInfo.setOnScreenKeypad(Integer.parseInt(
                                                 getAttributeValue(entry, StorageNames.ON_SCREEN_KEYPAD)));
            }
            else
            {
                Log.logError("Reading " + StorageNames.MIDP_PACKAGE_TABLE +
                             " table failed, no entries found");
            }
        }

        if (result)
        {
            // Read StorageNames.PREINSTALL_TABLE table.
            //result = false;
            aSuiteInfo.iPreinstallState = readPreinstallState(aSuiteInfo);
            if (aSuiteInfo.iPreinstallState >= 0)
            {
                result = true;
            }
            else
            {
                Log.logWarning("Reading " + StorageNames.PREINSTALL_TABLE +
                               " table failed, no entries found");
            }
        }

        return result;
    }

    /**
     * Reads suite attributes from JavaStorage. Retrieves info basing
     * on given suite uid.
     * If matching suite is not found from storage, returns null.
     */
    private Hashtable readAttributes(Uid aUid)
    {
        if (aUid == null)
        {
            return null;
        }
        Hashtable attrs = null;
        StorageEntry query = new StorageEntry();
        query.addAttribute(new StorageAttribute(
                               StorageNames.ID, aUid.getStringValue()));
        StorageEntry[] entries = iSession.search(
                                     StorageNames.APPLICATION_PACKAGE_ATTRIBUTES_TABLE, query);
        if (entries != null)
        {
            attrs = new Hashtable();
            for (int i = 0; i < entries.length; i++)
            {
                StorageEntry entry = entries[i];
                String name = getAttributeValue(entry, StorageNames.NAME);
                String value = getAttributeValue(entry, StorageNames.VALUE);
                String trustedStr = getAttributeValue(entry, StorageNames.TRUSTED);
                boolean trusted = Integer.parseInt(trustedStr) > 0;
                if (value == null)
                {
                    Log.logWarning("Attribute " + name + " has empty value");
                    value = "";
                }
                attrs.put(name, new Attribute(name, value, trusted));
            }
        }
        return attrs;
    }

    /**
     * Writes suite attributes to JavaStorage.
     */
    private void writeAttributes(Uid aUid, Hashtable aAttrs)
    {
        if (aUid == null || aAttrs == null)
        {
            return;
        }
        StorageEntry entry = null;
        Enumeration e = aAttrs.elements();
        while (e.hasMoreElements())
        {
            Attribute attr = (Attribute)e.nextElement();
            entry = new StorageEntry();
            entry.addAttribute(new StorageAttribute(
                                   StorageNames.ID, aUid.getStringValue()));
            entry.addAttribute(new StorageAttribute(
                                   StorageNames.NAME, attr.getName()));
            entry.addAttribute(new StorageAttribute(
                                   StorageNames.VALUE, attr.getValue()));
            entry.addAttribute(new StorageAttribute(
                                   StorageNames.TRUSTED, (attr.isTrusted()? "1": "0"),
                                   StorageAttribute.INT_TYPE));
            iSession.write(StorageNames.APPLICATION_PACKAGE_ATTRIBUTES_TABLE, entry);
        }
    }

    /**
     * Reads application info from JavaStorage. Retrieves info basing
     * on given application uid.
     * If matching application is not found from storage, returns null.
     */
    private ApplicationInfo readAppInfo(Uid aUid)
    {
        if (aUid == null)
        {
            return null;
        }
        ApplicationInfo appInfo = null;
        StorageEntry query = new StorageEntry();
        query.addAttribute(new StorageAttribute(
                               StorageNames.ID, aUid.getStringValue()));
        StorageEntry[] entries = iSession.search(
                                     StorageNames.APPLICATION_TABLE, query);
        if (entries != null)
        {
            for (int i = 0; i < entries.length; i++)
            {
                StorageEntry entry = entries[i];
                String autoStartStr = getAttributeValue(
                                          entry, StorageNames.AUTORUN);
                int autoStart = Integer.parseInt(autoStartStr);
                appInfo = new ApplicationInfo(aUid,
                                              getAttributeValue(entry, StorageNames.NAME),
                                              getAttributeValue(entry, StorageNames.MAIN_CLASS),
                                              autoStart);
            }
        }
        return appInfo;
    }

    /**
     * Writes application info to JavaStorage.
     */
    private void writeAppInfo(Uid aUid, ApplicationInfo aAppInfo)
    {
        if (aUid == null || aAppInfo == null)
        {
            return;
        }
        StorageEntry entry = new StorageEntry();
        entry.addAttribute(new StorageAttribute(
                               StorageNames.ID, aAppInfo.getUid().getStringValue()));
        entry.addAttribute(new StorageAttribute(
                               StorageNames.PACKAGE_ID, aUid.getStringValue()));
        entry.addAttribute(new StorageAttribute(
                               StorageNames.NAME, aAppInfo.getName()));
        entry.addAttribute(new StorageAttribute(
                               StorageNames.MAIN_CLASS, aAppInfo.getMainClass()));
        entry.addAttribute(new StorageAttribute(
                               StorageNames.AUTORUN, Integer.toString(aAppInfo.getAutoStart()),
                               StorageAttribute.INT_TYPE));
        iSession.write(StorageNames.APPLICATION_TABLE, entry);
    }

    /**
     * Removes info from JavaStorage for given suite uid.
     */
    private void removeSuiteInfo(SuiteInfo aSuiteInfo, boolean aUpdate)
    {
        Uid uid = aSuiteInfo.getUid();
        checkSession();
        checkTransaction();
        iSession.remove(StorageNames.APPLICATION_PACKAGE_TABLE, uid);
        iSession.remove(StorageNames.APPLICATION_PACKAGE_ATTRIBUTES_TABLE, uid);

        // StorageNames.MIDP_PACKAGE_TABLE table is removed when
        // AuthenticationModule.removeSecurityData()
        // is called in RemoveSecurityData step,
        // so do not remove it here.
        // If StorageNames.MIDP_PACKAGE_TABLE was removed here,
        // it would cause security error in update.

        // Remove data from StorageNames.APPLICATION_TABLE table.
        Uid[] appUids = getAppUids(uid);
        if (appUids != null)
        {
            for (int i = 0; i < appUids.length; i++)
            {
                iSession.remove(StorageNames.APPLICATION_TABLE, appUids[i]);
            }
        }
        if (aUpdate)
        {
            // User or Preinstaller is making an update,
            // do not change preinstall state.
        }
        else
        {
            // Not an update ==> suite is being uninstalled.
            int oldPreinstallState = readPreinstallState(aSuiteInfo);
            if (oldPreinstallState == aSuiteInfo.STATE_PREINSTALLED)
            {
                if (aSuiteInfo.getResetPreinstall())
                {
                    // Preinstallation state should be reset,
                    // remove existing data from
                    // StorageNames.PREINSTALL_TABLE table.
                    removePreinstallState(aSuiteInfo);
                }
                else
                {
                    // User is uninstalling a preinstalled application,
                    // set preinstall state to STATE_NO_PREINSTALL.
                    setNoPreinstallState(aSuiteInfo);
                }
            }
            else if (oldPreinstallState == aSuiteInfo.STATE_INSTALLED)
            {
                // Remove existing data from StorageNames.PREINSTALL_TABLE table.
                removePreinstallState(aSuiteInfo);
            }
        }
    }

    /**
     * Reads preinstall state from INSTALL_STATE column
     * value from StorageNames.PREINSTALL_TABLE table.
     * If INSTALL_STATE value is not found, returns -1.
     */
    private int readPreinstallState(SuiteInfo aSuiteInfo)
    {
        int result = -1;
        // Match preinstall state using name and vendor.
        StorageEntry query = new StorageEntry();
        query.addAttribute
        (new StorageAttribute
         (StorageNames.NAME, aSuiteInfo.getName()));
        query.addAttribute
        (new StorageAttribute
         (StorageNames.VENDOR, aSuiteInfo.getVendor()));
        query.addAttribute(new StorageAttribute(
                               StorageNames.INSTALL_STATE, ""));
        StorageEntry[] entries = iSession.search(
                                     StorageNames.PREINSTALL_TABLE, query);
        if (entries != null && entries.length > 0)
        {
            // Fill suite info.
            StorageEntry entry = entries[0];
            result = Integer.parseInt
                     (getAttributeValue(entry, StorageNames.INSTALL_STATE));
        }
        return result;
    }

    /**
     * Writes preinstall state to StorageNames.PREINSTALL_TABLE table.
     */
    private void writePreinstallState(SuiteInfo aSuiteInfo)
    {
        if (aSuiteInfo == null)
        {
            return;
        }

        // Before writing new preinstall state we must fetch the old state.
        int oldPreinstallState = readPreinstallState(aSuiteInfo);

        // Match preinstall state using name and vendor.
        StorageEntry matchEntry = new StorageEntry();
        matchEntry.addAttribute
        (new StorageAttribute
         (StorageNames.NAME, aSuiteInfo.getName()));
        matchEntry.addAttribute
        (new StorageAttribute
         (StorageNames.VENDOR, aSuiteInfo.getVendor()));

        // Create a new entry with name, vendor and version.
        StorageEntry entry = new StorageEntry();
        entry.addAttribute
        (new StorageAttribute
         (StorageNames.NAME, aSuiteInfo.getName()));
        entry.addAttribute
        (new StorageAttribute
         (StorageNames.VENDOR, aSuiteInfo.getVendor()));
        entry.addAttribute
        (new StorageAttribute
         (StorageNames.VERSION, aSuiteInfo.getVersion().toString()));

        if (oldPreinstallState >= 0)
        {
            // Entry exists, update version into it.
            if (aSuiteInfo.iPreinstallState == aSuiteInfo.STATE_PREINSTALLED)
            {
                // Update preinstall state only if the new state
                // indicates preinstallation.
                entry.addAttribute
                (new StorageAttribute
                 (StorageNames.INSTALL_STATE,
                  Integer.toString(aSuiteInfo.iPreinstallState),
                  StorageAttribute.INT_TYPE));
                Log.log("Setting preinstall state to " +
                        aSuiteInfo.iPreinstallState);
            }
            iSession.update(StorageNames.PREINSTALL_TABLE, entry, matchEntry);
        }
        else
        {
            // Entry does not exist, create it with name, vendor,
            // version and state.
            entry.addAttribute
            (new StorageAttribute
             (StorageNames.INSTALL_STATE,
              Integer.toString(aSuiteInfo.iPreinstallState),
              StorageAttribute.INT_TYPE));
            Log.log("Setting preinstall state to " +
                    aSuiteInfo.iPreinstallState);
            iSession.write(StorageNames.PREINSTALL_TABLE, entry);
        }
    }

    /**
     * Updates preinstall state to STATE_NO_PREINSTALL in
     * StorageNames.PREINSTALL_TABLE table.
     */
    private void setNoPreinstallState(SuiteInfo aSuiteInfo)
    {
        if (aSuiteInfo == null)
        {
            return;
        }

        // Match preinstall state using name and vendor.
        StorageEntry matchEntry = new StorageEntry();
        matchEntry.addAttribute
        (new StorageAttribute
         (StorageNames.NAME, aSuiteInfo.getName()));
        matchEntry.addAttribute
        (new StorageAttribute
         (StorageNames.VENDOR, aSuiteInfo.getVendor()));

        // Create a new entry with name, vendor, version and state.
        StorageEntry entry = new StorageEntry();
        entry.addAttribute
        (new StorageAttribute
         (StorageNames.NAME, aSuiteInfo.getName()));
        entry.addAttribute
        (new StorageAttribute
         (StorageNames.VENDOR, aSuiteInfo.getVendor()));
        entry.addAttribute
        (new StorageAttribute
         (StorageNames.VERSION, aSuiteInfo.getVersion().toString()));
        entry.addAttribute
        (new StorageAttribute
         (StorageNames.INSTALL_STATE,
          Integer.toString(aSuiteInfo.STATE_NO_PREINSTALL),
          StorageAttribute.INT_TYPE));

        Log.log("Setting preinstall state to " +
                aSuiteInfo.STATE_NO_PREINSTALL);
        iSession.update(StorageNames.PREINSTALL_TABLE, entry, matchEntry);
    }

    /**
     * Removes preinstall state for given suite from
     * preinstall table 
     */
    private void removePreinstallState(SuiteInfo aSuiteInfo)
    {
        if (aSuiteInfo == null)
        {
            return;
        }
        // Match preinstall state using name and vendor.
        StorageEntry matchEntry = new StorageEntry();
        matchEntry.addAttribute
        (new StorageAttribute
         (StorageNames.NAME, aSuiteInfo.getName()));
        matchEntry.addAttribute
        (new StorageAttribute
         (StorageNames.VENDOR, aSuiteInfo.getVendor()));

        Log.log("Removing preinstall state");
        iSession.remove(StorageNames.PREINSTALL_TABLE, matchEntry);
    }

    /**
     * Checks that JavaStorage session exists.
     * @throws InstallerException if session does not exist.
     */
    private void checkSession()
    {
        if (iSession == null)
        {
            InstallerException.internalError("Session does not exist.");
        }
    }

    /**
     * Checks that JavaStorage transaction exists.
     * @throws InstallerException if transaction does not exist.
     */
    private void checkTransaction()
    {
        if (!iTransactionExists)
        {
            InstallerException.internalError("Transaction does not exist.");
        }
    }

    /**
     * Sort the suite applications by matching application mainclass
     * names and classnames in MIDlet-n attributes.
     */
    private void sortApps(SuiteInfo aSuiteInfo)
    {
        Vector apps = aSuiteInfo.getApplications();
        ApplicationInfo[] sortedApps = new ApplicationInfo[apps.size()];
        for (int i = 0; i < apps.size(); i++)
        {
            ApplicationInfo app = (ApplicationInfo)apps.elementAt(i);
            String appMainClass = app.getMainClass();
            for (int j = 1; true; j++)
            {
                String attrValue = aSuiteInfo.getAttributeValue("MIDlet-" + j);
                if (attrValue == null)
                {
                    break;
                }
                String[] tokens = Tokenizer.split(attrValue, ",");
                if (tokens[2].trim().equals(appMainClass))
                {
                    while (j < sortedApps.length && sortedApps[j - 1] != null)
                    {
                        // Increase j in case the same class name is already
                        // used by some other application in the same suite.
                        j++;
                    }
                    sortedApps[j-1] = (ApplicationInfo)apps.elementAt(i);
                    break;
                }
            }
        }
        aSuiteInfo.setApplications(new Vector());
        for (int i = 0; i < sortedApps.length; i++)
        {
            aSuiteInfo.addApplication(sortedApps[i]);
        }
    }
}
