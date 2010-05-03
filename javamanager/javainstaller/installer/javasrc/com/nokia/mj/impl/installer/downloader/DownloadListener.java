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

/**
 * Downloader will notify DownloadListener about download progress.
 * Note that Downloader will ignore all exceptions thrown from
 * DownloadListener callback methods.
 *
 * @author Nokia Corporation
 * @version $Rev: 0 $ $Date$
 */
public interface DownloadListener
{

    /** Download has started. */
    public void started(DownloadInfo aDlInfo);

    /** Download has progressed. */
    public void updateProgress(DownloadInfo aDlInfo);

    /** Download has ended. */
    public void ended(DownloadInfo aDlInfo);

    /**
     * Downloader needs username and password for HTTP authentication.
     * This method returns an array of two strings, the first being username
     * and second being password. If username and password cannot be obtained,
     * method returns null.
     *
     * @param aUrl URL for which username and password is needed.
     */
    public String[] getUsernamePassword(String aUrl);
}
