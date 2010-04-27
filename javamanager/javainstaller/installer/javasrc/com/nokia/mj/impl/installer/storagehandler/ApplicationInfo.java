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


package com.nokia.mj.impl.installer.storagehandler;

import com.nokia.mj.impl.installer.utils.ComponentId;
import com.nokia.mj.impl.installer.utils.InstallerException;
import com.nokia.mj.impl.utils.Uid;

public class ApplicationInfo
{
    // Nokia-MIDlet-Auto-Start-<n> values: "false", "true" and "once".
    public static final int AUTOSTART_FALSE = 0;
    public static final int AUTOSTART_TRUE = 1;
    public static final int AUTOSTART_ONCE = 2;

    private Uid iUid = null;
    private ComponentId iCid = null;
    private String iName = null;
    private String iIconPath = "";
    private String iMainClass = null;
    private int iAutoStart = AUTOSTART_FALSE;

    /** Filename for converted icon. This member is not saved into storage. */
    private String iConvertedIconPath = null;

    /**
     * Private default constructor, not to be used by clients.
     */
    private ApplicationInfo()
    {
    }

    /**
     * Construct a new ApplicationInfo object.
     * @param aUid application uid
     * @param aName application's name
     * @param aIconPath icon path
     * @param aMainClass application's main class
     * @param aAutoStart flag telling if this is an autostart application
     */
    public ApplicationInfo(
        Uid aUid, String aName, String aIconPath,
        String aMainClass, int aAutoStart)
    {
        this(aUid, aName, aMainClass, aAutoStart);
        iIconPath = aIconPath;
    }

    /**
     * Construct a new ApplicationInfo object.
     * @param aUid application uid
     * @param aName application's name
     * @param aMainClass application's main class
     * @param aAutoStart flag telling if this is an autostart application
     */
    public ApplicationInfo(
        Uid aUid, String aName, String aMainClass, int aAutoStart)
    {
        if (aUid == null)
        {
            InstallerException.internalError("ApplicationInfo uid is null");
        }
        if (aName == null)
        {
            InstallerException.internalError("ApplicationInfo name is null");
        }
        if (aMainClass == null)
        {
            InstallerException.internalError("ApplicationInfo mainclass is null");
        }
        iUid = aUid;
        iName = aName;
        iMainClass = aMainClass;
        iAutoStart = aAutoStart;
    }

    /**
     * Get application uid.
     */
    public Uid getUid()
    {
        return iUid;
    }

    /**
     * Get component id.
     */
    public ComponentId getComponentId()
    {
        return iCid;
    }

    /**
     * Set component id.
     */
    public void setComponentId(ComponentId aCid)
    {
        iCid = aCid;
    }

    /**
     * Get application name.
     */
    public String getName()
    {
        return iName;
    }

    /**
     * Get icon path.
     */
    public String getIconPath()
    {
        return iIconPath;
    }

    /**
     * Set icon path.
     */
    public void setIconPath(String aIconPath)
    {
        iIconPath = aIconPath;
    }

    /**
     * Get application main class.
     */
    public String getMainClass()
    {
        return iMainClass;
    }

    /**
     * Get application autostart flag.
     */
    public int getAutoStart()
    {
        return iAutoStart;
    }

    /**
     * Get icon path for converted icon.
     */
    public String getConvertedIconPath()
    {
        return iConvertedIconPath;
    }

    /**
     * Set icon path for converted icon.
     */
    public void setConvertedIconPath(String aConvertedIconPath)
    {
        iConvertedIconPath = aConvertedIconPath;
    }

    /**
     * Set application autostart flag.
     */
    public void setAutoStart(int aAutoStart)
    {
        iAutoStart = aAutoStart;
    }

    /**
     * Returns a hash code value for this object.
     */
    public int hashCode()
    {
        return iUid.hashCode() + iName.hashCode() + iMainClass.hashCode();
    }

    /**
     * Indicates whether given object is equal to this one.
     */
    public boolean equals(Object aObj)
    {
        if (!(aObj instanceof ApplicationInfo))
        {
            return false;
        }
        ApplicationInfo ai = (ApplicationInfo)aObj;
        if (this.getUid().equals(ai.getUid()) &&
                this.getName().equals(ai.getName()) &&
                this.getMainClass().equals(ai.getMainClass()) &&
                this.getAutoStart() == ai.getAutoStart() &&
                ((this.getIconPath() == null && ai.getIconPath() == null) ||
                 (this.getIconPath() != null &&
                  this.getIconPath().equals(ai.getIconPath()))))
        {
            return true;
        }
        return false;
    }

    /**
     * Returns string representation of this object.
     */
    public String toString()
    {
        StringBuffer buf = new StringBuffer();
        buf.append(getUid()).append(", ");
        if (getComponentId() != null)
        {
            buf.append(getComponentId()).append(", ");
        }
        buf.append(getName()).append(", ");
        if (getIconPath() != null)
        {
            buf.append(getIconPath());
        }
        buf.append(", ");
        buf.append(getMainClass());
        buf.append(", autostart=").append(getAutoStart());
        if (getConvertedIconPath() != null)
        {
            buf.append(", converted icon: ").append(getConvertedIconPath());
        }
        return buf.toString();
    }
}
