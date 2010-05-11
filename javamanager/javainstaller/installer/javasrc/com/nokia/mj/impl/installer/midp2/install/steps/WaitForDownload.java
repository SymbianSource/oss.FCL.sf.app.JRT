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

import com.nokia.mj.impl.installer.exetable.ExeBall;
import com.nokia.mj.impl.installer.exetable.ExeStep;
import com.nokia.mj.impl.installer.utils.InstallerException;
import com.nokia.mj.impl.installer.utils.FileUtils;
import com.nokia.mj.impl.installer.utils.Log;

/**
 * Installation step WaitForDownload blocks and waits until
 * donwload gets completed.
 */
public class WaitForDownload extends ExeStep
{
    public void execute(ExeBall aBall)
    {
        InstallBall ball = (InstallBall)aBall;
        if (ball.iDownloader != null)
        {
            try
            {
                // Wait for download completion.
                ball.iDownloader.waitForCompletion();
                ball.iDownloader = null;
                // Check if there was an exception during download.
                InstallerException dlException =
                    ball.iDlListener.getDlInfo().getException();
                // If there was an exception during download, throw it.
                if (dlException != null)
                {
                    throw dlException;
                }
                if (!ball.isCancelled())
                {
                    // No exception and installation has not been cancelled.
                    ball.iDownloadSucceeded = true;
                }
                // Save file type of the downloaded file.
                ball.iJarFileType = ball.iDlListener.getDlInfo().getFileType();
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
                // If the downloaded file is DRM protected, set content info.
                if (ball.iDlListener.getDlInfo().isDrmContent())
                {
                    ball.iSuite.setContentInfo(ball.iSuite.CONTENT_INFO_DRM);
                    Log.log("Downloaded suite is DRM protected");
                }
                // Init suite jar filename with temp file name
                // instead of possibly cryptic name from url.
                ball.iSuite.setJarPath(ball.iSuite.getRootDir() +
                                       FileUtils.getName(ball.iJarFilename));
                Log.log("Jar path in suite: " + ball.iSuite.getJarPath());
            }
            catch (InstallerException iex)
            {
                throw iex;
            }
            catch (Exception ex)
            {
                InstallerException.internalError
                ("Unexpected error while waiting for download", ex);
            }
            if (ball.iDownloadSucceeded)
            {
                // Start to post pending OTA status notifications after
                // download has been completed.
                ball.startToPostPendingNotifications();
            }
        }
    }

    public void cancel(ExeBall aBall)
    {
        // nop
    }
}
