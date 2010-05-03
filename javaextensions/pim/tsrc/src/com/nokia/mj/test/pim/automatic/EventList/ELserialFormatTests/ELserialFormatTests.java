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


// PACKAGE
package com.nokia.mj.test.pim.automatic.EventList.ELserialFormatTests;
// IMPORTS
import javax.microedition.pim.*;
import java.util.Hashtable;
import java.util.Vector;
import java.util.Calendar;
import java.util.Date;
import java.util.TimeZone;
import com.nokia.mj.test.pim.automatic.*;
import com.nokia.mj.test.pim.adaptation.*;

// CLASS DEFINITION
/**
 * This class initializes the test data for PIM serialization tests
 * Only Event List specific are tested in this class
 *
 * Tested functions:
 * javax.microedition.pim.PIM.toSerialFormat()
 * javax.microedition.pim.PIM.fromSerialFormat()
 *
 *
 */

public class ELserialFormatTests
        extends MIDPTest
{

    public ELserialFormatTests(
        String sTestName, j2meunit.framework.TestMethod rTestMethod)
    {
        super(sTestName, rTestMethod);
    }

    /* PIM List and item */
    private PIMList iPIMList;
    private PIMItem iPIMItem;

    /* Test data including fields, attributes and field values */
    private SupportTestData iTestData = new SupportTestData();

    /**
     * Initializes the test
     */
    public void initTest()
    throws TestEndThrowable
    {
        int synchronization = 200;
        // Add test data. The last value is the result of the versit conversion
        iTestData.addFieldData(Event.SUMMARY,  0, "Summary",       "SUMMARY:Summary");
        iTestData.addFieldData(Event.NOTE,     0, "Description",   "DESCRIPTION:Description");
        iTestData.addFieldData(Event.LOCATION, 0, "Location",      "LOCATION:Location");
        iTestData.addFieldData(Event.CLASS,    0, 200,             "CLASS:CONFIDENTIAL");

        // Start date
        long currentTime = System.currentTimeMillis();
        Date date = new Date(currentTime);
        iTestData.addFieldData(Event.START,
                               0,
                               date.getTime(),
                               "DTSTART:" + creteVersitTimeStringFromDate(date));
        // End date
        currentTime = System.currentTimeMillis();
        date.setTime(currentTime);
        iTestData.addFieldData(Event.END,
                               0,
                               date.getTime(),
                               "DTEND:" + creteVersitTimeStringFromDate(date));
        // Revision
        currentTime = System.currentTimeMillis();
        date.setTime(currentTime);
        iTestData.addFieldData(Event.REVISION,
                               0,
                               date.getTime(),
                               "LAST-MODIFIED:" + creteVersitTimeStringFromDate(date));

        // Initialize contact list and contact item
        try
        {
            PIM pim = PIM.getInstance();
            iPIMList = pim.openPIMList(PIM.EVENT_LIST, PIM.READ_WRITE);
            iPIMItem = ((EventList) iPIMList).createEvent();
        }
        catch (PIMException pe)
        {
            failTest("Unexpected PIMException in initTest().");
            pe.printStackTrace();
        }
    }

    /**
     * Runs the test
     */
    public void runTest()
    throws TestEndThrowable
    {
        initTest();
        String error = null;
        log("Running ELserialFormatTests. Testing vCalendar 1.0");
        error = PserializationTester.testToSerialFormat(
                    iPIMList,
                    iPIMItem,
                    iTestData,
                    "UTF-8",
                    "VCALENDAR/1.0");

        if (error != null)
        {
            failTest(error);
        }
        passTest();
        cleanupTest();
    }

    /**
     * Removes the test items.
     */
    public void cleanupTest()
    {
        try
        {
            // NOTE: The test class doesn't commit the item so
            // we don't have to remove it
            //( ( EventList ) iPIMList ).removeEvent(
            //    ( Contact ) iPIMItem );
            iPIMList.close();
        }
        catch (PIMException pe)
        {
            // cannot call failTest
            log("Unexpected PIMException in cleanupTest().");
            pe.printStackTrace();
        }
    }

    private String creteVersitTimeStringFromDate(Date aDate)
    {
        long dateInMillis = aDate.getTime();
        String timeString = "";
        Calendar cal = Calendar.getInstance();

        // set time to calendar
        cal.setTime(aDate);

        // Time must be converted to UTC
        TimeZone tz = cal.getTimeZone();
        int hoursInMillis = tz.getRawOffset();

        dateInMillis -= hoursInMillis;
        // Remove daylight saving. TODO fix. useDaylightTime returns always true if
        // the zone supports daylight. This is not what we want when winter time is used
        /**if( tz.useDaylightTime() )
            {
            dateInMillis -= 3600000;
            } */
        cal.setTime(new Date(dateInMillis));

        timeString += cal.get(Calendar.YEAR) +
                      // +1 needs to be added because month numbers start from zero
                      addZeroIfNeeded(cal.get(Calendar.MONTH) + 1) +
                      addZeroIfNeeded(cal.get(Calendar.DATE)) + "T" +
                      addZeroIfNeeded(cal.get(Calendar.HOUR_OF_DAY)) +
                      addZeroIfNeeded(cal.get(Calendar.MINUTE)) +
                      addZeroIfNeeded(cal.get(Calendar.SECOND)) + "Z";

        return timeString;
    }

    private String addZeroIfNeeded(int aInt)
    {
        if (aInt < 10)
        {
            String zero = "0";
            zero += Integer.toString(aInt);
            return zero;
        }
        return Integer.toString(aInt);
    }
}