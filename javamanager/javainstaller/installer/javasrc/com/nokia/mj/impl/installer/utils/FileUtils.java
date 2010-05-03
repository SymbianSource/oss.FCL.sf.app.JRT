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


package com.nokia.mj.impl.installer.utils;

import com.nokia.mj.impl.fileutils.FileUtility;
import com.nokia.mj.impl.installer.utils.Log;
import com.nokia.mj.impl.utils.Uid;

import java.io.FileInputStream;
import java.io.InputStream;
import java.io.IOException;
import java.io.OutputStream;
import java.util.Vector;

public class FileUtils
{

    // Begin methods which use com.nokia.mj.impl.fileutils.FileUtility

    /**
     * Checks file or directory existence.
     * Returns true if file or directory with given name exists, false otherwise.
     */
    public static boolean exists(String aPathname)
    {
        return new FileUtility(aPathname).exists();
    }

    /**
     * Returns the name of the file or directory denoted by given pathname.
     * This is just the last name in the pathname's name sequence. If the
     * pathname's name sequence is empty, then the empty string is returned.
     */
    public static String getName(String aPathname)
    {
        return new FileUtility(aPathname).getName();
    }

    /**
     * Returns InputStream for given file. Caller must close
     * the stream after it is no longer used.
     */
    public static InputStream getInputStream(String aFilename)
    throws IOException
    {
        // Use InputStream from FileUtility after
        // OpenC fixes are available in every firmware.
        //return new FileUtility(aFilename).openInputStream();
        return new FileInputStream(aFilename);
    }

    /**
     * Returns OutputStream for given file. Caller must close
     * the stream after it is no longer used.
     */
    public static OutputStream getOutputStream(String aFilename)
    throws IOException
    {
        return getOutputStream(aFilename, false);
    }

    /**
     * Returns OutputStream for given file. Caller must close
     * the stream after it is no longer used.
     */
    public static OutputStream getOutputStream(String aFilename, boolean aAppend)
    throws IOException
    {
        OutputStream os = null;
        FileUtility file = new FileUtility(aFilename);
        if (file.exists())
        {
            if (aAppend)
            {
                os = file.openOutputStream(file.fileSize());
            }
            else
            {
                os = file.openOutputStream();
            }
        }
        else
        {
            boolean result = file.createNewFile();
            if (result)
            {
                os = file.openOutputStream();
            }
            else
            {
                throw new IOException("Creating file " + aFilename + " failed");
            }
        }
        return os;
    }

    /**
     * Returns the size of the file denoted by given pathname.
     * If the file does not exist or pathname points to directory
     * this method returns 0.
     */
    public static long getSize(String aPathname)
    {
        long result = 0;
        FileUtility file = new FileUtility(aPathname);
        if (file.isFile())
        {
            try
            {
                result = file.fileSize();
            }
            catch (IOException ioe)
            {
                Log.logError("FileUtils.getSize failed for " + aPathname, ioe);
            }
        }
        return result;
    }

    /**
     * Returns the content size of the file denoted by given pathname.
     * Content size can be different than file size for DRM protected files.
     * If the file does not exist or pathname points to directory
     * this method returns 0.
     */
    public static long getContentSize(String aPathname)
    {
        long size = 0;
        FileUtility file = new FileUtility(aPathname);
        if (file.isFile())
        {
            InputStream fis = null;
            try
            {
                // Use InputStream from FileUtility after it
                // starts to support available() and skip() for
                // DRM encrypted files.
                //fis = file.openInputStream();
                fis = new FileInputStream(aPathname);
            }
            catch (IOException ioe)
            {
                Log.logError
                ("FileUtils.getContentSize: error while opening file " +
                 aPathname, ioe);
                return size;
            }
            int available = 0;
            try
            {
                do
                {
                    available = fis.available();
                    size += available;
                    fis.skip(available);
                }
                while (available > 0);
            }
            catch (IOException ioe)
            {
                Log.logError
                ("FileUtils.getContentSize: error while getting content from " +
                 aPathname, ioe);
            }
            try
            {
                if (fis != null)
                {
                    fis.close();
                    fis = null;
                }
            }
            catch (IOException ioe)
            {
                Log.logError
                ("FileUtils.getContentSize: error while closing file " +
                 aPathname, ioe);
            }
        }
        Log.log("FileUtils.getContentSize returns " + size);
        return size;
    }

    /**
     * Returns true if given file is DRM protected, false otherwise.
     */
    public static boolean isDrmProtected(String aPath)
    {
        return (FileUtility.getContentId(aPath) != null);
    }

    // End methods which use com.nokia.mj.impl.fileutils.FileUtility

    /**
     * Returns the system dependent path separator char as a string.
     */
    public static String pathSeparator()
    {
        return System.getProperty("file.separator");
    }

    /**
     * Returns the system dependent path separator char.
     */
    public static char pathSeparatorChar()
    {
        return pathSeparator().charAt(0);
    }

    /**
     * Returns the parent of the specified path.
     * Parent is part of the path where the last
     * path item has been left out.
     */
    public static String getParent(String aPath)
    {
        if (aPath == null || aPath.length() == 0)
        {
            return null;
        }
        char pathSep = pathSeparatorChar();
        String path = aPath;
        int i = path.lastIndexOf(pathSep);
        if (i == path.length()-1)
        {
            // Remove trailing path separator.
            path = path.substring(0, path.length()-1);
            i = path.lastIndexOf(pathSep);
        }
        if (i != -1)
        {
            path = path.substring(0, i+1);
        }
        return path;
    }

    /**
     * Returns the file basename, that is the part of
     * the filename without extension. If there is no
     * extension, then returns the filename itself.
     */
    public static String getBasename(String aPath)
    {
        String result = getName(aPath);
        int index = result.lastIndexOf('.');
        if (index != -1)
        {
            result = result.substring(0, index);
        }
        return result;
    }

    /**
     * Returns filename extension. If no extension is found,
     * returns empty string.
     */
    public static String getExtension(String aFilename)
    {
        String result = "";
        String filename = getName(aFilename);
        int index = filename.lastIndexOf('.');
        if (index != -1)
        {
            result = filename.substring(index);
        }
        return result;
    }

    /**
     * Returns drive for given filename, or -1 if drive id is not found.
     */
    public static int getDrive(String aFilename)
    {
        int drive = -1;
        if (aFilename != null && aFilename.length() >= 2)
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
     * Returns string name for given drive.
     */
    public static String getDriveName(int aDrive)
    {
        StringBuffer driveName = new StringBuffer();
        if (Platform.isS60())
        {
            driveName.append((char)('A' + aDrive));
        }
        return driveName.toString();
    }

    /**
     * Returns given filename from given drive, or unmodified filename
     * if drive id is -1.
     */
    public static String setDrive(String aFilename, int aDrive)
    {
        if (aFilename != null && aDrive >= 0)
        {
            if (aFilename.charAt(1) == ':')
            {
                aFilename = (char)('a' + aDrive) + aFilename.substring(1);
            }
        }
        return aFilename;
    }

    /**
     * Returns true if given filenames are located on the same drive,
     * false otherwise.
     */
    public static boolean isSameDrive(String aFilename1, String aFilename2)
    {
        if (getDrive(aFilename1) == getDrive(aFilename2))
        {
            return true;
        }
        return false;
    }

    /**
     * Returns true if given path points to a file in Inbox.
     */
    public static boolean isInboxFile(String aPath)
    {
        if (!Platform.isS60())
        {
            return false;
        }
        boolean result = false;
        if (aPath != null && aPath.length() > 3 &&
                aPath.substring(3).toLowerCase().startsWith("private\\1000484b"))
        {
            result = true;
        }
        return result;
    }

    /**
     * Returns root folder for applications.
     */
    private static String iAppsRoot = null;
    public static String getAppsRoot()
    {
        if (iAppsRoot == null)
        {
            iAppsRoot = FileRoots.getAppsRoot();
        }
        return iAppsRoot;
    }

    /**
     * Sets root folder for applications to be in specified drive.
     */
    public static void setAppsRoot(int aDrive)
    {
        iAppsRoot = setDrive(getAppsRoot(), aDrive);
    }

    /**
     * Returns root folder for installer.
     */
    private static String iInstallerRoot = null;
    public static String getInstallerRoot()
    {
        if (iInstallerRoot == null)
        {
            iInstallerRoot = FileRoots.getInstallerRoot();
        }
        return iInstallerRoot;
    }

    /**
     * Returns root folder for IntegrityService.
     */
    public static String getIntegrityServiceRoot()
    {
        return FileRoots.getInstallerRoot() + "is" + pathSeparator();
    }

    /**
     * Returns path for application with given uid. Path is relative
     * to apps root returned from getAppsRoot().
     */
    public static String getAppRootPath(Uid aUid)
    {
        return aUid.getStringValue() + pathSeparator();
    }

    /**
     * Trims prefixing slash from given jar entry name.
     * This is needed because JarEntry class does not
     * find jar entries which begin with slash.
     */
    public static String trimJarEntry(String aName)
    {
        if (aName.startsWith("/"))
        {
            return aName.substring(1);
        }
        return aName;
    }
}
