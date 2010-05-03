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
package t_screenshots.textbox;

import j2meunit.framework.*;

import javax.microedition.lcdui.TextBox;
import javax.microedition.lcdui.TextField;

import t_uirobot.UITestBase;

import com.nokia.mj.impl.uitestutils.Key;

/**
 * This module test tests setInitialInputMode in TextBox. Most test
 * cases check their results automatically by comparing the content of the
 * text box with the expected content at the end of the test case. Some test
 * cases take screenshots for checking the result.
 *
 * This test can be run in the the normal developer build (Engineering English),
 * in the Japan variant or in the China variant. Testing Thai variant is not
 * fully implemented but it could be added by specifying the expected
 * result (THAI_adgj). The used variant is hard-coded in the initial value
 * of the "variant" variable, and need to be changed manually before testing,
 * see the declaration of the "variant" variable.
 *
 * Five cases are ignored.
 * They are not "safe" to run due to the input language selection from the
 * edit options menu:
 * the input language could be wrongly set up for the next (sub)cases.
 * The input language dialog remembers the last selected language.
 * Therefore, the language should be selected
 * by using indexes relative to e.g. English (now it is absolute).
 * However, this assumes that the starting language would be always English,
 * and it will lead into complications especially if the language list
 * will change in future.
 * The current index in the language list can't be easily found because
 * the current input language
 * could not be the same as the selected language from list.
 *  A solution would be too complicated and making assumptions.
 * However, the tests change again the input language at runtime
 * (by LCDUI API) and these tests would not be necessary.
 * They actually compare the result to e.g. Arabic, Cyrillic characters,
 * which tests that the language was changed accordingly, in fact.
 * This is tested by other subcases.
 *
 * Note: The references images differ from 3.0 to 3.1
 */

public class TextBoxInputModeTest extends UITestBase {
    private static final int LENGTH = 2000;

    /**
     * Empty constructor.
     */
    public TextBoxInputModeTest() {
    	super();
    }

    /**
	 * Class constructor.
	 *
	 * @param sTestName
	 *            name of the test
	 * @param rTestMethod
	 *            TestMethod used
	 */
	public TextBoxInputModeTest(String sTestName, TestMethod rTestMethod) {
		super(sTestName, rTestMethod);
	}

    /***************************************
     * To create the test suite. You need to add a new aSuite.addTest antry
     * for any new test methods.
     *
     * @return test suite
     */
    public Test suite() {
        TestSuite aSuite = new TestSuite();

        aSuite.addTest(new TextBoxInputModeTest("testEditOptWhenLatDigits",
                new TestMethod() {
                    public void run(TestCase tc) {
                        ((TextBoxInputModeTest) tc).testEditOptWhenLatDigits();
                    }
                }
        ));

        aSuite.addTest(new TextBoxInputModeTest("testEditOptionsWhenLatin",
                new TestMethod() {
                    public void run(TestCase tc) {
                        ((TextBoxInputModeTest) tc).testEditOptionsWhenLatin();
                    }
                }
        ));

        aSuite.addTest(new TextBoxInputModeTest("testEditOptionsWhenArabic",
                new TestMethod() {
                    public void run(TestCase tc) {
                        ((TextBoxInputModeTest) tc)
                            .testEditOptionsWhenArabic();
                    }
                }
        ));

        aSuite.addTest(new TextBoxInputModeTest(
            "testEditOptionsWhenNonPredictive",
                new TestMethod() {
                    public void run(TestCase tc) {
                        ((TextBoxInputModeTest) tc)
                            .testEditOptionsWhenNonPredictive();
                    }
                }
        ));

        aSuite.addTest(new TextBoxInputModeTest(
            "testEditOptionsWhenPassword",
                new TestMethod() {
                    public void run(TestCase tc) {
                        ((TextBoxInputModeTest) tc)
                            .testEditOptionsWhenPassword();
                    }
                }
        ));
        return aSuite;
    }


    /**
     * Tests EditOptions menu after setInitialInputMode("IS_LATIN_DIGITS")
     * in order to see that the user can change the input mode.
     */
    private void testEditOptWhenLatDigits() {
        int[] keyPresses = new int[]{'2', '3', '4', '5', Key.Edit};
        boolean passed = true;

        TextBox textBox = new TextBox("testEditOptWhenLatDigits", // title
            "", LENGTH, TextField.ANY);
        textBox.setInitialInputMode("IS_LATIN_DIGITS");
        block(KEYEVENT_DELAY);
        setCurrent(textBox);

        // Do key presses
        for (int i = 0; i < keyPresses.length; i++) {
            key(keyPresses[i]);
        }
        block(CHANGE_DISPLAYABLE_DELAY);
        passed = takeScreenshot("testEditOptWhenLatDigits" + ".gif");
        assertTrue("testEditOptWhenLatDigits", passed);
    }

    /**
     * Tests EditOptions menu after setInitialInputMode("IS_LATIN")
     * in order to see that the user can change the input mode and
     * and the input language.
     */
    private void testEditOptionsWhenLatin () {
        doInputModeTest(getName(),
                TextField.ANY, "IS_LATIN",
                "",                             // initial content
                new int[]{'2', '3', '4', '5', Key.Edit},  // key presses
                null);
    }

    /**
     * Tests EditOptions menu after setInitialInputMode("UCB_ARABIC")
     * in order to see that the user can change the input mode the
     * input language and also Arabic Number mode.
     */
    private void testEditOptionsWhenArabic() {
        doInputModeTest(getName(),
                TextField.ANY, "UCB_ARABIC",
                "",                             // initial content
                new int[]{'2', '3', '4', '5', Key.Edit},  // key presses
                null);  // no expected content, screenshot taken
    }

    /**
     * Tests EditOptions menu after setInitialInputMode("UCB_ARABIC")
     * when NON_PREDICTIVE. Edit Options menu does not contain any
     * "Predictive text on" menu item.
     */
    private void testEditOptionsWhenNonPredictive() {
        doInputModeTest(getName(),
                TextField.NON_PREDICTIVE, "UCB_GREEK",
                "",                             // initial content
                new int[]{'2', '3', '4', '5', Key.Edit},  // key presses
                null);  // no expected content, screenshot taken
    }

    /**
     * Tests EditOptions menu after setInitialInputMode("UCB_HEBREW")
     * when PASSWORD. Edit Options menu does not contain any
     * "Predictive text on" menu item.
     */
    private void testEditOptionsWhenPassword() {
        doInputModeTest(getName(),
                TextField.PASSWORD, "UCB_HEBREW",
                "",                             // initial content
                new int[]{'2', '3', '4', '5', Key.Edit},  // key presses
                null);  // no expected content, screenshot taken
    }


    /**
     * Private method for testing initial input mode. Creates TextBox and
     * sets the initial input mode before displaying it.
     * Generates the given key presses. At the end compares the content
     * of TextBox to the expected content, unless the expected content is null;
     * in that case takes a screenshot.
     *
     * @param testCaseName Name of the test case
     * @param constraints Combination of TextField.ANY,...
     * @param initialInputMode Intial input mode to be set, e.g "IS_LATIN";
     *      "NONE" means that setInitialInputMode is not called at all
     * @param initialContent Initial context of TextBox
     * @param pressedKeys Array of key presses to be generated
     * @param expectedContent Expected content of TextBox
     */
    private void doInputModeTest(String testCaseName,
            int constraints, String initialInputMode, String initialContent,
            int[] pressedKeys, String expectedContent) {
/*
        boolean passed = true;
        SpedeRunner spede = (SpedeRunner) SpedeRunner.getInstance();

        try {
            // Create TextBox
            TextBox textBox = new TextBox(
                    testCaseName, // title
                    initialContent, LENGTH, constraints);

            // Set initial input mode before displaying TextBox
            if ("NONE".equals(initialInputMode)) {
                // initial input mode not set
            }
            else {
                textBox.setInitialInputMode(initialInputMode);
                block(SpedeRunner.keyEventDelay);
            }
            Display.getDisplay(SpedeRunner.getInstance()).setCurrent(textBox);
            block(CHANGE_DISPLAYABLE_DELAY);

            // Do key presses
            if (pressedKeys != null) {
                for (int i = 0; i < pressedKeys.length; i++) {
                    spede.triggerKeyPressEvent(new Key(pressedKeys[i]));
                    block(SpedeRunner.keyEventDelay);
                }
            }

            /*block(CHANGE_DISPLAYABLE_DELAY);
            spede.takeScreenshot(testCaseName + ".gif");

        }
        catch (Exception exc) {
            System.out.println("!!!!======!!!!!");

            /*
            spede.addToResultsList(testCaseName + " test failed: "
                    + exc.toString());
            spede.addToResultsList(exc);
            passed = false;
        }

        System.out.println("before assertTrue");

        assertTrue(testCaseName, passed);
        System.out.println("after assertTrue");*/
    }

}

