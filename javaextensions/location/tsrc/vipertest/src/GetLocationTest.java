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

class GetLocationTest extends ViperUnitTest
{
    private Location iLoc;

    GetLocationTest()
    {
        super("GetLocationTest");
    }

    protected void runTest() throws Throwable
    {
        testBadArguments();
        testGetLocation();
        testAltitudeData();
        testSpeedData();
        testAddressInfo();
        testGetLocationTimeout();
        testReset();
        testGetLatest();
        testGetLocationOutOfService();
    }

    void testBadArguments() throws Exception
    {
        setCurrentTest("testBadArguments()");
        providerSetUp(null);
        assertTrue(iLP != null, "LocationProvider is null");

        assertBad(0);
        assertBad(-2);
        assertBad(-12345); // Sanity check
        assertBad(Integer.MIN_VALUE);
    }

    void testGetLocation() throws Exception
    {
        setCurrentTest("testGetLocation()");
        providerSetUp(null);
        assertTrue(iLP != null, "LocationProvider is null");
        simpleGetLocation();
    }

    void testAltitudeData() throws Exception
    {
        setCurrentTest("testAltitudeData()");

        Criteria criteria = new Criteria();
        criteria.setAltitudeRequired(true);
        criteria.setPreferredResponseTime(100); // 100 ms

        providerSetUp(criteria);
        assertTrue(iLP != null, "LocationProvider is null");
        simpleGetLocation();

        QualifiedCoordinates coords = iLoc.getQualifiedCoordinates();
        float altitude = coords.getAltitude();
        assertTrue(!Float.isNaN(altitude), "No altitude included");
        // echo("Altitude = " + altitude);
    }

    void testSpeedData() throws Exception
    {
        setCurrentTest("testSpeedData()");

        Criteria criteria = new Criteria();
        criteria.setPreferredResponseTime(100); // 100 ms
        criteria.setSpeedAndCourseRequired(true);

        providerSetUp(criteria);
        assertTrue(iLP != null, "LocationProvider is null");
        simpleGetLocation();

        float speed = iLoc.getSpeed();
        assertTrue(!Float.isNaN(speed), "No speed included");
        float course = iLoc.getCourse();
        assertTrue(!Float.isNaN(course), "No course included");
        // echo("Speed = " + speed + ", Course = " + course);
    }

    void testAddressInfo() throws Exception
    {
        setCurrentTest("testAddressInfo()");

        Criteria criteria = new Criteria();
        criteria.setPreferredResponseTime(100); // 100 ms
        criteria.setAddressInfoRequired(true);

        providerSetUp(criteria);
        if (iLP != null)
        {
            assertTrue(iLP != null, "LocationProvider is null");
            simpleGetLocation();

            AddressInfo addr = iLoc.getAddressInfo();
            if (addr != null)
            {
                boolean hasNonNullField = false;
                for (int i = 1; i <= 17; ++i)
                {
                    String field = addr.getField(i);
                    if (field != null)
                    {
                        hasNonNullField = true;
                        // echo("Field #" + i + ": " + field);
                    }
                }
                assertTrue(hasNonNullField, "All AddressInfo fields are null");
            }
        }
    }

    void testGetLocationTimeout() throws Exception
    {
        setCurrentTest("testGetLocationTimeout()");

        Criteria criteria = new Criteria();
        criteria.setCostAllowed(false); // This will select the right provider
        providerSetUp(criteria);
        assertTrue(iLP != null, "LocationProvider is null");
        iLoc = null;

        try
        {
            int TIMEOUT = 1; // seconds
            iLoc = iLP.getLocation(TIMEOUT);
            assertTrue(false, "No timeout for getLocation");
        }
        catch (LocationException le)
        {
            // Timed out correctly
        }

        // Sanity check - test that normal request works ok
        simpleGetLocation();

        try
        {
            int TIMEOUT = 10; // seconds
            long startTime = System.currentTimeMillis();
            iLoc = iLP.getLocation(TIMEOUT);
            long duration = System.currentTimeMillis() - startTime;
            assertTrue(duration <= TIMEOUT * 1000, "GetLocation took too long");
            checkLocationData(iLoc);
        }
        catch (LocationException le)
        {
            assertTrue(false, "Illegal timeout");
        }
    }

    void testReset() throws Exception
    {
        setCurrentTest("testReset()");

        Criteria criteria = new Criteria();
        criteria.setCostAllowed(false); // This will select the right provider
        providerSetUp(criteria);
        assertTrue(iLP != null, "LocationProvider is null");

        LocThread t1 = new LocThread(iLP);
        LocThread t2 = new LocThread(iLP);

        t1.start();
        t2.start();

        try
        {
            // Sleep to let LocThreads get a chance to call getLocation
            Thread.sleep(1000);
            // Reset should cancel all getLocation calls
            iLP.reset();
            t1.join();
            t2.join();
        }
        finally
        {
            assertTrue(!t1.iGotPosition, "Thread1 got a position");
            assertTrue(!t2.iGotPosition, "Thread2 got a position");
            assertTrue(t1.iGotReset, "Thread1 was never reset");
            assertTrue(t2.iGotReset, "Thread2 was never reset");
        }
    }

    void testGetLatest()
    {
        setCurrentTest("testGetLatest()");

        iLP = null;
        iLoc = null;

        // Get the last known location
        iLoc = LocationProvider.getLastKnownLocation();
    }

    void testGetLocationOutOfService() throws Exception
    {
        setCurrentTest("testGetLocationOutOfService()");

        Criteria criteria = new Criteria();
        criteria.setCostAllowed(false); // This will select the right provider
        providerSetUp(criteria);

        assertTrue(iLP != null, "LocationProvider is null");

        simpleGetLocation();

        userMessage("Disable all PSYs");

        // now we get the last known Location:
        Location lastKnownLocation = LocationProvider.getLastKnownLocation();
        checkLocationData(lastKnownLocation);

        assertTrue(lastKnownLocation.getTimestamp() == iLoc.getTimestamp(),
                   "Not the last known location returned");

        long start = System.currentTimeMillis();
        long end = 0;
        try
        {
            iLoc = iLP.getLocation(-1);
            assertTrue(false, "No exception thrown for getLocation");
        }
        catch (LocationException le)
        {
            // Exception thrown correctly
            end = System.currentTimeMillis();
        }
        assertTrue(end - start < 500, "Expected LocationException immediately");

        userMessage("Test passed. Enable PSYs again");
    }

    // ------------------------ Helper methods -----------------------

    void simpleGetLocation() throws Exception
    {
        iLoc = null;

        try
        {
            iLoc = iLP.getLocation(-1);
            checkLocationData(iLoc);
        }
        catch (LocationException le)
        {
            assertTrue(false, "Could not get location: " + le);
        }
    }

    void assertBad(int aBadParam) throws Exception
    {
        try
        {
            iLoc = iLP.getLocation(aBadParam);
            assertTrue(false, "IllegalArgumentException was not thrown for: "
                       + aBadParam);
        }
        catch (IllegalArgumentException iae)
        {
            // Exception thrown correctly
        }
    }

    // ------------------------ Helper class -----------------------

    private class LocThread extends Thread
    {
        private LocationProvider iLP;

        boolean iGotReset = false;

        boolean iGotPosition = false;

        LocThread(LocationProvider aLP)
        {
            iLP = aLP;
        }

        public void run()
        {
            try
            {
                int TIMEOUT = 10; // seconds
                Location l = iLP.getLocation(TIMEOUT);
                iGotPosition = true;
            }
            catch (InterruptedException ie)
            {
                iGotReset = true;
            }
            catch (Exception e)
            {
                echo("Unexpected exception thrown: " + e);
            }
        }
    }

}
