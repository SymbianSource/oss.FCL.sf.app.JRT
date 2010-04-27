/*
* Copyright (c) 2010 Nokia Corporation and/or its subsidiary(-ies).
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

import com.nokia.mj.impl.installer.utils.FileUtils;
import com.nokia.mj.impl.installer.utils.Log;

import java.io.IOException;
import java.io.OutputStream;

/**
 * This class provides means to create a forward locked file, that is a
 * file which can only be used in the device in which the file is created.
 */
public final class FileWriter
{
    /** MIME type for the file contents. */
    private String iMimeType = null;
    /** Suggested filename. */
    private String iSuggestedFilename = null;
    /** Filename chosen by DRM framework. */
    private String iOutputFilename = null;
    /** True if the file is written as forward locked, false otherwise. */
    private boolean iLocked = false;
    /** OutputStream for the file. */
    private OutputStream iOut = null;
    /** Handle to native side object. */
    private int iHandle = 0;

    /*** ----------------------------- PUBLIC ------------------------------ */

    /**
     * Constructor.
     *
     * @param aMimeType MIME type for the file contents.
     * @param aSuggestedFilename suggested filename
     * @param aLocked true if the file should be written as encrypted
     * (forward locked), false if the file should be written as unencrypted
     * (not forward locked).
     */
    public FileWriter(
        String aMimeType, String aSuggestedFilename, boolean aLocked)
    {
        iMimeType = aMimeType;
        iSuggestedFilename = aSuggestedFilename;
        iLocked = aLocked;
        Log.log("FileWriter: " + aSuggestedFilename + ", " +
                aMimeType + ", locked: " + aLocked);
    }

    /**
     * Creates a new file. After calling create the file can be written to.
     */
    public void create() throws IOException
    {
        if (iLocked)
        {
            String outputPath = FileUtils.getParent(iSuggestedFilename);
            String suggestedFilename = FileUtils.getName(iSuggestedFilename);
            int ret = _create(iMimeType, outputPath, suggestedFilename);
            if (ret < -1)
            {
                throw new IOException(
                    "FileWriter: creating file " + iSuggestedFilename +
                    " failed with code " + ret);
            }
            iHandle = ret;
        }
        else
        {
            iOut = FileUtils.getOutputStream(iSuggestedFilename);
        }
    }

    /**
     * Writes given bytes to the file.
     *
     * @param aBytes array of bytes to write
     * @param aOffset the start offset in aBytes array
     * @param aLength number of bytes to write
     */
    public void write(byte[] aBytes, int aOffset, int aLength) throws IOException
    {
        if (iLocked)
        {
            if (0 == iHandle)
            {
                throw new IOException(
                    "FileWriter write called before create.");
            }
            int ret = _write(iHandle, aBytes, aOffset, aLength);
            if (ret < -1)
            {
                throw new IOException(
                    "FileWriter: writing file " + iSuggestedFilename +
                    " failed with code " + ret);
            }
        }
        else
        {
            if (null == iOut)
            {
                throw new IOException(
                    "FileWriter write called before create.");
            }
            iOut.write(aBytes, aOffset, aLength);
        }
    }

    /**
     * Closes the file.
     */
    public void close() throws IOException
    {
        if (iLocked)
        {
            if (0 == iHandle)
            {
                throw new IOException(
                    "FileWriter close called before create.");
            }
            StringBuffer outputFilename = new StringBuffer();
            int ret = _close(iHandle, outputFilename);
            if (ret < -1)
            {
                throw new IOException(
                    "FileWriter: closing file " + iSuggestedFilename +
                    " failed with code " + ret);
            }
            iHandle = 0;
            iOutputFilename = outputFilename.toString();
        }
        else
        {
            if (null == iOut)
            {
                throw new IOException(
                    "FileWriter close called before create.");
            }
            iOut.close();
            iOut = null;
            iOutputFilename = iSuggestedFilename;
        }
    }

    /**
     * Returns filename that the DRM framework allocated.
     * This method can be called only after file has been close.
     */
    public String getOutputFilename()
    {
        return iOutputFilename;
    }

    /*** ---------------------------- PROTECTED --------------------------- */
    /*** ----------------------------- PACKAGE ---------------------------- */
    /*** ----------------------------- PRIVATE ---------------------------- */

    /**
     * Creates a new file. After calling create the file can be written to.
     *
     * @param aMimeType MIME type for the file contents
     * @param aOutputPath directory for the output file
     * @param aSuggestedFilename suggested filename
     * @return native handle or Symbian error code (negative number)
     */
    private static native int _create(
        String aMimeType, String aOutputPath, String aSuggestedFilename);

    /**
     * Writes given bytes to the file.
     *
     * @param aHandle native handle obtained from _create method
     * @param aBytes array of bytes to write
     * @param aOffset the start offset in aBytes array
     * @param aLength number of bytes to write
     * @return Symbian error code (negative number)
     */
    public static native int _write(
        int aHandle, byte[] aBytes, int aOffset, int aLength);

    /**
     * Closes the file.
     *
     * @param aHandle native handle obtained from _create method
     * @param aOutputFilename filename of the created file
     * @return Symbian error code (negative number)
     */
    public static native int _close(
        int aHandle, StringBuffer aOutputFilename);
}
