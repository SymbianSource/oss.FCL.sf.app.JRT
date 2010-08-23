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


package com.nokia.mj.impl.gcf.protocol.socket;

import com.nokia.mj.impl.security.common.PermissionBase;
import java.security.Permission;
import java.security.PermissionCollection;
import com.nokia.mj.impl.security.utils.SecurityPromptMessage;

public class SocketPermissionImpl extends PermissionBase
{
    /**
     * Constants for identifying the target/uri of connection: client or server
     */
     static String CLIENT_TARGET = "socket://*";
     static String SERVER_TARGET = "socket://";

     private String iTarget;

    /**
     * Returns the question (as localized text) associated with the security
     * prompt
     *
     * @return the localized text associated with the security prompt
     */

    public SocketPermissionImpl(String uri)
    {
        super(uri);
        iTarget = uri;
    }

    public String getSecurityPromptQuestion(int aInteractionMode)
    {
        // either provide own localized text
        // or use the pre-defined texts
        // Note: some permissions use the same text always (case in which the
        // ID and the params for the text could be chosen at this phase) and
        // some permissions use a more dynamic text, depending on the context
        // in which they are used, case in which the ID and the params could
        // come from the constructor

        return (SecurityPromptMessage.getInstance()).getText(
                   SecurityPromptMessage.QUESTION_ID_NETWORK_USAGE, null);

    }

    public String toString()
    {
        if (SERVER_TARGET.equals(iTarget))
        {
            return "javax.microedition.io.Connector.serversocket";
        }
        return "javax.microedition.io.Connector.socket";
    }

    public boolean implies(Permission p)
    {
        if (p instanceof SocketPermissionImpl)
        {
            SocketPermissionImpl o = (SocketPermissionImpl)p;
            if ((iTarget != null && iTarget.equals(o.getTarget()))
                || (iTarget == null && o.getTarget() == null))
            {
                return true;
            }
        }
        return false;
    }

    public boolean equals(Object obj)
    {
        return true;
    }

    public int hashCode()
    {
        return 0;
    }

    public String getActions()
    {
        return "";
    }

    public PermissionCollection newPermissionCollection()
    {
        return null;
    }

    String getTarget()
    {
        return iTarget;
    }
}