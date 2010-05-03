/*
* Copyright (c) 2009 Nokia Corporation and/or its subsidiary(-ies).
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
package com.nokia.mj.impl.http;

import com.nokia.mj.impl.utils.Logger;
import java.util.Vector;

public class DateUtil
{
    private static final String[] names = { "jan", "feb", "mar", "apr", "may",
                                            "jun", "jul", "aug", "sep", "oct", "nov", "dec"
                                          };

    private static final long SECS_PER_DAY = 86400;

    private static final long SECS_PER_HOUR = 3600;

    private static final long SECS_PER_MIN = 60;

    private static Vector monthNames = null;

    private int year;

    private int month;

    private int day;

    private int hour;

    private int minute;

    private int second;

    private String weekday;

    static
    {
        try
        {
            monthNames = new Vector(12);
            String t = "sss";
            for (int i = 0; i < 12; i++)
                monthNames.addElement(names[i]);
        }
        catch (Exception e)
        {
        }
    }

    public DateUtil(int y, int m, int d, int hr, int min, int s)
    {
        year = y;
        month = m;
        day = d;
        hour = hr;
        minute = min;
        second = s;
    }

    public DateUtil(String datetime)
    {
        String tmpWeekday = null;
        int tmpYear, tmpDay, tmpMonth;
        String tmpHour, tmpMinute, tmpSecond;
        int start = 0;
        int end = 0;
        String temp;
        int index = 0;
        int colon = 0;
        int begin = 0;

        // parse date in the format : Thu, 14 Aug 2008 09:49:08 GMT"
        // parse for weekday
        end = datetime.indexOf(',');
        if (end == -1)
            weekday = null;
        else
        {
            tmpWeekday = datetime.substring(start, end);
            if (datetime.charAt(end + 1) == ' ')
            {
                start = end + 2;
            }
            else
            {
                start = end + 1;
            }
            weekday = tmpWeekday;
        }

        // parse for day
        Logger.LOG(Logger.ESOCKET, Logger.EInfo, "weekday: " + tmpWeekday);
        end = datetime.indexOf(' ', start);
        if (end == -1)
            throw new IllegalArgumentException("illegal date format");
        temp = datetime.substring(start, end);
        day = Integer.parseInt(temp);
        start = end + 1;

        // parse for month
        end = datetime.indexOf(' ', start);
        if (end == -1)
            throw new IllegalArgumentException("illegal date format");
        temp = datetime.substring(start, end);
        temp = temp.toLowerCase();
        if ((index = monthNames.indexOf(temp)) != -1)
            month = index + 1;
        else
            throw new IllegalArgumentException("wrong date format");
        start = end + 1;

        // parse for year
        end = datetime.indexOf(' ', start);
        if (end == -1)
            throw new IllegalArgumentException("illegal date format");
        temp = datetime.substring(start, end);
        year = Integer.parseInt(temp);
        start = end + 1;

        // parse for time
        end = datetime.indexOf(' ', start);
        if (end == -1)
            throw new IllegalArgumentException("illegal date format");
        temp = datetime.substring(start, end);

        begin = start;
        while (begin < end)
        {
            index = datetime.indexOf(':', begin);
            if (index != -1)
            {
                colon = colon + 1;
                begin = index + 1;
            }
            if (index == -1)
                break;
        }
        if (colon != 2)
            throw new IllegalArgumentException("illegal date format");
        index = datetime.indexOf(':', start);
        tmpHour = datetime.substring(start, index);
        hour = Integer.parseInt(tmpHour);
        start = index + 1;
        index = datetime.indexOf(':', start);
        tmpMinute = datetime.substring(start, index);
        minute = Integer.parseInt(tmpMinute);
        start = index + 1;
        tmpSecond = datetime.substring(start, end);
        second = Integer.parseInt(tmpSecond);

    }

    public static long epocDifference(String s)
    {
        /* return number of milliseconds elapsed from Jan 1 1970, 00:00:00 GMT */
        DateUtil obj1 = new DateUtil(s);
        DateUtil obj2 = new DateUtil(1970, 1, 1, 0, 0, 0);
        long d1 = obj1.juliandayfromdate();
        long d2 = obj2.juliandayfromdate();
        Logger.LOG(Logger.ESOCKET, Logger.EInfo, "d1 = " + d1 + "d2 = " + d2);
        long days = d1 - d2;
        Logger.LOG(Logger.ESOCKET, Logger.EInfo, "days = " + days);
        long secs = obj1.secondsfrom() - obj2.secondsfrom();
        Logger.LOG(Logger.ESOCKET, Logger.EInfo, "secs = " + secs);
        if (days < 0 || (days == 0 && secs < 0))
            return -1;
        long retval = (days * SECS_PER_DAY) + secs;
        return retval * 1000;

    }

    public static long epocDifference(int y, int m, int d, int hr, int min,
                                      int s)
    {
        /* return number of milliseconds elapsed from Jan 1 1970, 00:00:00 GMT */
        DateUtil obj1 = new DateUtil(y, m, d, hr, min, s);
        DateUtil obj2 = new DateUtil(1970, 1, 1, 0, 0, 0);
        long d1 = obj1.juliandayfromdate();
        long d2 = obj2.juliandayfromdate();
        Logger.LOG(Logger.ESOCKET, Logger.EInfo, "d1 = " + d1 + "d2 = " + d2);
        long days = d1 - d2;
        Logger.LOG(Logger.ESOCKET, Logger.EInfo, "days = " + days);
        long secs = obj1.secondsfrom() - obj2.secondsfrom();
        Logger.LOG(Logger.ESOCKET, Logger.EInfo, "secs = " + secs);
        if (days < 0 || (days == 0 && secs < 0))
            return -1;
        long retval = (days * SECS_PER_DAY) + secs;
        return retval * 1000;

    }

    private long juliandayfromdate()
    {
        /* computes the number of days elapsed from 4713BC/01/01 00:00:00 */
        if (year < 0)
            ++year;

        if (year > 1582
                || (year == 1582 && (month > 10 || (month == 10 && day >= 15))))
        {
            // Gregorian calendar starting from October 15, 1582
            // Algorithm from Henry F. Fliegel and Thomas C. Van Flandern
            return (1461 *(year + 4800 + (month - 14) / 12)) / 4
                   + (367 *(month - 2 - 12 *((month - 14) / 12))) / 12
                   - (3 *((year + 4900 + (month - 14) / 12) / 100)) / 4 + day
                   - 32075;
        }
        else if (year < 1582
                 || (year == 1582 && (month < 10 || (month == 10 && day <= 4))))
        {
            // Julian calendar until October 4, 1582
            // Algorithm from Frequently Asked Questions about Calendars by Claus Toendering
            int a = (14 - month) / 12;
            return (153 *(month + (12 * a) - 3) + 2) / 5
                   + (1461 *(year + 4800 - a)) / 4 + day - 32083;
        }
        else
        {
            // the day following October 4, 1582 is October 15, 1582
            return 0;
        }
    }

    private long secondsfrom()
    {
        long sec = SECS_PER_HOUR * hour + SECS_PER_MIN * minute + second;
        return sec;
    }
}
