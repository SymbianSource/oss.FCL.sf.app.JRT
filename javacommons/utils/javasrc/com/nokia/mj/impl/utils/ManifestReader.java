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
import java.util.Enumeration;
import java.util.Hashtable;

/**
 * ManifestReader is used to parse Manifest files inside .jar file.
 *
 * BNF for parsing manifests is the default
 * BNF from MIDP 2.1 spec
 * + ignoring lines containing only CTLs and WSP
 * + ignore WSP after attrname
 * + ignore WSP before and after attribute value
 * + max attribute name is 512 characters
 * + no maximum line length
 * + max manifest length is 1MB
 *
 * Max attribute value length is AttributeReader.MAX_ATTR_VALUE_LEN,
 * currently 65536 characters
 *
 * manifest: *attrline
 * attrline: attrname [WSP] ":" [WSP] *otherchar newline *continuation
 * attrname: alphanum *headerchar
 * continuation: SP *otherchar newline
 *
 * alphanum: {A-Z} | {a-z} | {0-9}
 * headerchar: alphanum | - | _
 * valuechar: <any UTF-8 character, excluding NUL, CR and LF>
 * newline: CR LF | LF | CR
 * CR = <UTF-8 carriage return (U+000D)>
 * LF = <UTF-8 linefeed (U+000A)>
 * WSP: 1*( SP | HT )
 * SP = <UTF-8 space (U+0020)>
 * HT = <UTF-8 horizontal-tab (U+0009)>
 *
 */
public class ManifestReader extends AttributeReader
{
    /**
     * Reads Manifest attributes from given InputStream.
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
        return (new ManifestReader()).readAttributes(
                   aInputStream, null, MAX_ATTR_VALUE_LEN);
    }

    /*** ---------------------------- PROTECTED --------------------------- */

    /**
     * Default constructor.
     */
    protected ManifestReader()
    {
        super();
        iFileType = "Manifest";
    }

    protected boolean isNameChar(int ch)
    {
        // First character of name must be alphanum.
        if (iNextParseFirstNameChar)
        {
            iNextParseFirstNameChar = false;
            return isAlphanum(ch);
        }
        // Following characters can be header chars.
        return isHeaderChar(ch);
    }

    protected boolean isValueChar(int ch)
    {
        if ((ch > 0) && (ch != 0x0D) && (ch != 0x0A))
        {
            return true;
        }
        return false;
    }

    /*** ----------------------------- PRIVATE ---------------------------- */

    private static boolean isAlphanum(int ch)
    {
        // alphanum: {A-Z} | {a-z} | {0-9}
        if (((ch > 0x40) && (ch < 0x5B)) ||
                ((ch > 0x60) && (ch < 0x7B)) ||
                ((ch > 0x2F) && (ch < 0x3A)))
        {
            return true;
        }
        return false;
    }

    private static boolean isHeaderChar(int ch)
    {
        // '-' or '_' or alphanum
        if ((ch == 0x2D) || (ch == 0x5F) || isAlphanum(ch))
        {
            return true;
        }
        return false;
    }
}
