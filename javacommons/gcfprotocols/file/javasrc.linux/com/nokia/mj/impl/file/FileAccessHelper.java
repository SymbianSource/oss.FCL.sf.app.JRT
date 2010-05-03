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

package com.nokia.mj.impl.file;

import java.util.Hashtable;
import com.nokia.mj.impl.rt.support.ApplicationInfo;

/**
 * Linux specific checks. Logic to be added when things become clear.
 */
public class FileAccessHelper implements FileConstants
{
    public static boolean accessAllowed(String aPath, String aIntent,
                                        String aDomain, boolean aIsOpening)
    {
        return true;
    }

    public static boolean manufacturerDomainChecks(String aPath, String aIntent)
    {
        return true;
    }

    private static boolean otherDomainChecks(String aPath, String aIntent)
    {
        return true;
    }

    public static String getCategory(String aPath)
    {
        // SYSTEM_FILES, PRIVATE_USER_FILES, PUBLIC_DIRS, HOME_DIR
        return PUBLIC_DIRS;
    }

    public static boolean isCreateAllowedWithinDir(String aPath, String aDomain)
    {
        return true;
    }

    /**
     * To be used in case of list. If list is done on a directory that is equal
     * to, or higher in path hierarchy than one of the restricted paths, then we
     * need to check for access for all files, if not, then no need.
     */
    public static boolean isDirRestricted(String aPath)
    {
        return false;
    }
}
