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
package com.nokia.openlcdui.mt_uirobot;

import javax.microedition.lcdui.Command;
import javax.microedition.lcdui.Form;
import javax.microedition.lcdui.Item;
import javax.microedition.lcdui.ItemCommandListener;
import javax.microedition.lcdui.ItemStateListener;

import com.nokia.mj.impl.uitestutils.Key;

public abstract class ItemUITestBase extends UITestBase implements ItemCommandListener,
        ItemStateListener {

    private Command lastItemCommand;
    private Item lastItem;
    private Item lastStateListenerItem;

    protected Form form;

    public ItemUITestBase() {
    	this("");
    }
    
    public ItemUITestBase(String name) {
        super(name);
    }

    protected void setUp() throws Exception {
        super.setUp();
        form = new Form("Form: " + getName());
        form.setItemStateListener(this);
        lastItemCommand = null;
        lastItem = null;
    }

    /**
     * Test command on Item.
     *
     * @param item an item
     */
    protected void testCommand(Item item) {
        Command cmd = new Command("ItemCmd", Command.ITEM, 0);

        item.addCommand(cmd);
        item.setItemCommandListener(this);
        form.append(item);

        setCurrent(form);

        key(Key.Select);
        // TODO: this doesnt work in some cases due tue environment's key mapping
        assertItemCmdListener("", cmd, item);
    }

    /**
     * Assert the item command listener values.
     *
     * @param expCmd expected Command
     * @param expItem expected Item
     */
    protected void assertItemCmdListener(String msg, Command expCmd, Item expItem) {
        block(100);
        boolean expNull = (expCmd == null && expItem == null);
        boolean lastNull = (lastItemCommand == null && lastItem == null);
        if (lastNull && !expNull) {
            fail(msg + " - Listener should have been called with "
                    + expCmd + " on " + expItem);
        }
        else if (!lastNull && expNull) {
            fail(msg + " - Listener should not be called with "
                    + lastItemCommand + " on " + lastItem);
        }
        else {
            assertEquals(msg + " - Listener item mismatch ", expItem, lastItem);
            assertEquals(msg + " - Listener cmd mismatch ", expCmd, lastItemCommand);
        }
        lastItem = null;
        lastItemCommand = null;
    }

    public void commandAction(Command cmd, Item item) {
        lastItem = item;
        lastItemCommand = cmd;
    }

    /**
     * Assert the item state listener values.
     *
     * @param expItem expected Item
     */
    protected void assertItemStateChanged(String msg, Item expItem) {
        block(100);
        boolean expNull = (expItem == null);
        boolean lastNull = (lastStateListenerItem == null);
        if (lastNull && !expNull) {
            fail(msg + " - Listener should have been called on " + expItem);
        }
        else if (!lastNull && expNull) {
            fail(msg + " - Listener should not be called on "
                    + lastStateListenerItem);
        }
        else {
            assertEquals(msg + " - Listener item mismatch ", expItem, lastStateListenerItem);
        }
        lastStateListenerItem = null;
    }

    public void itemStateChanged(Item item) {
        lastStateListenerItem = item;
    }

}
