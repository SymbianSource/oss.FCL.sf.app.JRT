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

public class AuthenticateJad extends ExeStep
{
    public void execute(ExeBall aBall)
    {
        InstallBall ball = (InstallBall)aBall;
        if (ball.iJadAttributes == null)
        {
            return;
        }

        // Register OCSP listener before Jad authentication.
        // OCSP listener must be unregistered only after also
        // Jar has been authenticated.
        AuthenticationModule.getInstance().registerOcspEventListener(
            ball.iSuite.getUid(), new OcspListener(ball));

        ball.log("Authenticating Jad...");
        // SecurityAttributes is created and Jad attributes are
        // added already in GetAttributes step.
        ball.iAuthenticationCredentials =
            AuthenticationModule.getInstance().authenticateJad
            (ball.iSuite.getUid(),
             (ball.iOldSuite != null? ball.iOldSuite.getUid(): null),
             ball.iSecurityAttributes.getAuthenticationAttributes());
    }

    public void cancel(ExeBall aBall)
    {
        // nop
    }
}
