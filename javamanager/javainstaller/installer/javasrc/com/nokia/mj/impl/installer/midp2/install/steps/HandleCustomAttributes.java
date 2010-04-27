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
import com.nokia.mj.impl.installer.storagehandler.ApplicationInfo;
import com.nokia.mj.impl.installer.storagehandler.SuiteInfo;
import com.nokia.mj.impl.installer.utils.Log;
import com.nokia.mj.impl.utils.InstallerDetailedErrorMessage;
import com.nokia.mj.impl.utils.InstallerErrorMessage;
import com.nokia.mj.impl.utils.OtaStatusCode;
import com.nokia.mj.impl.utils.exception.InvalidAttributeException;

import java.util.Vector;

public class HandleCustomAttributes extends ExeStep
{
    public void execute(ExeBall aBall)
    {
        InstallBall ball = (InstallBall)aBall;

        // Check if debug attribute has been defined for this suite.
        aBall.setDebug(ball.getAttributeValue("Nokia-MIDlet-Install-Debug"));

        handleNokiaMidletOnScreenKeypad(ball);
        handleNokiaMidletAutoStart(ball);
    }

    public void cancel(ExeBall aBall)
    {
        // nop
    }

    /** Handles Nokia-MIDlet-On-Screen-Keypad attribute. */
    private void handleNokiaMidletOnScreenKeypad(InstallBall aBall)
    {
        if (aBall.iApplicationRegistrator.isOnDeviceKeypadNeeded())
        {
            // default value when on-screen-keypad is needed
            aBall.iSuite.setOnScreenKeypad(SuiteInfo.OSK_GAMEACTIONS);

            String attrName = "Nokia-MIDlet-On-Screen-Keypad";
            String attrValue = aBall.getAttributeValue(attrName);
            if (attrValue != null)
            {
                if (attrValue.equalsIgnoreCase("no"))
                {
                    aBall.iSuite.setOnScreenKeypad(SuiteInfo.OSK_NO);
                }
                else if (attrValue.equalsIgnoreCase("navigationkeys"))
                {
                    aBall.iSuite.setOnScreenKeypad(SuiteInfo.OSK_NAVIKEYS);
                }
                else if (attrValue.equalsIgnoreCase("gameactions"))
                {
                    aBall.iSuite.setOnScreenKeypad(SuiteInfo.OSK_GAMEACTIONS);
                }
                else
                {
                    Log.logError("Invalid " + attrName + " value " + attrValue);
                    throw new InvalidAttributeException
                    (InstallerErrorMessage.INST_CORRUPT_PKG, null,
                     InstallerDetailedErrorMessage.ATTR_UNSUPPORTED,
                     new String[] { attrName },
                     (aBall.attributeExistsInJad(attrName)?
                      OtaStatusCode.INVALID_DESCRIPTOR:
                      OtaStatusCode.INVALID_JAR));
                }
            }
        }
        else
        {
            // If the device does not need on-screen-keypad, the value is not set at all
            aBall.iSuite.setOnScreenKeypad(SuiteInfo.OSK_UNDEFINED);
        }

    }

    /** Handle Nokia-MIDlet-Auto-Start-<n> attribute. */
    private void handleNokiaMidletAutoStart(InstallBall aBall)
    {
        Vector apps = aBall.iSuite.getApplications();
        if (apps == null)
        {
            return;
        }
        for (int i = 1; true; i++)
        {
            String attrName = "Nokia-MIDlet-Auto-Start-" + i;
            String attrValue = aBall.getAttributeValue(attrName);
            if (attrValue != null)
            {
                if (apps.size() < i)
                {
                    break;
                }
                if (attrValue.equalsIgnoreCase("false"))
                {
                    ((ApplicationInfo)apps.elementAt(i-1)).
                    setAutoStart(ApplicationInfo.AUTOSTART_FALSE);
                }
                else if (attrValue.equalsIgnoreCase("true"))
                {
                    ((ApplicationInfo)apps.elementAt(i-1)).
                    setAutoStart(ApplicationInfo.AUTOSTART_TRUE);
                }
                else if (attrValue.equalsIgnoreCase("once"))
                {
                    ((ApplicationInfo)apps.elementAt(i-1)).
                    setAutoStart(ApplicationInfo.AUTOSTART_ONCE);
                }
                else
                {
                    Log.logError("Invalid " + attrName + " value " + attrValue);
                    throw new InvalidAttributeException
                    (InstallerErrorMessage.INST_CORRUPT_PKG, null,
                     InstallerDetailedErrorMessage.ATTR_UNSUPPORTED,
                     new String[] { attrName },
                     (aBall.attributeExistsInJad(attrName)?
                      OtaStatusCode.INVALID_DESCRIPTOR:
                      OtaStatusCode.INVALID_JAR));
                }
            }
            else
            {
                break;
            }
        }
    }
}