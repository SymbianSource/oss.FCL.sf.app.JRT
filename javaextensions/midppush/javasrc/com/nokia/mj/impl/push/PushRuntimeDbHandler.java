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
import com.nokia.mj.impl.storage.*;
import java.lang.Integer;
import java.lang.String;
import com.nokia.mj.impl.utils.Logger;


/**
 * This class is used by PushRegistryImpl class(i.e. Runtime process) to read and manipulate
 * push data in the Storage.
 */
final class PushRuntimeDbHandler
{

    /**
     * @return null if push reg by connection URI did not match to any push reg.
     */
    public static PushRegData getPushRegByConnection(String aUri,StorageSession aSession)
    {
        Logger.LOG(Logger.EJavaPush, Logger.EInfo, "PushRuntimeDbHandler.getUidAndRegTypeOfPushReg()");

        StorageEntry query = new StorageEntry();
        query.addAttribute(new StorageAttribute(StorageNames.URL,aUri));
        StorageEntry[] entries = aSession.search(StorageNames.PUSH_REGISTRATIONS_TABLE,query);
        PushRegData[] pushInfoTable = mapStorageEntryToPushInfo(entries);
        if (null == pushInfoTable)
            return null;
        //There can be only one push registration because Uri is only one in the primary key.
        //Null is returned by mapStorageEntryToPushInfo() operation if size of the 'entries'
        //table is zero.
        return pushInfoTable[0];
    }//end getPushRegByConnection

    /**
     *
     */
    public static PushRegData[] mapStorageEntryToPushInfo(StorageEntry[] aEntries)
    {
        Logger.LOG(Logger.EJavaPush, Logger.EInfo, "PushRuntimeDbHandler.mapStorageEntryToPushInfo() +");
        if (null == aEntries)
            return null;
        Logger.LOG(Logger.EJavaPush, Logger.EInfo, "PushRuntimeDbHandler.mapStorageEntryToPushInfo() #2");
        if (0 == aEntries.length)
            return null;
        Logger.LOG(Logger.EJavaPush, Logger.EInfo, "PushRuntimeDbHandler.mapStorageEntryToPushInfo() #3");
        PushRegData[] pushInfoTable = new PushRegData[aEntries.length];

        for (int i = 0; i < aEntries.length; i++)
        {
            StorageAttribute attr = aEntries[i].getAttribute(StorageNames.ID);
            Uid newUid = Uid.createUid(attr.getValue());
            attr = aEntries[i].getAttribute(StorageNames.REGISTRATION_TYPE);
            int regType = Integer.parseInt(attr.getValue());
            PushRegData infoObj = new PushRegData(newUid,regType);
            pushInfoTable[i] = infoObj;
        }//end for

        Logger.LOG(Logger.EJavaPush, Logger.EInfo, "PushRuntimeDbHandler.mapStorageEntryToPushInfo() -");
        return pushInfoTable;
    }//end mapStorageEntryToPushInfo

}//end final class PushRuntimeDbHandler



