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
import com.nokia.mj.impl.installer.storagehandler.SuiteInfo;
import com.nokia.mj.impl.installer.utils.InstallerException;
import com.nokia.mj.impl.installer.utils.Log;
import com.nokia.mj.impl.utils.Uid;

public class GetFromStorage extends ExeStep
{
    public void execute(ExeBall aBall)
    {
        UninstallBall ball = (UninstallBall)aBall;
        ball.log("Reading storage...");

        // Check if application suite can be found from storage.
        Uid uid = ball.iStorageHandler.getSuiteUid(ball.iUid);
        if (uid == null)
        {
            // Suite not found.
            InstallerException.internalError
            ("Application with uid " +
             ball.iUid + " not found from JavaStorage");
        }

        ball.iSuite = new SuiteInfo(uid);
        if (!ball.iStorageHandler.readSuiteInfo(ball.iSuite))
        {
            // Suite could not be read.
            InstallerException.internalError
            ("Reading suite with uid " +
             uid + " from JavaStorage failed");
        }

        // Check if debug attribute has been defined for this suite.
        ball.setDebug(ball.iSuite.getAttributeValue("Nokia-MIDlet-Install-Debug"));

        // Check from arguments if preinstall state should be reset.
        String arg = ball.iArgs.get("resetpreinstall");
        if (arg != null)
        {
            ball.iSuite.setResetPreinstall(true);
            ball.log("ResetPreinstall set");
        }


        // Log all suite info.
        //ball.log(ball.iSuite.toString());
    }

    public void cancel(ExeBall aBall)
    {
        // nop
    }
}
