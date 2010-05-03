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
 */

public class LandmarkTest extends TestCase
{

    public LandmarkTest()
    {
    }

    public LandmarkTest(String sTestName, TestMethod rTestMethod)
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

        aSuite.addTest(new LandmarkTest("testBadConstructor", new TestMethod()
        {
            public void run(TestCase tc)
            {
                ((LandmarkTest) tc).testBadConstructor();
            }
        }));

        aSuite.addTest(new LandmarkTest("testConstructor", new TestMethod()
        {
            public void run(TestCase tc)
            {
                ((LandmarkTest) tc).testConstructor();
            }
        }));

        aSuite.addTest(new LandmarkTest("testBadSetter", new TestMethod()
        {
            public void run(TestCase tc)
            {
                ((LandmarkTest) tc).testBadSetter();
            }
        }));

        aSuite.addTest(new LandmarkTest("testSetters", new TestMethod()
        {
            public void run(TestCase tc)
            {
                ((LandmarkTest) tc).testSetters();
            }
        }));

        return aSuite;

    }

    private void assertContinue(String aReason, boolean aCond)
    {
        if (!aCond)
            assertTrue(aReason, false);
    }

    public void testBadConstructor()
    {

        try
        {
            // Trying to create a landmark where name is null
            Landmark lm = new Landmark(null, null, null, null);
            assertContinue("No exception thrown for constructor values", false);
        }
        catch (NullPointerException npe)
        {
            // Exception thrown correctly
        }

        assertTrue("", true);
    }

    public void testConstructor()
    {

        // Create a Landmark object with correct parameters and check that
        // the values are unchanged when read.
        String name = "Office";
        Landmark lm1 = new Landmark(name, null, null, null);

        String description = "Where I work";
        Landmark lm2 = new Landmark(name, description, null, null);

        float hacc = 50.0f;
        float vacc = 80.0f;
        QualifiedCoordinates coords = new QualifiedCoordinates(57.0f, 17.0f,
                34.0f, hacc, vacc);

        Landmark lm3 = new Landmark(name, null, coords, null);
        Landmark lm4 = new Landmark(name, description, coords, null);

        AddressInfo address = new AddressInfo();

        Landmark lm5 = new Landmark(name, null, null, address);
        Landmark lm6 = new Landmark(name, description, null, address);
        Landmark lm7 = new Landmark(name, null, coords, address);

        Landmark lm = new Landmark(name, description, coords, address);

        // Check the Landmark's values
        assertContinue("Retrieved Landmark values incorrect",
                       lm.getName() == name && lm.getDescription() == description
                       && lm.getQualifiedCoordinates() == coords
                       && lm.getAddressInfo() == address);

        assertTrue("", true);
    }

    public void testBadSetter()
    {
        String name = "Office";

        // Create a new unmodified Landmark object
        Landmark lm = new Landmark(name, null, null, null);

        try
        {
            lm.setName(null);
            assertContinue("No exception thrown for bad argument", false);
        }
        catch (NullPointerException npe)
        {
            // Exception thrown correctly
        }
        assertTrue("", true);
    }

    public void testSetters()
    {

        String name = "Office";

        // Create a new unmodified Landmark object
        Landmark lm = new Landmark(name, null, null, null);

        String newName = "Home";
        String description = "Where I live";
        QualifiedCoordinates coords = new QualifiedCoordinates(57.0f, 17.0f,
                34.0f, 20.0f, 20.0f);
        AddressInfo address = new AddressInfo();

        lm.setName(newName);
        lm.setDescription(description);
        lm.setQualifiedCoordinates(coords);
        lm.setAddressInfo(address);

        // Check the Landmark's values
        assertContinue("Retrieved Landmark values incorrect",
                       lm.getName() == newName && lm.getDescription() == description
                       && lm.getQualifiedCoordinates() == coords
                       && lm.getAddressInfo() == address);

        assertTrue("", true);
    }
}
