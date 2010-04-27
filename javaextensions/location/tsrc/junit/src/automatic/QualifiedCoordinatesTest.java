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
package com.nokia.mj.test.lapi;

import j2meunit.framework.*;
import javax.microedition.location.*;

/**
 * <b>TEST CASE SPECIFICATION</b>
 *
 * This suite of testcases tests that: - Coordinates object can be created and
 * changed - legal parameters are accepted - illegal parameters are not accepted
 *
 */

public class QualifiedCoordinatesTest extends TestCase
{

    protected double iLat;

    protected double iLon;

    protected float iAlt;

    protected final static float[] LEGAL_ALT_VALUES = { Float.NaN,
            Float.MAX_VALUE, -Float.MAX_VALUE, Float.MIN_VALUE,
            Float.POSITIVE_INFINITY, Float.NEGATIVE_INFINITY
                                                      };

    private float iHacc;

    private float iVacc;

    private final static float[] LEGAL_ACC_VALUES = { 0, Float.NaN,
            Float.MAX_VALUE, Float.MIN_VALUE, Float.POSITIVE_INFINITY,
                                                    };

    public QualifiedCoordinatesTest()
    {
    }

    public QualifiedCoordinatesTest(String sTestName, TestMethod rTestMethod)
    {
        super(sTestName, rTestMethod);
    }

    /***************************************************************************
     * Creates the test suite. You need to add a new aSuite.addTest antry for
     * any new test methods, otherwise they won't be run.
     */
    public Test suite()
    {
        TestSuite aSuite = new TestSuite();

        aSuite.addTest(new QualifiedCoordinatesTest("testGoodArguments",
                       new TestMethod()
        {
            public void run(TestCase tc)
            {
                ((QualifiedCoordinatesTest) tc).testGoodArguments();
            }
        }));

        aSuite.addTest(new QualifiedCoordinatesTest("testArgumentsRange",
                       new TestMethod()
        {
            public void run(TestCase tc)
            {
                ((QualifiedCoordinatesTest) tc).testArgumentsRange();
            }
        }));

        return aSuite;

    }

    private void assertContinue(String aReason, boolean aCond)
    {
        if (!aCond)
            assertTrue(aReason, false);
    }

    public void testGoodArguments()
    {

        float iHacc = 100.0f;
        float iVacc = 150.0f;

        double lat = 57.111111d;
        double lon = 17.111111d;
        float alt = 31.111111f;

        // Test legal values for hacc
        for (int i = 0; i < LEGAL_ACC_VALUES.length; ++i)
        {
            assertGood(lat, lon, alt, LEGAL_ACC_VALUES[i], iVacc);
        }

        // Test legal values for vacc
        for (int i = 0; i < LEGAL_ACC_VALUES.length; ++i)
        {
            assertGood(lat, lon, alt, iHacc, LEGAL_ACC_VALUES[i]);
        }

        assertTrue("", true);
    }

    // Tests range of allowed values.
    void testArgumentsRange()
    {

        // Test maximum allowed values
        double lat = 90.0d;
        double lon = 179.99999999999d;
        float alt = Float.POSITIVE_INFINITY;
        assertGood(lat, lon, alt);

        // Test minimum allowed values
        lat = -90.0d;
        lon = -180.0d;
        alt = Float.NEGATIVE_INFINITY;
        assertGood(lat, lon, alt);

        // Test out of range positive values
        lat = 0;
        lon = 0;
        double badLat = 90.0000000000001d;
        double badLon = 180.0d;

        assertBad(badLat, lon, alt);
        assertBad(lat, badLon, alt);
        assertBad(badLat, badLon, alt);

        // Test out of range negative values
        badLat = -90.000000000001d;
        badLon = -180.00000000001d;

        assertBad(badLat, lon, alt);
        assertBad(lat, badLon, alt);
        assertBad(badLat, badLon, alt);

        assertTrue("", true);
    }

    // Inherited from CoordinatesTest
    protected Coordinates newCoordinates()
    {
        return new QualifiedCoordinates(iLat, iLon, iAlt, iHacc, iVacc);
    }

    // Inherited from CoordinatesTest
    protected Coordinates newZeroCoordinates()
    {
        return new QualifiedCoordinates(0, 0, 0, 0, 0);
    }

    // ------------------------ Helper methods -----------------------

    private void assertGood(double aLat, double aLon, float aAlt, float aHacc,
                            float aVacc)
    {
        iHacc = aHacc;
        iVacc = aVacc;
        assertGood(aLat, aLon, aAlt);

        QualifiedCoordinates coords = (QualifiedCoordinates) newCoordinates();

        assertContinue(
            "Coordinates values not equal to constructor input",
            (Float.isNaN(aHacc) ^ coords.getHorizontalAccuracy() == aHacc)
            && (Float.isNaN(aVacc) ^ coords.getVerticalAccuracy() == aVacc));

        // Test setters
        coords = (QualifiedCoordinates) newZeroCoordinates();

        try
        {
            coords.setHorizontalAccuracy(aHacc);
        }
        catch (IllegalArgumentException iae)
        {
            assertContinue("setHorizontalAccuracy(" + aHacc + ") failed", false);
        }

        try
        {
            coords.setVerticalAccuracy(aVacc);
        }
        catch (IllegalArgumentException iae)
        {
            assertContinue("setVerticalAccuracy(" + aVacc + ") failed", false);
        }
    }

    private void assertGood(double aLat, double aLon, float aAlt)
    {
        // Test constructor
        iLat = aLat;
        iLon = aLon;
        iAlt = aAlt;
        Coordinates coords = newCoordinates();

        assertContinue("Coordinates values not equal to constructor input ("
                       + aLat + "," + aLon + "," + aAlt + ")",
                       coords.getLatitude() == aLat && coords.getLongitude() == aLon
                       && (Float.isNaN(aAlt) ^ coords.getAltitude() == aAlt));

        // Test setters
        coords = newZeroCoordinates();

        try
        {
            coords.setLatitude(aLat);
        }
        catch (IllegalArgumentException iae)
        {
            assertContinue("setLatitude(" + aLat + ") failed", false);
        }

        try
        {
            coords.setLongitude(aLon);
        }
        catch (IllegalArgumentException iae)
        {
            assertContinue("setLongitude(" + aLon + ") failed", false);
        }

        try
        {
            coords.setAltitude(aAlt);
        }
        catch (IllegalArgumentException iae)
        {
            assertContinue("setAtitude(" + aAlt + ") failed", false);
        }

        assertContinue("Coordinates values not equal to set values (" + aLat
                       + "," + aLon + "," + aAlt + ")", coords.getLatitude() == aLat
                       && coords.getLongitude() == aLon
                       && (Float.isNaN(aAlt) ^ coords.getAltitude() == aAlt));
    }

    protected void assertBad(double aLat, double aLon, float aAlt)
    {
        // Test constructor
        try
        {
            iLat = aLat;
            iLon = aLon;
            iAlt = aAlt;
            Coordinates coords = newCoordinates();

            assertContinue("No exception thrown for constructor (" + aLat
                           + ", " + aLon + ", " + aAlt + ")", false);
        }
        catch (IllegalArgumentException iae)
        {
            // Exception was thrown correctly
        }

        // Test setters
        Coordinates coords = newZeroCoordinates();

        try
        {
            coords.setLatitude(aLat);
            coords.setLongitude(aLon);
            assertContinue("IllegalArgumentException not thrown for setter",
                           false);
        }
        catch (IllegalArgumentException iae)
        {
            // Exception was thrown correctly
        }

        // setAltitude should never throw exception
        coords.setAltitude(aAlt);
    }
}
