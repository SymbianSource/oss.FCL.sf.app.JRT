/*
* Copyright (c) 2008-2009 Nokia Corporation and/or its subsidiary(-ies).
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

import com.nokia.mj.impl.utils.Attribute;
import com.nokia.mj.impl.utils.BufferedReader;

import java.io.InputStream;
import java.io.InputStreamReader;
import java.io.IOException;
import java.io.Reader;
import java.util.Hashtable;

/**
 * AttributeReader is a base class which contains functionality
 * common to JadReader and ManifestReader. The JadReader
 * and ManifestReader subclasses must override the behaviour
 * implemented to this class in the cases where behaviour
 * of parsing Jad and Manifest differ.
 */
public class AttributeReader
{
    /** Attribute values can be max 64k characters long. */
    protected static final int MAX_ATTR_VALUE_LEN = 65536;

    /**
     * File type to be parsed, either Jad or Manifest.
     * Subclass must initialize this member.
     */
    protected String iFileType = "";

    /** Internal state member for parsing attribute names. */
    protected boolean iNextParseFirstNameChar = true;

    /**
     * Contains the first character of the next line when that has
     * been read "too early" just to see if the attribute value
     * continues in the next line (lookahead variable).
     * It has the value -2 when there is no unhandled
     * lookahead character.
     */
    protected int iLookaheadCh = -2;

    /**
     * Contains the number of characters iAttrValue currently contains.
     */
    protected int iInd = 0;

    /**
     * Work array for constructing a attribute value. The JAR manifest
     * specification requires that we must be able to parse attribute
     * values that are at least 64kB (32768 Unicode characters) long.
     */
    protected char[] iAttrValue = null;

    /**
     * Default constructor.
     */
    protected AttributeReader()
    {
    }

    /**
     * Returns true if given character is a valid character in attribute
     * name. This method must be implemented in the subclass.
     */
    protected boolean isNameChar(int ch)
    {
        throw new RuntimeException(
            "AttributeReader.isNameChar not implemented");
    }

    /**
     * Returns true if given characater is a valid character in attribute
     * value. This method must be implemented in the subclass.
     */
    protected boolean isValueChar(int ch)
    {
        throw new RuntimeException(
            "AttributeReader.isValueChar not implemented");
    }

    /**
     * Returns true if given character is a control character.
     * Note that also isCtl(EOF) returns true.
     */
    protected boolean isCtl(int ch)
    {
        if ((ch < 0x20) || (ch == 0x7F))
        {
            return true;
        }
        return false;
    }

    /**
     * Reads attributes from given InputStream using given character set.
     * If aCharSetName is null, uses UTF-8 character set.
     * Closes the InputStream in the end.
     */
    protected Hashtable readAttributes(
        InputStream aInputStream, String aCharSetName, int aMaxAttrValueLen)
    throws IOException
    {
        if (aInputStream == null)
        {
            throw new IOException(iFileType + " InputStream is null");
        }
        if ((aCharSetName == null) || (aCharSetName.trim().length() == 0))
        {
            aCharSetName = "UTF-8";
        }
        iAttrValue = new char[aMaxAttrValueLen];
        Reader in = new BufferedReader(
            new InputStreamReader(aInputStream, aCharSetName));
        return readAttributes(in);
    }

    /**
     * Reads attributes from given Reader.
     * Closes the Reader in the end.
     */
    protected Hashtable readAttributes(Reader in) throws IOException
    {
        // Setting the initial capacity to 42 allows handling max 31
        // attributes without slow rehash. Real midlets seem to have
        // on average 22 attributes (average of 14 commercial midlets),
        // only one had more than 31 attributes.
        Hashtable result = new Hashtable(42);
        Attribute attr;
        try
        {
            while ((attr = readAttrLine(in)) != null)
            {
                // Note that if there are many attributes with the same name,
                // the last value is used.
                result.put(attr.getName(), attr);
            }
        }
        finally
        {
            in.close();
        }
        return result;
    }

    private Attribute readAttrLine(Reader in) throws IOException
    {
        // The attribute names and values are trimmed.
        String name = readAttrName(in).trim();
        // When parsing next attribute name the first char must
        // be alphanumeric.
        iNextParseFirstNameChar = true;
        // Attribute must have a name but it does not have to have a value.
        if (name.length() == 0)
        {
            // End of file.
            return null;
        }
        String value = readAttrValue(in).trim();
        return new Attribute(name, value);
    }

    private String readAttrName(Reader in) throws IOException
    {
        int ch;
        int ch_prev = -2;
        int ind = 0;
        // Max length of attribute name is 512 characters.
        char[] chName = new char[512];

        // Get the first character of the attribute line either from
        // the lookahead variable or from the stream.
        if (iLookaheadCh != -2)
        {
            ch = iLookaheadCh;
            // Reset the lookahead variable after value has been handled.
            iLookaheadCh = -2;
        }
        else
        {
            ch = in.read();
        }

        // Skip possible empty lines.
        while (isCtl(ch) || (ch == 0x20))
        {
            ch_prev = ch;
            ch = in.read();
            if (ch == -1)
            {
                return ""; // EOF
            }
        }
        // Now ch contains character that is not control char, newline or SP.

        // Attribute name must start from the beginning of line,
        // it cannot be preceeded by any CTLs or WSP.
        if ((ch_prev != 0x0D) && (ch_prev != 0x0A) && (ch_prev != -2))
        {
            in.close();
            throw new IOException(
                iFileType +
                " attribute name was preceeded by illegal character 0x" +
                Integer.toHexString(ch_prev));
        }

        // Start handling characters that will form the attribute name.
        try
        {
            while (isNameChar(ch))
            {
                // Add to name buffer.
                chName[ind++] = (char)ch;
                ch = in.read();
                if (ch == -1)
                {
                    return ""; // EOF, ignore incomplete name in last line.
                }
            }
        }
        catch (IndexOutOfBoundsException e)
        {
            in.close();
            String beginning = new String(chName, 0, 64);
            throw new IOException(
                "Length of " + iFileType + " attribute name exceeded " +
                chName.length + " characters. Name starts: " + beginning);
        }
        if (ind == 0)
        {
            throw new IOException(iFileType + " attribute name was empty.");
        }
        String name = new String(chName, 0, ind);

        // All valid characters for name have been read, skip possible WSP.
        while (isWSP(ch))
        {
            ch = in.read();
            if (ch == -1)
            {
                return ""; // EOF, ignore incomplete name in last line.
            }
        }

        // Now the character must be ':'.
        if (ch != ':')
        {
            in.close();
            throw new IOException(
                iFileType + " attribute name contains illegal character 0x" +
                Integer.toHexString(ch) + ". Name starts: " + name);
        }

        return name;
    }

    private String readAttrValue(Reader in) throws IOException
    {
        // Prepare to call readAttrValuePart.
        iInd = 0;
        int ch = in.read();
        if (ch == -1)
        {
            // EOF, allow last line that contains attribute without value.
            return "";
        }

        // Skip possible WSP characters before the actual value starts.
        while (isWSP(ch))
        {
            ch = in.read();
        }

        if (ch == -1)
        {
            // EOF, return value read.
            return new String(iAttrValue, 0, iInd);
        }

        return readAttrValuePart(ch, in);
    }


    // Note that this function is recursive to handle 0..N continuation lines.
    private String  readAttrValuePart(int ch, Reader in)
    throws IOException
    {
        if (ch == -1)
        {
            // EOF, return value read.
            return new String(iAttrValue, 0, iInd);
        }

        // Start handling characters that will form the attribute value.
        try
        {
            while (isValueChar(ch))
            {
                // Add to value buffer.
                iAttrValue[iInd++] = (char)ch;
                ch = in.read();
                // Returning value when EOF found allows last value without
                // newline after it.
                if (ch == -1) return new String(iAttrValue, 0, iInd);
            }
        }
        catch (IndexOutOfBoundsException e)
        {
            in.close();
            String beginning = new String(iAttrValue, 0, 64);
            throw new IOException(
                "Length of " + iFileType + " attribute value exceeded " +
                MAX_ATTR_VALUE_LEN + " characters.  Value starts: " +
                beginning);
        }

        // Ignore possible WSP after attribute value.
        while (isWSP(ch))
        {
            ch = in.read();
            if (ch == -1)
            {
                // Returning value when EOF found allows last value without
                // newline after it.
                return new String(iAttrValue, 0, iInd);
            }
        }

        // Now ch contains either newline (which means end of value OR
        // continuation line) or other control character (illegal).
        if (!isNewline(ch))
        {
            // Log max first 64 characters of the value to log.
            String beginning = new String(iAttrValue, 0, ((iInd>64)?64:iInd));
            throw new IOException(
                iFileType + " attribute value contains illegal character 0x" +
                Integer.toHexString(ch) + ". Value starts: " + beginning);
        }

        if (ch == 0x0D)
        {
            // After CR character we may have LF that is ignored.
            ch = in.read();
            if (ch == 0x0A)
            {
                ch = in.read();
            }
        }
        else
        {
            ch = in.read();
        }

        // Now ch contains the next character after newline,
        // if it is SP this is beginning of continuation line.
        if (ch != 0x20)
        {
            // Now the character in ch must be passed to readAttrName
            // because it is part of the new, fresh line.
            iLookaheadCh = ch;
            // No continuation, whole value has been read and can be returned.
            return new String(iAttrValue, 0, iInd);
        }
        // ch contains SP that must be skipped because it is a
        // continuation mark.
        ch = in.read();

        // Read next continuation line.
        return readAttrValuePart(ch, in);
    }

    /**
     * Returns true if given character is a newline.
     */
    private static boolean isNewline(int ch)
    {
        if ((ch == 0x0A) || (ch == 0x0D))
        {
            return true;
        }
        return false;
    }

    /**
     * Returns true if given character is a whitespace.
     */
    private static boolean isWSP(int ch)
    {
        if ((ch == 0x20) || (ch == 0x09))
        {
            return true;
        }
        return false;
    }
}
