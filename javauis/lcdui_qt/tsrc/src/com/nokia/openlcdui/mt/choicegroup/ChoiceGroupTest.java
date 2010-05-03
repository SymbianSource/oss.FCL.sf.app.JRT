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
package com.nokia.openlcdui.mt.choicegroup;

import junit.framework.*;

import java.io.IOException;

import javax.microedition.lcdui.*;

import com.nokia.openlcdui.mt.SWTTestCase;

/**
 * TEST CASE SPECIFICATION <br>
 * <br>
 * Short description of the module test:
 * <li>Collection of tests to test ChoiceGroup's API functionality.<br>
 *
 * @created 1.8.2008
 */
public class ChoiceGroupTest extends SWTTestCase {

    private static final int NUMITEMS = 10;

    private Image itemImage;

    /**
     * Constructor.
     */
    public ChoiceGroupTest() {
    }

    /**
     * Constructor.
     *
     * @param sTestName name of the test
     * @param rTestMethod TestMethod used
     */
    public ChoiceGroupTest(String sTestName) {
        super(sTestName);
    }

    /**
     * Any pre-test setup can be done here
     */
    protected void setUp() throws Exception {
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
	    methodNames = ChoiceGroupTest.methodNames();
	    e = methodNames.elements();
	    while (e.hasMoreElements()) {
	        suite.addTest(new ChoiceGroupTest((String)e.nextElement()));
	    }
        
        return suite;
    }

    public static java.util.Vector methodNames() {
        java.util.Vector methodNames = new java.util.Vector();
        methodNames.addElement("testConstructors");
        methodNames.addElement("testAccessors1");
        methodNames.addElement("testAccessors2");
        methodNames.addElement("testMultiple1");
        methodNames.addElement("testMultiple2");
        methodNames.addElement("testMoveBetweenForms");
        return methodNames;
    }
    
    protected void runTest() throws Throwable {
        if (getName().equals("testConstructors")) testConstructors();
        else if (getName().equals("testAccessors1")) testAccessors(false);
        else if (getName().equals("testAccessors2")) testAccessors(true);
        else if (getName().equals("testMultiple1")) testMultiple(false);
        else if (getName().equals("testMultiple2")) testMultiple(true);
        else if (getName().equals("testMoveBetweenForms")) testMoveBetweenForms();
        else super.runTest();
    }        
    
    /**
     * Test method for constructors.
     */
    public void testConstructors() {
        try {
            new ChoiceGroup(null, 0);
            fail("IllegalArgumentException should be thrown");
        }
        catch (IllegalArgumentException iae) {
            // OK
        }

        new ChoiceGroup(null, Choice.EXCLUSIVE);

        new ChoiceGroup(null, Choice.MULTIPLE);

        new ChoiceGroup(null, Choice.POPUP);

        try {
            new ChoiceGroup(null, Choice.IMPLICIT);
            fail("IllegalArgumentException should be thrown");
        }
        catch (IllegalArgumentException iae) {
            // OK
        }

        try {
            new ChoiceGroup(null, Choice.EXCLUSIVE, null, null);
            fail("NullPointerException should be thrown");
        }
        catch (NullPointerException npe) {
            // OK
        }

        String[] goodStrArr = new String[] { "1", "2", "3", "4" };
        String[] badStrArr = new String[] { "1", null, "2", "3" };
        Image[] goodImgArr = new Image[] { itemImage, itemImage, itemImage,
                itemImage, };
        Image[] badImgArr = new Image[] { itemImage, itemImage, itemImage, };

        try {
            new ChoiceGroup(null, Choice.EXCLUSIVE, badStrArr, null);
            fail("NullPointerException should be thrown - "
                    + "for null item in text array");
        }
        catch (NullPointerException npe) {
            // OK
        }

        try {
            new ChoiceGroup(null, Choice.EXCLUSIVE, goodStrArr, badImgArr);
            fail("IllegalArgumentException should be thrown - "
                    + "text and image array length mismatch");
        }
        catch (IllegalArgumentException iae) {
            // OK
        }

        new ChoiceGroup(null, Choice.EXCLUSIVE, goodStrArr, goodImgArr);

    }

    /**
     * Test method for different accessor method (add/remove/fonts/selections).
     */
    public void testAccessors(boolean inForm) {
        ChoiceGroup choicegroup = new ChoiceGroup(null, Choice.EXCLUSIVE);
        if (inForm) {
            Form form = new Form(getName());
            form.append(choicegroup);
        }

        choicegroup.setFitPolicy(Choice.TEXT_WRAP_DEFAULT);
        assertEquals("Fitpolicy mismatch", Choice.TEXT_WRAP_DEFAULT,
                choicegroup.getFitPolicy());

        choicegroup.setFitPolicy(Choice.TEXT_WRAP_ON);
        assertEquals("Fitpolicy mismatch", Choice.TEXT_WRAP_ON, choicegroup
                .getFitPolicy());

        choicegroup.setFitPolicy(Choice.TEXT_WRAP_OFF);
        assertEquals("Fitpolicy mismatch", Choice.TEXT_WRAP_OFF, choicegroup
                .getFitPolicy());

        try {
            choicegroup.setFitPolicy(Choice.TEXT_WRAP_OFF + 1);
            fail("IllegalArgumentException not thrown - "
                    + "for invalid fit policy");
        }
        catch (IllegalArgumentException iae) {
            assertEquals("Fitpolicy mismatch", Choice.TEXT_WRAP_OFF,
                    choicegroup.getFitPolicy());
        }

        // append
        for (int i = 0; i < NUMITEMS; i++) {
            choicegroup.append("item" + i, itemImage);
        }
        assertEquals("ChoiceGroup size - after append", NUMITEMS, choicegroup
                .size());

        // invalid append
        try {
            choicegroup.append(null, itemImage);
            fail("NullPointerException not thrown - "
                    + "for invalid string append");
        }
        catch (NullPointerException npe) {
            // Fall-through
        }

        // delete
        for (int i = 0; i < NUMITEMS; i++) {
            choicegroup.delete(0);
        }
        assertEquals("ChoiceGroup size - after delete", 0, choicegroup.size());

        // insert
        for (int i = 0; i < NUMITEMS; i++) {
            choicegroup.insert(0, "item" + i, itemImage);
        }
        assertEquals("ChoiceGroup size - after insert", NUMITEMS, choicegroup
                .size());

        // invalid insert
        try {
            choicegroup.insert(NUMITEMS * 2, "item" + NUMITEMS * 2, itemImage);
            fail("IndexOutOfBoundsException not thrown - "
                    + "for invalid insert index");
        }
        catch (IndexOutOfBoundsException iobe) {
            // Fall-through
        }

        // set
        for (int i = 0; i < NUMITEMS; i++) {
            choicegroup.set(i, "item" + i * 2, itemImage);
        }
        assertEquals("ChoiceGroup size - after set", NUMITEMS, choicegroup
                .size());

        // check item strings individually
        for (int i = 0; i < NUMITEMS; i++) {
            assertEquals("Item " + i + " mismatch", "item" + i * 2, choicegroup
                    .getString(i));
        }

        // create fonts to plain=0, bold=1, italic=2
        Font[] fonts = new Font[3];
        for (int i = 0; i < 3; i++) {
            fonts[i] = Font.getFont(Font.FACE_SYSTEM, i, Font.SIZE_MEDIUM);
        }
        // set list fonts
        for (int i = 0; i < NUMITEMS; i++) {
            choicegroup.setFont(i, fonts[i % 3]);
        }
        // check item fonts individually
        for (int i = 0; i < NUMITEMS; i++) {
            assertEquals("Font " + i + " mismatch", fonts[i % 3], choicegroup
                    .getFont(i));
        }

        int selIndex = choicegroup.getSelectedIndex();
        assertTrue("Selected item mismatch", choicegroup.isSelected(selIndex));

        // test multi-selection flags on single selection Choicegroup
        boolean[] setSelArr = new boolean[NUMITEMS];
        // select every 3rd item
        for (int i = 1; i < NUMITEMS; i++) {
            setSelArr[i] = (i % 3 == 0);
        }
        choicegroup.setSelectedFlags(setSelArr);
        assertTrue("Selected item mismatch - after multisel array",
                choicegroup.isSelected(3));

        // delete all
        choicegroup.deleteAll();
        assertEquals("ChoiceGroup size - after delete all", 0, choicegroup
                .size());
    }

    /**
     * Test method for checking multiple selection functionality.
     */
    public void testMultiple(boolean inForm) {
        ChoiceGroup choicegroup = new ChoiceGroup(null, Choice.MULTIPLE);
        if (inForm) {
            Form form = new Form("Form");
            form.append(choicegroup);
        }

        // append
        for (int i = 0; i < NUMITEMS; i++) {
            choicegroup.append("item" + i, itemImage);
        }
        assertEquals("ChoiceGroup size - after append", NUMITEMS, choicegroup
                .size());

        try {
            choicegroup.setSelectedFlags(null);
            fail("NullPointerException not thrown - "
                    + "for null selected array");
        }
        catch (NullPointerException npe) {
            // OK
        }

        try {
            boolean[] badSelArr = new boolean[NUMITEMS / 2];
            choicegroup.setSelectedFlags(badSelArr);
            fail("IllegalArgumentException not thrown - "
                    + "for invalid selected array size");
        }
        catch (IllegalArgumentException iae) {
            // OK
        }

        boolean[] setSelArr = new boolean[NUMITEMS * 2];
        boolean[] getSelArr = new boolean[NUMITEMS * 2];

        // select odd items
        for (int i = 0; i < NUMITEMS; i++) {
            setSelArr[i] = (i % 2 == 0);
        }
        choicegroup.setSelectedFlags(setSelArr);
        choicegroup.getSelectedFlags(getSelArr);
        // check each item individually
        for (int i = 0; i < NUMITEMS; i++) {
            assertTrue("Selection mismatch at item " + i,
                    setSelArr[i] == getSelArr[i]);
        }

        // flip each item
        for (int i = 0; i < NUMITEMS; i++) {
            choicegroup.setSelectedIndex(i, !setSelArr[i]);
        }

        // check each item individually
        for (int i = 0; i < NUMITEMS; i++) {
            assertTrue("Selection mismatch at item " + i,
                    setSelArr[i] != choicegroup.isSelected(i));
        }
    }

    /**
     * Test method for moving a ChoiceGroup between two forms,
     * while keeping its internal state.
     */
    public void testMoveBetweenForms() {
        Form form1 = new Form("Form1");
        Form form2 = new Form("Form2");
        ChoiceGroup choicegroup = new ChoiceGroup(null, Choice.MULTIPLE);
        form1.append(choicegroup);

        // append1
        for (int i = 0; i < NUMITEMS; i++) {
            choicegroup.append("form1 item" + i, itemImage);
        }
        assertEquals("ChoiceGroup size - after 1. append ", NUMITEMS,
                choicegroup.size());
        // flip selection
        for (int i = 0; i < NUMITEMS; i++) {
            choicegroup.setSelectedIndex(i, !choicegroup.isSelected(i));
        }

        form1.deleteAll();

        // append 2
        for (int i = 0; i < NUMITEMS; i++) {
            choicegroup.append("noform item" + i, itemImage);
        }
        assertEquals("ChoiceGroup size - after 2. append ", 2 * NUMITEMS,
                choicegroup.size());

        // flip selection
        for (int i = 0; i < 2 * NUMITEMS; i++) {
            choicegroup.setSelectedIndex(i, !choicegroup.isSelected(i));
        }

        form2.append(choicegroup);

        // append 3
        for (int i = 0; i < NUMITEMS; i++) {
            choicegroup.append("form2 item" + i, itemImage);
        }
        assertEquals("ChoiceGroup size - after 3. append ", 3 * NUMITEMS,
                choicegroup.size());

        // flip selection
        for (int i = 0; i < 3 * NUMITEMS; i++) {
            choicegroup.setSelectedIndex(i, !choicegroup.isSelected(i));
        }

        // Check selection (
        for (int i = 0; i < NUMITEMS; i++) {
            assertTrue("Selection mismatch at item " + i,
                    choicegroup.isSelected(i));
        }

        for (int i = NUMITEMS; i < 2 * NUMITEMS; i++) {
            assertTrue("Selection mismatch at item " + i,
                    !choicegroup.isSelected(i));
        }

        for (int i = 2 * NUMITEMS; i < 3 * NUMITEMS; i++) {
            assertTrue("Selection mismatch at item " + i,
                    choicegroup.isSelected(i));
        }
    }

}
