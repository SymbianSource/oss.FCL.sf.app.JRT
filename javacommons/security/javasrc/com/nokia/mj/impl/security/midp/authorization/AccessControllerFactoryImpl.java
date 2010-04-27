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

package com.nokia.mj.impl.security.midp.authorization;

import com.nokia.mj.impl.storage.StorageSession;
import com.nokia.mj.impl.utils.Uid;
import java.util.Hashtable;

/**
 * Factory class for creating different MIDPAccessController instances for
 * different application instances. Every running application has an instance
 * of MIDPAccessController created through this class = the Runtime delegates
 * access control calls (checkPermission) to this class. Also the Runtime
 * informs this class when a certain application has been stopped, so that
 * the corresponding instance of MIDPAccessController will be marked as
 * inactive.
 */
public final class AccessControllerFactoryImpl
{
    /**
     * Retrieves an instance of AccessControllerImpl used to perform
     * security checks
     *
     * @param aAppSuiteUid    The uid of the application suite on
     *                        behalf of which the AccessControllerImpl
     *                        instance is retrieved
     * @param aAppName        The name of the application on behalf of
     *                        which the AccessControllerImpl instance
     *                        is retrieved
     */
    public static AccessControllerImpl getAccessController(Uid aAppSuiteUid,
            String aAppName)
    {
        return getAccessController(null /* storage session */, aAppSuiteUid, aAppName);
    }

    /**
     * Retrieves an instance of AccessControllerImpl used to perform
     * security checks
     *
     * @param aStorageSession The JavaStorage session to be used
     * @param aAppSuiteUid    The uid of the application suite on
     *                        behalf of which the AccessControllerImpl
     *                        instance is retrieved
     * @param aAppName        The name of the application on behalf of
     *                        which the AccessControllerImpl instance
     *                        is retrieved
     */
    public static AccessControllerImpl getAccessController(StorageSession aStorageSession,
            Uid aAppSuiteUid,
            String aAppName)
    {
        if (aAppSuiteUid == null)
        {
            return null;
        }
        // Synchronization missing
        AccessControllerImpl instance = (AccessControllerImpl)instances.get(
                                            aAppSuiteUid);

        if (instance == null)
        {
            instance = new AccessControllerImpl(aStorageSession,
                                                aAppSuiteUid, aAppName);
            instances.put(aAppSuiteUid, instance);
        }
        return instance;
    }

    public static void destroyAccessController(Uid appUID)
    {
        if (appUID == null)
        {
            return;
        }
        // This is to be called when a MIDlet suite is stoped
        // Synchronization missing
        // Remove the instance or mark it as inactive?
        AccessControllerImpl ac = (AccessControllerImpl)instances
                                  .remove(appUID);
        if (ac != null)
        {
            ac.destroy();
        }
    }

    // self
    private static final AccessControllerFactoryImpl self
    = new AccessControllerFactoryImpl();

    // the list of live MIDPAccessController objects
    private static Hashtable instances = new Hashtable();

}
