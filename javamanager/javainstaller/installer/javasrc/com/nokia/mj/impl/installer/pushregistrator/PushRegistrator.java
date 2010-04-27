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


package com.nokia.mj.impl.installer.pushregistrator;

import com.nokia.mj.impl.storage.StorageSession;
import com.nokia.mj.impl.utils.Uid;

public interface PushRegistrator
{

    /**
     * Register application to push framework.
     * @param aSession storage session. Storage session and transaction has been opened
     *                 before calling this operation.
     * @param aSuiteUid suite uid of installed application.
     * @param aPushInfos push registration infos. Installer has verified before calling
     *                   this operation that class name of the push registration is valid.
     * @throws PushRegistrationException if push registration fails
     */
    public void registerPush(StorageSession aSession, Uid aSuiteUid, PushInfo[] aPushInfos);

    /**
     * Unregister application from push framework.
     * @param aSession storage session. Storage session and transaction has been opened
     *                 before calling this operation.
     * @param aSuiteUid suite uid of installed application.
     * @param aUids uids of applications to be unregistered
     * @throws PushRegistrationException if push unregistration fails
     */
    public void unregisterPush(StorageSession aSession, Uid aSuiteUid, Uid[] aUids);

    /**
     * Upgrade applications registered to push framework.
     * @param aSession storage session. Storage session and transaction has been opened
     *                 before calling this operation.
     * @param aSuiteUid suite uid of installed application.
     * @param aOldPushApplicationInfos old push application infos for all the applications
     *        in the old suite
     * @param aNewPushApplicationInfos new push application infos for
     *        all the applications in the new suite
     * @param aNewPushInfos new push registration infos. Installer has verified before calling
     *                      this operation that class name of the push registration is valid.
     *                      Note: Implementation class might set one or more PushInfo item
     *                            to null in this table.
     *                            I.e. DO NOT USE THIS ARGUMENT AFTER THIS FUNCTION CALL.
     * @throws PushRegistrationException if push (un)registration fails
     */
    public void upgradePush(
        StorageSession aSession,
        Uid aSuiteUid,
        PushApplicationInfo[] aOldPushApplicationInfos,
        PushApplicationInfo[] aNewPushApplicationInfos,
        PushInfo[] aNewPushInfos);

    /**
     * Get info about connections registered to push framework.
     * @param aUid application uid
     * @return Info for push connections registered with given application uid. Null
     *         is returned if push regs by uid is not found from the db.
     * @throws PushRegistrationException if push (un)registration fails
     */
    public PushInfo[] getPushInfos(Uid aUid);
}
