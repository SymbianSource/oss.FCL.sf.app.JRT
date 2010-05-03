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
package t_uirobot.display;

import j2meunit.framework.*;

import javax.microedition.lcdui.*;

import t_uirobot.UITestBase;

import com.nokia.mj.impl.uitestutils.Key;

/**
 * TEST CASE SPECIFICATION <br>
 * <br>
 * Short description of the module test:
 * <li> UI-robot based test to test setCurrentItem()-method in Display. <br>
 * <br>
 * Created: 2008-08-26
 */
public class SetCurrentItemTest extends UITestBase implements
        ItemCommandListener {

    private static int count1;
    private static int count2;
    private Command latestCommand;
    private Item latestItem;

    private Command c1 = new Command("Ok1", "", Command.ITEM, 0);
    private Command c2 = new Command("Ok2", "", Command.ITEM, 0);
    private Command c3 = new Command("Ok3", "", Command.ITEM, 0);

    private StringItem[] sia = new StringItem[20];

    private StringItem button1 = new StringItem("", "click!", Item.BUTTON);
    private StringItem button2 = new StringItem("", "click me 2!", Item.BUTTON);
    private StringItem button3 = new StringItem("", "click me 3!", Item.BUTTON);

    /**
     * Constructor.
     */
    public SetCurrentItemTest() {
    }

    /**
     * Constructor.
     *
     * @param sTestName Test name.
     * @param rTestMethod Test method.
     */
    public SetCurrentItemTest(String sTestName, TestMethod rTestMethod) {
        super(sTestName, rTestMethod);
    }

    /**
     * Any pre-test setup can be done here
     */
    protected void setUp() throws Exception {
        super.setUp();
		for (int i = 0; i < sia.length; i++) {
			sia[i] = new StringItem("", "click" + (i + 1), Item.BUTTON);
		}
    }

    /**
     * Creates the test suite. You need to add a new aSuite.addTest antry for
     * any new test methods, otherwise they won't be run.
     *
     * @return New testsuite.
     */
    public Test suite() {
        TestSuite aSuite = new TestSuite();

        aSuite.addTest(new SetCurrentItemTest("testExceptions",
                new TestMethod() {
                    public void run(TestCase tc) {
                        ((SetCurrentItemTest) tc).testExceptions();
                    }
                }));

        aSuite.addTest(new SetCurrentItemTest("testSetCurrentItem",
                new TestMethod() {
                    public void run(TestCase tc) {
                        ((SetCurrentItemTest) tc).testSetCurrentItem();
                    }
                }));


        aSuite.addTest(new SetCurrentItemTest("testCrash",
                new TestMethod() {
                    public void run(TestCase tc) {
                        ((SetCurrentItemTest) tc).testCrash();
                    }
                }));
        aSuite.addTest(new SetCurrentItemTest("displayableSwithTest",
                new TestMethod() {
                    public void run(TestCase tc) {
                        ((SetCurrentItemTest) tc).displayableSwithTest();
                    }
                }));

        return aSuite;
    }

    /**
     * Makes sure that setCurrentItem() throws exceptions as
     * expected.
     */
    public void testExceptions() {
        boolean exceptionThrown = false;

        StringItem button1 = new StringItem("", "click!", Item.BUTTON);

        //Test null item:
        try {
            display.setCurrentItem(null);
        }
        catch (NullPointerException ex) {
            exceptionThrown = true;
        }

        if (!exceptionThrown) {
            fail("No exception when item was null.");
        }

        //Test to call method when item is not owned by a container:
        exceptionThrown = false;
        try {
            display.setCurrentItem(button1);
        }
        catch (IllegalStateException ex) {
            exceptionThrown = true;
        }

        if (!exceptionThrown) {
            fail("No exception when item was not owned by a container.");
        }
    }

    /**
     * Tests the functionality of setCurrentItem()-method. This test tests
     * only functional things. setCurrentItem()-method must set item
     * visible, is it focusable or not, but the check of item's visibility
     * is out of scope of this test.
     */
    public void testSetCurrentItem() {
        latestCommand = null;
        latestItem = null;

        Form form = new Form("form");
        Form form2 = new Form("form2");

        StringItem button1 = new StringItem("", "click!", Item.BUTTON);
        StringItem button2 = new StringItem("", "click me2", Item.BUTTON);
        StringItem[] lotOfButtons = new StringItem[25];
        for (int i = 0; i < lotOfButtons.length; i++) {
            lotOfButtons[i] = new StringItem(
                    "", "button " + (i + 1), Item.BUTTON);
        }

        Command ok = new Command("Ok", "", Command.ITEM, 0);
        button1.addCommand(ok);
        button1.setItemCommandListener(this);

        //Test method when no displayable set:
        form.append(button1);
        display.setCurrentItem(button1);
        block(CHANGE_DISPLAYABLE_DELAY);

        key(Key.Select);

        assertEquals("Command not activated or not correct command"
                + " (case 1).",
                ok, latestCommand);
        assertEquals("Wrong item delivered to commandAction-method"
                + " (case 1).",
                button1, latestItem);
        assertEquals("Unexpected displayable"
                + " (case 1).",
                form, display.getCurrent());

        latestCommand = null;
        latestItem = null;

        //Test method when item is in current displayable:
        display.setCurrentItem(button1);
        block(CHANGE_DISPLAYABLE_DELAY);

        key(Key.Select);

        assertEquals("Command not activated or not correct command"
                + " (case 2).",
                ok, latestCommand);
        assertEquals("Wrong item delivered to commandAction-method"
                + " (case 2).",
                button1, latestItem);
        assertEquals("Unexpected displayable"
                + " (case 2).",
                form, display.getCurrent());

        latestCommand = null;
        latestItem = null;

        //Test method when item is in another displayable:
        form.deleteAll();
        form2.append(button1);

        display.setCurrentItem(button1);
        block(CHANGE_DISPLAYABLE_DELAY);

        key(Key.Select);

        assertEquals("Command not activated or not correct command"
                + " (case 3).",
                ok, latestCommand);
        assertEquals("Wrong item delivered to commandAction-method"
                + " (case 3).",
                button1, latestItem);
        assertEquals("Unexpected displayable"
                + " (case 3).",
                form2, display.getCurrent());

        latestCommand = null;
        latestItem = null;

        //Test method when item is in another displayable
        //and there are lot of items before the item:
        form2.deleteAll();
        for (int i = 0; i < lotOfButtons.length; i++) {
            form.append(lotOfButtons[i]);
        }
        form.append(button1);
        display.setCurrentItem(button1);
        block(CHANGE_DISPLAYABLE_DELAY);

        //Setting focus to last item takes some time, so wait:
        block(500);
        key(Key.Select);
        assertEquals("Command not activated or not correct command"
                + " (case 4).",
                ok, latestCommand);
        assertEquals("Wrong item delivered to commandAction-method"
                + " (case 4).",
                button1, latestItem);
        assertEquals("Unexpected displayable"
                + " (case 4).",
                form, display.getCurrent());

        latestCommand = null;
        latestItem = null;

        //Test method when item is in current displayable
        //and there are lot of items before the item:
        form.deleteAll();
        form.append(button1);
        for (int i = 0; i < lotOfButtons.length; i++) {
            form.append(lotOfButtons[i]);
        }
        form.append(button2);

        display.setCurrentItem(button2);
        block(CHANGE_DISPLAYABLE_DELAY);

        display.setCurrentItem(button1);
        block(CHANGE_DISPLAYABLE_DELAY);

        key(Key.Select);

        assertEquals("Command not activated or not correct command"
                + " (case 5).",
                ok, latestCommand);
        assertEquals("Wrong item delivered to commandAction-method"
                + " (case 5).",
                button1, latestItem);
        assertEquals("Unexpected displayable"
                + " (case 5).",
                form, display.getCurrent());

        latestCommand = null;
        latestItem = null;

        //Test to set item current in case the item is not focusable.
        //(It seems there's no way to test items visibility without
        //screenshots. This test just makes sure that no crashes occurs):
        form2.deleteAll();
        StringItem plain1 = new StringItem("", "not focusable.", Item.PLAIN);
        form2.append(plain1);
        display.setCurrentItem(plain1);
        block(CHANGE_DISPLAYABLE_DELAY);
    }

    /**
     * Test tries to make setCurrentItem()-method crash or hang
     * the midlet somehow.
     */
    public void testCrash() {
        Form form = new Form("form");
        Form form2 = new Form("form2");
        StringItem plain1 = new StringItem("", "not focusable.", Item.PLAIN);
        StringItem button1 = new StringItem("", "click!", Item.BUTTON);

        //Test to set item current in case the item is not focusable.
        //(It seems there's no way to test items visibility without
        //screenshots. This test just makes sure that no crashes occurs)
        form.append(plain1);
        display.setCurrentItem(plain1);
        block(CHANGE_DISPLAYABLE_DELAY);
        print("case 1 ok.");

        //As above test, but this time the item is in another displayable:
        form.deleteAll();
        form2.append(plain1);
        display.setCurrentItem(plain1);
        block(CHANGE_DISPLAYABLE_DELAY);
        print("case 2 ok.");

        //Make many calls without delay:
        form.append(button1);
        for (int i = 0; i < 10; i++) {
            display.setCurrentItem(button1);
            display.setCurrentItem(plain1);
        }
        print("case 3 ok.");
    }

    /**
     * Check that commands are delivered to the correct Displayable when
     *  Displayable is switched to new one, but old still has
     *  doLayout() ongoing.
     */
    public void displayableSwithTest() {
        Form form = new Form("form");
        Form form2 = new Form("form2");

        button1.addCommand(c1);
        button1.setItemCommandListener(this);

        button2.addCommand(c2);
        button2.setItemCommandListener(this);

        button3.addCommand(c3);
        button3.setItemCommandListener(this);

        //button1.setDefaultCommand(c2);

        form.append(button1);
        form.append(button2);

        for (int i = 0; i < sia.length; i++) {
            form2.append(sia[i]);
            sia[i].addCommand(c3);
        }
        form2.append(button3);

        setCurrent(form);
        key(Key.Select);
        //as that command change form
        block(CHANGE_DISPLAYABLE_DELAY);
        key(Key.Select);
        assertEquals("Not Correct Comand was called, probable Displayable"
                + " switched back to initial.", c3, latestCommand);
    }

    /**
     * Catches item commands and saves latest command and item to
     * member variables.
     * @param c Command
     * @param item item
     */
    public void commandAction(Command c, Item item) {
        latestCommand = c;
        latestItem = item;
        if (c == c1) {
            System.out.println("button1 clicked, command: " + c + " item: "
                    + item);
            button1.setText("clicks: " + (++count1));
            display.setCurrentItem(button3);
            //display.setCurrentItem(sia[sia.length-1]);
        }
        else if (c == c2) {
            System.out.println("button2 clicked, command: " + c + " item: "
                    + item);
            button2.setText("clicks: " + (++count2));
        }
        else if (c == c3) {
            System.out.println("button3 clicked, command: " + c + " item: "
                    + item);
            //button3.setText("clicks: "+ (++count3));
            display.setCurrentItem(button1);
        }
    }
}
