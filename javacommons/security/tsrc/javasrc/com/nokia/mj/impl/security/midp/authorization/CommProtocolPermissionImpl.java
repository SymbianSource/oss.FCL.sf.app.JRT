/*
* Copyright (c) 2009 Nokia Corporation and/or its subsidiary(-ies).
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

package com.nokia.mj.impl.security.midp.authorization;

import com.nokia.mj.impl.security.common.PermissionBase;
import com.nokia.mj.impl.security.utils.SecurityPromptMessage;
import java.security.*;
import javax.microedition.io.*;

public class CommProtocolPermissionImpl
        extends PermissionBase
{
    private CommProtocolPermission iHelper;
    private SecurityPromptMessage pm = SecurityPromptMessage.getInstance();
    private String promptText;

    public CommProtocolPermissionImpl(String uri)
    {
        this(uri, null);
    }

    public CommProtocolPermissionImpl(String uri, String promptText)
    {
        super(uri);
        iHelper = new CommProtocolPermission(uri);
        this.promptText = promptText;
    }

    public boolean implies(Permission p)
    {
        if (p instanceof CommProtocolPermissionImpl)
        {
            return true;
        }
        return iHelper.implies(p);
    }

    public String getActions()
    {
        return iHelper.getActions();
    }
    public int hashCode()
    {
        return iHelper.hashCode();
    }

    public boolean equals(Object obj)
    {
        return iHelper.equals(obj);
    }

    public String getSecurityPromptQuestion(int aInteractionMode)
    {
        String q = "";
        if (promptText != null)
        {
            q = q + " " + promptText;
        }
        return "Networking prompt " + q;
    }

    public String toString()
    {
        return "(CommProtocolPermission" + " " + getName() + " " + getActions() + ")";
    }

}
