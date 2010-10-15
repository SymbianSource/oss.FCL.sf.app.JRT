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

import java.util.Date;

/**
 * Formatter for Avkon based platform.
 * @see Formatter
 */
public class FormatterAvkon extends Formatter
{
    /** Original pattern string */
    private String pattern;

    /** String with latest replacements */
    private String replaced;

    /**
     * Platform localisation type.
     * Either ResourceLoader.AVKON or ResourceLoader.QT.
     */
    private final int locType;

    /**
     * The first text parameter replacement index. For Avkon based
     * localisation this is 0, for Qt based localisation this is 1.
     */
    private final int startIndex;

    /** Next replacement index */
    private int nextIndex;

    /*** ----------------------------- PUBLIC ------------------------------ */

    /**
     * Replace the lowest numbered parameter in the string, which is not yet
     * replaced.
     *
     * @param string string to replace at the argument
     * @return same formatter
     */
    public Formatter arg(String string)
    {
        // Try to replace with patterns %[N...N]n, %nU, %n, %U
        String maxPattern = findMaxPattern();
        if ((maxPattern != null && replace(maxPattern, string)) ||
                replace("%" + nextIndex + "U", string) ||
                replace("%" + nextIndex, string) ||
                replace("%U", string))
        {
            nextIndex++;
        }
        else
        {
            Logger.WLOG(Logger.EUtils,
                        "FormatterAvkon: String replacement failed on parameter " +
                        nextIndex + ": " + pattern);
        }
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
        String localisedNumber = _formatInteger(number);

        // Try to replace with patterns %[N...N]n, %Ln, %nN, %n, %N
        String maxPattern = findMaxPattern();
        if ((maxPattern != null && replace(maxPattern, localisedNumber)) ||
                replace("%" + "L" + nextIndex, localisedNumber) ||
                replace("%" + nextIndex + "N", localisedNumber) ||
                replace("%" + nextIndex, localisedNumber) ||
                replace("%N", localisedNumber))
        {
            nextIndex++;

        }
        else
        {
            Logger.WLOG(Logger.EUtils,
                        "FormatterAvkon: Integer replacement failed on parameter " +
                        nextIndex + ": " + pattern);
        }
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
        String chString = new String(new char[] { ch });

        // Try to replace with patterns %nC, %n, %C
        if (replace("%" + nextIndex + "C", chString) ||
                replace("%" + nextIndex, chString) ||
                replace("%C", chString))
        {
            nextIndex++;

        }
        else
        {
            Logger.WLOG(Logger.EUtils,
                        "FormatterAvkon: Character replacement failed on parameter " +
                        nextIndex + ": " + pattern);
        }
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
        String dateString = _formatDate(date.getTime());
        return arg(dateString);
    }

    /**
     * Convert the current pattern to string, along with parameter
     * replacements.
     *
     * @return string where parameters are replaced
     */
    public String toString()
    {
        String result = replaced;

        // Reset for next usage
        replaced = pattern;
        nextIndex = startIndex;

        return result;
    }

    /**
     * Applies convertion from european digits into arabic-indic digits
     * based on existing language settings
     *
     * @param str String which might contain european digits
     * @return A string identical with the provided string but with the
     *         european digits (if any) converted to arabic-indic digits
     */
    public static String formatDigits(String str)
    {
        return _formatDigits(str);
    }

    /*** ----------------------------- PROTECTED -------------------------- */

    /**
     * Default constructor.
     */
    protected FormatterAvkon()
    {
        this("");
    }

    /**
     * Create a new formatter
     *
     * @param pattern formatter pattern
     */
    protected FormatterAvkon(String aPattern)
    {
        this(aPattern, ResourceLoader.AVKON);
    }

    /**
     * Create a new formatter
     *
     * @param pattern formatter pattern
     * @param aLocType platform localisation type
     */
    protected FormatterAvkon(String aPattern, int aLocType)
    {
        pattern = aPattern;
        replaced = aPattern;
        locType = aLocType;
        startIndex = (locType == ResourceLoader.QT? 1: 0);
        nextIndex = startIndex;
    }

    /*** ----------------------------- PRIVATE ---------------------------- */

    /**
     * Replace first occurrence of the string pattern in the replaced field.
     *
     * @param pattern string to search for
     * @param replacement string to replace patterns
     * @return true if pattern was found and replaced, false if pattern was
     *         not found
     */
    private boolean replace(String pattern, String replacement)
    {
        int index = replaced.indexOf(pattern);
        if (index != -1)
        {
            if (replaced.indexOf(pattern + "[]") != -1)
            {
                replaced =
                    replaced.substring(0, index) + replacement +
                    replaced.substring(index + pattern.length() + 2);
            }
            else if (replaced.indexOf(pattern + "[") != -1)
            {
                return replaceWithMax(pattern, replacement, index);
            }
            else
            {
                replaced =
                    replaced.substring(0, index) + replacement +
                    replaced.substring(index + pattern.length());
            }
            return true;
        }
        return false;
    }

    /**
     * Replace first occurrence of the string pattern in the replaced field.
     * Replace [N...N] defined amount of characters.
     *
     * @param pattern string to search for
     * @param replacement string to replace patterns
     * @param index of replacement tag.
     * @return true if pattern was found and replaced, false if pattern was
     *         not found
     */
    private boolean replaceWithMax(String pattern, String replacement, int maxIndex)
    {
        boolean result = false;
        int closingIndex = replaced.indexOf("]", maxIndex + pattern.length());

        // Check format [N...N] comply. If not skip.
        if (closingIndex > 0)
        {
            try
            {
                int maxLen = Integer.parseInt(replaced.substring(
                                                  maxIndex + pattern.length() + 1, closingIndex));

                if (maxLen > replacement.length())
                {
                    maxLen = replacement.length();
                }

                replaced = replaced.substring(0, maxIndex) +
                           replacement.substring(0, maxLen) +
                           replaced.substring(closingIndex + 1);
                result = true;
            }
            catch (NumberFormatException nfe)
            {
                Logger.WLOG(Logger.EUtils,
                            "FormatterAvkon: Replace with max failed to " +
                            "invalid replacement amount");
            }
        }
        return result;
    }

    /**
     * Finds next %[N...N]n pattern from the replaced field.
     * Returns found pattern, or null if no pattern was found.
     */
    private String findMaxPattern()
    {
        String result = null;
        String startPattern = "%[";
        String endPattern = "]" + nextIndex;
        int startIndex = replaced.indexOf(startPattern);
        if (startIndex >= 0)
        {
            int endIndex = replaced.indexOf(endPattern, startIndex);
            if (endIndex >= 0)
            {
                result = replaced.substring(
                    startIndex, endIndex + endPattern.length());
            }
        }
        return result;
    }

    /*** ----------------------------- NATIVE ----------------------------- */

    /**
     * Format integer to current locale.
     *
     * @param number to be formatted.
     * @return number formatted as current locale String.
     */
    private static native String _formatInteger(int number);

    /**
     * Format date to current locale.
     *
     * @param timeInMilliSecs to be formatted.
     * @param format Date format.
     * @return date formatted as current locale String.
     *
     */
    private static native String _formatDate(long timeInMilliSecs);

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
