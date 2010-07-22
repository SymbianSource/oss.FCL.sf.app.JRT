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


package com.nokia.mj.impl.gcf.protocol.comm;


import com.nokia.mj.impl.security.common.PermissionBase;
import java.security.Permission;
import java.security.PermissionCollection;
import com.nokia.mj.impl.security.utils.SecurityPromptMessage;

public class CommPermissionImpl extends PermissionBase
{

    /**
     * Returns the question (as localized text) associated with the security
     * prompt
     *
     * @return the localized text associated with the security prompt
     */

    public CommPermissionImpl(String uri)
    {
        super(uri);
    }

    public String getSecurityPromptQuestion(int aInteractionMode)
    {
        return (SecurityPromptMessage.getInstance()).getText(
                   SecurityPromptMessage.QUESTION_ID_LOCAL_CONNECTIVITY, null);
    }

    public String toString()
    {
        return "javax.microedition.io.Connector.comm";
    }

    public boolean implies(Permission p)
    {
        if (p instanceof CommPermissionImpl)
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