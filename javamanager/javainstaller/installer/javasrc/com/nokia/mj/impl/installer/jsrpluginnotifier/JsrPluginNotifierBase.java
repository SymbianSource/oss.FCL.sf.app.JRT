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


package com.nokia.mj.impl.installer.jsrpluginnotifier;

import com.nokia.mj.impl.installer.midp2.install.steps.InstallBall;
import com.nokia.mj.impl.installer.midp2.uninstall.steps.UninstallBall;
import com.nokia.mj.impl.installer.utils.FileUtils;
import com.nokia.mj.impl.installer.utils.InstallerException;
import com.nokia.mj.impl.installer.utils.Log;
import com.nokia.mj.impl.utils.BufferedReader;
import com.nokia.mj.impl.utils.LineReader;

import java.io.InputStreamReader;
import java.io.IOException;
import java.io.Reader;
import java.util.Iterator;
import java.util.Vector;

/**
 * Calls Installer Jsr Plugins when Java application is
 * installed / uninstalled or installation / uninstallation
 * is rolled back.
 */
public class JsrPluginNotifierBase
{
    /**
     * The name of the text config file that contains the class
     * names of the installer Jsr plugins.
     */
    protected static final String iConfigFileName = "inst_plugins.cfg";

    /**
     * The class names of the plugins that have been read from
     * the text config file.
     */
    Vector iClassNames;

    /**
     * The instantiated JSR plugins to be notified.
     */
    Vector iJsrPlugins;


    /*** ----------------------------- PUBLIC ------------------------------ */

    /**
     * Constructor allocates member vectors.
     */
    public JsrPluginNotifierBase()
    {
        // The initial capacity and the increment of the vectors are
        // both 5 because the number of installer Jsr plugins is
        // small (1-3)
        iClassNames = new Vector(5,5);
        iJsrPlugins = new Vector(5,5);
    }

    /**
     * Notifies Jsr plugins in iJsrPlugins vector that a MIDlet suite is
     * about to be installed by calling the install() method of the plugins.
     *
     * @param aInfo notification information object
     * @throws InstallerException if any of the Jsr plugins cancels installation
     * @see InstallerExtension#install
     */
    public void notifyInstallation(InstallerExtensionInfo aInfo)
    {
        Log.log("JsrPluginNotifierBase.NotifyInstallation called");

        // Are there any plugins to be notified
        if (iJsrPlugins.size() > 0)
        {
            boolean continueInstallation = true;
            Iterator iter = iJsrPlugins.iterator();
            InstallerExtension plugin;
            while (iter.hasNext())
            {
                plugin = (InstallerExtension)iter.next();
                continueInstallation = plugin.install(aInfo);

                // Check whether JSR plugin cancels installation
                if (!continueInstallation)
                {
                    InstallerException.internalError(
                        "Jsr plugin " + plugin.getClass().getName() +
                        " cancelled installation.");
                }
            }
        }
    }

    /**
     * Notifies Jsr plugins in iJsrPlugins vector that a MIDlet suite is
     * about to be uninstalled by calling the uninstall() method of the plugins.
     *
     * @param aInfo notification information object
     * @throws InstallerException if any of the Jsr plugins cancels uninstallation
     * @see InstallerExtension#uninstall
     */
    public void notifyUninstallation(InstallerExtensionInfo aInfo)
    {
        Log.log("JsrPluginNotifierBase.notifyUninstallation called");

        // Are there any plugins to be notified
        if (iJsrPlugins.size() > 0)
        {
            boolean continueUninstallation = true;
            Iterator iter = iJsrPlugins.iterator();
            InstallerExtension plugin;
            while (iter.hasNext())
            {
                plugin = (InstallerExtension)iter.next();
                continueUninstallation = plugin.uninstall(aInfo);

                // Check whether JSR plugin cancels uninstallation
                if (!continueUninstallation)
                {
                    InstallerException.internalError(
                        "Jsr plugin " + plugin.getClass().getName() +
                        " cancelled uninstallation.");
                }
            }
        }
    }

    /**
     * Notifies Jsr plugins in iJsrPlugins vector that the installation of
     * a MIDlet suite has been cancelled by calling the rollbackInstall()
     * method of the plugins.
     *
     * @param aInfo notification information object
     * @see InstallerExtension#rollbackInstall
     */
    public void notifyRollbackInstall(InstallerExtensionInfo aInfo)
    {
        Log.log("JsrPluginNotifierBase.notifyRollbackInstall called");

        // Are there any plugins to be notified
        if (iJsrPlugins.size() > 0)
        {
            Iterator iter = iJsrPlugins.iterator();
            InstallerExtension plugin;
            while (iter.hasNext())
            {
                plugin = (InstallerExtension)iter.next();
                plugin.rollbackInstall(aInfo);
            }
        }
    }

    /**
     * Notifies Jsr plugins in iJsrPlugins vector that the uninstallation of
     * a MIDlet suite has been cancelled by calling the rollbackUninstall()
     * method of the plugins.
     *
     * @param aInfo notification information object
     * @see InstallerExtension#rollbackUninstall
     */
    public void notifyRollbackUninstall(InstallerExtensionInfo aInfo)
    {
        Log.log("JsrPluginNotifierBase.notifyRollbackUninstall called");

        // Are there any plugins to be notified
        if (iJsrPlugins.size() > 0)
        {
            Iterator iter = iJsrPlugins.iterator();
            InstallerExtension plugin;
            while (iter.hasNext())
            {
                plugin = (InstallerExtension)iter.next();
                plugin.rollbackUninstall(aInfo);
            }
        }
    }

    /**
     * Returns true if this JsrPluginNotifier has any Jsr plugins.
     *
     * @return true if there are any Jsr plugins
     */
    public boolean anyJsrPlugins()
    {
        return (iJsrPlugins.size() > 0);
    }


    /*** ----------------------------- PROTECTED ---------------------------- */

    /**
     * Read the class names of the Jsr plugins from text file given in
     * fileName. Then load plugin classes, instantiate the plugins and
     * store them to iJsrPlugins. Skip illegal plugins (e.g. plugins that
     * do not implement InstallerExtension interface).
     *
     * @param aFileName the (absolute) file name of the text config file.
     * @throws InstallerException if internal class InstallerExtension
     *   cannot be loaded
     */
    protected void loadAllJsrPlugins(String aFileName)
    {
        // Read the class names of the JSR Plugins from text file
        // and store them to iClassNames
        readClassNames(aFileName);

        Iterator iter = iClassNames.iterator();
        String className;
        Class pluginClass;
        InstallerExtension extPlugin;

        // Load the plugin classes
        while (iter.hasNext())
        {
            className = (String)iter.next();
            try
            {
                pluginClass = Class.forName(className);

                // Instantiate plugin and try to cast it to correct type
                extPlugin = (InstallerExtension)(pluginClass.newInstance());

                // instantiate them and
                // store them to member variable
                iJsrPlugins.add(extPlugin);
            }
            catch (ClassNotFoundException e2)
            {
                Log.logError("Jsr plugin class" + className + " is not found.");
            }
            catch (InstantiationException e3)
            {
                Log.logError("Cannot instantiate Jsr plugin " + className);
            }
            catch (IllegalAccessException e4)
            {
                Log.logError("Cannot access Jsr plugin " + className);
            }
            catch (ClassCastException e5)
            {
                Log.logError("Jsr plugin " + className +
                             " does not implement interface InstallerExtension.");
            }
        }
    }


    /*** ----------------------------- PRIVATE ---------------------------- */

    /**
     * Read the class names of the Jsr plugins from text file given in
     * fileName. The format of the text file is one class name per line,
     * <CR> or <LF> or any combination of <CR> and <LF> as line end,
     * the plugins will be called in the order they are listed in the file.
     * The class names are stored in to member variable iClassNames.
     *
     * @param aFileName the (absolute) file name of the text config file.
     */
    private void readClassNames(String aFileName)
    {
        Log.log("JsrPluginNotifierBase readClassNames reading conf file "
                + aFileName);

        // Read the class names of the JSR plugins from text file
        // and store them to iClassNames.
        LineReader reader = null;
        try
        {
            reader = new LineReader(
                new BufferedReader(
                    new InputStreamReader(
                        FileUtils.getInputStream(aFileName))));

            String className = null;
            while ((className = reader.readLine()) != null)
            {
                className = className.trim();
                if (!className.equals("") && !className.startsWith("#"))
                {
                    // Line contains something else than a comment,
                    // store the new class name to string vector.
                    iClassNames.add(className);
                    Log.log("Name of Jsr plugin class: " + className);
                }
            }
        }
        catch (IOException e)
        {
            Log.logError("JsrPluginNotifier cannot read plugin " +
                         "config file " + aFileName, e);
            return;
        }
        finally
        {
            if (reader != null)
            {
                try
                {
                    reader.close();
                    reader = null;
                }
                catch (IOException ioe)
                {
                    Log.logError(
                        "JsrPluginNotifier: exception while closing reader",
                        ioe);
                }
            }
        }
    }

}
