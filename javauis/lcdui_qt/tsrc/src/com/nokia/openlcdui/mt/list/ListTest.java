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
package com.nokia.openlcdui.mt.list;

import junit.framework.*;

import java.io.IOException;

import javax.microedition.lcdui.*;

import com.nokia.openlcdui.mt.SWTTestCase;

/**
 * TEST CASE SPECIFICATION <br>
 * <br>
 * Short description of the module test:
 * <li>Collection of tests to test List's API functionality.<br>
 *
 * @created 1.8.2008
 */
public class ListTest extends SWTTestCase {

    private static final int NUMITEMS = 20;

    private Image itemImage;

    /**
     * Constructor.
     */
    public ListTest() {
    }

    /**
     * Constructor.
     *
     * @param sTestName name of the test
     * @param rTestMethod TestMethod used
     */
    public ListTest(String sTestName) {
        super(sTestName);
    }

    /**
     * Any pre-test setup can be done here
     */
    protected void setUp() throws Exception {
    	super.setUp();
        try {
            itemImage = Image.createImage("32x32.jpg");
        }
        catch (IOException ioe) {
            ioe.printStackTrace();
        }
    }

    /**
     * To create the test suite. You need to add a new aSuite.addTest entry for
     * any new test methods.
     *
     * @return New TestSuite.
     */
    public static Test suite() {
		TestSuite suite = new TestSuite();

	    java.util.Vector methodNames;
	    java.util.Enumeration e;

	    // Add widget tests
	    methodNames = ListTest.methodNames();
	    e = methodNames.elements();
	    while (e.hasMoreElements()) {
	        suite.addTest(new ListTest((String)e.nextElement()));
	    }

		return suite;
	}

    public static java.util.Vector methodNames() {
        java.util.Vector methodNames = new java.util.Vector();
        methodNames.addElement("testConstructors");
        methodNames.addElement("testAccessors");
        methodNames.addElement("testMultiList");
        return methodNames;
    }
    
    protected void runTest() throws Throwable {
        if (getName().equals("testConstructors")) testConstructors();
        else if (getName().equals("testAccessors")) testAccessors();
        else if (getName().equals("testMultiList")) testMultiList();
        else super.runTest();
    }

    /**
     * Test method.
     */
    public void testConstructors() {
        try {
            new List(null, 0);
            fail("IllegalArgumentException should be thrown");
        }
        catch (IllegalArgumentException iae) {
            // OK
        }

        new List(null, Choice.EXCLUSIVE);

        new List(null, Choice.MULTIPLE);

        new List(null, Choice.IMPLICIT);

        try {
            new List(null, Choice.IMPLICIT, null, null);
            fail("NullPointerException should be thrown");
        }
        catch (NullPointerException npe) {
            // OK
        }

        String[] goodStrArr = new String[] {"1", "2", "3", "4" };
        String[] badStrArr = new String[] {"1", null, "2", "3" };
        Image[] goodImgArr = new Image[] {itemImage, itemImage, itemImage,
            itemImage, };
        Image[] badImgArr = new Image[] {itemImage, itemImage, itemImage, };

        try {
            new List(null, Choice.IMPLICIT, badStrArr, null);
            fail("NullPointerException should be thrown - "
                    + "for null item in text array");
        }
        catch (NullPointerException npe) {
            // OK
        }

        try {
            new List(null, Choice.IMPLICIT, goodStrArr, badImgArr);
            fail("IllegalArgumentException should be thrown - "
                    + "text and image array length mismatch");
        }
        catch (IllegalArgumentException iae) {
            // OK
        }

        new List(null, Choice.IMPLICIT, goodStrArr, goodImgArr);

    }

    /**
     * Test method.
     */
    public void testAccessors() {
        List list = new List(null, Choice.IMPLICIT);

        list.setFitPolicy(Choice.TEXT_WRAP_DEFAULT);
        assertEquals("Fitpolicy mismatch", Choice.TEXT_WRAP_DEFAULT, list
                .getFitPolicy());

        list.setFitPolicy(Choice.TEXT_WRAP_ON);
        assertEquals("Fitpolicy mismatch", Choice.TEXT_WRAP_ON, list
                .getFitPolicy());

        list.setFitPolicy(Choice.TEXT_WRAP_OFF);
        assertEquals("Fitpolicy mismatch", Choice.TEXT_WRAP_OFF, list
                .getFitPolicy());

        try {
            list.setFitPolicy(Choice.TEXT_WRAP_OFF + 1);
            fail("IllegalArgumentException not thrown - "
                    + "for invalid fit policy");
        }
        catch (IllegalArgumentException iae) {
            assertEquals("Fitpolicy mismatch", Choice.TEXT_WRAP_OFF, list
                    .getFitPolicy());
        }

        // append
        for (int i = 0; i < NUMITEMS; i++) {
            list.append("item" + i, itemImage);
        }
        assertEquals("List size - after append", NUMITEMS, list.size());

        // invalid append
        try {
            list.append(null, itemImage);
            fail("NullPointerException not thrown - "
                    + "for invalid string append");
        }
        catch (NullPointerException npe) {
            // Fall-through
        }

        // delete
        for (int i = 0; i < NUMITEMS; i++) {
            list.delete(0);
        }
        assertEquals("List size - after delete", 0, list.size());

        // insert
        for (int i = 0; i < NUMITEMS; i++) {
            list.insert(0, "item" + i, itemImage);
        }
        assertEquals("List size - after insert", NUMITEMS, list.size());

        // invalid insert
        try {
            list.insert(NUMITEMS * 2, "item" + NUMITEMS * 2, itemImage);
            fail("IndexOutOfBoundsException not thrown - "
                    + "for invalid insert index");
        }
        catch (IndexOutOfBoundsException iobe) {
            // Fall-through
        }

        // set
        for (int i = 0; i < NUMITEMS; i++) {
            list.set(i, "item" + i * 2, itemImage);
        }
        assertEquals("List size - after set", NUMITEMS, list.size());

        // check item strings individually
        for (int i = 0; i < NUMITEMS; i++) {
            assertEquals("Item " + i + " mismatch", "item" + i * 2, list
                    .getString(i));
        }

        // create fonts to plain=0, bold=1, italic=2
        Font[] fonts = new Font[3];
        for (int i = 0; i < 3; i++) {
            fonts[i] = Font.getFont(Font.FACE_SYSTEM, i, Font.SIZE_MEDIUM);
        }
        // set list fonts
        for (int i = 0; i < NUMITEMS; i++) {
            list.setFont(i, fonts[i % 3]);
        }
        // check item fonts individually
        for (int i = 0; i < NUMITEMS; i++) {
            assertEquals("Font " + i + " mismatch", fonts[i % 3], list
                    .getFont(i));
        }

        int selIndex = list.getSelectedIndex();
        assertTrue("Selected item mismatch", list.isSelected(selIndex));

        // delete all
        list.deleteAll();
        assertEquals("List size - after delete all", 0, list.size());
    }

    /**
     * Test method.
     */
    public void testMultiList() {
        List list = new List(null, Choice.MULTIPLE);

        // append
        for (int i = 0; i < NUMITEMS; i++) {
            list.append("item" + i, itemImage);
        }
        assertEquals("List size - after append", NUMITEMS, list.size());

        try {
            list.setSelectedFlags(null);
            fail("NullPointerException not thrown - "
                    + "for null selected array");
        }
        catch (NullPointerException npe) {
            // OK
        }

        try {
            boolean[] badSelArr = new boolean[NUMITEMS / 2];
            list.setSelectedFlags(badSelArr);
            fail("IllegalArgumentException not thrown - "
                    + "for invalid selected array size");
        }
        catch (IllegalArgumentException iae) {
            // OK
        }

        boolean[] setSelArr = new boolean[NUMITEMS];
        boolean[] getSelArr = new boolean[NUMITEMS];

        // select odd items
        for (int i = 0; i < NUMITEMS; i++) {
            setSelArr[i] = (i % 2 == 0);
        }
        list.setSelectedFlags(setSelArr);
        list.getSelectedFlags(getSelArr);
        // check each item individually
        for (int i = 0; i < NUMITEMS; i++) {
            assertTrue("Selection mismatch at item " + i,
                    setSelArr[i] == getSelArr[i]);
        }

        // flip each item
        for (int i = 0; i < NUMITEMS; i++) {
            list.setSelectedIndex(i, !setSelArr[i]);
        }

        // check each item individually
        for (int i = 0; i < NUMITEMS; i++) {
            assertTrue("Selection mismatch at item " + i, setSelArr[i] != list
                    .isSelected(i));
        }
    }

}
