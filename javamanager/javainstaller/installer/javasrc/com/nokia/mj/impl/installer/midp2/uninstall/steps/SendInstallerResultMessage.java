/*
* Copyright (c) 2008-2010 Nokia Corporation and/or its subsidiary(-ies).
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

import com.nokia.mj.impl.installer.Installer;
import com.nokia.mj.impl.installer.InstallerResultMessage;
import com.nokia.mj.impl.installer.exetable.ExeBall;
import com.nokia.mj.impl.installer.exetable.ExeStep;
import com.nokia.mj.impl.installer.utils.InstallerException;
import com.nokia.mj.impl.installer.utils.Log;

/**
 * Installation step for sending InstallerResultMessage.
 *
 * @author Nokia Corporation
 * @version $Rev:  $ $Date:  $
 */
public class SendInstallerResultMessage extends ExeStep
{
    public void execute(ExeBall aBall)
    {
        UninstallBall ball = (UninstallBall)aBall;
        if (ball.iSuite == null)
        {
            // Do nothing when called as the first installation step.
            return;
        }
        if (ball.iSifNotifier == null &&
                (ball.iCommsResultEndpoints == null ||
                 ball.iCommsResultEndpoints.length == 0))
        {
            return;
        }
        try
        {
            InstallerResultMessage msg = new InstallerResultMessage();
            msg.setSifNotifier(ball.iSifNotifier);
            msg.addValue(msg.NAME_OPERATION, 1);
            msg.addValue(msg.NAME_RESULT, Installer.ERR_NONE);
            msg.init(ball.iSuite);
            Log.log("Sending " + msg.toString());
            msg.send(ball.iCommsResultEndpoints);
        }
        catch (Throwable t)
        {
            Log.logError("Sending InstallerResultMessage failed", t);
        }
    }

    public void cancel(ExeBall aBall)
    {
        UninstallBall ball = (UninstallBall)aBall;
        // Cancel method for the last execution step never gets called,
        // so when this method is called we are cancelling the first
        // step and we should indicate that installation has failed.
        if (ball.iSifNotifier == null &&
                (ball.iCommsResultEndpoints == null ||
                 ball.iCommsResultEndpoints.length == 0))
        {
            return;
        }
        try
        {
            InstallerResultMessage msg = new InstallerResultMessage();
            msg.setSifNotifier(ball.iSifNotifier);
            msg.addValue(msg.NAME_OPERATION, 1);
            msg.addResult(ball.getExecuteException());
            Log.log("Sending " + msg.toString());
            msg.send(ball.iCommsResultEndpoints);
        }
        catch (Throwable t)
        {
            Log.logError("Sending InstallerResultMessage failed", t);
        }
    }
}
