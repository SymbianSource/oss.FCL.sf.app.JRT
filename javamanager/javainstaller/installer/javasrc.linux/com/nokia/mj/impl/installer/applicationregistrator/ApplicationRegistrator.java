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


package com.nokia.mj.impl.installer.applicationregistrator;

import com.nokia.mj.impl.installer.Installer;
import com.nokia.mj.impl.installer.utils.InstallerException;
import com.nokia.mj.impl.installer.utils.FileRoots;
import com.nokia.mj.impl.installer.utils.FileUtils;
import com.nokia.mj.impl.installer.utils.Log;
import com.nokia.mj.impl.utils.Uid;

import java.io.InputStream;
import java.io.IOException;
import java.io.OutputStream;


/**
 * Registers (and unregisters) Java application to Linux platform.
 * OPEN: to which application registration databases does this
 * class register the Java application?
 *
 * startSession() must be called before the other methods of this
 * class can be used.
 *
 *
 * @author Nokia Corporation
 * @version $Rev: 10289 $
 */
public final class ApplicationRegistrator
{
    // The OMJ default (Linux) icon
    private static final String DEFAULT_ICON_NAME = "java_app.png";
    private static final String DEFAULT_ICON_SUFFIX = ".png";


    /*** ----------------------------- PUBLIC ------------------------------ */

    /**
     * Starts application registration session.
     * The registrations and unregistrations are done only
     * when commitSession is called.
     * If you want to discard the registrations and unregistrations
     * call rollbackSession.
     * Does nothing if session has already been successfully started
     * from this ApplicationRegistrator instance.
     *
     * @throws InstallerException if the session cannot created
     * @see commitSession
     * @see rollbackSession
     */
    public void startSession()
    {

    }

    /**
     * Registers one Java application to Linux.
     *
     * @param aAppRegInfo Information needed to register the application
     * @throws InstallerException if registration cannot done or
     *  startSession has not been called successfully
     * @see startSession
     * @see AppRegInfo
     */
    public void registerApplication(AppRegInfo aAppRegInfo)
    {
    }

    /**
     * Unregisters one Java application from Linux.
     *
     * @param aAppRegInfo Information needed to unregister the application
     * @throws InstallerException if unregistration cannot done or
     *  startSession has not been called successfully
     * @see startSession
     * @see AppRegInfo
     */
    public void unregisterApplication(AppRegInfo aAppRegInfo)
    {
    }

    /**
     * Commits the registrations and unregistrations.
     * Ends the current session if commit is successfull.
     * If commit fails the session is kept open so that
     * rollbackSession can be called.
     *
     * @param aSynchronous if true, makes synchronous commit
     *
     * @throws InstallerException if session cannot be committed
     */
    public void commitSession(boolean aSynchronous)
    {
    }

    /**
     * Rolls back the registrations and unregistrations.
     * Ends the current session.
     *
     * @throws InstallerException if session cannot be rolled back.
     *  If exception is thrown the session is closed but you can
     *  still call rollbackSession.
     */
    public void rollbackSession()
    {
    }

    /**
     * Closes the current session.
     * If registerApplication or unregisterApplication has been called,
     * commitSession or rollbackSession must be called instead of this method.
     */
    public void closeSession()
    {
    }


    /**
     * Checks whether the Uid is in use from Linux.
     *
     * @param aUid The Uid of the application.
     * @return true if the Uid is already in use
     * @throws InstallerException if checking cannot be done
     */
    public boolean uidInUse(Uid aUid)
    {
        // Cannot yet check whether Uid is in use in Linux
        return false;
    }

    /**
     * Returns the logical group name for an installed application.
     *
     * @param aUid The Uid of the application.
     * @return the group name of the application. Can be empty string.
     * @throws InstallerException if the group name cannot be returned
     */
    public String getGroupName(Uid aUid)
    {
        if (aUid.getStringValue() == null)
        {
            return "";
        }

        // Missing actual implementation
        return "";
    }

    /**
     * Converts icon to platform specific format.
     *
     * @param aInputIconFilename file name for input icon file
     * @param aOutputIconFilename file name for output icon file
     * @param aJarFilename jar file name if aInputIconFilename specifies
     *    file inside jar file,
     *    or null if aInputIconFilename specifies file from disk
     * @param aIconSuffix the correct suffix of the icon is returned through
     *    this parameter, e.g. '.png'
     * @return true if the conversion succeeds
     */
    public boolean convertIcon(
        String aInputIconFilename,
        String aOutputIconFilename,
        String aJarFilename,
        StringBuffer aIconSuffix)
    {
        // In Linux it should be enough to extract the (PNG) icon
        // from the .jar file and store it to 'aOutputIconFilename'

        if ((aInputIconFilename == null) ||
                (aInputIconFilename.length() == 0) ||
                (aOutputIconFilename == null) ||
                (aOutputIconFilename.length() == 0))
        {
            return false;
        }

        if ((aJarFilename == null) || (aJarFilename.length() == 0))
        {
            // Icon is already in a standalone file, just copy it to
            // correct place
            InputStream fis = null;
            OutputStream fos = null;
            try
            {
                if (!FileUtils.exists(aInputIconFilename))
                {
                    return false;
                }

                fis  = FileUtils.getInputStream(aInputIconFilename);
                fos = FileUtils.getOutputStream(aOutputIconFilename);
                // Typically icon size is small, 4Kb should be enough
                byte[] buf = new byte[4096];
                int i = 0;
                while ((i = fis.read(buf)) != -1)
                {
                    fos.write(buf, 0, i);
                }
            }
            catch (IOException ioe)
            {
                return false;
            }
            finally
            {
                if (fis != null)
                {
                    try
                    {
                        fis.close();
                    }
                    catch (IOException ioe2) {}
                }
                if (fos != null)
                {
                    try
                    {
                        fos.close();
                    }
                    catch (IOException ioe3) {}
                }
            }

            // Return the original suffix of the icon
            // if Linux requires it to recognise the correct
            // type of the icon

            return true;
        }
        else
        {
            // Must extract the icon from .jar file
            return false;
        }
    }

    /**
     * Return path to the default icon
     *
     * @return full path name to the default  icon
     */
    public String getDefaultIconPath()
    {
        return FileRoots.getResourceDir() + DEFAULT_ICON_NAME;
    }

    /**
     * Return file suffix of the default icon
     *
     * @return file suffix of the default icon ('.png' in Linux)
     */
    public String getDefaultIconSuffix()
    {
        return DEFAULT_ICON_SUFFIX;
    }

    /**
     * Returns true if the current device is touch screen
     * device without physical LSK and RSK
     *
     * @return true if Installer should store the value of
     *  Nokia-MIDlet-On-Screen-Keypad to Storage
     */
    public boolean isOnDeviceKeypadNeeded()
    {
        return false;
    }

    /**
     * Sends a process to process notification to the parent process
     * of Java Installer telling that Installer is about to start
     * displaying UI dialogs.
     * The parent process reacts to the notification by hiding its
     * installation related dialogs.
     */
    public void notifyLauncherThatUiIsReady()
    {
        return;
    }


    /*** ---------------------------- PROTECTED --------------------------- */
    /*** ----------------------------- PACKAGE ---------------------------- */
    /*** ----------------------------- PRIVATE ---------------------------- */
    /*** ----------------------------- NATIVE ----------------------------- */

}
