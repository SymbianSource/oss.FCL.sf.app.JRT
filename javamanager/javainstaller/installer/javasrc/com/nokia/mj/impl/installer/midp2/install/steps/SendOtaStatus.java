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

import com.nokia.mj.impl.installer.downloader.NotificationPoster;
import com.nokia.mj.impl.installer.exetable.ExeBall;
import com.nokia.mj.impl.installer.exetable.ExeStep;
import com.nokia.mj.impl.installer.storagehandler.OtaStatusHandler;
import com.nokia.mj.impl.installer.storagehandler.OtaStatusNotification;
import com.nokia.mj.impl.installer.utils.InstallerException;
import com.nokia.mj.impl.installer.utils.Log;
import com.nokia.mj.impl.installer.utils.PlatformUid;
import com.nokia.mj.impl.utils.Attribute;
import com.nokia.mj.impl.utils.OtaStatusCode;
import com.nokia.mj.impl.utils.Uid;
import com.nokia.mj.impl.utils.exception.InstallerExceptionBase;

/**
 * Installation step for sending OTA status notifications.
 * This is the first and the last step in installation execution table.
 * This execution step may not throw any exceptions.
 *
 * @author Nokia Corporation
 * @version $Rev:  $ $Date:  $
 */
public class SendOtaStatus extends ExeStep
{
    private static final String NOTIFY_ATTR_NAME = "MIDlet-Install-Notify";

    public void execute(ExeBall aBall)
    {
        InstallBall ball = (InstallBall)aBall;
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

        String notifyUrl = ball.getAttributeValue(NOTIFY_ATTR_NAME);
        if (notifyUrl != null)
        {
            if (!NotificationPoster.isNotificationUrl(notifyUrl))
            {
                Log.logWarning("Ignoring not supported " + NOTIFY_ATTR_NAME +
                               " url: " + notifyUrl);
                return;
            }
            // Notify url is present and valid, create a notification.
            OtaStatusNotification notification = new OtaStatusNotification
            (ball.iSuite.getUid(), System.currentTimeMillis(),
             OtaStatusNotification.TYPE_INSTALL, OtaStatusCode.SUCCESS,
             notifyUrl, 0, 0);
            sendNotification(ball, notification);
        }
    }

    public void cancel(ExeBall aBall)
    {
        InstallBall ball = (InstallBall)aBall;
        // Cancel method for the last execution step never gets called,
        // so when this method is called we are cancelling the first
        // step.

        if (ball.iArgs.get("skipotastatus") != null)
        {
            Log.log("Skipping OTA status handling");
            return;
        }

        // It can happen happen that installation fails before the
        // ball.iCombinedAttributes has been initialized, in that case
        // check if attribute exists directly from ball.iJadAttributes.
        String notifyUrl = ball.getAttributeValue(NOTIFY_ATTR_NAME);
        if (notifyUrl == null)
        {
            if (ball.attributeExistsInJad(NOTIFY_ATTR_NAME))
            {
                Attribute attr =
                    (Attribute)ball.iJadAttributes.get(NOTIFY_ATTR_NAME);
                if (attr != null)
                {
                    notifyUrl = attr.getValue();
                }
            }
        }
        // If installation fails before suite info has been initialized
        // or uids have been chosen, use zero uid.
        Uid uid = null;
        if (ball.iSuite != null && ball.iSuite.getUid() != null)
        {
            uid = ball.iSuite.getUid();
        }
        else
        {
            uid = PlatformUid.createUid(0);
        }

        // Installation has failed, send OTA status notification with
        // the correct status code.
        if (notifyUrl != null)
        {
            if (!NotificationPoster.isNotificationUrl(notifyUrl))
            {
                Log.logWarning("Ignoring not supported " + NOTIFY_ATTR_NAME +
                               " url: " + notifyUrl);
                return;
            }
            // Notify url is present and valid, create a notification.
            Exception ex = ball.getExecuteException();
            if (ex instanceof InstallerExceptionBase)
            {
                OtaStatusNotification notification = new OtaStatusNotification
                (uid, System.currentTimeMillis(),
                 OtaStatusNotification.TYPE_INSTALL,
                 ((InstallerExceptionBase)ex).getOtaStatusCode(),
                 notifyUrl, 0, 0);
                sendNotification(ball, notification);
            }
            else
            {
                Log.logWarning("Cannot get OTA status code for notification, " +
                               "exception is not based on InstallerExceptionBase", ex);
            }
        }
    }

    private void sendNotification(InstallBall aBall,
                                  OtaStatusNotification aNotification)
    {
        if (aBall == null || aNotification == null)
        {
            return;
        }
        try
        {
            if ((aBall.iIap != null && !aBall.iIap.equals("0")) ||
                    (aBall.iSnap != null && !aBall.iSnap.equals("0")) ||
                    (aBall.iJarUrl != null && aBall.iDownloadSucceeded))
            {
                // Access point has been specified or
                // a successful download has been made,
                // let's notify right away.
                aBall.log("Sending OTA status notification...");
                aBall.getNotificationPoster().notifyStatus(aNotification);
            }
            else
            {
                // Store notification for future sending.
                Log.log("Storing OTA status notification to be sent later...");
                (new OtaStatusHandler()).addNotification(aNotification);
            }
        }
        catch (Exception e)
        {
            Log.logError("SendOtaStatus.sendNotification failed", e);
        }
    }
}
