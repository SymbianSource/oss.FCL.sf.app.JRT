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
import com.nokia.mj.impl.installer.jadjarmatcher.JadJarFile;
import com.nokia.mj.impl.installer.jadjarmatcher.JadJarMatcher;
import com.nokia.mj.impl.installer.jadjarmatcher.MidletMessageHandler;
import com.nokia.mj.impl.utils.InstallerDetailedErrorMessage;
import com.nokia.mj.impl.utils.InstallerErrorMessage;
import com.nokia.mj.impl.utils.OtaStatusCode;

// Either Jad or Jar must be present in the device at this point,
// otherwise this step will throw an exception.
// After this step Jad and Jar paths and urls are initialized.
public class FindJadJar extends ExeStep
{
    public void execute(ExeBall aBall)
    {
        InstallBall ball = (InstallBall)aBall;

        // Initialise JadJarMatcher classes.
        JadJarMatcher.setIntegrityService(ball.iIntegrityService);
        MidletMessageHandler.setInstallationDrive(ball.iInstallationDrive);

        // Try to find jar basing on local jad file.
        if (ball.iJarFilename == null && ball.iJadFilename != null)
        {
            JadJarFile jadJarFile =
                JadJarMatcher.findJar(ball.iJadFilename, ball.iJadCharset);
            ball.iJadFilename = jadJarFile.iJadFilename;
            ball.iJarFilename = jadJarFile.iJarFilename;
            ball.iJadAttributes = jadJarFile.iJadAttributes;
            ball.iJarAttributes = jadJarFile.iJarAttributes;
            if (ball.iJarFilename != null)
            {
                ball.log("Found matching Jar: " + ball.iJarFilename);
            }
            else
            {
                ball.log("Matching Jar not found locally");
            }
        }

        // Try to find jad basing on local jar file.
        if (ball.iJadFilename == null && ball.iJarFilename != null)
        {
            JadJarFile jadJarFile =
                JadJarMatcher.findJad(ball.iJarFilename);
            ball.iJadFilename = jadJarFile.iJadFilename;
            ball.iJarFilename = jadJarFile.iJarFilename;
            ball.iJadAttributes = jadJarFile.iJadAttributes;
            ball.iJarAttributes = jadJarFile.iJarAttributes;
            if (ball.iJadFilename != null)
            {
                ball.log("Found matching Jad: " + ball.iJadFilename);
                // If the installation package is a midlet message,
                // installation file can be moved.
                ball.iMoveInstallationFile = jadJarFile.iMidletMessage;
            }
            else
            {
                ball.log("Matching Jad not found locally");
            }
        }

        if (ball.iJarFilename == null && ball.iJarUrl == null)
        {
            // No jar found, we must have jad at this point otherwise
            // installation cannot proceed further.
            if (ball.iJadFilename == null)
            {
                throw new InstallerException
                (InstallerErrorMessage.INST_CORRUPT_PKG, null,
                 InstallerDetailedErrorMessage.INTERNAL_ERROR,
                 new String[] { "No Jad or Jar file found" },
                 OtaStatusCode.INVALID_JAR);
            }
        }
    }

    public void cancel(ExeBall aBall)
    {
        // nop
    }
}
