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
package com.nokia.openlcdui.mt.ticker;

import junit.framework.*;

import javax.microedition.lcdui.Form;
import javax.microedition.lcdui.Ticker;

import com.nokia.openlcdui.mt.SWTTestCase;

/**
 * TEST CASE SPECIFICATION <br>
 * <br>
 * Short description of the module test:
 * <li> Collection of tests to test Ticker's functionality. <br>
 * <br>
 * Created: 2009-02-10
 */
public class TickerTest extends SWTTestCase
{

    private static final String STRING_WITH_NEWLINE_CHARACTER =
        "string with\nnewline character.";

    /**
     * Empty constructor.
     */
    public TickerTest()
    {
    }

    /**
     * Class constructor.
     * @param sTestName name of the test.
     * @param rTestMethod TestMethod used.
     */
    public TickerTest(String sTestName)
    {
        super(sTestName);
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
        methodNames = TickerTest.methodNames();
        e = methodNames.elements();
        while(e.hasMoreElements())
        {
            suite.addTest(new TickerTest((String)e.nextElement()));
        }

        return suite;
    }

    public static java.util.Vector methodNames()
    {
        java.util.Vector methodNames = new java.util.Vector();
        methodNames.addElement("testAccessors");
        methodNames.addElement("testOneTickerInManyDisplayables");
        methodNames.addElement("testManyTickersInOneDisplayable");
        return methodNames;
    }

    protected void runTest() throws Throwable
    {
        if(getName().equals("testAccessors")) testAccessors();
        else if(getName().equals("testOneTickerInManyDisplayables")) testOneTickerInManyDisplayables();
        else if(getName().equals("testManyTickersInOneDisplayable")) testManyTickersInOneDisplayable();
        else super.runTest();
    }

    /**
     * Tests basic functionality of Ticker's methods.
     */
    public void testAccessors()
    {

        // Instantiate a new Ticker with null-string and make sure an
        // exception is thrown:
        try
        {
            new Ticker(null);
            fail("No nullpointerException when instantiating"
                 + " a Ticker with null string.");
        }
        catch(NullPointerException ex)
        {
        }

        // Instantiate a new Ticker with empty string:
        Ticker ticker = new Ticker("");

        // Set null string and make sure an exception is thrown:
        try
        {
            ticker.setString(null);
            fail("No nullpointerException when calling tic"
                 + "ker.setString() with null parameter.");
        }
        catch(NullPointerException ex)
        {
        }

        // Set string with newline characters and make sure the string
        // returned by getString() is exactly same:
        ticker.setString(STRING_WITH_NEWLINE_CHARACTER);
        assertEquals(ticker.getString(), STRING_WITH_NEWLINE_CHARACTER);
    }

    /**
     * Tests to add one ticker to many displayables.
     */
    public void testOneTickerInManyDisplayables()
    {
        Form form1 = new Form("ticker test");
        Form form2 = new Form("ticker test");
        Form form3 = new Form("ticker test");

        Ticker ticker = new Ticker("ticker");

        form1.setTicker(ticker);
        form2.setTicker(ticker);
        form3.setTicker(ticker);

        assertEquals(ticker, form1.getTicker());
        assertEquals(ticker, form2.getTicker());
        assertEquals(ticker, form3.getTicker());
    }

    /**
     * Tests to add many tickers to one displayable.
     */
    public void testManyTickersInOneDisplayable()
    {
        Ticker ticker1 = new Ticker("ticker");
        Ticker ticker2 = new Ticker("ticker");
        Ticker ticker3 = new Ticker("ticker");

        Form form = new Form("ticker test");

        form.setTicker(ticker1);
        assertEquals(ticker1, form.getTicker());

        form.setTicker(ticker2);
        assertEquals(ticker2, form.getTicker());

        form.setTicker(ticker3);
        assertEquals(ticker3, form.getTicker());
    }

}

