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

public class LandmarkTest extends ViperUnitTest
{

    public LandmarkTest()
    {
        super("LandmarkTest");
    }

    protected void runTest() throws java.lang.Throwable
    {
        testBadConstructor();
        testConstructor();
        testBadSetter();
        testSetters();
    }

    void testBadConstructor()
    {
        setCurrentTest("testBadConstructor()");

        try
        {
            // Trying to create a landmark where name is null
            Landmark lm = new Landmark(null, null, null, null);
            assertTrue(false, "No exception thrown for constructor values");
        }
        catch (NullPointerException npe)
        {
            // Exception thrown correctly
        }
    }

    void testConstructor()
    {
        setCurrentTest("testConstructor()");

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
        assertTrue(lm.getName() == name && lm.getDescription() == description
                   && lm.getQualifiedCoordinates() == coords
                   && lm.getAddressInfo() == address,
                   "Retrieved Landmark values incorrect");

    }

    void testBadSetter()
    {
        setCurrentTest("testBadSetter()");
        String name = "Office";

        // Create a new unmodified Landmark object
        Landmark lm = new Landmark(name, null, null, null);

        try
        {
            lm.setName(null);
            assertTrue(false, "No exception thrown for bad argument");
        }
        catch (NullPointerException npe)
        {
            // Exception thrown correctly
        }

    }

    void testSetters()
    {
        setCurrentTest("testSetters()");

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
        assertTrue(lm.getName() == newName
                   && lm.getDescription() == description
                   && lm.getQualifiedCoordinates() == coords
                   && lm.getAddressInfo() == address,
                   "Retrieved Landmark values incorrect");
    }
}
