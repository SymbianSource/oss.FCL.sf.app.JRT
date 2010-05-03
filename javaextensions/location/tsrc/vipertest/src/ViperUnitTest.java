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
import java.util.Enumeration;

import javax.microedition.location.*;
import javax.microedition.lcdui.*;

public class ViperUnitTest
{
    final static int TIMETOFIX = 5;

    protected LocationProvider iLP = null;

    final protected String iTestSuite;

    protected String iTestName;

    long[] iTimeArray;

    private static Display iDisplay;

    ViperUnitTest()
    {
        iTestSuite = "Test (no name)";
    }

    protected ViperUnitTest(String aTestSuite)
    {
        iTestSuite = aTestSuite;
    }

    static String run1()
    {
        ViperUnitTest testRunner = new ViperUnitTest();
        String title = "Running fast unit tests";
        ViperUnitTest[] tests = new ViperUnitTest[] { new CoordinatesTest(),
                new QualifiedCoordinatesTest(), new AddressInfoTest(),
                new CriteriaTest(), new LandmarkTest(),
                new LandmarkStoreTest(), new MultipleLandmarkStoresTest(),
                new ErrorsTest()
                                                    };

        return testRunner.run(tests, title);
    }

    static String run2(Display aDisplay)
    {
        iDisplay = aDisplay;
        ViperUnitTest testRunner = new ViperUnitTest();
        String title = "Running LocationProvider unit tests";
        ViperUnitTest[] tests = new ViperUnitTest[] { new GetLocationTest(),
                new PeriodicUpdateTest()
                                                    };

        return testRunner.run(tests, title);
    }

    String run(ViperUnitTest[] tests, String title)
    {
        echo(title);
        String result = null;
        int current = 0;
        iTimeArray = new long[tests.length];
        long startTime = System.currentTimeMillis();

        try
        {
            for (current = 0; current < tests.length; ++current)
            {
                tests[current].runTest();
                iTimeArray[current] = System.currentTimeMillis();
            }

            echo("\n-= Time info =-");
            long testStartTime = startTime;
            for (int i = 0; i < tests.length; ++i)
            {
                long time = iTimeArray[i] - testStartTime;
                echo(tests[i].iTestSuite + ": " + time + " ms");
                testStartTime = iTimeArray[i];
            }
            echo("---------------");
            echo("Total = " + (iTimeArray[tests.length - 1] - startTime) + "ms");
            echo("---------------\n");

            result = "All tests completed successfully";
        }
        catch (Throwable t)
        {
            echo("\n\n##### TEST FAILED #####\n\n");
            ViperUnitTest tst = tests[current];
            result = tst.iTestSuite + "." + tst.iTestName + " failed!\n" + t;
        }
        echo(result);
        return result;
    }

    protected void runTest() throws Throwable
    {
    }

    // ------------------------ Helper methods -----------------------

    // Unit test helper methods

    protected void assertTrue(boolean aCondition, String aErrorMessage)
    throws Error
    {
        if (!aCondition)
            throw new Error(aErrorMessage);
    }

    protected void assertFalse(boolean aCondition, String aErrorMessage)
    throws Error
    {
        if (aCondition)
            throw new Error(aErrorMessage);
    }

    protected void assertNoMessage(Exception e)
    {
        assertTrue(e.getMessage() == null,
                   "Message not allowed for exception: " + e);
    }

    protected void setCurrentTest(String aTestName)
    {
        iTestName = aTestName;
        System.out.println("Running: " + aTestName);
    }

    protected void echo(String aStr)
    {
        System.out.println(aStr);
    }

    protected String diff(String s1, String s2)
    {
        if (s1.length() != s2.length())
        {
            return "Different length";
        }

        char[] chs = new char[s1.length()];

        for (int i = 0; i < s1.length(); i++)
        {
            if (s1.charAt(i) != s2.charAt(i))
            {
                chs[i] = '^';
            }
            else
            {
                chs[i] = ' ';
            }
        }

        return new String(chs);
    }

    protected void userMessage(String aStr)
    {
        if (iDisplay == null)
        {
            throw new Error("userMessage: No display set");
        }
        Alert a = new Alert(iTestName, aStr, null, AlertType.INFO);

        CommandListener cl = new CommandListener()
        {
            public void commandAction(Command c, Displayable d)
            {
                synchronized (this)
                {
                    notifyAll();
                }
            }
        };

        a.setCommandListener(cl);
        a.setTimeout(Alert.FOREVER);

        Displayable d = iDisplay.getCurrent();
        iDisplay.setCurrent(a);
        try
        {
            synchronized (cl)
            {
                cl.wait();
            }
        }
        catch (InterruptedException ie)
        {
        }
        iDisplay.setCurrent(d);
    }

    // Location API specific helper methods

    protected void providerSetUp(Criteria aCriteria)
    {
        iLP = null;
        try
        {
            iLP = LocationProvider.getInstance(aCriteria);
            // if (aCriteria == null) {
            // echo("Default provider: " + iLP);
            // } else {
            // echo("Selected provider: " + iLP);
            // }
            if (iLP != null)
            {
                int state = iLP.getState();
                assertTrue(state == LocationProvider.AVAILABLE,
                           "Initial state=" + state + ", expected AVAILABLE");
            }
        }
        catch (LocationException le)
        {
            echo("Could not create location provider: " + le);
        }
    }

    protected void checkLocationData(Location aLoc)
    {
        assertTrue(aLoc != null, "Location is null");
        assertTrue(aLoc.isValid(), "Location is invalid");
        assertTrue(aLoc.getQualifiedCoordinates() != null,
                   "Location is valid, but Coordinates are null");

        long timestamp = aLoc.getTimestamp();
        long now = System.currentTimeMillis();
        assertTrue(now >= timestamp && (now - timestamp < 30000)
                   && timestamp > 0, "Timestamp incorrect: t=" + timestamp
                   + ", now=" + now);

        QualifiedCoordinates coords = aLoc.getQualifiedCoordinates();
        double lat = coords.getLatitude();
        double lon = coords.getLongitude();

        // echo("Lat: " + lat + " Lon: " + lon);

        assertTrue(lat >= -90.0 || lat <= 90.0, "Latitude out of range");
        assertTrue(lon >= -180.0 || lon < 180.0, "Longitude out of range");

        float hacc = coords.getHorizontalAccuracy();
        assertTrue(Float.isNaN(hacc) || hacc >= 0,
                   "Horizontal accuracy is negative");

        float vacc = coords.getVerticalAccuracy();
        assertTrue(Float.isNaN(vacc) || vacc >= 0,
                   "Vertical accuracy is negative");

        float speed = aLoc.getSpeed();
        assertTrue(Float.isNaN(speed) || speed >= 0, "Speed is negative");

        float course = aLoc.getCourse();
        assertTrue(Float.isNaN(course) || (course >= 0 && course < 360),
                   "Course out of range");

        String nmea = aLoc.getExtraInfo("application/X-jsr179-location-nmea");
        if (nmea != null)
        {
            assertTrue(nmea.startsWith("$GP"), "Bad NMEA data");
            echo("Extra info:\n" + nmea);
        }
    }

    protected void addLandmarkToStore(LandmarkStore ls, Landmark landmark,
                                      String category) throws Exception
    {

        Enumeration e = ls.getLandmarks();
        int numLandmarksBefore = 0;
        if (e != null)
        {
            while (e.hasMoreElements())
            {
                Object o = e.nextElement();
                ++numLandmarksBefore;
            }
        }

        ls.addLandmark(landmark, category);

        // check that landmark was added
        e = ls.getLandmarks();
        assertTrue(e != null, "Landmarks enumeration is null");

        int numLandmarksAfter = 0;
        while (e.hasMoreElements())
        {
            ++numLandmarksAfter;
            Object o = e.nextElement();
        }

        assertTrue(numLandmarksAfter - numLandmarksBefore == 1,
                   "Expected only one landmark to be added");
    }

    protected void removeExistingStores() throws Exception
    {
        String[] stores = LandmarkStore.listLandmarkStores();
        if (stores != null)
        {
            for (int i = 0; i < stores.length; ++i)
            {
                LandmarkStore.deleteLandmarkStore(stores[i]);
            }
        }
    }

    protected void deleteAllLandmarksAndCategories() throws Exception
    {
        // Delete all the categories and Landmarks from the store
        LandmarkStore ls = LandmarkStore.getInstance(null);
        Enumeration c = ls.getCategories();
        while (c.hasMoreElements())
        {
            ls.deleteCategory((String) c.nextElement());
        }

        Enumeration l = ls.getLandmarks();
        if (l != null)
        {
            while (l.hasMoreElements())
            {
                ls.deleteLandmark((Landmark) l.nextElement());
            }
        }
    }

}
