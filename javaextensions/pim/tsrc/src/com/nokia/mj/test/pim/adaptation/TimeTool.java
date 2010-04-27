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


//PACKAGE
package com.nokia.mj.test.pim.adaptation;
//IMPORTS
import java.io.IOException;
import java.util.*;

// CLASS DEFINITION
/**
 */
public class TimeTool
{
    private static long iMillis2000 = 946677600000L;
    private static long[] daysInMonth = { 31L, 28L, 31L, 30L, 31L, 30L, 31L, 31L, 30L, 31L, 30L, 31L };

    // datestamp must be in format: YYMMDD:HHMMSS.sss, in which first part
    // must be present. If millisecond part is missing it will be 0.
    // If also the HHMMSS part is missing it will also be 0.
    //
    // Note: years begin from 00 (==2000)
    // Note: months begin from 01
    // Note: days begin from 01
    public static long getTimeInMillis(String datestamp)
    {
        long years, months, days, hours, minutes, seconds, millis;

        // parse milliseconds
        int pIndex = datestamp.indexOf('.');
        if (pIndex < 0)
        {
            millis = 0;
        }
        else
        {
            millis = Long.parseLong(datestamp.substring(pIndex + 1));
        }

        // parse hours, minutes and seconds
        int cIndex = datestamp.indexOf(':');
        if (cIndex < 0)
        {
            millis = 0;
            seconds = 0;
            minutes = 0;
            hours = 0;
        }
        else
        {
            hours = Long.parseLong(datestamp.substring(cIndex + 1, cIndex + 1 + 2));
            minutes = Long.parseLong(datestamp.substring(cIndex + 1 + 2, cIndex + 1 + 4));
            seconds = Long.parseLong(datestamp.substring(cIndex + 1 + 4, cIndex + 1 + 6));
        }

        // parse years, months and days
        years = Long.parseLong(datestamp.substring(0, 2));
        months = Long.parseLong(datestamp.substring(2, 4));
        days = Long.parseLong(datestamp.substring(4, 6));

        // then just simple calculations

        long value = iMillis2000;

        // add millis
        value += millis;

        // add seconds
        value += seconds * 1000L;

        // add minutes
        value += minutes * 60 * 1000L;

        // add hours
        value += hours * 60 * 60 * 1000L;

        // add days
        value += (days-1) * 24 * 60 * 60 * 1000L;

        // add months
        long tmp = 0;
        for (int i=0; i < months-1; i++)
        {
            tmp += daysInMonth[i];
        }
        value += tmp * 24 * 60 * 60 * 1000L;

        // add years
        value += years * 365 * 24 * 60 * 60 * 1000L;

        // add leapdays
        value += (years/4) * (24 * 60* 60 * 1000L); // add 1 day after each 4 years

        // 2000 is a leap year, so add 1 day in any year if february is passed
        if (months > 2)
        {
            value += (24 * 60* 60 * 1000L);
        }

        // that's it folks!
        return value;
    }

    public static long[] getTimeInMillis(String aDates[])
    {
        long[] values = new long[aDates.length];

        for (int i=0; i < aDates.length; i++)
        {
            values[i] = getTimeInMillis(aDates[i]);
        }
        return values;
    }

    public static void test()
    {
        String[] dates = { "000101:000000.000", "000101:000000", "000101",
                           "010101:235959.999", "010102:000000.000",
                           "010131:080000", "010201:080000",
                           "000229:235959.999", "000301:000000.000",
                           "040101:000000", "040201:000000"
                         };

        System.out.println("TimeTool TEST:");

        for (int i=0; i < dates.length; i++)
        {
            System.out.println("Time "+dates[i]+" as long= "+getTimeInMillis(dates[i]));
        }

    }
}