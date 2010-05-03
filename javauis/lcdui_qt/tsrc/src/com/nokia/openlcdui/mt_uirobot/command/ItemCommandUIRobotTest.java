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
package com.nokia.openlcdui.mt_uirobot.command;

import junit.framework.*;

import java.io.IOException;

import javax.microedition.lcdui.*;

import com.nokia.openlcdui.mt_uirobot.ItemUITestBase;

import com.nokia.mj.impl.uitestutils.Key;

/**
 * TEST CASE SPECIFICATION <br>
 * <br>
 * Short description of the module test:
 * <li> UI-robot based test to test item commands. <br>
 * <br>
 * Created: 2008-08-04
 */
public class ItemCommandUIRobotTest extends ItemUITestBase {

    private Command latestCommand;
    private Item latestItem;
    private Displayable latestDisplayable;
    private Command latestDisplayableCommand;


    /**
     * Constructor.
     */
    public ItemCommandUIRobotTest() {
    }

    /**
     * Constructor.
     *
     * @param sTestName Test name.
     * @param rTestMethod Test method.
     */
    public ItemCommandUIRobotTest(String sTestName) {
        super(sTestName);
    }

    public static Test suite() {
        TestSuite suite = new TestSuite();

        java.util.Vector methodNames;
	    java.util.Enumeration e;

	    // Add widget tests
	    methodNames = ItemCommandUIRobotTest.methodNames();
	    e = methodNames.elements();
	    while (e.hasMoreElements()) {
	        suite.addTest(new ItemCommandUIRobotTest((String)e.nextElement()));
	    }
        
        return suite;
    }

    public static java.util.Vector methodNames() {
        java.util.Vector methodNames = new java.util.Vector();
        methodNames.addElement("testListener");
        methodNames.addElement("testMultipleItems");
        methodNames.addElement("testInitializationOrders");
        methodNames.addElement("testCommandInItemAndInDisplayable");
        methodNames.addElement("testMultipleCommands");
        methodNames.addElement("testOneCommandInMultipleItems");
        methodNames.addElement("testDefaultCommand");
        methodNames.addElement("testDynamicUpdate");
        methodNames.addElement("testItemWithLabel");
        methodNames.addElement("testAddCommandToFocusedItem");
        return methodNames;
    }
    
    public void runTest() throws Throwable {
        if (getName().equals("testListener")) testListener();
        else if (getName().equals("testMultipleItems")) testMultipleItems();
        else if (getName().equals("testInitializationOrders")) testInitializationOrders();
        else if (getName().equals("testCommandInItemAndInDisplayable")) testCommandInItemAndInDisplayable();
        else if (getName().equals("testMultipleCommands")) testMultipleCommands();
        else if (getName().equals("testOneCommandInMultipleItems")) testOneCommandInMultipleItems();
        else if (getName().equals("testDefaultCommand")) testDefaultCommand();
        else if (getName().equals("testDynamicUpdate")) testDynamicUpdate();
        else if (getName().equals("testItemWithLabel")) testItemWithLabel();
        else if (getName().equals("testAddCommandToFocusedItem")) testAddCommandToFocusedItem();
        else super.runTest();
    }


    /**
     * Tests the basic functionality of itemCommandListener.
     */
    public void testListener() {
        Form form = new Form("form");

        StringItem button1 = new StringItem("", "click!", Item.BUTTON);
        Command ok = new Command("Ok", "", Command.ITEM, 0);
        button1.addCommand(ok);
        button1.setItemCommandListener(this);

        form.append(button1);

        setCurrent(form);

        //Click middle soft key, this should activate item's command:
        key(Key.Select);

        assertEquals("Command not activated or not correct command"
                + " when pressing MSK.",
                ok, latestCommand);
        assertEquals("Wrong item delivered to commandAction-method"
                + " when pressing MSK.",
                button1, latestItem);
        latestCommand = null;
        latestItem = null;

        //Click left soft key, this should activate item's command too:
        key(Key.CBA1);
        assertEquals("Command not activated or not correct command"
                + " when pressing LSK.",
                ok, latestCommand);
        assertEquals("Wrong item delivered to commandAction-method"
                + " when pressing LSK.",
                button1, latestItem);

    }

    /**
     * Tests that item commands works when there are multiple items
     * in a form and focus moves between items.
     */
    public void testMultipleItems() {
        latestCommand = null;
        latestItem = null;

        Form form = new Form("form");

        StringItem button1 = new StringItem("", "click1!", Item.BUTTON);
        StringItem button2 = new StringItem("", "click2!", Item.BUTTON);
        StringItem button3 = new StringItem("", "click3!", Item.BUTTON);
        Command ok1 = new Command("Ok1", "", Command.ITEM, 0);
        Command ok2 = new Command("Ok2", "", Command.ITEM, 0);
        Command ok3 = new Command("Ok3", "", Command.ITEM, 0);

        button1.addCommand(ok1);
        button2.addCommand(ok2);
        button3.addCommand(ok3);

        button1.setItemCommandListener(this);
        button2.setItemCommandListener(this);
        button3.setItemCommandListener(this);

        form.append(button1);
        form.append(button2);
        form.append(button3);

        setCurrent(form);

        //Click first button:
        key(Key.CBA1);
        assertEquals("Command not activated or not correct command"
                + " when pressing LSK. (case 1)",
                ok1, latestCommand);
        assertEquals("Wrong item delivered to commandAction-method"
                + " when pressing LSK. (case 1)",
                button1, latestItem);
        latestCommand = null;
        latestItem = null;


        //Move to next button and click it:
        key(Key.RightArrow);
        key(Key.CBA1);
        assertEquals("Command not activated or not correct command"
                + " when pressing LSK. (case 2)",
                ok2, latestCommand);
        assertEquals("Wrong item delivered to commandAction-method"
                + " when pressing LSK. (case 2)",
                button2, latestItem);
        latestCommand = null;
        latestItem = null;

        //Move to third button and click it:
        key(Key.RightArrow);
        key(Key.CBA1);
        assertEquals("Command not activated or not correct command"
                + " when pressing LSK. (case 3)",
                ok3, latestCommand);
        assertEquals("Wrong item delivered to commandAction-method"
                + " when pressing LSK. (case 3)",
                button3, latestItem);
        latestCommand = null;
        latestItem = null;
    }

    /**
     * Tests different orders to add command, set listener etc.
     */
    public void testInitializationOrders() {
        latestCommand = null;
        latestItem = null;

        Form form = new Form("form");

        StringItem button1 = new StringItem("", "click!", Item.BUTTON);
        Command ok = new Command("Ok", "", Command.ITEM, 0);

        //Add Command, set listener, add item to form:
        button1.addCommand(ok);
        button1.setItemCommandListener(this);

        form.append(button1);

        setCurrent(form);

        key(Key.CBA1);
        assertEquals("Command not activated or not correct command"
                + " when pressing LSK. (case 1)",
                ok, latestCommand);
        assertEquals("Wrong item delivered to commandAction-method"
                + " when pressing LSK. (case 1)",
                button1, latestItem);
        latestCommand = null;
        latestItem = null;

        //Clean up:
        button1.setItemCommandListener(null);
        button1.removeCommand(ok);
        form.delete(0);

        //Set listener, add Command, add item to form:
        button1.setItemCommandListener(this);
        button1.addCommand(ok);

        form.append(button1);

        setCurrent(form);

        key(Key.CBA1);
        assertEquals("Command not activated or not correct command"
                + " when pressing LSK. (case 2)",
                ok, latestCommand);
        assertEquals("Wrong item delivered to commandAction-method"
                + " when pressing LSK. (case 2)",
                button1, latestItem);
        latestCommand = null;
        latestItem = null;

        //Clean up:
        button1.setItemCommandListener(null);
        button1.removeCommand(ok);
        form.delete(0);

        //Add item to form, set listener, add Command:
        form.append(button1);
        button1.setItemCommandListener(this);
        button1.addCommand(ok);

        setCurrent(form);

        key(Key.CBA1);
        assertEquals("Command not activated or not correct command"
                + " when pressing LSK. (case 3)",
                ok, latestCommand);
        assertEquals("Wrong item delivered to commandAction-method"
                + " when pressing LSK. (case 3)",
                button1, latestItem);
        latestCommand = null;
        latestItem = null;

        //Clean up:
        button1.setItemCommandListener(null);
        button1.removeCommand(ok);
        form.delete(0);

        //Add item to form, add Command, set listener:
        form.append(button1);
        button1.addCommand(ok);
        button1.setItemCommandListener(this);

        setCurrent(form);

        key(Key.CBA1);
        assertEquals("Command not activated or not correct command"
                + " when pressing LSK. (case 4)",
                ok, latestCommand);
        assertEquals("Wrong item delivered to commandAction-method"
                + " when pressing LSK. (case 4)",
                button1, latestItem);
        latestCommand = null;
        latestItem = null;
    }

    /**
     * Makes sure that command works when it is added to item and
     * to displayable at the same time.
     */
    public void testCommandInItemAndInDisplayable() {
        latestCommand = null;
        latestItem = null;

        Form form = new Form("form");
        form.setCommandListener(this);

        StringItem button1 = new StringItem("", "click!", Item.BUTTON);
        Command ok = new Command("Ok", "", Command.ITEM, 0);
        button1.addCommand(ok);
        button1.setItemCommandListener(this);

        form.append(button1);
        form.addCommand(ok);

        setCurrent(form);

        //Because same command is added to item and to displayable,
        //command should appear two times in menu. First one
        //should be item command and second one displayable command.

        //Click left soft key twice to select first occurence of command:
        key(Key.CBA1);
        key(Key.CBA1);
        assertEquals("Command not activated or not correct command"
                + " when selecting first occurence.",
                ok, latestCommand);
        assertEquals("Wrong item delivered to commandAction-method"
                + " when selecting first occurence.",
                button1, latestItem);
        latestCommand = null;
        latestItem = null;

        //Click left soft key, down arrow and then left soft key again
        //to select second occurence of the command:
        key(Key.CBA1);
        key(Key.DownArrow);
        key(Key.CBA1);
        assertEquals("Command not activated or not correct command"
                + " when selecting second occurence.",
                ok, latestDisplayableCommand);
        assertEquals("Wrong item delivered to commandAction-method"
                + " when selecting second occurence.",
                form, latestDisplayable);
        latestDisplayableCommand = null;
        latestDisplayable = null;
    }

    /**
     * Test to add multiple commands to one item and verify the commands
     * work as expected.
     */
    public void testMultipleCommands() {
        latestCommand = null;
        latestItem = null;

        Form form = new Form("form");

        StringItem button1 = new StringItem("", "click!", Item.BUTTON);
        Command ok1 = new Command("Ok1", "", Command.ITEM, 1);
        Command ok2 = new Command("Ok2", "", Command.ITEM, 0);
        button1.addCommand(ok1);
        button1.addCommand(ok2);
        button1.setItemCommandListener(this);

        form.append(button1);

        setCurrent(form);

        //Open menu, scroll down and click left soft key,
        //this should activate item's first command:
        //(eSWT maps second command before first command in menu)
        key(Key.CBA1);
        key(Key.DownArrow);
        key(Key.CBA1);
        assertEquals("Command not activated or not correct command"
                + " when selecting item's first command.",
                ok1, latestCommand);
        assertEquals("Wrong item delivered to commandAction-method"
                + " when selecting item's first command.",
                button1, latestItem);
        latestCommand = null;
        latestItem = null;

        //Click left soft key twice, this should activate item's second command:
        key(Key.CBA1);
        key(Key.CBA1);
        assertEquals("Command not activated or not correct command"
                + " when selecting item's second command.",
                ok2, latestCommand);
        assertEquals("Wrong item delivered to commandAction-method"
                + " when selecting item's second command.",
                button1, latestItem);
        latestCommand = null;
        latestItem = null;
    }

    /**
     * Test to add one command to multiple items and verify the command
     * work as expected.
     */
    public void testOneCommandInMultipleItems() {
        latestCommand = null;
        latestItem = null;

        Form form = new Form("form");

        StringItem button1 = new StringItem("", "click!", Item.BUTTON);
        StringItem button2 = new StringItem("", "click2!", Item.BUTTON);
        Command ok1 = new Command("Ok1", "", Command.ITEM, 1);
        button1.addCommand(ok1);
        button2.addCommand(ok1);
        button1.setItemCommandListener(this);
        button2.setItemCommandListener(this);

        form.append(button1);
        form.append(button2);

        setCurrent(form);

        //Click first button:
        key(Key.CBA1);
        assertEquals("Command not activated or not correct command"
                + " when clicking first button.",
                ok1, latestCommand);
        assertEquals("Wrong item delivered to commandAction-method"
                + " when clicking first button.",
                button1, latestItem);
        latestCommand = null;
        latestItem = null;

        //Click second button:
        key(Key.RightArrow);
        key(Key.CBA1);
        assertEquals("Command not activated or not correct command"
                + " when clicking second button.",
                ok1, latestCommand);
        assertEquals("Wrong item delivered to commandAction-method"
                + " when clicking second button.",
                button2, latestItem);
        latestCommand = null;
        latestItem = null;
    }


    /**
     * Tests the functionality of default command.
     */
    public void testDefaultCommand() {
        latestCommand = null;
        latestItem = null;

        Form form = new Form("form");

        StringItem button1 = new StringItem("", "click!", Item.BUTTON);
        Command ok1 = new Command("Ok1", "", Command.ITEM, 0);
        Command ok2 = new Command("Ok2", "", Command.ITEM, 0);
        button1.addCommand(ok1);
        button1.addCommand(ok2);
        button1.setItemCommandListener(this);

        button1.setDefaultCommand(ok1);

        form.append(button1);

        setCurrent(form);

        //Activate item's first command:
        //(normally eSWT maps second command before first command
        //but because first command is now default command it should
        //appear before second command)
        key(Key.CBA1);
        key(Key.CBA1);
        assertEquals("Command not activated or not correct command"
                + " when selecting item's first command.",
                ok1, latestCommand);
        assertEquals("Wrong item delivered to commandAction-method"
                + " when selecting item's first command.",
                button1, latestItem);
        latestCommand = null;
        latestItem = null;

        //Set second command to default one and verify that order
        //has changed:
        button1.setDefaultCommand(ok2);

        //Dynamic command order change wont happen immediately
        //so give some time for it:
        block(250);

        key(Key.CBA1);
        key(Key.CBA1);

        assertEquals("Command not activated or not correct command"
                + " when selecting item's second command.",
                ok2, latestCommand);
        assertEquals("Wrong item delivered to commandAction-method"
                + " when selecting item's second command.",
                button1, latestItem);
        latestCommand = null;
        latestItem = null;

        //Now try to press MSK. That should activate default command:
        key(Key.Select);

        assertEquals("Command not activated or not correct command"
                + " when selecting item's default command from MSK.",
                ok2, latestCommand);
        assertEquals("Wrong item delivered to commandAction-method"
                + " when selecting item's default command from MSK.",
                button1, latestItem);
        latestCommand = null;
        latestItem = null;
    }

    /**
     * Tests that items works when screen is updated dynamically.
     * For example makes sure that focus is where it should be.
     *
     */
    public void testDynamicUpdate() {
        latestCommand = null;
        latestItem = null;

        Form form = new Form("form");

        StringItem button1 = new StringItem("", "click1!", Item.BUTTON);
        StringItem button2 = new StringItem("", "click2!", Item.BUTTON);
        Command ok1 = new Command("Ok1", "", Command.ITEM, 0);
        Command ok2 = new Command("Ok2", "", Command.ITEM, 0);

        button1.addCommand(ok1);
        button2.addCommand(ok2);

        button1.setItemCommandListener(this);
        button2.setItemCommandListener(this);

        form.append(button1);
        form.append(button2);

        setCurrent(form);

        //Move to second button and click it:
        key(Key.RightArrow);
        key(Key.CBA1);
        assertEquals("Command not activated or not correct command"
                + " when pressing second button.",
                ok2, latestCommand);
        assertEquals("Wrong item delivered to commandAction-method"
                + " when pressing second button.",
                button2, latestItem);
        latestCommand = null;
        latestItem = null;

        //Change the text of second button dynamically and verify that
        //focus stays in second button:
        button2.setText("newText");
        block(CHANGE_DISPLAYABLE_DELAY);

        key(Key.CBA1);
        assertEquals("Command not activated or not correct command"
                + " when pressing second button after update.",
                ok2, latestCommand);
        assertEquals("Wrong item delivered to commandAction-method"
                + " when pressing second after update.",
                button2, latestItem);
        latestCommand = null;
        latestItem = null;

        //Now move back to first button and verify it works:
        key(Key.LeftArrow);
        key(Key.CBA1);
        assertEquals("Command not activated or not correct command"
                + " when pressing first button.",
                ok1, latestCommand);
        assertEquals("Wrong item delivered to commandAction-method"
                + " when pressing first button.",
                button1, latestItem);
        latestCommand = null;
        latestItem = null;
    }

    /**
     * Tests commands when there's label in item.
     */
    public void testItemWithLabel() {
        latestCommand = null;
        latestItem = null;

        Form form = new Form("form");

        StringItem button1 = new StringItem("label", "click!", Item.BUTTON);
        Command ok = new Command("Ok", "", Command.ITEM, 0);
        button1.addCommand(ok);
        button1.setItemCommandListener(this);

        form.append(button1);

        setCurrent(form);

        //Click middle soft key, this should activate item's command:
        key(Key.Select);
        assertEquals("Command not activated or not correct command"
                + " when pressing MSK (StringItem).",
                ok, latestCommand);
        assertEquals("Wrong item delivered to commandAction-method"
                + " when pressing MSK (StringItem).",
                button1, latestItem);
        latestCommand = null;
        latestItem = null;

        //Click left soft key, this should activate item's command too:
        key(Key.CBA1);
        assertEquals("Command not activated or not correct command"
                + " when pressing LSK (StringItem).",
                ok, latestCommand);
        assertEquals("Wrong item delivered to commandAction-method"
                + " when pressing LSK (StringItem).",
                button1, latestItem);
        latestCommand = null;
        latestItem = null;

        //Test ImageItem next:
        Image image = null;
        try {
            image = Image.createImage("100x100.png");
        }
        catch (IOException e) {
            fail("Cannot load image 100x100.png");
        }
        ImageItem image1 = new ImageItem(
                "label", image, Item.BUTTON, "altText", ImageItem.BUTTON);

        image1.addCommand(ok);
        image1.setItemCommandListener(this);

        form.append(image1);
        block(CHANGE_DISPLAYABLE_DELAY);

        //Move to ImageItem:
        key(Key.RightArrow);

        //Click middle soft key, this should activate item's command:
        key(Key.Select);
        assertEquals("Command not activated or not correct command"
                + " when pressing MSK (ImageItem).",
                ok, latestCommand);
        assertEquals("Wrong item delivered to commandAction-method"
                + " when pressing MSK (ImageItem).",
                image1, latestItem);
        latestCommand = null;
        latestItem = null;

        //Click left soft key, this should activate item's command too:
        key(Key.CBA1);
        assertEquals("Command not activated or not correct command"
                + " when pressing LSK (ImageItem).",
                ok, latestCommand);
        assertEquals("Wrong item delivered to commandAction-method"
                + " when pressing LSK (ImageItem).",
                image1, latestItem);
        latestCommand = null;
        latestItem = null;
    }

    /**
     * Tests to add command to item that's focused.
     */
    public void testAddCommandToFocusedItem() {
        latestCommand = null;
        latestItem = null;

        Form form = new Form("form");

        StringItem button1 = new StringItem("", "click!", Item.BUTTON);
        Command ok = new Command("Ok", "", Command.ITEM, 0);
        Command dummy = new Command("Dummy", "", Command.ITEM, 0);
        button1.setItemCommandListener(this);

        // It is not possible to move focus to StringItem that doesn't
        // has commands. That's why dummy command is added first.
        button1.addCommand(dummy);

        form.append(button1);
        setCurrent(form);

        // Item should has focus because there's dummy command in it.
        // Add ok-command and remove the dummy one and verify the
        // ok-command works:
        button1.addCommand(ok);
        button1.removeCommand(dummy);

        //Click middle soft key, this should activate item's command:
        key(Key.Select);

        assertEquals("Command not activated or not correct command"
                + " when pressing MSK.",
                ok, latestCommand);
        assertEquals("Wrong item delivered to commandAction-method"
                + " when pressing MSK.",
                button1, latestItem);
        latestCommand = null;
        latestItem = null;

        //Click left soft key, this should activate item's command too:
        key(Key.CBA1);
        assertEquals("Command not activated or not correct command"
                + " when pressing LSK.",
                ok, latestCommand);
        assertEquals("Wrong item delivered to commandAction-method"
                + " when pressing LSK.",
                button1, latestItem);
        latestCommand = null;
        latestItem = null;
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
    }

    /**
     * Catches displayable commands and saves latest command and
     * displayable to member variables.
     * @param c Command
     * @param d Displayable
     */
    public void commandAction(Command c, Displayable d) {
        latestDisplayableCommand = c;
        latestDisplayable = d;
    }

}
