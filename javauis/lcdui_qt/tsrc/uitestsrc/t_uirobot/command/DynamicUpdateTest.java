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
package t_uirobot.command;

import j2meunit.framework.*;

import javax.microedition.lcdui.*;

import spede.SpedeRunner;
import t_uirobot.UITestBase;

import com.nokia.mj.impl.uitestutils.Key;

/**
 * TEST CASE SPECIFICATION <br>
 * <br>
 * Short description of the module test:
 * <li> UI-robot based test to add and remove commands to displayables at the
 * runtime. <br>
 * <br>
 * Created: 2008-05-02
 */
public class DynamicUpdateTest extends UITestBase implements CommandListener {

    private static final int MAX_NUM_OF_CHARS_IN_TEST_TEXTBOX = 100;

    private Command latestCommand;
    private Displayable latestDisplayable;

    private Command latestCommand2;
    private Displayable latestDisplayable2;
    private int eventCount2;
    private Command latestCommand3;
    private Displayable latestDisplayable3;
    private int eventCount3;

    /**
     * Constructor.
     */
    public DynamicUpdateTest() {
    }

    /**
     * Constructor.
     *
     * @param sTestName Test name.
     * @param rTestMethod Test method.
     */
    public DynamicUpdateTest(String sTestName, TestMethod rTestMethod) {
        super(sTestName, rTestMethod);
    }

    /**
     * Creates the test suite. You need to add a new aSuite.addTest antry for
     * any new test methods, otherwise they won't be run.
     *
     * @return New testsuite.
     */
    public Test suite() {
        TestSuite aSuite = new TestSuite();

        aSuite.addTest(new DynamicUpdateTest("testAddAndRemoveManyCommands",
                new TestMethod() {
                    public void run(TestCase tc) {
                        ((DynamicUpdateTest) tc).testAddAndRemoveManyCommands();
                    }
                }));

        aSuite.addTest(new DynamicUpdateTest("testCommandInEmptyForm",
                new TestMethod() {
                    public void run(TestCase tc) {
                        ((DynamicUpdateTest) tc).testCommandInEmptyForm();
                    }
                }));

        aSuite.addTest(new DynamicUpdateTest(
                "testAddCommandToManyDisplayables", new TestMethod() {
                    public void run(TestCase tc) {
                        ((DynamicUpdateTest) tc)
                                .testAddCommandToManyDisplayables();
                    }
                }));

        aSuite.addTest(new DynamicUpdateTest("testManyCommandListeners",
                new TestMethod() {
                    public void run(TestCase tc) {
                        ((DynamicUpdateTest) tc).testManyCommandListeners();
                    }
                }));

        return aSuite;
    }

    /**
     * Adds and removes many kind of commands to displayable and makes sure that
     * events occurs correctly all the time.
     */
    public void testAddAndRemoveManyCommands() {
        boolean testPassed = true;
        String testMsg = "";
        latestCommand = null;
        latestDisplayable = null;

        TextBox textBox = new TextBox("title", "content",
                MAX_NUM_OF_CHARS_IN_TEST_TEXTBOX, 0);

        Command item1 = new Command("item", "item", Command.ITEM, 0);
        Command item2 = new Command("item2", "item2", Command.ITEM, 1);
        Command item3 = new Command("item3", "item3", Command.ITEM, 2);
        Command exit1 = new Command("exit1", "exit1", Command.EXIT, 0);
        Command exit2 = new Command("exit2", "exit2", Command.EXIT, 0);
        Command ok1 = new Command("ok1", "ok1", Command.OK, 0);
        Command ok2 = new Command("ok2", "ok2", Command.OK, 1);
        Command ok3 = new Command("ok3", "ok3", Command.OK, 2);
        Command cancel = new Command("cancel1", "cancel1", Command.CANCEL, 0);
        Command screen1 = new Command("screen1", "screen1", Command.SCREEN, 0);
        Command screen2 = new Command("screen2", "screen2", Command.SCREEN, 1);

        textBox.setCommandListener(this);

        setCurrent(textBox);

        textBox.addCommand(item1);

        // If there's only one command of type ITEM it's mapped to LSK.
        // Click it:
        key(Key.CBA1);

        if (latestCommand == null) {
            testPassed = false;
            testMsg = "no event from ITEM-command.";
        }
        else if (latestCommand != item1) {
            testPassed = false;
            testMsg = "not ITEM-command in event.";
        }
        else if (latestDisplayable != textBox) {
            testPassed = false;
            testMsg = "unexpected displayable in event.";
        }

        // Add new command of type OK. It should be mapped to middle soft
        // button. Verify that by clicking MSK:
        latestCommand = null;
        latestDisplayable = null;
        textBox.addCommand(ok1);
        block(CHANGE_DISPLAYABLE_DELAY);

        key(Key.Select);

        if (latestCommand == null) {
            testPassed = false;
            testMsg = "no event from OK-command.";
        }
        else if (latestCommand != ok1) {
            testPassed = false;
            testMsg = "not OK-command in event.";
        }
        else if (latestDisplayable != textBox) {
            testPassed = false;
            testMsg = "unexpected displayable in event.";
        }

        // Add lot of new commands and try to click one in menu:
        latestCommand = null;
        latestDisplayable = null;
        textBox.addCommand(item2);
        textBox.addCommand(item3);
        textBox.addCommand(exit1);
        textBox.addCommand(exit2);
        textBox.addCommand(ok2);
        textBox.addCommand(ok3);
        textBox.addCommand(cancel);
        textBox.addCommand(screen1);
        textBox.addCommand(screen2);
        block(CHANGE_DISPLAYABLE_DELAY);

        key(Key.CBA1, 0);
        key(Key.DownArrow, 0);
        key(Key.DownArrow, 0);
        key(Key.Select);

        if (latestCommand == null) {
            testPassed = false;
            testMsg = "no event from EXIT2-command.";
        }
        else if (latestCommand != exit2) {
            testPassed = false;
            testMsg = "not EXIT2-command in event.";
        }
        else if (latestDisplayable != textBox) {
            testPassed = false;
            testMsg = "unexpected displayable in event.";
        }

        // Next remove some of the commands and again click one in menu:
        latestCommand = null;
        latestDisplayable = null;
        textBox.removeCommand(exit2);
        textBox.removeCommand(ok1);
        block(CHANGE_DISPLAYABLE_DELAY);

        key(Key.CBA1, 0);
        key(Key.DownArrow, 0);
        key(Key.DownArrow, 0);
        key(Key.Select);

        if (latestCommand == null) {
            testPassed = false;
            testMsg = "no event from SCREEN1-command.";
        }
        else if (latestCommand != screen1) {
            testPassed = false;
            testMsg = "not SCREEN1-command in event.";
        }
        else if (latestDisplayable != textBox) {
            testPassed = false;
            testMsg = "unexpected displayable in event.";
        }

        // Finally remove all commands and verify the commands are really
        // removed
        // by pressing MSK, LSK and RSK:
        latestCommand = null;
        latestDisplayable = null;
        textBox.removeCommand(item1);
        textBox.removeCommand(item2);
        textBox.removeCommand(item3);
        textBox.removeCommand(ok1);
        textBox.removeCommand(ok2);
        textBox.removeCommand(ok3);
        textBox.removeCommand(screen1);
        textBox.removeCommand(screen2);
        textBox.removeCommand(cancel);
        textBox.removeCommand(exit1);

        block(CHANGE_DISPLAYABLE_DELAY);

        key(Key.CBA1, 0);
        key(Key.CBA2, 0);
        key(Key.Select);

        if (latestCommand != null) {
            testPassed = false;
            testMsg = "Event occured without commands.";
        }

        assertTrue(getName() + " failed, " + testMsg, testPassed);
    }

    /**
     * Test to add command to empty form and verify it works.
     */
    public void testCommandInEmptyForm() {
        latestCommand = null;
        latestDisplayable = null;

        Form form = new Form("form");
        Command ok = new Command("Ok", "", Command.ITEM, 0);
        form.addCommand(ok);
        form.setCommandListener(this);

        setCurrent(form);

        // If there's only one command of type ITEM it's mapped to LSK.
        // Click it:
        key(Key.CBA1);

        assertEquals("Command not activated or not correct command.",
                ok, latestCommand);
        assertEquals("Wrong displayable delivered to commandAction-method.",
                form, latestDisplayable);
    }

    /**
     * Tests that it's possible to add one command to many displayables and
     * removing command from one displayable doesn't affect another displayable.
     */
    public void testAddCommandToManyDisplayables() {
        boolean testPassed = true;
        String testMsg = "";
        latestCommand = null;
        latestDisplayable = null;

        TextBox textBox = new TextBox("title", "content",
                MAX_NUM_OF_CHARS_IN_TEST_TEXTBOX, 0);
        TextBox textBox2 = new TextBox("title2", "content2",
                MAX_NUM_OF_CHARS_IN_TEST_TEXTBOX, 0);

        Command item1 = new Command("item", "item", Command.ITEM, 0);

        textBox.setCommandListener(this);
        textBox2.setCommandListener(this);

        // Add command to two displayables:
        textBox.addCommand(item1);
        textBox2.addCommand(item1);

        // Verify that command works in first displayable:
        setCurrent(textBox);

        key(Key.CBA1);

        if (latestCommand == null) {
            testPassed = false;
            testMsg = "no event from ITEM-command in first displayable.";
        }
        else if (latestCommand != item1) {
            testPassed = false;
            testMsg = "not ITEM-command in event in first displayable.";
        }
        else if (latestDisplayable != textBox) {
            testPassed = false;
            testMsg = "unexpected displayable in event.";
        }

        // Switch to second displayable and verify that command works
        // in that displayable too:
        latestCommand = null;
        latestDisplayable = null;
        setCurrent(textBox2);

        key(Key.CBA1);

        if (latestCommand == null) {
            testPassed = false;
            testMsg = "no event from ITEM-command in second displayable.";
        }
        else if (latestCommand != item1) {
            testPassed = false;
            testMsg = "not ITEM-command in event in second displayable.";
        }
        else if (latestDisplayable != textBox2) {
            testPassed = false;
            testMsg = "unexpected displayable in event.";
        }

        // Remove command from first displayable and verify that it still works
        // in second one:
        latestCommand = null;
        latestDisplayable = null;
        textBox.removeCommand(item1);

        key(Key.CBA1);

        if (latestCommand == null) {
            testPassed = false;
            testMsg = "no event from ITEM-command in second displayable.";
        }
        else if (latestCommand != item1) {
            testPassed = false;
            testMsg = "not ITEM-command in event in second displayable.";
        }
        else if (latestDisplayable != textBox2) {
            testPassed = false;
            testMsg = "unexpected displayable in event.";
        }

        // Add command again to first displayable, remove it from second one,
        // switch to first displayable and verify that command works in
        // the first displayable:
        latestCommand = null;
        latestDisplayable = null;
        textBox.addCommand(item1);
        textBox2.removeCommand(item1);
        setCurrent(textBox);

        key(Key.CBA1);

        if (latestCommand == null) {
            testPassed = false;
            testMsg = "no event from ITEM-command in first displayable.";
        }
        else if (latestCommand != item1) {
            testPassed = false;
            testMsg = "not ITEM-command in event in first displayable.";
        }
        else if (latestDisplayable != textBox) {
            testPassed = false;
            testMsg = "unexpected displayable in event.";
        }

        assertTrue(getName() + " failed, " + testMsg, testPassed);
    }

    /**
     * Tests the functionality of Command and CommandListener when there are
     * more than one CommandListener listening one Command.
     */
    public void testManyCommandListeners() {
        boolean testPassed = true;
        String testMsg = "";
        latestCommand2 = null;
        latestDisplayable2 = null;
        eventCount2 = 0;
        latestCommand3 = null;
        latestDisplayable3 = null;
        eventCount3 = 0;

        // Create two Displayables and two CommandListeners and add
        // same Command to both Displayables:
        TextBox textBox = new TextBox("title", "content",
                MAX_NUM_OF_CHARS_IN_TEST_TEXTBOX, 0);
        TextBox textBox2 = new TextBox("title2", "content2",
                MAX_NUM_OF_CHARS_IN_TEST_TEXTBOX, 0);
        Command item1 = new Command("item", "item", Command.ITEM, 0);

        textBox.setCommandListener(new CommandListener() {
            public void commandAction(Command c, Displayable d) {
                latestCommand2 = c;
                latestDisplayable2 = d;
                eventCount2++;
            }
        });

        textBox2.setCommandListener(new CommandListener() {
            public void commandAction(Command c, Displayable d) {
                latestCommand3 = c;
                latestDisplayable3 = d;
                eventCount3++;
            }
        });

        textBox.addCommand(item1);
        textBox2.addCommand(item1);

        // Set second displayable current and try to activate the command:
        setCurrent(textBox2);

        key(Key.CBA1);

        if (latestCommand3 == null) {
            testPassed = false;
            testMsg = "no event in second displayable.";
        }
        else if (latestCommand3 != item1) {
            testPassed = false;
            testMsg = "wrong command in second displayable.";
        }
        else if (latestDisplayable3 != textBox2) {
            testPassed = false;
            testMsg = "unexpected displayable in event.";
        }
        else if (eventCount3 != 1) {
            testPassed = false;
            testMsg = "Unexpected number of events: " + eventCount3;
        }

        // Switch to first displayable and try to activate the command:
        setCurrent(textBox);

        key(Key.CBA1);

        if (latestCommand2 == null) {
            testPassed = false;
            testMsg = "no event in first displayable.";
        }
        else if (latestCommand2 != item1) {
            testPassed = false;
            testMsg = "wrong command in first displayable.";
        }
        else if (latestDisplayable2 != textBox) {
            testPassed = false;
            testMsg = "unexpected displayable in event.";
        }
        else if (eventCount2 != 1) {
            testPassed = false;
            testMsg = "Unexpected number of events: " + eventCount2;
        }
        else if (eventCount3 != 1) {
            testPassed = false;
            testMsg = "Unexpected number of events in second displayable: "
                    + eventCount2;
        }

        // Clear counters, remove command from second displayable and
        // activate it in first displayable again:
        latestCommand2 = null;
        latestDisplayable2 = null;
        eventCount2 = 0;
        latestCommand3 = null;
        latestDisplayable3 = null;
        eventCount3 = 0;

        textBox2.removeCommand(item1);

        key(Key.CBA1);

        if (latestCommand2 == null) {
            testPassed = false;
            testMsg = "no event in first displayable.";
        }
        else if (latestCommand2 != item1) {
            testPassed = false;
            testMsg = "wrong command in first displayable.";
        }
        else if (latestDisplayable2 != textBox) {
            testPassed = false;
            testMsg = "unexpected displayable in event.";
        }
        else if (eventCount2 != 1) {
            testPassed = false;
            testMsg = "Unexpected number of events: " + eventCount2;
        }
        else if (eventCount3 != 0) {
            testPassed = false;
            testMsg = "Unexpected number of events in second displayable: "
                    + eventCount3;
        }
        else if (latestCommand3 != null) {
            testPassed = false;
            testMsg = "Unexpected event in second displayable.";
        }

        assertTrue(getName() + " failed, " + testMsg, testPassed);
    }

    /**
     * Catches commands and saves Command as well as Displayable to local member
     * variables.
     *
     * @param c Command.
     * @param d Displayable.
     */
    public void commandAction(Command c, Displayable d) {
        latestCommand = c;
        latestDisplayable = d;
    }

}
