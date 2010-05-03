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

package com.nokia.mj.impl.utils;

import java.io.InputStream;
import java.io.IOException;
import java.util.Hashtable;

/**
 * JadReader is used to parse application descriptor files.
 *
 * BNF for Parsing Application Descriptors is the default
 * BNF from MIDP 2.1 spec
 * + added support for continuation lines (Manifest style)
 * + accepting standalone CR as newline
 * + ignoring lines containing only CTLs and WSP
 * + ignore WSP after attrname
 * + ignore WSP before and after attribute value
 * + ignore UTF-8, UTF-16 and UTF-32 BOMs (byte order marks)
 *
 * Max attribute value length is AttributeReader.MAX_ATTR_VALUE_LEN,
 * currently 65536 characters
 *
 * appldesc: *attrline
 * attrline: attrname [WSP] ":" [WSP] attrvalue | ( *(CTL | WSP) 1*newline )
 * attrname: 1*namechar
 * attrvalue: attrvaluepart *(newline SP attrvaluepart) [WSP] newline
 * attrvaluepart: 1*valuechar
 *
 * namechar: <any Unicode character, excluding CTLs and separators>
 * valuechar: <any Unicode character, excluding CTLs and WSP>
 * newline: CR LF | LF | CR
 * CR = <Unicode carriage return (U+000D)>
 * LF = <Unicode linefeed (U+000A)>
 * WSP: 1*( SP | HT )
 * SP = <Unicode space (U+0020)>
 * HT = <Unicode horizontal-tab (U+0009)>
 * CTL = <Unicode characters U+0000 - U+001F and U+007F>
 * separators: "(" | ")" | "<" | ">" | "@" |
 * "," | ";" | ":" | "'" | <"> | "/" | "[" |
 * "]" | "?" | "=" | "{" | "}" | SP  | HT
 *
 */
public class JadReader extends AttributeReader
{
    /**
     * Reads Jad attributes from given InputStream using UTF-8 charset.
     * Given InputStream must support mark and reset methods.
     * If duplicate attributes are found,
     * the last value of the attribute is used.
     *
     * @param aInputStream InputStream from where the attributes are parsed
     * @return hashtable that contains all the attributes
     * @throws IOException if the attributes cannot be read
     */
    public static Hashtable getAttributes(InputStream aInputStream)
    throws IOException
    {
        return getAttributes(aInputStream, null);
    }

    /**
     * Reads Jad attributes from given InputStream.
     * Given InputStream must support mark and reset methods.
     * If duplicate attributes are found,
     * the last value of the attribute is used.
     *
     * @param aInputStream InputStream from where the attributes are parsed
     * @param aCharSetName character encoding used in Jad file,
     *  if null or empty string UTF-8 is used
     * @return hashtable that contains all the attributes
     * @throws IOException if the attributes cannot be read
     */
    public static Hashtable getAttributes(
        InputStream aInputStream, String aCharSetName) throws IOException
    {
        return (new JadReader()).parseJad(
                   aInputStream, aCharSetName, MAX_ATTR_VALUE_LEN);
    }

    /*** ---------------------------- PROTECTED --------------------------- */

    /**
     * Default constructor.
     */
    protected JadReader()
    {
        super();
        iFileType = "Jad";
    }

    protected boolean isNameChar(int ch)
    {
        if (isCtl(ch) || isSeparator(ch))
        {
            return false;
        }
        return true;
    }

    protected boolean isValueChar(int ch)
    {
        return !isCtl(ch);
    }

    /**
     * Reads attributes from given InputStream.
     * Given InputStream must support mark and reset methods.
     * Handles duplicate attributes like java.util.jar.Manifest
     * (== the last value of the attribute is used).
     *
     * @param aInputStream InputStream to be parsed
     * @param aCharSetName character encoding used in .jad file,
     *  if empty string UTF-8 is used
     * @param aBufSize buffer size for attribute values
     * @return hashtable that contains all attributes read from .jad file
     * @throws IOException is the file cannot read or file is not valid .jad
     *  according to BNF
     * @throws UnsupportedEncodingException if the character set
     *  encoding is not supported
     * @see ManifestReader
     */
    protected Hashtable parseJad(
        InputStream aInputStream, String aCharSetName, int aBufSize)
    throws IOException
    {
        boolean charSetSpecified = true;
        if ((null == aCharSetName) || (aCharSetName.trim().length() == 0))
        {
            // If charset is not given, try to determine it later based on
            // possible BOM (byte order mark).
            charSetSpecified = false;
            // By default use UTF-8 character encoding.
            aCharSetName = "UTF-8";
        }

        // Make sanity checks for the buffer size.
        if (aBufSize < 12)
        {
            aBufSize = 1024;
        }
        else if (aBufSize > MAX_ATTR_VALUE_LEN)
        {
            aBufSize = MAX_ATTR_VALUE_LEN;
        }

        // Read BOM.
        InputStream inTmp = aInputStream;
        if (!inTmp.markSupported())
        {
            // If InputStream does not support mark and reset methods,
            // do not proceed further.
            throw new IOException(
                "JadReader: InputStream does not support mark and reset");
        }
        inTmp.mark(8);
        int[]   bom = new int[4];
        long    nBytesToSkip = 0;
        String  charSetFromBom = null;
        bom[0] = inTmp.read();
        bom[1] = inTmp.read();
        bom[2] = inTmp.read();
        bom[3] = inTmp.read();
        if ((bom[0] == 0xFE) && (bom[1] == 0xFF))
        {
            // UTF-16 BEBOM
            charSetFromBom = "UTF-16BE";
            nBytesToSkip = 2;
        }
        else if ((bom[0] == 0xFF) && (bom[1] == 0xFE) && (bom[2] == 0x00) && (bom[3] == 0x00))
        {
            // UTF-32 LEBOM
            charSetFromBom = "UTF-32LE";
            nBytesToSkip = 4;
        }
        else if ((bom[0] == 0xFF) && (bom[1] == 0xFE))
        {
            // UTF-16 LEBOM
            charSetFromBom = "UTF-16LE";
            nBytesToSkip = 2;
        }
        else if ((bom[0] == 0xEF) && (bom[1] == 0xBB) && (bom[2] == 0xBF))
        {
            // UTF-8 BOM
            charSetFromBom = "UTF-8";
            nBytesToSkip = 3;
        }
        else if ((bom[0] == 0x00) && (bom[1] == 0x00) && (bom[2] == 0xFE) && (bom[3] == 0xFF))
        {
            // UTF-32 BEBOM
            charSetFromBom = "UTF-32BE";
            nBytesToSkip = 4;
        }
        inTmp.reset();
        // Skip BOM if it was present.
        if (nBytesToSkip > 0)
        {
            inTmp.skip(nBytesToSkip);
        }

        // If character set was not given as parameter use
        // the character set that was determined from byte order mark
        // in the beginning of the Jad file (if byte order mark was present).
        if ((!charSetSpecified) && (charSetFromBom != null))
        {
            aCharSetName = charSetFromBom;
        }

        // Read the attributes and return them.
        return readAttributes(inTmp, aCharSetName, aBufSize);
    }

    /*** ----------------------------- PRIVATE ---------------------------- */

    private static boolean isSeparator(int ch)
    {
        // separators: "(" | ")" | "<" | ">" | "@" |
        // "," | ";" | ":" | "'" | <"> | "/" | "[" |
        // "]" | "?" | "=" | "{" | "}" | SP  | HT

        // unic vals:  $28 | $29 | $3C | $3E | $40 |
        // $2C | $3B | $3A | $27 | $22 | $2F | $5B |
        // $5D | $3F | $3D | $7B | $7D | $20 | $09

        // Make comparison in somewhat optimised order so that
        // normal characters 'a'...'z' and 'A'...'Z' do not need
        // too many equals checks.
        if ((ch == 0x7B) || (ch == 0x7D) || (ch == 0x5B) || (ch == 0x5D))
        {
            return true;
        }

        if (ch > 0x40)
        {
            return false;
        }

        if ((ch == 0x28) || (ch == 0x29) || (ch == 0x3C) || (ch == 0x3E) ||
                (ch == 0x2C) || (ch == 0x3B) || (ch == 0x3A) || (ch == 0x27) ||
                (ch == 0x22) || (ch == 0x2F) || (ch == 0x3F) || (ch == 0x3D) ||
                (ch == 0x20) || (ch == 0x09))
        {
            return true;
        }

        return false;
    }
}
