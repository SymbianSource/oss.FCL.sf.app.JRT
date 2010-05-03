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
package com.nokia.openlcdui.mt_uirobot.imageitem;

import junit.framework.*;

import java.io.IOException;

import javax.microedition.lcdui.*;

import com.nokia.openlcdui.mt_uirobot.ItemUITestBase;

import com.nokia.mj.impl.uitestutils.Key;

/**
 * TEST CASE SPECIFICATION <br>
 * <br>
 * Short description of the module test:
 * <li> UI-robot based test to test hyperlink. <br>
 * <br>
 * Created: 2008-10-20
 */
public class ImageItemHyperLinkTest extends ItemUITestBase {

    private Image smallImage;
    private Image mediumImage;
    private Image longImage;

    /**
     * Constructor.
     */
    public ImageItemHyperLinkTest() {
    }

    /**
     * Constructor.
     *
     * @param sTestName Test name.
     * @param rTestMethod Test method.
     */
    public ImageItemHyperLinkTest(String sTestName) {
        super(sTestName);
    }

    public static Test suite() {
        TestSuite suite = new TestSuite();

        java.util.Vector methodNames;
	    java.util.Enumeration e;

	    // Add widget tests
	    methodNames = ImageItemHyperLinkTest.methodNames();
	    e = methodNames.elements();
	    while (e.hasMoreElements()) {
	        suite.addTest(new ImageItemHyperLinkTest((String)e.nextElement()));
	    }
        
        return suite;
    }

    public static java.util.Vector methodNames() {
        java.util.Vector methodNames = new java.util.Vector();
        methodNames.addElement("testCommands");
        methodNames.addElement("testAppearanceModeChangePlain");
        methodNames.addElement("testAppearanceModeChangeButton");
        methodNames.addElement("testAppearanceModeChangeHyperlink");
        methodNames.addElement("testFocusInAppearanceModeChangePlain");
        methodNames.addElement("testFocusInAppearanceModeChangeButton");
        methodNames.addElement("testFocusInAppearanceModeChangeHyperlink");
        methodNames.addElement("testLongImageHyperlink");
        return methodNames;
    }
    
    public void runTest() throws Throwable {
        if (getName().equals("testCommands")) testCommands();
        else if (getName().equals("testAppearanceModeChangePlain")) testAppearanceModeChange(ImageItem.PLAIN);
        else if (getName().equals("testAppearanceModeChangeButton")) testAppearanceModeChange(ImageItem.BUTTON);
        else if (getName().equals("testAppearanceModeChangeHyperlink")) testAppearanceModeChange(ImageItem.HYPERLINK);
        else if (getName().equals("testFocusInAppearanceModeChangePlain")) testFocusInAppearanceModeChange(ImageItem.PLAIN);
        else if (getName().equals("testFocusInAppearanceModeChangeButton")) testFocusInAppearanceModeChange(ImageItem.BUTTON);
        else if (getName().equals("testFocusInAppearanceModeChangeHyperlink")) testFocusInAppearanceModeChange(ImageItem.HYPERLINK);
        else if (getName().equals("testLongImageHyperlink")) testLongImageHyperlink();      
        else super.runTest();
    }


    /**
     * Any pre-test setup can be done here
     */
    protected void setUp() throws Exception {
    	super.setUp();
        try {
            smallImage = Image.createImage("32x32.jpeg");
        }
        catch (IOException e) {
            //
        }
        try {
            mediumImage = Image.createImage("100x100.png");
        }
        catch (IOException e) {
            //
        }
        try {
            longImage = Image.createImage("100x300.png");
        }
        catch (IOException e) {
            //
        }
    }


    /**
     * Tests the basic functionality of command added to Hyperlink.
     */
    public void testCommands() {
        ImageItem hyperlink = new ImageItem("label", smallImage, 0, "small",
                ImageItem.HYPERLINK);

        testCommand(hyperlink);

        Command ok = new Command("Ok", "", Command.ITEM, 0);

        form.deleteAll();
        Command dummyCommand = new Command("dummy", "", Command.ITEM, 0);
        ImageItem button1 = new ImageItem(null, smallImage, 0, "small",
                ImageItem.BUTTON);
        ImageItem button2 = new ImageItem(null, smallImage, 0, "small",
                ImageItem.BUTTON);
        button1.addCommand(dummyCommand);
        button2.addCommand(dummyCommand);

        ImageItem hyperlink3 = new ImageItem(null, mediumImage, 0, "medium",
                       ImageItem.HYPERLINK);
        hyperlink3.addCommand(ok);
        hyperlink3.setItemCommandListener(this);

        form.append(button1);
        form.append(hyperlink3);
        form.append(button2);

        block(CHANGE_DISPLAYABLE_DELAY);

        key(Key.RightArrow);
        key(Key.CBA1);
        assertItemCmdListener("hyperlink focus transfer (case1)", ok, hyperlink3);

        key(Key.RightArrow);
        key(Key.LeftArrow);
        key(Key.CBA1);
        assertItemCmdListener("hyperlink focus transfer (case2)", ok, hyperlink3);
    }

    /**
     * Tests that commands are working as expected when commands
     * are added or removed dynamically.
     *
     * @param mode Appearancemode of ImageItem.
     */
    public void testAppearanceModeChange(int mode) {
        switch (mode) {
            case ImageItem.PLAIN:
                print("Running test with appearance mode PLAIN.");
                break;
            case ImageItem.BUTTON:
                print("Running test with appearance mode BUTTON.");
                break;
            case ImageItem.HYPERLINK:
                print("Running test with appearance mode HYPERLINK.");
                break;
            default:
                fail("Invalid test (incorrect appearance mode).");
                break;
        }

        ImageItem imageItem = new ImageItem("label", smallImage, 0, "url", mode);

        Command ok = new Command("Ok", "", Command.ITEM, 0);

        form.append(imageItem);
        imageItem.addCommand(ok);
        imageItem.setItemCommandListener(this);

        setCurrent(form);

        // Remove command and make sure the command is not working anymore:
        imageItem.removeCommand(ok);
        block(CHANGE_DISPLAYABLE_DELAY);

        key(Key.CBA1);
        assertItemCmdListener("Removed command", null, null);

        // Add command again and verify it works:
        imageItem.addCommand(ok);
        block(CHANGE_DISPLAYABLE_DELAY);

		key(Key.CBA1);
        assertItemCmdListener("Added command", ok, imageItem);
    }

    /**
	 * Tests that focus transfer works when commands are added/removed
	 * dynamically.
	 *
	 * @param mode
	 *            Appearancemode of ImageItem.
	 */
    public void testFocusInAppearanceModeChange(int mode) {
        switch (mode) {
            case ImageItem.PLAIN:
                print("Running test with appearance mode PLAIN.");
                break;
            case ImageItem.BUTTON:
                print("Running test with appearance mode BUTTON.");
                break;
            case ImageItem.HYPERLINK:
                print("Running test with appearance mode HYPERLINK.");
                break;
            default:
                fail("Invalid test (incorrect appearance mode).");
                break;
        }

        ImageItem imageItem = new ImageItem(null, smallImage, 0, "url", mode);

        ImageItem button1 = new ImageItem(null, mediumImage, 0, "button1",
                ImageItem.BUTTON);
        ImageItem button2 = new ImageItem(null, smallImage, 0, "button2",
                ImageItem.BUTTON);

        Command ok = new Command("Ok", "", Command.ITEM, 0);
        Command buttonCommand1 = new Command("bc1", "", Command.ITEM, 0);
        Command buttonCommand2 = new Command("bc2", "", Command.ITEM, 0);

        form.append(button1);
        form.append(imageItem);
        form.append(button2);

        // Set listener but do not add commands yet:
        imageItem.setItemCommandListener(this);
        button1.addCommand(buttonCommand1);
        button2.addCommand(buttonCommand2);
        button1.setItemCommandListener(this);
        button2.setItemCommandListener(this);

        setCurrent(form);

        // At this point there are two buttons and plain ImageItem
        // in middle of them. Focus is in first button.
        // Move to second button, add command to ImageItem, move
        // to ImageItem and verify command works:
        key(Key.RightArrow);

        imageItem.addCommand(ok);
        block(CHANGE_DISPLAYABLE_DELAY);

        key(Key.LeftArrow);

        key(Key.CBA1);
        assertItemCmdListener("case1", ok, imageItem);

        // Now remove command from ImageItem. Test assumes that in this
        // case focus is moved to next item which is the second button.
        // Verify that happens and also make sure it is possible to move
        // to first button also:
        imageItem.removeCommand(ok);
        block(CHANGE_DISPLAYABLE_DELAY);

        key(Key.CBA1);
        assertItemCmdListener("case2", buttonCommand2, button2);

        key(Key.LeftArrow);

        key(Key.CBA1);
        assertItemCmdListener("case3", buttonCommand1, button1);

        // Now add command back to ImageItem again and make sure it
        // works:
        imageItem.addCommand(ok);
        block(CHANGE_DISPLAYABLE_DELAY);

        key(Key.RightArrow);

        key(Key.CBA1);
        assertItemCmdListener("case4", ok, imageItem);

        // And verify also the functionality of first button after
        // these changes:
        key(Key.LeftArrow);
        key(Key.CBA1);
        assertItemCmdListener("case5", buttonCommand1, button1);
    }

    /**
     * test that ImageItem of HYPERLINK type working ok with when image is
     * bigger then screensize.
     */
    public void testLongImageHyperlink() {
        ImageItem imageItem = new ImageItem(null, smallImage, 0, "button",
                ImageItem.BUTTON);

        ImageItem hyperlink1 = new ImageItem(null, longImage, 0, "hyperlink",
                ImageItem.HYPERLINK);
        hyperlink1.setLayout(Item.LAYOUT_RIGHT);

        ImageItem hyperlink2 = new ImageItem(null, smallImage, 0, "button2",
                ImageItem.BUTTON);
        hyperlink2.setLayout(Item.LAYOUT_CENTER);

        Form forma = new Form("Form A");
        Form formb = new Form("Form B");
        Command ok = new Command("Ok", "", Command.ITEM, 0);
        Command hyperCommand1 = new Command("hc1", "", Command.ITEM, 0);
        Command hyperCommand2 = new Command("hc2", "", Command.ITEM, 0);
        imageItem.addCommand(ok);
        hyperlink1.addCommand(hyperCommand1);
        hyperlink2.addCommand(hyperCommand2);

        imageItem.setItemCommandListener(this);
        hyperlink1.setItemCommandListener(this);
        hyperlink2.setItemCommandListener(this);

        forma.append(imageItem);
        forma.append(hyperlink1);
        forma.append(hyperlink2);
        setCurrent(forma);
        key(Key.CBA1);
        // check that by default first item is activated.
        assertItemCmdListener("case1", ok, imageItem);

        key(Key.DownArrow);
        key(Key.CBA1);
        // change focusable item to next (long item) and check that it is
        // processed correctly.
        assertItemCmdListener("case2", hyperCommand1, hyperlink1);

        // change to another form and then back, and check that commands still
        // working OK
        setCurrent(formb);
        setCurrent(forma);
        key(Key.CBA1);
        // change focusable item to next (long item) and check that it is
        // processed correctly.
        assertItemCmdListener("case3", hyperCommand1, hyperlink1);

        // change to last Item and check that commands are correct
        key(Key.DownArrow);
        key(Key.DownArrow);
        key(Key.DownArrow);
        key(Key.DownArrow);
        key(Key.DownArrow);
        key(Key.CBA1);
        assertItemCmdListener("case4", hyperCommand2, hyperlink2);
    }

}
