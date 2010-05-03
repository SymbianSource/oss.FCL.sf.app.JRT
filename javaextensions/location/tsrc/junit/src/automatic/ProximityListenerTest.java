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
 *
 *
 * @created 2009-07-24
 *
 */

public class ProximityListenerTest extends TestCase
{

    public ProximityListenerTest()
    {
    }

    public ProximityListenerTest(String sTestName, TestMethod rTestMethod)
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

        aSuite.addTest(new ProximityListenerTest("testBadArguments",
                       new TestMethod()
        {
            public void run(TestCase tc)
            {
                ((ProximityListenerTest) tc).testBadArguments();
            }
        }));

        aSuite.addTest(new ProximityListenerTest("testProximityListener",
                       new TestMethod()
        {
            public void run(TestCase tc)
            {
                ((ProximityListenerTest) tc).testProximityListener();
            }
        }));

        aSuite.addTest(new ProximityListenerTest("testRemoveProximityListener",
                       new TestMethod()
        {
            public void run(TestCase tc)
            {
                ((ProximityListenerTest) tc).testRemoveProximityListener();
            }
        }));

        return aSuite;
    }

    private void assertContinue(String aReason, boolean aCond)
    {
        if (!aCond)
            assertTrue(aReason, false);
    }

    public void testBadArguments()
    {
        Listener L1 = new Listener(60.0, 21.0, 0.0f, 100);

        try
        {
            try
            {
                LocationProvider.addProximityListener(null,
                                                      L1.coordinates,
                                                      L1.proximityRadius);

                assertContinue("NullPointerException not thrown 1", false);
            }
            catch (NullPointerException npe)
            {
                // Exception was thrown correctly
            }

            try
            {
                LocationProvider.addProximityListener(L1,
                                                      L1.coordinates,
                                                      0);

                assertContinue("IllegalArgumentException not thrown 1", false);
            }
            catch (IllegalArgumentException iae)
            {
                // Exception was thrown correctly
            }

            try
            {
                LocationProvider.addProximityListener(L1,
                                                      L1.coordinates,
                                                      -1);

                assertContinue("IllegalArgumentException not thrown 2", false);
            }
            catch (IllegalArgumentException iae)
            {
                // Exception was thrown correctly
            }

            try
            {
                LocationProvider.addProximityListener(L1,
                                                      L1.coordinates,
                                                      Float.NaN);

                assertContinue("IllegalArgumentException not thrown 3", false);
            }
            catch (IllegalArgumentException iae)
            {
                // Exception was thrown correctly
            }

            assertTrue("", true);
        }
        catch (Exception e)
        {
            assertTrue(e.getMessage(), false);
        }
    }

    public void testRemoveProximityListener()
    {
        Listener L1 = new Listener(78.338500d, 56.338500d, 0.0f, 500.0f);
        boolean timeout = false;
        long startTime = System.currentTimeMillis();
        try
        {
            LocationProvider.addProximityListener(L1,
                                                  L1.coordinates,
                                                  L1.proximityRadius);

            LocationProvider.removeProximityListener(L1);

            while (L1.IsCorrectEvent == false)
            {
                // Wait a moment before checking things
                try
                {
                    Thread.sleep(250);
                }
                catch (InterruptedException ie)
                {
                }
                System.out.println("Module Tests 11");
                // Wait for 1 minutes to get updates
                if (System.currentTimeMillis() - startTime > 60000)
                {
                    System.out.println("Module Tests 22");
                    timeout = true;
                    break;
                }
            }

            // If timeout occurs, the listener was removed correctly
            if (timeout)
            {
                assertTrue("", true);
            }
            else
            {
                assertTrue("Listener Not removed", false);
            }
        }
        catch (Exception e)
        {
            assertTrue(e.getMessage(), false);
        }

    }

    public void testProximityListener()
    {
        Listener L1 = new Listener(78.338500d, 56.338500d, 0.0f, 500.0f);

        boolean timeout = false;
        long startTime = System.currentTimeMillis();
        try
        {
            // Wait a moment before checking things
            try
            {
                Thread.sleep(250);
            }
            catch (InterruptedException ie)
            {
            }
            LocationProvider.addProximityListener(L1,
                                                  L1.coordinates,
                                                  L1.proximityRadius);
        }
        catch (Exception e)
        {
            assertTrue(e.getMessage(), false);
        }
        System.out.println("Module Tests 33");
        while (L1.IsCorrectEvent == false)
        {
            // Wait a moment before checking things
            try
            {
                Thread.sleep(250);
            }
            catch (InterruptedException ie)
            {
            }
            System.out.println("Module Tests 44");
            System.out.println("Module Tests System.currentTimeMillis() = " + System.currentTimeMillis());

            // Use a safeguard timeout in case the updating thread hangs
            // or takes too long. Wait for 1 minutes to get updates
            if (System.currentTimeMillis() - startTime > 60000)
            {
                timeout = true;
                break;
            }
        }
        if (timeout)
        {
            assertTrue("Timeout occured", false);
        }
        else
        {
            assertTrue("", true);
        }
    }
}

// Helper class
class Listener implements ProximityListener
{
    public float proximityRadius = Float.NaN;

    public Coordinates coordinates = null;

    public boolean IsCorrectEvent = false;

    Listener(double aLat, double aLon, float aAlt, float aProximityRadius)
    {
        coordinates = new Coordinates(aLat, aLon, aAlt);
        proximityRadius = aProximityRadius;
    }

    public void monitoringStateChanged(boolean aState)
    {
    }

    public void proximityEvent(Coordinates cord, Location loc)
    {
        IsCorrectEvent = true;
    }
}

// End of file
