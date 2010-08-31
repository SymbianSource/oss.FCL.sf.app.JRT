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


package com.nokia.mj.impl.media;

import java.security.Permission;
import java.security.PermissionCollection;
import com.nokia.mj.impl.security.common.PermissionBase;
import com.nokia.mj.impl.security.utils.SecurityPromptMessage;

public class RTSPPermission extends PermissionBase
{
    public RTSPPermission(String aUri)
    {
        super(aUri);
    }

    /**
     * Returns the question (as localized text) associated with the security
     * prompt
     *
     * @return the localized text associated with the security prompt
     */
    public String getSecurityPromptQuestion(int aInteractionMode)
    {
        return (SecurityPromptMessage.getInstance()).getText(
                   SecurityPromptMessage.QUESTION_ID_NETWORK_USAGE,
                   null);
    }


    public String toString()
    {
        return "javax.microedition.io.Connector.rtsp";
    }

    public boolean implies(Permission p)
    {
        if (p instanceof RTSPPermission)
        {
            return true;
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
}