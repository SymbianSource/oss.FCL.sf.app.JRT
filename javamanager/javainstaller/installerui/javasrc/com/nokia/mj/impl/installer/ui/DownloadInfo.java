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


package com.nokia.mj.impl.installer.ui;

/**
 * Download info for JavaInstaller UI.
 */
public class DownloadInfo
{
    /** Download url. */
    private String iUrl = null;
    /** Name of the suite to be downloaded.
        In case of a Jad download, this member is null. */
    private String iName = null;
    /** Currently downloaded data size. */
    private long iCurrentSize = 0;
    /** Download total size, or -1 if not known. */
    private long iTotalSize = -1;

    /** Constructor. */
    private DownloadInfo()
    {
    }

    /** Constructor. */
    public DownloadInfo(String aUrl, String aName)
    {
        iUrl = aUrl;
        iName = aName;
    }

    /** Get download url. */
    public String getUrl()
    {
        return iUrl;
    }

    /** Get name for the suite to be downloaded. In case
        of a Jad download, this method returns null. */
    public String getName()
    {
        return iName;
    }

    /** Get currently downloaded data size. */
    public long getCurrentSize()
    {
        return iCurrentSize;
    }

    /** Get download total size, or -1 if not known. */
    public long getTotalSize()
    {
        return iTotalSize;
    }

    /** Set currently downloaded data size. */
    public void setCurrentSize(long aSize)
    {
        iCurrentSize = aSize;
    }

    /** Set download total size. */
    public void setTotalSize(long aSize)
    {
        iTotalSize = aSize;
    }

    public String toString()
    {
        StringBuffer buf = new StringBuffer("DownloadInfo\n");
        buf.append("Url: ").append(getUrl()).append("\n");
        buf.append("Name: ").append(getName()).append("\n");
        buf.append("CurrentSize: ").append(getCurrentSize()).append("\n");
        buf.append("TotalSize: ").append(getTotalSize());
        return buf.toString();
    }
}
