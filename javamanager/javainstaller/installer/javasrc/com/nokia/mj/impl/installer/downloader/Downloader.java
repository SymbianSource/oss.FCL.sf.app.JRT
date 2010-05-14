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

import com.nokia.mj.impl.installer.Installer;
import com.nokia.mj.impl.installer.utils.FileUtils;
import com.nokia.mj.impl.installer.utils.InstallerException;
import com.nokia.mj.impl.installer.utils.Log;
import com.nokia.mj.impl.installer.utils.SysUtil;
import com.nokia.mj.impl.rt.support.Jvm;
import com.nokia.mj.impl.utils.Base64;
import com.nokia.mj.impl.utils.InstallerErrorMessage;
import com.nokia.mj.impl.utils.InstallerDetailedErrorMessage;
import com.nokia.mj.impl.utils.OtaStatusCode;
import com.nokia.mj.impl.utils.Tokenizer;

import java.io.InputStream;
import java.io.IOException;
import java.io.OutputStream;
import java.io.UnsupportedEncodingException;

/**
 * Downloader downloads file from specified URL.
 * Each Downloader instance downloads one file at a time in
 * a separate thread. Parallel downloads are possible with
 * multiple Downloader instances.
 */
abstract public class Downloader implements Runnable
{
    // Maximum number of HTTP redirects to be followed.
    protected static final int MAX_REDIRECT_COUNT = 5;

    // Internal downloading states.
    protected static final int STATE_DOWNLOADING = 1;
    protected static final int STATE_STOPPED = 2;

    // Internal downloading state.
    protected int iState = 0;
    // Download info.
    protected DownloadInfo iDlInfo = null;
    // Download listener.
    protected DownloadListener iDlListener = null;
    // Internet access point id.
    protected String iIap = null;
    // Service network access point id.
    protected String iSnap = null;
    // HTTP redirect count.
    protected int iRedirectCount = 0;
    // HTTP Basic authentication header value
    protected String iAuthorizationHeader = null;
    // OutpustStream for the downloaded file.
    private OutputStream iOut = null;

    /** Constructor. */
    protected Downloader()
    {
    }

    /** Constructor. */
    protected Downloader(DownloadListener aDownloadListener,
                         String aIap, String aSnap)
    {
        if (aDownloadListener == null)
        {
            InstallerException.internalError("Downloader: no listener specified");
        }
        iDlListener = aDownloadListener;
        iIap = aIap;
        iSnap = aSnap;
    }

    /**
     * Start a new download.
     *
     * @param aDlInfo DownloadInfo object containing download URL and
     * filename to which download is made.
     */
    synchronized public void start(DownloadInfo aDlInfo)
    {
        if (aDlInfo == null)
        {
            InstallerException.internalError
            ("Downloader.start: no download info specified");
        }
        if (iDlInfo != null)
        {
            InstallerException.internalError
            ("Downloader.start: downloader is already in use");
        }
        iDlInfo = aDlInfo;
        iState = STATE_DOWNLOADING;
        Thread thread = new Thread(this, "DownloaderThread");
        Jvm.setThreadAsDaemon(thread, true);
        thread.start();
    }

    /**
     * Stop download.
     */
    synchronized public void stop()
    {
        if (iDlInfo == null)
        {
            return;
        }
        if (iState == STATE_DOWNLOADING)
        {
            iState = STATE_STOPPED;
            try
            {
                if (iOut != null)
                {
                    // Close OutputStream.
                    iOut.close();
                    iOut = null;
                }
            }
            catch (Throwable t)
            {
                Log.logError("Downloader.stop: closing OutputStream failed", t);
            }
            // Notify waitForCompletion() that download has been stopped.
            this.notify();
        }
    }

    /**
     * This method blocks until download has been completed.
     * If download is not going on, this method returns immediately.
     */
    synchronized public void waitForCompletion()
    {
        if (iDlInfo == null || iState != STATE_DOWNLOADING)
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
     * This method performs the actual download and is executed
     * in its own Thread.
     */
    public void run()
    {
        Log.log("Downloader: download thread starts");
        if (iDlInfo == null)
        {
            Log.logWarning("Downloader: no DownloadInfo, download thread exits");
            return;
        }

        iDlInfo.setStartTime(System.currentTimeMillis());
        try
        {
            // Notify listener that download has started.
            iDlListener.started(iDlInfo);
        }
        catch (Exception ex)
        {
            // Ignore
        }

        try
        {
            // Open file for writing.
            iOut = FileUtils.getOutputStream(iDlInfo.getFilename());
            // Do the actual download.
            doDownload(iOut);
        }
        catch (InstallerException iex)
        {
            Log.logError("Downloader: download failed", iex);
            iDlInfo.setException(iex);
        }
        catch (Throwable t)
        {
            Log.logError("Downloader: download failed", t);
            iDlInfo.setException(new InstallerException
                                 (InstallerErrorMessage.INST_NO_NET, null,
                                  InstallerDetailedErrorMessage.NO_NET,
                                  new String[] { iDlInfo.getUrl() },
                                  OtaStatusCode.LOSS_OF_SERVICE, t));
        }
        finally
        {
            try
            {
                if (iOut != null)
                {
                    // Close OutputStream.
                    iOut.close();
                    iOut = null;
                }
            }
            catch (Throwable t)
            {
                Log.logError("Downloader: closing OutputStream failed", t);
            }
        }

        iDlInfo.setEndTime(System.currentTimeMillis());
        try
        {
            // Notify listener that download has ended.
            iDlListener.ended(iDlInfo);
        }
        catch (Exception ex)
        {
            // Ignore
        }
        Log.log("Downloader: download thread exits");

        // Notify that waitForCompletion() can proceed.
        synchronized (this)
        {
            iState = STATE_STOPPED;
            iDlInfo = null;
            this.notify();
        }
    }

    /**
     * Returns true if given URL is downloadable, false otherwise.
     * URL is downloadable if it has "http://" or "https://"
     * prefix.
     */
    public static boolean isDownloadUrl(String aUrl)
    {
        if (aUrl == null || aUrl.length() == 0)
        {
            return false;
        }
        String lcUrl = aUrl.toLowerCase();
        return (lcUrl.startsWith("http://") || lcUrl.startsWith("https://"));
    }

    /**
     * Returns Base64 encoded string used in HTTP Basic authentication from
     * username and password.
     */
    static String getBasicAuthBase64(String aUsername, String aPassword)
    {
        String basicAuthStr = aUsername + ":" + aPassword;
        String base64Str = null;
        try
        {
            // RFC2617 does not specify which character encoding should be
            // used in strings used in basic authentication. Let's assume
            // ISO-8859-1 which is default in RFC2616.
            base64Str = Base64.encode(basicAuthStr.getBytes("ISO8859_1"));
        }
        catch (UnsupportedEncodingException uee)
        {
            InstallerException.internalError
            ("Base64 encoding for Basic HTTP authentication failed", uee);
        }
        return base64Str;
    }

    /**
     * Do the actual download and write downloaded data to specified
     * OutputStream. This method must be implemented in
     * Downloader subclass.
     *
     * @param aOut OutputStream to which downloaded data is written.
     */
    abstract protected void doDownload(OutputStream aOut) throws IOException;

    /**
     * Do the download from specified InputStream to specified OutputStream.
     *
     * @param aIn InputStream from which downloaded data is read.
     * @param aOut OutputStream to which downloaded data is written.
     */
    protected void doDownload(InputStream aIn, OutputStream aOut)
    throws IOException
    {
        // Size of data chunks read from net.
        int netReadLen = 512;
        // Amount of data read in between UI progress updates.
        long progressUpdateLen = 4096;
        if (iDlInfo.getTotalSize() > 0)
        {
            progressUpdateLen = iDlInfo.getTotalSize() / 20;
        }
        // Size of data chunks written do disk.
        int diskWriteLen = 32768;
        // Buffer for reading data.
        byte[] buf = new byte[diskWriteLen];
        int bufOffset = 0; // write offset to buf
        int readCount = 0; // read count from is.read()
        long lastProgressUpdate = 0; // when was progress updated
        while (iState == STATE_DOWNLOADING && readCount >= 0)
        {
            int bufReadLen = buf.length - bufOffset;
            if (bufReadLen > netReadLen)
            {
                bufReadLen = netReadLen;
            }
            readCount = aIn.read(buf, bufOffset, bufReadLen);
            if (readCount > 0)
            {
                // Successfully read data, let's update counters.
                bufOffset += readCount;
                iDlInfo.addCurrentSize(readCount);
                // Check that download size is withing expected range.
                checkDownloadSize();
                // Check if it is time to write data.
                if (bufOffset == buf.length)
                {
                    // Check that there is enough free disk space.
                    checkDiskSpace();
                    // Write data from buf to output stream.
                    aOut.write(buf, 0, bufOffset);
                    bufOffset = 0;
                }
                // Check if it is time to do UI progress update.
                if (iDlInfo.getCurrentSize() - lastProgressUpdate
                        > progressUpdateLen)
                {
                    lastProgressUpdate = iDlInfo.getCurrentSize();
                    try
                    {
                        // Notify listener that download has progressed.
                        iDlListener.updateProgress(iDlInfo);
                    }
                    catch (Exception ex)
                    {
                        // Ignore
                    }
                }
            }
        }
        if (bufOffset > 0)
        {
            // Check that there is enough free disk space.
            checkDiskSpace();
            // Write remaining bufOffset amount of data from buf
            // to output stream.
            aOut.write(buf, 0, bufOffset);
            bufOffset = 0;
        }
        if (lastProgressUpdate < iDlInfo.getCurrentSize())
        {
            try
            {
                // Make final download progress update.
                iDlListener.updateProgress(iDlInfo);
            }
            catch (Exception ex)
            {
                // Ignore
            }
        }
    }

    /**
     * Checks if HTTP redirect is needed. If redirect is needed,
     * this method updates new URL to iDlInfo and increases
     * redirect counter.
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
                isDownloadUrl(aLocation))
        {
            // Redirect is needed.
            iDlInfo.setUrl(aLocation);
            iRedirectCount++;
            result = true;
        }
        return result;
    }

    /**
     * Checks if HTTP Basic authentication is needed.
     * If HTTP Basic authentication is needed,
     * this method updates Authorization header
     * value to iAuthorization member.
     *
     * @param aHttpStatus HTTP response status code
     * @param aAuthenticate Authenticate header from HTTP response
     * @return true if HTTP Basic authentication is needed, false otherwise
     */
    protected boolean basicAuthNeeded(int aHttpStatus, String aAuthenticate)
    {
        boolean result = false;
        if (iAuthorizationHeader != null)
        {
            // Authorization header has already been set, which means
            // that authentication has been tried and it failed.
            // Do not try again.
            return result;
        }
        if (aAuthenticate == null || aAuthenticate.length() == 0 ||
                !aAuthenticate.trim().startsWith("Basic"))
        {
            return result;
        }
        if (aHttpStatus == 401 || aHttpStatus == 407)
        {
            iAuthorizationHeader = getAuthorizationHeaderValue();
            result = true;
        }
        return result;
    }

    /**
     * Asks username and password from DownloadListener
     * and creates HTTP Authorization header used in HTTP
     * Basic authentication.
     *
     * @return HTTP Authorization header value, or null
     * if username and password could not be obtained
     */
    synchronized protected String getAuthorizationHeaderValue()
    {
        String[] usernamePassword =
            iDlListener.getUsernamePassword(iDlInfo.getUrl());
        if (usernamePassword == null)
        {
            // If username and password are not available,
            // download cannot proceed.
            Log.log("Downloader: username and password for HTTP authentication not available");
            iState = STATE_STOPPED;
            Installer.cancel();
            return null;
        }
        return "Basic " + getBasicAuthBase64
               (usernamePassword[0], usernamePassword[1]);
    }

    /**
     * Parses HTTP status code from HTTP response status line.
     *
     * @param aStatusLine HTTP status line
     * @return HTTP status code
     */
    protected static int getStatusCode(String aStatusLine)
    {
        if (aStatusLine == null || aStatusLine.length() == 0)
        {
            return 0;
        }
        String[] tokens = Tokenizer.split(aStatusLine, " ");
        if (tokens == null || tokens.length < 2)
        {
            Log.logError("Downloader.getStatusCode: invalid HTTP Status-Line: " +
                         aStatusLine);
            return 0;
        }
        return Integer.parseInt(tokens[1]);
    }

    /**
     * Checks that download size does not exceed the
     * expected download size. Throws InstallerException
     * if the expected size is exceeded.
     */
    protected void checkDownloadSize()
    {
        if (iDlInfo.isDrmContent())
        {
            // If the content is DRM protected its length can
            // exceed the expected value.
            return;
        }
        long expectedSize = iDlInfo.getExpectedSize();
        if (expectedSize > 0)
        {
            boolean incorrectSize = false;
            if (iDlInfo.getCurrentSize() > expectedSize)
            {
                incorrectSize = true;
                Log.logError
                ("Downloader.checkDownloadSize: download size (" +
                 iDlInfo.getCurrentSize() +
                 ") exceeds the expected size (" + expectedSize + ")");
            }
            if (!incorrectSize && iDlInfo.getTotalSize() > 0 &&
                    iDlInfo.getTotalSize() != expectedSize)
            {
                incorrectSize = true;
                Log.logError
                ("Downloader.checkDownloadSize: download total size (" +
                 iDlInfo.getTotalSize() +
                 ") differs from the expected size (" + expectedSize + ")");
            }
            if (incorrectSize)
            {
                throw new InstallerException
                (InstallerErrorMessage.INST_NO_NET, null,
                 InstallerDetailedErrorMessage.ATTR_HANDLING_FAILED,
                 new String[] { "MIDlet-Jar-Size" },
                 OtaStatusCode.JAR_SIZE_MISMATCH);
            }
        }
    }

    /**
     * Checks that there is enough free disk space for the
     * remaining download. Throws appropriate InstallerException
     * if there is not enough free disk space.
     */
    protected void checkDiskSpace()
    {
        int remainingSize =
            (int)(iDlInfo.getExpectedSize() - iDlInfo.getCurrentSize());
        if (remainingSize > 0)
        {
            if (SysUtil.isDiskSpaceBelowCriticalLevel(
                        remainingSize, iDlInfo.getDrive()))
            {
                Log.logError("Disk space below critical level " +
                             "during download, required space " +
                             remainingSize + " bytes, drive " +
                             FileUtils.getDriveName(iDlInfo.getDrive()));
                throw InstallerException.getOutOfDiskSpaceException(
                    remainingSize, null);
            }
        }
    }
}
