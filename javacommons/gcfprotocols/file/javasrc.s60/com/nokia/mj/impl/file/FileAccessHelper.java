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
import java.util.Vector;
import com.nokia.mj.impl.rt.support.ApplicationInfo;

public class FileAccessHelper implements FileConstants
{
    public static int PATHS_EQUAL = 0;
    public static int PATHS_NO_MATCH = 1;
    public static int PATH_ABOVEIN_HIERARCHY = 2;
    public static int PATH_BELOWIN_HIERARCHY = 3;
    private static String PATH_PRIVATE = "private";
    private static String PATH_SYSTEM = "system";
    private static String[] iRestrictedPathList;
    private static String[] iForbiddenPathList;

    static
    {
        // Populate forbidden path list.
        Vector forbidden = FileSystemUtils.getForbiddenPaths();
        iForbiddenPathList = new String[forbidden.size()];
        for (int index = 0; index < forbidden.size(); index++)
        {
            iForbiddenPathList[index] = (String) forbidden.elementAt(index);
        }

        // Populate restricted path list.
        Vector restricted = FileSystemUtils.getRestrictedPaths();
        iRestrictedPathList = new String[restricted.size() + 1];
        for (int index = 0; index < restricted.size(); index++)
        {
            iRestrictedPathList[index] = (String) restricted.elementAt(index);
        }
        // Add midlet's private directory also to restricted path list.
        iRestrictedPathList[restricted.size()] = FileSystemUtils
                .getAppPrivateDir();
    }

    /**
     * Checks to see if the application has access to a specific path.
     *
     * @param aPath
     *            path which the application is trying to access.
     * @param aIntent
     *            mode in which the application wants to access the target.
     * @param aDomain
     *            domain of the application
     * @param aIsOpening
     *            if the operation being performed is equivalent to opening a
     *            connection.
     * @return true in case access is allowed. False otherwise
     */
    public static boolean accessAllowed(String aPath, String aIntent,
                                        String aDomain, boolean aIsOpening)
    {
        FileLogger.Log("FileAccessHelper.accessAllowed: Checking access: \n");

        if (isHomeDir(aPath))
        {
            return true;
        }

        if (isForbidden(aPath))
        {
            return false;
        }

        if (isIllegalAccessToRestrictedDir(aPath, aIntent, aIsOpening, aDomain))
        {
            return false;
        }

        if (aDomain.equals(ApplicationInfo.MANUFACTURER_DOMAIN))
        {
            return manufacturerDomainChecks(aPath, aIntent, aIsOpening);
        }
        else
        {
            return otherDomainChecks(aPath, aIntent, aIsOpening);
        }
    }

    /**
     * To be used in case of list. If list is done on a directory that is equal
     * to, or higher in path hierarchy than one of the restricted paths, then we
     * need to check for access for all files, if not, then no need.
     */
    public static boolean isDirRestricted(String aPath)
    {
        for (int index = 0; index < iRestrictedPathList.length; index++)
        {
            int matchResult = matchPaths(aPath, iRestrictedPathList[index]);
            if ((matchResult != PATHS_NO_MATCH)
                    && (matchResult != PATH_BELOWIN_HIERARCHY))
            {
                return true;
            }
        }
        return false;
    }

    /**
     * Checks to see if the path being accessed in forbidden.
     *
     * @param aPath
     *            path being accessed
     * @return true in case the path is forbidden, false otherwise
     */
    private static boolean isForbidden(String aPath)
    {
        for (int index = 0; index < iForbiddenPathList.length; index++)
        {
            int matchPathResult = matchPaths(aPath, iForbiddenPathList[index]);

            // Forbidden paths should match exactly or should be such that the
            // path must be lower in hierarchy.
            // Example: e:/system is forbidden, e:/ is not.
            // e:/system is forbidden, e:/system/dir is also forbidden
            if ((matchPathResult == PATHS_EQUAL)
                    || (matchPathResult == PATH_BELOWIN_HIERARCHY))
            {
                return true;
            }
        }
        return false;
    }

    /**
     * Checks if the access to restricted paths is being made in correct intent
     * based on the domain.
     *
     * @param aPath
     *            path of the file/directory being accessed
     * @param aIntent
     *            intent with which it is being accessed (read or write)
     * @param aOpening
     *            set to true in case it is being used by Connector.open or
     *            setFileConnection. Both are considered as open and not as
     *            acutal read or write operations.
     * @param aDomain
     *            domain of the application.
     * @return true in case there is an access violation, false if the access is
     *         allowed.
     */
    public static boolean isIllegalAccessToRestrictedDir(String aPath,
            String aIntent, boolean aOpening, String aDomain)
    {
        for (int index = 0; index < iRestrictedPathList.length; index++)
        {
            int matchResult = matchPaths(aPath, iRestrictedPathList[index]);
            if ((matchResult != PATHS_NO_MATCH)
                    && (matchResult != PATH_BELOWIN_HIERARCHY))
            {
                if (aIntent.equals(INTENT_WRITE)
                        || aIntent.equals(INTENT_READ_WRITE))
                {
                    if (!aOpening)
                    {
                        return true;
                    }
                }
            }
        }
        return false;
    }

    /**
     * Performs manufacturer domain specific checks. Manufacturer domain apps
     * are not allowed to access any path in C:/private apart from its private
     * directory. Other checks are forbidden directories and restricted paths.
     * This is done before (accessAllowed()) So, no need to check once again.
     *
     * @param aPath
     *            path which is being accessed
     * @param aIntent
     *            intent with which access is being made. read, write
     * @param aIsOpening
     *            true in case its an open operation (open, setFileConnection)
     * @return true in case access is allowed. false otherwise.
     */
    public static boolean manufacturerDomainChecks(String aPath,
            String aIntent, boolean aIsOpening)
    {
        // Check if it is private directory.
        if (aPath.indexOf(PATH_PRIVATE) == 3)
        {
            if (matchPaths(aPath, FileSystemUtils.getAppPrivateDir()) == PATHS_NO_MATCH)
            {
                return false;
            }
        }
        return true;
    }

    /**
     * Performs domains other than manufacturer domain.
     *
     * @param aPath
     *            path which is being accessed
     * @param aIntent
     *            intent with which access is being made. read, write
     * @param aIsOpening
     *            true in case its an open operation (open, setFileConnection)
     * @return true in case access is allowed. false otherwise.
     */
    private static boolean otherDomainChecks(String aPath, String aIntent,
            boolean aIsOpening)
    {
        if (aPath.length() < 3)
        {
            // Path will be valid. This will be only in case file:///c: is given
            aPath += "/";
        }

        String rom = FileSystemUtils.getRomDrive().toLowerCase()
                     .substring(0, 2);
        String temp = FileSystemUtils.getTemporaryDrive().toLowerCase()
                      .substring(0, 2);

        if (aPath.toLowerCase().startsWith(rom)
                || aPath.toLowerCase().startsWith(temp))
        {
            return false;
        }

        // Other domains can access only below restricted paths or
        // in other drives.
        for (int index = 0; index < iRestrictedPathList.length; index++)
        {
            int matchResult = matchPaths(aPath, iRestrictedPathList[index]);

            if ((matchResult != PATH_BELOWIN_HIERARCHY)
                    && (matchResult != PATHS_NO_MATCH))
            {
                if ((!aIntent.equals(INTENT_READ)) && (!aIsOpening))
                {
                    // Anything other than read operation on par or above
                    // restricted path hierarchy is not allowed when not opening
                    return false;
                }
            }
        }

        if (partialMatchWithRestrictedPaths(aPath))
        {
            return false;
        }

        return true;
    }

    private static boolean partialMatchWithRestrictedPaths(String aPath)
    {
        String path1 = aPath;
        boolean initialNoMatch = true;

        // Partial match is only when path is not a substring initially,
        // but when stripped, becomes a substring of one of the restricted paths
        for (int index = 0; index < iRestrictedPathList.length; index++)
        {
            int matchResult = matchPaths(aPath, iRestrictedPathList[index]);
            if (matchResult == PATH_BELOWIN_HIERARCHY
                    || matchResult == PATH_ABOVEIN_HIERARCHY
                    || matchResult == PATHS_EQUAL)
            {
                return false;
            }
        }

        if (path1.length() > 3)
        {
            path1 = path1.substring(0, path1.lastIndexOf('/'));
        }

        // path1 is stripped to know in case the file is being created inside
        // root.
        while (path1.length() > 3)
        {
            for (int index = 0; index < iRestrictedPathList.length; index++)
            {
                if (iRestrictedPathList[index].toLowerCase().startsWith(
                            path1.toLowerCase()))
                {
                    return true;
                }
            }
            path1 = path1.substring(0, path1.lastIndexOf('/'));
        }

        // C:/data/somefile should be matched with c:/data/images,c:/data/videos
        // and must return true but c:/data or c:/ must not return as true
        return false;
    }

    /**
     * Resolves a path to a one of the following categories:
     *
     * <pre>
     * PUBLIC_DIRS - C:/Data/Images
     *               C:/Data/Videos
     *               C:/Data/Graphics
     *               C:/Data/Sounds
     *               C:/Data/Music
     *               C:/Data/Recordings and all files therein
     * HOME_DIR - App's private directory
     * PRIVATE_USER_FILES - All files and directories higher in path hierarchy
     *                      of PUBLIC_DIRS
     * SYSTEM_FILES - Z drive
     * </pre>
     *
     * @param aPath
     *            path that has to be mapped to a particular category.
     * @return category of the path specified.<br/> One of the following:
     *         SYSTEM_FILES, PRIVATE_USER_FILES, PUBLIC_DIRS, HOME_DIR
     */
    public static String getCategory(String aPath)
    {
        FileLogger.Log("+ FileAccessHelper: getCategory: " + aPath);
        // SYSTEM_FILES, PRIVATE_USER_FILES, PUBLIC_DIRS, HOME_DIR
        if (aPath.equals(SYSTEM_FILES) || aPath.equals(PRIVATE_USER_FILES)
                || aPath.equals(PUBLIC_DIRS) || aPath.equals(HOME_DIR)
                || aPath.equals(RESTRICTED_PUBLIC_FILES))
        {
            // if it is already mapped
            FileLogger.Log("- FileAccessHelper: getCategory: returning: "
                           + aPath);
            return aPath;
        }

        if (aPath.equals(""))
        {
            // Used in case of FileSystemRegistry
            return PUBLIC_DIRS;
        }

        // First check for Home directory. Restricted paths list contains
        // app's private directory too.
        if (isHomeDir(aPath))
        {
            return HOME_DIR;
        }

        int matchResult = PATHS_NO_MATCH;
        // Paths below restricted paths in hierarchy are part of Public files
        // Paths above in hierarchy are part of private used files.
        for (int index = 0; index < iRestrictedPathList.length; index++)
        {
            matchResult = matchPaths(aPath, iRestrictedPathList[index]);
            if (PATH_BELOWIN_HIERARCHY == matchResult)
            {
                FileLogger.Log("- FileAccessHelper: getCategory: returning: "
                               + PUBLIC_DIRS);
                return PUBLIC_DIRS;
            }

            if (PATHS_EQUAL == matchResult)
            {
                return PUBLIC_DIRS;
            }

            // Do we need this at all? Restricted PUBLIC Files can be removed
            if (PATH_ABOVEIN_HIERARCHY == matchResult)
            {
                FileLogger.Log("- FileAccessHelper: getCategory: returning: "
                               + PUBLIC_DIRS);
                return PUBLIC_DIRS;
            }
        }

        String rom = FileSystemUtils.getRomDrive().toLowerCase()
                     .substring(0, 2);
        String temp = FileSystemUtils.getTemporaryDrive().toLowerCase()
                      .substring(0, 2);

        if (aPath.toLowerCase().startsWith(rom)
                || aPath.toLowerCase().startsWith(temp))
        {
            FileLogger.Log("- FileAccessHelper: getCategory: returning: "
                           + SYSTEM_FILES);
            return SYSTEM_FILES;

        }
        else if (aPath.toLowerCase().startsWith(
                     FileSystemUtils.getDefaultRoot().toLowerCase()))
        {
            FileLogger.Log("- FileAccessHelper: getCategory: returning: "
                           + RESTRICTED_PUBLIC_FILES);
            // It is however known that the default root of the device can
            // change.
            return RESTRICTED_PUBLIC_FILES;
        }
        else if ((aPath.toLowerCase().indexOf(PATH_PRIVATE) == 3)
                 || (aPath.toLowerCase().indexOf(PATH_SYSTEM) == 3))
        {
            FileLogger.Log("- FileAccessHelper: getCategory: returning: "
                           + SYSTEM_FILES);
            return SYSTEM_FILES;
        }

        FileLogger.Log("- FileAccessHelper: getCategory: returning: "
                       + PUBLIC_DIRS);
        return PUBLIC_DIRS;
    }

    /**
     * Checks to see if the specified path is same as application's private
     * directory.
     */
    private static boolean isHomeDir(String aPath)
    {
        String appPrivateDir = FileSystemUtils.getAppPrivateDir();
        if (aPath.equalsIgnoreCase(appPrivateDir))
        {
            return true;
        }
        if (aPath.startsWith(appPrivateDir))
        {
            return true;
        }

        return false;
    }

    /**
     * Checks to see if a file/directory can be created within the specidied
     * path.
     *
     * @param aPath
     *            directory within which the application intends to create a
     *            file.
     * @param aDomain
     *            domain of the application
     * @return true in case access is allowed, false otherwise.
     */
    public static boolean isCreateAllowedWithinDir(String aPath, String aDomain)
    {
        if (aDomain.equals(ApplicationInfo.MANUFACTURER_DOMAIN))
        {
            return true;
        }

        boolean allowed = false;

        if (!aPath.startsWith(FileSystemUtils.getDefaultRoot()))
        {
            return true;
        }

        for (int index = 0; index < iRestrictedPathList.length; index++)
        {
            String path = iRestrictedPathList[index];
            int matchResult = matchPaths(aPath, path);
            // Domains other than manufacturer are allowed to create content
            // only within restricted directories.
            if ((PATHS_EQUAL == matchResult)
                    || (PATH_BELOWIN_HIERARCHY == matchResult))
            {
                allowed = true;
                break;
            }
        }
        return allowed;
    }

    /**
     * Tries to match paths. Returns how "path2" is related to "path1". Checks if
     * the path is above or below in path hierarchy. Also checks to see if paths
     * are same or are totally different.
     */
    private static int matchPaths(String aPath1, String aPath2)
    {
        // Strip trailing slash in case its present.
        String path1 = aPath1.endsWith("/") ? aPath1.substring(0, aPath1
                       .length() - 1) : aPath1;

        String path2 = aPath2.endsWith("/") ? aPath2.substring(0, aPath2
                       .length() - 1) : aPath2;

        // In case both paths are the same.
        if (path1.equalsIgnoreCase(path2))
        {
            return PATHS_EQUAL;
        }

        // Check if path1 is higher in path hierarchy
        if (path2.toLowerCase().startsWith(path1.toLowerCase()))
        {
            return PATH_ABOVEIN_HIERARCHY;
        }

        if (path1.toLowerCase().startsWith(path2.toLowerCase()))
        {
            return PATH_BELOWIN_HIERARCHY;
        }

        return PATHS_NO_MATCH;
    }
}
