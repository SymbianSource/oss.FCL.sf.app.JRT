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

import javax.microedition.lcdui.Command;
import javax.microedition.lcdui.TextBox;

import t_uirobot.UITestBase;

import com.nokia.mj.impl.uitestutils.Key;

/**
 * TEST CASE SPECIFICATION <br>
 * <br>
 * Short description of the module test:
 * <li> UI-robot based tests to test Displayable's Command-related
 * functionality. <br>
 * <br>
 * Created: 2008-04-30
 */
public class AddCommandTest extends UITestBase
{

    private static final int MAX_CHARS = 100;

    public AddCommandTest()
    {
        super();
    }

    public AddCommandTest(String name, TestMethod method)
    {
        super(name, method);
    }

    /**
    /**
     * Creates the test suite. You need to add a new aSuite.addTest antry for
     * any new test methods, otherwise they won't be run.
     *
     * @return New testsuite.
     */
    public Test suite()
    {
        TestSuite aSuite = new TestSuite();

        aSuite.addTest(new AddCommandTest("testAddCommandBeforeListener",
                                          new TestMethod()
        {
            public void run(TestCase tc)
            {
                ((AddCommandTest) tc).testAddCommandBeforeListener();
            }
        }));

        aSuite.addTest(new AddCommandTest("testAddCommandAfterListener",
                                          new TestMethod()
        {
            public void run(TestCase tc)
            {
                ((AddCommandTest) tc).testAddCommandAfterListener();
            }
        }));

        aSuite.addTest(new AddCommandTest("testRemoveListener",
                                          new TestMethod()
        {
            public void run(TestCase tc)
            {
                ((AddCommandTest) tc).testRemoveListener();
            }
        }));

        aSuite.addTest(new AddCommandTest("testAddToManyDisplayables",
                                          new TestMethod()
        {
            public void run(TestCase tc)
            {
                ((AddCommandTest) tc).testAddToManyDisplayables();
            }
        }));

        aSuite.addTest(new AddCommandTest("testAddCommandTwice",
                                          new TestMethod()
        {
            public void run(TestCase tc)
            {
                ((AddCommandTest) tc).testAddCommandTwice();
            }
        }));

        return aSuite;
    }

    /**
     * Test command adding before CommandListener is set.
     */
    public void testAddCommandBeforeListener()
    {
        TextBox textBox = new TextBox("title", "content", MAX_CHARS, 0);
        Command c1 = new Command("item1", "item1", Command.ITEM, 0);

        textBox.addCommand(c1);
        textBox.setCommandListener(this);

        setCurrent(textBox);

        // If there's only one command of type ITEM it's mapped to LSK.
        // Click it:
        key(Key.CBA1);
        assertCmdListener("", c1, textBox);
    }

    /**
     * Test command adding after CommandListener is set.
     */
    public void testAddCommandAfterListener()
    {
        TextBox textBox = new TextBox("title", "content", MAX_CHARS, 0);
        Command c1 = new Command("item1", "item1", Command.ITEM, 0);

        textBox.setCommandListener(this);
        textBox.addCommand(c1);

        setCurrent(textBox);

        // If there's only one command of type ITEM it's mapped to LSK.
        // Click it:
        key(Key.CBA1);
        assertCmdListener("", c1, textBox);
    }

    /**
     * Test that removing CommandListener will work and no commands are
     * delivered to CommandListener anymore.
     */
    public void testRemoveListener()
    {
        TextBox textBox = new TextBox("title", "content", MAX_CHARS, 0);
        Command c1 = new Command("item1", "item1", Command.ITEM, 0);

        textBox.setCommandListener(this);
        textBox.addCommand(c1);

        setCurrent(textBox);

        // Remove commandListener by setting it to null:
        textBox.setCommandListener(null);

        // If there's only one command of type ITEM it's mapped to LSK.
        // Click it:
        key(Key.CBA1);
        assertCmdListener("event still occured", null, null);
    }

    /**
     * In LCDUI it's possible to add same Command to many Displayables. This
     * test tests that events occurs correctly in such case.
     */
    public void testAddToManyDisplayables()
    {
        // There are two Displayables and one Command in this test:
        TextBox textBox = new TextBox("title", "content", MAX_CHARS, 0);
        TextBox textBox2 = new TextBox("title2", "content2", MAX_CHARS, 0);
        Command c1 = new Command("item1", "item1", Command.ITEM, 0);

        // Add Command to both Displayables but listener only to first one:
        textBox.addCommand(c1);
        textBox2.addCommand(c1);
        textBox.setCommandListener(this);

        // Make sure that event occur when first displayable is visible:
        setCurrent(textBox);

        key(Key.CBA1);
        assertCmdListener("", c1, textBox);

        // Now switch to second displayable and make sure that event
        // doesn't occur:
        setCurrent(textBox2);

        key(Key.Select);
        assertCmdListener("No listener on second displayable", null, null);

        // Remove commandListener from first displayable and add it
        // to second one (which is currently visible):
        textBox.setCommandListener(null);
        textBox2.setCommandListener(this);

        // Make sure that event occurs now when pressing the key:
        key(Key.CBA1);
        assertCmdListener("No event on second displayable", c1, textBox2);

        // And then switch back to first displayable (which doesn't have
        // listener
        // at the moment) and make sure no event occur:
        setCurrent(textBox);

        key(Key.CBA1);
        assertCmdListener("No listener on first displayable", null, null);

        // Finally remove listener from second displayable also and switch
        // to it and make sure no event occurs:
        textBox2.setCommandListener(null);
        setCurrent(textBox2);

        key(Key.CBA1);
        assertCmdListener("All listeners removed", null, null);
    }

    /**
     * Tests that nothing happens if command is added to the displayable which
     * already contains the command.
     */
    public void testAddCommandTwice()
    {
        TextBox textBox = new TextBox("title", "content", MAX_CHARS, 0);
        Command c1 = new Command("item1", "item1", Command.ITEM, 0);

        textBox.setCommandListener(this);
        setCurrent(textBox);

        // Add same command two times to the same displayable. Second add
        // should do nothing:
        textBox.addCommand(c1);
        textBox.addCommand(c1);

        // If command is added twice, there should be menu and pressing
        // LSK should not cause event. So press LSK and if event occurs
        // then there's only one command displayed as expected:
        key(Key.CBA1);
        assertCmdListener("Same command appears twice", c1, textBox);
    }

}
