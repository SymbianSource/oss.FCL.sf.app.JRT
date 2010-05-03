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

import com.nokia.mj.impl.utils.Uid;

/**
 * Application info for JavaInstaller UI.
 */
public class ApplicationInfo
{
    /** Uid for the application. */
    private Uid iUid = null;
    /** Name for the application. */
    private String iName = null;
    /** Path to the icon file for the application. */
    private String iIconPath = null;

    /** Constructor. */
    private ApplicationInfo()
    {
    }

    /** Constructor. */
    public ApplicationInfo(Uid aUid, String aName, String aIconPath)
    {
        iUid = aUid;
        iName = aName;
        iIconPath = aIconPath;
    }

    /** Get application uid. */
    public Uid getUid()
    {
        return iUid;
    }

    /** Get application name. */
    public String getName()
    {
        return iName;
    }

    /** Get path to application icon. */
    public String getIconPath()
    {
        return iIconPath;
    }

    public String toString()
    {
        StringBuffer buf = new StringBuffer("Application ");
        buf.append("Uid: ").append(getUid()).append(", ");
        buf.append("Name: ").append(getName()).append(", ");
        buf.append("IconPath: ").append(getIconPath());
        return buf.toString();
    }
}
