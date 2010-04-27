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


package com.nokia.mj.impl.fileutils;

import java.io.IOException;

/**
 * FilePlatformSpecifics priovides services that are platform specific.
 * <p>
 * There are many cases where behaviours are entirely different for different
 * platforms. For example, hiding a file is just rename on Linux and in
 * Windows/S60 it is all about setting an attribute. These behaviours can often
 * need totally different implementations.
 */
final class FilePlatformSpecifics
{
    /**
     * Resolves the real target given a pathname without drive using the agreed
     * eclipsing rules. Currently for OMJ, we have agreed for C: and Z:
     *
     * @param path
     *            contains path without drive letter
     * @return resolved absolute path with drive letter; null if not found
     */
    public static String resolveDrive(String aPath)
    {
        String path = aPath;
        String separator = System.getProperty("file.separator");

        path = path.replace('/', '\\');
        if (!path.startsWith(separator))
        {
            return null;
        }

        FileUtility resolved = new FileUtility("C:" + path);
        if (resolved.exists())
        {
            return "C:" + path;
        }
        else
        {
            resolved = new FileUtility("Z:" + path);
            if (resolved.exists())
            {
                return "Z:" + path;
            }
            else
            {
                return null;
            }
        }
    }

    /**
     * Copies a file from one location to another.
     *
     * @param aSourceFile
     *            absolute path name of the source file.
     * @param aDestFile
     *            absolute path name of the destication file.
     * @param aOverwrite
     *            if the destFile already exists, this option allows to
     *            overwrite its contents
     * @return true in case copy was successful, false otherwise.
     */
    public static boolean copy(String aSourceFile, String aDestFile,
                               boolean aOverwrite)
    {
        FileUtility source = new FileUtility(aSourceFile);
        //FileUtility destination = new FileUtility(destFile);

        if (!source.isFile())
        {
            throw new IllegalArgumentException("Source not a file");
        }

        if (aDestFile.endsWith("/") || aDestFile.endsWith("\\"))
        {
            throw new IllegalArgumentException("Invalid destination path");
        }

        try
        {
            return _copy(aSourceFile, aDestFile, aOverwrite);
        }
        catch (IOException ex)
        {
            return false;
        }
    }

    private static native boolean _copy(String sourceFile, String destFile,
                                        boolean overwrite) throws IOException;

    /**
     * Moves a file from one location to another.
     *
     * @param aSourceFile
     *            absolute path name of the source file.
     * @param aDestFile
     *            absolute path name of the destication file.
     * @param aOverwrite
     *            if the destFile already exists, this option allows to
     *            overwrite its contents
     * @return true in case copy was successful, false otherwise.
     */
    public static boolean move(String aSourceFile, String aDestFile,
                               boolean aOverwrite)
    {
        try
        {
            return _move(aSourceFile, aDestFile, aOverwrite);
        }
        catch (IOException ex)
        {
            return false;
        }
    }

    private static native boolean _move(String sourceFile, String destFile,
                                        boolean overwrite) throws IOException;

    /**
     * Copies a file from one location to another.
     *
     * @param sourceFile
     *            absolute path name of the source file.
     * @param destFile
     *            absolute path name of the destication file.
     * @param aOverwrite
     *            if the destFile already exists, this option allows to
     *            overwrite its contents
     * @return true in case copy was successful, false otherwise.
     */
    public static boolean copyAll(String aSourceFile, String aDestFile,
                                  boolean aOverwrite)
    {
        String sourceFile = aSourceFile;
        String destFile = aDestFile;

        FileUtility source = new FileUtility(sourceFile);
        FileUtility destination = new FileUtility(destFile);

        if (source.isDirectory())
        {
            if (sourceFile.indexOf("\\")!=-1)
            {
                if (!sourceFile.endsWith("\\"))
                {
                    sourceFile += "\\";
                }
            }
            else if (!sourceFile.endsWith("/"))
            {
                sourceFile += "/";
            }
        }
        else
        {
            throw new IllegalArgumentException("Source is not Directory");
        }

        if (destination.exists())
        {
            if (!destination.isDirectory())
            {
                throw new IllegalArgumentException("Destination is not Directory");
            }
        }
        else
        {
            try
            {
                destination.mkdir();
            }
            catch (Exception e)
            {
                return false;
            }
        }

        if (destFile.indexOf("\\")!=-1)
        {
            if (!destFile.endsWith("\\"))
            {
                destFile += "\\";
            }
        }
        else if (!destFile.endsWith("/"))
        {
            destFile += "/";
        }

        try
        {
            return _copyAll(sourceFile, destFile, aOverwrite);
        }
        catch (IOException ex)
        {
            return false;
        }
    }

    private static native boolean _copyAll(String sourceFile, String destFile,
                                           boolean overwrite) throws IOException;

    /**
     * Deletes contents of the entire directory alonw with the directory.
     *
     * @param aTargetFile
     *            absolute path name of the directory to be deleted.
     * @return true in case delete was successful, false otherwise.
     */
    public static boolean deleteAll(String aTargetFile)
    {
        String target = aTargetFile;
        if ((!aTargetFile.endsWith("/")) && (!aTargetFile.endsWith("\\")))
        {
            if (target.indexOf('\\') != -1)
            {
                // Implies that the path contained is already symbian specific
                target = aTargetFile + "\\";
            }
            else
            {
                // If this is not done, Symbian deletes the source directory as
                // well.
                target = aTargetFile + "/";
            }
        }

        try
        {
            return _deleteAll(target);
        }
        catch (IOException ex)
        {
            return false;
        }
    }

    private static native boolean _deleteAll(String targetFile)
    throws IOException;

    /**
     * Sets the hidden attribute of the selected file to the value provided. The
     * attribute is applied to the file on the actual file system immediately
     * upon invocation of this method if the file system and platform support
     * it.
     * <p>
     * If the file system doesn't support a hidden attribute, this method is
     * ignored and isHidden() always returns false. Since the exact definition
     * of hidden is system-dependent, this method only works on file systems
     * that support a settable file attribute.
     * <p>
     * For example, on Win32 and FAT file systems, a file may be considered
     * hidden if it has been marked as such in the file's attributes; therefore
     * this method is applicable.<br/> However on UNIX systems a file may be
     * considered to be hidden if its name begins with a period character ('.').
     * In the UNIX case, this method may be ignored and the method to make a
     * file hidden may be the rename() method.
     *
     * @param aHidden
     *            The new state of the hidden flag of the selected file.
     * @throws IOException
     *             if the connection's target does not exist or is not
     *             accessible.
     */
    public static void setHidden(FileUtility aCaller, boolean aHidden)
    throws IOException
    {
        _setHidden(aCaller.getAbsolutePath(), aHidden);
    }

    // IN S60, there is a hidden attribute, we set this value.
    private static native void _setHidden(String fileName, boolean hidden)
    throws IOException;

    /**
     * Get file Content ID. Only DRM protected files have content Id.
     *
     * @param path to file where id is read.
     * @return Content ID. Null if cannot be read or does not exists.
     */
    public static String getContentId(String aFilePath)
    {
        return _getContentId(aFilePath);
    }

    private static native String _getContentId(String filePath);

}