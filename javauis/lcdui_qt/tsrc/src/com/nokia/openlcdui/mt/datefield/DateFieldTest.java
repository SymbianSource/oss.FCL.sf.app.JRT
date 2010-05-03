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
package com.nokia.openlcdui.mt.datefield;

import junit.framework.*;

import javax.microedition.lcdui.*;

import java.util.Calendar;
import java.util.Date;
import java.util.TimeZone;

import com.nokia.openlcdui.mt.SWTTestCase;

/**
 * TEST CASE SPECIFICATION
 *
 * <br><br>Short description of the module test:
 * <li>Collection of tests to test DateField's functionality.
 *
 * <br><br>
 * Created:    2008-09-30
 *
 */
public class DateFieldTest extends SWTTestCase {

    //2008-10-07 11:30:35 is 1223368235775 in millis.
    private static final long TEST_DATE_IN_MILLIS = 1223368235775L;

    private static final int ZERO_EPOCH_YEAR = 1970;

    /**
     * Constructor.
     */
    public DateFieldTest() {
    }

    /**
     * @param testName Test name.
     * @param testMethod Test method.
     */
    public DateFieldTest(String testName) {
        super(testName);
    }

    /**
     * Creates the test suite. You need to add a new aSuite.addTest antry for
     * any new test methods, otherwise they won't be run.
     * @return New testsuite.
     */
    public static Test suite() {
		TestSuite suite = new TestSuite();

	    java.util.Vector methodNames;
	    java.util.Enumeration e;

	    // Add widget tests
	    methodNames = DateFieldTest.methodNames();
	    e = methodNames.elements();
	    while (e.hasMoreElements()) {
	        suite.addTest(new DateFieldTest((String)e.nextElement()));
	    }

		return suite;
    }

    public static java.util.Vector methodNames() {
        java.util.Vector methodNames = new java.util.Vector();
        methodNames.addElement("testConstructors");
        methodNames.addElement("testAccessors");
        return methodNames;
    }
    
    protected void runTest() throws Throwable {
        if (getName().equals("testConstructors")) testConstructors();
        else if (getName().equals("testAccessors")) testAccessors();
        else super.runTest();
    }
    
    /**
     * Tests the functionality of constructors.
     */
    public void testConstructors() {
        // DateField with label:
        DateField dateField = new DateField("label", DateField.DATE);
        assertNotNull("Label was null.", dateField.getLabel());
        assertNull("Initial date wasn't null.", dateField.getDate());

        // DateField without label:
        dateField = new DateField(null, DateField.DATE);
        assertNull("Label not null.", dateField.getLabel());

        // DateField with different kind of input modes:
        dateField = new DateField(null, DateField.DATE);
        assertEquals("Invalid mode.", DateField.DATE, dateField.getInputMode());
        dateField = new DateField(null, DateField.TIME);
        assertEquals("Invalid mode.", DateField.TIME, dateField.getInputMode());
        dateField = new DateField(null, DateField.DATE_TIME);
        assertEquals("Invalid mode.", DateField.DATE_TIME,
                dateField.getInputMode());

        // DateField with invalid mode:
        try {
            dateField = new DateField(null, 0);
            fail("No illegalArgumentException when creating DateField with"
                    + " invalid mode.");
        }
        catch (IllegalArgumentException ex) {
        }

        // DateField with null timezone:
        dateField = new DateField(null, DateField.DATE_TIME, null);

        // DateField with timezone:
        dateField = new DateField(null, DateField.DATE_TIME,
                TimeZone.getTimeZone("GMT"));
    }

    /**
     * Tests the functionality of basic accessors methods.
     */
    public void testAccessors() {
        DateField dateField = new DateField(null, DateField.DATE);
        DateField dateField2 = new DateField(null, DateField.TIME);
        DateField dateField3 = new DateField(null, DateField.DATE_TIME);

        Date testDate = new Date(TEST_DATE_IN_MILLIS);

        // Get initial date, must be null:
        assertEquals("Initial date not null.", null, dateField.getDate());

        // Set Date:
        dateField.setDate(testDate);
        checkSameDate("Date set failed.", testDate, dateField.getDate());

        //Make sure the time-components are zero in DateField of type DATE:
        Calendar dateCalendar = Calendar.getInstance();
        dateCalendar.setTime(dateField.getDate());

        assertEquals("Hour not zero in DateField of type DATE.",
                0, dateCalendar.get(Calendar.HOUR_OF_DAY));
        assertEquals("Minute not zero in DateField of type DATE.",
                0, dateCalendar.get(Calendar.MINUTE));

        // Set Time:
        Calendar timeCalendar = Calendar.getInstance();
        timeCalendar.set(Calendar.YEAR, ZERO_EPOCH_YEAR);
        timeCalendar.set(Calendar.MONTH, 1);
        timeCalendar.set(Calendar.DAY_OF_MONTH, 1);
        Date testTime = timeCalendar.getTime();
        dateField2.setDate(testTime);
        checkSameTime("Time set failed.", testTime, dateField2.getDate());

        // Try to set time with Date-object which date-part is not 1970-01-01
        // and make sure the returned Date is null after that:
        dateField2.setDate(testDate);
        assertNull("Date not null when setting time and date-part"
                + " not 1970-01-01", dateField2.getDate());

        // Set DateTime:
        dateField3.setDate(testDate);
        checkSameDateTime("DateTime set failed.",
                testDate, dateField3.getDate());

        // Set Mode:
        dateField.setInputMode(DateField.DATE_TIME);
        assertEquals("Mode set failed.",
                DateField.DATE_TIME, dateField.getInputMode());

        // Set invalid mode:
        try {
            dateField.setInputMode(0);
            fail("No illegalArgumentException when setting"
                    + " invalid mode.");
        }
        catch (IllegalArgumentException ex) {
        }
    }

    /**
     * Checks if dates are same. If not, calls fail().
     *
     * @param msg Message to be printed to error report.
     * @param expected First Date to be compared.
     * @param actual Second Date.
     */
    private void checkSameDate(String msg, Date expected, Date actual) {
        Calendar c1 = Calendar.getInstance();
        c1.setTime(expected);
        Calendar c2 = Calendar.getInstance();
        c2.setTime(actual);

        if ((c1.get(Calendar.YEAR) != c2.get(Calendar.YEAR))
            || (c1.get(Calendar.MONTH) != c2.get(Calendar.MONTH))
            || (c1.get(Calendar.DAY_OF_MONTH) != c2.get(Calendar.DAY_OF_MONTH))
            ) {
            fail(msg + " expected: " + c1 + " actual: " + c2);
        }
    }

    /**
     * Checks if times are same. If not, calls fail(). Times
     * are same if hours and minutes are same.
     *
     * @param msg Message to be printed to error report.
     * @param expected First Date to be compared.
     * @param actual Second Date.
     */
    private void checkSameTime(String msg, Date expected, Date actual) {
        Calendar c1 = Calendar.getInstance();
        c1.setTime(expected);
        Calendar c2 = Calendar.getInstance();
        c2.setTime(actual);

        if ((c1.get(Calendar.HOUR_OF_DAY) != c2.get(Calendar.HOUR_OF_DAY))
            || (c1.get(Calendar.MINUTE) != c2.get(Calendar.MINUTE))
            ) {
            fail(msg + " expected: " + c1 + " actual: " + c2);
        }
    }

    /**
     * Checks if datetimes are same. If not, calls fail().
     *
     * @param msg Message to be printed to error report.
     * @param expected First Date to be compared.
     * @param actual Second Date.
     */
    private void checkSameDateTime(String msg, Date expected, Date actual) {
        checkSameDate(msg, expected, actual);
        checkSameTime(msg, expected, actual);
    }
}
