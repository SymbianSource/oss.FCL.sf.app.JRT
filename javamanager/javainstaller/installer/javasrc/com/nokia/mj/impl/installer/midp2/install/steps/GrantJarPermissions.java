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
import com.nokia.mj.impl.security.midp.authorization.PermissionGranter;
import com.nokia.mj.impl.security.midp.common.SecurityAttributes;
import com.nokia.mj.impl.utils.Attribute;

import java.util.Enumeration;
import java.util.Hashtable;

public class GrantJarPermissions extends ExeStep
{
    public void execute(ExeBall aBall)
    {
        InstallBall ball = (InstallBall)aBall;
        if (ball.iJarAttributes == null || ball.iJarPermissionsGranted)
        {
            return;
        }

        ball.log("Granting Jar permissions...");
        if (ball.iSecurityAttributes == null)
        {
            ball.iSecurityAttributes = new SecurityAttributes();
        }
        ball.iSecurityAttributes.addManifestAttributes(ball.iJarAttributes);
        PermissionGranter.getInstance().grantJarPermissions
        (ball.iSuite.getUid(),
         (ball.iOldSuite != null? ball.iOldSuite.getUid(): null),
         ball.iSecurityAttributes.getPermissionAttributes(),
         ball.iAuthenticationCredentials);
        ball.iJarPermissionsGranted = true;
    }

    public void cancel(ExeBall aBall)
    {
        // nop
    }
}
