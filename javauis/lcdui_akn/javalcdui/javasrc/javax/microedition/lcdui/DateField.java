/*
* Copyright (c) 1999 - 2004 Nokia Corporation and/or its subsidiary(-ies).
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


package javax.microedition.lcdui;

import java.util.Calendar;
import java.util.Date;
import java.util.TimeZone;
import java.util.Vector;

import com.nokia.mj.impl.rt.legacy.NativeError;

public class DateField extends Item
{
    public static final int DATE = 1;
    public static final int TIME = 2;
    public static final int DATE_TIME = 3;
    //
    private static final long NULL_DATE = -62168256000000L;
    private static final int JAVA_EPOCH_YEAR = 1970;
    //
    private Calendar iCalendar;
    private Calendar iDefaultCal;
    private Date iDateWrapper;
    private int iMode;
    private boolean iInitialized;

    public DateField(String aLabel,int aMode)
    {
        this(aLabel,aMode,null);
    }

    public DateField(String aLabel,int aMode,TimeZone aTimeZone)
    {
        if (aMode < DATE || aMode > DATE_TIME)
            throw new IllegalArgumentException();
        //
        iMode = aMode;
        iLabel = aLabel;
        if (aTimeZone == null)
            aTimeZone = TimeZone.getDefault();
        iCalendar = Calendar.getInstance(aTimeZone);
        iDefaultCal = Calendar.getInstance(TimeZone.getTimeZone("GMT"));
        iDateWrapper = new Date();
        synchronized (iToolkit)
        {
            iHandle = Toolkit.checkHandle(_create(iToolkit.getHandle(),this,aLabel == null?"":aLabel,aMode));
        }
    }

    public Date getDate()
    {
        synchronized (iToolkit)
        {
            if (!iInitialized)
            {
                return null;
            }

            final long currentDate = _getDate(iToolkit.getHandle(), getHandle());
            iDateWrapper.setTime(currentDate);
            iDefaultCal.setTime(iDateWrapper);

            int year = iDefaultCal.get(Calendar.YEAR);
            int month = iDefaultCal.get(Calendar.MONTH);
            int day = iDefaultCal.get(Calendar.DAY_OF_MONTH);
            int hour = iDefaultCal.get(Calendar.HOUR_OF_DAY);
            int minute = iDefaultCal.get(Calendar.MINUTE);

            switch (iMode)
            {
            case DATE:
            {
                hour = 0;
                minute = 0;
                break;
            }
            case TIME:
            {
                year = JAVA_EPOCH_YEAR;
                month = Calendar.JANUARY;
                day = 1;
                break;
            }
            default:
                // Do nothing. This applies to DATE_TIME
            }

            //convert to local time
            iCalendar.set(Calendar.YEAR,year);
            iCalendar.set(Calendar.MONTH,month);
            iCalendar.set(Calendar.DAY_OF_MONTH,day);
            iCalendar.set(Calendar.HOUR_OF_DAY,hour);
            iCalendar.set(Calendar.MINUTE,minute);
            iCalendar.set(Calendar.SECOND,0);
            iCalendar.set(Calendar.MILLISECOND,0);
            return iCalendar.getTime();
        }
    }

    public void setDate(Date aDate)
    {
        synchronized (iToolkit)
        {
            long date = NULL_DATE;
            if (aDate != null)
            {
                iCalendar.setTime(aDate);

                if (iMode != TIME)
                {
                    // convert to local date\time
                    iDefaultCal.set(Calendar.YEAR, iCalendar.get(Calendar.YEAR));
                    iDefaultCal.set(Calendar.MONTH, iCalendar.get(Calendar.MONTH));
                    iDefaultCal.set(Calendar.DAY_OF_MONTH, iCalendar.get(Calendar.DAY_OF_MONTH));
                    iDefaultCal.set(Calendar.HOUR, iCalendar.get(Calendar.HOUR));
                    iDefaultCal.set(Calendar.MINUTE, iCalendar.get(Calendar.MINUTE));
                    iDefaultCal.set(Calendar.AM_PM, iCalendar.get(Calendar.AM_PM));
                    date = iDefaultCal.getTime().getTime();
                }
                else
                {
                    iDefaultCal.setTime(aDate);
                    final int year  = iDefaultCal.get(Calendar.YEAR);
                    final int month = iDefaultCal.get(Calendar.MONTH);
                    final int day   = iDefaultCal.get(Calendar.DAY_OF_MONTH);

                    // Date components must be set to the "zero epoch" value of January 1, 1970.
                    // Otherwise must be in the "not initialized" state.
                    if (iMode == TIME && year == JAVA_EPOCH_YEAR && month == Calendar.JANUARY && day == 1)
                    {
                        // convert to local time
                        iDefaultCal.set(Calendar.HOUR,iCalendar.get(Calendar.HOUR));
                        iDefaultCal.set(Calendar.MINUTE,iCalendar.get(Calendar.MINUTE));
                        iDefaultCal.set(Calendar.AM_PM,iCalendar.get(Calendar.AM_PM));
                        date = iDefaultCal.getTime().getTime();
                    }
                }
            }

            iInitialized = (date != NULL_DATE);
            if (iInitialized)
            {
                _setDate(iToolkit.getHandle(),getHandle(),date);
            }
            else
            {
                _setUninitialized(iToolkit.getHandle(),getHandle());
            }
            RefreshForm();
        }
    }

    public int getInputMode()
    {
        return iMode;
    }

    public void setInputMode(int aMode)
    {
        if (aMode < DATE || aMode > DATE_TIME)
        {
            throw new IllegalArgumentException();
        }
        synchronized (iToolkit)
        {
            if (iMode == aMode)
            {
                return;
            }

            NativeError.check(_setInputMode(iToolkit.getHandle(), getHandle(), aMode));
            iMode = aMode;

            // notify form that peer handle has changed.
            UpdateForm();
        }
    }

    final void setDateInitialized(boolean aBoolean)
    {
        iInitialized = aBoolean;
    }

    private native int _create(int aToolkit,DateField aDateField,String aLabel,int aFieldType);
    private native void _setDate(int aToolkit, int aItem, long aDate);
    private native int  _setInputMode(int aToolkit, int aItem, int aMode);
    private native long _getDate(int aToolkit, int aItem);
    private native void _setUninitialized(int aToolkit,int aItem);
}
