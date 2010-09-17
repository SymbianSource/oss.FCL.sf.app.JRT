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

import java.util.Date;

/**
 * Utility for formatting text strings from a pattern with positional
 * parameters.
 * <br>
 * Supported parameters are:
 * <UL>
 * <LI>%nU - String in position n
 * <LI>%U - Next string
 * <LI>%Ln - Integer in position n
 * <LI>%nN - Integer in position n
 * <LI>%N - Next integer
 * <LI>%nC - Character in position n
 * <LI>%C - Next character
 * <LI>%n - Parameter in position n
 * </UL>
 * <br>
 * Example of usage:
 * <pre>
 *   Formatter formatter = new Formatter("You have %N email(s) left");
 *   String message = formatter.arg(5).toString();
 *   // Becomes :You have 5 email(s) left
 *
 *   Formatter formatter = new Formatter("There are %N0 files in folder \"%U1\"");
 *   String message = formatter.arg(5).arg("photos").toString();
 *   // Becomes :There are 5 files in folder "photos"
 * </pre>
 * <br>
 * Limitation: more than 10 positional arguments are not supported (only 0...9)
 * <br>
 * Note that Formatter supports Avkon and Qt based localisation.
 * Text parameter indices start from 0 when Avkon is used and from 1
 * when Qt is used.
 */
public class Formatter
{
    /** Original pattern string */
    private String pattern;

    /** String with latest replacements */
    private String replaced;

    /**
     * Platform localisation type.
     * Either ResourceLoader.AVKON or ResourceLoader.QT. */
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
     * Create a new formatter
     *
     * @param pattern formatter pattern
     */
    Formatter(String aPattern)
    {
        this(aPattern, ResourceLoader.AVKON);
    }

    /**
     * Create a new formatter
     *
     * @param pattern formatter pattern
     * @param aLocType platform localisation type
     */
    Formatter(String aPattern, int aLocType)
    {
        pattern = aPattern;
        replaced = aPattern;
        locType = aLocType;
        startIndex = (locType == ResourceLoader.QT? 1: 0);
        nextIndex = startIndex;
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
                        "String replacement failed on parameter " +
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
                        "Integer replacement failed on parameter " +
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
                        "Character replacement failed on parameter " +
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
     * Replace the least numbered parameter in the string, which is not yet
     * replaced.
     *
     * @param o object whose toString() will be used for argument position
     * @return same formatter
     */
    public Formatter arg(Object o)
    {
        if (o != null)
        {
            if (o instanceof String)
            {
                return arg((String) o);
            }
            else if (o instanceof Integer)
            {
                return arg(((Integer) o).intValue());
            }
            else if (o instanceof Character)
            {
                return arg(((Character) o).charValue());
            }
            else if (o instanceof Date)
            {
                return arg((Date) o);
            }
            // Skip not supported types.
        }
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
        String result = replaced;

        // Reset for next usage
        replaced = pattern;
        nextIndex = startIndex;

        return result;
    }

    /**
     * Formats localised text with specified parameters from an array.
     *
     * Note that the arg().arg().toString() is preferred method of
     * usage. E.g. Date format type can not be defined with this method.
     *
     * @param textParameters parameters to be filled into the text
     * @return localised text formatted with the provided parameters
     */
    public String format(Object[] textParameters)
    {
        if (textParameters != null)
        {
            for (int i = 0; i < textParameters.length; i++)
            {
                if (textParameters[i] instanceof String)
                {
                    arg((String)textParameters[i]);
                }
                else if (textParameters[i] instanceof Integer)
                {
                    arg(((Integer)textParameters[i]).intValue());
                }
                else if (textParameters[i] instanceof Date)
                {
                    arg((Date)textParameters[i]);
                }
                else
                {
                    arg(textParameters[i]);
                }
            }
        }
        return toString();
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
                Logger.WLOG(Logger.EUtils, "Replace with max failed to invalid"
                            + " replacement amount");
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
    private native String _formatInteger(int number);

    /**
     * Format date to current locale.
     *
     * @param timeInMilliSecs to be formatted.
     * @param format Date format.
     * @return date formatted as current locale String.
     *
     */
    private native String _formatDate(long timeInMilliSecs);

    /**
     * Applies convertion from european digits into arabic-indic digits
     * based on existing language settings
     *
     * @param str String which might contain european digits
     * @return A string identical with the provided string but with the
     *         european digits (if any) converted to arabic-indic digits
     */
    private static native String _formatDigits(String str);
}
