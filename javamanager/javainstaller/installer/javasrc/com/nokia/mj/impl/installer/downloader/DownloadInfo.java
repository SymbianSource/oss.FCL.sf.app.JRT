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

import com.nokia.mj.impl.installer.utils.InstallerException;
import com.nokia.mj.impl.installer.utils.FileUtils;

import com.nokia.mj.impl.utils.Tokenizer;
import java.util.Date;

/**
 * DownloadInfo contains information of one file download.
 *
 * @author Nokia Corporation
 * @version $Rev: 0 $ $Date$
 */
public class DownloadInfo
{
    // Download URL.
    private String iUrl = null;
    // Local filename for the download.
    private String iFilename = null;
    // Drive where the file is downloaded to.
    private int iDrive = -1;
    // Download content type.
    private String iContentType = null;
    // Download character set.
    private String iCharSet = null;
    // Downloaded file type concluded from content type.
    private String iFileType = null;
    // Flag telling if download content is DRM protected.
    private boolean iDrmContent = false;
    // Currently downloaded amount.
    private long iCurrentSize = 0;
    // Total size of the download.
    private long iTotalSize = -1;
    // Expected download size.
    private long iExpectedSize = -1;
    // Download start time.
    private long iStartTime = 0;
    // Download end time.
    private long iEndTime = 0;
    // Indicates success/error after dl has ended.
    private InstallerException iException = null;

    /** Constructor. */
    private DownloadInfo()
    {
    }

    /**
     * Constructor.
     *
     * @param aUrl download URL
     * @param aFilename name for the downloaded file
     */
    public DownloadInfo(String aUrl, String aFilename)
    {
        if (aUrl == null)
        {
            InstallerException.internalError("DownloadInfo: url is null");
        }
        if (aFilename == null)
        {
            InstallerException.internalError("DownloadInfo: filename is null");
        }
        iUrl = aUrl;
        iFilename = aFilename;
        iDrive = FileUtils.getDrive(iFilename);
    }

    /** Get downlaod url. */
    public String getUrl()
    {
        return iUrl;
    }

    /** Get local filename. */
    public String getFilename()
    {
        return iFilename;
    }

    /** Get drive to which the file is downloaded. */
    public int getDrive()
    {
        return iDrive;
    }

    /**
     * Returns download content type or null if
     * content type is not known.
     */
    public String getContentType()
    {
        return iContentType;
    }

    /**
     * Returns download character set or null if
     * character set is not known.
     */
    public String getCharSet()
    {
        return iCharSet;
    }

    /**
     * Returns downloaded file type concluded from
     * content type or null if file type is not known.
     */
    public String getFileType()
    {
        return iFileType;
    }

    /**
     * Returns true if download content is known to be
     * DRM protected, false otherwise.
     */
    public boolean isDrmContent()
    {
        return iDrmContent;
    }

    /** Set expected download size. */
    public void setExpectedSize(long aSize)
    {
        iExpectedSize = aSize;
    }

    /** Get expected download size. */
    public long getExpectedSize()
    {
        return iExpectedSize;
    }

    /** Get currently downloaded data size. */
    public long getCurrentSize()
    {
        return iCurrentSize;
    }

    /** Get download total size. */
    public long getTotalSize()
    {
        return iTotalSize;
    }

    /** Get download start time. */
    public long getStartTime()
    {
        return iStartTime;
    }

    /** Get download end time. */
    public long getEndTime()
    {
        return iEndTime;
    }

    /**
     * Returns download duration in milliseconds.
     */
    public long getDurationMillis()
    {
        long endTime = System.currentTimeMillis();
        if (getEndTime() != 0)
        {
            // Download has ended.
            endTime = getEndTime();
        }
        return endTime - getStartTime();
    }

    /**
     * Returns download speed in kilobytes/second.
     * If download duration is less than one second this
     * method returns zero.
     */
    public int getSpeed()
    {
        long duration = (getDurationMillis() / 1000);
        if (duration == 0)
        {
            return 0;
        }
        long size = getCurrentSize() / 1024;
        return (int)(size / duration);
    }

    /**
     * Returns download progress as percentage amount.
     * If download total size is not known this method
     * returns -1.
     */
    public int getProgress()
    {
        long totalSize = getTotalSize();
        if (totalSize <= 0)
        {
            return -1;
        }
        long currentSize = getCurrentSize();
        return (int)((currentSize * 100) / totalSize);
    }

    /**
     * Get InstallerException indicating download success or failure.
     * If douwnload was successful, this method returns null, if download
     * failed this method returns exception detailing the error.
     */
    public InstallerException getException()
    {
        return iException;
    }

    /** Set URL. */
    void setUrl(String aUrl)
    {
        iUrl = aUrl;
    }

    /**
     * Set download content type. Sets also character set
     * if found from content type.
     */
    void setContentType(String aContentType)
    {
        iContentType = aContentType;
        if (iContentType != null)
        {
            // Init iCharSet member.
            iCharSet = parseCharSet(iContentType);
            // Init iFileType and iDrmContent members.
            initFileType(iContentType);
        }
    }

    /**
     * Set download character set.
     */
    void setCharSet(String aCharSet)
    {
        iCharSet = aCharSet;
    }

    /** Increase current download amount. */
    void addCurrentSize(long aSize)
    {
        iCurrentSize += aSize;
    }

    /** Set current download amount. */
    void setCurrentSize(long aSize)
    {
        iCurrentSize = aSize;
    }

    /** Set total download size. */
    void setTotalSize(long aSize)
    {
        iTotalSize = aSize;
    }

    /** Set download start time. */
    void setStartTime(long aStartTime)
    {
        iStartTime = aStartTime;
    }

    /** Set download end time. */
    void setEndTime(long aEndTime)
    {
        iEndTime = aEndTime;
    }

    /** Set exception indicating download success or failure. */
    void setException(InstallerException aException)
    {
        iException = aException;
    }

    /**
     * Parses character set from given content type.
     * Returns null if character set cannot be parsed.
     */
    private String parseCharSet(String aContentType)
    {
        if (aContentType == null || aContentType.length() == 0)
        {
            return null;
        }
        String result = null;
        String charsetParam = "charset=";
        String[] tokens = Tokenizer.split(aContentType.toLowerCase(), ";");
        if (tokens != null)
        {
            for (int i = 0; i < tokens.length; i++)
            {
                String token = tokens[i].trim();
                if (token.startsWith(charsetParam))
                {
                    result = token.substring(charsetParam.length());
                }
            }
        }
        return result;
    }

    /**
     * Initialises iFileType and iDrmContent members basing on content type.
     */
    private void initFileType(String aContentType)
    {
        iDrmContent = false;
        iFileType = null;
        int index = -1;
        if (iFileType == null)
        {
            index = iContentType.indexOf("application/java-archive");
            if (index == -1)
            {
                index = iContentType.indexOf("application/x-java-archive");
            }
            if (index == -1)
            {
                index = iContentType.indexOf("application/java");
            }
            if (index != -1)
            {
                // Java archive
                iFileType = ".jar";
            }
        }
        if (iFileType == null)
        {
            index = iContentType.indexOf("text/vnd.sun.j2me.app-descriptor");
            if (index != -1)
            {
                // Java application descriptor
                iFileType = ".jad";
            }
        }
        if (iFileType == null)
        {
            index = iContentType.indexOf("application/vnd.oma.drm.message");
            if (index != -1)
            {
                // DRM message
                iDrmContent = true;
                iFileType = ".dm";
            }
        }
        if (iFileType == null)
        {
            index = iContentType.indexOf("application/vnd.oma.drm.content");
            if (index != -1)
            {
                // DRM content
                iDrmContent = true;
                iFileType = ".dcf";
            }
        }
    }

    /** Returns string representation of this object. */
    public String toString()
    {
        StringBuffer buf = new StringBuffer("DownloadInfo:\n");
        buf.append("URL: ").append(getUrl()).append("\n");
        buf.append("Filename: ").append(getFilename()).append("\n");
        if (getContentType() != null)
        {
            buf.append("ContentType: ").append(getContentType()).append("\n");
            buf.append("FileType: ").append(getFileType()).append("\n");
            buf.append("DrmContent: ").append(isDrmContent()).append("\n");
        }
        if (getCharSet() != null)
        {
            buf.append("CharSet: ").append(getCharSet()).append("\n");
        }
        if (getExpectedSize() > 0)
        {
            buf.append("ExpectedSize: ").append(getExpectedSize()).append("\n");
        }
        buf.append("CurrentSize: ").append(getCurrentSize()).append("\n");
        if (getTotalSize() > 0)
        {
            buf.append("TotalSize: ").append(getTotalSize()).append("\n");
            buf.append("Progress: ").append(getProgress()).append(" %\n");
        }
        if (getStartTime() != 0)
        {
            buf.append("StartTime: ")
            .append(new Date(getStartTime())).append(" (")
            .append(getStartTime()).append(")\n");
        }
        if (getEndTime() != 0)
        {
            buf.append("EndTime: ")
            .append(new Date(getEndTime())).append(" (")
            .append(getEndTime()).append(")\n");
        }
        int speed = getSpeed();
        if (speed != 0)
        {
            buf.append("DownloadSpeed: ").append(speed).append(" kBps\n");
        }
        if (getException() != null)
        {
            buf.append("Exception: ").append(getException()).append("\n");
        }
        return buf.toString();
    }

}
