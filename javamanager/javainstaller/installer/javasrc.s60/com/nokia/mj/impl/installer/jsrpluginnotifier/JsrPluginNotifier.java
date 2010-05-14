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
 * is rolled back.
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
        loadAllJsrPlugins(aIntegrityService);
    }

    /*** ----------------------------- PRIVATE ---------------------------- */

    /**
     * Read the class names of the Jsr plugins from text file in S60
     * specific location.
     * If the text config file cannot be found from the current drive,
     * but if can be found from S60 default ROM drive, copy the file
     * from ROM to the current drive and start using it.
     * Base class implementation will read and initialize the Jsr plugins.
     *
     * @param aIntegrityService Java Installer integrity service, needed for
     *  file operations.
     * @throws InstallerException if internal class InstallerExtension
     *   cannot be loaded
     */
    protected void loadAllJsrPlugins(IntegrityService aIntegrityService)
    {
        String configFile = FileUtils.getInstallerRoot() + iConfigFileName;
        // If the text file does not exist in RAM, try to copy it from ROM
        if (!FileUtils.exists(configFile))
        {
            // Replace the first character with 'Z' (ROM drive letter)
            // This code uses the knowledge that in S60 the canonical
            // path is in format "C:\abc\def\..."
            String ramName = configFile;
            Log.log("Jsr plugin config file " + ramName + " does not exist.");
            String romName = "Z" + ramName.substring(1);
            if (FileUtils.exists(romName))
            {
                // copy file from ROM to RAM
                Log.log("Copy file " + romName + " to " + ramName);
                boolean fileOpResult = aIntegrityService.copy(romName, ramName);
                if (!fileOpResult)
                {
                    Log.logError("Cannot copy config file " + configFile +
                                 " from ROM to RAM");
                    return;
                }
            }
            else
            {
                // no config file in RAM nor in ROM
                return;
            }
        }

        // Call the base class implementation that will
        // read the class names from text file,
        // load the plugin classes, instantiate them and
        // store them to a member variable
        loadAllJsrPlugins(configFile);
    }

}
