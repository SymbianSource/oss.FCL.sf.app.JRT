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
package com.nokia.openlcdui.mt.font;

import junit.framework.*;

import javax.microedition.lcdui.*;
import com.nokia.openlcdui.mt.SWTTestCase;

/**
 * TEST CASE SPECIFICATION <br>
 * <br>
 * Short description of the module test:
 * <li>Collection of tests to test Font's functionality. <br>
 * <br>
 */
public class FontTest extends SWTTestCase
{

    private static String shortString = "Lorem ipsum";

    /**
     * Constructor.
     */
    public FontTest()
    {
    }

    /**
     * Constructor.
     *
     * @param sTestName Test name.
     * @param rTestMethod Test method.
     */
    public FontTest(String sTestName)
    {
        super(sTestName);
    }

    /**
     * To create the test suite.
     *
     * @return New TestSuite.
     */
    public static Test suite()
    {
        TestSuite suite = new TestSuite();

        java.util.Vector methodNames;
        java.util.Enumeration e;

        // Add widget tests
        methodNames = FontTest.methodNames();
        e = methodNames.elements();
        while(e.hasMoreElements())
        {
            suite.addTest(new FontTest((String)e.nextElement()));
        }

        return suite;
    }

    public static java.util.Vector methodNames()
    {
        java.util.Vector methodNames = new java.util.Vector();
        methodNames.addElement("testConstructors");
        methodNames.addElement("testAccessors");
        return methodNames;
    }

    protected void runTest() throws Throwable
    {
        if(getName().equals("testConstructors")) testConstructors();
        else if(getName().equals("testAccessors")) testAccessors();
        else super.runTest();
    }


    /**
     * Test method.
     */
    public void testConstructors()
    {
        Font.getDefaultFont();

        try
        {
            Font.getFont(-1);
            fail("1. IllegalArgumentException should be thrown");
        }
        catch(IllegalArgumentException iae)
        {
            // OK
        }

        Font.getFont(Font.FONT_STATIC_TEXT);

        Font.getFont(Font.FONT_INPUT_TEXT);

        try
        {
            Font.getFont(2);
            fail("2. IllegalArgumentException should be thrown");
        }
        catch(IllegalArgumentException iae)
        {
            // OK
        }

        try
        {
            Font.getFont(-1, 0, 0);
            fail("3. IllegalArgumentException should be thrown");
        }
        catch(IllegalArgumentException iae)
        {
            // OK
        }

        try
        {
            Font.getFont(0, -1, 0);
            fail("4. IllegalArgumentException should be thrown");
        }
        catch(IllegalArgumentException iae)
        {
            // OK
        }

        try
        {
            Font.getFont(0, 0, -1);
            fail("5. IllegalArgumentException should be thrown");
        }
        catch(IllegalArgumentException iae)
        {
            // OK
        }
    }

    /**
     * Test method.
     */
    public void testAccessors()
    {
        Font font = Font.getDefaultFont();
        assertNotNull("Default font shouldn't be null", font);

        assertTrue("Default font height shouldn't be null",
                   font.getHeight() != 0);

        int prevWidth = -1;
        for(int i = 0; i < shortString.length(); i++)
        {
            int witdh = font.substringWidth(shortString, 0, i);
            assertTrue("String width should increase width at pos " + i,
                       (witdh > prevWidth));
            prevWidth = witdh;
        }
    }

}
