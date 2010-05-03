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

import com.nokia.mj.impl.utils.exception.InvalidAttributeException;
import com.nokia.mj.impl.utils.InstallerDetailedErrorMessage;
import com.nokia.mj.impl.utils.InstallerErrorMessage;
import com.nokia.mj.impl.utils.JavaLanguageSyntaxChecker;
import com.nokia.mj.impl.utils.Logger;
import com.nokia.mj.impl.utils.OtaStatusCode;
import com.nokia.mj.impl.utils.Tokenizer;
import com.nokia.mj.impl.utils.Uid;

/**
 * PushInfo contains information of a single push connection.
 */
public class PushInfo
{
    // PushRegistrator implementation class name
    public static final String PUSH_REGISTRATOR_CLASS =
        "com.nokia.mj.impl.push.PushRegistratorImpl";

    // Registration types
    public static final int REG_DYNAMIC = 0;
    public static final int REG_STATIC = 1;

    private Uid iUid = null;
    private String iName = null;
    private String iConnectionUrl = null;
    private String iClassName = null;
    private String iFilter = null;
    private int iRegType = REG_STATIC;

    /**
     * Construct a new PushInfo object.
     * @param aUid application uid
     * @param aPushAttribute application's push attribute value
     */
    public PushInfo(Uid aUid, String aPushAttribute)
    {
        String[] tokens = Tokenizer.split(aPushAttribute, ",");
        if (tokens == null || tokens.length != 3)
        {
            Logger.ELOG(Logger.EJavaInstaller,
                        "Invalid number of tokens in push attribute: " +
                        aPushAttribute);
            throw new InvalidAttributeException
            (InstallerErrorMessage.INST_CORRUPT_PKG,
             null,
             InstallerDetailedErrorMessage.ATTR_HANDLING_FAILED,
             new String[] { aPushAttribute },
             OtaStatusCode.INVALID_DESCRIPTOR);
        }
        iUid = aUid;
        iConnectionUrl = tokens[0].trim();
        iClassName = tokens[1].trim();
        iFilter = tokens[2].trim();
        if (!JavaLanguageSyntaxChecker.checkClassName(iClassName))
        {
            Logger.ELOG(Logger.EJavaInstaller,
                        "Invalid class name in push attribute: " +
                        aPushAttribute);
            throw new InvalidAttributeException
            (InstallerErrorMessage.INST_CORRUPT_PKG,
             null,
             InstallerDetailedErrorMessage.ATTR_BAD_SYNTAX,
             new String[] { aPushAttribute },
             OtaStatusCode.INVALID_DESCRIPTOR);
        }
    }

    /**
     * Construct a new PushInfo object.
     * @param aUid application uid
     * @param aConnectionUrl connection url
     * @param aClassName class name
     * @param aFilter filter
     * @param aRegType registration type
     */
    public PushInfo(
        Uid aUid, String aConnectionUrl, String aClassName,
        String aFilter, int aRegType)
    {
        iUid = aUid;
        iConnectionUrl = aConnectionUrl;
        iClassName = aClassName;
        iFilter = aFilter;
        iRegType = aRegType;
    }

    /**
     * Set application name.
     */
    public void setName(String aName)
    {
        iName = aName;
    }

    /**
     * Get application name.
     */
    public String getName()
    {
        return iName;
    }

    /**
     * Set application uid.
     */
    public void setUid(Uid aUid)
    {
        iUid = aUid;
    }

    /**
     * Get application uid.
     */
    public Uid getUid()
    {
        return iUid;
    }

    /**
     * Get connection url.
     */
    public String getConnectionUrl()
    {
        return iConnectionUrl;
    }

    /**
     * Get class name.
     */
    public String getClassName()
    {
        return iClassName;
    }

    /**
     * Get filter.
     */
    public String getFilter()
    {
        return iFilter;
    }

    /**
     * Get push attribute value.
     */
    public String getPushAttribute()
    {
        return getConnectionUrl() + ", " + getClassName() + ", " + getFilter();
    }

    /**
     * Get registration type.
     */
    public int getRegType()
    {
        return iRegType;
    }

    /**
     * Get string representation of this object.
     */
    public String toString()
    {
        String regType = (getRegType() == REG_STATIC? "static": "dynamic");
        return getName() + ", " + getUid() + ", " + regType +
               ": " + getPushAttribute();
    }
}
