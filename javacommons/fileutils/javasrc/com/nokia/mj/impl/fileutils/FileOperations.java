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
 * Provides facility to perform File operations such as Copy, Move etc that are
 * performed on directory trees.
 * <p>
 * It is important to note that opening file streams and copying is not
 * acceptable as they have an impact on DRM rights of the file. So, we need to
 * use the capabilities provided by the platform.
 */
public final class FileOperations
{
    /**
     * Copies a file from one location to another.
     *
     * @param aSourceFile
     *            absolute path name of the source file.
     * @param aDestFile
     *            absolute path name of the destication file.
     * @param aOverWrite
     *            specifies if the destination files are to be overwitten in
     *            case they alreay exist
     * @throws IllegalArgumentException
     *             in case the paths specified are not valid
     * @return true in case copy was successful, false otherwise.
     */
    public static boolean copy(String aSourceFile, String aDestFile,
                               boolean aOverWrite) throws IOException
    {
        FileUtility source = new FileUtility(aSourceFile);
        if (!source.exists())
        {
            throw new IOException(FileUtilityErrorStrings.SOURCE_NOT_PRESENT
                                  + ": " + source);
        }
        else if (!source.isFile())
        {
            throw new IOException(FileUtilityErrorStrings.SOURCE_NOT_FILE
                                  + ": " + source);
        }

        FileUtility destination = new FileUtility(aDestFile);
        if (destination.exists())
        {
            if (destination.isDirectory())
            {
                // Cannot copy source file to be a directory.
                throw new IOException(
                    FileUtilityErrorStrings.DESTINATION_IS_DIRECTORY + ": "
                    + destination);
            }
        }
        try
        {
            return FilePlatformSpecifics.copy(aSourceFile, aDestFile,
                                              aOverWrite);
        }
        catch (Exception e)
        {
            return false;
        }
    }

    /**
     * Moves a file from source to destination.
     *
     * @param aSourceFile
     *            absolute path name of the source file.
     * @param aDestFile
     *            absolute path name of the destication file.
     * @param aOverWrite
     *            specifies if the destination files are to be overwitten in
     *            case they alreay exist
     * @return true in case move was successful, false otherwise.
     */
    public static boolean move(String aSourceFile, String aDestFile,
                               boolean aOverWrite) throws IOException
    {
        FileUtility source = new FileUtility(aSourceFile);
        FileUtility destination = new FileUtility(aDestFile);

        if (!source.exists())
        {
            throw new IOException(FileUtilityErrorStrings.SOURCE_NOT_PRESENT
                                  + ": " + source);
        }

        if (destination.exists())
        {
            if (destination.isDirectory())
            {
                // Cannot move file to be a directory.
                throw new IOException(
                    FileUtilityErrorStrings.DESTINATION_IS_DIRECTORY + ": "
                    + source);
            }

            if (!aOverWrite)
            {
                throw new IOException(
                    FileUtilityErrorStrings.DESTINATION_ALREADY_PRESENT
                    + ": " + source);
            }
        }
        return FilePlatformSpecifics.move(aSourceFile, aDestFile, aOverWrite);
    }

    /**
     * Copies the contents of a directory to another directory. Copy is
     * recursive and replicates the entire source tree in the destination.
     *
     * @param aSourceDir
     *            absolute path of the name of the directory to be copied
     * @param aDestDir
     *            absolute path of the name of the directory to be copied into
     * @param aOverWrite
     *            specifies if the destination files are to be overwitten in
     *            case they alreay exist
     * @return true in case copy was successful, false otherwise
     */
    public static boolean copyAll(String aSourceDir, String aDestDir,
                                  boolean aOverWrite) throws IOException
    {
        FileUtility source = new FileUtility(aSourceDir);
        FileUtility destination = new FileUtility(aDestDir);

        if (!source.exists())
        {
            throw new IOException(FileUtilityErrorStrings.SOURCE_NOT_PRESENT
                                  + ": " + source);
        }

        if (!source.isDirectory())
        {
            throw new IOException(FileUtilityErrorStrings.SOURCE_NOT_DIRECTORY
                                  + ": " + source);
        }

        if (!aOverWrite)
        {
            if (destination.exists())
            {
                return false;
            }
        }
        try
        {
            boolean retVal = FilePlatformSpecifics.copyAll(aSourceDir,
                             aDestDir, aOverWrite);
            return retVal;
        }
        catch (Exception e)
        {
            return false;
        }
    }

    /**
     * Deletes the contents of a directory. Delete is recursive and deletes the
     * entire source tree.
     *
     * @param aTargetDir
     *            absolute path of the name of the directory to be deleted
     * @return true in case delete was successful, false otherwise
     */
    public static boolean deleteAll(String aTargetDir) throws IOException
    {
        FileUtility source = new FileUtility(aTargetDir);
        if (!source.exists())
        {
            throw new IOException(FileUtilityErrorStrings.TARGET_DOES_NOT_EXIST
                                  + ": " + source);
        }

        if (!source.isDirectory())
        {
            throw new IOException(FileUtilityErrorStrings.SOURCE_NOT_DIRECTORY
                                  + ": " + source);
        }

        try
        {
            return FilePlatformSpecifics.deleteAll(aTargetDir);
        }
        catch (Exception e)
        {
            return false;
        }
    }
}
