/*
* Copyright (c) 2008-2009 Nokia Corporation and/or its subsidiary(-ies).
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


package com.nokia.mj.impl.installer.midp2.install.steps;

import com.nokia.mj.impl.installer.exetable.ExeBall;
import com.nokia.mj.impl.installer.exetable.ExeStep;
import com.nokia.mj.impl.installer.utils.InstallerException;
import com.nokia.mj.impl.installer.utils.Log;
import com.nokia.mj.impl.utils.Uid;
import com.nokia.mj.impl.security.midp.authorization.PermissionGranter;

/**
 * Installation step which waits until user has
 * answered to blanket permissions query.
 */
public class WaitForPermissionsQuery extends ExeStep
{
    public void execute(ExeBall aBall)
    {
        InstallBall ball = (InstallBall)aBall;
        synchronized (ball)
        {
            // If dialog is open it means that launch query
            // is still being displayed to user. Wait for
            // it to be closed.
            if (ball.iDialogOpen)
            {
                try
                {
                    ball.wait();
                }
                catch (InterruptedException ie)
                {
                }
            }
        }
        if (ball.iBlanketPermissionsGranted)
        {
            Log.log("Set permissions to blanket");
            PermissionGranter.getInstance().setPermissionsToBlanket(
                ball.iStorageHandler.getSession(), ball.iSuite.getUid());
        }
    }

    public void cancel(ExeBall aBall)
    {
        // nop
    }
}
