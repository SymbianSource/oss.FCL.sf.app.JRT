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

import com.nokia.mj.impl.installer.utils.Log;

import java.io.InputStream;
import java.io.IOException;
import java.io.OutputStream;
import javax.microedition.io.Connector;
import javax.microedition.io.HttpConnection;

/**
 * GcfDownloader implements Downloader using MIDP GCF classes.
 *
 * @author Nokia Corporation
 * @version $Rev: 0 $ $Date$
 */
public class GcfDownloader extends Downloader
{

    /** Constructor. */
    protected GcfDownloader()
    {
        super();
    }

    /** Constructor. */
    protected GcfDownloader(DownloadListener aDownloadListener,
                            String aIap, String aSnap)
    {
        super(aDownloadListener, aIap, aSnap);
    }

    /**
     * Get a new Downloader instance. Returned Downloader uses
     * system default access point.
     *
     * @param aDownloadListener DownloadListener to which download
     * progress gets notified.
     * @return New Downloader instance.
     */
    public static Downloader getDownloader
    (DownloadListener aDownloadListener)
    {
        return getDownloader(aDownloadListener, null, null);
    }

    /**
     * Get a new Downloader instance.
     *
     * @param aDownloadListener DownloadListener to which download
     * progress gets notified.
     * @param aIap Internet access point to be used.
     * @param aSnap Service network access point to be used.
     * @return New Downloader instance.
     */
    public static Downloader getDownloader
    (DownloadListener aDownloadListener, String aIap, String aSnap)
    {
        return new GcfDownloader(aDownloadListener, aIap, aSnap);
    }

    /**
     * Appends SNAP or IAP to given URL in format supported by GCF.
     */
    protected static String getUrlWithAccessPoint
    (String aUrl, String aIap, String aSnap)
    {
        String result = aUrl;
        if (aSnap != null && !aSnap.equals("0"))
        {
            result += ";nokia_netid=" + aSnap;
        }
        else if (aIap != null && !aIap.equals("0"))
        {
            result += ";nokia_apnid=" + aIap;
        }
        return result;
    }

    /**
     * Do the actual download.
     *
     * @param aOut OutputStream to which downloaded data is written.
     */
    protected void doDownload(OutputStream aOut) throws IOException
    {
        // Open connection.
        String url = getUrlWithAccessPoint(iDlInfo.getUrl(), iIap, iSnap);
        Log.log("GcfDownloader: opening " + url);
        HttpConnection connection = (HttpConnection)Connector.open(url);
        if (iAuthorizationHeader != null)
        {
            connection.setRequestProperty("Authorization",
                                          iAuthorizationHeader);
            Log.log("GcfDownloader: set Authorization header");
        }
        // Open InputStream for reading.
        InputStream in = connection.openInputStream();
        // Get content length, type and charset.
        iDlInfo.setTotalSize(connection.getLength());
        iDlInfo.setContentType(connection.getType());

        // Get response code and message.
        int status = connection.getResponseCode();
        Log.log("GcfDownloader: got HTTP status: " + status +
                " " + connection.getResponseMessage());

        // Check if HTTP redirect is needed.
        String location = connection.getHeaderField("Location");
        if (redirectNeeded(status, location))
        {
            Log.log("GcfDownloader: got HTTP redirect to: " + location);
            // Do not use the same auth credentials after HTTP redirect.
            iAuthorizationHeader = null;
            if (in != null)
            {
                // Close old InputStream.
                in.close();
                in = null;
            }
            // Close old connection.
            connection.close();
            connection = null;
            // Start a new download to redirect url.
            doDownload(aOut);
            return;
        }

        // Check if HTTP Basic authentication is needed.
        String authenticate = connection.getHeaderField("WWW-Authenticate");
        if (authenticate == null)
        {
            authenticate = connection.getHeaderField("Proxy-Authenticate");
        }
        if (basicAuthNeeded(status, authenticate))
        {
            if (in != null)
            {
                // Close old InputStream.
                in.close();
                in = null;
            }
            // Close old connection.
            connection.close();
            connection = null;
            if (iState == STATE_DOWNLOADING)
            {
                Log.log("GcfDownloader: Basic authentication needed");
                doDownload(aOut);
            }
            else
            {
                Log.log("GcfDownloader: download was cancelled");
            }
            return;
        }

        // Check if HTTP response code indicates error.
        if (status != connection.HTTP_OK)
        {
            // Server returned error status, let's throw an exception.
            if (in != null)
            {
                // Close old InputStream.
                in.close();
                in = null;
            }
            // Close old connection.
            connection.close();
            connection = null;
            throw new IOException("Server returned error status " + status);
        }

        try
        {
            // Check that download size is withing expected range.
            checkDownloadSize();
            // Do download from InputStream to OutputStream.
            doDownload(in, aOut);
        }
        catch (IOException ioe)
        {
            Log.logError("Unexpected exception during download", ioe);
            throw ioe;
        }
        finally
        {
            if (in != null)
            {
                // Close InputStream.
                in.close();
                in = null;
            }
            // Close connection.
            connection.close();
            connection = null;
        }
    }
}
