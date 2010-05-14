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


package com.nokia.mj.impl.installer.midp2.install.steps;

import com.nokia.mj.impl.installer.downloader.DownloadListener;
import com.nokia.mj.impl.installer.downloader.DownloadInfo;
import com.nokia.mj.impl.installer.ui.InstallerUi;
import com.nokia.mj.impl.installer.utils.InstallerException;
import com.nokia.mj.impl.installer.utils.Log;

/**
 * DownloadListener implementation for MIDP2 jad and jar downloads.
 */
public class DlListener implements DownloadListener
{
    private InstallBall iBall = null;
    private DownloadInfo iDlInfo = null;
    // InstallerUI instance.
    private InstallerUi iInstallerUi = null;

    DlListener(InstallBall aBall)
    {
        iBall = aBall;
        if (iBall != null)
        {
            iInstallerUi = iBall.getInstallerUi();
        }
    }

    /** Download has started. */
    public void started(DownloadInfo aDlInfo)
    {
        iDlInfo = aDlInfo;
        iBall.log("Downloading " + iDlInfo.getUrl());
        if (iInstallerUi != null)
        {
            try
            {
                iInstallerUi.started(getUiDlInfo(aDlInfo));
            }
            catch (Throwable t)
            {
                Log.logError(
                    "DlListener: InstallerUi.started threw exception", t);
            }
        }
    }

    /** Download has progressed. */
    public void updateProgress(DownloadInfo aDlInfo)
    {
        iDlInfo = aDlInfo;
        iBall.log("Downloaded " + iDlInfo.getProgress() + " %");
        if (iInstallerUi != null)
        {
            try
            {
                iInstallerUi.updateProgress(getUiDlInfo(aDlInfo));
            }
            catch (Throwable t)
            {
                Log.logError(
                    "DlListener: InstallerUi.updateProgress threw exception", t);
            }
        }
        if (iBall.iSifNotifier != null)
        {
            Log.log("DlListener.set: update SifNotifier to " +
                    aDlInfo.getCurrentSize() + " / " + aDlInfo.getTotalSize());
            try
            {
                iBall.iSifNotifier.notifyProgress(
                    iBall.iSifNotifier.SUB_OP_DOWNLOAD,
                    (int)aDlInfo.getCurrentSize(),
                    (int)aDlInfo.getTotalSize());
            }
            catch (Throwable t)
            {
                Log.logError(
                    "DlListener: SifNotifier.notifyProgress threw exception", t);
            }
        }
    }

    /** Download has ended. */
    public void ended(DownloadInfo aDlInfo)
    {
        iDlInfo = aDlInfo;
        if (iInstallerUi != null)
        {
            try
            {
                iInstallerUi.ended(getUiDlInfo(aDlInfo));
            }
            catch (Throwable t)
            {
                Log.logError(
                    "DlListener: InstallerUi.ended threw exception", t);
            }
        }
        if (iBall != null)
        {
            String dlInfoStr = "\n" + iDlInfo.toString();
            iBall.addTimesMsg(dlInfoStr);
        }
    }

    /**
     * Downloader needs username and password for HTTP authentication.
     * This method returns an array of two strings, the first being username
     * and second being password. If username and password cannot be obtained,
     * method returns null.
     *
     * @param aUrl URL for which username and password is needed.
     */
    public String[] getUsernamePassword(String aUrl)
    {
        String[] result = null;
        if (iBall != null &&
                iBall.iDownloadUsername != null &&
                iBall.iDownloadPassword != null)
        {
            result = new String[]
            {
                iBall.iDownloadUsername, iBall.iDownloadPassword
            };
        }
        if (result == null && iInstallerUi != null)
        {
            try
            {
                result = iInstallerUi.getUsernamePassword(aUrl);
            }
            catch (Throwable t)
            {
                Log.logError(
                    "DlListener: InstallerUi.getUsernamePassword threw exception", t);
            }
        }
        return result;
    }

    /**
     * Get last notified DownloadInfo.
     */
    DownloadInfo getDlInfo()
    {
        return iDlInfo;
    }

    /**
     * Returns Downloadinfo object used in the InstallerUi API.
     */
    private com.nokia.mj.impl.installer.ui.DownloadInfo getUiDlInfo
    (DownloadInfo aDlInfo)
    {
        String name = null;
        if (iBall.iSuite != null)
        {
            name = iBall.iSuite.getName();
        }
        com.nokia.mj.impl.installer.ui.DownloadInfo result =
            new com.nokia.mj.impl.installer.ui.DownloadInfo
        (aDlInfo.getUrl(), name);
        result.setCurrentSize(aDlInfo.getCurrentSize());
        result.setTotalSize(aDlInfo.getTotalSize());
        return result;
    }
}
