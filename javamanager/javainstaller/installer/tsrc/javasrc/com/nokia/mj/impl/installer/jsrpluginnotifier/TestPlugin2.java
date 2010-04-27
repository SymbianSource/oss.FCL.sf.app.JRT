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


package com.nokia.mj.impl.installer.jsrpluginnotifier;

/**
 * Installer Jsr plugin just for test purposes.
 *
 * @author Nokia Corporation
 * @version $Rev: 9336 $ $Date: 2010-01-14 14:18:29 +0200 (Thu, 14 Jan 2010) $
 * @see InstallerExtension
 */
public class TestPlugin2 implements InstallerExtension
{
    /**
     * This method is called when installation has been done
     * nearly completely (only committing changes is left).
     * @param aInstallerExtensionInfo installation info
     * @return true if installation can be finished
     *              false if installation must be cancelled
     */
    public boolean install(InstallerExtensionInfo aInstallerExtensionInfo)
    {
        aInstallerExtensionInfo.log("TestPlugin2.install called");

        // tell that this plugin has been called
        System.setProperty("TestPlugin2", "install");

        // If the MIDlet to be installed has JAD/JAR attribute
        // "JSR-Plugin-Test-Cancel" with any value, cancel install
        if ((aInstallerExtensionInfo.iAttributes != null) &&
                (aInstallerExtensionInfo.iAttributes.get("JSR-Plugin-Test-Cancel") != null))
        {
            return false;
        }

        return true;
    }

    /**
     * This method is called when uninstallation is starting
     * (all information is still available in databases).
     * @param aInstallerExtensionInfo uninstallation info
     * @return true if uninstallation can be finished
     *              false if uninstallation must be cancelled
     */
    public boolean uninstall(InstallerExtensionInfo aInstallerExtensionInfo)
    {
        aInstallerExtensionInfo.log("TestPlugin2.uninstall called");

        // tell that this plugin has been called
        System.setProperty("TestPlugin2", "uninstall");

        // If the MIDlet to be installed has JAD/JAR attribute
        // "JSR-Plugin-Test-Cancel" with any value, cancel install
        if ((aInstallerExtensionInfo.iAttributes != null) &&
                (aInstallerExtensionInfo.iAttributes.get("JSR-Plugin-Test-Cancel") != null))
        {
            return false;
        }

        return true;
    }


    /**
     * Called when installation has failed and will be rolled back.
     * Can be called even after install() has been called.
     *
     * @param aInstallerExtensionInfo installation info
     */
    public void rollbackInstall(InstallerExtensionInfo aInstallerExtensionInfo)
    {
        aInstallerExtensionInfo.log("TestPlugin2.rollbackInstall called");

        // tell that this plugin has been called
        System.setProperty("TestPlugin2", "rollbackInstall");
    }

    /**
     * Called when uninstallation has failed and will be rolled back.
     * Can be called after uninstall() has been called.
     *
     * @param aInstallerExtensionInfo installation info
     */
    public void rollbackUninstall(InstallerExtensionInfo aInstallerExtensionInfo)
    {
        aInstallerExtensionInfo.log("TestPlugin2.rollbackUninstall called");

        // tell that this plugin has been called
        System.setProperty("TestPlugin2", "rollbackUninstall");
    }
}
