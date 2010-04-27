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

import com.nokia.mj.impl.installer.downloader.Downloader;
import com.nokia.mj.impl.installer.downloader.DownloadInfo;
import com.nokia.mj.impl.installer.exetable.ExeBall;
import com.nokia.mj.impl.installer.exetable.ExeStep;
import com.nokia.mj.impl.installer.utils.InstallerException;
import com.nokia.mj.impl.installer.utils.Log;

/**
 * Installation step DownloadJad takes care of downloading Jad
 * file if Jad url is specified in the command line arguments.
 * This is mostly used in Linux platform because in S60 platform
 * either jad or Jar is already present in the device when
 * installation is started.
 *
 * @author Nokia Corporation
 * @version $Rev:  $ $Date:  $
 */
public class DownloadJad extends ExeStep
{
    public void execute(ExeBall aBall)
    {
        InstallBall ball = (InstallBall)aBall;

        if ((ball.iIap != null && !ball.iIap.equals("0")) ||
                (ball.iSnap != null && !ball.iSnap.equals("0")))
        {
            // If access point is specified start to post
            // pending OTA status notifications.
            // Uncomment next line if posting of pending OTA status
            // notifications are to be made in parallel with download.
            //ball.startToPostPendingNotifications();
        }

        // Download jad if jad url is specified with "-jad" option.
        if (ball.iJadUrl != null)
        {
            if (!ball.iAllowDownload)
            {
                InstallerException.internalError("Downloading is disabled");
            }
            // Get temp file name for jad file download.
            // Jad file must be downloaded to temp file
            // because uids have not yet been selected.
            ball.iJadFilename = ball.iIntegrityService
                                .createTempFile(ball.iInstallationDrive, ".jad");
            download(ball, ball.iJadUrl, ball.iJadFilename, true);

        }

        // If jar url is specified with "-jar" option, download also it.
        if (ball.iJarUrl != null)
        {
            if (!ball.iAllowDownload)
            {
                InstallerException.internalError("Downloading is disabled");
            }
            // Get temp file name for jar file download.
            // Jar file must be downloaded to temp file
            // because uids have not yet been selected.
            ball.iJarFilename = ball.iIntegrityService
                                .createTempFile(ball.iInstallationDrive, ".tmp");
            // Downloaded file will be moved, not copied.
            ball.iMoveInstallationFile = true;
            download(ball, ball.iJarUrl, ball.iJarFilename, false);
            if (ball.iJarFileType != null)
            {
                // The downloaded file must be renamed with correct
                // extension because DRM protected files cannot be
                // read if their extension does not indicate that
                // they are DRM protected.
                String newJarFilename = ball.iJarFilename + ball.iJarFileType;
                boolean fileOpResult = ball.iIntegrityService.move
                                       (ball.iJarFilename, newJarFilename);
                if (!fileOpResult)
                {
                    InstallerException.internalError
                    ("Moving file " + ball.iJarFilename +
                     " to " + newJarFilename + " failed.");
                }
                ball.iJarFilename = newJarFilename;
            }
        }
    }

    public void cancel(ExeBall aBall)
    {
        // nop
    }

    private void download(InstallBall aBall, String aUrl,
                          String aFilename, boolean aJad)
    {
        InstallerException dlException = null;
        try
        {
            // Start download.
            if (aBall.iDlListener == null)
            {
                aBall.iDlListener = new DlListener(aBall);
            }
            aBall.iDownloader = aBall.getDownloader(aBall.iDlListener);
            DownloadInfo dlInfo = new DownloadInfo(aUrl, aFilename);
            aBall.iDownloader.start(dlInfo);
            // Uncomment next line if posting of pending OTA status
            // notifications are to be made in parallel with download.
            //aBall.startToPostPendingNotifications();
            // Wait for download completion.
            aBall.iDownloader.waitForCompletion();
            aBall.iDownloader = null;
            // Check if there was an exception during download.
            dlException = dlInfo.getException();
            if (dlException == null && !aBall.isCancelled())
            {
                // No exception and installation has not been cancelled.
                aBall.iDownloadSucceeded = true;
            }
            if (aJad && aBall.iJadCharset == null)
            {
                // Get Jad charset if not overridden from command line.
                aBall.iJadCharset = dlInfo.getCharSet();
            }
            if (!aJad)
            {
                aBall.iJarFileType = dlInfo.getFileType();
            }
        }
        catch (Exception ex)
        {
            InstallerException.internalError
            ("Unexpected error during download", ex);
        }
        // If there was an exception during download, throw it.
        if (dlException != null)
        {
            throw dlException;
        }
    }
}
