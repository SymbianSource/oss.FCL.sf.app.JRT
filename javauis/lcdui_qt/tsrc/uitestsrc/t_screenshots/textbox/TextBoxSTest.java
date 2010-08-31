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

/**
 * TEST CASE SPECIFICATION <br>
 * <br>
 * Short description of the module test:
 * <li> This moduletest does screenshot based testing for TextBox. <br>
 * <br>
 * Created: 2008-05-06
 */
public class TextBoxSTest extends UITestBase
{

    private static final int LENGTH = 100;
    private static final int NEW_LENGTH = 200;
    private static final int CARET_SHIFT = 2;

    private TextBox textbox;
    private int[] inputTextShort = new int[] {'7', '8', '8', '9',
            '4', '5', '5', '5', '5',
                                             };
    private int[] inputSentence = new int [] {'3', '3', '8', '8', '8',
            '3', '3', '7', '7', '7',
                                             };

    /**
     * Empty constructor.
     */
    public TextBoxSTest()
    {
        super();
    }

    /**
     * Class constructor.
     * @param sTestName name of the test
     * @param rTestMethod TestMethod used
     */
    public TextBoxSTest(String sTestName, TestMethod rTestMethod)
    {
        super(sTestName, rTestMethod);
    }

    /***************************************
    * To create the test suite.
    * You need to add a new aSuite.addTest entry for
    * any new test methods.
    * @return aSuite.
    */
    public Test suite()
    {
        TestSuite aSuite = new TestSuite();
        aSuite.addTest(new TextBoxSTest("textboxPasswordTest()",
                                        new TestMethod()
        {
            public void run(TestCase tc)
            {
                ((TextBoxSTest) tc).textboxPasswordTest();
            }
        }
                                       ));
        aSuite.addTest(new TextBoxSTest("textboxComplexScreenshotTest()",
                                        new TestMethod()
        {
            public void run(TestCase tc)
            {
                ((TextBoxSTest) tc).textboxComplexScreenshotTest();
            }
        }
                                       ));
        return aSuite;
    }


    /**
     * Tests password input.
     */
    public void textboxPasswordTest()
    {
        textbox = new TextBox("PASSWORD", null, LENGTH, TextField.PASSWORD);
        setCurrent(textbox);

        String img = "textboxPasswordIndicator.gif";
        for(int i = 0; i < inputTextShort.length; i++)
        {
            key(inputTextShort[i], 0);
        }

        // wait for the last symbol to turn to '*'.
        block(1500);

        boolean passed = true;
        passed = takeScreenshot(img);
        assertTrue("textboxPasswordTest", passed);

        textbox = null;
    }

    /**
     * TextBox test.
     * Complex screenshot based test, designed to test as much
     * features as possible in one test.
     */
    public void textboxComplexScreenshotTest()
    {
        String getString = null;
        String setString = null;
        String insertString = "|insert()|";
        int caretposition = 0;
        int currentMaxSize = 0;
        int getSize = 0;
        String addContent = null;

        textbox = new TextBox("Complex test", "inital content", LENGTH,
                              TextField.ANY);
        setCurrent(textbox);

        String img = "textboxComplexSTest.gif";
        getString = textbox.getString();
        setString = getString + " and " + getString;
        //set the string which is copy of
        //double initial content with "and" between them
        textbox.setString(setString);
        //input "Ever"
        for(int i = 0; i < inputSentence.length; i++)
        {
            key(inputSentence[i], 0);
        }
        //get caret position.
        caretposition = textbox.getCaretPosition();
        //delete er from the last inputed
        textbox.delete((caretposition - 2), CARET_SHIFT);
        block(KEYEVENT_DELAY);
        //Insert string at position CARET_SHIFT
        textbox.insert(insertString, CARET_SHIFT);
        // use of getMaxSize()
        currentMaxSize = textbox.getMaxSize();
        //insert current max to the end of textbox
        addContent = "\ngetMaxSize() is " + currentMaxSize;
        textbox.insert(addContent, LENGTH);
        //set new maxSize
        currentMaxSize = textbox.setMaxSize(NEW_LENGTH);
        addContent = null;
        addContent = "\nsetMaxSize returns: " + currentMaxSize;
        textbox.insert(addContent, NEW_LENGTH);
        getSize = textbox.size();
        addContent = null;
        addContent = "\nsize() returns: " + getSize;
        textbox.insert(addContent, NEW_LENGTH);
        //Delay to take screenshot
        block(CHANGE_DISPLAYABLE_DELAY);

        boolean passed = true;
        passed = takeScreenshot(img);
        assertTrue("textboxComplexSTest", passed);

        textbox = null;
    }

}
