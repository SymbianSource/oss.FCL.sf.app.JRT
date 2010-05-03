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


package com.nokia.mj.impl.installer.downloader;

import com.nokia.mj.impl.installer.storagehandler.OtaStatusNotification;
import com.nokia.mj.impl.installer.utils.Log;

import java.io.InputStream;
import java.io.IOException;
import java.io.OutputStream;
import javax.microedition.io.Connector;
import javax.microedition.io.HttpConnection;

/**
 * GcfNotificationPoster implements NotificationPoster using
 * MIDP GCF classes.
 *
 * @author Nokia Corporation
 * @version $Rev: 0 $ $Date$
 */
public class GcfNotificationPoster extends NotificationPoster
{

    /** Constructor. */
    protected GcfNotificationPoster()
    {
        super();
    }

    /** Constructor. */
    protected GcfNotificationPoster(String aIap, String aSnap)
    {
        super(aIap, aSnap);
    }

    /**
     * Get a new NotificationPoster instance. Returned NotificationPoster
     * uses system default access point.
     *
     * @return New NotificationPoster instance.
     */
    public static NotificationPoster getNotificationPoster()
    {
        return getNotificationPoster(null, null);
    }

    /**
     * Get a new NotificationPoster instance.
     *
     * @param aIap Internet access point to be used.
     * @param aSnap Service network access point to be used.
     * @return New NotificationPoster instance.
     */
    public static NotificationPoster getNotificationPoster
    (String aIap, String aSnap)
    {
        return new GcfNotificationPoster(aIap, aSnap);
    }

    /**
     * Appends timeout parameter to given URL in format supported by GCF.
     */
    protected static String getUrlWithTimeout(String aUrl, int aSeconds)
    {
        return aUrl + ";nokia_timeout=" + aSeconds;
    }

    /**
     * Do the actual notification posting with HTTP POST request.
     *
     * @param aOtaStatusNotification notification to be posted
     * @param aLocation redirect url for this notification, can be null
     */
    protected void doPost(OtaStatusNotification aOtaStatusNotification,
                          String aLocation)
    throws IOException
    {
        Log.log("GcfNotificationPoster: posting notification:\n" +
                aOtaStatusNotification);

        // Open connection.
        String url = getUrlWithTimeout(
                         GcfDownloader.getUrlWithAccessPoint(
                             aOtaStatusNotification.getUrl(), iIap, iSnap), 60);
        Log.logInfoPrd("GcfNotificationPoster: opening " + url);
        HttpConnection connection = (HttpConnection)Connector.open(url);
        connection.setRequestMethod(connection.POST); // we will post content
        connection.setRequestProperty("Content-Type", CONTENT_TYPE);

        // Post notification.
        OutputStream out = null;
        try
        {
            Log.log("GcfNotificationPoster: opening OutputStream");
            out = connection.openOutputStream();
            Log.log("GcfNotificationPoster: writing to OutputStream");
            out.write(aOtaStatusNotification.getStatusReportBody()
                      .getBytes(CHARSET));
            out.flush();
        }
        catch (IOException ioe)
        {
            Log.logError("Unexpected exception during notification posting", ioe);
            throw ioe;
        }
        finally
        {
            if (out != null)
            {
                // Close OutputStream.
                Log.log("GcfNotificationPoster: closing OutputStream");
                out.close();
                out = null;
            }
        }

        // Check status from the response.
        Log.log("GcfNotificationPoster: getting HTTP status");
        int status = connection.getResponseCode();
        Log.logInfoPrd("GcfNotificationPoster: got HTTP status: " + status);

        // Check if HTTP redirect is needed.
        String location = connection.getHeaderField("Location");
        if (redirectNeeded(status, location))
        {
            Log.log("GcfNotificationPoster: got HTTP redirect to: " +
                    location);
            connection.close();
            doPost(aOtaStatusNotification, location);
            return;
        }

        // Check if notification posting was successful.
        if (status != connection.HTTP_OK)
        {
            // Server returned error status, notification sending
            // must be retried.
            connection.close();
            throw new IOException("Server returned error status " + status);
        }

        // Close the connection.
        connection.close();
    }
}
