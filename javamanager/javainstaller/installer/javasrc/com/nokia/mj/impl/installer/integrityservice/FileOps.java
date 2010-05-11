/*
* Copyright (c) 2008-2010 Nokia Corporation and/or its subsidiary(-ies).
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


package com.nokia.mj.impl.installer.integrityservice;

import com.nokia.mj.impl.fileutils.FileOperations;
import com.nokia.mj.impl.fileutils.FileUtility;
import com.nokia.mj.impl.installer.utils.FileUtils;
import com.nokia.mj.impl.installer.utils.Log;

import java.io.IOException;

/**
 * FileOps class offers file operation services for IntegrityService.
 */
class FileOps
{

    // Begin methods which use com.nokia.mj.impl.fileutils.FileUtility

    static boolean copy(String aFrom, String aTo)
    {
        //Log.log("FileOps.copy: " + aFrom + " -> " + aTo);
        boolean result = false;
        try
        {
            result = FileOperations.copy(aFrom, aTo, true);
            if (!result)
            {
                Log.logError("FileOps.copy failed: " + aFrom + " -> " + aTo);
            }
        }
        catch (Exception ex)
        {
            result = false;
            Log.logError("FileOps.copy exception: " + aFrom + " -> " + aTo, ex);
        }
        return result;
    }

    static boolean copyAll(String aFrom, String aTo)
    {
        //Log.log("FileOps.copyAll: " + aFrom + " -> " + aTo);
        boolean result = false;
        try
        {
            FileUtility fromFile = new FileUtility(aFrom);
            if (fromFile.isDirectory())
            {
                result = FileOperations.copyAll(aFrom, aTo, true);
            }
            else
            {
                result = FileOperations.copy(aFrom, aTo, true);
            }
            if (!result)
            {
                Log.logError("FileOps.copyAll failed: " + aFrom + " -> " + aTo);
            }
        }
        catch (Exception ex)
        {
            result = false;
            Log.logError("FileOps.copyAll exception: " + aFrom + " -> " + aTo, ex);
        }
        return result;
    }

    static boolean move(String aFrom, String aTo)
    {
        //Log.log("FileOps.move: " + aFrom + " -> " + aTo);
        boolean result = false;
        FileUtility fromFile = new FileUtility(aFrom);
        try
        {
            result = fromFile.renameTo(aTo);
        }
        catch (Exception ex)
        {
            result = false;
            Log.logWarning("FileOps.move: renameTo failed: " + aFrom + " -> " + aTo, ex);
        }
        if (!result)
        {
            result = true;
            if (fromFile.isDirectory())
            {
                result = copyAll(aFrom, aTo);
                if (result)
                {
                    result = deleteAll(aFrom);
                }
            }
            else
            {
                result = copy(aFrom, aTo);
                if (result)
                {
                    result = fromFile.forceDelete();
                    if (!result)
                    {
                        Log.logError("FileOps.move: delete failed: " + aFrom);
                    }
                }
            }
        }
        if (!result)
        {
            Log.logError("FileOps.move failed: " + aFrom + " -> " + aTo);
        }
        return result;
    }

    static boolean deleteAll(String aFile)
    {
        //Log.log("FileOps.deleteAll: " + aFile);
        boolean result = false;
        try
        {
            FileUtility file = new FileUtility(aFile);
            if (!file.exists())
            {
                // File does not exist, there is nothing to delete.
                return true;
            }
            if (file.isDirectory())
            {
                result = FileOperations.deleteAll(aFile);
            }
            else
            {
                result = file.forceDelete();
            }
            if (!result)
            {
                Log.logError("FileOps.deleteAll failed: " + aFile);
            }
        }
        catch (Exception ex)
        {
            result = false;
            Log.logError("FileOps.deleteAll exception: " + aFile, ex);
        }
        return result;
    }

    static boolean mkdirs(String aPath)
    {
        boolean result = true;
        FileUtility dir = new FileUtility(aPath);
        if (!dir.exists())
        {
            try
            {
                result = dir.mkdirs();
                if (!result)
                {
                    Log.logError("FileOps.mkdirs failed: " + aPath);
                }
            }
            catch (IOException ioe)
            {
                result = false;
                Log.logError("FileOps.mkdirs failed: " + aPath, ioe);
            }
        }
        return result;
    }

    static String getCanonicalPath(String aFilename) throws IOException
    {
        return FileUtility.getCanonicalPath(aFilename);
    }

    // End methods which use com.nokia.mj.impl.fileutils.FileUtility

    /**
     * Returns drive for given filename, or -1 if drive letter is not found.
     */
    static int getDrive(String aFilename)
    {
        int drive = -1;
        if (aFilename != null)
        {
            if (aFilename.charAt(1) == ':')
            {
                char c = Character.toLowerCase(aFilename.charAt(0));
                drive = c - 'a';
            }
        }
        return drive;
    }

    /**
     * Returns given path from given drive. If drive is -1 returns
     * path itself.
     */
    static String getPathFromDrive(int aDrive, String aPath)
    throws IOException
    {
        if (aDrive == -1)
        {
            return aPath;
        }
        String newPath = getCanonicalPath(aPath);
        int oldDrive = getDrive(aPath);
        if (oldDrive != -1 && oldDrive != aDrive)
        {
            char newDrive = (char)(aDrive + 'a');
            newPath = String.valueOf(newDrive) + aPath.substring(1);
        }
        else
        {
            newPath = aPath;
        }
        return newPath;
    }
}
