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
import java.io.InputStream;
import java.io.OutputStream;
import com.nokia.mj.impl.utils.Logger;

/**
 * FileURL manages URL/Path handling. It is responsible to manage paths which
 * have path separators specific to the platform. It will also manage
 * differences between URL and Absolute path and make it transparent to use
 * from.
 */
final class FilePlatformSpecifics
{
    /**
     * Resolves the real target given a pathname without drive using the agreed
     * eclipsing rules. Currently for OMJ, we have agreed for C: and Z:
     *
     * @param aPath
     *            contains path without drive letter
     * @return resolved absolute path with drive letter; null if not found
     */
    public static String resolveDrive(String aPath)
    {
        return null;
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
                               boolean aOverwrite) throws IOException
    {
        FileUtility aFrom = new FileUtility(aSourceFile);
        FileUtility aTo = new FileUtility(aDestFile);

        if (!aFrom.exists())
        {
            throw new IOException("Cannot copy, file does not exist: " + aFrom);
        }

        FileUtility toParent = aTo.getParentFile();
        if (!toParent.exists())
        {
            // Directory for target file does not exist, let's create it.
            toParent.mkdirs();
        }

        if (!aTo.exists())
        {
            aTo.createNewFile();
        }
        else if (!aOverwrite)
        {
            throw new IOException("Target already present");
        }

        InputStream fis = aFrom.openInputStream();
        OutputStream fos = aTo.openOutputStream();

        try
        {
            byte[] buf = new byte[16384];
            int i = 0;
            while ((i = fis.read(buf)) != -1)
            {
                fos.write(buf, 0, i);
            }
        }
        catch (IOException ioe)
        {
            throw ioe;
        }
        finally
        {
            if (fis != null)
            {
                fis.close();
            }
            if (fos != null)
            {
                fos.close();
            }
        }

        return true;
    }

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
                               boolean aOverwrite) throws IOException
    {
        FileUtility aFrom = new FileUtility(aSourceFile);
        FileUtility aTo = new FileUtility(aDestFile);

        boolean result = aFrom.rename(aTo.getName());
        if (!result)
        {
            result = true;
            if (aFrom.isDirectory())
            {
                result = copyAll(aFrom.getAbsolutePath(),
                                 aTo.getAbsolutePath(), aOverwrite);
                if (result)
                {
                    result = deleteAll(aFrom.getAbsolutePath());
                }
            }
            else
            {
                try
                {
                    copy(aFrom.getAbsolutePath(), aTo.getAbsolutePath(),
                         aOverwrite);
                }
                catch (IOException ioe)
                {
                    result = false;
                    Logger.ELOG(Logger.EJavaFile, "FileOps.move: copy failed: "
                                + ioe);
                }
                if (result)
                {
                    result = aFrom.delete();
                    if (!result)
                    {
                        Logger.ELOG(Logger.EJavaFile,
                                    "FileOps.move: delete failed: [" + aFrom + "]");
                    }
                }
            }
        }
        if (!result)
        {
            Logger.ELOG(Logger.EJavaFile, "FileOps.move failed: [" + aFrom
                        + "], [" + aTo + "]");
        }
        return result;
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
    public static boolean copyAll(String aSourceFile, String aDestFile,
                                  boolean aOverwrite) throws IOException
    {
        boolean result = true;
        FileUtility aFrom = new FileUtility(aSourceFile);
        FileUtility aTo = new FileUtility(aDestFile);

        if (aFrom == null || !aFrom.exists() || aTo == null)
        {
            result = false;
        }

        if (result)
        {
            if (aFrom.isDirectory())
            {
//                if (aTo.exists())
//                {
//                    // Target dir exists, remove it before copying.
//                    result = deleteAll(aTo.getAbsolutePath());
//                }

                if (result)
                {
                    result = aTo.mkdirs();
                    if (!result)
                    {
                        Logger.ELOG(Logger.EJavaFile,
                                    "FileOps.copyAll: mkdirs failed: "
                                    + aTo.toString());
                    }
                }
                else
                {
                    Logger.ELOG(Logger.EJavaFile,
                                "FileOps.copyAll: delete target dir failed: "
                                + aTo.toString());
                }
                FileUtility[] files = aFrom.listFiles();

                if (files != null)
                {
                    for (int i = 0; i < files.length && result; i++)
                    {

                        String newTo = aTo.getAbsolutePath() + "/"
                                       + files[i].getName();
                        result = copyAll(files[i].getAbsolutePath(), newTo,
                                         aOverwrite);
                    }
                }
            }
            else
            {
                try
                {
                    copy(aFrom.getAbsolutePath(), aTo.getAbsolutePath(),
                         aOverwrite);
                }
                catch (IOException ioe)
                {
                    result = false;
                    Logger.ELOG(Logger.EJavaFile,
                                "FileOps.copyAll: copy failed: " + ioe);
                }
            }
        }
        if (!result)
        {
            Logger.ELOG(Logger.EJavaFile, "FileOps.copyAll failed: [" + aFrom
                        + "], [" + aTo + "]");
        }
        return result;
    }

    /**
     * Deletes contents of the entire directory alonw with the directory.
     *
     * @param aTargetFile
     *            absolute path name of the directory to be deleted.
     * @return true in case delete was successful, false otherwise.
     */
    public static boolean deleteAll(String aTargetFile) throws IOException
    {
        boolean result = true;
        FileUtility aFile = new FileUtility(aTargetFile);
        if (aFile == null)
        {
            result = false;
        }

        if (result && aFile.isDirectory())
        {
            FileUtility[] files = aFile.listFiles();
            if (files != null)
            {
                for (int i = 0; i < files.length && result; i++)
                {
                    result = deleteAll(files[i].getAbsolutePath());
                }
            }
        }

        if (result && aFile.exists())
        {
            result = aFile.delete();
        }

        if (!result)
        {
            Logger.ELOG(Logger.EJavaFile, "FileOps.deleteAll failed: [" + aFile
                        + "]");
        }
        return result;
    }

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
        String path = aCaller.getPath();
        String name = aCaller.getName();

        String newName = "." + name;
        aCaller.rename(newName);
    }

    // IN S60, there is a hidden attribute, we set this value.
    // Whereas on Linux, it is rename to .<something> so we do not use the
    // native call
    // private static native void _setHidden(String fileName, boolean hidden) throws IOException;

    /**
     * Get file Content ID.
     *
     * @param path to file where id is read.
     * @return Content ID. Null if cannot be read or does not exists.
     */
    public static String getContentId(String aFilePath)
    {
        // Not currently supported on Linux platform.
        return null;
    }
}
