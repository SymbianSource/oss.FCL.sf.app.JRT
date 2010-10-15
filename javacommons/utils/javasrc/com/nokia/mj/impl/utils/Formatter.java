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
 * Supported parameters in Avkon environment are:
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
 * Supported parameters in Qt environment are:
 * <UL>
 * <LI>%1, %2, %3, ... - for String parameters
 * <LI>%L1, %L2, %L3, ... - for Integer parameters (%1, %2, %3, ... is also ok)
 * <LI>%Ln - for Integers indicating plurality
 * </UL>
 * <br>
 * Text parameter indices start from 0 when Avkon is used and from 1
 * when Qt is used. One text can only contain one %Ln parameter.
 * <br>
 * Example of usage:
 * <pre>
 *   Formatter formatter = resourceLoader.format("You have %N email(s) left");
 *   String message = formatter.arg(5).toString();
 *   // Becomes: You have 5 email(s) left
 *
 *   Formatter formatter = new Formatter("There are %N0 files in folder \"%U1\"");
 *   String message = formatter.arg(5).arg("photos").toString();
 *   // Becomes: There are 5 files in folder "photos"
 * </pre>
 * <br>
 */
abstract public class Formatter
{
    /*** ----------------------------- PUBLIC ------------------------------ */

    /**
     * Set the plurality for this Formatter. Note that this method does
     * nothing when Avkon localisation is used.
     *
     * @param n number indicating plurality
     * @return same formatter
     */
    public Formatter argn(int n)
    {
        return this;
    }

    /**
     * Replace the lowest numbered parameter in the string, which is not yet
     * replaced.
     *
     * @param string string to replace at the argument
     * @return same formatter
     */
    abstract public Formatter arg(String string);

    /**
     * Replace the least numbered parameter in the string, which is not yet
     * replaced.
     *
     * @param number number to replace at the argument
     * @return same formatter
     */
    abstract public Formatter arg(int number);

    /**
     * Replace the least numbered parameter in the string, which is not yet
     * replaced.
     *
     * @param ch character to replace at the argument
     * @return same formatter
     */
    abstract public Formatter arg(char ch);

    /**
     * Replace the least numbered parameter in the string, which is not yet
     * replaced. Date is formatted according to current device date format.
     *
     * @param date date to replace at the argument
     * @return same formatter
     */
    abstract public Formatter arg(Date date);

    /**
     * Replace the least numbered parameter in the string, which is not yet
     * replaced.
     *
     * @param o object which will be used for argument position
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
    abstract public String toString();

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
                arg(textParameters[i]);
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
        if (ResourceLoader.getLocaleIdQt() == null)
        {
            return FormatterAvkon.formatDigits(str);
        }
        else
        {
            return FormatterQt.formatDigits(str);
        }
    }

    /*** ----------------------------- PROTECTED -------------------------- */

    /**
     * Default constructor.
     */
    protected Formatter()
    {
    }

    /*** ----------------------------- PRIVATE ---------------------------- */

    /*** ----------------------------- NATIVE ----------------------------- */
}
