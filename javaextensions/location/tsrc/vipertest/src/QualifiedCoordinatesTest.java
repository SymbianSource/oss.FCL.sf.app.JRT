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
import javax.microedition.location.*;

/**
 * This suite of testcases tests that: - Coordinates object can be created and
 * changed - legal parameters are accepted - illegal parameters are not accepted
 */
public class QualifiedCoordinatesTest extends CoordinatesTest
{
    private float iHacc;

    private float iVacc;

    private final static float[] LEGAL_ACC_VALUES = { 0, Float.NaN,
            Float.MAX_VALUE, Float.MIN_VALUE, Float.POSITIVE_INFINITY,
                                                    };

    public QualifiedCoordinatesTest()
    {
        super("QualifiedCoordinatesTest");
    }

    protected void runTest() throws Throwable
    {
        testGoodArguments();
        testBadArguments();
        testArgumentsRange();
    }

    void testGoodArguments()
    {
        setCurrentTest("testGoodArguments()");

        float iHacc = 100.0f;
        float iVacc = 150.0f;

        super.testGoodArguments();

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
    }

    void testBadArguments()
    {
        setCurrentTest("testBadArguments()");

        super.testBadArguments();
    }

    // Tests range of allowed values.
    void testArgumentsRange()
    {
        setCurrentTest("testArgumentsRange()");

        super.testArgumentsRange();

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

        assertTrue(
            (Float.isNaN(aHacc) ^ coords.getHorizontalAccuracy() == aHacc)
            && (Float.isNaN(aVacc) ^ coords.getVerticalAccuracy() == aVacc),
            "Coordinates values not equal to constructor input");

        // Test setters
        coords = (QualifiedCoordinates) newZeroCoordinates();

        try
        {
            coords.setHorizontalAccuracy(aHacc);
        }
        catch (IllegalArgumentException iae)
        {
            assertTrue(false, "setHorizontalAccuracy(" + aHacc + ") failed");
        }

        try
        {
            coords.setVerticalAccuracy(aVacc);
        }
        catch (IllegalArgumentException iae)
        {
            assertTrue(false, "setVerticalAccuracy(" + aVacc + ") failed");
        }
    }
}
