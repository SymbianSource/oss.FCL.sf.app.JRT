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
package t_screenshots.textfield;

import j2meunit.framework.*;

import javax.microedition.lcdui.*;

import spede.SpedeRunner;
import t_uirobot.UITestBase;

import com.nokia.mj.impl.uitestutils.Key;

/**
 *
 *          RUN AS A FIRST/ONLY TEST.
 * If something is in the clipboard, the results may be incorrect.
 *
 *
 * This module test tests setInitialInputMode in TextField. Most test
 * cases check their results automatically by comparing the content of the
 * text field with the expected content at the end of the test case. Some test
 * cases take screenshots for checking the result.
 *
 * Currently this test can be run only in the the normal developer build
 * (Engineering English). If you want to run this test in the Japan,
 * China or Thai variant, you need to modify the test cases and expected
 * results similar to the test cases in textbox.TextBoxSetInitialInputModeTest.
 *
 * Three cases are ignored.
 * They are not "safe" to run due to the input language
 * selection from the edit options menu:
 * the input language could be wrongly set up for the next (sub)cases.
 * The input language dialog
 * remembers the last selected language.
 * Therefore, the language should be selected
 * by using indexes relative to e.g. English (now it is absolute).
 * However, this assumes that the starting language would be always English,
 * and it will lead into complications especially if the language
 * list will change in future.
 * The current index in the language list can't be easily found because
 * the current input language
 * could not be the same as the selected language from list.
 * A solution would be too complicated and making assumptions.
 * However, the tests change again the input language at runtime (by LCDUI API)
 * and these tests would not be necessary.
 * They compare the result to e.g. Arabic, Cyrillic characters,
 * which tests that the language was changed accordingly, in fact.
 * This is tested by other subcases.
 *
 * The case test_insertLineFeed was also ignored on 2006-11-20
 * due the above reason.
 * Note: The references images differ from 3.0 to 3.1, and 3.2
 */

public class SetInitInModeTest extends UITestBase {


    // ==============================================================
    private final int normalSize = 2000;

    /**
     * Default constructor.
     */
    public SetInitInModeTest() {
    }

    /**
     * Constructor.
     *
     * @param sTestName name of testcase.
     * @param rTestMethod test method.
     */
    public SetInitInModeTest(String sTestName,
            TestMethod rTestMethod) {
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

        aSuite.addTest(new SetInitInModeTest(
                "test_EditOptionsWhenLatinDigits", new TestMethod() {
                    public void run(TestCase tc) {
                        ((SetInitInModeTest) tc)
                                .testEditOptionsWhenLatinDigits();
                    }
                }));

        aSuite.addTest(new SetInitInModeTest(
                "test_EditOptionsWhenLatin", new TestMethod() {
                    public void run(TestCase tc) {
                        ((SetInitInModeTest) tc)
                                .testEditOptionsWhenLatin();
                    }
                }));

        aSuite.addTest(new SetInitInModeTest(
                "test_EditOptionsWhenArabic", new TestMethod() {
                    public void run(TestCase tc) {
                        ((SetInitInModeTest) tc)
                                .testEditOptionsWhenArabic();
                    }
                }));

        aSuite.addTest(new SetInitInModeTest(
                "test_EditOptionsWhenNonPredictive", new TestMethod() {
                    public void run(TestCase tc) {
                        ((SetInitInModeTest) tc)
                                .testEditOptionsWhenNonPredictive();
                    }
                }));

        aSuite.addTest(new SetInitInModeTest(
                "test_EditOptionsWhenPassword", new TestMethod() {
                    public void run(TestCase tc) {
                        ((SetInitInModeTest) tc)
                                .testEditOptionsWhenPassword();
                    }
                }));



        return aSuite;
    }

    // ==============================================================


    /**
     * Tests EditOptions menu after setInitialInputMode("IS_LATIN_DIGITS") in
     * order to see that the user can change the input mode.
     */
    private void testEditOptionsWhenLatinDigits() {
        doInputModeTest(getName(), TextField.ANY, "IS_LATIN_DIGITS", "",
                // initial content
                new int[] {'2', '3', '4', '5', Key.Edit, }, // key presses
                null);
    }

    /**
     * Tests EditOptions menu after setInitialInputMode("IS_LATIN") in order to
     * see that the user can change the input mode and and the input language.
     */
    private void testEditOptionsWhenLatin() {
        doInputModeTest(getName(), TextField.ANY, "IS_LATIN", "", // initial
                                                                    // content
                new int[] {'2', '3', '4', '5', Key.Edit, }, // key presses
                null);
    }

    /**
     * Tests EditOptions menu after setInitialInputMode("UCB_ARABIC") in order
     * to see that the user can change the input mode the input language and
     * also Arabic Number mode.
     */
    private void testEditOptionsWhenArabic() {
        doInputModeTest(getName(), TextField.ANY, "UCB_ARABIC", "", // initial
                                                                    // content
                new int[] {'2', '3', '4', '5', Key.Edit, }, // key presses
                null); // no expected content, screenshot taken
    }

    /**
     * Tests EditOptions menu after setInitialInputMode("UCB_ARABIC") when
     * NON_PREDICTIVE. Edit Options menu does not contain any "Predictive text
     * on" menu item.
     */
    private void testEditOptionsWhenNonPredictive() {
        doInputModeTest(getName(), TextField.NON_PREDICTIVE, "UCB_GREEK", "",
             // initial content
                new int[] {'2', '3', '4', '5', Key.Edit, }, // key presses
                null); // no expected content, screenshot taken
    }

    /**
     * Tests EditOptions menu after setInitialInputMode("UCB_HEBREW") when
     * PASSWORD. Edit Options menu does not contain any "Predictive text on"
     * menu item.
     */
    private void testEditOptionsWhenPassword() {
        doInputModeTest(getName(), TextField.PASSWORD, "UCB_HEBREW", "",
                // initial content
                new int[] {'2', '3', '4', '5', Key.Edit, }, // key presses
                null); // no expected content, screenshot taken
    }


    /**
     * Private method for testing initial input mode. Creates TextField and
     * sets the initial input mode after constructing the textField.
     * Generates the given key presses. At the end compares the content
     * of TextField to the expected content,
     *  unless the expected content is null;
     * in that case takes a screenshot.
     *
     * @param testCaseName Name of the test case
     * @param constraints Combination of TextField.ANY,...
     * @param initialInputMode Intial input mode to be set, e.g "IS_LATIN";
     *      "NONE" means that setInitialInputMode is not called at all
     * @param initialContent Initial context of TextField
     * @param pressedKeys Array of key presses to be generated
     * @param expectedContent Expected content of TextField
     */
    private void doInputModeTest(String testCaseName, int constraints,
            String initialInputMode, String initialContent, int[] pressedKeys,
            String expectedContent) {

        boolean passed = true;

        try {
            // Create a form with two text fields
            Form form = new Form(testCaseName);
            TextField textField = new TextField("Text field", initialContent,
                    normalSize, constraints);
            TextField otherField = new TextField("Other field", "", normalSize,
                    TextField.ANY);

            // Set initial input mode before displaying TextField
            if ("NONE".equals(initialInputMode)) {
                // initial input mode not set
            }
            else {
                textField.setInitialInputMode(initialInputMode);
            }
            form.append(textField);
            form.append(otherField);
            Display.getDisplay(SpedeRunner.getInstance()).setCurrent(form);
            block(CHANGE_DISPLAYABLE_DELAY);
            block(CHANGE_DISPLAYABLE_DELAY);
            block(CHANGE_DISPLAYABLE_DELAY);


            // Do key presses
            if (pressedKeys != null) {
                for (int i = 0; i < pressedKeys.length; i++) {
                    key(pressedKeys[i]);
                }
            }
            block(KEYEVENT_DELAY);

            // Check the result of key presses (i.e. the used input mode)
            passed = checkResult(testCaseName, textField, expectedContent,
                    expectedContent == null ? true : false);

        }
        catch (Exception exc) {
            print(testCaseName + " test failed: " + exc.toString());
			print(exc);
            passed = false;
        }

        assertTrue(testCaseName, passed);
    }


    /**
     * Checks whether the content of the given text field corresponds to
     * the expected content.
     *
     * @param testCaseName Name of the test case
     * @param textBox TextField to be checked
     * @param expectedContent The expected content
     * @param takeScreenshot If true, screenshot is taken
     * @return result Returns whether test case passed or failed
     */
    private boolean checkResult(String testCaseName, TextField textBox,
            String expectedContent, boolean takeScreenshot) {

        boolean passed = true;
        SpedeRunner spede = (SpedeRunner) SpedeRunner.getInstance();

        // Check the expected content
        if (expectedContent != null) {
            if (!expectedContent.equals(textBox.getString())) {
                passed = false;
                spede.addToResultsList(testCaseName + " test "
                        + " failed: content: " + textBox.getString()
                        + ", expected: " + expectedContent);
            }
        }

        // Take the screenshot and compare it with the previous one
        if (takeScreenshot) {
            if (!spede.takeScreenshot(testCaseName + ".gif")) {
                passed = false;
                spede.addToResultsList(testCaseName + " test "
                        + " failed: screenshot does not match");

                spede.addToResultsList("If failure is caused by different "
                        + "special characters which are show in the cell at "
                        + "the top of the Edit Menu, please IGNORE the result"
                        + " and treat the test as PASSED!");

            }
        }

        if (passed) {
            spede.addToResultsList(testCaseName + " test succeeded: "
                    + textBox.getString());
        }
        return passed;
    }
}
