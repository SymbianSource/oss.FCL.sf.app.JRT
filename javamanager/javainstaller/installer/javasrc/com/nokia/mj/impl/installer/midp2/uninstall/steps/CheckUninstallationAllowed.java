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
import com.nokia.mj.impl.installer.utils.InstallerException;
import com.nokia.mj.impl.installer.utils.Log;
import com.nokia.mj.impl.rt.installer.ApplicationInfoImpl;
import com.nokia.mj.impl.rt.support.ApplicationInfo;
import com.nokia.mj.impl.security.midp.authentication.AuthenticationModule;
import com.nokia.mj.impl.utils.Attribute;
import com.nokia.mj.impl.utils.InstallerDetailedErrorMessage;
import com.nokia.mj.impl.utils.InstallerErrorMessage;
import com.nokia.mj.impl.utils.OtaStatusCode;

/**
 * Check if uninstallation is allowed from Nokia-MIDlet-Block-Uninstall
 * attribute and protection domain.
 */
public class CheckUninstallationAllowed extends ExeStep
{
    public void execute(ExeBall aBall)
    {
        UninstallBall ball = (UninstallBall)aBall;
        ball.log("Checking if uninstallation is allowed...");

        // Get protection domain and initialize suite's trusted flag.
        String domain = AuthenticationModule.getInstance()
                        .getProtectionDomainCategory(ball.iStorageHandler.getSession(),
                                                     ball.iSuite.getUid());
        if (domain != null)
        {
            Log.log("Protection domain: " + domain);
            if (domain.equals(ApplicationInfo.UNIDENTIFIED_THIRD_PARTY_DOMAIN))
            {
                ball.iSuite.setTrusted(false);
            }
            // Set domain to ApplicationInfo so that possible uninstallation
            // time security prompts display correct security icons.
            ApplicationInfoImpl appInfoImpl =
                (ApplicationInfoImpl)ApplicationInfo.getInstance();
            appInfoImpl.setProtectionDomain(domain);
        }

        // Uninstallation is always allowed when iForceUninstall flag is set.
        if (ball.iForceUninstall)
        {
            Log.log("Uninstallation allowed because iForceUninstall is set");
            return;
        }

        // Check if Nokia-MIDlet-Block-Uninstall attribute is present.
        String attrName = "Nokia-MIDlet-Block-Uninstall";
        Attribute attr = ball.iSuite.getAttribute(attrName);
        if (attr == null || !attr.isTrusted() ||
                !attr.getValue().equalsIgnoreCase("true"))
        {
            // Attribute does not exist, is not trusted,
            // or does not have value "true", so
            // application can be uninstalled.
            Log.log("Uninstallation not blocked, attribute: " + attr);
            return;
        }

        // Attribute requests blocking uninstallation, now check
        // that application belongs to manufacturer or operator
        // protection domain.
        if (domain != null)
        {
            if (domain.equals(ApplicationInfo.MANUFACTURER_DOMAIN) ||
                    domain.equals(ApplicationInfo.OPERATOR_DOMAIN))
            {
                throw new InstallerException
                (InstallerErrorMessage.UNINST_NOT_ALLOWED, null,
                 InstallerDetailedErrorMessage.UNINST_BLOCKED, null,
                 OtaStatusCode.USER_CANCELLED);
            }
        }
    }

    public void cancel(ExeBall aBall)
    {
        // nop
    }
}
