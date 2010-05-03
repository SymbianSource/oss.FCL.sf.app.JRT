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
package t_uirobot.datefield;

import j2meunit.framework.*;

import java.util.Calendar;
import java.util.Date;

import javax.microedition.lcdui.DateField;

import t_uirobot.ItemUITestBase;

import com.nokia.mj.impl.uitestutils.Key;

/**
 * TEST CASE SPECIFICATION <br>
 * <br>
 * Short description of the module test:
 * <li> UI-robot based test to test datefield's functionality. <br>
 * <br>
 * Created: 2008-10-02
 */
public class DateFieldUIRobotTest extends ItemUITestBase {

    //2008-10-07 11:30:35 is 1223368235775 in millis.
    private static final long TEST_DATE_IN_MILLIS = 1223368235775L;
    private static final int ZERO_EPOCH_YEAR = 1970;

    private static final int TEST_DAY_OF_MONTH = 20;
    private static final int TEST_MONTH = 11;
    private static final int TEST_YEAR = 2009;

    private static final int TEST_HOUR = 11;
    private static final int TEST_MINUTE = 39;

    private static final int DATEFIELD_ADD_DELAY = 100;

    /**
     * Constructor.
     */
    public DateFieldUIRobotTest() {
    }

    /**
     * Constructor.
     *
     * @param testName Test name.
     * @param testMethod Test method.
     */
    public DateFieldUIRobotTest(String testName, TestMethod testMethod) {
        super(testName, testMethod);
    }

    /**
     * Creates the test suite.
     *
     * @return New testsuite.
     */
    public Test suite() {
        TestSuite aSuite = new TestSuite();

        aSuite.addTest(new DateFieldUIRobotTest("testCommands",
                new TestMethod() {
                    public void run(TestCase tc) {
                        ((DateFieldUIRobotTest) tc).testCommands();
                    }
                }));

        aSuite.addTest(new DateFieldUIRobotTest("testMinimumSize",
                new TestMethod() {
                    public void run(TestCase tc) {
                        ((DateFieldUIRobotTest) tc).testMinimumSize();
                    }
                }));

        // TODO: Following tests wont pass until bug in eSWT is fixed.

        /*
        aSuite.addTest(new DateFieldUIRobotTest("testInputModeChange",
                new TestMethod() {
            public void run(TestCase tc) {
                ((DateFieldUIRobotTest) tc).testInputModeChange();
            }
        }));

        aSuite.addTest(new DateFieldUIRobotTest("testItemStateChange",
                new TestMethod() {
                    public void run(TestCase tc) {
                        ((DateFieldUIRobotTest) tc).testItemStateChange();
                    }
                }));

        aSuite.addTest(new DateFieldUIRobotTest("testGetDateWhenModifying",
                new TestMethod() {
                    public void run(TestCase tc) {
                        ((DateFieldUIRobotTest) tc).testGetDateWhenModifying();
                    }
                }));

        aSuite.addTest(new DateFieldUIRobotTest("testSetAndGetDate",
                new TestMethod() {
                    public void run(TestCase tc) {
                        ((DateFieldUIRobotTest) tc).testSetAndGetDate();
                    }
                }));

        aSuite.addTest(new DateFieldUIRobotTest("testSetAndGetTime",
                new TestMethod() {
                    public void run(TestCase tc) {
                        ((DateFieldUIRobotTest) tc).testSetAndGetTime();
                    }
                }));

        aSuite.addTest(new DateFieldUIRobotTest("testSetAndGetDateTime",
                new TestMethod() {
                    public void run(TestCase tc) {
                        ((DateFieldUIRobotTest) tc).testSetAndGetDateTime();
                    }
                }));
        */

        return aSuite;
    }

    /**
     * Tests the basic functionality of command added to DateField.
     */
    public void testCommands() {
        DateField dateField = new DateField("label", DateField.DATE);
        testCommand(dateField);
    }

    /**
     * Tests that getMinimumSize returns non-zero values.
     */
    public void testMinimumSize() {
        //With label:
        DateField dateField = new DateField("label", DateField.DATE);

        int w = dateField.getMinimumWidth();
        int h = dateField.getMinimumHeight();

        print("Size returned (with label), w: " + w + ", h: " + h);
        if ((w <= 0) || (h <= 0)) {
            fail("Minimum width or height was zero or less.");
        }

        //Without label:
        DateField dateField2 = new DateField(null, DateField.DATE);

        w = dateField2.getMinimumWidth();
        h = dateField2.getMinimumHeight();

        print("Size returned (without label), w: " + w + ", h: " + h);
        if ((w <= 0) || (h <= 0)) {
            fail("Minimum width or height was zero or less.");
        }
    }

    /**
     * Tests to change input mode dynamically.
     */
    public void testInputModeChange() {
        // Create DateField of input mode TIME:
        DateField dateField = new DateField("label", DateField.TIME);
        form.append(dateField);
        setCurrent(form);

        //Wait a moment to make sure that dateField is added to form.
        block(DATEFIELD_ADD_DELAY);

        // Change input mode dynamically to DATE:
        dateField.setInputMode(DateField.DATE);

        // Change the day-field of the DateField:
        key('2');
        key('0');

        // Verify the day-value is changed. If input mode change
        // has failed, then the keypresses will change the hour-field
        // and test will fail:
        Date newDate = dateField.getDate();
        assertNotNull("getDate returned null", newDate);

        Calendar calendar = Calendar.getInstance();
        calendar.setTime(newDate);

        assertEquals("Wrong day returned after input mode changed.",
                TEST_DAY_OF_MONTH, calendar.get(Calendar.DAY_OF_MONTH));
    }

    /**
     * Tests the ItemStateChange-listener added to DateField.
     */
    public void testItemStateChange() {
        DateField dateField = new DateField("label", DateField.DATE);
        form.append(dateField);
        setCurrent(form);

        // Change item's state by using keyboard and make sure the
        // listener is called and value is updated:
        key('2');
        key('0');
        assertItemStateChanged("case1", dateField);

        Date newDate = dateField.getDate();
        Calendar calendar = Calendar.getInstance();
        calendar.setTime(newDate);

        assertEquals("Wrong day returned.",
                TEST_DAY_OF_MONTH, calendar.get(Calendar.DAY_OF_MONTH));

        // Change item's state programmatically and make sure the
        // listener is _not_ called:
        dateField.setDate(new Date(TEST_DATE_IN_MILLIS));
        assertItemStateChanged("case2", null);
    }

    /**
     * Test to get date when user is modifying datefield.
     */
    public void testGetDateWhenModifying() {
        DateField dateField = new DateField("label", DateField.DATE);
        form.append(dateField);
        setCurrent(form);

        // Start to modify date by pressing zero:
        key('0');

        // At this point the date is not valid, and it should return
        // null (or date before modification which is in this case also null):
        assertNull("Date Not null when modifying field.", dateField.getDate());
    }

    /**
     * Test to set and get date.
     */
    public void testSetAndGetDate() {
        DateField dateField = new DateField("label", DateField.DATE);
        form.append(dateField);
        setCurrent(form);

        // Set date programmatically, change it by pressing keys
        // and verify value after that:
        dateField.setDate(new Date(TEST_DATE_IN_MILLIS));
        key('2');
        key('0');

        key('1');
        key('1');

        key('2');
        key('0');
        key('0');
        key('9');

        Date newDate = dateField.getDate();
        assertNotNull("getDate returned null", newDate);

        Calendar calendar = Calendar.getInstance();
        calendar.setTime(newDate);

        assertEquals("Wrong day returned.",
                TEST_DAY_OF_MONTH, calendar.get(Calendar.DAY_OF_MONTH));
        assertEquals("Wrong month returned.",
                TEST_MONTH, calendar.get(Calendar.MONTH));
        assertEquals("Wrong year returned.",
                TEST_YEAR, calendar.get(Calendar.YEAR));
    }

    /**
     * Test to set and get time.
     */
    public void testSetAndGetTime() {
        DateField dateField = new DateField("label", DateField.TIME);
        form.append(dateField);
        setCurrent(form);

        // Set time programmatically, change it by pressing keys
        // and verify value after that:
        Calendar timeCalendar = Calendar.getInstance();
        timeCalendar.set(Calendar.YEAR, ZERO_EPOCH_YEAR);
        timeCalendar.set(Calendar.MONTH, 1);
        timeCalendar.set(Calendar.DAY_OF_MONTH, 1);
        Date testTime = timeCalendar.getTime();
        dateField.setDate(testTime);

        key('1');
        key('1');

        key('3');
        key('9');

        Date newDate = dateField.getDate();
        Calendar calendar = Calendar.getInstance();
        calendar.setTime(newDate);

        assertEquals("Wrong hour returned.",
                TEST_HOUR, calendar.get(Calendar.HOUR));
        assertEquals("Wrong minute returned.",
                TEST_MINUTE, calendar.get(Calendar.MINUTE));
    }

    /**
     * Test to set and get date_time.
     */
    public void testSetAndGetDateTime() {
        DateField dateField = new DateField("label", DateField.DATE_TIME);
        form.append(dateField);
        setCurrent(form);

        // Set date_time programmatically, change it by pressing keys
        // and verify value after that:
        dateField.setDate(new Date(TEST_DATE_IN_MILLIS));

        key('1');
        key('1');

        key('3');
        key('9');

        //Skip am/pm selection:
        key(Key.RightArrow);

        key('2');
        key('0');

        key('1');
        key('1');

        key('2');
        key('0');
        key('0');
        key('9');

        Date newDate = dateField.getDate();
        Calendar calendar = Calendar.getInstance();
        calendar.setTime(newDate);

        assertEquals("Wrong day returned.",
                TEST_DAY_OF_MONTH, calendar.get(Calendar.DAY_OF_MONTH));
        assertEquals("Wrong month returned.",
                TEST_MONTH, calendar.get(Calendar.MONTH));
        assertEquals("Wrong year returned.",
                TEST_YEAR, calendar.get(Calendar.YEAR));
        assertEquals("Wrong hour returned.",
                TEST_HOUR, calendar.get(Calendar.HOUR));
        assertEquals("Wrong minute returned.",
                TEST_MINUTE, calendar.get(Calendar.MINUTE));
    }

}
