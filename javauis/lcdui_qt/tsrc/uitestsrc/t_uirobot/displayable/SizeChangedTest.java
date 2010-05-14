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
package t_uirobot.displayable;

import j2meunit.framework.*;

import javax.microedition.lcdui.*;

import t_uirobot.UITestBase;

/**
 * TEST CASE SPECIFICATION <br>
 * <br>
 * Short description of the module test:
 * <li> UI-robot based tests to test Displayable's sizeChanged-method. <br>
 * <br>
 * Created: 2008-05-19
 */
public class SizeChangedTest extends UITestBase
{

    private static final int MAX_CHARS = 100;
    private static final int DELAY_AFTER_RESOLUTION_CHANGE = 300;

    private boolean sizeChangedFlag;
    private int newWidth;
    private int newHeight;
    private boolean firstCallFailure;

    /**
     * Constructor.
     */
    public SizeChangedTest()
    {
        super();
    }

    /**
     * Constructor.
     *
     * @param sTestName Test name.
     * @param rTestMethod Test method.
     */
    public SizeChangedTest(String sTestName, TestMethod rTestMethod)
    {
        super(sTestName, rTestMethod);
    }

    /**
     * Creates the test suite. You need to add a new aSuite.addTest antry for
     * any new test methods, otherwise they won't be run.
     *
     * @return New testsuite.
     */
    public Test suite()
    {
        TestSuite aSuite = new TestSuite();

        aSuite.addTest(new SizeChangedTest("testSizeChange", new TestMethod()
        {
            public void run(TestCase tc)
            {
                ((SizeChangedTest) tc).testSizeChange();
            }
        }));

        return aSuite;
    }

    /**
     * Creates class that extends Displayable and implements
     * sizeChanged()-method. Then verifies the method is called as described in
     * API spesification.
     */
    public void testSizeChange()
    {
        boolean testPassed = true;
        String testMsg = "";
        int width;
        int height;

        TextBox textBox = new TextBox("title", "content", MAX_CHARS, 0);
        SizeChangeListener scl = new SizeChangeListener();

        // Verify that sizeChanged() is called once before
        // Displayable is set to visible:
        setCurrent(scl);

        if(firstCallFailure)
        {
            testPassed = false;
            testMsg = "sizeChanged not called before"
                      + " displayable set to visible.";
        }
        sizeChangedFlag = false;
        width = newWidth;
        height = newHeight;

        // Change the size of the displayable by switching
        // different resolution (this emulates dynamic change from
        // portrait to landscape screen):
        spede().switchResolution();

        // Verify that sizeChanged() is called and the
        // width and height values are not the same as before:
        if(!sizeChangedFlag)
        {
            testPassed = false;
            testMsg = "sizeChanged not called when"
                      + " screen dimension changed.";
        }

        else if(width == newWidth)
        {
            testPassed = false;
            testMsg = "width not updated when" + " screen dimension changed.";
        }
        else if(height == newHeight)
        {
            testPassed = false;
            testMsg = "height not updated when" + " screen dimension changed.";
        }

        sizeChangedFlag = false;
        width = newWidth;
        height = newHeight;

        // Set displayable invisible by setting other displayable
        // current:
        setCurrent(textBox);

        // Change the size of the invisible displayable by
        // changing the screen resolution back to original:
        for(int i = 0; i < spede().getNumberofResolutions() - 1; i++)
        {
            spede().switchResolution();
        }
        block(DELAY_AFTER_RESOLUTION_CHANGE);

        // Verify that sizeChanged() is called at least
        // once when displayable in background is set to current again.
        // Also make sure the width and height are updated.
        setCurrent(scl);

        if(!sizeChangedFlag)
        {
            testPassed = false;
            testMsg = "sizeChanged not called when screen"
                      + " dimension changed and displayable in background.";
        }

        else if(width == newWidth)
        {
            testPassed = false;
            testMsg = "width not updated when screen"
                      + " dimension changed and displayable in background.";
        }
        else if(height == newHeight)
        {
            testPassed = false;
            testMsg = "height not updated when screen"
                      + " dimension changed and displayable in background.";
        }

        assertTrue(getName() + " failed, " + testMsg, testPassed);
    }

    /**
     * Inner class which implements the sizeChanged-method. It's prohibited to
     * extend Displayable directly. That's why Canvas is extended here. Another
     * possible class would be Form.
     */
    class SizeChangeListener extends Canvas
    {

        private boolean firstCall = true;

        /* (non-Javadoc)
         * @see javax.microedition.lcdui.Canvas#sizeChanged(int, int)
         */
        protected void sizeChanged(int w, int h)
        {
            if(firstCall)
            {
                // sizeChanged must be called at least once
                // before displayable is set to visible.
                // Make sure the displayable is not visible yet:
                if(isShown())
                {
                    firstCallFailure = true;
                }
                firstCall = false;
            }

            sizeChangedFlag = true;
            newWidth = w;
            newHeight = h;
        }

        protected void paint(Graphics g)
        {
        }
    }
}
