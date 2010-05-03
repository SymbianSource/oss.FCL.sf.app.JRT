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

package com.nokia.mj.impl.bluetooth;

import java.security.Permission;
import java.security.PermissionCollection;

import com.nokia.mj.impl.security.common.PermissionBase;
import com.nokia.mj.impl.security.utils.SecurityPromptMessage;

/**
 * Class defines permissions for Bluetooth protocols. It is to be used with
 * btl2cap and btspp protocols.
 * <p>
 * Corresponding MIDP2 permission strings are:<br/>
 * <ol>
 * <li>javax.microedition.io.Connector.bluetooth.server - For Server connections
 * </li>
 * <li>javax.microedition.io.Connector.bluetooth.client - For Client Connections
 * </li>
 * </ol>
 * <p>
 */
public class BTPermission extends PermissionBase
{
    public static String SERVER_MODE = "server";

    public static String CLIENT_MODE = "client";

    public String iMode = null;

    private String iDestination = null;

    /**
     * Constructs BTPermission instance.
     *
     * @param aUri
     *            URI passed to Connector.open()
     * @param aMode
     *            Mode in which the connection was requested. It can be either
     *            server or client mode.
     *
     */
    public BTPermission(String aUri, String aMode)
    {
        super(aUri);
        iMode = aMode;
    }

    /**
     * Returns the question (as localized text) associated with the security
     * prompt
     *
     * @return the localized text associated with the security prompt
     */
    public String getSecurityPromptQuestion(int aInteractionMode)
    {
        // either provide own localized text
        // or use the pre-defined texts
        // Note: some permissions use the same text always (case in which the
        // ID and the params for the text could be chosen at this phase) and
        // some permissions use a more dynamic text, depending on the context
        // in which they are used, case in which the ID and the params could
        // come from the constructor
        if (iMode.compareTo(SERVER_MODE) == 0)
        {
            return (SecurityPromptMessage.getInstance()).getText(
                       SecurityPromptMessage.QUESTION_ID_LOCAL_CONNECTIVITY, null);
        }

        if (iMode.compareTo(CLIENT_MODE) == 0)
        {
            return (SecurityPromptMessage.getInstance()).getText(
                       SecurityPromptMessage.QUESTION_ID_LOCAL_CONNECTIVITY, null);
        }

        return null;
    }

    public String toString()
    {
        if (iMode.compareTo(SERVER_MODE) == 0)
        {
            return "javax.microedition.io.Connector.bluetooth.server";
        }

        if (iMode.compareTo(CLIENT_MODE) == 0)
        {
            return "javax.microedition.io.Connector.bluetooth.client";
        }

        return null;
    }

    public boolean implies(Permission perm)
    {
        if (perm instanceof BTPermission)
        {
            BTPermission permission = (BTPermission) perm;
            if (matchActions(iMode, permission.iMode))
            {
                return true;
            }
        }
        return false;
    }

    public int hashCode()
    {
        return 0;
    }

    public boolean equals(Object obj)
    {
        return true;
    }

    public String getActions()
    {
        return "";
    }

    public PermissionCollection newPermissionCollection()
    {
        return null;
    }
}
