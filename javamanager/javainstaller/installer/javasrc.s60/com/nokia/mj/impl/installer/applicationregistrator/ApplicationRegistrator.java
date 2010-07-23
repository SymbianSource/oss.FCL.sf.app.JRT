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


package com.nokia.mj.impl.installer.applicationregistrator;

import com.nokia.mj.impl.installer.storagehandler.SuiteInfo;
import com.nokia.mj.impl.installer.utils.InstallerException;
import com.nokia.mj.impl.installer.utils.FileRoots;
import com.nokia.mj.impl.installer.utils.FileUtils;
import com.nokia.mj.impl.installer.utils.Log;
import com.nokia.mj.impl.installer.utils.PlatformUid;
import com.nokia.mj.impl.utils.Uid;


/**
 * Registers (and unregisters) Java application to S60 platform.
 * This version registers MIDlet to Symbian AppArc.
 * startSession() must be called before the other methods of this
 * class can be used.
 */
public final class ApplicationRegistrator
{
    // The OMJ default (svg) icon
    private static final String DEFAULT_SVG_ICON_NAME = "java_app.mif";
    private static final String DEFAULT_92_SVG_ICON_NAME = "java_app_92.mif";
    private static final String DEFAULT_SVG_ICON_SUFFIX = ".mif";

    /**
     * Native session handle
     */
    private int iSessionHandle = 0;


    /*** ----------------------------- PUBLIC ------------------------------ */

    /**
     * Starts application registration session.
     * Before the new session is started rolls back any pending (un)registrations
     * that are not committed (for example power off case). So do NOT start
     * sessions from several instances of ApplicationRegistrator concurrently.
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
        if (0 != iSessionHandle)
        {
            // session has already been created, do nothing
            Log.logWarning("ApplicationRegistrator.startSession called although session is already open.");
            return;
        }

        // native method writes error log and returns
        // negative Symbian error code if it fails
        int ret = _startSession();
        if (ret < 0)
        {
            InstallerException.internalError("Creating session failed with code " + ret);
        }

        iSessionHandle = ret;
    }

    /**
     * Registers one Java application to S60 AppArc.
     *
     * @param aAppRegInfo Information needed to register the application
     * @throws InstallerException if registration cannot done or
     *  startSession has not been called successfully
     * @see startSession
     * @see AppRegInfo
     */
    public void registerApplication(AppRegInfo aAppRegInfo)
    {
        if (0 == iSessionHandle)
        {
            InstallerException.internalError("No valid session.");
        }

        // check that aAppRegInfo.iTargetDrive is "A:" ... "Z:"
        int err = 0;
        aAppRegInfo.iTargetDrive = aAppRegInfo.iTargetDrive.toUpperCase();
        if (aAppRegInfo.iTargetDrive.length() != 2)
        {
            err = 1;
        }
        else if (aAppRegInfo.iTargetDrive.charAt(1) != ':')
        {
            err = 1;
        }
        else if ((aAppRegInfo.iTargetDrive.charAt(0) < 'A') ||
                 (aAppRegInfo.iTargetDrive.charAt(0) > 'Z'))
        {
            err = 1;
        }
        if (1 == err)
        {
            InstallerException.internalError(
                "Invalid target drive " + aAppRegInfo.iTargetDrive);
        }

        // Symbian AppArc can store max 16 characters for group name.
        String groupName = aAppRegInfo.iGroupName;
        if (groupName.length() > 16)
        {
            groupName = aAppRegInfo.iGroupName.substring(0, 16);
            Log.logWarning("ApplicationRegistrator.registerApplication: Application group name len > 16, cut to "
                           + groupName);
        }

        Log.log("ApplicationRegistrator registerApplication called with  "
                + aAppRegInfo.toString());

        // Applications with name longer than 80 characters cannot be
        // launched in Symbian.
        String midletName = aAppRegInfo.iMIDletName;
        if (midletName.length() > 80)
        {
            midletName = midletName.substring(0, 80);
            Log.logWarning("ApplicationRegistrator.registerApplication: midlet name len > 80, cut to "
                           + midletName);
        }

        try
        {
            err = _registerApplication(
                      iSessionHandle,
                      PlatformUid.getIntValue(aAppRegInfo.iUid),
                      groupName,
                      midletName,
                      aAppRegInfo.iTargetDrive,
                      aAppRegInfo.iIconFileName,
                      aAppRegInfo.iJarFileName,
                      aAppRegInfo.iHidden,
                      aAppRegInfo.iBackground);
        }
        catch (Throwable t)
        {
            InstallerException.internalError("Registering application "
                                             + "failed with unexpected throwable " + t.toString());
        }

        if (err < 0)
        {
            InstallerException.internalError("Registering application "
                                             + aAppRegInfo.iUid.getStringValue() + " failed with code " + err);
        }
    }

    /**
     * Unregisters one Java application from S60 AppArc.
     *
     * @param aAppRegInfo Information needed to unregister the application,
     *  only Uid is needed to unregister application in S60
     * @throws InstallerException if unregistration cannot done or
     *  startSession has not been called successfully
     * @see startSession
     * @see AppRegInfo
     */
    public void unregisterApplication(AppRegInfo aAppRegInfo)
    {
        if (0 == iSessionHandle)
        {
            InstallerException.internalError("No valid session.");
        }

        Log.log("Unregistering application " + aAppRegInfo.iUid.getStringValue());

        int err = _unregisterApplication(iSessionHandle, PlatformUid.getIntValue(aAppRegInfo.iUid));
        if (err < 0)
        {
            InstallerException.internalError("Unregistering application "
                                             + aAppRegInfo.iUid.getStringValue() + " failed with code " + err);
        }
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
        if (0 == iSessionHandle)
        {
            InstallerException.internalError("No valid session.");
        }

        int err = _commitSession(iSessionHandle, aSynchronous);
        if (err < 0)
        {
            InstallerException.internalError("Commiting session failed with code " + err);
        }

        // Current session has been closed
        iSessionHandle = 0;
    }

    /**
     * Rolls back the registrations and unregistrations.
     * Ends the current session.
     *
     * @throws InstallerException if session cannot be rolled back.
     */
    public void rollbackSession()
    {
        if (0 == iSessionHandle)
        {
            InstallerException.internalError("No valid session.");
        }

        int err = _rollbackSession(iSessionHandle);
        // Session is closed always when rollback is called
        iSessionHandle = 0;
        if (err < 0)
        {
            InstallerException.internalError("Rolling back the session failed with code " + err);
        }
    }

    /**
     * Closes the current session if it still open.
     * If registerApplication or unregisterApplication has been called,
     * commitSession or rollbackSession must be called instead of this method.
     */
    public void closeSession()
    {
        if (0 == iSessionHandle)
        {
            return;
        }

        _closeSession(iSessionHandle);

        // Current session has been closed
        iSessionHandle = 0;
    }

    /**
     * Checks whether the Uid is in use from S60 AppArc.
     *
     * @param aUid The Uid of the application.
     * @return true if the Uid is already in use
     * @throws InstallerException if checking cannot be done
     */
    public boolean uidInUse(Uid aUid)
    {
        if (0 == iSessionHandle)
        {
            InstallerException.internalError("No valid session.");
        }

        int ret;

        PlatformUid s60Uid = (PlatformUid)PlatformUid.createUid(aUid.getStringValue());
        if (null == s60Uid)
        {
            InstallerException.internalError("Illegal S60 Uid");
        }

        ret = _uidInUse(iSessionHandle, s60Uid.getIntValue());
        if (0 == ret)
        {
            return false;
        }
        else
        {
            return true;
        }
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
        if (0 == iSessionHandle)
        {
            InstallerException.internalError("No valid session.");
        }

        int ret;

        StringBuffer groupName = new StringBuffer("");
        PlatformUid s60Uid = (PlatformUid)PlatformUid.createUid(aUid.getStringValue());
        if (null == s60Uid)
        {
            InstallerException.internalError("Illegal S60 Uid");
        }

        ret = _getGroupName(iSessionHandle, s60Uid.getIntValue(), groupName);
        if (-1 == ret)
        {
            // Symbian error code KErrNotFound means that there is no
            // application with aUid, just log this error so that
            // the installation / uninstallation operation currently on-going
            // is not stopped.

            Log.log("ApplicationRegistrator.getGroupName() failed. No app with such uid.");
        }
        else if (ret < -1)
        {
            InstallerException.internalError("Getting group name failed with code " + ret);
        }

        return groupName.toString();
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
     *    this parameter, will contain '.mbm' or '.mif' when function returns
     * @return true if the conversion succeeds
     */
    public boolean convertIcon(
        String aInputIconFilename,
        String aOutputIconFilename,
        String aJarFilename,
        StringBuffer aIconSuffix)
    {
        Log.log("ApplicationRegistrator.convertIcon: " + aInputIconFilename +
                " --> " + aOutputIconFilename + ", from jar " + aJarFilename);
        return _convertIcon(aInputIconFilename, aOutputIconFilename,
                            aJarFilename, aIconSuffix);
    }

    /**
     * Return path to the default icon (SVG icon in S60)
     *
     * @return full path name to the default icon.
     */
    public String getDefaultIconPath()
    {
        String iconPath;

        if (_runningIn92Emulator())
        {
            // In real 9.2 devices the icon has the same name as in earlier
            // devices. Only emulator is special case.
            iconPath = FileRoots.getResourceDir() + DEFAULT_92_SVG_ICON_NAME;
        }
        else
        {
            iconPath = FileRoots.getResourceDir() + DEFAULT_SVG_ICON_NAME;
        }

        if (FileUtils.exists(iconPath))
        {
            return iconPath;
        }

        // Executing code is some special environment, icon is not in the same
        // drive as installer itself.
        String driveLetter = iconPath.substring(0,1).toLowerCase();
        if (driveLetter.charAt(0) == 'z')
        {
            driveLetter = "C";
        }
        else
        {
            driveLetter = "Z";
        }

        // change drive letter
        StringBuffer newIconPath = new StringBuffer(iconPath);
        newIconPath.replace(0, 1, driveLetter);
        return newIconPath.toString();
    }

    /**
     * Return file suffix of the default icon
     *
     * @return file suffix of the default icon ('.mif' is S60)
     */
    public String getDefaultIconSuffix()
    {
        return DEFAULT_SVG_ICON_SUFFIX;
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
        return _isOnDeviceKeypadNeeded();
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
        _notifyLauncherThatUiIsReady();
        return;
    }

    /**
     * Adds an entry to platform installation log.
     *
     * @param aSuite application suite information
     * @param aAction 0 - installation, 1 - uninstallation
     */
    public static void addInstallLogEntry(SuiteInfo aSuite, int aAction)
    {
        // In S60 the version number is restricted to limited range.
        // If version number is outside of this range, use version
        // number 0 instead.
        int result = _addInstallLogEntry(
            aAction, PlatformUid.getIntValue(aSuite.getUid()),
            aSuite.getName(), aSuite.getVendor(),
            getVersion(aSuite.getVersion().getMajor(), 0, 127),
            getVersion(aSuite.getVersion().getMinor(), 0, 99),
            getVersion(aSuite.getVersion().getMicro(), 0, 32767));
        if (result < 0)
        {
            Log.logError("Adding installation log entry failed, err=" + result);
        }
        else
        {
            Log.log("Added installation log entry");
        }
    }

    /*** ----------------------------- PACKAGE ---------------------------- */
    /*** ----------------------------- PRIVATE ---------------------------- */

    /**
     * Checks that given version number is between specified minimum and
     * maximum value range (inclusive) and if it is returns version number.
     * If version number is out of given range, returns 0.
     */
    private static int getVersion(int aVersion, int aMin, int aMax)
    {
        if (aVersion >= aMin && aVersion <= aMax)
        {
            return aVersion;
        }
        return 0;
    }

    /*** ----------------------------- NATIVE ----------------------------- */

    /**
     * Starts native application registration session.
     *
     * @return native session handle or Symbian error code (negative number)
     */
    private static native int _startSession();

    /**
     * Registers Java Application to S60 AppArc
     *
     * @param aSessionHandle
     * @param aUid The Uid of the application.
     * @param aGroupName
     * @param aMIDletName
     * @param aTargetDrive Must be "C:", "E:", ..., "J:"
     * @param aIconFileName the name of the icon file
     * @param aJarFileName Full path name of the .jar file
     * @param aHidden
     * @param aBackground
     * @return 0 if registration succeeded or Symbian error code
     */
    private static native int _registerApplication(
        int aSessionHandle,
        int aUid,
        String aGroupName,
        String aMIDletName,
        String aTargetDrive,
        String aIconFileName,
        String aJarFileName,
        boolean aHidden,
        boolean aBackground);

    /**
     * Unregisters Java Application from S60 AppArc
     *
     * @param aSessionHandle
     * @param aUid The Uid of the application to be unregistered..
     * @return 0 if unregistration succeeded or Symbian error code
     */
    private static native int _unregisterApplication(
        int aSessionHandle,
        int aUid);

    /**
     * Commits native application registration session.
     * If commit succeeds the native session is closed.
     *
     * @param aSessionHandle
     * @param aSynchronous if true, makes synchronous commit
     * @return 0 or Symbian error code (negative number)
     */
    private static native int _commitSession(int aSessionHandle, boolean aSynchronous);

    /**
     * Rolls back and closes native application registration session.
     *
     * @param aSessionHandle
     * @return 0 or Symbian error code (negative number)
     */
    private static native int _rollbackSession(int aSessionHandle);

    /**
     * Tells whether the uid is in use in AppArc.
     * Calls AppArc RApaLsSession::GetAppType
     *
     * @param aSessionHandle
     * @param aUid
     * @return 0 if the uid is not in use,
     *  1 if the uid is in use
     */
    private static native int _uidInUse(
        int aSessionHandle,
        int aUid);

    /**
     * Returns the logical group name of the application.
     * Calls AppArc RApaLsSession::GetAppCapability
     *
     * @param aSessionHandle
     * @param aUid
     * @param aGroupName should be "" when called, will contain group name
     *  when function returns if call was successfull
     * @return Symbian error code (negative number) if fails, otherwise 0
     */
    private static native int _getGroupName(
        int aSessionHandle,
        int aUid,
        StringBuffer aGroupName);

    /**
     * Closes native application registration session.
     *
     * @param aSessionHandle the session to be closed
     */
    private static native void _closeSession(int aSessionHandle);

    /**
     * Converts icon to platform specific format.
     *
     * @param aInputIconFilename file name for input icon file
     * @param aOutputIconFilename file name for output icon file
     * @param aJarFilename jar file name if aInputIconFilename specifies
     *     file inside jar file,
     *     or null if aInputIconFilename specifies file from disk
     * @param aIconSuffix the correct suffix of the icon is returned through
     *    this parameter, will contain '.mbm' or '.mif' when function returns
     * @return true if the conversion succeeds
     */
    private static native boolean _convertIcon(
        String aInputIconFilename,
        String aOutputIconFilename,
        String aJarFilename,
        StringBuffer aIconSuffix);

    /**
     * Returns true if the current device is touch screen
     * device without physical LSK and RSK
     *
     * @return true if virtual on screen keypad is needed
     */
    private static native boolean _isOnDeviceKeypadNeeded();

    /**
     * Sends a process to process notification to the parent process
     * of Java Installer telling that Installer is about to start
     * displaying UI dialogs.
     * The parent process reacts to the notification by hiding its
     * installation related dialogs.
     */
    private static native void _notifyLauncherThatUiIsReady();

    /**
     * Returns true if the code is executed in Symbian 9.2
     * emulator environment.
     *
     * @return true if Symbian 9.2 emulator environment
     */
    private static native boolean _runningIn92Emulator();

    /**
     * Adds an entry to platform installation log.
     *
     * @param aAction 0 - installation, 1 - uninstallation
     * @param aUid application suite uid
     * @param aName application suite name
     * @param aVendor application suite vendor
     * @param aMajorVersion application suite major version
     * @param aMinorVersion application suite minor version
     * @param aMicroVersion application suite micro version
     * @return Symbian error code (negative number) if fails, otherwise 0
     */
    private static native int _addInstallLogEntry(
        int aAction, int aUid, String aName, String aVendor,
        int aMajorVersion, int aMinorVersion, int aMicroVersion);
}
