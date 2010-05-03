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
package com.nokia.openlcdui.mt_uirobot.textfield;

import junit.framework.*;

import javax.microedition.lcdui.*;

import com.nokia.openlcdui.mt_uirobot.UITestBase;

import com.nokia.mj.impl.uitestutils.Key;

/**
 * TEST CASE SPECIFICATION <br>
 * <br>
 * Short description of the module test:
 * <li> UI-robot based test to test TextField's focus behavior. <br>
 * <br>
 * Created: 2008-09-15
 */
public class FormChangeFocusCheckTest extends UITestBase {

    private static int maxsize = 2000;

    /**
     * Constructor.
     */
    public FormChangeFocusCheckTest() {
    }

    /**
     * Constructor.
     *
     * @param sTestName Test name.
     * @param rTestMethod Test method.
     */
    public FormChangeFocusCheckTest(String sTestName) {
        super(sTestName);
    }

    public static Test suite() {
        TestSuite suite = new TestSuite();

        java.util.Vector methodNames;
	    java.util.Enumeration e;

	    // Add widget tests
	    methodNames = FormChangeFocusCheckTest.methodNames();
	    e = methodNames.elements();
	    while (e.hasMoreElements()) {
	        suite.addTest(new FormChangeFocusCheckTest((String)e.nextElement()));
	    }
        
        return suite;
    }

    public static java.util.Vector methodNames() {
        java.util.Vector methodNames = new java.util.Vector();
        methodNames.addElement("testFormChangeAndFocus");
        methodNames.addElement("setTextFieldToAnotherFormTest");
        methodNames.addElement("removeTextFieldSetTextTest");
        methodNames.addElement("testCaretPossitionAfterDisplayableChange");
        return methodNames;
    }
    
    public void runTest() throws Throwable {
        if (getName().equals("testFormChangeAndFocus")) testFormChangeAndFocus();
        else if (getName().equals("setTextFieldToAnotherFormTest")) setTextFieldToAnotherFormTest();
        else if (getName().equals("removeTextFieldSetTextTest")) removeTextFieldSetTextTest();
        else if (getName().equals("testCaretPossitionAfterDisplayableChange")) testCaretPossitionAfterDisplayableChange();
        else super.runTest();
    }

    /**
	 * Test that focus behaves correct after switching displayables when
	 * setCurrentItem is used.
	 */
    public void testFormChangeAndFocus() {
        Form forma =  new Form ("Form A");
        Form formb =  new Form ("Form B");
        StringItem first = new StringItem (null, "first");
        String expectedContent = "Adg";
        String expectedContent2 = "Adgjmp";
        TextField tf1  = new TextField("name", null, maxsize, TextField.ANY);
        forma.append(first);
        forma.append("row");
        forma.append("\nrow");
        forma.append("\nrow");
        forma.append("\nrow");
        forma.append("\nrow");
        forma.append("\nrow");
        forma.append("\nrow");
        forma.append("\nrow");
        forma.append("\nrow");
        forma.append("\nrow");
        forma.append("\nrow");
        forma.append("\nrow");
        forma.append("\nrow");
        forma.append("\nrow");
        forma.append("\nrow");
        forma.append("\nrow");
        forma.append("\nrow");
        forma.append("\nrow");
        forma.append("\nrow");
        forma.append("\nrow");
        forma.append("\nrow");
        forma.append("\nrow");
        forma.append("\nrow");
        forma.append("\nrow");
        forma.append(tf1);
        forma.append("\nrow");
        forma.append("\nrow");
        forma.append("\nrow");
        forma.append("\nrow");
        forma.append("\nrow");
        forma.append("\nrow");
        forma.append("\nrow");
        formb.append("\nrow");
        formb.append("\nrow");
        formb.append("\nrow");
        formb.append("\nrow");
        formb.append("\nrow");
        formb.append("\nrow");
        formb.append("\nrow");

        display.setCurrentItem(tf1);
        // seems that in case of setCurrentItem we need to wait more time to
		// change displayable.
		block(1000);

        key('2'); // A
		key('3'); // d
		key('4'); // g
		assertEquals("(case1) No text was inputed", expectedContent, tf1
				.getString());
		setCurrent(formb);

		setCurrent(forma);

		block(1000);//
		// key('5', 0); //
		key('5'); // k
		key('6'); // m
		key('7'); // p
		assertEquals("(case2)No text was inputed after switching Displayble",
				expectedContent2, tf1.getString());
		display.setCurrentItem(first);
		block(CHANGE_DISPLAYABLE_DELAY);

        key('2'); // a
		key('3'); // d
		key('4'); // g
		assertEquals("(case3)text was inputed when setCurrentItem was set to "
				+ "StringItem", expectedContent2, tf1.getString());
		setCurrent(formb);
		block(CHANGE_DISPLAYABLE_DELAY);

		setCurrent(forma);
		block(CHANGE_DISPLAYABLE_DELAY);

		key('2'); // a
		key('3'); // d
		key('4'); // g
        assertEquals("(case4)text was inputed when setCurrent was set to "
                + "back to Form A and TextField was not visible",
                expectedContent2, tf1.getString());
    }

    /**
	 * Check that caretPosition is remains the same when TextField is set to
	 * other form.
	 */
	public void setTextFieldToAnotherFormTest() {
		String expectedContent = "Boaddy";
		Form forma = new Form("Form A");
		Form formb = new Form("Form B");
		TextField tf = new TextField("hader", "Body", maxsize, TextField.ANY);
		forma.append(tf);

		setCurrent(forma);
		key(Key.LeftArrow);
		key(Key.LeftArrow); // caret o|d
		forma.delete(0);
		formb.append(tf);
		setCurrent(formb);
		key('2'); // a
		key('3'); // d
		assertEquals("caret position was not the same after assigning TextField"
						+ " to another form", expectedContent, tf.getString());
		forma = null;
		formb = null;
		tf = null;
	}

    /**
	 * Check that get caretPosition return correctPosition after TextField
	 * Removed from the Form and setString() is used.
	 */
	public void removeTextFieldSetTextTest() {
		Form form = new Form("Form");
		TextField tf = new TextField("header", "some text", maxsize,
				TextField.ANY);
		form.append(tf);
		setCurrent(form);
		// enter some text to TextField
		key('2'); // a
		key('3'); // d
		form.delete(0);
		tf.setString("four");
		assertEquals("caret position was not set to correct, after setStrin()",
				tf.size(), tf.getCaretPosition());
		form = null;
		tf = null;
	}

    /**
	 * Check that caret positioned correctly after switching displayables.
	 */
	public void testCaretPossitionAfterDisplayableChange() {
		Form forma = new Form("Form A");
		Form formb = new Form("Form B");
		TextField tf1 = new TextField("header", null, maxsize, TextField.ANY);
		TextField tf2 = new TextField("header", null, maxsize, TextField.ANY);
		tf1.setString("some text that should occupy few lines");
		tf2.setString("some text that should occupy few lines also");
		forma.append(tf1);
		forma.append(tf2);
		setCurrent(forma);
		key(Key.LeftArrow);
		key(Key.LeftArrow);
		assertEquals("caret is not on the correct place", tf1.size() - 2, tf1
				.getCaretPosition());
		setCurrent(formb);
		setCurrent(forma);
		assertEquals("caret is not on the correct place", tf1.size() - 2, tf1
				.getCaretPosition());
		//go to next textField
		key(Key.DownArrow);
		key(Key.DownArrow);
		key(Key.LeftArrow);
		key(Key.LeftArrow);
		assertEquals("caret is not on the correct place", tf2.size() - 1, tf2
				.getCaretPosition());
		setCurrent(formb);
		setCurrent(forma);
		assertEquals("caret is not on the correct place", tf2.size() - 1, tf2
				.getCaretPosition());
	}

}
