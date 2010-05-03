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
package com.nokia.openlcdui.mt_uirobot.item;

import junit.framework.*;

import javax.microedition.lcdui.*;

import com.nokia.openlcdui.mt_uirobot.ItemUITestBase;

import com.nokia.mj.impl.uitestutils.Key;

/**
 * TEST CASE SPECIFICATION <br>
 * <br>
 * Short description of the module test:
 * <li> UI-robot based test to test Item. <br>
 * <br>
 * Created: 2008-12-05
 */
public class ItemUITest extends ItemUITestBase {

    private static final int BIG_PREFERRED_HEIGHT = 500;
    private static final int BIG_PREFERRED_WIDTH = 2500;

    private static final int GAUGE_MAX_VALUE = 100;
    private static final int GAUGE_INITIAL_VALUE = 15;

    private static final int STATE_NO_EXTRA_ITEMS = 0;
    private static final int STATE_ITEM_BEFORE = 1;
    private static final int STATE_ITEM_AFTER = 2;
    private static final int STATE_ITEM_BEFORE_AND_AFTER = 3;

    private int state;
    private Command itemCommand = new Command("item", "", Command.ITEM, 0);

    /**
     * Constructor.
     */
    public ItemUITest() {
    }

    /**
     * Constructor.
     *
     * @param testName Test name.
     * @param testMethod Test method.
     */
    public ItemUITest(String testName) {
        super(testName);
    }

    public static Test suite() {
        TestSuite suite = new TestSuite();

        java.util.Vector methodNames;
	    java.util.Enumeration e;

	    // Add widget tests
	    methodNames = ItemUITest.methodNames();
	    e = methodNames.elements();
	    while (e.hasMoreElements()) {
	        suite.addTest(new ItemUITest((String)e.nextElement()));
	    }
        
        return suite;
    }

    public static java.util.Vector methodNames() {
        java.util.Vector methodNames = new java.util.Vector();
        methodNames.addElement("testPreferredSizesDateFieldTime");
        methodNames.addElement("testPreferredSizesDateFieldDate");
        methodNames.addElement("testPreferredSizesDateFieldDateTime");
        methodNames.addElement("testPreferredSizesNonInteractiveGauge");
        methodNames.addElement("testPreferredSizesInteractiveGauge");
        methodNames.addElement("testPreferredSizesShortText");
        methodNames.addElement("testPreferredSizesLongText");
        methodNames.addElement("testPreferredSizesHyperLinkShortText");
        methodNames.addElement("testPreferredSizesHyperLinkLongText");
        methodNames.addElement("testPreferredSizesPlainShortText");
        methodNames.addElement("testPreferredSizesPlainLongText");
        return methodNames;
    }
    
    public void runTest() throws Throwable {
        if (getName().equals("testPreferredSizesDateFieldTime")) {
        	testPreferredSizes(new DateField(null, DateField.TIME), true);
        }
        else if (getName().equals("testPreferredSizesDateFieldDate")) {
        	testPreferredSizes(new DateField(null, DateField.DATE), true);
        }
        else if (getName().equals("testPreferredSizesDateFieldDateTime")) {
        	testPreferredSizes(new DateField(null, DateField.DATE_TIME), true);
        }
        else if (getName().equals("testPreferredSizesNonInteractiveGauge")) {
        	testPreferredSizes(
                    new Gauge(null, false,
                            GAUGE_MAX_VALUE,
                            GAUGE_INITIAL_VALUE), false);
        }
        else if (getName().equals("testPreferredSizesInteractiveGauge")) {
        	testPreferredSizes(
                    new Gauge(null, true,
                            GAUGE_MAX_VALUE,
                            GAUGE_INITIAL_VALUE), true);
        }
        else if (getName().equals("testPreferredSizesShortText")) {
        	testPreferredSizes(
                    new StringItem(null, "short text",
                            Item.BUTTON), true);
        }
        else if (getName().equals("testPreferredSizesLongText")) {
        	testPreferredSizes(
                    new StringItem(null,
                            "This is long text,"
                            + " so long that it "
                            + "won't fit to one line",
                            Item.BUTTON), true);
        }
        else if (getName().equals("testPreferredSizesHyperLinkShortText")) {
        	testPreferredSizes(
                    new StringItem(null, "short text",
                            Item.HYPERLINK), true);
        }
        else if (getName().equals("testPreferredSizesHyperLinkLongText")) {
        	testPreferredSizes(
                    new StringItem(null,
                            "This is long text,"
                            + " so long that it "
                            + "won't fit to one line",
                            Item.HYPERLINK), true);
        }
        else if (getName().equals("testPreferredSizesPlainShortText")) {
        	testPreferredSizes(
                    new StringItem(null, "short text",
                            Item.PLAIN), false);
        }
        else if (getName().equals("testPreferredSizesPlainLongText")) {
        	testPreferredSizes(
                    new StringItem(null,
                            "This is long text,"
                            + " so long that it "
                            + "won't fit to one line",
                            Item.PLAIN), false);
        }
        else super.runTest();
    }    
   

    /**
	 * Tests preferred size of specified item. This method only makes sure that
	 * setting and getting preferred size won't crash the application and the
	 * values returned are somewhat meaningful, for example not negative. It
	 * also makes sure the focus stays in item when preferred size is changed.
	 *
	 * @param item
	 *            Item to be tested. It doesn't matter does item has label or
	 *            not. Method will test preferred size with and without label
	 *            automatically.
	 * @param testCommands
	 *            If true, method will make sure the focus is not changed when
	 *            preferred size is changed and commands works all the time.
	 *            This must be false if item cant have commands.
	 */
    public void testPreferredSizes(Item item, boolean testCommands) {
        int w;
        int h;

        StringItem button1 = new StringItem("", "button1", Item.BUTTON);
        StringItem button2 = new StringItem("", "button2", Item.BUTTON);
        Command ok1 = new Command("Ok1", "", Command.ITEM, 0);
        Command ok2 = new Command("Ok2", "", Command.ITEM, 0);

        if (testCommands) {
            button1.addCommand(ok1);
            button2.addCommand(ok2);

            button1.setItemCommandListener(this);
            button2.setItemCommandListener(this);

            // Add commands to item only in first round:
            if (state == STATE_NO_EXTRA_ITEMS) {
                item.addCommand(itemCommand);
                item.setItemCommandListener(this);
            }
        }

        // Unlock preferred size:
        item.setPreferredSize(-1, -1);

        switch (state) {
            case STATE_NO_EXTRA_ITEMS:
                form.append(item);
                break;
            case STATE_ITEM_BEFORE:
                form.append(button1);
                form.append(item);
                break;
            case STATE_ITEM_AFTER:
                form.append(item);
                form.append(button1);
                break;
            case STATE_ITEM_BEFORE_AND_AFTER:
                form.append(button1);
                form.append(item);
                form.append(button2);
                break;
            default:
                fail("invalid state, bug in test.");
                break;
        }

        display.setCurrentItem(item);

        //Test Item without label:
        item.setLabel(null);
        block(CHANGE_DISPLAYABLE_DELAY);

        w = item.getPreferredWidth();
        h = item.getPreferredHeight();
        if (state == STATE_NO_EXTRA_ITEMS) {
            print("Preferred size of Item without label was: w="
                + w + ", h=" + h);
        }
        this.assertTrue("Width not greater than zero"
                + " (without label), state: " + state, w > 0);
        this.assertTrue("Height not greater than zero"
                + " (without label), state: " + state, h > 0);
        this.assertTrue("Item's width is larger than screen's width,"
                + " state: " + state, w <= form.getWidth());

        //Test Item with label:
        item.setLabel("label");
        block(CHANGE_DISPLAYABLE_DELAY);
        w = item.getPreferredWidth();
        h = item.getPreferredHeight();

        if (state == STATE_NO_EXTRA_ITEMS) {
            print("Preferred size of Item with label was: w="
                + w + ", h=" + h);
        }
        this.assertTrue("Width not greater than zero"
                + " (with label), state: " + state, w > 0);
        this.assertTrue("Height not greater than zero"
                + " (with label), state: " + state, h > 0);
        this.assertTrue("Item's width is larger than screen's width,"
                + " state: " + state, w <= form.getWidth());

        // Set preferred width larger than screen width and make
        // sure the width is not larger than screen width:
        item.setPreferredSize(BIG_PREFERRED_WIDTH, item.getPreferredHeight());
        block(CHANGE_DISPLAYABLE_DELAY);

        w = item.getPreferredWidth();
        h = item.getPreferredHeight();
        if (state == STATE_NO_EXTRA_ITEMS) {
            print("Preferred size of Item with label after setting"
                   + " preferred width to " + BIG_PREFERRED_WIDTH + " was: w="
                + w + ", h=" + h);
        }

        this.assertTrue("Item's width equal or smaller than zero,"
                + " state: " + state, w > 0);
        this.assertTrue("Item's height equal or smaller than zero,"
                + " state: " + state, h > 0);
        this.assertTrue("Item's width is larger than screen's width,"
                + " state: " + state, w <= form.getWidth());

        item.setLabel(null);
        block(CHANGE_DISPLAYABLE_DELAY);

        w = item.getPreferredWidth();
        h = item.getPreferredHeight();
        if (state == STATE_NO_EXTRA_ITEMS) {
            print("Preferred size of Item without label after setting"
                   + " preferred width to " + BIG_PREFERRED_WIDTH + " was: w="
                + w + ", h=" + h);
        }

        this.assertTrue("Item's width equal or smaller than zero (no label),"
                + " state: " + state, w > 0);
        this.assertTrue("Item's height equal or smaller than zero (no label),"
                + " state: " + state, h > 0);
        this.assertTrue("Item's width is larger than screen's width (no label),"
                + " state: " + state, w <= form.getWidth());

        //Test command, that verifies the focus is not changed:
        if (testCommands) {
            key(Key.CBA1);
            assertItemCmdListener("", itemCommand, item);
        }

        // Set preferred height and make sure the preferred values
        // are meaningful:
        item.setLabel("label");
        block(CHANGE_DISPLAYABLE_DELAY);

        item.setPreferredSize(item.getPreferredWidth(), BIG_PREFERRED_HEIGHT);
        block(CHANGE_DISPLAYABLE_DELAY);

        w = item.getPreferredWidth();
        h = item.getPreferredHeight();
        if (state == STATE_NO_EXTRA_ITEMS) {
            print("Preferred size of Item with label after setting"
                   + " preferred height to " + BIG_PREFERRED_HEIGHT + " was: w="
                + w + ", h=" + h);
        }
        this.assertTrue("Item's width equal or smaller than zero,"
                + " state: " + state, w > 0);
        this.assertTrue("Item's height equal or smaller than zero,"
                + " state: " + state, h > 0);

        item.setLabel(null);
        block(CHANGE_DISPLAYABLE_DELAY);

        w = item.getPreferredWidth();
        h = item.getPreferredHeight();
        if (state == STATE_NO_EXTRA_ITEMS) {
            print("Preferred size of Item without label after setting"
                   + " preferred height to " + BIG_PREFERRED_HEIGHT + " was: w="
                + w + ", h=" + h);
        }
        this.assertTrue("Item's width equal or smaller than zero (no label),"
                + " state: " + state, w > 0);
        this.assertTrue("Item's height equal or smaller than zero (no label),"
                + " state: " + state, h > 0);

        //Test command, that verifies the focus is not changed:
        if (testCommands) {
            key(Key.CBA1);
            assertItemCmdListener("", itemCommand, item);
        }

        form.deleteAll();

        if (state < STATE_ITEM_BEFORE_AND_AFTER) {
            state++;
            testPreferredSizes(item, testCommands);
        }
        else {
            state = 0;
        }
    }

}
