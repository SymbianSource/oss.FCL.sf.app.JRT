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


import com.nokia.mj.impl.installer.applicationregistrator.ApplicationRegistrator;
import com.nokia.mj.impl.installer.exetable.ExeBall;
import com.nokia.mj.impl.installer.exetable.ExeStep;
import com.nokia.mj.impl.installer.storagehandler.ApplicationInfo;
import com.nokia.mj.impl.installer.storagehandler.SuiteInfo;
import com.nokia.mj.impl.installer.utils.FileUtils;
import com.nokia.mj.impl.installer.utils.InstallerException;

import com.nokia.mj.impl.utils.Uid;

import java.util.Vector;

/**
 * MIDP installation step ConvertIcons.
 *
 * This step is called two times.
 *
 * First call happens after the confirmation UI dialog
 * is shown to the user. If .jar file is already available (local installation),
 * converts the suite icon and the application icons and stores them to temp
 * place inside Integrity Server session. If .jar is not yet present,
 * sets the path to the default icon (either .svg or if the device does not
 * support .svg, then .mbm in S60, .png in Linux) to suite icon and
 * application icons.
 *
 * Second call happens before AuthenticateJar step.
 * If new .jar file was downloaded after first call and contains icons
 * used as application icons, converts the icons and stores them to temp
 * place inside Integrity Server session.
 * Otherwise if default icons are used copies the default icons
 * to temp place with an unique name inside Integrity Server session and
 * sets path to the converted and renamed icon to suite and application
 * icons. [This must be done in S60 because icon cannot be registered
 * into AppArc unless it can be opened in write mode and it has unique name.]
 */
public class ConvertIcons extends ExeStep
{
    private boolean iSecondCall = false;

    public void execute(ExeBall aBall)
    {
        InstallBall ball = (InstallBall)aBall;
        boolean fileOpResult = true;

        if (iSecondCall)
        {
            ball.log("Finalising icon information ...");

            // If .jar file has been downloaded since the first call
            // of this method OR
            // if this installation is silent installation,
            // convert the icons in the .jar now
            if (ball.iDownloadSucceeded || ball.isSilent())
            {
                convertIconsInJar(ball);
            }

            // In any case, all default application icons
            // must be copied to uniquely named files so that
            // they can be registered in S60 and uninstalling one
            // application does not remove icons from all applications
            // using the default icon
            String defaultIcon = ball.iApplicationRegistrator.getDefaultIconPath();
            String newName;
            Vector newApps = ball.iSuite.getApplications();

            // Handle application icons
            for (int i = 0; i < newApps.size(); i++)
            {
                ApplicationInfo newApp = (ApplicationInfo)newApps.elementAt(i);

                if (newApp.getConvertedIconPath().equals(defaultIcon))
                {
                    // also the suffix of the file must be set to correct
                    newName =
                        FileUtils.getIntegrityServiceRoot() +
                        newApp.getUid().getStringValue() +
                        "_icon" +
                        ball.iApplicationRegistrator.getDefaultIconSuffix();

                    fileOpResult = ball.iIntegrityService.copy(defaultIcon, newName);
                    if (!fileOpResult)
                    {
                        InstallerException.internalError(
                            "Copying file " + defaultIcon + " to " + newName +  " failed.");
                    }

                    newApp.setConvertedIconPath(newName);
                }
            }
        }
        else
        {
            ball.log("Setting preliminary icon information ...");

            if (ball.isSilent())
            {
                // No icons will to be shown to the user,
                // no need to do anything yet
                iSecondCall = true;
                return;
            }

            // Ask javainstalllauncher to stop displaying
            // 'Preparing installation' dialog.
            ball.iApplicationRegistrator.notifyLauncherThatUiIsReady();

            convertIconsInJar(ball);

            iSecondCall = true;
        }

    } // end of execute method


    public void cancel(ExeBall aBall)
    {
        // nop
    }

    /**
     * Convert the icons in the .jar file if any.
     * Otherwise set default icon to suite icon and
     * application icons.
     *
     * @throws InstallerException if the icon file cannot be
     *  renamed so that it would have correct suffix
     */
    protected void convertIconsInJar(InstallBall ball)
    {
        String  defaultIcon = ball.iApplicationRegistrator.getDefaultIconPath();
        Vector  newApps = ball.iSuite.getApplications();
        boolean fileOpResult = true;

        // If .jar file is not available, use default icon.
        if (ball.iJarFilename == null)
        {
            // Set suite icon
            ball.iSuite.setConvertedIconPath(defaultIcon);
            ball.iSuite.setUseDefaultIcon(true);

            // Set application icons
            for (int i = 0; i < newApps.size(); i++)
            {
                ApplicationInfo app = (ApplicationInfo)newApps.elementAt(i);
                app.setConvertedIconPath(defaultIcon);
                app.setUseDefaultIcon(true);
            }
        }
        else
        {
            // Check icon attributes and convert icons if they are in .jar.
            // If no icon attributes defined or icons are not in .jar or
            // they cannot be converted, use default icon

            // The icon of the suite can be specified in two attributes.
            // The priority order of the attributes is:
            // 1. Nokia-Scalable-Icon
            // 2. MIDlet-Icon
            // 3. Use default icon if neither of the attributes has been defined
            String suiteIconName = ball.getAttributeValue("Nokia-Scalable-Icon");
            if (null == suiteIconName)
            {
                // no scalable suite icon defined
                suiteIconName = ball.getAttributeValue("MIDlet-Icon");
                if (null == suiteIconName)
                {
                    // no suite icon defined, use default icon
                    suiteIconName = "";
                }
            }

            StringBuffer suiteIconSuffix = new StringBuffer();
            String suiteIconFile = "";
            if (suiteIconName.length() > 0)
            {
                suiteIconFile =
                    ball.iIntegrityService.createTempFile(-1);
                if (ball.iApplicationRegistrator.convertIcon(
                            suiteIconName, suiteIconFile, ball.iJarFilename, suiteIconSuffix))
                {
                    // Conversion succeeded, icon is now in place specified
                    // by suiteIconFile

                    // Must rename the temp file to have the correct suffix
                    // so that the device OS recognizes the icon type correctly
                    if (suiteIconSuffix.length() > 0)
                    {
                        fileOpResult = ball.iIntegrityService.move(
                                           suiteIconFile, suiteIconFile + suiteIconSuffix.toString());
                        if (!fileOpResult)
                        {
                            InstallerException.internalError(
                                "Renaming file " + suiteIconFile + " to " +
                                suiteIconFile + suiteIconSuffix.toString() +  " failed.");
                        }
                        suiteIconFile += suiteIconSuffix.toString();
                    }

                    ball.iSuite.setConvertedIconPath(suiteIconFile);
                }
                else
                {
                    ball.iSuite.setConvertedIconPath(defaultIcon);
                    ball.iSuite.setUseDefaultIcon(true);
                    // Cannot use suite icon file
                    suiteIconFile = "";
                }
            }
            else
            {
                ball.iSuite.setConvertedIconPath(defaultIcon);
                ball.iSuite.setUseDefaultIcon(true);
            }

            // The icon of the application can be specified in two attributes.
            // The priority order of the attributes is:
            // 1. Nokia-Scalable-Icon-MIDlet-<N>
            // 2. MIDlet-<N>
            // 3. Use suite icon
            // 4. Use default icon
            StringBuffer iconSuffix = new StringBuffer();
            String iconName;
            for (int i = 0; i < newApps.size(); i++)
            {
                // clean up old suffix
                iconSuffix.setLength(0);
                ApplicationInfo newApp = (ApplicationInfo)newApps.elementAt(i);

                // loop variable i goes from 0..(N-1) but in jad/jar attributes
                // numbering is 1..N
                iconName =
                    ball.getAttributeValue("Nokia-Scalable-Icon-MIDlet-" + (i+1));
                if (null == iconName)
                {
                    // The ApplicationInfo object contains the value of
                    // the attribute MIDlet-<N>
                    iconName = newApp.getIconPath();
                }
                if (iconName.length() == 0)
                {
                    // No MIDlet icon defined, use already converted suite
                    // icon if it exists
                    if (suiteIconFile.length() > 0)
                    {
                        // Must make separate copy of the suite icon for each
                        // midlet. Otherwise removing one of the midlets that
                        // have suite icon removes the icon also from the
                        // other midlets that have the suite icon.
                        String newName =
                            FileUtils.getIntegrityServiceRoot() +
                            newApp.getUid().getStringValue() +
                            "_icon" +
                            suiteIconSuffix.toString();
                        fileOpResult = ball.iIntegrityService.copy(
                                           suiteIconFile,
                                           newName);
                        if (!fileOpResult)
                        {
                            InstallerException.internalError(
                                "Copying file " + suiteIconFile + " to " +
                                newName +  " failed.");
                        }

                        newApp.setConvertedIconPath(newName);
                        continue;
                    }
                    else
                    {
                        // use default icon
                        newApp.setConvertedIconPath(defaultIcon);
                        newApp.setUseDefaultIcon(true);
                    }
                }
                else
                {
                    // Convert icon. It is inside .jar file.
                    String iconFile = ball.iIntegrityService.createTempFile(-1);
                    String finalIconFile;

                    if (ball.iApplicationRegistrator.convertIcon(
                                iconName, iconFile, ball.iJarFilename, iconSuffix))
                    {
                        // Conversion succeeded, icon is now in place
                        // specified by iconFile.

                        // Must rename the temp file to have the correct suffix
                        // so that the device OS recognizes the icon type correctly.
                        // Form the final name based on the Uid of the application
                        // so that the name of the icon file stays the same when
                        // the application upgraded (prevents problems with device UI)
                        finalIconFile =
                            FileUtils.getIntegrityServiceRoot() +
                            newApp.getUid().getStringValue() +
                            "_icon" +
                            iconSuffix.toString();
                        fileOpResult = ball.iIntegrityService.move(
                                           iconFile,
                                           finalIconFile);
                        if (!fileOpResult)
                        {
                            InstallerException.internalError(
                                "Renaming file " + iconFile + " to " +
                                finalIconFile +  " failed.");
                        }

                        newApp.setConvertedIconPath(finalIconFile);
                    }
                    else
                    {
                        ball.log("Warning: Converting icon " + iconName + " inside .jar file "
                                 + ball.iJarFilename + " failed.");

                        // MIDlet icon cannot be used, use already converted suite
                        // icon if it exists
                        if (suiteIconFile.length() > 0)
                        {
                            // Must make separate copy of the suite icon for each
                            // midlet. Otherwise removing one of the midlets that
                            // have suite icon removes the icon also from the
                            // other midlets that have the suite icon.
                            String newName =
                                FileUtils.getIntegrityServiceRoot() +
                                newApp.getUid().getStringValue() +
                                "_icon" +
                                suiteIconSuffix.toString();
                            fileOpResult = ball.iIntegrityService.copy(
                                               suiteIconFile,
                                               newName);
                            if (!fileOpResult)
                            {
                                InstallerException.internalError(
                                    "Copying file " + suiteIconFile + " to " +
                                    newName +  " failed.");
                            }

                            ball.log("Using suite icon for midlet number " + (i+1));
                            newApp.setConvertedIconPath(newName);
                            continue;
                        }
                        else
                        {
                            // use default icon
                            ball.log("Using default icon for midlet number " + (i+1));
                            newApp.setConvertedIconPath(defaultIcon);
                            newApp.setUseDefaultIcon(true);
                        }
                    }
                }
            }   // for loop
        } // else, .jar file was available
    }  // end of convertIconsInJar

}
