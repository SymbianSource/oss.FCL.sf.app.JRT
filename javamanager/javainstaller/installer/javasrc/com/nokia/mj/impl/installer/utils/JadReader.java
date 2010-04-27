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

import java.io.BufferedInputStream;
import java.io.FileInputStream;
import java.io.InputStream;
import java.io.IOException;
import java.util.Hashtable;

/**
 * JadReader is used to parse application descriptor files.
 * This class extends the functionality of
 * com.nokia.mj.impl.utils.JadReader
 * by adding possibility to parse from given jad file.
 *
 * @see com.nokia.mj.impl.utils.JadReader
 */
public class JadReader extends com.nokia.mj.impl.utils.JadReader
{
    /**
     * Reads attributes from given application descriptor file.
     * Handles duplicate attributes like java.util.jar.Manifest
     * (== the last value of the attribute is used).
     * Assumes that given application descriptor file uses
     * UTF-8 character encoding.
     *
     * @param aFilename .jad file to be parsed
     * @return hashtable that contains all attributes read from .jad file
     * @throws IOException is the file cannot read or file is not valid .jad
     *  according to BNF
     * @throws UnsupportedEncodingException if the character set
     *  encoding is not supported
     */
    public static Hashtable getAttributes(String aFilename) throws IOException
    {
        return getAttributes(aFilename, null);
    }

    /**
     * Reads attributes from given application descriptor file.
     * Handles duplicate attributes like java.util.jar.Manifest
     * (== the last value of the attribute is used).
     *
     * @param aFilename .jad file to be parsed
     * @param aCharSetName character encoding used in .jad file,
     *  if null or empty string UTF-8 is used
     * @return hashtable that contains all attributes read from .jad file
     * @throws IOException is the file cannot read or file is not valid .jad
     *  according to BNF
     * @throws UnsupportedEncodingException if the character set
     *  encoding is not supported
     */
    public static Hashtable getAttributes(String aFilename, String aCharSetName)
    throws IOException
    {
        return (new JadReader()).parseJad(aFilename, aCharSetName);
    }

    /*** ---------------------------- PROTECTED --------------------------- */

    /**
     * Default constructor.
     */
    protected JadReader()
    {
        super();
    }

    /*** ----------------------------- PRIVATE ---------------------------- */

    /**
     * Reads attributes from given application descriptor file.
     * Handles duplicate attributes like java.util.jar.Manifest
     * (== the last value of the attribute is used).
     *
     * @param aFilename .jad file to be parsed
     * @param aCharSetName character encoding used in .jad file,
     *  if empty string UTF-8 is used
     * @return hashtable that contains all attributes read from .jad file
     * @throws IOException is the file cannot read or file is not valid .jad
     *  according to BNF
     * @throws UnsupportedEncodingException if the character set
     *  encoding is not supported
     */
    private Hashtable parseJad(String aFilename, String aCharSetName)
    throws IOException
    {
        //Log.log("JadReader: Going to parse Jad file " + aFilename);
        // Use FileInputStream wrapped into BufferedInputStream because
        // it supports mark and reset methods and is somewhat faster to
        // parse than InputStream from FileUtility.
        FileUtility file = new FileUtility(aFilename);
        Hashtable result = parseJad(
                               new BufferedInputStream(new FileInputStream(aFilename)),
                               aCharSetName, (int)file.fileSize());
        //Log.log("JadReader returns attributes using charset " + aCharSetName);
        //Log.log("JadReader returns attributes:\n" + result);
        return result;
    }

}
