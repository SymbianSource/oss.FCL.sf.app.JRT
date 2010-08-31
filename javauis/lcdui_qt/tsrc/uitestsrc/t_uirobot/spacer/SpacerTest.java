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
package t_uirobot.spacer;

import j2meunit.framework.*;

import java.io.IOException;

import javax.microedition.lcdui.*;

import spede.SpedeRunner;
import t_uirobot.ItemUITestBase;

import com.nokia.mj.impl.uitestutils.Key;

/**
 * TEST CASE SPECIFICATION <br>
 * <br>
 * Short description of the module test:
 * <li> UI-robot based test to test spacer. <br>
 * <br>
 * Created: 2008-10-20
 */
public class SpacerTest extends ItemUITestBase
{

    private Command latestCommand;
    private Item latestItem;
    private Image smallImage;

    /**
     * Constructor.
     */
    public SpacerTest()
    {
    }

    /**
     * Constructor.
     *
     * @param sTestName Test name.
     * @param rTestMethod Test method.
     */
    public SpacerTest(String sTestName, TestMethod rTestMethod)
    {
        super(sTestName, rTestMethod);
    }

    /**
     * Any pre-test setup can be done here
     */
    protected void setUp() throws Exception
    {
        super.setUp();
        try
        {
            smallImage = Image.createImage("32x32.jpeg");
        }
        catch(IOException e)
        {
            //
        }
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

        aSuite.addTest(new SpacerTest("testSizeChange",
                                      new TestMethod()
        {
            public void run(TestCase tc)
            {
                ((SpacerTest) tc).testSizeChange();
            }
        }));



        return aSuite;
    }

    /**
     * Tests the basic functionality of command added to Hyperlink.
     */
    public void testSizeChange()
    {
        ImageItem button = new ImageItem("label", smallImage, 0, "small",
                                         ImageItem.BUTTON);
        ImageItem button2 = new ImageItem("label", smallImage, 0, "small",
                                          ImageItem.BUTTON);
        Spacer spacer = new Spacer(0, 0);

        Command ok = new Command("Ok", "", Command.ITEM, 0);
        Command ok2 = new Command("Ok", "", Command.ITEM, 0);
        button.addCommand(ok);
        button.setItemCommandListener(this);
        button2.addCommand(ok2);
        button2.setItemCommandListener(this);

        form.append(button);
        form.append(spacer);
        form.append(button2);

        Display.getDisplay(getMIDlet()).setCurrent(form);
        block(CHANGE_DISPLAYABLE_DELAY);

        // Click command and verify it works:
        key(Key.CBA1);
        assertEquals("Command not activated or not correct command.(case 1)",
                     ok, latestCommand);
        assertEquals("Wrong item delivered to commandAction-method.(case 1)",
                     button, latestItem);
        latestCommand = null;
        latestItem = null;
        // Check that after arrowDown focus didn't change.
        key(Key.DownArrow);
        key(Key.CBA1);
        assertEquals("Command not activated or not correct command.(case 2)",
                     ok, latestCommand);
        assertEquals("Wrong item delivered to commandAction-method.(case 2)",
                     button, latestItem);
        latestCommand = null;
        latestItem = null;
        // change spacers's size to form width and again press arrow down.
        spacer.setMinimumSize(form.getWidth(), 1);
        // time to re-layout
        block(CHANGE_DISPLAYABLE_DELAY);
        key(Key.DownArrow);
        key(Key.CBA1);
        assertEquals("Command not activated or not correct "
                     + "command when using MSK.(case 3)", ok2, latestCommand);
        assertEquals("Wrong item delivered to commandAction-method"
                     + " when using MSK.(case 3)", button2, latestItem);
    }

    /**
     * Catches item commands and saves latest command and item to
     * member variables.
     * @param c Command
     * @param item item
     */
    public void commandAction(Command c, Item item)
    {
        latestCommand = c;
        latestItem = item;
    }
}
