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


package com.nokia.mj.impl.push;

import java.io.IOException;
import com.nokia.mj.impl.utils.Uid;
import com.nokia.mj.impl.installer.pushregistrator.PushInfo;
import com.nokia.mj.impl.storage.*;
import java.lang.Integer;
import java.lang.String;
import com.nokia.mj.impl.utils.Logger;

/**
 * This class is used by PushRegistratorImpl class(i.e. Installer process) to read and manipulate
 * push data in the Storage.
 */
final class PushDbHandler
{

    /**
     *
     */
    public static void storePushRegs(StorageSession aSession, PushInfo[] aPushInfos)
    {
        Logger.LOG(Logger.EJavaInstaller, Logger.EInfo, "PushDbHandler.storePushRegs()");

        if (null == aPushInfos)
            return;

        for (int i = 0; i < aPushInfos.length; ++i)
        {
            StorageEntry entry = new StorageEntry();

            entry.addAttribute(new StorageAttribute(
                                   StorageNames.ID, aPushInfos[i].getUid().getStringValue()));
            entry.addAttribute(new StorageAttribute(
                                   StorageNames.URL, aPushInfos[i].getConnectionUrl()));
            entry.addAttribute(new StorageAttribute(
                                   StorageNames.NAME, aPushInfos[i].getClassName()));
            entry.addAttribute(new StorageAttribute(
                                   StorageNames.FILTER, aPushInfos[i].getFilter()));
            entry.addAttribute(new StorageAttribute(
                                   StorageNames.REGISTRATION_TYPE, Integer.toString(aPushInfos[i].getRegType()),
                                   StorageAttribute.INT_TYPE));

            aSession.write(StorageNames.PUSH_REGISTRATIONS_TABLE, entry);
        }//end for
    }//end storePushRegs

    /**
     *
     */
    public static void deletePushRegs(StorageSession aSession,Uid aUid)
    {
        Logger.LOG(Logger.EJavaInstaller, Logger.EInfo, "PushDbHandler.deletePushRegs()");

        aSession.remove(StorageNames.PUSH_REGISTRATIONS_TABLE,aUid);
    }//end deletePushRegs

    /**
     *
     */
    public static void deleteAlarm(StorageSession aSession,Uid aUid)
    {
        Logger.LOG(Logger.EJavaInstaller, Logger.EInfo, "PushDbHandler.deleteAlarm()");

        aSession.remove(StorageNames.ALARM_REGISTRATIONS_TABLE,aUid);
    }//end deleteAlarms

    /**
     *
     */
    public static PushInfo[] getPushRegs(StorageSession aSession,Uid aUid)
    {
        Logger.LOG(Logger.EJavaInstaller, Logger.EInfo, "PushDbHandler.getPushRegs()");

        StorageEntry query = new StorageEntry();
        query.addAttribute(new StorageAttribute(StorageNames.ID,aUid.getStringValue()));
        StorageEntry[] entries = aSession.search(StorageNames.PUSH_REGISTRATIONS_TABLE,query);
        return mapStorageEntryToPushInfo(entries);
    }//end getPushRegs

    /**
     * @param StorageSession This operation creates a StoreageSession if 'aSession' argument is null.
     *                       StorageSession is not closed if it is provided by user.
     */
    public static PushMidletData[] getClassNamesAndUidsOfMidletsInSuite(Uid aSuiteUid,StorageSession aSession)
    throws IOException
    {
        Logger.LOG(Logger.EJavaPush, Logger.EInfo, "PushDbHandler.getClassNamesOfMidletsInSuite()");
        boolean sessionCreatedByUser = false;
        if (null != aSession)
            sessionCreatedByUser = true;

        StorageSession sessionObj = aSession;
        try
        {
            if (false == sessionCreatedByUser)
            {
                sessionObj = StorageFactory.createSession();
                sessionObj.open();
            }
            StorageEntry query = new StorageEntry();
            query.addAttribute(new StorageAttribute(StorageNames.PACKAGE_ID,aSuiteUid.getStringValue()));
            query.addAttribute(new StorageAttribute(StorageNames.MAIN_CLASS,""));
            query.addAttribute(new StorageAttribute(StorageNames.ID,""));
            query.addAttribute(new StorageAttribute(StorageNames.NAME,""));
            StorageEntry[] entries = sessionObj.search(StorageNames.APPLICATION_TABLE,query);

            if (null == entries)
                return null;
            if (0 == entries.length)
                return null;

            PushMidletData[] midletData = new PushMidletData[entries.length];
            for (int i = 0; i < entries.length; i++)
            {
                StorageAttribute attr = entries[i].getAttribute(StorageNames.ID);
                Uid newUid = Uid.createUid(attr.getValue());
                attr = entries[i].getAttribute(StorageNames.MAIN_CLASS);
                String mainClassName = attr.getValue();
                attr = entries[i].getAttribute(StorageNames.NAME);
                String midletName = attr.getValue();
                PushMidletData tmpObj = new PushMidletData(newUid,mainClassName,midletName);
                midletData[i] = tmpObj;
            }//end for
            return midletData;
        }
        catch (Throwable ex)
        {
            throw new IOException("Getting name of MIDlets failed: " + ex);
        }
        finally
        {
            if (false == sessionCreatedByUser)
            {
                if (null != sessionObj)
                {
                    sessionObj.close();
                    sessionObj.destroySession();
                }
            }//end if(false == sessionCreatedByUser)
        }//end finally
    }//end getClassNamesOfMidletsInSuite()

    /**
     *
     */
    public static PushInfo[] mapStorageEntryToPushInfo(StorageEntry[] aEntries)
    {
        Logger.LOG(Logger.EJavaInstaller, Logger.EInfo, "PushDbHandler.mapStorageEntryToPushInfo() +");
        if (null == aEntries)
            return null;
        Logger.LOG(Logger.EJavaInstaller, Logger.EInfo, "PushDbHandler.mapStorageEntryToPushInfo() #2");
        if (0 == aEntries.length)
            return null;
        Logger.LOG(Logger.EJavaInstaller, Logger.EInfo, "PushDbHandler.mapStorageEntryToPushInfo() #3");
        PushInfo[] pushInfoTable = new PushInfo[aEntries.length];

        for (int i = 0; i < aEntries.length; i++)
        {
            StorageAttribute attr = aEntries[i].getAttribute(StorageNames.ID);
            Uid newUid = Uid.createUid(attr.getValue());
            attr = aEntries[i].getAttribute(StorageNames.URL);
            String uri = attr.getValue();
            attr = aEntries[i].getAttribute(StorageNames.NAME);
            String className = attr.getValue();
            attr = aEntries[i].getAttribute(StorageNames.FILTER);
            String filter = attr.getValue();
            attr = aEntries[i].getAttribute(StorageNames.REGISTRATION_TYPE);
            int regType = Integer.parseInt(attr.getValue());
            PushInfo infoObj = new PushInfo(newUid,uri,className,filter,regType);
            pushInfoTable[i] = infoObj;
        }//end for

        Logger.LOG(Logger.EJavaInstaller, Logger.EInfo, "PushDbHandler.mapStorageEntryToPushInfo() -");
        return pushInfoTable;
    }//end mapStorageEntryToPushInfo

}
