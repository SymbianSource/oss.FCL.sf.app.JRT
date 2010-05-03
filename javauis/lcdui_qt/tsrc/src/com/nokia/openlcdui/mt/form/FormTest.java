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
package com.nokia.openlcdui.mt.form;

import junit.framework.*;

import javax.microedition.lcdui.*;

import com.nokia.openlcdui.mt.SWTTestCase;

/**
 * TEST CASE SPECIFICATION <br>
 * <br>
 * Short description of the module test:
 * <li> Collection of tests to test Form's functionality. <br>
 * <br>
 * Created: 2008-07-10
 */
public class FormTest extends SWTTestCase {

    private static final int INVALID_APPEARANCE_MODE = 3;
    private static final int INVALID_LAYOUT = 4;

    /**
     * Constructor.
     */
    public FormTest() {
    }

    /**
     * Constructor.
     *
     * @param sTestName Test name.
     * @param rTestMethod Test method.
     */
    public FormTest(String sTestName) {
        super(sTestName);
    }

    /**
     * Creates the test suite. You need to add a new aSuite.addTest entry for
     * any new test methods, otherwise they won't be run.
     *
     * @return New testsuite.
     */
    public static Test suite() {
		TestSuite suite = new TestSuite();

	    java.util.Vector methodNames;
	    java.util.Enumeration e;

	    // Add widget tests
	    methodNames = FormTest.methodNames();
	    e = methodNames.elements();
	    while (e.hasMoreElements()) {
	        suite.addTest(new FormTest((String)e.nextElement()));
	    }
	    return suite;
    }

    public static java.util.Vector methodNames() {
        java.util.Vector methodNames = new java.util.Vector();
        methodNames.addElement("testFormException");
        methodNames.addElement("testAppendException");
        methodNames.addElement("testInsertException");
        methodNames.addElement("testDeleteException");
        methodNames.addElement("testSetException");
        methodNames.addElement("testGetException");
        methodNames.addElement("testSize");
        methodNames.addElement("testDeleteAll");
        methodNames.addElement("testGetWidthGetHeight");
        methodNames.addElement("testFormSetIOOBExceptions");
        methodNames.addElement("testFormSetNPExceptions");
        methodNames.addElement("testFormSetISExceptions");
        methodNames.addElement("testFormInsertIOOBExceptions");
        methodNames.addElement("testFormInsertNPExceptions");
        methodNames.addElement("testFormInsertISExceptions");
        methodNames.addElement("testFormGetIOOBExceptions");
        methodNames.addElement("testFormDeleteIOOBExceptions");
        methodNames.addElement("testFormAppend");
        methodNames.addElement("testFormAppendImage");
        methodNames.addElement("testFormConstructor");
        return methodNames;
    }
        
    protected void runTest() throws Throwable {
        if (getName().equals("testFormException")) testFormException();
        else if (getName().equals("testAppendException")) testAppendException();
        else if (getName().equals("testInsertException")) testInsertException();
        else if (getName().equals("testDeleteException")) testDeleteException();
        else if (getName().equals("testSetException")) testSetException();
        else if (getName().equals("testGetException")) testGetException();
        else if (getName().equals("testSize")) testSize();
        else if (getName().equals("testDeleteAll")) testDeleteAll();
        else if (getName().equals("testGetWidthGetHeight")) testGetWidthGetHeight();
        else if (getName().equals("testFormSetIOOBExceptions")) testFormSetIOOBExceptions();
        else if (getName().equals("testFormSetNPExceptions")) testFormSetNPExceptions();
        else if (getName().equals("testFormSetISExceptions")) testFormSetISExceptions();
        else if (getName().equals("testFormInsertIOOBExceptions")) testFormInsertIOOBExceptions();
        else if (getName().equals("testFormInsertNPExceptions")) testFormInsertNPExceptions();
        else if (getName().equals("testFormInsertISExceptions")) testFormInsertISExceptions();
        else if (getName().equals("testFormGetIOOBExceptions")) testFormGetIOOBExceptions();
        else if (getName().equals("testFormDeleteIOOBExceptions")) testFormDeleteIOOBExceptions();
        else if (getName().equals("testFormAppend")) testFormAppend();
        else if (getName().equals("testFormAppendImage")) testFormAppendImage();
        else if (getName().equals("testFormConstructor")) testFormConstructor();
        else super.runTest();
    }
        
        
    /**
     * Constructor Exception test.
     */
    public void testFormException() {
        Form formContainer = new Form("Form Title");
        StringItem itemOwned = new StringItem("label", "text", 0);
        formContainer.append(itemOwned);

        Item[] items1 = {new StringItem(null, "body",  0), itemOwned, };
        Item[] items2 = {new StringItem(null, "body2", 0), null, };

        try {
            Form formEx = new Form("header", items1);
            fail("IllegalStateException not thrown");
        }
        catch (IllegalStateException ise) {
        	// Ok
        }

        try {
            Form formEx = new Form("header", items2);
            fail("NullPointerException not thrown");
        }
        catch (NullPointerException npe) {
        	// OK
        }
    }

    /**
     * Tests that append throws correct exceptions in case of incorrect input.
     */
    public void testAppendException() {
        Form appendExForm = new Form("form title");
        Form container = new Form ("Form");
        String appendString = null;
        Image appendImage = null;
        StringItem appendStringItem = null;

        try {
            appendExForm.append(appendString);
            fail("NullPointerException not thrown if string is null");
        }
        catch (NullPointerException npe) {
        	// Ok
        }

        try {
            appendExForm.append(appendImage);
            fail("NullPointerException not thrown if image is null");
        }
        catch (NullPointerException npe) {
        	// Ok
        }

        try {
            appendExForm.append(appendStringItem);
            fail("NullPointerException not thrown if item is null");
        }
        catch (NullPointerException npe) {
        	// Ok
        }

        appendStringItem = new StringItem("turur", "tururu", 0);
        container.append(appendStringItem);
        try {
            appendExForm.append(appendStringItem);
            fail("IllegalStateException not thrown if item is already owned");
        }
        catch (IllegalStateException npe) {
        	// Ok
        }
    }

    /**
     * Form.insert()test.
     * Tests that insert() throws correct exceptions
     * in case of incorrect input.
     */
    public void testInsertException() {
        Form insertException = new Form("insertEx");
        Form formContainer = new Form("Form Title");
        StringItem itemOwned = new StringItem("label", "text", 0);
        formContainer.append(itemOwned);
        insertException.append("one item");
        insertException.append("second item");

        try {
            insertException.insert(3, new StringItem(null, "text", 0));
            fail("IndexOutOfBoundsException not thrown");
        }
        catch (IndexOutOfBoundsException iob) {
        	// Ok
        }

        try {
            insertException.insert(1, itemOwned);
            fail("IllegalStateException not thrown if Item is owned");
        }
        catch (IllegalStateException ise) {
        	// Ok
        }

        try {
            insertException.insert(1, null);
            fail("NullPointerException not thrown if Item is null");
        }
        catch (NullPointerException npe) {
        	// Ok
        }
    }

    /**
     * Form.delete() test.
     * Tests that delete() throws correct exception
     * in case of incorrect input.
     */
    public void testDeleteException() {
        Form deleteExForm = new Form("heder");
        deleteExForm.append("string1");
        deleteExForm.append("String2");
        try {
            deleteExForm.delete(2);
            fail("IndexOutOfBoundsException not thrown");
        }
        catch (IndexOutOfBoundsException e) {
        	// Ok
        }
    }

    /**
     * Form.set() test.
     * Tests that set() throws correct exceptions
     * in case of incorrect input.
     */
    public void testSetException() {
        Form setForm = new Form ("set Form");
        setForm.append("string 1");
        setForm.append("string 2");
        Form formContainer = new Form("Form Title");
        StringItem itemOwned = new StringItem("label", "text", 0);
        formContainer.append(itemOwned);

        try {
            setForm.set(3, new StringItem(null, "text", 0));
            fail("IndexOutOfBoundsException not thrown");
        }
        catch (IndexOutOfBoundsException e) {
        	// Ok
        }

        try {
            setForm.set(1, itemOwned);
            fail("IllegalStateException not thrown if Item is owned");
        }
        catch (IllegalStateException e) {
        	// Ok
        }

        try {
            setForm.set(1, null);
            fail("NullPointerException not thrown if Item is null");
        }
        catch (NullPointerException npe) {
        	// Ok
        }
    }

    /**
     * Form.get() test.
     * Tests that get() throws correct exceptions in case of incorrect input.
     */
    public void testGetException () {
        Form getForm = new Form("getForm");
        getForm.append("string1");
        getForm.append("String2");
        try {
            getForm.get(2);
            fail("IndexOutOfBoundsException not thrown");
        }
        catch (IndexOutOfBoundsException e) {
            // Ok
        }
    }

    /**
     *Form.size() test.
     *Tests that size() returns correct result.
     */
    public void testSize() {
        Form formSize = new Form("form header");
        formSize.append("String 1");
        formSize.append("String 2");
        assertTrue("Form.size(). Expected return 2 but returned"
                + formSize.size(), formSize.size() == 2);
    }

    /**
     * Form deleteAll() test.
     * Tests that deleteAll() working properly
     */
    public void testDeleteAll() {
        Form formDeleteAll = new Form("deleteaAll");
        formDeleteAll.append("String 1");
        formDeleteAll.append("String 2");

        formDeleteAll.deleteAll();
        assertTrue("Form.deleteAll(). Expected size() returns 0 but returned"
                + formDeleteAll.size(), formDeleteAll.size() == 0);
    }

    /**
     *Form.getWidth(),Form.getHeight() test.
     *
     */
    public void testGetWidthGetHeight () {
        Form formGetWidthHeight = new Form ("getWidth() getHeight()");
        int getWidth = 0;
        int getHeight = 0;
        getWidth = formGetWidthHeight.getWidth();
        getHeight = formGetWidthHeight.getHeight();
        assertTrue("Form.getWidth(),getHeight(). Expected return not 0 but"
                + " returned width " + getWidth + " and height " + getHeight,
                (getWidth != 0) && (getHeight != 0));
    }

    /**
     * Enhanced test for IndexOutOfBoundsExceptions for Form.set().
     */
    public void testFormSetIOOBExceptions () {
        int []positions = {Integer.MIN_VALUE, Integer.MIN_VALUE + 1, -256,
                           -255, -1, 1, 255, 256, Integer.MAX_VALUE - 1,
                           Integer.MAX_VALUE, };
        Form f = new Form("Test");
        f.append("stub");

        for (int i = 0; i < positions.length; i++) {
            try {
				f.set(positions[i], new StringItem("0", "1"));
				fail("IndexOutOfBoundsException not thrown for value"
						+ positions[i]);
			}
            catch (IndexOutOfBoundsException e1) {
            	// Ok
            }
            catch (Exception e) {
                fail("Unexpected " + e);
                break;
            }
        }
    }

    /**
     * Enhanced test for NullPointerException in Form.set().
     */
    public void testFormSetNPExceptions() {
        Item[] items = {new StringItem("label", "text") };
        Form f1 = null;

        f1 = new Form("Form 1", items); // Assign items to f1

        try {
            f1.set(0, null);
            fail("NullPointerException not thrown");
        }
        catch (NullPointerException e1) {
            // Ok
        }
    }

    /**
     * enhanced test for IllegalStateException in Form.set().
     */
    public void testFormSetISExceptions () {
        Item []items = {new StringItem("label", "text")};
        Form f1 = null;
        Form f2 = null;

        f2 = new Form("Form 2", items); // Assign items to f2
        f1 = new Form("Form 1");
        f1.append("some");

        try {
            f1.set(0, items[0]);
            fail("IllegalStateException not thrown");
        }
        catch (IllegalStateException e1) {
            // Ok
        }
    }

    /**
     * Enhanced test for IndexOutOfBoundsException for Form.insert().
     */
    public void testFormInsertIOOBExceptions () {
        int []positions = {Integer.MIN_VALUE, Integer.MIN_VALUE + 1, -256,
                          -255, -1, 255, 256, Integer.MAX_VALUE - 1,
                          Integer.MAX_VALUE,
        };
        Form f = new Form("Test");
        f.append("stub");

        for (int i = 0; i < positions.length; i++) {
            try {
                f.insert(positions[i], new StringItem("0", "1"));
                fail("IndexOutOfBoundsException not thrown for value"
						+ positions[i]);
            }
            catch (IndexOutOfBoundsException e1) {
            	// Ok
            }
            catch (Exception e) {
                fail("Unexpected " + e);
            }
        }
    }

    /**
     * Enhanced test for NullPointerException for Form.insert().
     */
    public void testFormInsertNPExceptions () {
        Item []items = {new StringItem("label", "text")};
        Form f1 = null;

        f1 = new Form("Form 1", items); // Assign items to f1

        try {
            f1.insert(0, null);
            fail("NullPointerException not thrown");
        }
        catch (NullPointerException e1) {
            // Ok
        }
    }

    /**
     * Enhanced test for IllegalStateException for Form.insert().
     */
    public void testFormInsertISExceptions() {
        Item []items = {new StringItem("label", "text")};
        Form f1 = null;
        Form f2 = null;

        f2 = new Form("Form 2", items); // Assign items to f2
        f1 = new Form("Form 1");
        f1.append("stub");

        try {
            f1.insert(0, items[0]);
            fail("IllegalStateException not thrown");
        }
        catch (IllegalStateException e1) {
            // Ok
        }
    }

    /**
     * Enhanced test for IndexOutOfBoundsException for Form.get().
     */
    public void testFormGetIOOBExceptions() {
        int []positions = {Integer.MIN_VALUE, Integer.MIN_VALUE + 1, -256,
                            -255, -1, 1, 255, 256, Integer.MAX_VALUE - 1,
                            Integer.MAX_VALUE,
        };
        Form f = new Form("Test");
        f.append("stub");

        for (int i = 0; i < positions.length; i++) {
            try {
                f.get(positions[i]);
                fail("IndexOutOfBoundsException not thrown for value"
						+ positions[i]);
            }
            catch (IndexOutOfBoundsException e1) {
            	// Ok
            }
            catch (Exception e) {
                fail("Unexpected " + e);
            }
        }
    }

    /**
     * Enhanced test for IndexOutOfboundsException for form.delete().
     */
    public void testFormDeleteIOOBExceptions () {
        int []positions = {Integer.MIN_VALUE, Integer.MIN_VALUE + 1, -256,
                            -255, -1, 1, 255, 256, Integer.MAX_VALUE - 1,
                            Integer.MAX_VALUE,
        };
        Form form = new Form("Test");
        form.append("stub");

        for (int i = 0; i < positions.length; i++) {
            try {
                form.delete(positions[i]);
                fail("IndexOutOfBoundsException not thrown for value"
						+ positions[i]);
            }
            catch (IndexOutOfBoundsException e1) {
            	// Ok
            }
            catch (Exception e) {
                fail("Unexpected " + e);
            }
        }
    }

    /**
     * Enhanced test for IllegalStateException for form.delete().
     */
    public void testFormAppendISExceptions () {
        Item []items = {new StringItem("label", "text")};
        Form f1 = null;
        Form f2 = null;

        f1 = new Form("Form 1", items); // Assign items to f1
        f2 = new Form("Form 2"); // Assign items to f1

        try {
            f2.append(items[0]);
            fail("IllegalStateException not thrown");
        }
        catch (IllegalStateException e1) {
            // Ok
        }
    }

    /**
     * Enhanced Form.append() test.
     */
    public void testFormAppend() {
        String[] strings = {"string1", "string2", "string3" };
        Form form = new Form("Title");
        int ind = 0;

        for (int j = 0; j < strings.length; j++) {
            ind = form.append(strings[j]);

            if (!(((StringItem) form.get(ind)).getText().equals(strings[j]))) {
                fail("Failed at " + j);
            }
        }
    }

    /**
     * Enhanced test for Form.append().
     * appending image to a form
     */
    public void testFormAppendImage() {
        Image image = null;
        try {
            image = Image.createImage("100x100.png");
        }
        catch (Exception e) {
            e.printStackTrace();
        }
        Form form = new Form("Title");
        int index = 0;
        index = form.append(image);
        if (!(((ImageItem) form.get(index)).getImage() == image)) {
            fail("Failed to append image");
        }
    }

    /**
     * Enhanced test for Form (String title, Item [] items.
     */
    public void testFormConstructor() {
        String title = "Title";
        Item[][] items = {{}, null, };
        Form form = null;

        for (int j = 0; j < items.length; j++) {
            try {
                form = new Form(title, items[j]);
            }
            catch (Exception e) {
                fail("Unexpected " + e);
            }

            if (!(form.size() == 0)) {
                fail("Failed");
            }
        }
    }

}
