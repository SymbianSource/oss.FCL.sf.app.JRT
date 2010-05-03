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


package com.nokia.mj.impl.gcf.protocol.http;

import com.nokia.mj.impl.security.common.PermissionBase;
import java.security.Permission;
import java.security.PermissionCollection;
import com.nokia.mj.impl.security.utils.SecurityPromptMessage;

public class HttpPermissionImpl extends PermissionBase
{
    /**
     * Returns the question (as localized text) associated with the security
     * prompt
     *
     * @return the localized text associated with the security prompt
     */
    private String iMessage = null;

    public HttpPermissionImpl(String uri)
    {
        super(uri);
    }

    public String getSecurityPromptQuestion(int aInteractionMode)
    {
        return (SecurityPromptMessage.getInstance()).getText(
                   SecurityPromptMessage.QUESTION_ID_NETWORK_USAGE, new String[]
                   {
                       Integer.toString(1), iMessage
                   });
    }

    public String toString()
    {
        return "javax.microedition.io.HttpProtocolPermission";
    }

    public boolean implies(Permission p)
    {
        if (p instanceof HttpPermissionImpl)
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