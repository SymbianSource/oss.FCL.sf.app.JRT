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


package com.nokia.mj.impl.installer.midp2.uninstall.steps;

import com.nokia.mj.impl.installer.downloader.NotificationPoster;
import com.nokia.mj.impl.installer.exetable.ExeBall;
import com.nokia.mj.impl.installer.exetable.ExeStep;
import com.nokia.mj.impl.installer.storagehandler.OtaStatusHandler;
import com.nokia.mj.impl.installer.storagehandler.OtaStatusNotification;
import com.nokia.mj.impl.installer.utils.InstallerException;
import com.nokia.mj.impl.installer.utils.Log;
import com.nokia.mj.impl.utils.OtaStatusCode;
import com.nokia.mj.impl.utils.exception.InstallerExceptionBase;

/**
 * Uninstallation step for sending OTA status notifications.
 * This is the first and the last step in uninstallation execution table.
 * This execution step may not throw any exceptions.
 *
 * @author Nokia Corporation
 * @version $Rev:  $ $Date:  $
 */
public class SendOtaStatus extends ExeStep
{
    private static final String NOTIFY_ATTR_NAME = "MIDlet-Delete-Notify";

    public void execute(ExeBall aBall)
    {
        UninstallBall ball = (UninstallBall)aBall;
        if (ball.iSuite == null)
        {
            // Do nothing when called as the first installation step.
            return;
        }

        if (ball.iArgs.get("skipotastatus") != null)
        {
            Log.log("Skipping OTA status handling");
            return;
        }

        String notifyUrl = ball.iSuite.getAttributeValue(NOTIFY_ATTR_NAME);
        if (notifyUrl != null)
        {
            if (!NotificationPoster.isNotificationUrl(notifyUrl))
            {
                Log.logWarning("Ignoring not supported " + NOTIFY_ATTR_NAME +
                               " url: " + notifyUrl);
                return;
            }

            // Notify url is present and valid, create and save notification
            // for future posting.
            OtaStatusNotification notification = new OtaStatusNotification
            (ball.iSuite.getUid(), System.currentTimeMillis(),
             OtaStatusNotification.TYPE_UNINSTALL,
             OtaStatusCode.DELETION_NOTIFICATION,
             notifyUrl, 0, 0);
            sendNotification(ball, notification);
        }
    }

    public void cancel(ExeBall aBall)
    {
        UninstallBall ball = (UninstallBall)aBall;
        if (ball.iSuite == null)
        {
            // Do nothing if called when suite info is not yet available.
            return;
        }

        // Cancel method for the last execution step never gets called,
        // so when this method is called we are cancelling the first
        // step.

        // Uninstallation has failed and OTA status notification
        // for uninstallation should only be sent when uninstallation
        // has succeeded so do nothing here.
    }

    private void sendNotification(UninstallBall aBall,
                                  OtaStatusNotification aNotification)
    {
        if (aBall == null || aNotification == null)
        {
            return;
        }
        try
        {
            // Always send uninstall notification right away.
            aBall.log("Sending OTA status notification...");
            aBall.getNotificationPoster().notifyStatus(aNotification);
        }
        catch (Exception e)
        {
            Log.logError("SendOtaStatus.sendNotification failed", e);
        }
    }
}
