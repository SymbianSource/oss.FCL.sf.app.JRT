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


package com.nokia.mj.impl.installer.downloader;

import com.nokia.mj.impl.installer.storagehandler.OtaStatusHandler;
import com.nokia.mj.impl.installer.storagehandler.OtaStatusNotification;
import com.nokia.mj.impl.installer.utils.InstallerException;
import com.nokia.mj.impl.installer.utils.Log;
import com.nokia.mj.impl.rt.support.Jvm;

import java.io.IOException;

/**
 * NotificationPoster takes care of posting OTA status notifcations.
 * It uses a separate thread for posting pending notifications.
 * HTTP authentication is not supported for the OTA status notifications.
 *
 * @author Nokia Corporation
 * @version $Rev: 0 $ $Date$
 */
abstract public class NotificationPoster implements Runnable
{
    // Character set used in notification body:
    // use MIDP default character set UTF-8.
    protected static final String CHARSET = "utf-8";
    // ContentType used in notification.
    protected static final String CONTENT_TYPE =
        "text/plain; charset=" + CHARSET;

    // Maximum number of HTTP redirects to be followed.
    protected static final int MAX_REDIRECT_COUNT = 5;

    // Internal states.
    protected static final int STATE_NOT_STARTED = 0;
    protected static final int STATE_POSTING = 1;
    protected static final int STATE_STOPPED = 2;

    // Internal state.
    protected int iState = STATE_NOT_STARTED;
    // Internet access point id.
    protected String iIap = null;
    // Service network access point id.
    protected String iSnap = null;
    // HTTP redirect count.
    protected int iRedirectCount = 0;

    // OtaStatusHandler instance.
    protected OtaStatusHandler iOtaStatusHandler = new OtaStatusHandler();

    /** Constructor. */
    protected NotificationPoster()
    {
    }

    /** Constructor. */
    protected NotificationPoster(String aIap, String aSnap)
    {
        iIap = aIap;
        iSnap = aSnap;
    }

    /**
     * Send OTA status notification. If notification cannot be sent,
     * use OtaStatusHandler to save it to storage for future sending.
     * This is a synchronous call and returns only after notification
     * sending has been completed.
     */
    synchronized public void notifyStatus(OtaStatusNotification aOtaStatusNotification)
    {
        if (iState == STATE_POSTING)
        {
            InstallerException.internalError
            ("NotificationPoster.notifyStatus: already posting");
        }
        if (aOtaStatusNotification == null)
        {
            InstallerException.internalError
            ("NotificationPoster.notifyStatus: null parameter");
        }
        if (!isNotificationUrl(aOtaStatusNotification.getUrl()))
        {
            // Not a valid notification URL, let's ignore it
            // but write a log warning message.
            Log.logWarning("NotificationPoster.notifyStatus: not a " +
                           "valid notification URL\n" +
                           aOtaStatusNotification);
            return;
        }
        try
        {
            doPost(aOtaStatusNotification, null);
        }
        catch (Throwable t)
        {
            Log.logWarning("NotificationPoster.notifyStatus: posting " +
                           "notification failed", t);
            // Posting failed, save notification to storage for
            // further retry attempts.
            aOtaStatusNotification.setLatestRetryTime
            (System.currentTimeMillis());
            iOtaStatusHandler.addNotification(aOtaStatusNotification);
        }
    }

    /**
     * Send pending OTA status notifications. Use OtaStatusHandler to
     * retrieve pending notifications and try to send them. If sending
     * fails, use OtaStatusHandler to save the remaining notifications
     * to storage for future sending attempts.
     * This is an asynchronous call and starts a new thread for
     * notification sending. Thread can be stopped with stop() method,
     * or its completion can be waited with waitForCompletion() method.
     */
    synchronized public void notifyPendingStatuses()
    {
        if (iState == STATE_POSTING)
        {
            InstallerException.internalError
            ("NotificationPoster.notifyPendingStatuses: already posting");
        }
        iState = STATE_POSTING;
        Thread thread = new Thread(this, "NotificationPosterThread");
        Jvm.setThreadAsDaemon(thread, true);
        thread.start();
    }

    /**
     * Returns true if notification posting has been started.
     * Note that this method returns true also if notification posting
     * has been started and stopped, so it does not tell if posting is
     * currently going on.
     */
    synchronized public boolean isStarted()
    {
        if (iState == STATE_NOT_STARTED)
        {
            return false;
        }
        return true;
    }

    /**
     * Stop notification posting.
     */
    synchronized public void stop()
    {
        if (iState == STATE_POSTING)
        {
            iState = STATE_STOPPED;
            // Notify waitForCompletion() that notification posting has been stopped.
            this.notify();
        }
    }

    /**
     * This method blocks until notification posting has been completed.
     * If posting is not going on, this method returns immediately.
     */
    synchronized public void waitForCompletion()
    {
        if (iState != STATE_POSTING)
        {
            return;
        }
        try
        {
            this.wait();
        }
        catch (InterruptedException ie)
        {
            // Ignore
        }
    }

    /**
     * This method posts pending notifications in a separate Thread.
     */
    public void run()
    {
        Log.log("NotificationPoster: thread starts");

        try
        {
            // Get pending notifications from storage and try to send them.
            OtaStatusNotification[] notifications =
                iOtaStatusHandler.getNotifications();
            if (notifications != null)
            {
                Log.log("NotificationPoster: found " + notifications.length +
                        " pending notifications");
                for (int i = 0;
                        i < notifications.length && iState == STATE_POSTING;
                        i++)
                {
                    try
                    {
                        Log.log("NotificationPoster: posting notifications[" +
                                i + "]");
                        doPost(notifications[i], null);
                        // Posting succeeded, remove notification
                        // from storage.
                        iOtaStatusHandler.removeNotification
                        (notifications[i]);
                    }
                    catch (Throwable t)
                    {
                        Log.logWarning("NotificationPoster: posting " +
                                       "notification failed", t);
                        // Posting failed, update counter and save
                        // notification to storage for further retry
                        // attempts.
                        if (notifications[i].increaseRetryCount())
                        {
                            iOtaStatusHandler.updateNotification
                            (notifications[i]);
                        }
                        else
                        {
                            // Maxmimum number of retries has been exceeded,
                            // posting this notification should not be retried
                            // anymore, remove it from storage.
                            iOtaStatusHandler.removeNotification
                            (notifications[i]);
                        }
                    }
                }
            }
        }
        catch (Throwable t)
        {
            Log.logWarning("NotificationPoster: unexpected exception", t);
        }

        Log.log("NotificationPoster: thread exits");

        // Notify that waitForCompletion() can proceed.
        synchronized (this)
        {
            iState = STATE_STOPPED;
            this.notify();
        }

    }

    /**
     * Returns true if given URL is valid for notifications, false otherwise.
     */
    public static boolean isNotificationUrl(String aUrl)
    {
        if (aUrl == null || aUrl.length() == 0)
        {
            return false;
        }
        String lcUrl = aUrl.toLowerCase();
        return (lcUrl.startsWith("http://") || lcUrl.startsWith("https://"));
    }


    /**
     * Posts given OTA status notification with HTTP POST request.
     *
     * @param aOtaStatusNotification notification to be posted
     * @param aLocation redirect url for this notification, can be null
     */
    abstract protected void doPost
    (OtaStatusNotification aOtaStatusNotification,
     String aLocation) throws IOException;

    /**
     * Checks if HTTP redirect is needed. If redirect is needed,
     * this method increases redirect counter.
     *
     * @param aHttpStatus HTTP response status code
     * @param aLocation Location header from HTTP response
     * @return true if redirect is needed, false otherwise
     */
    protected boolean redirectNeeded(int aHttpStatus, String aLocation)
    {
        boolean result = false;
        if (iRedirectCount >= MAX_REDIRECT_COUNT)
        {
            // Maximum number of redirections has been exceeded.
            return result;
        }
        if (aHttpStatus >= 301 && aHttpStatus <= 307 &&
                isNotificationUrl(aLocation))
        {
            // Redirect is needed.
            iRedirectCount++;
            result = true;
        }
        return result;
    }
}
