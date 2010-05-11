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

import com.nokia.mj.impl.installer.applicationregistrator.AppRegInfo;
import com.nokia.mj.impl.installer.applicationregistrator.ApplicationRegistrator;
import com.nokia.mj.impl.installer.customisationproperties.CustomisationProperties;
import com.nokia.mj.impl.installer.exetable.ExeBall;
import com.nokia.mj.impl.installer.exetable.ExeStep;
import com.nokia.mj.impl.installer.storagehandler.ApplicationInfo;
import com.nokia.mj.impl.utils.Uid;

import java.util.Vector;

/**
 * MIDP installation step RegisterApplication.
 * Added to MIDP2 Install table in com.nokia.mj.impl.installer.Installer
 */
public class RegisterApplication extends ExeStep
{
    public void execute(ExeBall aBall)
    {
        InstallBall ball = (InstallBall)aBall;
        if (ball.iOldSuite != null)
        {
            // Old suite exists, this is an upgrade.
            ball.log("Removing old info from application registry...");

            // Unregister all old applications
            Vector oldApps = ball.iOldSuite.getApplications();

            for (int j = 0; j < oldApps.size(); j++)
            {
                ApplicationInfo oldApp = (ApplicationInfo)oldApps.elementAt(j);

                AppRegInfo appRegInfo = new AppRegInfo(
                    // Uid aUid,
                    oldApp.getUid(),
                    // String aGroupName, dummy
                    "",
                    // String aMIDletName, dummy
                    "",
                    // String aTargetDrive, dummy
                    "",
                    // String aIconFileName, dummy
                    "",
                    // String aJarFileName, dummy
                    "",
                    // boolean aHidden,
                    false,
                    // boolean aBackground
                    false);

                ball.iApplicationRegistrator.unregisterApplication(appRegInfo);
            }
        }

        ball.log("Adding info to application registry...");

        // Register all new applications
        Vector newApps = ball.iSuite.getApplications();

        // Convert installation drive to string. This is S60 specific,
        // in Linux drives are not identified by letters
        char raw[] = {'A', ':'};
        raw[0] = (char)(raw[0] + ball.iInstallationDrive);
        String targetDrive = new String(raw);

        // It is possible to define the installation folder using
        // several possible JAD/JAR attributes.
        // Check the configured attribute names in customisation
        // properties ('Nokia-MIDlet-Category' is always supported).
        String groupName = ball.iInstallationGroup;
        if (groupName == null)
        {
            Vector attributeNames = ball.iCustomisationProperties.getProperty(
                                        CustomisationProperties.ShellFolderAttributeNames);
            Vector possibleInstallationFolders = ball.iCustomisationProperties.getProperty(
                                                     CustomisationProperties.PossibleInstallationFolders);
            String attributeName;
            String attributeValue;
            for (int nInd = 0; nInd < attributeNames.size(); nInd++)
            {
                attributeName = (String)attributeNames.elementAt(nInd);
                attributeValue = ball.iSuite.getAttributeValue(attributeName);
                // If this attribute has value for this midlet suite we
                // must still check whether the value is one of the allowed
                // installation folders.
                if (null != attributeValue)
                {
                    groupName = getMatchingGroupName(
                                    attributeValue, possibleInstallationFolders);
                    if (null != groupName)
                    {
                        // Found matching group name.
                        break;
                    }
                    else
                    {
                        // It is not allowed to install midlet to the folder
                        // specified in the attribute.
                        ball.log(attributeName + " specifies installation folder " +
                                 attributeValue + " that is not one of the allowed folders " +
                                 possibleInstallationFolders.toString());
                        groupName = null;
                    }
                }
            }
            if (null == groupName)
            {
                // Use the default installation folder
                groupName = (String)(ball.iCustomisationProperties.getProperty(
                                         CustomisationProperties.DefaultInstallationFolder).firstElement());
            }
            ball.iInstallationGroup = groupName;
        }

        // Now register all java applications in the suite one at a time
        for (int i = 0; i < newApps.size(); i++)
        {
            ApplicationInfo newApp = (ApplicationInfo)newApps.elementAt(i);
            // Use localized java application name if it exists
            String attrName = "Nokia-MIDlet-Localized-" + (i + 1);
            String midletName = ball.getAttributeValue(attrName);
            if (midletName == null)
            {
                midletName = newApp.getName();
            }

            AppRegInfo appRegInfo = new AppRegInfo(
                // Uid aUid,
                newApp.getUid(),
                // String aGroupName,
                groupName,
                // String aMIDletName,
                midletName,
                // String aTargetDrive,
                targetDrive,
                // String aIconFileName,
                newApp.getConvertedIconPath(),
                // String aJarFileName, the full path name of the jar file
                ball.iJarFilename,
                // boolean aHidden,
                false,
                // boolean aBackground
                false);

            ball.iApplicationRegistrator.registerApplication(appRegInfo);
        }
    }

    public void cancel(ExeBall aBall)
    {
        // nop
    }

    /**
     * Matches the group name from Nokia-MIDlet-Category attribute value
     * to possible installation folders. Matching is case insensitive
     * and ignores the 's' suffixes. Returns matching value from possible
     * installation folders or null if no matching folder is found.
     */
    private static String getMatchingGroupName(
        String aGroupName, Vector aPossibleInstallationFolders)
    {
        if (aGroupName == null || aPossibleInstallationFolders == null)
        {
            return null;
        }
        String result = null;
        for (int i = 0; i < aPossibleInstallationFolders.size(); i++)
        {
            String possibleFolder =
                (String)aPossibleInstallationFolders.elementAt(i);
            if (groupsMatch(aGroupName, possibleFolder))
            {
                result = possibleFolder;
                break;
            }
        }
        return result;
    }

    /**
     * Returns true if given strings match when compared case
     * insensitively and ignoring the optional 's' suffix.
     */
    private static boolean groupsMatch(String aGroup1, String aGroup2)
    {
        if (aGroup1 == null || aGroup2 == null)
        {
            return false;
        }
        String s1 = aGroup1.toLowerCase();
        String s2 = aGroup2.toLowerCase();
        int len = s1.length();
        if (len > 0 && s1.charAt(len - 1) == 's')
        {
            s1 = s1.substring(0, len - 1);
        }
        len = s2.length();
        if (len > 0 && s2.charAt(len - 1) == 's')
        {
            s2 = s2.substring(0, len - 1);
        }
        return s1.equals(s2);
    }

}
