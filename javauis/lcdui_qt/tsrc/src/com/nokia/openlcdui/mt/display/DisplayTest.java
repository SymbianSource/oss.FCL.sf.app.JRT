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
package com.nokia.openlcdui.mt.display;

import junit.framework.*;

import java.util.Date;

import javax.microedition.lcdui.*;

import com.nokia.openlcdui.mt.SWTTestCase;

/**
 * TEST CASE SPECIFICATION <br>
 * <br>
 * Short description of the module test:
 * <li> This module test tests return values of the Display class methods. <br>
 * <br>
 * Created: 2008-04-15
 */
public class DisplayTest extends SWTTestCase {

    private Display display;

    // invalid color specifier for Display.getColor();
    private static final int INVALID_SPECIFIER = 4325214;
    // max length for TextBox
    private static final int LENGTH = 100;
    private static final int ANIMATION_SLEEP = 40;

    /**
     * Constructor..
     */
    public DisplayTest() {
    }

    /**
     * Constructor.
     *
     * @param sTestName name of the test
     * @param rTestMethod TestMethod used
     */
    public DisplayTest(String sTestName) {
        super(sTestName);
    }

    /**
     * Any pre-test setup can be done here.
     */
    protected void setUp() throws Exception {
        display = Display.getDisplay(getMIDlet());
    }

    /**
     * Creates new test suite.You need to add a new aSuite.addTest entry for any
     * new test methods, otherwise they won't be run.
     *
     * @return suite
     */
    public static Test suite() {
		TestSuite suite = new TestSuite();

	    java.util.Vector methodNames;
	    java.util.Enumeration e;

	    // Add widget tests
	    methodNames = DisplayTest.methodNames();
	    e = methodNames.elements();
	    while (e.hasMoreElements()) {
	        suite.addTest(new DisplayTest((String)e.nextElement()));
	    }

		return suite;
    }

    public static java.util.Vector methodNames() {
        java.util.Vector methodNames = new java.util.Vector();
        methodNames.addElement("testGetDisplayException");
        methodNames.addElement("testGetDisplay");
        methodNames.addElement("testGetCurrentSetCurrent");
        methodNames.addElement("testSetCurrentNull");
        methodNames.addElement("testCallSeriallyWithCanvas");
        methodNames.addElement("testVibrateAndFlashBacklights");
        methodNames.addElement("testNumColors");
        methodNames.addElement("testIsColor");
        methodNames.addElement("testGetColorException");
        methodNames.addElement("testGetColor");
        methodNames.addElement("testNumAlphaLevels");
        return methodNames;
    }
    
    protected void runTest() throws Throwable {
        if (getName().equals("testGetDisplayException")) testGetDisplayException();
        else if (getName().equals("testGetDisplay")) testGetDisplay();
        else if (getName().equals("testGetCurrentSetCurrent")) testGetCurrentSetCurrent();
        else if (getName().equals("testSetCurrentNull")) testSetCurrentNull();
        else if (getName().equals("testCallSeriallyWithCanvas")) testCallSeriallyWithCanvas();
        else if (getName().equals("testVibrateAndFlashBacklights")) testVibrateAndFlashBacklights();
        else if (getName().equals("testNumColors")) testNumColors();
        else if (getName().equals("testIsColor")) testIsColor();
        else if (getName().equals("testGetColorException")) testGetColorException();
        else if (getName().equals("testGetColor")) testGetColor();
        else if (getName().equals("testNumAlphaLevels")) testNumAlphaLevels();
        else super.runTest();
    }

    /**
     * This method tests that Display.getDisplay(MIDlet m) returns not null.
     */
    public void testGetDisplay() {
        Display getDisplay = Display.getDisplay(getMIDlet());
        assertNotNull("Display.getDisplay(midlet) is null", getDisplay);
    }

    /**
     * This method tests that getDisplay(MIDlet midlet) method throws
     * NullPointerException if midlet is null.
     */
    public void testGetDisplayException() {
        try {
            Display.getDisplay(null);
            fail("Display.getDisplay(null) NullPointerException expected.");
        }
        catch (NullPointerException npe) {
            // Ok
        }
    }

    /**
     * This method tests that getColor(int specifier) method throws
     * IllegalArgumentException specifier is not valid.
     */
    public void testGetColorException() {
        try {
            display.getColor(INVALID_SPECIFIER);
            fail("getColor(invSpecifier) IllegalArgumentException expected.");
        }
        catch (IllegalArgumentException iae) {
            // Ok
        }
    }

    /**
     * This method test that setCurrent(Alert alert, Displayable displayable)
     * method throws NullPointerException if alert is null.
     */
    public void testSetCurrentException1() {
        Alert alert = null;
        TextBox nextDisplayable = new TextBox(getName(), "", LENGTH, TextField.ANY);
        try {
            display.setCurrent(alert, nextDisplayable);
            fail("Display.setCurrent(null, nextDisplayable). " +
                    "NullPointerException expected.");
        }
        catch (NullPointerException npe) {
            // Ok
        }
    }

    /**
     * This method test that setCurrent(Alert alert, Displayable displayable)
     * method throws NullPointerException if displayable is null.
     */
    public void testSetCurrentException2() {
        Alert alert = new Alert(getName());
        TextBox nextDisplayable = null;
        try {
            display.setCurrent(alert, nextDisplayable);
            fail("Display.setCurrent(alert, null). " +
                    "NullPointerException expected.");
        }
        catch (NullPointerException npe) {
            // Ok
        }
    }

    /**
     * This method test that setCurrent(Alert alert, Displayable displayable)
     * method throws NullPointerException if next displayable is Alert.
     */
    public void testSetCurrentException3() {
        Alert alert = new Alert("alert1");
        Alert nextAlert = new Alert("alert2");
        try {
            display.setCurrent(alert, nextAlert);
            fail("Display.setCurrent(alert, null). " +
                    "IllegalArgumentException expected.");
        }
        catch (IllegalArgumentException iae) {
            // Ok
        }
    }

    /**
     * This method tests that Display.getCurrent() returns current Displayable.
     * And Display.setCurrent(Displayable d)
     */
    public void testGetCurrentSetCurrent() {
        TextBox currentDisplayable = new TextBox(getName(), "", LENGTH, TextField.ANY);
        Displayable getDisplayable;

        display.setCurrent(currentDisplayable);

        getDisplayable = display.getCurrent();
        assertEquals("Display.getCurrent()", currentDisplayable, getDisplayable);
    }

    /**
     * This method tests that Display.setCurrent(null) behaves correctly.
     */
    public void testSetCurrentNull() {
        TextBox currentDisplayable = new TextBox(getName(), "", LENGTH, TextField.ANY);
        Displayable getDisplayable;

        display.setCurrent(currentDisplayable);
        display.setCurrent(null);
        getDisplayable = display.getCurrent();
        assertEquals("Display.getCurrent()", currentDisplayable, getDisplayable);
    }

    /**
     * Test CallSerially method.
     */
    public void testCallSeriallyWithCanvas() {
        long paintTime = 0;
        long threadTime = 0;
        boolean passed = true;
        String message = "OK";

        MoveShapeTCanvas screen = new MoveShapeTCanvas(display);
        // screen.setTitle("Canvas"); // canvas's title cannot be set
        MyThreadClass runnable = new MyThreadClass();

        for (int i = 1; i <= LENGTH; i++) {
            screen.setWH(i, i);
            screen.repaint();
            display.callSerially(runnable);
            // runnable.run();
            paintTime = screen.getTimestamp();
            threadTime = runnable.getRunTimeStamp();
            if ((paintTime > threadTime)
                    && (screen.getCounter() <= runnable.getCounter())) {
                passed = false;
                message = "CallSerially fails on iteration: " + i;
            }
            block(ANIMATION_SLEEP);
        }
        assertTrue("Display.callSerially(Runnable r). " + message, passed);
    }

    /**
     * Dummy Runnable to check Call serially
     */
    class MyThreadClass implements Runnable {

        private long runTimestamp;
        private int counter;

        public void run() {
            runTimestamp = new Date().getTime();
            counter++;
        }

        public int getCounter() {
            return counter;
        }

        public long getRunTimeStamp() {
            return runTimestamp;
        }
    }

    class MoveShapeTCanvas extends Canvas {
        private Display display;
        private long paintTimestamp;
        private int w;
        private int h;
        private int counter;

        /**
         * Constructor;
         *
         * @param aDisplay display to be set for the Canvas
         */
        public MoveShapeTCanvas(Display aDisplay) {
            display = aDisplay;
            display.setCurrent(this);
        }

        /**
         * Set Width and Height for fillRect
         *
         * @param aW
         * @param aH
         */
        public void setWH(int aW, int aH) {
            w = aW;
            h = aH;
        }

        public void paint(Graphics g) {
            g.fillRect(0, 0, w, h);
            paintTimestamp = new Date().getTime();
            counter++;
        }

        public int getCounter() {
            return counter;
        }

        /**
         * Timestamp when paint() function was called.
         *
         * @return timestamp
         */
        public long getTimestamp() {
            return paintTimestamp;
        }
    }

    /**
     * Tests to call vibrate and flashbacklights methods with illegal parameter
     * values and verify an exception is thrown.
     */
    public void testVibrateAndFlashBacklights() {
        try {
            display.vibrate(-1);
            fail("display.vibrate(-1). IllegalArgumentException expected");
        }
        catch (IllegalArgumentException ex) {
            // Ok
        }

        try {
            display.flashBacklight(-1);
            fail("display.flashBacklight(-1);. IllegalArgumentException expected");
        }
        catch (IllegalArgumentException ex) {
            // Ok
        }
    }

    /**
     * Test Display.numColors().
     */
    public void testNumColors() {
        int numColors = 0;
        numColors = display.numColors();
        assertTrue("Display.numColors(). " +
        		"Expected return not 0, but returned " + numColors, numColors != 0);
    }

    /**
     * Test Display.isColor(). note: to pass this test you need to have non
     * black and white screen device
     */
    public void testIsColor() {
        boolean passed = false;
        passed = display.isColor();
        assertTrue("Display.isColor(). Expected return true, but returned "
                + passed, passed);
    }

    /**
     * Display.getColor(int specifier) test.
     */
    public void testGetColor() {
        int color = 0;
        color = display.getColor(Display.COLOR_BACKGROUND);
        assertTrue("Display.getColor(COLOR_BACKGROUND). " +
        		"Expected returns not 0 but returned " + color, color != 0);
    }

    /**
     * Display.numAlphaLevels() test.
     */
    public void testNumAlphaLevels() {
        int numAlpha = display.numAlphaLevels();
        assertTrue("Display.numAlphaLevels(). " +
                "Expected returns not 0 but returned "+ numAlpha, numAlpha != 0);
    }

}
