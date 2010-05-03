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
package com.nokia.openlcdui.mt.textbox;

import junit.framework.*;

import javax.microedition.lcdui.TextBox;
import javax.microedition.lcdui.TextField;

import com.nokia.openlcdui.mt.SWTTestCase;

/**
 * TEST CASE SPECIFICATION
 *
 * <br><br>Short description of the module test:
 * <li> This module test tests TextBox class.
 *
 * <br><br>
 * Created:    2008-04-23
 *
 */
public class TextBoxTest extends SWTTestCase {

    private static final int LENGTH = 100;
    private static final int LENGTH_BIG = 32000;
    private static final int LENGTH_INCORRECT = 0;
    private static final int LENGTH_SMALL = 5;
    private static final int MINUS_INV_CONSTRAINT = -5;
    private static final int BIG_INV_CONSTRAINT = 6;
    private static final int INV_TEXT_FLAG = 4194304; //10000000000000000000000
    private static final int START_INDEX = 5;
    private static final int COPY_COUNT = 7;
    private static final int MINUS_VALUE = -2;
    private static final int EXPECTED_SIZE = 12;

    private static final String LONG_TEXT = "Lorem ipsum dolor sit amet, "
        + "consectetur adipisicing elit, sed do eiusmod tempor incididunt"
        + " ut labore et dolore magna aliqua. Ut enim ad minim veniam, "
        + "quis nostrud exercitation ullamco laboris nisi ut aliquip ex "
        + "ea commodo consequat. Duis aute irure dolor in reprehenderit "
        + "in voluptate velit esse cillum dolore eu fugiat nulla pariatur."
        + " Excepteur sint occaecat cupidatat non proident, sunt in culpa"
        + " qui officia deserunt mollit anim id est laborum."
        + "Lorem ipsum dolor sit amet, "
        + "consectetur adipisicing elit, sed do eiusmod tempor incididunt"
        + " ut labore et dolore magna aliqua. Ut enim ad minim veniam, "
        + "quis nostrud exercitation ullamco laboris nisi ut aliquip ex "
        + "ea commodo consequat. Duis aute irure dolor in reprehenderit "
        + "in voluptate velit esse cillum dolore eu fugiat nulla pariatur."
        + " Excepteur sint occaecat cupidatat non proident, sunt in culpa"
        + " qui officia deserunt mollit anim id est laborum."
        + "Lorem ipsum dolor sit amet, "
        + "consectetur adipisicing elit, sed do eiusmod tempor incididunt"
        + " ut labore et dolore magna aliqua. Ut enim ad minim veniam, "
        + "quis nostrud exercitation ullamco laboris nisi ut aliquip ex "
        + "ea commodo consequat. Duis aute irure dolor in reprehenderit "
        + "in voluptate velit esse cillum dolore eu fugiat nulla pariatur."
        + " Excepteur sint occaecat cupidatat non proident, sunt in culpa"
        + " qui officia deserunt mollit anim id est laborum."
        + "Lorem ipsum dolor sit amet, "
        + "consectetur adipisicing elit, sed do eiusmod tempor incididunt"
        + " ut labore et dolore magna aliqua. Ut enim ad minim veniam, "
        + "quis nostrud exercitation ullamco laboris nisi ut aliquip ex "
        + "ea commodo consequat. Duis aute irure dolor in reprehenderit "
        + "in voluptate velit esse cillum dolore eu fugiat nulla pariatur."
        + " Excepteur sint occaecat cupidatat non proident, sunt in culpa"
        + " qui officia deserunt mollit anim id est laborum."
        + "consectetur adipisicing elit, sed do eiusmod tempor incididunt"
        + " ut labore et dolore magna aliqua. Ut enim ad minim veniam, "
        + "quis nostrud exercitation ullamco laboris nisi ut aliquip ex "
        + "ea commodo consequat. Duis aute irure dolor in reprehenderit "
        + "in voluptate velit esse cillum dolore eu fugiat nulla pariatur."
        + " Excepteur sint occaecat cupidatat non proident, sunt in culpa"
        + " qui officia deserunt mollit anim id est laborum."
        + "Lorem ipsum dolor sit amet, "
        + "consectetur adipisicing elit, sed do eiusmod tempor incididunt"
        + " ut labore et dolore magna aliqua. Ut enim ad minim veniam, "
        + "quis nostrud exercitation ullamco laboris nisi ut aliquip ex "
        + "ea commodo consequat. Duis aute irure dolor in reprehenderit "
        + "in voluptate velit esse cillum dolore eu fugiat nulla pariatur."
        + " Excepteur sint occaecat cupidatat non proident, sunt in culpa"
        + " qui officia deserunt mollit anim id est laborum."
        + "consectetur adipisicing elit, sed do eiusmod tempor incididunt"
        + " ut labore et dolore magna aliqua. Ut enim ad minim veniam, "
        + "quis nostrud exercitation ullamco laboris nisi ut aliquip ex "
        + "ea commodo consequat. Duis aute irure dolor in reprehenderit "
        + "in voluptate velit esse cillum dolore eu fugiat nulla pariatur."
        + " Excepteur sint occaecat cupidatat non proident, sunt in culpa"
        + " qui officia deserunt mollit anim id est laborum."
        + "Lorem ipsum dolor sit amet, "
        + "consectetur adipisicing elit, sed do eiusmod tempor incididunt"
        + " ut labore et dolore magna aliqua. Ut enim ad minim veniam, "
        + "quis nostrud exercitation ullamco laboris nisi ut aliquip ex "
        + "ea commodo consequat. Duis aute irure dolor in reprehenderit "
        + "in voluptate velit esse cillum dolore eu fugiat nulla pariatur."
        + " Excepteur sint occaecat cupidatat non proident, sunt in culpa"
        + " qui officia deserunt mollit anim id est laborum.";

    /**
     * Empty constructor.
     */
    public TextBoxTest() {
    }

    /**
     * Class constructor.
     * @param sTestName name of the test.
     * @param rTestMethod TestMethod used.
     */
    public TextBoxTest(String sTestName) {
        super(sTestName);
    }

    /**
     * Creates new test suite.You need to add a new aSuite.addTest entry for
     * any new test methods, otherwise they won't be run.
     * @return suite.
     */
    public static Test suite() {
		TestSuite suite = new TestSuite();

	    java.util.Vector methodNames;
	    java.util.Enumeration e;

	    // Add widget tests
	    methodNames = TextBoxTest.methodNames();
	    e = methodNames.elements();
	    while (e.hasMoreElements()) {
	        suite.addTest(new TextBoxTest((String)e.nextElement()));
	    }

		return suite;
	}
    
    public static java.util.Vector methodNames() {
        java.util.Vector methodNames = new java.util.Vector();
        methodNames.addElement("testTextBox");
        methodNames.addElement("testTextBoxException");
        methodNames.addElement("testSetStringException");
        methodNames.addElement("testSetString");
        methodNames.addElement("testGetString");
        methodNames.addElement("testSetStringNull");
        methodNames.addElement("testSetLongTextTB");
        methodNames.addElement("testGetCharsExceptions");
        methodNames.addElement("testGetChars");
        methodNames.addElement("testGetCharsCopyCount");
        methodNames.addElement("testSetCharsExceptions");
        methodNames.addElement("testSetChars");
        methodNames.addElement("testInsertExceptions");
        methodNames.addElement("testInsert");
        methodNames.addElement("testInsertArrayExceptions");
        methodNames.addElement("testInsertArray");
        methodNames.addElement("testDeleteExceptions");
        methodNames.addElement("testDelete");
        methodNames.addElement("testGetMaxSize");
        methodNames.addElement("testSetMaxSizeExceptions");
        methodNames.addElement("testSetMaxSize");
        methodNames.addElement("testSize");
        methodNames.addElement("testSetConstraintsExceptions");
        methodNames.addElement("testGetConstraints");
        methodNames.addElement("testNullHeaderContent");
        return methodNames;
    }
    
    protected void runTest() throws Throwable {
        if (getName().equals("testTextBox")) testTextBox();
        else if (getName().equals("testTextBoxException")) testTextBoxExceptions();
        else if (getName().equals("testSetStringException")) testSetStringExceptions();
        else if (getName().equals("testSetString")) testSetString();
        else if (getName().equals("testGetString")) testGetString();
        else if (getName().equals("testSetStringNull")) testSetStringNull();
        else if (getName().equals("testSetLongTextTB")) testSetLongTextTB();
        else if (getName().equals("testGetCharsExceptions")) testGetCharsExceptions();
        else if (getName().equals("testGetChars")) testGetChars();
        else if (getName().equals("testGetCharsCopyCount")) testGetCharsCopyCount();
        else if (getName().equals("testSetCharsExceptions")) testSetCharsExceptions();
        else if (getName().equals("testSetChars")) testSetChars();
        else if (getName().equals("testInsertExceptions")) testInsertExceptions();
        else if (getName().equals("testInsert")) testInsert();
        else if (getName().equals("testInsertArrayExceptions")) testInsertArrayExceptions();
        else if (getName().equals("testInsertArray")) testInsertArray();
        else if (getName().equals("testDeleteExceptions")) testDeleteExceptions();
        else if (getName().equals("testDelete")) testDelete();
        else if (getName().equals("testGetMaxSize")) testGetMaxSize();
        else if (getName().equals("testSetMaxSizeExceptions")) testSetMaxSizeExceptions();
        else if (getName().equals("testSetMaxSize")) testSetMaxSize();
        else if (getName().equals("testSize")) testSize();
        else if (getName().equals("testSetConstraintsExceptions")) testSetConstraintsExceptions();
        else if (getName().equals("testGetConstraints")) testGetConstraints();
        else if (getName().equals("testNullHeaderContent")) testNullHeaderContent();
        else super.runTest();
    }


    /**
     * This method tests that TextBox with correct parameters doesn't
     * throw any exception.
     */
    public void testTextBox() {
        new TextBox("correct Box", "correct text", LENGTH, TextField.ANY);

        //Numeric text box with - in constructor as text.
        /*TextBox tbObj;
        try {
            tbObj = new TextBox ("label", "-", LENGTH, TextField.NUMERIC);
            fail("exception not trhown when '-' is set as parameter in Constructor");
        }
        catch (IllegalArgumentException e) {
            //ok
        }*/
    }

    /**
     * Correct Exception throwing check.
     * This method tests that TextBox constructor throws
     * IllegalArgumentException if parameters are not valid.
     */
    public void testTextBoxExceptions() {
        /*
         * first block for testing incorrect maxSize;
         * maxSize is set to 0
         */
        try {
        	new TextBox("", "", LENGTH_INCORRECT, TextField.ANY);
            fail("IllegalArgumentException not thrown on incorrect length");
        }
        catch (IllegalArgumentException e) {
        	// Ok
        }

        /*
         * second block for testing if the length of string
         * exceeds maxSize.
         * maxSize is set to 1, but text is "too many"
         */
        try {
            new TextBox("", "too many", LENGTH_SMALL, TextField.ANY);
            fail("IllegalArgumentException not thrown on small length");
        }
        catch (IllegalArgumentException e) {
        	// Ok
        }

        /*
         * third block tests if the constraint parameter
         * is invalid.
         * constraint is set to negative
         */
        try {
        	new TextBox("", "", LENGTH, MINUS_INV_CONSTRAINT);
        	fail("IllegalArgumentException not thrown on invalid constraints (case1)");
        }
        catch (IllegalArgumentException e) {
        	// Ok
        }

        try {
        	new TextBox("", "", LENGTH, BIG_INV_CONSTRAINT);
            fail("IllegalArgumentException not thrown on invalid constraints (case2)");
        }
        catch (IllegalArgumentException e) {
        	// Ok
        }

        try {
        	new TextBox("", "", LENGTH, INV_TEXT_FLAG);
            fail("IllegalArgumentException not thrown on invalid constraints (case2)");
        }
        catch (IllegalArgumentException e) {
        	// Ok
        }
    }

    /**
	 * Correct Exception throwing check. This method tests that
	 * TextBox.setString(txt) throws IllegalArgumentException when txt is not
	 * valid input
	 */
    public void testSetStringExceptions() {
        /**
         * first block for testing if the length of string
         * exceeds maxSize.
         */

        TextBox maxSizeExceed = new TextBox("", "", LENGTH_SMALL, TextField.ANY);
        try {
            maxSizeExceed.setString("Exceeding maxSize");
            fail("IllegalArgumentException not thrown on long string");
        }
        catch (IllegalArgumentException e) {
        	// Ok
        }

        String title = "TextBox Test";
        String text = "1234567890";
        int mSize = 20;
        int constraints = TextField.NUMERIC;
        String newText = "FOO";

        TextBox tbObj = new TextBox(title, text, mSize, constraints);
        try {
            tbObj.setString(newText);
            fail("IllegalArgumentException not thrown on invalid string");
        }
        catch (IllegalArgumentException e) {
            // Ok
        }
        maxSizeExceed = null;
    }

    /**
     * This method tests that TextBox.getString()
     * return correct result.
     */
    public void testGetString() {
        TextBox getStringTextBox = new TextBox(getName(), "TestGetString", LENGTH, TextField.ANY);
        String preset = "TestGetString";
        String getString;
        getString = getStringTextBox.getString();
        assertTrue("TextBox.getString(). Expected "
                + getString + " should be equal to "
                + preset, getString.equals(preset));
        getStringTextBox = null;
    }

    /**
     * This method tests that TextBox.setString(txt)
     * works correctly.
     */
    public void testSetString() {
        TextBox testSetStringTextBox = new TextBox(getName(), "", LENGTH, TextField.ANY);
        String setString = "newly setted String";
        String getString;
        testSetStringTextBox.setString(setString);
        getString = testSetStringTextBox.getString();
        assertTrue("TextBox.setString(). Expected"
                + getString + " should be equal to "
                + setString, getString.equals(setString));
        testSetStringTextBox = null;
    }

    /**
     * This method test that TextBox.setString(null)
     * empties the content of TextBox.
     */
    public void testSetStringNull() {
        TextBox setStringNullTB = new TextBox(getName(), "some text", LENGTH, TextField.ANY);
        String getString;
        String nullString = null;
        setStringNullTB.setString(nullString);
        getString = setStringNullTB.getString();
        assertTrue("TextBox.setString(null). Expected: "
                + "TextBox.getString() return empty String"
                + "but returned " + getString, getString.equals(""));
        setStringNullTB = null;
    }

    /**
     * Tests that TextBox.setString(LONG_TEXT) working OK.
     */
    public void testSetLongTextTB() {
        String getString;
        TextBox setLongString;

        setLongString = new TextBox (getName(), LONG_TEXT, LENGTH_BIG, TextField.ANY);
        getString = setLongString.getString();
        assertTrue("TextBox.setString(LONG_TEXT). Expected"
                + " TextBox.getString() returns: LING_TEXT, "
                + "but returned: " + getString, getString.equals(LONG_TEXT));
        setLongString = null;
    }

    /**
     * Exception throwing check.
     * Tests that TextBox.getChars(char[] dataArray) throws
     * ArrayIndexOutOfBoundsException if array is too short(shorter
     * then textBoxContent, and NullPointerException if dataArray is null.
     */
    public void testGetCharsExceptions() {
        TextBox getChars = new TextBox(getName(), "get char", LENGTH, TextField.ANY);

        char[] dataArray = new char[LENGTH_SMALL];
        try {
            getChars.getChars(dataArray);
            fail("ArrayIndexOutOfBoundsException not thrown on invalid chars");
        }
        catch (ArrayIndexOutOfBoundsException e) {
            // Ok
        }

        dataArray = null;
        try {
            getChars.getChars(dataArray);
            fail("NullPointerException not thrown on invalid chars");
        }
        catch (NullPointerException e) {
            // Ok
        }
    }

    /**
     * getChars(char[] arrayData() test.
     * Tests that array elements beyond copied content
     * are left unchanged, and copied content is correct.
     */
    public void testGetChars() {
        TextBox getChars = new TextBox(getName(), "tb content",
                LENGTH, TextField.ANY);
        String mergedString  = null;
        // charData contains "some other content"
        char[] charData = {'s', 'o', 'm', 'e', ' ',
                           'o', 't', 'h', 'e', 'r', ' ',
                           'c', 'o', 'n', 't', 'e', 'n', 't', };
        getChars.getChars(charData);
        mergedString = new String (charData);
        assertTrue("TextBox.getChars(dataChars). Expected "
                + "tb content content, but got " + mergedString,
                mergedString.equals("tb content content"));
    }

    /**
     * int getChars(char[] arrayData) test.
     * Tests that getChars method returns correct
     * number of copied characters.
     */
    public void testGetCharsCopyCount () {
        TextBox getChars = new TextBox(getName(), "five5", LENGTH, TextField.ANY);
        int numberOfCopied;
        char[] charData = new char [LENGTH];
        numberOfCopied = getChars.getChars(charData);
        assertTrue("TextBox.getChars(dataChars). Expected "
                + "returns 5, but returned " + numberOfCopied,
                numberOfCopied == LENGTH_SMALL);
    }

    /**
     * TextBox.setChars() test.
     * Tests that TexBox.setChars(char[] dataArray, int offset, int lngth)
     * throws correct exceptions in case of incorrect parameters.
     */
    public void testSetCharsExceptions() {
        /*
         * Test ArrayIndexOutOfBounds Exception if
         * specified range of chars to set is incorrect
         */
        TextBox setCharsEx = new TextBox(getName(), "text", LENGTH, TextField.ANY);
        char[] charData = {'s', 'o', 'm', 'e', ' ',
                           'c', 'o', 'n', 't', 'e', 'n', 't', };
        try {
            setCharsEx.setChars(charData, START_INDEX, LENGTH);
            fail("ArrayIndexOutOfBoundsException not thrown on invalid range");
        }
        catch (ArrayIndexOutOfBoundsException e) {
            // Ok
        }

        setCharsEx = new TextBox(getName(), "data", LENGTH_SMALL, TextField.ANY);
        try {
            setCharsEx.setChars(charData, START_INDEX, charData.length
                    - LENGTH_SMALL - 1);
            fail("IllegalArgumentException not thrown on invalid start index");
        }
        catch (IllegalArgumentException e) {
            // Ok
        }
    }

    /**
     * TextBox.setChars() test.
     * Tests that TextBox.setChar inserts correct content into
     * TextBox.
     */
    public void testSetChars() {
        TextBox setChars = new TextBox(getName(), "text", LENGTH, TextField.ANY);
        String getString = null;
        char[] charData = {'s', 'o', 'm', 'e', ' ',
                           'c', 'o', 'n', 't', 'e', 'n', 't', };
        setChars.setChars(charData, START_INDEX , COPY_COUNT);
        getString = setChars.getString();
        assertTrue("TextBox.setChars(). Expected "
                + "getString() returns \"content\" but got"
                + getString, getString.equals("content"));
    }

    /**
     * TextBox.insert() test.
     * Tests that TextBox.insert(String str, int startIndex)
     * throws correct exceptions in case of incorrect parameters.
     */
    public void testInsertExceptions () {
        TextBox insertTextBox = new TextBox(getName(), "text", LENGTH_SMALL, TextField.ANY);
        String insertString = null;
        try {
            insertTextBox.insert(insertString, START_INDEX);
            fail("NullPointerException not thrown on invalid string");
        }
        catch (NullPointerException e) {
            // Ok
        }

        insertString = "some content";
        try {
            insertTextBox.insert(insertString, START_INDEX);
            fail("IllegalArgumentException not thrown on exceding size");
        }
        catch (IllegalArgumentException e) {
            // Ok
        }
    }

    /**
     * TextBox.insert(String str, int position) test.
     * Test that TextBox.insert() working correctly
     *
     */
    public void testInsert() {
        String initialContent = "some content";
        String insertContent = "insert";
        String getStringBefore;
        String getStringAfter;
        String getStringInside;
        String getStringBeforeExpected = "insertsome content";
        String getStringAfterExpected = "some contentinsert";
        String getStringInsideExpected = "some insertcontent";

        TextBox insertTextBox = new TextBox(getName(), initialContent,
                LENGTH, TextField.ANY);
        //Testing insertion inside of content of TextBox
        insertTextBox.insert(insertContent, START_INDEX);
        getStringInside = insertTextBox.getString();
        insertTextBox.setString(initialContent);
        //testing insertion before content of TextBoc
        insertTextBox.insert(insertContent, MINUS_VALUE);
        getStringBefore = insertTextBox.getString();
        insertTextBox.setString(initialContent);
        //Testing insertion after content of TextBox
        insertTextBox.insert(insertContent, LENGTH);
        getStringAfter = insertTextBox.getString();
        assertTrue("TextBox.insert(). Expected: "
                + getStringBeforeExpected + " but got "
                + getStringBefore + "__________________"
                + "TextBox.insert(). Expected: "
                + getStringAfterExpected + " but got "
                + getStringAfter + "__________________"
                + "TextBox.insert(). Expected: "
                + getStringInsideExpected + " but got "
                + getStringInside,
                (getStringBefore.equals(getStringBeforeExpected))
                && (getStringAfter.equals(getStringAfterExpected))
                && (getStringInside.equals(getStringInsideExpected)));

    }

    /**
     * TextBox.insert() test.
     * Tests that TextBox.insert(char[] charData, int offset,
     * int length, int position) throws correct exceptions
     * in case of incorrect parameters.
     */
    public void testInsertArrayExceptions() {
        /*
         * Test ArrayIndexOutOfBounds Exception if
         * specified range of chars to insert is incorrect
         */
        TextBox insertTextBoxEx = new TextBox(getName(), "some content",
                                              LENGTH, TextField.ANY);
        char[] charData = {'s', 'o', 'm', 'e', ' ',
                           'c', 'o', 'n', 't', 'e', 'n', 't', };
        try {
            insertTextBoxEx.insert(charData, MINUS_VALUE, COPY_COUNT, START_INDEX);
            fail("ArrayIndexOutOfBoundsException not thrown on invalid range");
        }
        catch (ArrayIndexOutOfBoundsException e) {
            // Ok
        }

        insertTextBoxEx = null;
        insertTextBoxEx = new TextBox(getName(), "data", LENGTH_SMALL, TextField.ANY);
        try {
            insertTextBoxEx.insert(charData, START_INDEX, COPY_COUNT, START_INDEX);
            fail("IllegalArgumentException not thrown on exceding size (case1)");
        }
        catch (IllegalArgumentException e) {
            // Ok
        }

        insertTextBoxEx = null;
        charData = null;
        insertTextBoxEx = new TextBox(getName(), "data", LENGTH, TextField.ANY);
        try {
            insertTextBoxEx.insert(charData, START_INDEX, COPY_COUNT, START_INDEX);
            fail("NullPointerException not thrown on exceding size (case2)");
        }
        catch (NullPointerException e) {
            // Ok
        }
    }

    /**
     * TextBox.insert() test.
     * Tests that TextBox.insert(char[] charData, int offset,
     * int length, int position)  working correctly.
     */
    public void testInsertArray() {
        String getStringExpected = "some contentcontent";
        String getStringActual = null;
        TextBox insertTextBox = new TextBox(getName(), "some content",
                LENGTH, TextField.ANY);
        char[] charData = {'s', 'o', 'm', 'e', ' ',
                           'c', 'o', 'n', 't', 'e', 'n', 't', };
        insertTextBox.insert(charData, START_INDEX , COPY_COUNT, START_INDEX);
        getStringActual = insertTextBox.getString();
        assertTrue("TextBox.insert(char[],int,int,int). Expected: "
                + getStringExpected + " but got " + getStringActual,
                getStringActual.equals(getStringExpected));
    }

    /**
     * TextBox.delete() test.
     * Test that TextBox.delete(int offset, int length)
     * throws correct exceptions in case of incorrect parameters.
     */
    public void testDeleteExceptions() {
        TextBox deleteEx = new TextBox(getName(), "exception", LENGTH, TextField.ANY);
        try {
            deleteEx.delete(START_INDEX, LENGTH_SMALL);
            fail("StringIndexOutOfBoundsException not thrown on invalid parameters");
        }
        catch (StringIndexOutOfBoundsException e) {
            // Ok
        }
    }

    /**
     * TextBox.delete() test.
     * Tests that TextBox.delete(int offset, int length)
     * is working correctly.
     */
    public void testDelete () {
        String getString = null;
        String getStringExpected = "some nt";
        TextBox deleteTextBox = new TextBox(getName(), "some content", LENGTH, TextField.ANY);
        deleteTextBox.delete(START_INDEX, LENGTH_SMALL);
        getString = deleteTextBox.getString();
        assertTrue("TextBox.delete(int offset, int length). Expected: "
                + getStringExpected + " but got: " + getString,
                getString.equals(getStringExpected));
    }

    /**
     * TextBox.getmaxSize() test.
     * Tests that TextBox.getMaxSize() is working correctly
     */
    public void testGetMaxSize() {
        int getMaxSize;
        TextBox getMaxSizeTB = new TextBox(getName(), "content",
                LENGTH, TextField.ANY);
        getMaxSize = getMaxSizeTB.getMaxSize();
        assertTrue("TextBox.getMaxSize(). Expected: " + LENGTH
                + " but got: " + getMaxSize, getMaxSize == LENGTH);
    }

    /**
     * TextBox.setMaxSize() test.
     * Tests that TextBox.setMaxSuze() throws correct exceptions
     * in case of incorrect input.
     */
    public void testSetMaxSizeExceptions() {
        TextBox setMaxSizeEx = new TextBox(getName(), "some content", LENGTH, TextField.ANY);
        try {
            setMaxSizeEx.setMaxSize(LENGTH_INCORRECT);
            fail("IllegalArgumentException not thrown on invalid length");
        }
        catch (IllegalArgumentException e) {
            // Ok
        }

        setMaxSizeEx.setString("someone@test.com");
        setMaxSizeEx.setConstraints(TextField.EMAILADDR);
        try {
            setMaxSizeEx.setMaxSize(LENGTH_SMALL);
            fail("IllegalArgumentException not thrown on invalid content after truncation");
        }
        catch (IllegalArgumentException e) {
            // Ok
        }
    }

    /**
     * TextBox.setMaxSize() test.
     * Test that TextBox.setMaxSize sets the correct size
     * and content of the TextBox is truncated.
     */
    public void testSetMaxSize() {
        int maxSize = 0;
        String getText = null;
        String getTextExpected = "some ";
        TextBox setMaxSize = new TextBox(getName(), "some content", LENGTH, TextField.ANY);
        maxSize = setMaxSize.setMaxSize(LENGTH_SMALL);
        // test that content is truncated
        getText = setMaxSize.getString();
        assertTrue("TextBox.setMaxSize(int maxSize). Expected "
                + "return: " + LENGTH_SMALL + " but got: " + maxSize
                + " TextBox.getText() returns: " + getTextExpected
                + " but got: " + getText, (maxSize == LENGTH_SMALL)
                && getText.equals(getTextExpected));
    }

    /**
     * TextBox.size() test.
     * Tests that TextBox.size() returns correct number of
     * characters inputed into TextBox
     */
    public void testSize() {
        int currentSize = 0;
        TextBox size = new TextBox(getName(), "some content",
                LENGTH, TextField.ANY);
        currentSize = size.size();
        assertTrue("TextBox.size(). Expected returns " + EXPECTED_SIZE
                + " but got " + currentSize, currentSize == EXPECTED_SIZE);
    }

    /**
     * TextBox.setConstraints(int aConstraints) test.
     * Tests that TextBox.setConstraints(int aConstraints) throws
     * correct exception in case of incorrect input.
     */
    public void testSetConstraintsExceptions() {
        TextBox setConstraints = new TextBox(getName(), "some content",
                LENGTH, TextField.ANY);
        try {
            setConstraints.setConstraints(BIG_INV_CONSTRAINT);
            fail("IllegalArgumentException not thrown on invalid constraints");
        }
        catch (IllegalArgumentException e) {
            // Ok
        }
    }

    /**
     * TextBox.getConstraints() test.
     * Tests that TextBox.getConstraints() works correctly.
     */
    public void testGetConstraints() {
        int actualConstraints = -1;
        TextBox getConstraintsTB = new TextBox(getName(), "some content",
                LENGTH, TextField.ANY);
        getConstraintsTB.setConstraints(TextField.EMAILADDR);
        actualConstraints = getConstraintsTB.getConstraints();
        assertTrue("TextBox.getConstraints(). Expected: returns "
                + TextField.EMAILADDR + " but got " + actualConstraints,
                actualConstraints == TextField.EMAILADDR);
    }

    /**
     * Test fix for bug 5448(Creating TextBox with null title and null text
     * causes IllegalArgumentException).
     */
    public void testNullHeaderContent() {
        TextBox nullTextBox = null;
        boolean passed = true;
        try {
            nullTextBox = new TextBox(null, null, LENGTH, TextField.ANY);
        }
        catch (Exception e) {
            passed = false;
           // spede.addToResultsList("Exception occured: " + e.getMessage());
        }
        assertTrue("TextBox bug fix. Expected no Exception ", passed);
    }
}
