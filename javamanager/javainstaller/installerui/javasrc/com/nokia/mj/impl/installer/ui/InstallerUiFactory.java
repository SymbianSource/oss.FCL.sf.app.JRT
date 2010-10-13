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


package com.nokia.mj.impl.installer.ui;

import com.nokia.mj.impl.utils.Logger;

/**
 * Factory for JavaInstaller user interfaces.
 */
public class InstallerUiFactory
{
    /**
     * Get instance of installer UI.
     *
     * @param aMode InstallerUi.MODE_INSTALL or InstallerUi.MODE_UNINSTALL
     * @param aListener installer UI listener
     * @return installer UI
     */
    public static InstallerUi getInstallerUi
    (int aMode, InstallerUiListener aListener)
    {
        String className = "com.nokia.mj.impl.installer.ui.InstallerUi";
        String uiProperty =
            System.getProperty("com.nokia.mj.impl.installer.ui");
        if (uiProperty != null)
        {
            className = uiProperty;
        }

        InstallerUi installerUi = null;
        try
        {
            Class clazz = Class.forName(className);
            installerUi = (InstallerUi)clazz.newInstance();
            installerUi.init(aMode, aListener);
        }
        catch (Exception ex)
        {
            logError("Instantiating " + className + " failed", ex);
        }
        return installerUi;
    }

    /**
     * Writes an info log entry to JavaInstaller log.
     *
     * @param aMsg message to be logged
     */
    private static void log(String aMsg)
    {
        Logger.ILOG(Logger.EJavaInstaller, aMsg);
    }

    /**
     * Writes an error log entry to JavaInstaller log.
     *
     * @param aMsg message to be logged
     * @param aThrowable Throwable to be logged
     */
    private static void logError(String aMsg, Throwable aThrowable)
    {
        Logger.ELOG(Logger.EJavaInstaller, aMsg, aThrowable);
    }
}
