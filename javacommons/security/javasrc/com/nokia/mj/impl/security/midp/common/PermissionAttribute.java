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

package com.nokia.mj.impl.security.midp.common;

import com.nokia.mj.impl.utils.Tokenizer;
import com.nokia.mj.impl.utils.exception.InvalidAttributeException;
import com.nokia.mj.impl.utils.InstallerDetailedErrorMessage;
import com.nokia.mj.impl.utils.InstallerErrorMessage;
import com.nokia.mj.impl.utils.OtaStatusCode;
import com.nokia.mj.impl.utils.JavaLanguageSyntaxChecker;
import com.nokia.mj.impl.security.utils.Logger;
import java.util.Hashtable;
import java.util.Vector;

/**
 * MIDP authorization is about protecting access to sensitive functionality.
 * One of the main concepts in MIDP authorization is Permission:
 *    - the means to protect access to sensitive functionality
 *    - it is a triplet {permission class, target, action} where target defines
 *      the protected resource and the action defines the action to be carried
 *      upon the resource
 *    - MIDlet suites request permissions in the JAD file. The requested
 *      permissions have an importance: mandatory = permissions which are
 *      critical for the MIDlet's functionality and optional = permissions
 *      which are not critical for the MIDlet's functionality.
 *      The permission JAD attributes used in MIDP2 (legacy attributes)
 *      are different than the permission JAD attributes used in MIDP3.
 * This class understands them both MIDP2 and MIDP3 permission JAD attributes
 * This class helps in extracting the permission information from the
 * application descriptor (JAD)
 */
public abstract class PermissionAttribute
        extends MIDPPermission
{
    /**
     * Constant assigned to mandatory permissions
     */
    public static final int MANDATORY_PERMISSION = 1;

    /**
     * Constant assigned to optional permissions
     */
    public static final int OPTIONAL_PERMISSION = 2;

    /**
     * The JAD attribute holding the optional permissions
     */
    public static final String OPTIONAL_ATTRIBUTE_PREFIX
    = "MIDlet-Permission-Opt-";

    /**
     * The legacy JAD attribute holding the optional permissions
     */
    public static final String OPTIONAL_LEGACY_ATTRIBUTE_NAME
    = "MIDlet-Permissions-Opt";

    /**
     * The JAD attribute holding the mandatory permissions
     */
    public static final String MANDATORY_ATTRIBUTE_PREFIX
    = "MIDlet-Permission-";

    /**
     * The legacy JAD attribute holding the mandatory permissions
     */
    public static final String MANDATORY_LEGACY_ATTRIBUTE_NAME
    = "MIDlet-Permissions";

    /**
     * The name of the attribute
     */
    private String attrName  = null;

    PermissionAttribute(String attrName, String attrValue)
    {
        // attrValue = name[" "target[" "actionList]]
        String[] tokens = Tokenizer.split(attrValue, " ");
        if (tokens == null || tokens.length > 3)
        {
            // let's give it one more try: remove the leading and trailing spaces
            int i=0;
            // get the classname
            i = skipSpaces(attrValue, i);
            name = readChars(attrValue, i);
            i = i + name.length();
            // get the target
            i = skipSpaces(attrValue, i);
            target = readChars(attrValue, i);
            i = i + target.length();
            // get the actionList
            i = skipSpaces(attrValue, i);
            actionList = readChars(attrValue, i);
            i = i + actionList.length();
            i = skipSpaces(attrValue, i);
            // if we are not at the end of the attrValue it's a failure
            if (i<attrValue.length())
            {
                throw new InvalidAttributeException(
                    InstallerErrorMessage.INST_CORRUPT_PKG,
                    null, /* no params for short msg */
                    InstallerDetailedErrorMessage.ATTR_HANDLING_FAILED,
                    new String[] {attrName},
                    OtaStatusCode.APPLICATION_AUTHENTICATION_FAILURE);
            }
            // switch empty to null
            if (name.length() == 0)
            {
                name = null;
            }
            if (target.length() == 0)
            {
                target = null;
            }
            if (actionList.length() == 0)
            {
                actionList = null;
            }
        }
        else
        {
            switch (tokens.length)
            {
            case 3:
                actionList = tokens[2];
            case 2:
                target = tokens[1];
            case 1:
                name = tokens[0];
            }
        }
        // try to map it to the internal name
        MIDPPermission internalPerm = PermissionMappingTable.get(name);
        if (internalPerm == null)
        {
            internalPerm = SecurityExtensionsReader.getExtPermission(name);
        }
        if (internalPerm != null)
        {
            name = internalPerm.getName();
        }
        // validate the classname
        if (!JavaLanguageSyntaxChecker.checkClassName(name))
        {
            throw new InvalidAttributeException(
                InstallerErrorMessage.INST_CORRUPT_PKG,
                null, /* no params for short msg */
                InstallerDetailedErrorMessage.ATTR_HANDLING_FAILED,
                new String[] {attrName},
                OtaStatusCode.APPLICATION_AUTHENTICATION_FAILURE);
        }
        this.attrName = attrName;
    }

    PermissionAttribute(
        String name,
        String target,
        String actionList,
        String attrName)
    {
        super(name, target, actionList);
        this.attrName = attrName;
    }

    PermissionAttribute(String attrName)
    {
        this.attrName = attrName;
    }

    public abstract int getImportance();

    public abstract String toString();

    public String getAttributeName()
    {
        return attrName;
    }

    public boolean isLegacyAttribute()
    {
        return (attrName != null
                && (attrName.equalsIgnoreCase(MANDATORY_LEGACY_ATTRIBUTE_NAME)
                    || attrName.equalsIgnoreCase(OPTIONAL_LEGACY_ATTRIBUTE_NAME)));
    }

    protected static MIDPPermission[] InitFromLegacyAttribute(String attrName, String attrValue, boolean ignoreUnknownValues)
    {
        // attrValue = comma separated list of individual permissions
        String[] legacyPerms = Tokenizer.split(attrValue, ",");
        if (legacyPerms == null || legacyPerms.length == 0)
        {
            throw new InvalidAttributeException(
                InstallerErrorMessage.INST_CORRUPT_PKG,
                null, /* no params for short msg */
                InstallerDetailedErrorMessage.ATTR_HANDLING_FAILED,
                new String[] {attrName},
                OtaStatusCode.APPLICATION_AUTHENTICATION_FAILURE);
        }
        Vector midp3Perms = new Vector();
        for (int i=0; i<legacyPerms.length; i++)
        {
            // remove leading and trailing whitespaces
            legacyPerms[i] = legacyPerms[i].trim();
            MIDPPermission midp3Perm = PermissionMappingTable.get(
                                           legacyPerms[i]);
            if (midp3Perm == null)
            {
                midp3Perm = SecurityExtensionsReader.getExtPermission(legacyPerms[i]);
            }
            if (midp3Perm == null)
            {
                if (!ignoreUnknownValues)
                {
                    Logger.logError("Permission " + legacyPerms[i] + " not supported");
                    throw new InvalidAttributeException(
                        InstallerErrorMessage.INST_CORRUPT_PKG,
                        null, /* no params for short msg */
                        InstallerDetailedErrorMessage.ATTR_UNSUPPORTED,
                        new String[] {attrName},
                        OtaStatusCode.APPLICATION_AUTHORIZATION_FAILURE);
                }
            }
            else
            {
                // don't put duplicates
                if (!midp3Perms.contains(midp3Perm))
                {
                    midp3Perms.addElement(midp3Perm);
                }
            }
        }
        MIDPPermission[] perms = new MIDPPermission[midp3Perms.size()];
        midp3Perms.copyInto(perms);
        return perms;
    }

    private int skipSpaces(String str, int startIndex)
    {
        while (startIndex<str.length()
                && str.charAt(startIndex) == ' ')
        {
            startIndex++;
        }
        return startIndex;
    }

    private String readChars(String str, int startIndex)
    {
        String rStr = "";
        while (startIndex<str.length()
                && str.charAt(startIndex) != ' ')
        {
            rStr = rStr + str.charAt(startIndex);
            startIndex++;
        }
        return rStr;
    }

}
