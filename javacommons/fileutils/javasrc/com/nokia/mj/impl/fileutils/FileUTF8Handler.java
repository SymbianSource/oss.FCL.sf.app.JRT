/*
* Portions copyright (c) 2009 Nokia Corporation and/or its subsidiary(-ies).
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

import java.io.ByteArrayOutputStream;

/**
 * FileUTF8Handler helps in encoding an decoding a URL given.
 */
public class FileUTF8Handler
{
    public static String decode(String iEscapedUrl)
    {
        String decoded = iEscapedUrl;
        validateUnescapedUrl(decoded);
        if (iEscapedUrl.indexOf("%") != -1)
        {
            decoded = unescape(iEscapedUrl);
        }
        return decoded;
    }

    public static String encode(String iUnescapedUrl)
    {
        try
        {
            return URLUTF8Encoder.encode(decode(iUnescapedUrl));
        }
        catch (Exception e)
        {
            // Nothing to do
        }
        return iUnescapedUrl;
    }

    private static boolean isValidHexDigit(char aChar)
    {
        if (!Character.isDigit(aChar) && !(aChar >= 'a' && aChar <= 'f')
                && !(aChar >= 'A' && aChar <= 'F'))
        {
            return false;
        }
        return true;
    }

    private static void validateUnescapedUrl(String aUrl)
    {
        boolean isUnescaped = false;
        char charip1 = 0;
        char charip2 = 0;
        try
        {
            for (int i = 0; i < aUrl.length(); i++)
            {
                if (aUrl.charAt(i) == '%')
                {
                    isUnescaped = true;
                    charip1 = aUrl.charAt(i + 1);
                    charip2 = aUrl.charAt(i + 2);

                    if ((!isValidHexDigit(charip1) || !isValidHexDigit(charip2)))
                    {
                        throw new IllegalArgumentException();
                    }
                }
                else
                {
                    if ((aUrl.charAt(i) > 127) && isUnescaped)
                    {
                        throw new IllegalArgumentException();
                    }
                }
            }
        }
        catch (StringIndexOutOfBoundsException ex)
        {
            // Occurs in case % is not followed by 2 characters.
            throw new IllegalArgumentException();
        }
    }

    /*
     * Created: 17 April 1997 Author: Bert Bos <bert@w3.org>
     *
     * unescape: http://www.w3.org/International/unescape.java
     *
     * Copyright © 1997 World Wide Web Consortium, (Massachusetts Institute of
     * Technology, European Research Consortium for Informatics and Mathematics,
     * Keio University). All Rights Reserved. This work is distributed under the
     * W3C® Software License [1] in the hope that it will be useful, but
     * WITHOUT ANY WARRANTY; without even the implied warranty of
     * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
     *
     * [1] http://www.w3.org/Consortium/Legal/2002/copyright-software-20021231
     */
    public static String unescape(String s)
    {
        StringBuffer sbuf = new StringBuffer("");
        int l = s.length();
        int ch = -1;
        int b, sumb = 0;
        for (int i = 0, more = -1; i < l; i++)
        {
            /* Get next byte b from URL segment s */
            switch (ch = s.charAt(i))
            {
            case '%':
                ch = s.charAt(++i);
                int hb = (Character.isDigit((char) ch) ? ch - '0'
                          : 10 + Character.toLowerCase((char) ch) - 'a') & 0xF;
                ch = s.charAt(++i);
                int lb = (Character.isDigit((char) ch) ? ch - '0'
                          : 10 + Character.toLowerCase((char) ch) - 'a') & 0xF;
                b = (hb << 4) | lb;
                break;
            case '+':
                b = ' ';
                break;
            default:
                b = ch;
            }
            /* Decode byte b as UTF-8, sumb collects incomplete chars */
            if ((b & 0xc0) == 0x80)
            { // 10xxxxxx (continuation byte)
                sumb = (sumb << 6) | (b & 0x3f); // Add 6 bits to sumb
                if (--more == 0)
                    sbuf.append((char) sumb); // Add char to sbuf
            }
            else if ((b & 0x80) == 0x00)
            { // 0xxxxxxx (yields 7 bits)
                sbuf.append((char) b); // Store in sbuf
            }
            else if ((b & 0xe0) == 0xc0)
            { // 110xxxxx (yields 5 bits)
                sumb = b & 0x1f;
                more = 1; // Expect 1 more byte
            }
            else if ((b & 0xf0) == 0xe0)
            { // 1110xxxx (yields 4 bits)
                sumb = b & 0x0f;
                more = 2; // Expect 2 more bytes
            }
            else if ((b & 0xf8) == 0xf0)
            { // 11110xxx (yields 3 bits)
                sumb = b & 0x07;
                more = 3; // Expect 3 more bytes
            }
            else if ((b & 0xfc) == 0xf8)
            { // 111110xx (yields 2 bits)
                sumb = b & 0x03;
                more = 4; // Expect 4 more bytes
            }
            else
                /* if ((b & 0xfe) == 0xfc) */  // 1111110x (yields 1 bit)
            {
                sumb = b & 0x01;
                more = 5; // Expect 5 more bytes
            }
            /* We don't test if the UTF-8 encoding is well-formed */
        }
        return sbuf.toString();
    }

}

/**
 * Provides a method to encode any string into a URL-safe form. Non-ASCII
 * characters are first encoded as sequences of two or three bytes, using the
 * UTF-8 algorithm, before being encoded as %HH escapes.
 *
 * Created: 17 April 1997 Author: Bert Bos <bert@w3.org>
 *
 * URLUTF8Encoder: http://www.w3.org/International/URLUTF8Encoder.java
 *
 * Copyright © 1997 World Wide Web Consortium, (Massachusetts Institute of
 * Technology, European Research Consortium for Informatics and Mathematics,
 * Keio University). All Rights Reserved. This work is distributed under the
 * W3C® Software License [1] in the hope that it will be useful, but WITHOUT
 * ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
 * FOR A PARTICULAR PURPOSE.
 *
 * [1] http://www.w3.org/Consortium/Legal/2002/copyright-software-20021231
 */
class URLUTF8Encoder
{
    final static String[] hex = { "%00", "%01", "%02", "%03", "%04", "%05",
                                  "%06", "%07", "%08", "%09", "%0a", "%0b", "%0c", "%0d", "%0e",
                                  "%0f", "%10", "%11", "%12", "%13", "%14", "%15", "%16", "%17",
                                  "%18", "%19", "%1a", "%1b", "%1c", "%1d", "%1e", "%1f", "%20",
                                  "%21", "%22", "%23", "%24", "%25", "%26", "%27", "%28", "%29",
                                  "%2a", "%2b", "%2c", "%2d", "%2e", "%2f", "%30", "%31", "%32",
                                  "%33", "%34", "%35", "%36", "%37", "%38", "%39", "%3a", "%3b",
                                  "%3c", "%3d", "%3e", "%3f", "%40", "%41", "%42", "%43", "%44",
                                  "%45", "%46", "%47", "%48", "%49", "%4a", "%4b", "%4c", "%4d",
                                  "%4e", "%4f", "%50", "%51", "%52", "%53", "%54", "%55", "%56",
                                  "%57", "%58", "%59", "%5a", "%5b", "%5c", "%5d", "%5e", "%5f",
                                  "%60", "%61", "%62", "%63", "%64", "%65", "%66", "%67", "%68",
                                  "%69", "%6a", "%6b", "%6c", "%6d", "%6e", "%6f", "%70", "%71",
                                  "%72", "%73", "%74", "%75", "%76", "%77", "%78", "%79", "%7a",
                                  "%7b", "%7c", "%7d", "%7e", "%7f", "%80", "%81", "%82", "%83",
                                  "%84", "%85", "%86", "%87", "%88", "%89", "%8a", "%8b", "%8c",
                                  "%8d", "%8e", "%8f", "%90", "%91", "%92", "%93", "%94", "%95",
                                  "%96", "%97", "%98", "%99", "%9a", "%9b", "%9c", "%9d", "%9e",
                                  "%9f", "%a0", "%a1", "%a2", "%a3", "%a4", "%a5", "%a6", "%a7",
                                  "%a8", "%a9", "%aa", "%ab", "%ac", "%ad", "%ae", "%af", "%b0",
                                  "%b1", "%b2", "%b3", "%b4", "%b5", "%b6", "%b7", "%b8", "%b9",
                                  "%ba", "%bb", "%bc", "%bd", "%be", "%bf", "%c0", "%c1", "%c2",
                                  "%c3", "%c4", "%c5", "%c6", "%c7", "%c8", "%c9", "%ca", "%cb",
                                  "%cc", "%cd", "%ce", "%cf", "%d0", "%d1", "%d2", "%d3", "%d4",
                                  "%d5", "%d6", "%d7", "%d8", "%d9", "%da", "%db", "%dc", "%dd",
                                  "%de", "%df", "%e0", "%e1", "%e2", "%e3", "%e4", "%e5", "%e6",
                                  "%e7", "%e8", "%e9", "%ea", "%eb", "%ec", "%ed", "%ee", "%ef",
                                  "%f0", "%f1", "%f2", "%f3", "%f4", "%f5", "%f6", "%f7", "%f8",
                                  "%f9", "%fa", "%fb", "%fc", "%fd", "%fe", "%ff"
                                };

    /**
     * Encode a string to the "x-www-form-urlencoded" form, enhanced with the
     * UTF-8-in-URL proposal. This is what happens:
     *
     * <ul>
     * <li>
     * <p>
     * The ASCII characters 'a' through 'z', 'A' through 'Z', and '0' through
     * '9' remain the same.
     *
     * <li>
     * <p>
     * The unreserved characters - _ . ! ~ * ' ( ) remain the same.
     *
     * <li>
     * <p>
     * The space character ' ' is converted into a plus sign '+'.
     *
     * <li>
     * <p>
     * All other ASCII characters are converted into the 3-character string
     * "%xy", where xy is the two-digit hexadecimal representation of the
     * character code
     *
     * <li>
     * <p>
     * All non-ASCII characters are encoded in two steps: first to a sequence of
     * 2 or 3 bytes, using the UTF-8 algorithm; secondly each of these bytes is
     * encoded as "%xx".
     * </ul>
     *
     * @param s
     *            The string to be encoded
     * @return The encoded string
     */
    public static String encode(String s)
    {
        StringBuffer sbuf = new StringBuffer();
        int len = s.length();
        for (int i = 0; i < len; i++)
        {
            int ch = s.charAt(i);
            if ('A' <= ch && ch <= 'Z')
            { // 'A'..'Z'
                sbuf.append((char) ch);
            }
            else if ('a' <= ch && ch <= 'z')
            { // 'a'..'z'
                sbuf.append((char) ch);
            }
            else if ('0' <= ch && ch <= '9')
            { // '0'..'9'
                sbuf.append((char) ch);
            }
            else if (ch == ' ')
            { // space
                // sbuf.append('+');
                sbuf.append("%20");
            }
            else if (ch == '-'
                     || ch == '_' // unreserved
                     || ch == '.' || ch == '!' || ch == '~' || ch == '*'
                     || ch == '\'' || ch == '(' || ch == ')' || ch == ':'
                     || ch == '/')
            {
                sbuf.append((char) ch);
            }
            else if (ch <= 0x007f)
            { // other ASCII
                sbuf.append(hex[ch]);
            }
            else if (ch <= 0x07FF)
            { // non-ASCII <= 0x7FF
                sbuf.append(hex[0xc0 | (ch >> 6)]);
                sbuf.append(hex[0x80 | (ch & 0x3F)]);
            }
            else
            { // 0x7FF < ch <= 0xFFFF
                sbuf.append(hex[0xe0 | (ch >> 12)]);
                sbuf.append(hex[0x80 | ((ch >> 6) & 0x3F)]);
                sbuf.append(hex[0x80 | (ch & 0x3F)]);
            }
        }
        return sbuf.toString();
    }
}
