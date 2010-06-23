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

/**
 * It defines three types of permissions: user permission, allowed or
 * assigned permissions
 */
public interface PolicyBasedPermission
{
    public static final int USER_TYPE = 1;
    public static final int ALLOWED_TYPE = 2;
    public static final int ASSIGNED_TYPE = 3;
    public static final int USER_ASSIGNED_TYPE = 4;

    public String getName();
    public String getTarget();
    public String getActionList();
    public int getType();
    public UserSecuritySettings getUserSecuritySettings();
}
