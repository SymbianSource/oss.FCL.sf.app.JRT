/*
* Copyright (c) 2010 Nokia Corporation and/or its subsidiary(-ies).
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

import com.nokia.mj.impl.installer.utils.InstallerException;
import com.nokia.mj.impl.installer.utils.Log;

/**
 * Sends installation and uninstallation progress notifications
 * to platform's software installation framework.
 */
public final class SifNotifier
{
    /** Install operation. */
    public static final int OP_INSTALL = 1;
    /** Uninstall operation. */
    public static final int OP_UNINSTALL = 2;
    /** Update operation. */
    public static final int OP_UPDATE = 3;

    /** Indicates installaion or uninstallation without
        specific suboperation. */
    public static final int SUB_OP_NO = 1;
    /** OCSP phase during installation. */
    public static final int SUB_OP_OCSP = 2;
    /** Download phase during installation. */
    public static final int SUB_OP_DOWNLOAD = 3;

    /** Operation being notified. */
    private int iOperation = 0;
    /** Global component id for the application. */
    private String iGlobalComponentId = null;
    /** Component name (i.e. suite name). */
    private String iComponentName = null;
    /** Array of pplication names. */
    private String[] iApplicationNames = null;
    /** Component initial size. */
    private int iComponentSize = 0;
    /** Component icon path. */
    private String iComponentIconPath = null;

    /*** ----------------------------- PUBLIC ------------------------------ */

    /**
     * Constructor.
     */
    public SifNotifier()
    {
    }

    /**
     * Returns true if SIF progress notifications are enabled, false otherwise.
     */
    public static boolean enabled()
    {
        return false;
    }

    /*
     * Notifies SIF that installation or uninstallation has started.
     *
     * @throws InstallerException in case an error occurs
     */
    public void notifyStart(
        int aOperation, String aGlobalComponentId, String aComponentName,
        String[] aApplicationNames, int aComponentSize,
        String aComponentIconPath)
    {
        iOperation = aOperation;
        iGlobalComponentId = aGlobalComponentId;
        iComponentName = aComponentName;
        iApplicationNames = aApplicationNames;
        iComponentSize = aComponentSize;
        iComponentIconPath = aComponentIconPath;
    }

    /**
     * Notifies SIF that installation or uinstallation has ended.
     *
     * @throws InstallerException in case an error occurs
     */
    public void notifyEnd(
        int aErrCategory, int aErrCode, String aErrMsg, String aErrMsgDetails)
    {
    }

    /**
     * Notifies SIF about installation or uninstallation progress.
     *
     * @throws InstallerException in case an error occurs
     */
    public void notifyProgress(int aSubOperation, int aCurrent, int aTotal)
    {
    }

    /**
     * Destroys SifNotifier. This method releawse native resources and
     * must be called after SifNotifier is no longer used.
     *
     * @throws InstallerException in case an error occurs
     */
    public void destroy()
    {
    }

    /*** ----------------------------- PACKAGE ---------------------------- */
    /*** ----------------------------- PRIVATE ---------------------------- */
}
