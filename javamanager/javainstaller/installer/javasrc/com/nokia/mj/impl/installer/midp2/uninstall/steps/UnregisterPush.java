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


package com.nokia.mj.impl.installer.midp2.uninstall.steps;

import com.nokia.mj.impl.installer.exetable.ExeBall;
import com.nokia.mj.impl.installer.exetable.ExeStep;
import com.nokia.mj.impl.installer.pushregistrator.PushInfo;
import com.nokia.mj.impl.installer.pushregistrator.PushRegistrator;
import com.nokia.mj.impl.installer.utils.InstallerException;
import com.nokia.mj.impl.installer.utils.Log;

public class UnregisterPush extends ExeStep
{
    public void execute(ExeBall aBall)
    {
        UninstallBall ball = (UninstallBall)aBall;

        if (ball.iStorageHandler.getSession() == null)
        {
            ball.log("No StorageSession, skipping push unregistration");
            return;
        }

        PushRegistrator pushRegistrator = null;
        try
        {
            // Instantiate PushRegistrator implementation.
            Class clazz = Class.forName(PushInfo.PUSH_REGISTRATOR_CLASS);
            pushRegistrator = (PushRegistrator)clazz.newInstance();
        }
        catch (Exception ex)
        {
            InstallerException.internalError(
                "Instantiating PushRegistrator " +
                PushInfo.PUSH_REGISTRATOR_CLASS + " failed", ex);
        }

        ball.log("Removing info from push registery...");
        pushRegistrator.unregisterPush(
            ball.iStorageHandler.getSession(),
            ball.iSuite.getUid(),
            ball.iSuite.getApplicationUids());
    }

    public void cancel(ExeBall aBall)
    {
        // nop
    }
}
