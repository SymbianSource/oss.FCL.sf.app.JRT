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
package t_uirobot.alert;

import j2meunit.framework.*;

import java.io.IOException;

import javax.microedition.lcdui.*;

import spede.SpedeRunner;
import t_uirobot.UITestBase;

import com.nokia.mj.impl.uitestutils.Key;

/**
 * TEST CASE SPECIFICATION <br>
 * <br>
 * Short description of the module test:
 * <li> UI-robot based tests to test Alert's functionality. <br>
 * <br>
 *
 * @created  2008-07-01
 */
public class AlertUITest extends UITestBase {

    private static final int SETCURRENT_TIMEOUT = 6000;
    private static final int TEXBOX_LENGTH = 100;

    private static String shortString = "Lorem ipsum";

    private static String longString = "Lorem ipsum dolor sit amet, "
        + "consectetuer adipiscing elit. Cras turpis ligula, "
        + "condimentum nec, rhoncus quis, molestie in, arcu. "
        + "Curabitur id lacus. Quisque dictum nulla id odio. "
        + "Nullam nec urna. Cras ac lacus nec lacus iaculis aliquet."
        + " Integer ut eros. Proin laoreet justo et augue. "
        + "Praesent dui. Proin vel leo a eros auctor convallis. "
        + "Aenean urna nunc, sagittis vel, pellentesque a, "
        + "luctus a, metus. Phasellus posuere lacus nec augue."
        + "Nullam nec urna. Cras ac lacus nec lacus iaculis aliquet."
        + " Integer ut eros. Proin laoreet justo et augue. "
        + "Praesent dui. Proin vel leo a eros auctor convallis. "
        + "Aenean urna nunc, sagittis vel, pellentesque a, "
        + "luctus a, metus. Phasellus posuere lacus nec augue.";

    private TextBox textBox;
    private Command exitCmd;

    /**
     * Constructor.
     */
    public AlertUITest() {
    	super();
    }

    /**
     * Constructor.
     *
     * @param sTestName Test name.
     * @param rTestMethod Test method.
     */
    public AlertUITest(String sTestName, TestMethod rTestMethod) {
        super(sTestName, rTestMethod);
    }

    /**
     * Any pre-test setup can be done here
     */
    protected void setUp() throws Exception {
    	super.setUp();
        exitCmd = new Command("exit", "exit", Command.EXIT, 0);
        textBox = new TextBox("textBox", "", TEXBOX_LENGTH, TextField.ANY);
        textBox.addCommand(exitCmd);
    }

    /**
     * Creates the test suite. You need to add a new aSuite.addTest entry for
     * any new test methods, otherwise they won't be run.
     *
     * @return New testsuite.
     */
    public Test suite() {
        TestSuite aSuite = new TestSuite();

        aSuite.addTest(new AlertUITest("testShowTimedAlert", new TestMethod() {
            public void run(TestCase tc) {
                ((AlertUITest) tc).testShowTimedAlert();
            }
        }));

        aSuite.addTest(new AlertUITest("testCustomListener", new TestMethod() {
            public void run(TestCase tc) {
                ((AlertUITest) tc).testCustomListener();
            }
        }));

        aSuite.addTest(new AlertUITest("testCustomListenerCustomCommand",
                new TestMethod() {
                    public void run(TestCase tc) {
                        ((AlertUITest) tc).testCustomListenerCustomCommand();
                    }
                }));

        aSuite.addTest(new AlertUITest("testKeypressOnCustomCommands",
                new TestMethod() {
                    public void run(TestCase tc) {
                        ((AlertUITest) tc).testKeypressOnCustomCommands();
                    }
                }));

        aSuite.addTest(new AlertUITest("testLongTextScrolling",
                new TestMethod() {
                    public void run(TestCase tc) {
                        ((AlertUITest) tc).testLongTextScrolling();
                    }
                }));

        aSuite.addTest(new AlertUITest("testImageAndTextCombinations",
                new TestMethod() {
                    public void run(TestCase tc) {
                        ((AlertUITest) tc).testImageAndTextCombinations();
                    }
                }));

        return aSuite;
    }

    /**
     * Tests showing a timed alert with default values.
     */
    public void testShowTimedAlert() {
        setCurrent(textBox);

        Alert alert = new Alert("timedAlert");
        int timeout = alert.getTimeout();
        assertTrue("Alert should be timed", timeout != Alert.FOREVER);

        // LSK should be empty - should just dismiss Alert
        setCurrent(alert);
        key(Key.CBA1);
        assertEquals("LSK should dismiss", textBox, display.getCurrent());

        // RSK should be empty - should just dismiss Alert
        setCurrent(alert);
        key(Key.CBA2);
        assertEquals("RSK should dismiss", textBox, display.getCurrent());

        // Select should just dismiss Alert
        setCurrent(alert);
        key(Key.Select);
        assertEquals("Select should dismiss", textBox, display.getCurrent());

        // Default listener should dismisses the Alert
        setCurrent(alert);
        block(timeout + CHANGE_DISPLAYABLE_DELAY);
        assertEquals("Textbox should be current",
                textBox, display.getCurrent());
    }

    /**
     * Tests showing a timed alert with custom command listener.
     */
    public void testCustomListener() {
    	setCurrent(textBox);

        Alert alert = new Alert("customListenerAlert");
        assertTrue("Alert should be timed",
                alert.getTimeout() != Alert.FOREVER);
        alert.setCommandListener(this);
        setCurrent(alert);
        // wait for max 10sec to get command listener callback
        assertTrue("Textbox isShown() should be false", !textBox.isShown());
        assertCmdListener("", Alert.DISMISS_COMMAND, alert);
    }

    /**
     * Tests showing a timed alert with custom command listener and custom
     * command.
     */
    public void testCustomListenerCustomCommand() {
    	setCurrent(textBox);

        Alert alert = new Alert("customCmdAlert");
        assertTrue("Alert should be timed",
                alert.getTimeout() != Alert.FOREVER);
        alert.setCommandListener(this);
        Command testCmd = new Command("test", "test", Command.ITEM, 0);
        alert.addCommand(testCmd);
        setCurrent(alert);
        // wait for max 10sec to get command listener callback
        assertTrue("Textbox isShown() should be false", !textBox.isShown());
        assertCmdListener("", testCmd, alert);
    }

    /**
     * Tests pressing keys on Alert's custom commands.
     */
    public void testKeypressOnCustomCommands() {
    	setCurrent(textBox);

        Alert alert = new Alert("keypressAlert");
        assertTrue("Alert should be timed",
                alert.getTimeout() != Alert.FOREVER);
        alert.setCommandListener(this);
        Command testCmd1 = new Command("Cmd1", "Command 1", Command.SCREEN, 0);
        Command testCmd2 = new Command("Cmd2", "Command 2", Command.SCREEN, 1);
        alert.addCommand(testCmd1);
        alert.addCommand(testCmd2);
        assertTrue("Alert should be modal",
                alert.getTimeout() == Alert.FOREVER);

        setCurrent(alert);
        key(Key.CBA1, 0);
        key(Key.Select);
        assertCmdListener("", testCmd1, alert);
        setCurrent(textBox);

        setCurrent(alert);
        key(Key.CBA1, 0);
        key(Key.DownArrow, 0);
        key(Key.Select);
        assertCmdListener("", testCmd2, alert);
    }

    /**
     * Tests scrolling a long text.
     */
    public void testLongTextScrolling() {
    	setCurrent(textBox);

        Alert alert = new Alert("scrollingAlert");
        alert.setString(longString);
        alert.setCommandListener(this);
        assertTrue("Alert should be modal",
                alert.getTimeout() == Alert.FOREVER);

        setCurrent(alert);
        for (int i = 0; i < 10; i++) {
            key((i < 5 ? Key.DownArrow : Key.UpArrow));
        }

        key(Key.CBA1);
        assertCmdListener("", Alert.DISMISS_COMMAND, alert);
    }

    /**
     * Tests different text and image setting combinations.
     */
    public void testImageAndTextCombinations() {
    	setCurrent(textBox);

        Image smallImg = null;
        Image largeImg = null;
        try {
            smallImg = Image.createImage("100x100.png");
        }
        catch (IOException e) {
            fail("Cannot load image 100x100.png");
        }
        try {
            largeImg = Image.createImage("200x200.png");
        }
        catch (IOException e) {
            fail("Cannot load image 200x200.png");
        }

        Alert alert = new Alert("varyingAlert");
        alert.setTimeout(Alert.FOREVER);
        alert.setCommandListener(this);
        assertTrue("Alert should be modal",
                alert.getTimeout() == Alert.FOREVER);

        setCurrent(alert);
        for (int textIndex = 0; textIndex < 3; textIndex++) {
            switch (textIndex) {
                case 0:
                    alert.setString(null);
                    break;
                case 1:
                    alert.setString(shortString);
                    break;
                case 2:
                    alert.setString(longString);
                    break;
                default:
                    break;
            }
            for (int imgIndex = 0; imgIndex < 3; imgIndex++) {
                switch (imgIndex) {
                    case 0:
                        alert.setImage(null);
                        break;
                    case 1:
                        alert.setImage(smallImg);
                        break;
                    case 2:
                        alert.setImage(largeImg);
                        break;
                    default:
                        break;
                }
                block(CHANGE_DISPLAYABLE_DELAY); // 0.3 sec
            }
        }
        block(1000); // 1sec

        key(Key.CBA1);
        assertCmdListener("", Alert.DISMISS_COMMAND, alert);
    }

}
