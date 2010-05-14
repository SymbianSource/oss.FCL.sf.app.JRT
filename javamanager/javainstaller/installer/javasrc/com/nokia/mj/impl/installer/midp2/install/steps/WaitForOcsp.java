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
import com.nokia.mj.impl.installer.utils.Log;
import com.nokia.mj.impl.utils.exception.InstallerExceptionBase;

/**
 * Installation step WaitForOcsp blocks and waits until
 * certificate validation (OCSP) has been completed.
 */
public class WaitForOcsp extends ExeStep
{
    public void execute(ExeBall aBall)
    {
        InstallBall ball = (InstallBall)aBall;
        synchronized (ball)
        {
            if (ball.getInstallerUi() != null &&
                    ball.getInstallerUi().getOcspIndicator())
            {
                // OCSP is ongoing, wait for it to complete.
                try
                {
                    ball.wait();
                }
                catch (InterruptedException ie)
                {
                }
            }
        }
        if (ball.iOcspException != null && !ball.isCancelled())
        {
            Log.logError("Error during OCSP: " + ball.iOcspException);
            if (ball.iOcspException instanceof InstallerExceptionBase)
            {
                throw(InstallerExceptionBase)ball.iOcspException;
            }
            else
            {
                InstallerException.internalError(
                    "Unexpected exception during OCSP", ball.iOcspException);
            }
        }
    }

    public void cancel(ExeBall aBall)
    {
        // nop
    }
}
