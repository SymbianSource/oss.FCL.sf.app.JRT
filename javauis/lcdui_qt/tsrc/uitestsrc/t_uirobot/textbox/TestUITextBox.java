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
package t_uirobot.textbox;

import j2meunit.framework.*;

import javax.microedition.lcdui.TextBox;
import javax.microedition.lcdui.TextField;

import t_uirobot.UITestBase;

/**
 * TEST CASE SPECIFICATION
 *
 * <br><br>Short description of the module test:
 * <li> UI-robot based test for TextBox.
 *
 * <br><br>
 * Created:    2008-04-25
 */
public class TestUITextBox extends UITestBase {

    private static final int LENGTH = 100;
    private static final int EXPECTED_CARET_POSITION = 5;

    private TextBox textbox;
    private int[] inputTextShort = new int[]{'7', '8', '8', '9',
                                             '4', '5', '5', '5', '5', };
    private int[] inputSentence = new int [] {'3', '3', '8', '8', '8',
                                              '3', '3', '7', '7', '7',
                                              '9', '9', '9', '1', '0', '9',
                                              '6', '6', '6', '7', '7',
                                              '7', '3', '0', '4', '4',
                                              '4', '7', '7', '7', '7',
                                              '0', '2', '2', '2', };

    /**
     * Constructor.
     */
    public TestUITextBox() {
    }

    /**
     * Constructor.
     *
     * @param sTestName name of the test
     * @param rTestMethod TestMethod used
     */
    public TestUITextBox(String sTestName, TestMethod rTestMethod) {
        super(sTestName, rTestMethod);
    }

    /**
     * Creates new test suite.You need to add a new aSuite.addTest entry for
     * any new test methods, otherwise they won't be run.
     * @return suite
     */
    public Test suite() {
        TestSuite aSuite = new TestSuite();

        aSuite.addTest(new TestUITextBox("testNormalInputMode()",
                new TestMethod() {
                    public void run(TestCase tc) {
                        ((TestUITextBox) tc).testNormalInputMode();
                    }
                }));
        aSuite.addTest(new TestUITextBox("testNonEditable()", new TestMethod() {
            public void run(TestCase tc) {
                ((TestUITextBox) tc).testNonEditable();
            }
        }));
        aSuite.addTest(new TestUITextBox("testCapSent()", new TestMethod() {
            public void run(TestCase tc) {
                ((TestUITextBox) tc).testCapSent();
            }
        }));
        aSuite.addTest(new TestUITextBox("testGetCaretPosition()",
                new TestMethod() {
                    public void run(TestCase tc) {
                        ((TestUITextBox) tc).testGetCaretPosition();
                    }
                }));
        return aSuite;
    }

    /**
     * This method tests that TextBox getString, returns
     * correct text which was inputed with UIRobot.
     */
    public void testNormalInputMode() {
        textbox = new TextBox("ANY", "", LENGTH, TextField.ANY);
        String getString;

        setCurrent(textbox);
        for (int i = 0; i < inputTextShort.length; i++) {
            key(inputTextShort[i], 0);
        }
        getString = textbox.getString();
        assertTrue("Expected: TextBox.getText() returns: "
                + "Puwg5. But returned "
                + getString, getString.equals("Puwg5"));
        textbox = null;
    }

    /**
     * Test that TextBox is not editable if
     * UNEDITABLE is specified in constructor.
     */
    public void testNonEditable() {
        textbox = new TextBox("No Edit", "editing is not allowed", LENGTH,
                TextField.UNEDITABLE | TextField.ANY);
        String getString;

        setCurrent(textbox);
        for (int i = 0; i < inputTextShort.length; i++) {
            key(inputTextShort[i], 0);
        }
        getString = textbox.getString();
        assertTrue("Expected: TextBox.getText() returns: "
                + "editing is not allowed. But returned " + getString,
                getString.equals("editing is not allowed"));
        textbox = null;
    }

    /**
     * Test that TextBox behaves correctly if INITIAL_CAPS_WORD specified.
     * every word should start with capital letter.
     * TODO: waiting for fixes from eSWT.
     */
    public void testCapWord() {
        textbox = new TextBox("Capital Words", "", LENGTH,
                TextField.INITIAL_CAPS_WORD | TextField.ANY);
        String getString;

        setCurrent(textbox);
        for (int i = 0; i < inputSentence.length; i++) {
            key(inputSentence[i], 0);
        }
        getString = textbox.getString();
        assertTrue("Expected: TextBox.getText() returns: "
                + "Every. Word Is C. But returned " + getString, getString
                .equals("Every. Word Is C"));
        textbox = null;

    }

    /**
     * Test that TextBox behaves correctly if INITIAL_CAPS_SENTENCE specified.
     * every sentence should start with capital letter.
     */
    public void testCapSent() {
        textbox = new TextBox("Capital Sentence", "", LENGTH,
                TextField.INITIAL_CAPS_SENTENCE | TextField.ANY);
        String getString;

        setCurrent(textbox);
        for (int i = 0; i < inputSentence.length; i++) {
            key(inputSentence[i], 0);
        }
        getString = textbox.getString();
        assertTrue("Expected: TextBox.getText() return: "
                + "Every word is c. But returned " + getString, getString
                .equals("Every. Word is c"));
        textbox = null;
    }

    /**
     * TextBox.getCaretPosition() test.
     * Tests that getCaretPosition works correctly.
     */
    public void testGetCaretPosition() {
        int getCaretPosition = 0;
        textbox = new TextBox("Get Caret", "", LENGTH,
                TextField.INITIAL_CAPS_SENTENCE | TextField.ANY);

        setCurrent(textbox);
        for (int i = 0; i < inputTextShort.length; i++) {
            key(inputTextShort[i], 0);
        }

        getCaretPosition = textbox.getCaretPosition();
        assertTrue("TextBox.getCaretPosition(). Expected: returns "
                + EXPECTED_CARET_POSITION + " but got " + getCaretPosition,
                getCaretPosition == EXPECTED_CARET_POSITION);
        textbox = null;
    }

}
