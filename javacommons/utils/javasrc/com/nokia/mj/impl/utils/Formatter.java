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

import java.util.Date;

/**
 * Utility for formatting text strings from a pattern with positional
 * parameters.
 * <br>
 * Supported parameters are:
 * <UL>
 * <LI>%nU - String in position n
 * <LI>%U - Next string
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
 *
 * @author Nokia Corporation
 * @version 1.0
 */
public class Formatter
{
    /** Original pattern string */
    private String pattern;

    /** String with latest replacements */
    private String replaced;

    /** Next replacement index */
    private int nextIndex = 0;

    /*** ----------------------------- PUBLIC ------------------------------ */

    /**
     * Create a new formatter
     *
     * @param pattern formatter pattern
     */
    public Formatter(String aPattern)
    {
        pattern = aPattern;
        replaced = aPattern;
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
        // Try to replace with patterns %nU,%n, %U
        if (replace("%" + nextIndex + "U", string) ||
                replace("%" + nextIndex, string) ||
                replace("%U", string))
        {
            nextIndex++;

        }
        else
        {
            Logger.WLOG(Logger.EUtils, "String replacement failed");
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

        // Try to replace with patterns %nN, %n, %N
        if (replace("%" + nextIndex + "N", localisedNumber) ||
                replace("%" + nextIndex, localisedNumber) ||
                replace("%N", localisedNumber))
        {
            nextIndex++;

        }
        else
        {
            Logger.WLOG(Logger.EUtils, "Integer replacement failed");
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

        // Try to replace with patterns %nC,%n, %C
        if (replace("%" + nextIndex + "C", chString) ||
                replace("%" + nextIndex, chString) ||
                replace("%C", chString))
        {
            nextIndex++;

        }
        else
        {
            Logger.WLOG(Logger.EUtils, "Character replacement failed");
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
        nextIndex = 0;

        return result;
    }

    /**
     * Gets a clone of this formatter. This can be used for caching preparsed
     * Formatters.
     *
     * @return clone of the formatter, as if new Formatter were created with
     * same pattern as current one.
     */
    public Formatter getClone()
    {
        return new Formatter(pattern);
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
     * Replace only [NN] defined amount of characters.
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
        int closingIndex = maxIndex + pattern.length() + 3;

        // Check format [NN] comply. If not skip.
        if (replaced.length() > closingIndex
                && replaced.charAt(closingIndex) == ']')
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
                           replaced.substring(maxIndex + pattern.length() + 4);
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
}
