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

/*
 * This suite of testcases tests that:
 * - Coordinates object can be created and changed
 * - legal parameters are accepted
 * - illegal parameters are not accepted
 */
public class CoordinatesTest extends ViperUnitTest
{
    protected double iLat;

    protected double iLon;

    protected float iAlt;

    protected final static float[] LEGAL_ALT_VALUES = { Float.NaN,
            Float.MAX_VALUE, -Float.MAX_VALUE, Float.MIN_VALUE,
            Float.POSITIVE_INFINITY, Float.NEGATIVE_INFINITY
                                                      };

    public CoordinatesTest()
    {
        super("CoordinatesTest");
    }

    protected CoordinatesTest(String s)
    {
        super(s);
    }

    protected void runTest() throws Throwable
    {
        testGoodArguments();
        testBadArguments();
        testArgumentsRange();

        testLocationMathNormal();
        testLocationMathReverse();
        testLocationMathShortDistance();
        testLocationMathPolar();
        testLocationMathSameCoords();
        testLocationMathNullCoord();

        testConvertFromStringBadArguments();
        testConvertFromString_DD_MM();
        testConvertFromString_DD_MM_SS();

        testConvertToStringBadArguments();
        testConvertToString_DD_MM();
        testConvertToString_DD_MM_SS();

        testLocationMathThreadSafe();
    }

    void testGoodArguments()
    {
        setCurrentTest("testGoodArguments()");

        // Test some values in allowed range
        double lat = 57.111111d;
        double lon = 17.111111d;
        float alt = 31.111111f;

        // tests -lat, -lon and -alt too
        assertReallyGood(lat, lon, alt);

        // Test some legal values for alt
        for (int i = 0; i < LEGAL_ALT_VALUES.length; ++i)
        {
            assertGood(lat, lon, LEGAL_ALT_VALUES[i]);
        }
    }

    void testBadArguments()
    {
        setCurrentTest("testBadArguments()");

        double lat = 57.111111d;
        double lon = 17.111111d;
        float alt = 31.111111f;

        // Test that NaN is illegal for lat, lon
        assertBad(Double.NaN, lon, alt);
        assertBad(lat, Double.NaN, alt);
        assertBad(Double.NaN, Double.NaN, alt);

        // Test some more bad values for lat
        assertBad(Double.MAX_VALUE, lon, alt);
        assertBad(Double.POSITIVE_INFINITY, lon, alt);
        assertBad(Double.NEGATIVE_INFINITY, lon, alt);

        // Test some more bad values for lon
        assertBad(lat, Double.MAX_VALUE, alt);
        assertBad(lat, Double.POSITIVE_INFINITY, alt);
        assertBad(lat, Double.NEGATIVE_INFINITY, alt);
    }

    // Tests range of allowed values.
    void testArgumentsRange()
    {
        setCurrentTest("testArgumentsRange()");

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

    // Tests that a 'normal' calculation computes correct results.
    void testLocationMathNormal() throws Exception
    {
        setCurrentTest("testLocationMathNormal()");

        // Define the expected results
        float expectedDistance = 407342.9950f; // from www.fai.org
        float expectedBearing = 33.2208175f; // from www.fai.org

        checkDistanceAndAzimuth(new Coordinates(57.0, 17.0, -1000.0f),
                                new Coordinates(60.0, 21.0, 2000.0f), expectedDistance,
                                expectedBearing);

        // Define the expected results
        expectedDistance = 6500256.303349322f; // from www.fai.org
        expectedBearing = 200.08970684975316f; // from www.fai.org

        checkDistanceAndAzimuth(new Coordinates(57.0, 17.0, -1000.0f),
                                new Coordinates(0.0, 0.0, 2000.0f), expectedDistance,
                                expectedBearing);

    }

    // Tests that the reverse of the 'normal' calculation computes correct
    void testLocationMathReverse() throws Exception
    {
        setCurrentTest("testLocationMathReverse()");

        // Create two Coordinates object with 'normal' parameters and
        // check that the computed distance and bearing are correct
        Coordinates fromCoords = new Coordinates(60.0, 21.0, 2000.0f);
        Coordinates toCoords = new Coordinates(57.0, 17.0, -1000.0f);

        // Define the expected results
        float expectedDistance = (float) 407342.9950; // from www.fai.org
        float expectedBearing = (float) 216.63292698852845; // from www.fai.org

        checkDistanceAndAzimuth(fromCoords, toCoords, expectedDistance,
                                expectedBearing);
    }

    // Tests that a 'normal' calculation computes correct results.
    void testLocationMathShortDistance() throws Exception
    {
        setCurrentTest("testLocationMathNormal2()");

        // Define the expected results
        float expectedDistance = 789.11491522f; // from www.fai.org
        float expectedBearing = 44.96957375f; // from www.fai.org

        checkDistanceAndAzimuth(new Coordinates(57.0, 17.0, -1000.0f),
                                new Coordinates(57.005013, 17.009178, 2000.0f),
                                expectedDistance, expectedBearing);

        checkDistanceAndAzimuth(new Coordinates(0, 0, 0.0f), new Coordinates(0,
                                0.00001, 0.0f), 1.11319f, 90.0f);
    }

    // Tests that polar values give correct results.
    void testLocationMathPolar() throws Exception
    {
        setCurrentTest("testLocationMathPolar()");

        Coordinates northPole = new Coordinates(90.0, 0.0, 0.0f);
        Coordinates southPole = new Coordinates(-90.0, 0.0, 0.0f);

        // Define the expected results
        float expectedDistance = 20003931.458622963f; // from www.fai.org

        checkDistanceAndAzimuth(northPole, southPole, expectedDistance, 180.0f);
        checkDistanceAndAzimuth(southPole, northPole, expectedDistance, 0.0f);
    }

    // Tests that same coordinates doesn't cause exception.
    void testLocationMathSameCoords() throws Exception
    {
        setCurrentTest("testLocationMathSameCoords()");

        // Expected results
        float expectedDistance = 0.0f;
        float expectedBearing = 0.0f;

        checkDistanceAndAzimuth(new Coordinates(57.0, 17.0, 0.0f),
                                new Coordinates(57.0, 17.0, 0.0f), expectedDistance,
                                expectedBearing);

        // Altitude should not be used in distance and azimuth calculation
        checkDistanceAndAzimuth(new Coordinates(57.0, 17.0, 4000.0f),
                                new Coordinates(57.0, 17.0, -1000.0f), expectedDistance,
                                expectedBearing);

        // North pole
        checkDistanceAndAzimuth(new Coordinates(90.0, 10.0, 0.0f),
                                new Coordinates(90.0, 10.0, 0.0f), expectedDistance,
                                expectedBearing);

        // South pole
        checkDistanceAndAzimuth(new Coordinates(-90.0, 10.0, 0.0f),
                                new Coordinates(-90.0, 10.0, 0.0f), expectedDistance,
                                expectedBearing);
    }

    // Tests that a null coordinate causes exception.
    void testLocationMathNullCoord()
    {
        setCurrentTest("testLocationMathNullCoord()");

        Coordinates fromCoords = new Coordinates(57.0, 17.0, -1000.0f);
        try
        {
            float distance = fromCoords.distance(null);
            assertTrue(false, "NullPointerException was never thrown");
        }
        catch (NullPointerException npe)
        {
            // Exception thrown correctly
            assertTrue(npe.getMessage() == null, "Message not allowed for "
                       + npe);
        }

        try
        {
            float azimuth = fromCoords.azimuthTo(null);
            assertTrue(false, "NullPointerException was never thrown");
        }
        catch (NullPointerException npe)
        {
            // Exception thrown correctly
            assertTrue(npe.getMessage() == null, "Message not allowed for "
                       + npe);
        }
    }

    // Tests that the convert() function works.
    void testConvertFromStringBadArguments()
    {
        setCurrentTest("testConvertFromStringBadArguments()");
        try
        {
            double result = Coordinates.convert(null);
            assertTrue(false, "NullPointerException was never thrown");
        }
        catch (NullPointerException npe)
        {
            // Exception thrown correctly
            assertTrue(npe.getMessage() == null, "Message not allowed for "
                       + npe);
        }

        // Illegal values
        assertConvertBad("");
        assertConvertBad("44.44");
        assertConvertBad("1234");
        assertConvertBad("123A");
        assertConvertBad("convertThis!");

        // DD:MM
        assertConvertBad(":"); // empty
        assertConvertBad("a:11"); // Degrees illegal value
        assertConvertBad("1#:11"); // Degrees illegal value
        assertConvertBad("a1:11"); // Degrees illegal value
        assertConvertBad("+90:22"); // Degrees illegal value
        assertConvertBad("90@:22"); // Degrees illegal value
        assertConvertBad("1?2:33"); // Degrees illegal value
        assertConvertBad("-x12:44"); // Degrees illegal value
        assertConvertBad("01:10"); // starts with 0
        assertConvertBad("011:11"); // starts with 0
        assertConvertBad("-09:12"); // starts with -0
        assertConvertBad("-099:19"); // starts with -0
        assertConvertBad("180:00"); // >= 180
        assertConvertBad("-181:01"); // < -180
        assertConvertBad("-180:59.99999"); // < -180
        assertConvertBad("60:0"); // Minutes < 10 does not start with 0
        assertConvertBad("-160:1"); // Minutes < 10 does not start with 0
        assertConvertBad("70:9.9"); // Minutes < 10 does not start with 0
        assertConvertBad("20:300"); // Minutes > 59
        assertConvertBad("-10:60"); // Minutes > 59
        assertConvertBad("80:-1"); // Minutes < 0
        assertConvertBad("90:+2"); // Minutes illegal value
        assertConvertBad("90:?2"); // Minutes illegal value
        assertConvertBad("80:33+1"); // Minutes illegal value
        assertConvertBad("90:44;5"); // Minutes illegal value
        assertConvertBad("-70:12.123456"); // Minutes too many decimals

        // DD:MM:SS
        assertConvertBad("::"); // empty
        assertConvertBad("b:11:11"); // Degrees illegal value
        assertConvertBad("1$:11:22"); // Degrees illegal value
        assertConvertBad("c1:11:33"); // Degrees illegal value
        assertConvertBad("*90:22:44"); // Degrees illegal value
        assertConvertBad("90!:22:55"); // Degrees illegal value
        assertConvertBad("1-2:33:01"); // Degrees illegal value
        assertConvertBad("-z12:44:02"); // Degrees illegal value
        assertConvertBad(":10:20"); // starts with :
        assertConvertBad("01:10:20"); // starts with 0
        assertConvertBad("010:10:21"); // starts with 0
        assertConvertBad("-09:10:22"); // starts with -0
        assertConvertBad("-090:10:29"); // starts with -0
        assertConvertBad("180:00:00"); // >= 180
        assertConvertBad("280:59:59"); // >= 180
        assertConvertBad("-181:00:00"); // < -180
        assertConvertBad("-180:01:00"); // < -180
        assertConvertBad("-180:00:00.001"); // < -180
        assertConvertBad("12:3:40"); // Minutes < 10 does not start with 0
        assertConvertBad("56:07.8:09"); // Minutes has decimals
        assertConvertBad("31:17.33:12"); // Minutes has decimals
        assertConvertBad("100:60:10"); // Minutes > 59
        assertConvertBad("20:300:21"); // Minutes > 59
        assertConvertBad("-10:60:01"); // Minutes > 59
        assertConvertBad("80:-1:11"); // Minutes illegal value
        assertConvertBad("90:?2:22"); // Minutes illegal value
        assertConvertBad("80:3+:33"); // Minutes illegal value
        assertConvertBad("90:4;:44"); // Minutes illegal value
        assertConvertBad("60:10:0"); // Seconds < 10 does not start with 0
        assertConvertBad("59:59:1"); // Seconds < 10 does not start with 0
        assertConvertBad("69:49:9.999"); // Seconds < 10 does not start with 0
        assertConvertBad("89:29:60"); // Seconds > 59.999
        assertConvertBad("-90:39:60.000"); // Seconds > 59.999
        assertConvertBad("99:19:100"); // Seconds > 59.999
        assertConvertBad("10:10:-1"); // Seconds illegal value
        assertConvertBad("20:20:?2"); // Seconds illegal value
        assertConvertBad("-123:45.x2"); // Seconds illegal value
        assertConvertBad("30:30:3+.3"); // Seconds illegal value
        assertConvertBad("40:40:4;.4"); // Seconds illegal value
        assertConvertBad("-80:01:31..97"); // Seconds illegal value
        assertConvertBad("9:09:12.1234"); // Seconds too many decimals
        assertConvertBad("-70:30:12.3456"); // Seconds too many decimals
    }

    // Tests that the convert() function works.
    void testConvertFromString_DD_MM() throws Exception
    {
        setCurrentTest("testConvertFromString_DD_MM()");

        // All legal values that should be equal to zero
        String[] zeroValues = { "0:00", "0:00.0", "0:00.00", "0:00.000",
                                "0:00.0000", "0:00.00000", "-0:00", "-0:00.0", "-0:00.00",
                                "-0:00.000", "-0:00.0000", "-0:00.00000"
                              };

        for (int i = 0; i < zeroValues.length; ++i)
        {
            double zero = Coordinates.convert(zeroValues[i]);
            assertTrue(zero == 0d, "Conversion failed for " + zeroValues[i]
                       + ": " + zero + " != 0");
        }

        // Define the input values
        String[] strings = { "-0:00.06", "-0:30.0", "-0:00.5", "-59:40.4",
                             "-0:06.666", "0:39.996", "179:59.9994", "61:30.6"
                           };

        // Define the expected results
        double[] expecteds = { -0.0010d, -0.5d, -0.00833d, -59.6733d, -0.1111d,
                               0.6666d, 179.99999d, 61.51d
                             };

        // Define the tolerance
        double tol = 0.001d;

        for (int i = 0; i < strings.length; ++i)
        {
            double result = Coordinates.convert(strings[i]);
            assertTrue(Math.abs(expecteds[i] - result) < tol,
                       "Conversion failed for " + strings[i] + ": " + result
                       + " != " + expecteds[i]);
        }
    }

    // Tests that the convert() function works.
    void testConvertFromString_DD_MM_SS() throws Exception
    {
        setCurrentTest("testConvertFromString_DD_MM_SS()");

        // All legal values that should be equal to zero
        String[] zeroValues = { "0:00:00", "0:00:00.0", "0:00:00.00",
                                "0:00:00.000", "-0:00:00", "-0:00:00.0", "-0:00:00.00",
                                "-0:00:00.000"
                              };

        for (int i = 0; i < zeroValues.length; ++i)
        {
            double zero = Coordinates.convert(zeroValues[i]);
            assertTrue(zero == 0d, "Conversion failed for " + zeroValues[i]
                       + ": " + zero + " != 0");
        }

        // Define the input values
        String[] strings = { "-0:30:00.0", "-59:40", "-0:06:39.96",
                             "0:39:59.76", "179:59:59.964", "61:30:36"
                           };

        // Define the expected results
        double[] expecteds = { -0.5d, -59.6667d, -0.1111d, 0.6666d, 179.99999d,
                               61.51d
                             };

        // Define the tolerance
        double tol = 0.0001d;

        for (int i = 0; i < strings.length; i++)
        {
            double result = Coordinates.convert(strings[i]);
            assertTrue(Math.abs(expecteds[i] - result) < tol,
                       "Conversion failed for " + strings[i] + ": " + result
                       + " != " + expecteds[i]);
        }
    }

    // Tests that the convert() function works.
    void testConvertToStringBadArguments()
    {
        setCurrentTest("testConvertToStringBadArguments()");

        double[] badCoords = { Double.NaN, Double.MAX_VALUE, -Double.MAX_VALUE,
                               Double.POSITIVE_INFINITY, Double.NEGATIVE_INFINITY, 180.0d,
                               -180.0001d, 300.99999d
                             };

        for (int i = 0; i < badCoords.length; ++i)
        {
            assertConvertBad(badCoords[i], Coordinates.DD_MM);
            assertConvertBad(badCoords[i], Coordinates.DD_MM_SS);
        }
    }

    // Tests that the convert() function works.
    void testConvertToString_DD_MM()
    {
        setCurrentTest("testConvertToString_DD_MM()");

        double[] coords = { 61.51d, 57.0d, 0.6666d, -0.1111d, 179.99999d,
                            30.1234567d, 1.333333d, -5.1d, -97.99d, -180.0d, 0.01d, 0.0d,
                            1.000013d, 80.133602d, 1.666666666666666e-7d, 10.9833333d,
                            0.999999916666667d
                          };

        String[] expected = { "61:30.6", "57:00.0", "0:39.996", "-0:06.666",
                              "179:59.9994", "30:07.4074", "1:19.99998", "-5:06.0",
                              "-97:59.4", "-180:00.0", "0:00.6", "0:00.0", "1:00.00078",
                              "80:08.01612", "0:00.00001", "10:59.0", "1:00.0"
                            };

        for (int i = 0; i < coords.length; i++)
        {
            String result = Coordinates.convert(coords[i], Coordinates.DD_MM);
            assertTrue(expected[i].equals(result), "Conversion failed for "
                       + coords[i] + " (" + result + " != " + expected[i] + ")");
        }
    }

    // Tests that the convert() function works.
    void testConvertToString_DD_MM_SS()
    {
        setCurrentTest("testConvertToString_DD_MM_SS()");

        double[] coords = { 61.51d, 57.0d, 0.6666d, -0.1111d, 179.99999d,
                            30.1234567d, 1.333333d, -5.1d, -97.99d, -180.0d,
                            7.000277777777778d, 0.01d, 0.0d, -179.000002d, -16.4d,
                            2.777777777777778e-7d, 7.001111d, -8.002222d,
                            0.999999916666667d
                          };

        String[] expected = { "61:30:36.0", "57:00:00.0", "0:39:59.76",
                              "-0:06:39.96", "179:59:59.964", "30:07:24.444", "1:19:59.999",
                              "-5:06:00.0", "-97:59:24.0", "-180:00:00.0", "7:00:01.0",
                              "0:00:36.0", "0:00:00.0", "-179:00:00.007", "-16:24:00.0",
                              "0:00:00.001", "7:00:04.0", "-8:00:07.999", "1:00:00.0"
                            };

        for (int i = 0; i < coords.length; i++)
        {
            String result = Coordinates
                            .convert(coords[i], Coordinates.DD_MM_SS);
            assertTrue(expected[i].equals(result), "Conversion failed for "
                       + coords[i] + " (" + result + " != " + expected[i] + ")");
        }
    }

    void testLocationMathThreadSafe() throws InterruptedException
    {
        setCurrentTest("testLocationMathThreadSafe()");
        ThreadSafeTester t2 = new ThreadSafeTester();

        Coordinates from = new Coordinates(57.0f, 17.0f, 0f);
        Coordinates to = new Coordinates(60.0f, 21.0f, 0f);

        t2.start();

        for (int i = 0; i < 100; i++)
        {
            float d = from.distance(to);
            float a = from.azimuthTo(to);
            checkDistanceAndAzimuth(from, to, d, a);
        }

        t2.join();
        assertTrue(t2.iSuccess, "Thread t2 produced the wrong result");
    }

    private class ThreadSafeTester extends Thread
    {
        boolean iSuccess = false;

        public void run()
        {
            Coordinates from = new Coordinates(57.0f, 17.0f, 0f);
            Coordinates to = new Coordinates(56.0f, 17.0f, 0f);
            for (int i = 0; i < 100; i++)
            {
                float d = from.distance(to);
                float a = from.azimuthTo(to);
                checkDistanceAndAzimuth(from, to, d, a);
            }
            iSuccess = true;
        }
    }

    // Also used in QualifiedCoordinatesTest
    protected Coordinates newCoordinates()
    {
        return new Coordinates(iLat, iLon, iAlt);
    }

    // Also used in QualifiedCoordinatesTest
    protected Coordinates newZeroCoordinates()
    {
        return new Coordinates(0, 0, 0);
    }

    //------------------------ Helper methods -----------------------

    private void assertReallyGood(double lat, double lon, float alt)
    {
        assertGood(lat, lon, alt);
        assertGood(-lat, lon, alt);
        assertGood(lat, -lon, alt);
        assertGood(lat, lon, -alt);
        assertGood(-lat, lon, -alt);
        assertGood(lat, -lon, -alt);
        assertGood(-lat, -lon, -alt);
    }

    protected void assertGood(double aLat, double aLon, float aAlt)
    {
        // Test constructor
        iLat = aLat;
        iLon = aLon;
        iAlt = aAlt;
        Coordinates coords = newCoordinates();

        assertTrue(coords.getLatitude() == aLat
                   && coords.getLongitude() == aLon
                   && (Float.isNaN(aAlt) ^ coords.getAltitude() == aAlt),
                   "Coordinates values not equal to constructor input (" + aLat
                   + "," + aLon + "," + aAlt + ")");

        // Test setters
        coords = newZeroCoordinates();

        try
        {
            coords.setLatitude(aLat);
        }
        catch (IllegalArgumentException iae)
        {
            assertTrue(false, "setLatitude(" + aLat + ") failed");
        }

        try
        {
            coords.setLongitude(aLon);
        }
        catch (IllegalArgumentException iae)
        {
            assertTrue(false, "setLongitude(" + aLon + ") failed");
        }

        try
        {
            coords.setAltitude(aAlt);
        }
        catch (IllegalArgumentException iae)
        {
            assertTrue(false, "setAtitude(" + aAlt + ") failed");
        }

        assertTrue(coords.getLatitude() == aLat
                   && coords.getLongitude() == aLon
                   && (Float.isNaN(aAlt) ^ coords.getAltitude() == aAlt),
                   "Coordinates values not equal to set values (" + aLat + ","
                   + aLon + "," + aAlt + ")");
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

            assertTrue(false, "No exception thrown for constructor (" + aLat
                       + ", " + aLon + ", " + aAlt + ")");
        }
        catch (IllegalArgumentException iae)
        {
            // Exception was thrown correctly
            assertTrue(iae.getMessage() == null, "Message not allowed for "
                       + iae);
        }

        // Test setters
        Coordinates coords = newZeroCoordinates();

        try
        {
            coords.setLatitude(aLat);
            coords.setLongitude(aLon);
            assertTrue(false, "IllegalArgumentException not thrown for setter");
        }
        catch (IllegalArgumentException iae)
        {
            // Exception was thrown correctly
            assertTrue(iae.getMessage() == null, "Message not allowed for "
                       + iae);
        }

        // setAltitude should never throw exception
        coords.setAltitude(aAlt);
    }

    private void checkDistanceAndAzimuth(Coordinates aFrom, Coordinates aTo,
                                         float aExpectedDistance, float aExpectedAzimuth)
    {
        // Define the tolerance
        float distTol = 0.0035f; // Relative error
        float azTol = 1f; // Absolute error

        float distance = aFrom.distance(aTo);
        if (aExpectedDistance != 0.0f)
        {
            float distErr = Math.abs((distance - aExpectedDistance)
                                     / aExpectedDistance);

            assertTrue(distErr <= distTol, "Computed distance " + distance
                       + " != " + aExpectedDistance);
        }
        else
        {
            assertTrue(distance == 0.0f, "Computed distance " + distance
                       + " != 0.0");
        }

        float azimuth = aFrom.azimuthTo(aTo);
        if (aExpectedAzimuth != 0.0f)
        {
            assertTrue(Math.abs(azimuth - aExpectedAzimuth) <= azTol,
                       "Computed bearing " + azimuth + " != " + aExpectedAzimuth);
        }
        else
        {
            assertTrue(azimuth == 0.0f, "Computed bearing " + azimuth
                       + " != 0.0");
        }
    }

    private void assertConvertBad(String aString)
    {
        try
        {
            double result = Coordinates.convert(aString);
            assertTrue(false, "No exception thrown for convert(" + aString
                       + ")");
        }
        catch (IllegalArgumentException iae)
        {
            // Exception was thrown correctly
            assertTrue(iae.getMessage() == null, "Message not allowed for "
                       + iae);
        }
    }

    private void assertConvertBad(double aCoord, int aFormat)
    {
        try
        {
            String result = Coordinates.convert(aCoord, aFormat);
            assertTrue(false, "IllegalArgumentException was never thrown");
        }
        catch (IllegalArgumentException iae)
        {
            // Exception thrown correctly
            assertTrue(iae.getMessage() == null, "Message not allowed for "
                       + iae);
        }
    }
}
