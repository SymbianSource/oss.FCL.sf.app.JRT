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

import com.nokia.mj.impl.security.midp.authorization.TestPermissionMappingTable;

import java.util.Hashtable;
import java.util.Enumeration;

/**
 * Mapping between named permissions and class named permissions
 */
public class PermissionMappingTable
{
    /**
     * Getter for permission mappings
     */
    public static MIDPPermission get(String permissionName)
    {
        return (MIDPPermission)TestPermissionMappingTable.getMappingTable().get(permissionName);
    }
}
