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


package com.nokia.mj.impl.utils;

import java.io.*;
import java.util.*;

/**
 * A line based reader. Either line feed ("\n") or carriage return ('\r') or
 * crlf sequences ("\r\n") are considered as line breaks.
 * <br>
 * The Reader used for LineReader should preferably have buffering, as this
 * LineReader does not buffer the input.
 *
 * @author Nokia Corporation
 * @version 1.0
 */
public class LineReader extends java.io.Reader
{
    /** Underlying reader */
    private Reader reader;

    /** Buffer for reading lines */
    private StringBuffer buffer = new StringBuffer();

    /** Tells if last read char was a carriage return */
    private boolean lastCarriageReturn = false;

    /** Next line number */
    private int line = 0;

    /*** ----------------------------- PUBLIC ------------------------------ */

    /**
     * Create a line reader
     *
     * @param aReader reader instance
     */
    public LineReader(Reader aReader)
    {
        reader = aReader;
    }

    /**
     * Basic reader support. Note that line number is not increased when
     * read(...) is used.
     */
    public int read(char[] buf, int pos, int len)
    throws IOException
    {
        return reader.read(buf, pos, len);
    }

    /**
     * Closes the underlying input.
     */
    public void close()
    throws IOException
    {
        reader.close();
    }

    /**
     * Reads one line. Note that combining read(...) and readLine calls on
     * same LineReader leads to unexpected results.
     *
     * @return next line with ending newlines remove, or null when at
     * end-of-input
     * @throws IOException if error occurs on input
     */
    public String readLine()
    throws IOException
    {
        buffer.setLength(0);
        int c;

        while ((c = reader.read()) != -1)
        {
            if (c == '\n')
            {
                if (lastCarriageReturn)
                {
                    // This is part of last crlf, ignore
                }
                else
                {
                    // New line feed, breakout
                    line++;
                    break;
                }
            }
            else if (c == '\r')
            {
                // New carriage return, breakout
                line++;
                break;
            }
            else
            {
                buffer.append((char)c);
            }
            lastCarriageReturn = false;
        }
        lastCarriageReturn = (c == '\r');

        if (c == -1 && buffer.length() == 0)
        {
            return null;
        }

        return buffer.toString();
    }

    /**
     * Line number of next line to read. Line number start from zero.
     *
     * @return next line number
     */
    public int getLineNumber()
    {
        return line;
    }
}
