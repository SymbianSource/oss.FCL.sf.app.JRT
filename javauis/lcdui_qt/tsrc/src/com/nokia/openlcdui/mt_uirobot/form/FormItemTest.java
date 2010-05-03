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
package com.nokia.openlcdui.mt_uirobot.form;

import junit.framework.*;

import javax.microedition.lcdui.*;

import com.nokia.openlcdui.mt_uirobot.ItemUITestBase;

import com.nokia.mj.impl.uitestutils.Key;

/**
 * TEST CASE SPECIFICATION <br>
 * <br>
 * Short description of the module test:
 * <li> UI-robot based test to test item-related form functionality,
 * for example adding and removing items to form. <br>
 * <br>
 * Created: 2008-11-12
 */
public class FormItemTest extends ItemUITestBase {

    /**
     * Constructor.
     */
    public FormItemTest() {
    	super();
    }

    /**
     * Constructor.
     *
     * @param sTestName Test name.
     * @param rTestMethod Test method.
     */
    public FormItemTest(String sTestName) {
        super(sTestName);
    }

    public static Test suite() {
        TestSuite suite = new TestSuite();

        java.util.Vector methodNames;
	    java.util.Enumeration e;

	    // Add widget tests
	    methodNames = FormItemTest.methodNames();
	    e = methodNames.elements();
	    while (e.hasMoreElements()) {
	        suite.addTest(new FormItemTest((String)e.nextElement()));
	    }
        
        return suite;
    }

    public static java.util.Vector methodNames() {
        java.util.Vector methodNames = new java.util.Vector();
        methodNames.addElement("testAppendItemAfterSetCurrent");
        methodNames.addElement("testRemoveCommandFromLabeledFocusedItem");
        methodNames.addElement("testAddRemoveItems");
        methodNames.addElement("testRemoveItemsWhenBackground");
        return methodNames;
    }
    
    public void runTest() throws Throwable {
        if (getName().equals("testAppendItemAfterSetCurrent")) testAppendItemAfterSetCurrent();
        else if (getName().equals("testRemoveCommandFromLabeledFocusedItem")) testRemoveCommandFromLabeledFocusedItem();
        else if (getName().equals("testAddRemoveItems")) testAddRemoveItems();
        else if (getName().equals("testRemoveItemsWhenBackground")) testRemoveItemsWhenBackground();
        else super.runTest();
    }    
    

    /**
     * Tests to remove command from focused item which has label and
     * verifies that focus is moved to next/previous focusable item.
     */
    public void testRemoveCommandFromLabeledFocusedItem() {
        StringItem button1 = new StringItem("label", "click!", Item.BUTTON);
        StringItem button2 = new StringItem("label", "click2!", Item.BUTTON);

        Command c1 = new Command("Ok1", "", Command.ITEM, 0);
        Command c2 = new Command("Ok2", "", Command.ITEM, 0);

        button1.addCommand(c1);
        button1.setItemCommandListener(this);
        button2.addCommand(c2);
        button2.setItemCommandListener(this);

        form.append(button1);
        form.append(button2);

        setCurrent(form);

        // First item should be focused. Remove command from it, and
        // make sure the focus is transfered to second button:
        button1.removeCommand(c1);
        block(CHANGE_DISPLAYABLE_DELAY);

        key(Key.CBA1);
        assertItemCmdListener("case1", c2, button2);

        // Add command back to first item and remove command from second one.
        // Verify that the focus is moved to first one:
        button1.addCommand(c1);
        button2.removeCommand(c2);
        //block(CHANGE_DISPLAYABLE_DELAY);
        block(CHANGE_DISPLAYABLE_DELAY);

        key(Key.CBA1);
        assertItemCmdListener("case2", c1, button1);
    }

    /**
     * Tests to add and remove items to/from form.
     */
    public void testAddRemoveItems() {
        Command ok1 = new Command("Ok1", "", Command.ITEM, 0);
        Command ok2 = new Command("Ok2", "", Command.ITEM, 0);

        Form form2 = new Form("form2");
        StringItem button1 = new StringItem(null, "button1", StringItem.BUTTON);
        StringItem button2 = new StringItem(null, "button2", StringItem.BUTTON);
        button1.addCommand(ok1);
        button2.addCommand(ok2);
        button1.setItemCommandListener(this);
        button2.setItemCommandListener(this);
        form.append(button1);
        form.append(button2);

        setCurrent(form);

        // Move to second button, hide form, remove second button, show
        // form and verify that the first button is selected:
        key(Key.RightArrow);

        setCurrent(form2);

        form.delete(1);

        setCurrent(form);
        key(Key.CBA1);
        assertItemCmdListener("case1", ok1, button1);

        // Add second button again and move to it and verify the command
        // works:
        form.append(button2);
        block(CHANGE_DISPLAYABLE_DELAY);
        key(Key.RightArrow);

        key(Key.CBA1);
        assertItemCmdListener("case2", ok2, button2);
    }

    /**
     * Tests to append item to form after form is set current. Then
     * verifies the focus transfer works.
     */
    public void testAppendItemAfterSetCurrent() {
        StringItem hyperlink = new StringItem("label", "url",
                StringItem.HYPERLINK);
        StringItem button1 = new StringItem(null, "button1",
                StringItem.BUTTON);

        Command dummyCommand = new Command("dummy", "", Command.ITEM, 0);
        button1.addCommand(dummyCommand);

        Command ok = new Command("Ok", "", Command.ITEM, 0);

        setCurrent(form);

        form.append(button1);
        form.append(hyperlink);

        hyperlink.setItemCommandListener(this);
        hyperlink.addCommand(ok);

        // Give some time for layouter because adding command to hyperlink
        // means relayouting:
        block(CHANGE_DISPLAYABLE_DELAY);

        // Move to hyperlink, click command and verify it works:
        key(Key.RightArrow);
        key(Key.CBA1);
        assertItemCmdListener("case1", ok, hyperlink);

        // Click command by using MSK and verify it works:
        key(Key.Select);
        assertItemCmdListener("case2", ok, hyperlink);
    }

    /**
     * Test that command are working properly in case of
     */
    public void testRemoveItemsWhenBackground() {
        Form forma = new Form ("Form A");
        Form formb = new Form ("Form B");
        Command dummyCommand1 = new Command("dummy1", "", Command.ITEM, 0);
        Command dummyCommand2 = new Command("dummy2", "", Command.ITEM, 0);
        Command dummyCommand3 = new Command("dummy3", "", Command.ITEM, 0);
        Command dummyCommand4 = new Command("dummy4", "", Command.ITEM, 0);
        Gauge g1 = new Gauge("Gauge1", true, 10, 0);
        Gauge g2 = new Gauge("Gauge2", true, 10, 0);
        Gauge g3 = new Gauge("Gauge3", true, 10, 0);
        Gauge g4 = new Gauge("Gauge4", true, 10, 0);
        setCurrent(forma);

        forma.append(g1);
        forma.append(g2);
        forma.append(g3);
        forma.append(g4);
        g1.addCommand(dummyCommand1);
        g1.setItemCommandListener(this);
        g2.addCommand(dummyCommand2);
        g2.setItemCommandListener(this);
        g3.addCommand(dummyCommand3);
        g3.setItemCommandListener(this);
        g4.addCommand(dummyCommand4);
        g4.setItemCommandListener(this);
        block(CHANGE_DISPLAYABLE_DELAY);
        key(Key.DownArrow);
        key(Key.DownArrow);
        key(Key.DownArrow);
        key(Key.Select);
        assertItemCmdListener("case1", dummyCommand4, g4);

        setCurrent(formb);

        //delete 3 list items from Form A
        forma.delete(1);
        forma.delete(1);
        forma.delete(1);
        formb.append(g2);
        block(CHANGE_DISPLAYABLE_DELAY);
        key(Key.Select);
        assertItemCmdListener("case2", dummyCommand2, g2);

        //check the focus was not changed when another item is added.
        formb.append(g3);
        block(CHANGE_DISPLAYABLE_DELAY);
        key(Key.Select);
        assertItemCmdListener("case3", dummyCommand2, g2);

        //switch displayble back to Form a
        setCurrent(forma);
        key(Key.Select);
        assertItemCmdListener("case4", dummyCommand1, g1);

        //delete first item from From B
        formb.delete(0);
        //add it to Form A
        forma.append(g2);
        block(CHANGE_DISPLAYABLE_DELAY);
        key(Key.DownArrow);
        key(Key.Select);
        assertItemCmdListener("case5", dummyCommand2, g2);
    }

}
