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
 * This suite of testcases tests that: - OrientationTest object can be created -
 * Default values are correct -
 *
 *
 * @created 2008-07-20
 *
 */

public class OrientationTest extends TestCase
{

    public OrientationTest()
    {
    }

    public OrientationTest(String sTestName, TestMethod rTestMethod)
    {
        super(sTestName, rTestMethod);
    }

    /**
     *
     * /*************************************** Creates the test suite. You need
     * to add a new aSuite.addTest antry for any new test methods, otherwise
     * they won't be run.
     */
    public Test suite()
    {
        TestSuite aSuite = new TestSuite();

        aSuite.addTest(new OrientationTest("testAzimuthData", new TestMethod()
        {
            public void run(TestCase tc)
            {
                ((OrientationTest) tc).testAzimuthData();
            }
        }));

        aSuite.addTest(new OrientationTest("testPitchData", new TestMethod()
        {
            public void run(TestCase tc)
            {
                ((OrientationTest) tc).testPitchData();
            }
        }));

        aSuite.addTest(new OrientationTest("testRollData", new TestMethod()
        {
            public void run(TestCase tc)
            {
                ((OrientationTest) tc).testRollData();
            }
        }));

        aSuite.addTest(new OrientationTest("testIsOrientationMagneticData",
                                           new TestMethod()
        {
            public void run(TestCase tc)
            {
                ((OrientationTest) tc).testIsOrientationMagneticData();
            }
        }));

        return aSuite;
    }

    // Test Azimuth Data
    public void testAzimuthData()
    {

        try
        {
            Orientation orientation = Orientation.getOrientation();

            if (orientation != null)
            {
                float Azimuth = orientation.getCompassAzimuth();

                if (!((Azimuth <= 360.0f) && (Azimuth >= 0.0f)))
                {
                    assertTrue("Azimuth is out of range(0,360)", false);
                }

            }
            else
            {
                assertTrue("orientation can’t be currently determined", true);
            }
            assertTrue("", true);
        }
        catch (LocationException e)
        {
            assertTrue("Implmentation doesn't support Orientation", true);
        }
        catch (SecurityException e)
        {
            assertTrue("Calling application doesn't have the permission to"
                       + "query Orientation Data", true);
        }
    }

    // Test Pitch Data
    public void testPitchData()
    {

        try
        {
            Orientation orientation = Orientation.getOrientation();

            if (orientation != null)
            {
                float Pitch = orientation.getPitch();

                if (!(((Pitch <= 90.0f) && (Pitch >= -90.0f)) || Float
                        .isNaN(Pitch)))
                {
                    assertTrue("Pitch is out of range(90,-90)", false);
                }
            }
            else
            {
                assertTrue("orientation can’t be currently determined", true);
            }

            assertTrue("", true);
        }
        catch (LocationException e)
        {
            assertTrue("Implmentation doesn't support Orientation", true);
        }
        catch (SecurityException e)
        {
            assertTrue("Calling application doesn't have the permission to"
                       + "query Orientation Data", true);
        }
    }

    // Test Roll Data
    public void testRollData()
    {

        try
        {
            Orientation orientation = Orientation.getOrientation();

            if (orientation != null)
            {
                float Roll = orientation.getRoll();

                if (!(((Roll <= 180.0f) && (Roll >= -180.0f)) || Float
                        .isNaN(Roll)))
                {
                    assertTrue("Roll is out of range(180,-180)", false);
                }
            }
            else
            {
                assertTrue("orientation can’t be currently determined", true);
            }

            assertTrue("", true);
        }
        catch (LocationException e)
        {
            assertTrue("Implmentation doesn't support Orientation", true);
        }
        catch (SecurityException e)
        {
            assertTrue("Calling application doesn't have the permission to"
                       + "query Orientation Data", true);
        }
    }

    // Test Orientation data is w.r.t Magnetic Data or True North Data
    public void testIsOrientationMagneticData()
    {

        try
        {
            Orientation orientation = Orientation.getOrientation();

            if (orientation != null)
            {
                boolean IsMagnetic = orientation.isOrientationMagnetic();

                if (!IsMagnetic)
                {
                    assertTrue("IsMagnertic Data is wrong", false);
                }
            }
            else
            {
                assertTrue("orientation can’t be currently determined", true);
            }

            assertTrue("", true);
        }
        catch (LocationException e)
        {
            assertTrue("Implmentation doesn't support Orientation", true);
        }
        catch (SecurityException e)
        {
            assertTrue("Calling application doesn't have the permission to"
                       + "query Orientation Data", true);
        }
    }
}
