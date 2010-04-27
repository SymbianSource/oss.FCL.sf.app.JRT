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

package com.nokia.mj.impl.security.common;

import java.security.Permission;

public class PermissionImpl
        extends PermissionBase
{
    public PermissionImpl(String name)
    {
        super(name);
    }

    public String getSecurityPromptQuestion(int aInteractionMode)
    {
        return "securityPromptQuestion";
    }

    public String getActions()
    {
        return "";
    }

    public int hashCode()
    {
        return 0;
    }

    public boolean equals(Object obj)
    {
        return true;
    }

    public boolean implies(Permission p)
    {
        return true;
    }

    public String toString()
    {
        return "java.security.Permission";
    }

}
