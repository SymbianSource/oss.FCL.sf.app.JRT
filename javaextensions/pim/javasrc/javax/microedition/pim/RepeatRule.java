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
* Description:  Repeating information associated with Event items.
 *
*/


// PACKAGE
package javax.microedition.pim;

// IMPORTS
import com.nokia.mj.impl.pim.ErrorString;
import com.nokia.mj.impl.pim.GenericException;
import com.nokia.mj.impl.pim.PIMManager;
import com.nokia.mj.impl.pim.PIMTools;
import com.nokia.mj.impl.rt.support.ApplicationUtils;
import com.nokia.mj.impl.rt.support.Finalizer;
import com.nokia.mj.impl.rt.support.ShutdownListener;
import com.nokia.mj.impl.pim.utils.NativeError;
import java.util.Enumeration;
import java.util.Date;
import java.util.Calendar;
import java.util.Vector;

// CLASS DEFINITION
/**
 * PIM API 1.0 compliant RepeatRule. The class is concrete and can be created
 * with its own constructor (in contrast to all other classes which are created
 * through factory methods) so it must be implemented in the this
 * (javax.microedition.pim) package.
 *
 * Objects of the RepeatRule class are created in different contexts, which
 * affects the way how its native side counterpart is created.
 *
 * @li A MIDlet programmer can create a RepeatRule at any time and access its
 *     contents. RepeatRule creates its own native side peer object which, like
 *     the Java side object, is not associated with an Event item (and never
 *     will).
 * @li When an Event item is created, an associated RepeatRule is created, too.
 *     The native side peer object for the RepeatRule is created before the Java
 *     side object and the RepeatRule is initialized with a reference to that
 *     native side object. A MIDlet programmer can never have a reference to a
 *     RepeatRule owned by an Event item; only copies of it are provided
 *     outside.
 *
 * MIDlet programmer might derive one's own classes from RepeatRule. Minimal
 * nonpublic (PIM API defined) members are left protected; they are either
 * private or package private instead.
 *
 * RepeatRule methods need to be synchronized, but we cannot use the
 * "synchronized" method modifier because of the PIM TCK signature test.
 * Synchronized block statement is used instead.
 */
public class RepeatRule
{
    // Constants

    public static final int APRIL = 1048576;

    public static final int AUGUST = 16777216;

    public static final int COUNT = 32;

    public static final int DAILY = 16;

    public static final int DAY_IN_MONTH = 1;

    public static final int DAY_IN_WEEK = 2;

    public static final int DAY_IN_YEAR = 4;

    public static final int DECEMBER = 268435456;

    public static final int END = 64;

    public static final int FEBRUARY = 262144;

    public static final int FIFTH = 16;

    public static final int FIFTHLAST = 512;

    public static final int FIRST = 1;

    public static final int FOURTH = 8;

    public static final int FOURTHLAST = 256;

    public static final int FREQUENCY = 0;

    public static final int FRIDAY = 2048;

    public static final int INTERVAL = 128;

    public static final int JANUARY = 131072;

    public static final int JULY = 8388608;

    public static final int JUNE = 4194304;

    public static final int LAST = 32;

    public static final int MARCH = 524288;

    public static final int MAY = 2097152;

    public static final int MONDAY = 32768;

    public static final int MONTH_IN_YEAR = 8;

    public static final int MONTHLY = 18;

    public static final int NOVEMBER = 134217728;

    public static final int OCTOBER = 67108864;

    public static final int SATURDAY = 1024;

    public static final int SECOND = 2;

    public static final int SECONDLAST = 64;

    public static final int SEPTEMBER = 33554432;

    public static final int SUNDAY = 65536;

    public static final int THIRD = 4;

    public static final int THIRDLAST = 128;

    public static final int THURSDAY = 4096;

    public static final int TUESDAY = 16384;

    public static final int WEDNESDAY = 8192;

    public static final int WEEK_IN_MONTH = 16;

    public static final int WEEKLY = 17;

    public static final int YEARLY = 19;


    // Member data

    private Finalizer iFinalizer;

    /** Handle to the native side Repeat Rule. */
    private int iRepeatRuleHandle;

    // Constructors and destructor

    /**
     * Default constructor, part of PIM API.
     */
    public RepeatRule()
    {
        PIMManager.getInstance();
        int[] error = new int[1];
        iRepeatRuleHandle = _createRepeatRule(error);
        if (!NativeError.checkSuccess(error[0]))
        {
            throw new GenericException(ErrorString.GENERAL_ERROR_COLON + error[0]);
        }
        setShutdownListener();
        iFinalizer = registerForFinalization();
    }

    /**
     * Package private constructor which can be used for creating a RepeatRule
     * for an existing native side peer object.
     */
    RepeatRule(int aRepeatRulehandle)
    {
        iRepeatRuleHandle = aRepeatRulehandle;
        setShutdownListener();
        iFinalizer = registerForFinalization();
    }

    Finalizer registerForFinalization()
    {
        return new Finalizer()
        {
            public void finalizeImpl()
            {

                doFinalize();

            }
        };

    }

    void doFinalize()
    {
        if (iFinalizer == null)
        {
            return;
        }
        iFinalizer = null;

        if (iRepeatRuleHandle != 0)
        {
            _dispose(iRepeatRuleHandle);
            iRepeatRuleHandle = 0;
        }
    }

    /**
     * Registers for shutdown listener
     */
    private void setShutdownListener()
    {
        // Get the insatnce of ApplicationUtils.
        ApplicationUtils appUtils = ApplicationUtils.getInstance();

        // Get the name of the application.
        appUtils.addShutdownListener(new ShutdownListener()
        {
            public void shuttingDown()
            {

                if (iRepeatRuleHandle != 0)
                {
                    _dispose(iRepeatRuleHandle);
                    iRepeatRuleHandle = 0;
                }
            }

        });
    }

    // New methods

    public Enumeration dates(long aStartDate, long aSubsetBeginning,
                             long aSubsetEnding)
    {
        synchronized (this)
        {
            if (aSubsetBeginning > aSubsetEnding)
            {
                throw new IllegalArgumentException(
                    "Start date is greater than end date.");
            }

            int[] repeatDates = _dates(
                                    iRepeatRuleHandle, aStartDate, aSubsetBeginning, aSubsetEnding);

            if (repeatDates == null)
            {
                throw new OutOfMemoryError();
            }

            long[] repeatDatesLong = PIMTools.intArrayToLongArray(repeatDates);

            return longArrayDatesToEnumeration(repeatDatesLong);
        }
    }

    public void addExceptDate(long aValue)
    {
        synchronized (this)
        {
            _addExceptDate(iRepeatRuleHandle, aValue);
        }
    }

    public void removeExceptDate(long aValue)
    {
        synchronized (this)
        {
            _removeExceptDate(iRepeatRuleHandle, aValue);
        }
    }

    public Enumeration getExceptDates()
    {
        synchronized (this)
        {
            int[] exceptDates = _getExceptDates(iRepeatRuleHandle);

            if (exceptDates == null)
            {
                throw new OutOfMemoryError();
            }

            long[] exceptDatesLong = PIMTools.intArrayToLongArray(exceptDates);

            return longArrayDatesToEnumeration(exceptDatesLong);
        }
    }

    public int getInt(int aField)
    {
        synchronized (this)
        {
            int[] error = new int[1];

            int value = _getInt(iRepeatRuleHandle, aField, error);

            NativeError.handleRRFieldHandlingError(error[0], aField);

            return value;
        }
    }

    public void setInt(int aField, int aValue)
    {
        synchronized (this)
        {
            // First check that the field and value are valid
            switch (aField)
            {

            case FREQUENCY:
            {
                if (aValue != DAILY && aValue != WEEKLY && aValue != MONTHLY
                        && aValue != YEARLY)
                {
                    throw new IllegalArgumentException(
                        "Invalid value for field FREQUENCY.");
                }
                break;
            }

            case INTERVAL:
            {
                if (aValue < 1)
                {
                    throw new IllegalArgumentException(
                        "Invalid value for field INTERVAL.");
                }
                break;
            }

            case DAY_IN_MONTH:
            {
                if (aValue < 1 || aValue > 31)
                {
                    throw new IllegalArgumentException(
                        "Invalid value for field DAY_IN_MONTH.");
                }
                break;
            }

            case DAY_IN_WEEK:
            {
                if ((aValue & SUNDAY) == 0 && (aValue & MONDAY) == 0
                        && (aValue & TUESDAY) == 0 && (aValue & WEDNESDAY) == 0
                        && (aValue & THURSDAY) == 0 && (aValue & FRIDAY) == 0
                        && (aValue & SATURDAY) == 0)
                {
                    throw new IllegalArgumentException(
                        "Invalid value for field DAY_IN_WEEK.");
                }
                break;
            }

            case DAY_IN_YEAR:
            {
                if (aValue < 1 || aValue > 366)
                {
                    throw new IllegalArgumentException(
                        "Invalid value for field DAY_IN_YEAR.");
                }
                break;
            }

            case MONTH_IN_YEAR:
            {
                if ((aValue & JANUARY) == 0 && (aValue & FEBRUARY) == 0
                        && (aValue & MARCH) == 0 && (aValue & APRIL) == 0
                        && (aValue & MAY) == 0 && (aValue & JUNE) == 0
                        && (aValue & JULY) == 0 && (aValue & AUGUST) == 0
                        && (aValue & SEPTEMBER) == 0 && (aValue & OCTOBER) == 0
                        && (aValue & NOVEMBER) == 0 && (aValue & DECEMBER) == 0)
                {
                    throw new IllegalArgumentException(
                        "Invalid value for field MONTH_IN_YEAR.");
                }
                break;
            }

            case WEEK_IN_MONTH:
            {
                if ((aValue & FIRST) == 0 && (aValue & SECOND) == 0
                        && (aValue & THIRD) == 0 && (aValue & FOURTH) == 0
                        && (aValue & FIFTH) == 0 && (aValue & LAST) == 0
                        && (aValue & SECONDLAST) == 0
                        && (aValue & THIRDLAST) == 0
                        && (aValue & FOURTHLAST) == 0
                        && (aValue & FIFTHLAST) == 0)
                {
                    throw new IllegalArgumentException(
                        "Invalid value for field WEEK_IN_MONTH.");
                }
                break;
            }
            case COUNT:
            {
                if (aValue < 0)
                {
                    throw new IllegalArgumentException(
                        "Invalid value for field COUNT.");
                }
                break;
            }
            default:
            {
                throw new IllegalArgumentException(ErrorString.INVALID_FIELD_COLON
                                                   + aField);
            }
            }

            int[] error = new int[1];

            _setInt(iRepeatRuleHandle, aField, aValue, error);

            NativeError.handleRRFieldHandlingError(error[0], aField);
        }
    }

    public long getDate(int aField)
    {
        synchronized (this)
        {
            int[] error = new int[1];

            long value = _getDate(iRepeatRuleHandle, aField, error);

            NativeError.handleRRFieldHandlingError(error[0], aField);

            return value;
        }
    }

    public void setDate(int aField, long aValue)
    {
        synchronized (this)
        {
            int[] error = new int[1];

            _setDate(iRepeatRuleHandle, aField, aValue, error);

            NativeError.handleRRFieldHandlingError(error[0], aField);
        }
    }

    public int[] getFields()
    {
        synchronized (this)
        {
            int[] fields = _getFields(iRepeatRuleHandle);

            if (fields == null)
            {
                throw new OutOfMemoryError();
            }

            return fields;
        }
    }

    public boolean equals(Object aObj)
    {
        synchronized (this)
        {
            boolean retVal = false;

            if (aObj == null)
            {
                retVal = false;
            }

            else if (aObj instanceof RepeatRule)
            {
                RepeatRule compareRule = (RepeatRule) aObj;

                if (compareRepeatRuleFields(compareRule)
                        && compareExceptionDates(compareRule))
                {
                    retVal = true;
                }
            }

            return retVal;
        }
    }

    private boolean compareRepeatRuleFields(RepeatRule aRepeatRule)
    {
        int[] compareFields = aRepeatRule.getFields();
        int[] fields = getFields();

        if (fields.length != compareFields.length)
        {
            return false;
        }
        else
        {
            for (int i = 0; i < fields.length; i++)
            {
                if (fields[i] != compareFields[i])
                {
                    return false;
                }
                else
                {
                    if (fields[i] == END)
                    {
                        Date endDate = new Date(getDate(END));
                        Date endDateCompare = new Date(aRepeatRule.getDate(END));
                        if (!resetTimePortion(endDate).equals(
                                    resetTimePortion(endDateCompare)))
                        {
                            return false;
                        }
                    }
                    else
                    {
                        if (getInt(fields[i]) != aRepeatRule.getInt(fields[i]))
                        {
                            return false;
                        }
                    }
                } // else
            } // for
        } // else

        // If the comparisons were passed without returning false, return true
        return true;
    }

    private boolean compareExceptionDates(RepeatRule aRepeatRule)
    {
        Enumeration compareExceptDates = aRepeatRule.getExceptDates();
        Enumeration exceptDates = getExceptDates();

        Vector exceptDatesVector = new Vector();

        while (exceptDates.hasMoreElements())
        {
            exceptDatesVector.addElement(resetTimePortion((Date) exceptDates
                                         .nextElement()));
        }

        int searchIndex = -1;

        while (compareExceptDates.hasMoreElements())
        {
            searchIndex = exceptDatesVector
                          .indexOf(resetTimePortion((Date) compareExceptDates
                                                    .nextElement()));
            if (searchIndex == -1)
            {
                return false;
            }
            else
            {
                exceptDatesVector.removeElementAt(searchIndex);
            }
        }

        // Check if the exception Vector still has exceptions left
        if (exceptDatesVector.size() > 0)
        {
            return false;
        }

        // If the comparisons were passed without returning false, return true
        return true;
    }

    private Enumeration longArrayDatesToEnumeration(long[] aArray)
    {
        Vector retVal = new Vector(aArray.length);
        for (int i = 0; i < aArray.length; i++)
        {
            retVal.addElement(new Date(aArray[i]));
        }

        return retVal.elements();
    }

    private Date resetTimePortion(Date aDate)
    {
        Calendar cal = Calendar.getInstance();
        cal.setTime(aDate);
        cal.set(Calendar.AM_PM, Calendar.AM);
        cal.set(Calendar.HOUR, 0);
        cal.set(Calendar.MINUTE, 0);
        cal.set(Calendar.SECOND, 0);
        cal.set(Calendar.MILLISECOND, 0);
        return cal.getTime();
    }

    /**
     * Gets the native handle of this object.
     */
    int nativeHandle()
    {
        return iRepeatRuleHandle;
    }

    /**
     * Sets the native peer object, disposing the old peer object.
     */
    void setNativePeer(int aRepeatRuleHandle)
    {
        _dispose(iRepeatRuleHandle);
        iRepeatRuleHandle = aRepeatRuleHandle;
    }

    // Native operations

    private native int _createRepeatRule(int[] aError);

    private native void _dispose(int aRepeatRuleHandle);

    /**
     * Gets all repeat dates without the exception dates..
     */
    private native int[] _dates(int aRepeatRuleHandle, long aStartDate,
                                long aSubsetBeginning, long aSubsetEnding);

    /**
     * Adds the given date to the list of exception dates.
     */
    private native void _addExceptDate(int aRepeatRuleHandle, long aValue);

    /**
     * Removes the given date from the list of exception dates.
     */
    private native void _removeExceptDate(int aRepeatRuleHandle, long aValue);

    /**
     * Gets all exception dates.
     */
    private native int[] _getExceptDates(int aRepeatRuleHandle);

    /**
     * Gets the int value of a given field.
     */
    private native int _getInt(int aRepeatRuleHandle, int aField, int[] aError);

    /**
     * Sets the given int value for the specified field.
     */
    private native void _setInt(int aRepeatRuleHandle, int aField, int aValue,
                                int[] aError);

    /**
     * Gets the date value of a given field.
     */
    private native long _getDate(int aRepeatRuleHandle, int aField, int[] aError);

    /**
     * Sets the given date value for the specified field.
     */
    private native void _setDate(int aRepeatRuleHandle, int aField, long aValue,
                                 int[] aError);

    /**
     * Gets the fields currently in use in an int array.
     */
    private native int[] _getFields(int aRepeatRuleHandle);

}