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

import com.nokia.mj.impl.utils.Logger;
import com.nokia.mj.impl.utils.Uid;
import java.util.Hashtable;

/**
 * Information passed to JSR plugins when installing or uninstalling
 * Java applications
 *
 * @author Nokia Corporation
 * @version $Rev: 10381 $ $Date: 2010-04-06 15:34:34 +0300 (Tue, 06 Apr 2010) $
 * @see JsrPluginNotifier
 */
public final class InstallerExtensionInfo
{
    /**
     * MIDlet Suite Uid. Can be null.
     */
    public Uid iUid;

    /**
     * true if upgrade installation
     */
    public boolean iUpgrade;

    /**
     * true if silent installation
     */
    public boolean iSilent;

    /**
     * Combined jad./.jar attributes.
     * Key is attribute name, value is com.nokia.mj.impl.utils.Attribute object
     * Can be null.
     */
    public Hashtable iAttributes;

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
