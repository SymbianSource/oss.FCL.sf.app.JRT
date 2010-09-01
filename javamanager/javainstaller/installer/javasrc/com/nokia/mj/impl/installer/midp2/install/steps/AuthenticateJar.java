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
import com.nokia.mj.impl.installer.utils.Log;
import com.nokia.mj.impl.security.midp.authentication.AuthenticationModule;
import com.nokia.mj.impl.installer.storagehandler.SuiteInfo;

public class AuthenticateJar extends ExeStep
{
    public void execute(ExeBall aBall)
    {
        InstallBall ball = (InstallBall)aBall;
        if (ball.iJarAttributes == null || ball.iJarAuthenticated)
        {
            return;
        }

        ball.log("Authenticating Jar...");
        AuthenticationModule.getInstance().authenticateJar
        (ball.iStorageHandler.getSession(),
         ball.iSuite.getUid(),
         (ball.iOldSuite != null? ball.iOldSuite.getUid(): null),
         ball.iJarFilename,
         (ball.iSuite.getContentInfo() == SuiteInfo.CONTENT_INFO_DRM? true: false));
        ball.iJarAuthenticated = true;
        // Unregister OCSP listener after Jar authentication.
        AuthenticationModule.getInstance().unregisterOcspEventListener(
            ball.iSuite.getUid());
    }

    public void cancel(ExeBall aBall)
    {
        // nop
    }
}
