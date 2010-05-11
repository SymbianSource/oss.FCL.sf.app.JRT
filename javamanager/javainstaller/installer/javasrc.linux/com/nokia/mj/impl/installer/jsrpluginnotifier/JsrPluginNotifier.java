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

import com.nokia.mj.impl.installer.integrityservice.IntegrityService;
import com.nokia.mj.impl.installer.utils.FileUtils;
import com.nokia.mj.impl.installer.utils.InstallerException;
import com.nokia.mj.impl.installer.utils.Log;


/**
 * Calls Installer Jsr Plugins when Java application is
 * installed / uninstalled or installation / uninstallation
 * is rolled back. Linux specific implementation.
 *
 * @see JsrPluginNotifierBase
 */
public final class JsrPluginNotifier extends JsrPluginNotifierBase
{

    /*** ----------------------------- PUBLIC ------------------------------ */

    /**
     * Constructor initializes member variables and loads
     * Jsr plugins if any has been defined in the text config file.
     *
     * @param aIntegrityService needed for possible file operations
     */
    public JsrPluginNotifier(IntegrityService aIntegrityService)
    {
        super();
        loadAllJsrPlugins();
    }

    /*** ----------------------------- PRIVATE ---------------------------- */

    /**
     * Read the class names of the Jsr plugins from text file in Linux
     * specific location. Base class implementation will read and
     * initialize the Jsr plugins.
     *
     * @throws InstallerException if internal class InstallerExtension
     *   cannot be loaded
     */
    protected void loadAllJsrPlugins()
    {
        String configFile = FileUtils.getInstallerRoot() + iConfigFileName;
        if (FileUtils.exists(configFile))
        {
            // Call the base class implementation that will
            // read the class names from text file,
            // load the plugin classes, instantiate them and
            // store them to a member variable
            loadAllJsrPlugins(configFile);
        }

        // No jsr plugin config file, do nothing.
        Log.log("Jsr plugin config file " + configFile + " does not exist.");
        return;
    }

}
