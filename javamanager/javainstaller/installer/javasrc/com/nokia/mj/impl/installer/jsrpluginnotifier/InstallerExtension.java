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
 * JSR Installer Plugin interface. All Java Installer
 * JSR plugins must implement this interface and have
 * constructor that accepts empty argument list.
 * @see JsrPluginNotifier
 */
public interface InstallerExtension
{
    /**
     * This method is called when installation has been done
     * nearly completely (only committing changes is left).
     * @param aInstallerExtensionInfo installation info
     * @return true if installation can be finished
     *              false if installation must be cancelled
     */
    public boolean install(InstallerExtensionInfo aInstallerExtensionInfo);

    /**
     * This method is called when uninstallation is starting
     * (all information is still available in databases).
     * @param aInstallerExtensionInfo uninstallation info
     * @return true if uninstallation can be finished
     *              false if uninstallation must be cancelled
     */
    public boolean uninstall(InstallerExtensionInfo aInstallerExtensionInfo);

    /**
     * Called when installation has failed and will be rolled back.
     * Called after install().
     *
     * @param aInstallerExtensionInfo installation info
     */
    public void rollbackInstall(InstallerExtensionInfo aInstallerExtensionInfo);

    /**
     * Called when uninstallation has failed and will be rolled back.
     * Called after uninstall().
     *
     * @param aInstallerExtensionInfo installation info
     */
    public void rollbackUninstall(InstallerExtensionInfo aInstallerExtensionInfo);
}
