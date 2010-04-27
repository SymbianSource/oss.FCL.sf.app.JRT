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


package com.nokia.mj.impl.installer.midp2.install.steps;

import com.nokia.mj.impl.security.midp.authentication.OcspEventListener;
import com.nokia.mj.impl.installer.ui.InstallerUi;
import com.nokia.mj.impl.installer.utils.Log;

/**
 * OcspEventListener implementation.
 */
public class OcspListener implements OcspEventListener
{
    private InstallBall iBall = null;
    // InstallerUI instance.
    private InstallerUi iInstallerUi = null;

    OcspListener(InstallBall aBall)
    {
        iBall = aBall;
        if (iBall != null)
        {
            iInstallerUi = iBall.getInstallerUi();
        }
    }

    /**
     * Callback upon different ocsp events
     *
     * @param aEventId    Identifier for the ocsp event being propagated
     *                   (OCSP_START_EVENT or OCSP_FINISH_EVENT)
     * @param aOcspException If there are any errors related to the ocsp event,
     *                   they are propagated as well via this parameter.
     *                   If there are no errors to be signaled, this parameter
     *                   is null
     */
    public void ocspEvent(int aEventId, Throwable aOcspException)
    {
        Log.log("OcspListener.ocspEvent: " + aEventId);
        // Save possible OCSP exception.
        if (aOcspException != null)
        {
            iBall.iOcspException = aOcspException;
            Log.log("OcspListener.ocspEvent exception: " + aOcspException);
        }
        if (aEventId == OcspEventListener.OCSP_START_EVENT)
        {
            if (iInstallerUi != null)
            {
                // Ask javainstalllauncher to stop displaying
                // 'Preparing installation' dialog.
                iBall.iApplicationRegistrator.notifyLauncherThatUiIsReady();
                try
                {
                    iInstallerUi.setOcspIndicator(true);
                }
                catch (Throwable t)
                {
                    Log.logError("Exception from InstallerUi.setOcspIndicator", t);
                }
            }
            if (iBall.iSifNotifier != null)
            {
                Log.log("OcspListener.ocspEvent: update SifNotifier to 0 / 0");
                try
                {
                    iBall.iSifNotifier.notifyProgress(
                        iBall.iSifNotifier.SUB_OP_OCSP, 0, 0);
                }
                catch (Throwable t)
                {
                    Log.logError("SifNotifier.notifyProgress threw exception", t);
                }
            }
        }
        else if (aEventId == OcspEventListener.OCSP_FINISH_EVENT)
        {
            synchronized (iBall)
            {
                if (iInstallerUi != null)
                {
                    try
                    {
                        iInstallerUi.setOcspIndicator(false);
                    }
                    catch (Throwable t)
                    {
                        Log.logError("Exception from InstallerUi.setOcspIndicator", t);
                    }
                }
                if (iBall.iSifNotifier != null)
                {
                    Log.log("OcspListener.ocspEvent: update SifNotifier to 100 / 100");
                    try
                    {
                        iBall.iSifNotifier.notifyProgress(
                            iBall.iSifNotifier.SUB_OP_OCSP, 100, 100);
                    }
                    catch (Throwable t)
                    {
                        Log.logError("SifNotifier.notifyProgress threw exception", t);
                    }
                }
                // Notify InstallBall so that WaitForOcsp step can proceed.
                iBall.notify();
            }
        }
        else
        {
            Log.logError("OcspListener.ocspEvent: Unknown event: " + aEventId);
        }
    }
}
