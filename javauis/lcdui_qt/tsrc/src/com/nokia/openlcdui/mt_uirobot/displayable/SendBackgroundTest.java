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
package com.nokia.openlcdui.mt_uirobot.displayable;

import junit.framework.*;

import javax.microedition.lcdui.TextBox;

import com.nokia.openlcdui.mt_uirobot.UITestBase;

import com.nokia.mj.impl.uitestutils.Key;

/**
 * TEST CASE SPECIFICATION <br>
 * <br>
 * Short description of the module test:
 * <li> UI-robot based tests to test Displayable's isShow when MIDlet or
 * displayable is sent to background. <br>
 * <br>
 * Created: 2008-05-07
 */
public class SendBackgroundTest extends UITestBase
{

    private static final int MAX_NUM_OF_CHARS_IN_TEST_TEXTBOX = 100;
    private static final int SEND_BACKGROUND_DELAY = 300;
    private static final int SEND_FOREGROUND_DELAY = 300;

    /**
     * Constructor.
     */
    public SendBackgroundTest()
    {
        super();
    }

    /**
     * Constructor.
     *
     * @param sTestName Test name.
     * @param rTestMethod Test method.
     */
    public SendBackgroundTest(String sTestName)
    {
        super(sTestName);
    }

    public static Test suite()
    {
        TestSuite suite = new TestSuite();

        java.util.Vector methodNames;
        java.util.Enumeration e;

        // Add widget tests
        methodNames = SendBackgroundTest.methodNames();
        e = methodNames.elements();
        while(e.hasMoreElements())
        {
            suite.addTest(new SendBackgroundTest((String)e.nextElement()));
        }

        return suite;
    }

    public static java.util.Vector methodNames()
    {
        java.util.Vector methodNames = new java.util.Vector();
        methodNames.addElement("testIsShownWhenBackground");
        methodNames.addElement("testIsShownWhenNotCurrent");
        methodNames.addElement("testGetHeightAndWidth");
        return methodNames;
    }

    public void runTest() throws Throwable
    {
        if(getName().equals("testIsShownWhenBackground")) testIsShownWhenBackground();
        else if(getName().equals("testIsShownWhenNotCurrent")) testIsShownWhenNotCurrent();
        else if(getName().equals("testGetHeightAndWidth")) testGetHeightAndWidth();
        else super.runTest();
    }

    /**
     * Test that isShown()-method returns false when displayable is in
     * background.
     */
    public void testIsShownWhenBackground()
    {
        boolean testPassed = true;
        String testMsg = "";

        TextBox textBox = new TextBox("title", "content",
                                      MAX_NUM_OF_CHARS_IN_TEST_TEXTBOX, 0);

        setCurrent(textBox);

        // Send MIDlet to background and verify that isShown()
        // returns false:
        key(Key.Applications, SEND_BACKGROUND_DELAY);

        if(textBox.isShown())
        {
            testPassed = false;
            testMsg = "isShown() returned true when MIDlet was in background.";
        }

        // Move MIDlet back to foreground and verify isShown()
        // returns true:
        key(Key.CBA1, 0);
        key(Key.UpArrow, 0);
        key(Key.Select, 0);
        key(Key.Select, SEND_FOREGROUND_DELAY);

        if(!textBox.isShown())
        {
            testPassed = false;
            testMsg = "isShown() returned false when MIDlet was in foreground.";
        }

        assertTrue(getName() + " failed, " + testMsg, testPassed);
    }

    /**
     * Tests that isShown() returns false if displayable is first set current
     * and then another displayable is set current.
     */
    public void testIsShownWhenNotCurrent()
    {
        boolean testPassed = true;
        String testMsg = "";

        TextBox textBox = new TextBox("title", "content",
                                      MAX_NUM_OF_CHARS_IN_TEST_TEXTBOX, 0);
        TextBox textBox2 = new TextBox("title2", "content2",
                                       MAX_NUM_OF_CHARS_IN_TEST_TEXTBOX, 0);

        // First set displayable current and make sure it's shown:
        setCurrent(textBox);
        if(!textBox.isShown())
        {
            testPassed = false;
            testMsg = "isShown() returned false when displayable current.";
        }

        // Then set another displayable current and make sure the first
        // one isn't shown:
        setCurrent(textBox2);

        if(textBox.isShown())
        {
            testPassed = false;
            testMsg = "isShown() returned true when displayable isn't current.";
        }

        // Now move first displayable back to current and make sure
        // it knows it's shown:
        setCurrent(textBox);
        if(!textBox.isShown())
        {
            testPassed = false;
            testMsg = "isShown() returned false when displayable set "
                      + " back to current.";
        }

        assertTrue(getName() + " failed, " + testMsg, testPassed);
    }

    /**
     * Tests the functionality of getWidth() and getHeight()-methods when
     * displayable not current and when MIDlet is sent to background.
     */
    public void testGetHeightAndWidth()
    {
        boolean testPassed = true;
        String testMsg = "";
        int width = 0;
        int height = 0;

        TextBox textBox = new TextBox("title", "content",
                                      MAX_NUM_OF_CHARS_IN_TEST_TEXTBOX, 0);
        TextBox textBox2 = new TextBox("title2", "content2",
                                       MAX_NUM_OF_CHARS_IN_TEST_TEXTBOX, 0);

        // Get initial values:
        width = textBox.getWidth();
        height = textBox.getHeight();

        // Set displayable to current and make sure the dimension isn't changed:
        setCurrent(textBox);

        if((textBox.getWidth() != width) || (textBox.getHeight() != height))
        {
            testPassed = false;
            testMsg = "dimension changed after displayable set current:"
                      + " original w: " + width + " original h:" + height
                      + " new w: " + textBox.getWidth() + " new h: "
                      + textBox.getHeight();
        }

        // Send MIDlet to background and verify that dimension remains same:
        key(Key.Applications, SEND_BACKGROUND_DELAY);

        if((textBox.getWidth() != width) || (textBox.getHeight() != height))
        {
            testPassed = false;
            testMsg = "dimension changed after MIDlet sent to background:"
                      + " original w: " + width + " original h:" + height
                      + " new w: " + textBox.getWidth() + " new h: "
                      + textBox.getHeight();
        }

        // Move MIDlet back to foreground:
        key(Key.CBA1, 0);
        key(Key.UpArrow, 0);
        key(Key.Select, 0);
        key(Key.Select, SEND_FOREGROUND_DELAY);

        // Switch to second displayable and make sure the dimension
        // of first displayable remains same:
        setCurrent(textBox2);

        if((textBox.getWidth() != width) || (textBox.getHeight() != height))
        {
            testPassed = false;
            testMsg = "dimension changed after other displayable set current:"
                      + " original w: " + width + " original h:" + height
                      + " new w: " + textBox.getWidth() + " new h: "
                      + textBox.getHeight();
        }

        assertTrue(getName() + " failed, " + testMsg, testPassed);
    }

}
