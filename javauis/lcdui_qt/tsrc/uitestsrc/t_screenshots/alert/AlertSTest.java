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
package t_screenshots.alert;

import j2meunit.framework.*;

import java.io.IOException;

import javax.microedition.lcdui.*;

import t_uirobot.UITestBase;

/**
 * TEST CASE SPECIFICATION <br>
 * <br>
 * Short description of the module test:
 * <li>This module test does screenshot based testing for Alert. <br>
 *
 * @created 2008-07-23
 */
public class AlertSTest extends UITestBase {

    private static String shortString = "Lorem ipsum";

    private static String longString = "Lorem ipsum dolor sit amet, "
            + "consectetuer adipiscing elit. Cras turpis ligula, "
            + "condimentum nec, rhoncus quis, molestie in, arcu. "
            + "Curabitur id lacus. Quisque dictum nulla id odio. "
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
    public AlertSTest() {
    }

    /**
     * Constructor.
     *
     * @param sTestName name of the test
     * @param rTestMethod TestMethod used
     */
    public AlertSTest(String sTestName, TestMethod rTestMethod) {
        super(sTestName, rTestMethod);
    }

    /**
     * Any pre-test setup can be done here
     */
    protected void setUp() throws Exception {
        super.setUp();
        exitCmd = new Command("exit", "exit", Command.EXIT, 0);
        textBox = new TextBox("textBox", "", 100, 0);
        textBox.addCommand(exitCmd);
    }

    protected void tearDown() throws Exception {
        textBox.removeCommand(exitCmd);
        textBox = null;
        exitCmd = null;
    }

    /**
     * To create the test suite. You need to add a new aSuite.addTest entry for
     * any new test methods.
     *
     * @return New testsuite.
     */
    public Test suite() {
        TestSuite aSuite = new TestSuite();

        aSuite.addTest(new AlertSTest("testMethod", new TestMethod() {
            public void run(TestCase tc) {
                ((AlertSTest) tc).testMethod();
            }
        }));

        return aSuite;
    }

    /**
     * Test method.
     */
    public void testMethod() {
        setCurrent(textBox);

        Gauge smallInd = new Gauge(null, false, 100, 0);
        Gauge largeInd = new Gauge(null, false, 100, 100);

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

        String fnamePrefix = "alert";
        String fnamePostfix = ".gif";

        // null text, small text, large text
        String[] textArr = {"NT", "ST", "LT"};

        // null image, small image, large image
        String[] imgArr = {"NI", "SI", "LI"};

        // null gauge, empty gauge, full gauge
        String[] indArr = {"NG", "EG", "FG"};

        Alert alert = new Alert(shortString);
        alert.setTimeout(Alert.FOREVER);
        setCurrent(alert);

        boolean match = true;

        for (int i = 0; i < textArr.length; i++) {
            switch (i) {
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
            for (int j = 0; j < imgArr.length; j++) {
                switch (j) {
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
                for (int k = 0; k < indArr.length; k++) {
                    switch (k) {
                        case 0:
                            alert.setIndicator(null);
                            break;
                        case 1:
                            alert.setIndicator(smallInd);
                            break;
                        case 2:
                            alert.setIndicator(largeInd);
                            break;
                        default:
                            break;
                    }
                    block(CHANGE_DISPLAYABLE_DELAY); // 0.3 sec
                    String filename = fnamePrefix
                         + textArr[i] + imgArr[j] + indArr[k] + fnamePostfix;
                    match = takeScreenshot(filename) && match;
                }
            }
        }
        setCurrent(textBox);
        assertTrue("Match failed", match);
    }

}
