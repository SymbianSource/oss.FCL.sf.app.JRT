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
package t_uirobot.display;

import j2meunit.framework.*;

import javax.microedition.lcdui.Display;
import javax.microedition.lcdui.TextBox;

import t_uirobot.UITestBase;

import com.nokia.mj.impl.uitestutils.Key;

/**
 * TEST CASE SPECIFICATION <br>
 * <br>
 * Short description of the module test:
 * <li> UI-robot based tests to test Display's vibrate and flashBacklights
 * -methods. <br>
 * <br>
 * Created: 2008-05-23
 */
public class FlashAndVibrateTest extends UITestBase {

    private static final int MAX_NUM_OF_CHARS_IN_TEST_TEXTBOX = 100;
    private static final int SEND_BACKGROUND_DELAY = 300;
    private static final int SEND_FOREGROUND_DELAY = 300;

    private static final int VIBRA_TIME = 500;
    private static final int FLASH_TIME = 500;

    /**
     * Constructor.
     */
    public FlashAndVibrateTest() {
    }

    /**
     * Constructor.
     *
     * @param sTestName Test name.
     * @param rTestMethod Test method.
     */
    public FlashAndVibrateTest(String sTestName, TestMethod rTestMethod) {
        super(sTestName, rTestMethod);
    }

    /**
     * Creates the test suite. You need to add a new aSuite.addTest antry for
     * any new test methods, otherwise they won't be run.
     *
     * @return New testsuite.
     */
    public Test suite() {
        TestSuite aSuite = new TestSuite();

        /*
        TODO: Remove comments when implementation ready:
        aSuite.addTest(new FlashAndVibrateTest("testMethodsWhenBackground",
                new TestMethod() {
                    public void run(TestCase tc) {
                        ((FlashAndVibrateTest) tc).testMethodsWhenBackground();
                    }
                }));
        */
        return aSuite;
    }

    /**
     * Test that vibrate() and flashBacklights() -methods returns false when
     * MIDlet is in background. Note that this method assumes that backlight
     * flashing and vibration are supported and will pass even if there's
     * no support to the functionality.
     */
    public void testMethodsWhenBackground() {
        boolean testPassed = true;
        String testMsg = "";

        Display display = Display.getDisplay(getMIDlet());
        TextBox textBox = new TextBox("title", "content",
                MAX_NUM_OF_CHARS_IN_TEST_TEXTBOX, 0);

        setCurrent(textBox);

        //Send MIDlet to background and verify that vibrate()
        //and flashBacklights both return false:

        key(Key.Applications, SEND_BACKGROUND_DELAY);

        if (display.vibrate(VIBRA_TIME)) {
            testPassed = false;
            testMsg = "vibrate() returned true when MIDlet was in background.";
        }

        if (display.flashBacklight(FLASH_TIME)) {
            testPassed = false;
            testMsg = "flashBacklights() returned true when MIDlet"
                    + " was in background.";
        }

        //Move MIDlet back to foreground:
        key(Key.CBA1, 0);
        key(Key.UpArrow, 0);
        key(Key.Select, 0);
        key(Key.Select, SEND_FOREGROUND_DELAY);

        assertTrue(getName() + " failed, " + testMsg, testPassed);
    }

}

