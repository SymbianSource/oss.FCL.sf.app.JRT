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


package com.nokia.mj.impl.utils;

import java.util.Calendar;
import java.util.Date;
import java.util.Vector;

/**
 * Formatter for Qt based platform.
 * @see Formatter
 */
class FormatterQt extends Formatter
{
    /** ResourceLoader to be used with this Formatter. */
    private ResourceLoaderQt iResourceLoader = null;

    /** Original text id. */
    private String iTextId = null;

    /** Number indicating plurality set with argn() method. */
    private int iArgN = -1;

    /** Text parameters set with arg() methods. */
    private Vector iArgs = new Vector();

    /*** ----------------------------- PUBLIC ------------------------------ */

    /**
     * Set the plurality for this Formatter.
     *
     * @param n number indicating plurality
     * @return same formatter
     */
    public Formatter argn(int n)
    {
        iArgN = n;
        return this;
    }

    /**
     * Replace the lowest numbered parameter in the string, which is not yet
     * replaced.
     *
     * @param string string to replace at the argument
     * @return same formatter
     */
    public Formatter arg(String string)
    {
        iArgs.addElement(string);
        return this;
    }

    /**
     * Replace the least numbered parameter in the string, which is not yet
     * replaced.
     *
     * @param number number to replace at the argument
     * @return same formatter
     */
    public Formatter arg(int number)
    {
        iArgs.addElement(new Integer(number));
        return this;
    }

    /**
     * Replace the least numbered parameter in the string, which is not yet
     * replaced.
     *
     * @param ch character to replace at the argument
     * @return same formatter
     */
    public Formatter arg(char ch)
    {
        iArgs.addElement(new Character(ch));
        return this;
    }

    /**
     * Replace the least numbered parameter in the string, which is not yet
     * replaced. Date is formatted according to current device date format.
     *
     * @param date date to replace at the argument
     * @return same formatter
     */
    public Formatter arg(Date date)
    {
        Calendar cal = Calendar.getInstance();
        cal.setTime(date);
        iArgs.addElement(cal);
        return this;
    }

    /**
     * Convert the current pattern to string, along with parameter
     * replacements.
     *
     * @return string where parameters are replaced
     */
    public String toString()
    {
        // Fetch localized text from iResourceLoader.
        String result = iResourceLoader.string(iTextId, iArgN);
        // Replace text parameters.
        Object[] args = new Object[iArgs.size()];
        for (int i = 0; i < iArgs.size(); i++)
        {
            args[i] = iArgs.elementAt(i);
        }
        result = _formatParameters(result, args);
        // Reset for next usage.
        iArgN = -1;
        iArgs.removeAllElements();
        return result;
    }

    /**
     * Applies conversion from european digits into arabic-indic digits
     * based on existing language settings
     *
     * @param str String which might contain european digits
     * @return A string identical with the provided string but with the
     *         european digits (if any) converted to arabic-indic digits
     */
    public static String formatDigits(String str)
    {
        return str;
    }

    /*** ----------------------------- PROTECTED -------------------------- */

    /**
     * Default constructor.
     */
    protected FormatterQt()
    {
        this(null, "");
    }

    /**
     * Create a new formatter
     *
     * @param aResourceLoader ResourceLoader to be used with this Formatter
     * @param aTextId text id
     */
    protected FormatterQt(ResourceLoaderQt aResourceLoader, String aTextId)
    {
        iResourceLoader = aResourceLoader;
        iTextId = aTextId;
    }

    /*** ----------------------------- PRIVATE ---------------------------- */

    /*** ----------------------------- NATIVE ----------------------------- */

    /**
     * Formats given parameters to given text.
     *
     * @param aText text to be formatted
     * @param aArgs text parameters
     * @return formatted text
     */
    private static native String _formatParameters(String aText, Object[] aArgs);

    /**
     * Applies conversion from european digits into arabic-indic digits
     * based on existing language settings
     *
     * @param str String which might contain european digits
     * @return A string identical with the provided string but with the
     *         european digits (if any) converted to arabic-indic digits
     */
    private static native String _formatDigits(String str);
}
