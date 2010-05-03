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
import com.nokia.mj.impl.installer.storagehandler.SuiteInfo;
import com.nokia.mj.impl.installer.utils.FileUtils;
import com.nokia.mj.impl.installer.utils.InstallerException;
import com.nokia.mj.impl.installer.utils.Log;
import com.nokia.mj.impl.security.midp.authentication.AuthenticationModule;

public class GetFromStorage extends ExeStep
{
    public void execute(ExeBall aBall)
    {
        InstallBall ball = (InstallBall)aBall;
        ball.log("Reading storage...");

        // Check if application suite can be found from storage.
        ball.iOldSuite = new SuiteInfo(
            ball.getAttributeValue("MIDlet-Name"),
            ball.getAttributeValue("MIDlet-Vendor"));
        if (!ball.iStorageHandler.readSuiteInfo(ball.iOldSuite))
        {
            // Old suite not found.
            ball.iOldSuite = null;
        }
        else if (!ball.iAllowUpgrade)
        {
            // Old suite was found and upgrade is disabled,
            // abort installation.
            InstallerException.internalError("Upgrading is disabled");
        }

        if (ball.iOldSuite != null && !ball.iAllowOverwrite)
        {
            if (ball.iSuite.getVersion().compareTo
                    (ball.iOldSuite.getVersion()) <= 0)
            {
                // Overwriting application with the same or
                // older version is disabled, abort installation.
                InstallerException.internalError(
                    "Overwriting application with the same " +
                    "or older version is disabled.");
            }
        }

        if (ball.iOldSuite != null)
        {
            Log.log("This is update from version " +
                    ball.iOldSuite.getVersion() + " to " +
                    ball.iSuite.getVersion());
            // This is an update, by default install to the same drive
            // where the application already exists except if the drive
            // has been specified as a command line option.
            if (ball.iArgs.get("drive") == null)
            {
                ball.iInstallationDrive =
                    FileUtils.getDrive(ball.iOldSuite.getRootDir());
            }
            // Get certificates for previous version of this application.
            ball.iOldCertificates = AuthenticationModule.getInstance()
                                    .getCertificatesDetails(ball.iStorageHandler.getSession(),
                                                            ball.iOldSuite.getUid());
            if (ball.iOldCertificates != null)
            {
                Log.log("Certificates for old version of this application:");
                for (int i = 0; i < ball.iOldCertificates.length; i++)
                {
                    Log.log("Certificate[" + i + "]:\n" +
                            ball.iOldCertificates[i]);
                }
            }
        }
        else
        {
            Log.log("This is new installation of version " +
                    ball.iSuite.getVersion());
        }
    }

    public void cancel(ExeBall aBall)
    {
        // nop
    }
}
