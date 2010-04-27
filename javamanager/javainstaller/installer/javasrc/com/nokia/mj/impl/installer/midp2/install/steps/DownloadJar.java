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


package com.nokia.mj.impl.installer.midp2.install.steps;

import com.nokia.mj.impl.installer.downloader.Downloader;
import com.nokia.mj.impl.installer.downloader.DownloadInfo;
import com.nokia.mj.impl.installer.exetable.ExeBall;
import com.nokia.mj.impl.installer.exetable.ExeStep;
import com.nokia.mj.impl.installer.utils.FileUtils;
import com.nokia.mj.impl.installer.utils.InstallerException;
import com.nokia.mj.impl.installer.utils.Log;
import com.nokia.mj.impl.utils.InstallerErrorMessage;
import com.nokia.mj.impl.utils.InstallerDetailedErrorMessage;
import com.nokia.mj.impl.utils.OtaStatusCode;

/**
 * Installation step DownloadJar takes care of downloading Jar file.
 *
 * @author Nokia Corporation
 * @version $Rev:  $ $Date:  $
 */
public class DownloadJar extends ExeStep
{
    public void execute(ExeBall aBall)
    {
        InstallBall ball = (InstallBall)aBall;
        if (ball.iJarFilename != null)
        {
            // Jar file is already present, no need to download it.
            return;
        }
        if (ball.iDownloader != null)
        {
            // Download has already been started, do not start it again.
            return;
        }

        if (ball.iJarUrl == null)
        {
            // Deduct jar url from jad file MIDlet-Jar-URL
            // attribute and possibly from jad and sourceurl
            // command line options. Jar url must be deducted
            // before user confirmation dialog is displayed so
            // that if there is an error in the url it is
            // displayed to user before confirmation is asked.
            String attrName = "MIDlet-Jar-URL";
            ball.iJarUrl = ball.getAttributeValue(attrName);
            if (!Downloader.isDownloadUrl(ball.iJarUrl))
            {
                if (!Downloader.isDownloadUrl(ball.iSourceUrl))
                {
                    ball.iSourceUrl = ball.iJadUrl;
                }
                if (Downloader.isDownloadUrl(ball.iSourceUrl))
                {
                    int lastIndex = ball.iSourceUrl.lastIndexOf('/');
                    if (lastIndex >= 0)
                    {
                        ball.iJarUrl =
                            ball.iSourceUrl.substring(0, lastIndex + 1) +
                            ball.iJarUrl;
                    }
                }
                else
                {
                    // Couldn't deduct valid download url.
                    throw new InstallerException
                    (InstallerErrorMessage.INST_NO_NET, null,
                     InstallerDetailedErrorMessage.ATTR_HANDLING_FAILED,
                     new String[] { attrName },
                     OtaStatusCode.INVALID_DESCRIPTOR);
                }
            }
        }

        if ((ball.iIap == null || ball.iIap.equals("0")) &&
                (ball.iSnap == null || ball.iSnap.equals("0")) &&
                ball.iUserConfirmation == null)
        {
            // Download can only be started if IAP is known, or if the
            // user has answered to installation confirmation dialog.
            return;
        }

        if (!ball.iAllowDownload)
        {
            InstallerException.internalError("Downloading is disabled");
        }

        if (ball.iJarUrl != null)
        {
            try
            {
                // Get temp file name for jar file download.
                // Jar file must be downloaded to temp file
                // because old jar file can still be in use.
                ball.iJarFilename = ball.iIntegrityService
                                    .createTempFile(ball.iInstallationDrive, ".tmp");
                // Downloaded file will be moved, not copied.
                ball.iMoveInstallationFile = true;
                // Start download.
                if (ball.iDlListener == null)
                {
                    ball.iDlListener = new DlListener(ball);
                }
                ball.iDownloader = ball.getDownloader(ball.iDlListener);
                DownloadInfo dlInfo = new DownloadInfo
                (ball.iJarUrl, ball.iJarFilename);
                // Set expected download size.
                String jarSize = "MIDlet-Jar-Size";
                if (ball.attributeExists(jarSize))
                {
                    dlInfo.setExpectedSize(ball.getAttributeIntValue(jarSize));
                }
                ball.iDownloader.start(dlInfo);
                // Uncomment next line if posting of pending OTA status
                // notifications are to be made in parallel with jar download.
                //ball.startToPostPendingNotifications();
                // Download completion is waited in separate
                // WaitForDownload step.
            }
            catch (Exception ex)
            {
                InstallerException.internalError
                ("Unexpected error during Jar download", ex);
            }
        }
    }

    public void cancel(ExeBall aBall)
    {
        // nop
    }
}
