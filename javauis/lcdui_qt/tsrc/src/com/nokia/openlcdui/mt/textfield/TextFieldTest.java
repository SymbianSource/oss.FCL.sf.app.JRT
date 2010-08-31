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
package com.nokia.openlcdui.mt.textfield;

import junit.framework.*;

import javax.microedition.lcdui.*;
import com.nokia.openlcdui.mt.SWTTestCase;

/**
 * TEST CASE SPECIFICATION <br>
 * <br>
 * Short description of the module test:
 * <li>Collection of tests to test TextField's API functionality. <br>
 * <br>
 *
 * @created  2008-09-23
 */
public class TextFieldTest extends SWTTestCase
{

    /**
     * Constructor.
     */
    public TextFieldTest()
    {
    }

    /**
     * Constructor.
     *
     * @param sTestName Test name.
     * @param rTestMethod Test method.
     */
    public TextFieldTest(String sTestName)
    {
        super(sTestName);
    }

    /**
     * Any pre-test setup can be done here.
     */
    protected void setUp() throws Exception
    {
    }

    /**
     * To create the test suite. You need to add a new aSuite.addTest entry for
     * any new test methods.
     *
     * @return new testsuite.
     */
    public static Test suite()
    {
        TestSuite suite = new TestSuite();

        java.util.Vector methodNames;
        java.util.Enumeration e;

        // Add widget tests
        methodNames = TextFieldTest.methodNames();
        e = methodNames.elements();
        while(e.hasMoreElements())
        {
            suite.addTest(new TextFieldTest((String)e.nextElement()));
        }

        return suite;
    }

    public static java.util.Vector methodNames()
    {
        java.util.Vector methodNames = new java.util.Vector();
        methodNames.addElement("testConstructor");
        methodNames.addElement("testAccessors");
        methodNames.addElement("testNullBodyHeader");
        methodNames.addElement("testSetNullString");
        return methodNames;
    }

    protected void runTest() throws Throwable
    {
        if(getName().equals("testConstructor")) testConstructor();
        else if(getName().equals("testAccessors")) testAccessors();
        else if(getName().equals("testNullBodyHeader")) testNullBodyHeader();
        else if(getName().equals("testSetNullString")) testSetNullString();
        else super.runTest();
    }



    /**
     * Test the TextField accessor methods.
     */
    public void testConstructor()
    {
        TextField tf;
        final int illegalConstraint = -10;
        final int smallSize = 5;

        // Constructor tests
        try
        {
            tf = new TextField("label", "text", 0, TextField.ANY);
            fail("No IllegalArgumentExceptin thrown in constructor,"
                 + " if maxSize is incorrect");
        }
        catch(IllegalArgumentException iae)
        {
            //OK.
        }
        try
        {
            tf = new TextField("label", "text", 1, illegalConstraint);
            fail("No IllegalArgumentExceptin thrown in constructor,"
                 + " if constraints are incorrect");
        }
        catch(IllegalArgumentException iae)
        {
            //OK.
        }
        /*try {
            tf = new TextField ("label", "text", normalSize, TextField.DECIMAL);
            fail("No IllegalArgumentExceptin thrown in constructor,"
                    + " if text and constraints are not matching");
        }
        catch (IllegalArgumentException iae) {
            //OK.
        }*/

        try
        {
            tf = new TextField("label", "long text", smallSize, TextField.ANY);
            fail("No IllegalArgumentExceptin thrown in constructor,"
                 + " if text is too long for specified maxSize");
        }
        catch(IllegalArgumentException iae)
        {
            //OK.
        }

    }

    public void testAccessors()
    {
        TextField tf;
        final int normalSize = 100;
        final int smallSize = 5;
        final int offset = 5;
        final int incorrectLength = 100;
        final int negativeOffset = -5;
        final int normalLength = 3;
        final int copyCount = 7;
        final int invalidConstraints = -1345;
        final int expectedSize = 12;
        try
        {
            //setText()
            tf = new TextField("label", "1234", smallSize, TextField.DECIMAL);
            tf.setString("12345678");
            fail("No IllegalArgumentExceptin thrown in setString(),"
                 + " if maxSize was exceeded");
        }
        catch(IllegalArgumentException iae)
        {
            //OK
        }
        /*
        try {
            tf = new TextField("label", "1234", smallSize, TextField.DECIMAL);
            tf.setString("text");
            fail("No IllegalArgumentExceptin thrown in setString(),"
                    + " if text and constraints are not matching");
        }
        catch (IllegalArgumentException iae) {
            //OK
        }*/
        //getChars
        char []ch = null;

        try
        {
            tf = new TextField("label" , "text", normalSize, TextField.ANY);
            ch = new char [2];
            tf.getChars(ch);
            fail("No ArrayIndexOutOfBoundsException is thrown in getChars()"
                 + " if char[] is too short for the content");
        }
        catch(ArrayIndexOutOfBoundsException aioobe)
        {
            //OK
        }
        try
        {
            ch = null;
            tf = new TextField("label" , "text", normalSize, TextField.ANY);
            tf.getChars(ch);
            fail("No NullPointerException is thrown in getChars()"
                 + " if char[] is null");
        }
        catch(NullPointerException npe)
        {
            //OK
        }
        //setChars()

        char []charData = {'s', 'o', 'm', 'e', ' ',
                           'c', 'o', 'n', 't', 'e', 'n', 't',
                          };
        try
        {
            tf = new TextField("label" , "text", normalSize, TextField.ANY);
            tf.setChars(charData, offset, incorrectLength);
            fail("No ArrayIndexOutOfBoundsException is thrown in setChars()"
                 + " if offset and length do not specify valid range");
        }
        catch(ArrayIndexOutOfBoundsException aioobe)
        {
            //OK
        }
        try
        {
            tf = new TextField("label" , "text", normalSize, TextField.ANY);
            tf.setChars(charData, negativeOffset, normalLength);
            fail("No ArrayIndexOutOfBoundsException is thrown in setChars()"
                 + " if offset and length do not specify valid range");
        }
        catch(ArrayIndexOutOfBoundsException aioobe)
        {
            //OK
        }
        /*try {
            tf = new TextField("label" , "12345678", normalSize,
                    TextField.DECIMAL);
            tf.setChars(charData, offset, normalLength);
            fail ("No IllegalArgumentException is thrown on setChars()"
                    + " if data is incorrect for specified constraints");
        }
        catch (IllegalArgumentException iae) {
            //OK
        }*/
        try
        {
            tf = new TextField("label" , "text", smallSize, TextField.ANY);
            tf.setChars(charData, offset, copyCount);
            fail("No IllegalArgumentException is thrown in setChars()"
                 + " if text exceeds current TextField's maxSize");
        }
        catch(IllegalArgumentException iae)
        {
            //OK
        }
        //insert(String, position)
        /*try {
            tf = new TextField("label", "12345", normalSize, TextField.DECIMAL);
            tf.insert("txt", 1);
            fail ("No IllegalArgumentException is thrown in insert"
                    + " if data is incorrect for specified constraints");
        }
        catch (IllegalArgumentException iae) {
            //OK
        }*/
        try
        {
            tf = new TextField("label" , "text", smallSize, TextField.ANY);
            tf.insert("more text", 1);
            fail("No IllegalArgumentException is thrown in insert()"
                 + " if text exceeds current TextField's maxSize");

        }
        catch(IllegalArgumentException iae)
        {
            //OK
        }
        try
        {
            tf = new TextField("label" , "text", smallSize, TextField.ANY);
            tf.insert(null, 1);
            fail("No NullPointerException is thrown in insert()"
                 + " if text is null");

        }
        catch(NullPointerException npe)
        {
            //OK
        }
        // insert([])
        try
        {
            tf = new TextField("label" , "text", normalSize, TextField.ANY);
            tf.insert(charData, negativeOffset, normalLength, offset);
            fail("No ArrayIndexOutOfBoundsException is thrown in insert([])"
                 + " if offset and length do not specify valid range");
        }
        catch(ArrayIndexOutOfBoundsException aioobe)
        {
            //OK
        }
        try
        {
            tf = new TextField("label", "text", smallSize, TextField.ANY);
            tf.insert(charData, offset, normalLength, offset);
            //2nd offset means position
            fail("No IllegalArgumentException is thrown in insert([])"
                 + " if text exceeds current TextField's maxSize");
        }
        catch(IllegalArgumentException iae)
        {
            //OK
        }
        try
        {
            tf = new TextField("label", "text", smallSize, TextField.ANY);
            tf.insert(null, offset, 1, offset);
            fail("No NullPointerException is thrown in insert()"
                 + " if charData is null");
        }
        catch(NullPointerException npe)
        {
            //OK
        }
        /*try {
            tf = new TextField("label", "12345", normalSize, TextField.DECIMAL);
            tf.insert(charData, offset, normalLength, offset);
            //2nd offset means position
            fail ("No IllegalArgumentException is thrown in insert([])"
                + " if data is incorrect for specified constraints");
        }
        catch (IllegalArgumentException iae) {
            //OK
        } */
        //delete()
        try
        {
            tf = new TextField("label", "some text", normalSize, TextField.ANY);
            tf.delete(offset, smallSize); //smallSize = number of chars
            fail("No StringIndexOutOfBoundsException is thrown in delete()"
                 + " if offset and length do not specify valid range");
        }
        catch(StringIndexOutOfBoundsException sioobe)
        {
            //OK
        }
        //setMaxSize()
        try
        {
            tf = new TextField("label", "some text", normalSize, TextField.ANY);
            tf.setMaxSize(0);
            fail("No IllegalArgumentException is thrown in setMaxSize()"
                 + " if newMaxSize is invalid");
        }
        catch(IllegalArgumentException iae)
        {
            //OK
        }
        //setConstraints()
        try
        {
            tf = new TextField("label", "some text", normalSize, TextField.ANY);
            tf.setConstraints(invalidConstraints);
            fail("No IllegalArgumentException is thrown in setConstraints()"
                 + " if newConstriants is invalid");
        }
        catch(IllegalArgumentException iae)
        {
            //OK
        }
        tf = new TextField("",
                           "content", normalSize, TextField.ANY);
        String setString = "TestGetString";
        tf.setString(setString);
        assertTrue("TextField setString() getString() not working correct",
                   setString.equals(tf.getString()));
        tf = null;

        tf = new TextField("header", "tb content",
                           normalSize, TextField.ANY);
        String mergedString  = null;
        // charData contains "some other content"
        char[] newCharData = {'s', 'o', 'm', 'e', ' ', 'o', 't', 'h', 'e',
                              'r', ' ', 'c', 'o', 'n', 't', 'e', 'n', 't',
                             };
        tf.getChars(newCharData);
        mergedString = new String(newCharData);
        assertTrue("TextField getChars([]) not working properly",
                   mergedString.equals("tb content content"));
        tf = null;
        tf = new TextField("getChar", "five5", normalSize, TextField.ANY);
        int numberOfCopied;
        charData = new char [normalSize];
        numberOfCopied = tf.getChars(charData);
        assertTrue("TextField.getChars(dataChars). Expected "
                   + "returns 5, but returned " + numberOfCopied,
                   numberOfCopied == smallSize);
        tf = null;

        tf = new TextField("", "text", normalSize, TextField.ANY);

        String getString = null;
        charData = null;
        char []charData2 = {'s', 'o', 'm', 'e', ' ',
                            'c', 'o', 'n', 't', 'e', 'n', 't',
                           };
        tf.setChars(charData2, offset , copyCount);
        getString = tf.getString();
        assertTrue("TextField.setChars(). Expected "
                   + "getString() returns \"content\" but got"
                   + getString, getString.equals("content"));
        tf = null;

        String initialContent = "some content";
        String insertContent = "insert";
        String getStringBefore;
        String getStringAfter;
        String getStringInside;
        String getStringBeforeExpected = "insertsome content";
        String getStringAfterExpected = "some contentinsert";
        String getStringInsideExpected = "some insertcontent";

        tf = new TextField("", initialContent,
                           normalSize, TextField.ANY);
        //Testing insertion inside of content of TextField
        tf.insert(insertContent, offset);
        getStringInside = tf.getString();
        tf.setString(initialContent);
        //testing insertion before content of TextField
        tf.insert(insertContent, negativeOffset);
        getStringBefore = tf.getString();
        tf.setString(initialContent);
        //Testing insertion after content of TextBox
        tf.insert(insertContent, normalSize);
        getStringAfter = tf.getString();
        assertTrue("TextField.insert(). Expected: "
                   + getStringBeforeExpected + " but got "
                   + getStringBefore + "__________________"
                   + "TextField.insert(). Expected: "
                   + getStringAfterExpected + " but got "
                   + getStringAfter + "__________________"
                   + "TextField.insert(). Expected: "
                   + getStringInsideExpected + " but got "
                   + getStringInside,
                   (getStringBefore.equals(getStringBeforeExpected))
                   && (getStringAfter.equals(getStringAfterExpected))
                   && (getStringInside.equals(getStringInsideExpected)));
        tf = null;

        String getStringExpected = "some contentcontent";
        String getStringActual = null;
        tf = new TextField("", "some content", normalSize, TextField.ANY);
        char[] charData3 = {'s', 'o', 'm', 'e', ' ',
                            'c', 'o', 'n', 't', 'e', 'n', 't',
                           };
        tf.insert(charData3, offset , copyCount, offset);
        getStringActual = tf.getString();
        assertTrue("TextField.insert(char[],int,int,int). Expected: "
                   + getStringExpected + " but got " + getStringActual,
                   getStringActual.equals(getStringExpected));
        tf = null;

        getString = null;
        getStringExpected = "some nt";
        tf = new TextField("delete", "some content", normalSize, TextField.ANY);
        tf.delete(offset, smallSize);
        getString = tf.getString();
        assertTrue("TextField.delete(int offset, int length). Expected: "
                   + getStringExpected + " but got: " + getString,
                   getString.equals(getStringExpected));
        tf = null;

        int getMaxSize;
        tf = new TextField("Header", "content", normalSize, TextField.ANY);
        getMaxSize = tf.getMaxSize();
        assertTrue("TextBox.getMaxSize(). Expected: " + normalSize
                   + " but got: " + getMaxSize, getMaxSize == normalSize);
        tf = null;

        int maxSize = 0;
        String getText = null;
        String getTextExpected = "some ";
        tf = new TextField("header", "some content", normalSize, TextField.ANY);
        maxSize = tf.setMaxSize(smallSize);
        //test that content is truncated
        getText = tf.getString();
        assertTrue("TextField.setMaxSize(int maxSize). Expected "
                   + "return: " + smallSize + " but got: " + maxSize
                   + " TextField.getText() returns: " + getTextExpected
                   + " but got: " + getText, (maxSize == smallSize)
                   && getText.equals(getTextExpected));
        tf = null;

        int currentSize = 0;
        tf = new TextField("header", "some content", normalSize, TextField.ANY);
        currentSize = tf.size();
        assertTrue("TextField.size(). Expected returns " + expectedSize
                   + " but got " + currentSize, currentSize == expectedSize);
        tf = null;

        int actualConstraints = -1;
        tf = new TextField("header", "some content", normalSize, TextField.ANY);
        tf.setConstraints(TextField.EMAILADDR);
        actualConstraints = tf.getConstraints();
        assertTrue("TextField.getConstraints(). Expected: returns "
                   + TextField.EMAILADDR + " but got " + actualConstraints,
                   actualConstraints == TextField.EMAILADDR);
    }

    /**
     * Test that null body and header are processed properly.
     */
    public void testNullBodyHeader()
    {
        final int maxsize = 1000;
        String header = null;
        String body = null;
        TextField tf;

        tf = new TextField("header", body, maxsize, TextField.ANY);
        tf = null;
        tf = new TextField(header, "body", maxsize, TextField.ANY);
        this.assertTrue(true);
        tf = null;

    }

    /**
     * Test that setString(null) working properly.
     */
    public void testSetNullString()
    {
        final int maxsize = 1000;
        String header = "header";
        String body = "body";
        TextField tf;

        tf = new TextField(header, body, maxsize, TextField.ANY);
        tf.setString(null);

    }

}
