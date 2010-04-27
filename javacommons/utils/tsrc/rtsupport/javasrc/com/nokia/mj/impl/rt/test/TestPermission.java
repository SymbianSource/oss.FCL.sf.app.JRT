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


package com.nokia.mj.impl.rt.test;

import java.security.Permission;

public class TestPermission extends Permission
{

    public TestPermission(String permissionName)
    {
        super("TestPermission");
        System.out.println("TestPermission.TestPermission()");
    }

    public boolean equals(Object obj)
    {
        System.out.println("TestPermission.equals");
        return false;
    }

    public int hashCode()
    {
        System.out.println("TestPermission.hashCode");
        return 0;
    }

    public String getActions()
    {
        System.out.println("TestPermission.getActions");
        return "TestPermission Actions";
    }

    public boolean implies(Permission permission)
    {
        System.out.println("TestPermission.implies");
        return true;
    }
}
