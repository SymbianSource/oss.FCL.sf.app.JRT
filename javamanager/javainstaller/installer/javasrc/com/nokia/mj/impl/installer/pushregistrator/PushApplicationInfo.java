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


package com.nokia.mj.impl.installer.pushregistrator;

import com.nokia.mj.impl.utils.InstallerDetailedErrorMessage;
import com.nokia.mj.impl.utils.InstallerErrorMessage;
import com.nokia.mj.impl.utils.OtaStatusCode;
import com.nokia.mj.impl.utils.Uid;

/**
 * PushApplicationInfo contains information of an application in
 * the suite.
 */
public class PushApplicationInfo
{

    private Uid iUid = null;
    private String iClassName = null;

    /**
     * Private default constructor, not to be used by clients.
     */
    private PushApplicationInfo()
    {
    }

    /**
     * Construct a new PushApplicationInfo object.
     * @param aUid application uid
     * @param aClassName application's main class name
     */
    public PushApplicationInfo(Uid aUid, String aClassName)
    {
        if (aUid == null)
        {
            throw new PushRegistrationException
            (InstallerErrorMessage.INST_PUSH_REG_ERR, null,
             InstallerDetailedErrorMessage.INTERNAL_ERROR,
             new String[] { "PushApplicationInfo uid is null" },
             OtaStatusCode.PUSH_REGISTRATION_FAILURE);
        }
        if (aClassName == null)
        {
            throw new PushRegistrationException
            (InstallerErrorMessage.INST_PUSH_REG_ERR, null,
             InstallerDetailedErrorMessage.INTERNAL_ERROR,
             new String[] { "PushApplicationInfo classname is null" },
             OtaStatusCode.PUSH_REGISTRATION_FAILURE);
        }
        iUid = aUid;
        iClassName = aClassName;
    }

    /**
     * Get application uid.
     */
    public Uid getUid()
    {
        return iUid;
    }

    /**
     * Get class name.
     */
    public String getClassName()
    {
        return iClassName;
    }

    /**
     * Returns a hash code value for this object.
     */
    public int hashCode()
    {
        return getUid().hashCode() + getClassName().hashCode();
    }

    /**
     * Indicates whether given object is equal to this one.
     */
    public boolean equals(Object aObj)
    {
        if (!(aObj instanceof PushApplicationInfo))
        {
            return false;
        }
        PushApplicationInfo ai = (PushApplicationInfo)aObj;
        if (this.getUid().equals(ai.getUid()) &&
                this.getClassName().equals(ai.getClassName()))
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
        buf.append(getClassName());
        return buf.toString();
    }
}
