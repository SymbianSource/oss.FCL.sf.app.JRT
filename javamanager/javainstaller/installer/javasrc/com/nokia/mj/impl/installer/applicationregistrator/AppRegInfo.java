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

import com.nokia.mj.impl.utils.Uid;


/**
 * Contains all information needed to register Java application to
 * any platform.
 */
public final class AppRegInfo
{
    /**
     * Java application Uid  (MIDlet Uid).
     */
    protected Uid iUid;

    /**
     * The value of Nokia-MIDlet-Category attribute or empty.
     */
    protected String iGroupName;

    /**
     * MIDlet name from MIDlet-<n> attribute.
     */
    protected String iMIDletName;

    /**
     * The installation drive
     */
    protected String iTargetDrive;

    /**
     * Full path name to the icon file.
     */
    protected String iIconFileName;

    /**
     * Full path name to the jar file.
     */
    protected String iJarFileName;

    /**
     * Should be application be hidden from user.
     * Optional. By default false;
     */
    protected boolean iHidden = false;

    /**
     * Should the application be executed in background.
     * Optional. By default false;
     */
    protected boolean iBackground = false;


    /*** ----------------------------- PUBLIC ------------------------------ */

    /**
     * Construct new AppRegInfo object.
     * When this object is used for registering applications, all
     * params must be valid. When used for unregistering applications,
     * only aUid is really needed.
     * Use default values for optional information.
     *
     * @param aUid  Java application Uid  (MIDlet Uid).
     * @param aGroupName    The value of Nokia-MIDlet-Category attribute or empty.
     *  Note that in Symbian only 16 first characters are stored to
     *  application registry.
     * @param aMIDletName     MIDlet name from MIDlet-<n> attribute.
     * @param aTargetDrive    The installation drive, e.g. "C:" in S60
     * @param aIconFileName   The full path name to the icon file in file system.
     *    The file must be accessible and the path must have \\ chars.
     * @param aJarFileName    Full path name to jar file.
     * @see ApplicationRegistrator
     */
    public AppRegInfo(
        Uid aUid,
        String aGroupName,
        String aMIDletName,
        String aTargetDrive,
        String aIconFileName,
        String aJarFileName)
    {
        iUid = aUid;
        iGroupName = aGroupName;
        iMIDletName = aMIDletName;
        iTargetDrive = aTargetDrive;
        iIconFileName = aIconFileName;
        iJarFileName = aJarFileName;
    }


    /**
     * Construct new AppRegInfo object.
     * When this object is used for registering applications, all
     * params must be valid. When used for unregistering applications,
     * only aUid is really needed.
     * Specify all information.
     *
     * @param aUid  Java application Uid  (MIDlet Uid).
     * @param aGroupName     The value of Nokia-MIDlet-Category attribute or empty.
     *  Note that in Symbian only 16 first characters are stored to
     *  application registry.
     * @param aMIDletName    MIDlet name from MIDlet-<n> attribute.
     * @param aTargetDrive   The installation drive, e.g. "C:" in S60
     * @param aIconFileName   The full path name to the icon file in file system.
     * @param aJarFileName   Full path name to jar file.
     * @param aHidden        Should be application be hidden from user.
     * @param aBackground    Should the application be executed in background.
     * @see ApplicationRegistrator
     */
    public AppRegInfo(
        Uid aUid,
        String aGroupName,
        String aMIDletName,
        String aTargetDrive,
        String aIconFileName,
        String aJarFileName,
        boolean aHidden,
        boolean aBackground)
    {
        iUid = aUid;
        iGroupName = aGroupName;
        iMIDletName = aMIDletName;
        iTargetDrive = aTargetDrive;
        iIconFileName = aIconFileName;
        iJarFileName = aJarFileName;
        iHidden = aHidden;
        iBackground = aBackground;
    }


    /**
     * Returns string representation of this object.
     */
    public String toString()
    {
        StringBuffer buf = new StringBuffer("AppRegInfo:\n");
        buf.append("Uid: ").append(iUid).append("\n");
        buf.append("GroupName: ").append(iGroupName).append("\n");
        buf.append("MIDletName: ").append(iMIDletName).append("\n");
        buf.append("TargetDrive: ").append(iTargetDrive).append("\n");
        buf.append("IconFileName: ").append(iIconFileName).append("\n");
        buf.append("JarFileName: ").append(iJarFileName).append("\n");
        buf.append("Hidden: ").append(iHidden).append("\n");
        buf.append("Background: ").append(iBackground).append("\n");
        return buf.toString();
    }

    /*** ----------------------------- PRIVATE ---------------------------- */

    /**
     * Deny other code from using default constructor.
     */
    private AppRegInfo()
    {
    }

}
