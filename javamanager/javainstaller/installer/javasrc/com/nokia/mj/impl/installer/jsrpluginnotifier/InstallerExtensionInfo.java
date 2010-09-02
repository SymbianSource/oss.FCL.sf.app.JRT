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

import com.nokia.mj.impl.storage.StorageSession;
import com.nokia.mj.impl.utils.Logger;
import com.nokia.mj.impl.utils.Uid;
import java.util.Hashtable;

/**
 * Information passed to JSR plugins when installing or uninstalling
 * Java applications.
 * @see JsrPluginNotifier
 */
public final class InstallerExtensionInfo
{
    /**
     * MIDlet Suite Uid.
     */
    public Uid iUid = null;

    /**
     * Application Uids.
     */
    public Uid[] iAppUids = null;

    /**
     * Application suite root directory path.
     */
    public String iRootPath = null;

    /**
     * True if upgrade installation.
     */
    public boolean iUpgrade = false;

    /**
     * True if silent installation.
     */
    public boolean iSilent = false;

    /**
     * Combined jad/jar attributes.
     * Key is attribute name, value is com.nokia.mj.impl.utils.Attribute object
     * Can be null.
     */
    public Hashtable iAttributes = null;

    /**
     * JavaStorage session where installation/uninstallation changes are made.
     */
    public StorageSession iStorageSession = null;

    /*** ----------------------------- PUBLIC ------------------------------ */

    /**
     * Messages are logged to Java Installer log (at EInfo level).
     *
     * @param aMsg  The message to be logged.
     */
    public void log(String aMsg)
    {
        Logger.ILOG(Logger.EJavaInstaller, aMsg);
    }
}
