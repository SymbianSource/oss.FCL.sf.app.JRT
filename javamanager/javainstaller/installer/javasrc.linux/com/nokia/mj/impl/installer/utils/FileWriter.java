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
    }

    /**
     * Creates a new file. After calling create file can be written to.
     */
    public void create() throws IOException
    {
        iOut = FileUtils.getOutputStream(iSuggestedFilename);
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
        if (null == iOut)
        {
            throw new IOException(
                "FileWriter write called before create.");
        }
        iOut.write(aBytes, aOffset, aLength);
    }

    /**
     * Closes the file.
     */
    public void close() throws IOException
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
}
