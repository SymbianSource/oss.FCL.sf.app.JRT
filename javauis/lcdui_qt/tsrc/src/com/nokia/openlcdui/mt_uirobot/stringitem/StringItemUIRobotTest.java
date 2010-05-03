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
package com.nokia.openlcdui.mt_uirobot.stringitem;

import junit.framework.*;

import javax.microedition.lcdui.*;

import com.nokia.openlcdui.mt_uirobot.UITestBase;

import com.nokia.mj.impl.uitestutils.Key;

/**
 * TEST CASE SPECIFICATION <br>
 * <br>
 * Short description of the module test:
 * <li> UI-robot based test to test StringItem. <br>
 * <br>
 * Created: 2008-08-27
 */
public class StringItemUIRobotTest extends UITestBase implements
        ItemCommandListener, CommandListener {

    private Form form;
    private StringItem button1;
    private StringItem button2;
    private StringItem button3;
    private StringItem si;
    private Command c1;
    private Command c2;
    private Command c3;

    /**
     * Constructor.
     */
    public StringItemUIRobotTest() {
    }

    /**
     * Constructor.
     *
     * @param sTestName Test name.
     * @param rTestMethod Test method.
     */
    public StringItemUIRobotTest(String sTestName) {
        super(sTestName);
    }

    public static Test suite() {
        TestSuite suite = new TestSuite();

        java.util.Vector methodNames;
	    java.util.Enumeration e;

	    // Add widget tests
	    methodNames = StringItemUIRobotTest.methodNames();
	    e = methodNames.elements();
	    while (e.hasMoreElements()) {
	        suite.addTest(new StringItemUIRobotTest((String)e.nextElement()));
	    }
        
        return suite;
    }

    public static java.util.Vector methodNames() {
        java.util.Vector methodNames = new java.util.Vector();
        methodNames.addElement("testSetTextNullEmpty");
        return methodNames;
    }
    
    public void runTest() throws Throwable {
        if (getName().equals("testSetTextNullEmpty")) testSetTextNullEmpty();
        else super.runTest();
    }

    /**
     * Test that StringItem.setText(text) working properly.
     * Only text = null and text = "" investigated.
     */
    public void testSetTextNullEmpty() {
        button1 = new StringItem("", "click!", Item.BUTTON);
        button2 = new StringItem("", "click 2!", Item.BUTTON);
        button3 = new StringItem("", "click 3!", Item.BUTTON);
        si = new StringItem("this is the label", "someString");
        c1 = new Command("Ok1", "", Command.ITEM, 0);
        c2 = new Command("Ok2", "", Command.ITEM, 0);
        c3 = new Command("Ok3", "", Command.ITEM, 0);
        form = new Form ("setTextTest");
        form.setCommandListener(this);
        button1.addCommand(c1);
        button1.setItemCommandListener(this);
        button2.addCommand(c2);
        button2.setItemCommandListener(this);
        button3.addCommand(c3);
        button3.setItemCommandListener(this);

        form.append(button1);
        form.append(button2);
        form.append(button3);
        form.append(si);

        setCurrent(form);

        key(Key.Select);
        assertEquals("StringItem content was not changed!", null, si.getText());
        key(Key.RightArrow);
        key(Key.Select);
        assertEquals("StringItem content was not changed!", null, si.getText());
        key(Key.RightArrow);
        key(Key.Select);
        assertEquals("StringItem content was not changed!", "some text", si
                .getText());

    }

    /**
     *
     * @param c Command.
     * @param item that triggered the command.
     */
    public void commandAction(Command c, Item item) {
        if (c == c1) {
            si.setText("");
        }
        if (c == c2) {
            si.setText(null);
        }
        if (c == c3) {
            si.setText("some text");
        }
    }

    /**
     * @param c Command.
     * @param d displayable that triggered the command.
     */
    public void commandAction(Command c, Displayable d) {

    }

}
