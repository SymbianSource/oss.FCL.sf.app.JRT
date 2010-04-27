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

public class StopApplication extends ExeStep
{
    public void execute(ExeBall aBall)
    {
        InstallBall ball = (InstallBall)aBall;
        if (!ball.iCaptainMsgs)
        {
            // Message sending to JavaCaptain has been disabled.
            return;
        }
        if (ball.iOldSuite != null &&
                ball.iOldSuite.getApplicationUids() != null &&
                ball.iOldSuite.getApplicationUids().length > 0)
        {
            // This is an upgrade, ask JavaCaptain to stop the application.
            ball.log("Stopping application...");
            ball.getCaptainService().
            stopApp(ball.iOldSuite.getApplicationUids());
        }
    }

    public void cancel(ExeBall aBall)
    {
        InstallBall ball = (InstallBall)aBall;
        if (!ball.iCaptainMsgs)
        {
            // Message sending to JavaCaptain has been disabled.
            return;
        }
        if (ball.iOldSuite != null &&
                ball.iOldSuite.getApplicationUids() != null &&
                ball.iOldSuite.getApplicationUids().length > 0)
        {
            // Upgrade has failed, notify JavaCaptain that
            // old version of application still exists.
            ball.getCaptainService().appUpdated(
                null, ball.iOldSuite.getApplicationUids());
        }
    }
}
