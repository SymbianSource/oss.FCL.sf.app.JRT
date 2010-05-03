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


package com.nokia.mj.impl.installer.midp2.install.steps;

import com.nokia.mj.impl.installer.exetable.ExeBall;
import com.nokia.mj.impl.installer.exetable.ExeStep;
import com.nokia.mj.impl.installer.ui.PermissionInfo;
import com.nokia.mj.impl.installer.utils.InstallerException;
import com.nokia.mj.impl.installer.utils.Log;
import com.nokia.mj.impl.security.midp.authorization.PermissionGranter;

/**
 * ConfirmPermissions shows permission confirmation dialog to user.
 */
public class ConfirmPermissions extends ExeStep
{
    public void execute(ExeBall aBall)
    {
        InstallBall ball = (InstallBall)aBall;
        if (ball.getInstallerUi() != null)
        {
            // Get requested permissions from security framework.
            String[] blanketPermissions =
                PermissionGranter.getInstance()
                .getBlanketPermissions(ball.iSuite.getUid());
            if (blanketPermissions != null &&
                    blanketPermissions.length > 0)
            {
                Log.log("Possible blanket permissions:");
                for (int i = 0; i < blanketPermissions.length; i++)
                {
                    Log.log("  " + blanketPermissions[i]);
                }
                // Display blanket permissions query to the user.
                String permissionsDetails = PermissionGranter.getInstance()
                                            .getBlanketPermissionsDetails(ball.iSuite.getUid());
                createPermissionsQueryThread(
                    ball, blanketPermissions, permissionsDetails);
            }
        }
    }

    public void cancel(ExeBall aBall)
    {
        // nop
    }

    /**
     * Creates and starts a new thread for asking blanket permissions
     * query from the user.
     */
    private void createPermissionsQueryThread(
        InstallBall aBall, String[] aBlanketPermissions, String aPermissionsDetails)
    {
        final InstallBall ball = aBall;
        final String[] blanketPermissions = aBlanketPermissions;
        final String permissionsDetails = aPermissionsDetails;
        ball.iDialogOpen = true;
        new Thread(new Runnable()
        {
            public void run()
            {
                PermissionInfo permissionInfo = null;
                boolean userConfirmation = true;
                try
                {
                    permissionInfo = new PermissionInfo(
                        blanketPermissions, permissionsDetails);
                    userConfirmation = ball.getInstallerUi()
                                       .confirmPermissions(permissionInfo);
                    Log.log("PermissionInfo after confirmation: " +
                            permissionInfo);
                }
                catch (Throwable t)
                {
                    Log.logError(
                        "Exception from InstallerUi.confirmPermissions: " + t,
                        t);
                }
                if (!userConfirmation)
                {
                    ball.cancel();
                }
                else if (permissionInfo != null &&
                         permissionInfo.isPermissionAllowed() != null &&
                         permissionInfo.isPermissionAllowed().booleanValue() == true)
                {
                    ball.iBlanketPermissionsGranted = true;
                }

                synchronized (ball)
                {
                    ball.iDialogOpen = false;
                    // Notify WaitForPermissionsQuery step that
                    // we are finished with launch app query.
                    ball.notify();
                }
            }
        }, "PermissionsQueryThread").start();
    }
}
