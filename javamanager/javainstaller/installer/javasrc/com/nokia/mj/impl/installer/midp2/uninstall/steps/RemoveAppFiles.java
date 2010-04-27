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

import com.nokia.mj.impl.installer.applicationregistrator.SifRegistrator;
import com.nokia.mj.impl.installer.exetable.ExeBall;
import com.nokia.mj.impl.installer.exetable.ExeStep;
import com.nokia.mj.impl.installer.utils.FileRoots;
import com.nokia.mj.impl.installer.utils.FileUtils;
import com.nokia.mj.impl.installer.utils.Log;

public class RemoveAppFiles extends ExeStep
{
    public void execute(ExeBall aBall)
    {
        UninstallBall ball = (UninstallBall)aBall;
        ball.log("Removing application files...");
        ball.iIntegrityService.delete(ball.iSuite.getRootDir());

        if (SifRegistrator.getSifMode() > 0)
        {
            // Delete application icons.
            deleteRegisteredIcons(ball);
        }
    }

    public void cancel(ExeBall aBall)
    {
        // nop
    }

    private void deleteRegisteredIcons(UninstallBall aBall)
    {
        String iconFileDir = FileRoots.getRegisteredIconDir(
                                 FileUtils.getDrive(aBall.iSuite.getRootDir())) +
                             aBall.iSuite.getUid().toString();
        boolean result = aBall.iIntegrityService.delete(iconFileDir);
        if (!result)
        {
            Log.logWarning("Deleting application icon dir " +
                           iconFileDir + " failed.");
        }
    }
}
