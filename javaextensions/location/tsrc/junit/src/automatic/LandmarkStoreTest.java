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
import java.util.Enumeration;

/**
 * <b>TEST CASE SPECIFICATION</b>
 *
 */

public class LandmarkStoreTest extends TestCase
{
    static final int MAX_NAME_LENGTH = 255;

    static final int MAX_CATEGORY_LENGTH = 124;

    static final int MAX_DESCRIPTION_LENGTH = 4095;

    static final int MAX_ADDRESSFIELD_LENGTH = 255;

    static final String extension = "23";

    static final String street = "11 Mount Avenue";

    static final String city = "London";

    static final String county = "Ealing";

    static final String postal_code = "W5 1QB";

    static final String state = "England";

    static final String district = "Middlesex";

    static final String country = "United Kingdom";

    static final String country_code = "GB";

    static final String building_name = "The Castle";

    static final String building_floor = "3";

    static final String building_room = "Front Room";

    static final String building_zone = "Upstairs";

    static final String crossing1 = "Mount Avenue";

    static final String crossing2 = "Eaton Rise";

    static final String url = "http://www.upmystreet.co.uk";

    static final String phone_number = "+358401234567";

    static final String searchCategory0 = "category0";

    static final String searchCategory1 = "category1";

    static final String searchCategory2 = "category2";

    static final String searchName0 = "name0"; // Will not be added

    static final String searchName1 = "name1"; // Will be added once

    static final String searchName2 = "name2"; // Will be added twice

    static final String searchName3 = "name3"; // Will be added three times

    final static int TIMETOFIX = 5;

    protected LocationProvider iLP = null;

    long[] iTimeArray;

    private LandmarkStore ls;

    private String iCategory;

    public LandmarkStoreTest()
    {
    }

    public LandmarkStoreTest(String sTestName, TestMethod rTestMethod)
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

        aSuite.addTest(new LandmarkStoreTest("testGetInstance",
                                             new TestMethod()
        {
            public void run(TestCase tc)
            {
                ((LandmarkStoreTest) tc).testGetInstance();
            }
        }));

        aSuite.addTest(new LandmarkStoreTest("testDeleteLandmarkBadArguments",
                                             new TestMethod()
        {
            public void run(TestCase tc)
            {
                ((LandmarkStoreTest) tc)
                .testDeleteLandmarkBadArguments();
            }
        }));

        aSuite.addTest(new LandmarkStoreTest("testGetLandmarksEmptyStore",
                                             new TestMethod()
        {
            public void run(TestCase tc)
            {
                ((LandmarkStoreTest) tc).testGetLandmarksEmptyStore();
            }
        }));

        aSuite.addTest(new LandmarkStoreTest("setCategory", new TestMethod()
        {
            public void run(TestCase tc)
            {
                ((LandmarkStoreTest) tc).setCategory(null);
            }
        }));

        aSuite.addTest(new LandmarkStoreTest(
                           "testAddGetDeleteAndUpdateLandmarks", new TestMethod()
        {
            public void run(TestCase tc)
            {
                ((LandmarkStoreTest) tc)
                .testAddGetDeleteAndUpdateLandmarks();
            }
        }));

        aSuite.addTest(new LandmarkStoreTest("testAddCategoryBadArguments",
                                             new TestMethod()
        {
            public void run(TestCase tc)
            {
                ((LandmarkStoreTest) tc).testAddCategoryBadArguments();
            }
        }));

        aSuite.addTest(new LandmarkStoreTest("testDeleteCategoryBadArguments",
                                             new TestMethod()
        {
            public void run(TestCase tc)
            {
                ((LandmarkStoreTest) tc)
                .testDeleteCategoryBadArguments();
            }
        }));

        aSuite.addTest(new LandmarkStoreTest("testAddDeleteCategory",
                                             new TestMethod()
        {
            public void run(TestCase tc)
            {
                ((LandmarkStoreTest) tc).testAddDeleteCategory();
            }
        }));

        aSuite.addTest(new LandmarkStoreTest("testAddDeleteManyCategories",
                                             new TestMethod()
        {
            public void run(TestCase tc)
            {
                ((LandmarkStoreTest) tc).testAddDeleteManyCategories();
            }
        }));

        aSuite.addTest(new LandmarkStoreTest("testAddCategoryTruncate",
                                             new TestMethod()
        {
            public void run(TestCase tc)
            {
                ((LandmarkStoreTest) tc).testAddCategoryTruncate();
            }
        }));

        aSuite.addTest(new LandmarkStoreTest("testAddLandmarkToCategory1",
                                             new TestMethod()
        {
            public void run(TestCase tc)
            {
                ((LandmarkStoreTest) tc).testAddLandmarkToCategory1();
            }
        }));

        aSuite.addTest(new LandmarkStoreTest("testAddLandmarkToCategory2",
                                             new TestMethod()
        {
            public void run(TestCase tc)
            {
                ((LandmarkStoreTest) tc).testAddLandmarkToCategory2();
            }
        }));

        aSuite.addTest(new LandmarkStoreTest("testAddLandmarkToCategory3",
                                             new TestMethod()
        {
            public void run(TestCase tc)
            {
                ((LandmarkStoreTest) tc).testAddLandmarkToCategory3();
            }
        }));

        aSuite.addTest(new LandmarkStoreTest(
                           "testAddLandmarkToCategoryDeleteCategory", new TestMethod()
        {
            public void run(TestCase tc)
            {
                ((LandmarkStoreTest) tc)
                .testAddLandmarkToCategoryDeleteCategory();
            }
        }));

        aSuite.addTest(new LandmarkStoreTest(
                           "testAddLandmarkSameNameDifferentCategory", new TestMethod()
        {
            public void run(TestCase tc)
            {
                ((LandmarkStoreTest) tc)
                .testAddLandmarkSameNameDifferentCategory();
            }
        }));

        aSuite.addTest(new LandmarkStoreTest("setCategory", new TestMethod()
        {
            public void run(TestCase tc)
            {
                ((LandmarkStoreTest) tc).setCategory("Work");
            }
        }));

        aSuite.addTest(new LandmarkStoreTest(
                           "testAddGetDeleteAndUpdateLandmarks", new TestMethod()
        {
            public void run(TestCase tc)
            {
                ((LandmarkStoreTest) tc)
                .testAddGetDeleteAndUpdateLandmarks();
            }
        }));

        aSuite.addTest(new LandmarkStoreTest(
                           "testRemoveLandmarkFromCategoryBadArguments", new TestMethod()
        {
            public void run(TestCase tc)
            {
                ((LandmarkStoreTest) tc)
                .testRemoveLandmarkFromCategoryBadArguments();
            }
        }));

        aSuite.addTest(new LandmarkStoreTest("testRemoveLandmarkFromCategory",
                                             new TestMethod()
        {
            public void run(TestCase tc)
            {
                ((LandmarkStoreTest) tc)
                .testRemoveLandmarkFromCategory();
            }
        }));

        aSuite.addTest(new LandmarkStoreTest("testRemoveLandmarkFromCategory2",
                                             new TestMethod()
        {
            public void run(TestCase tc)
            {
                ((LandmarkStoreTest) tc)
                .testRemoveLandmarkFromCategory2();
            }
        }));

        aSuite.addTest(new LandmarkStoreTest(
                           "testGetLandmarksByNameBadArguments", new TestMethod()
        {
            public void run(TestCase tc)
            {
                ((LandmarkStoreTest) tc)
                .testGetLandmarksByNameBadArguments();
            }
        }));

        aSuite.addTest(new LandmarkStoreTest("testGetLandmarksByName",
                                             new TestMethod()
        {
            public void run(TestCase tc)
            {
                ((LandmarkStoreTest) tc).testGetLandmarksByName();
            }
        }));

        aSuite.addTest(new LandmarkStoreTest(
                           "testGetLandmarksByNameUncategorized", new TestMethod()
        {
            public void run(TestCase tc)
            {
                ((LandmarkStoreTest) tc)
                .testGetLandmarksByNameUncategorized();
            }
        }));

        aSuite.addTest(new LandmarkStoreTest("testGetLandmarksByNameWildcards",
                                             new TestMethod()
        {
            public void run(TestCase tc)
            {
                ((LandmarkStoreTest) tc)
                .testGetLandmarksByNameWildcards();
            }
        }));

        aSuite.addTest(new LandmarkStoreTest(
                           "testGetLandmarksByCategoryBadArguments", new TestMethod()
        {
            public void run(TestCase tc)
            {
                ((LandmarkStoreTest) tc)
                .testGetLandmarksByCategoryBadArguments();
            }
        }));

        aSuite.addTest(new LandmarkStoreTest("testGetLandmarksByCategory",
                                             new TestMethod()
        {
            public void run(TestCase tc)
            {
                ((LandmarkStoreTest) tc).testGetLandmarksByCategory();
            }
        }));

        aSuite.addTest(new LandmarkStoreTest(
                           "testGetLandmarksByNameAndCategory", new TestMethod()
        {
            public void run(TestCase tc)
            {
                ((LandmarkStoreTest) tc)
                .testGetLandmarksByNameAndCategory();
            }
        }));

        aSuite.addTest(new LandmarkStoreTest(
                           "testGetLandmarksByAreaBadArguments", new TestMethod()
        {
            public void run(TestCase tc)
            {
                ((LandmarkStoreTest) tc)
                .testGetLandmarksByAreaBadArguments();
            }
        }));

        aSuite.addTest(new LandmarkStoreTest("testGetLandmarksByArea",
                                             new TestMethod()
        {
            public void run(TestCase tc)
            {
                ((LandmarkStoreTest) tc).testGetLandmarksByArea();
            }
        }));

        aSuite.addTest(new LandmarkStoreTest("testGetLandmarksByArea2",
                                             new TestMethod()
        {
            public void run(TestCase tc)
            {
                ((LandmarkStoreTest) tc).testGetLandmarksByArea2();
            }
        }));

        aSuite.addTest(new LandmarkStoreTest(
                           "testGetLandmarksByAreaAndCategory", new TestMethod()
        {
            public void run(TestCase tc)
            {
                ((LandmarkStoreTest) tc)
                .testGetLandmarksByAreaAndCategory();
            }
        }));

        aSuite.addTest(new LandmarkStoreTest("testEnumerationDeleteLandmark1",
                                             new TestMethod()
        {
            public void run(TestCase tc)
            {
                ((LandmarkStoreTest) tc)
                .testEnumerationDeleteLandmark1();
            }
        }));

        aSuite.addTest(new LandmarkStoreTest("testEnumerationDeleteLandmark2",
                                             new TestMethod()
        {
            public void run(TestCase tc)
            {
                ((LandmarkStoreTest) tc)
                .testEnumerationDeleteLandmark2();
            }
        }));

        aSuite.addTest(new LandmarkStoreTest("assertEmptyLandmarkStore",
                                             new TestMethod()
        {
            public void run(TestCase tc)
            {
                ((LandmarkStoreTest) tc).assertEmptyLandmarkStore();
            }
        }));

        aSuite.addTest(new LandmarkStoreTest("testGetInstance2",
                                             new TestMethod()
        {
            public void run(TestCase tc)
            {
                ((LandmarkStoreTest) tc).testGetInstance2();
            }
        }));

        aSuite.addTest(new LandmarkStoreTest("testGetInstanceWithGc",
                                             new TestMethod()
        {
            public void run(TestCase tc)
            {
                ((LandmarkStoreTest) tc).testGetInstanceWithGc();
            }
        }));

        aSuite.addTest(new LandmarkStoreTest("testCreateLandmarkStore1",
                                             new TestMethod()
        {
            public void run(TestCase tc)
            {
                ((LandmarkStoreTest) tc).testCreateLandmarkStore1();
            }
        }));

        aSuite.addTest(new LandmarkStoreTest("testCreateLandmarkStore2",
                                             new TestMethod()
        {
            public void run(TestCase tc)
            {
                ((LandmarkStoreTest) tc).testCreateLandmarkStore2();
            }
        }));

        aSuite.addTest(new LandmarkStoreTest("testCreateLandmarkStore3",
                                             new TestMethod()
        {
            public void run(TestCase tc)
            {
                ((LandmarkStoreTest) tc).testCreateLandmarkStore3();
            }
        }));

        aSuite.addTest(new LandmarkStoreTest("testCreateLandmarkStore4",
                                             new TestMethod()
        {
            public void run(TestCase tc)
            {
                ((LandmarkStoreTest) tc).testCreateLandmarkStore4();
            }
        }));

        return aSuite;

    }

    private void assertContinue(String aReason, boolean aCond)
    {
        if (!aCond)
            assertTrue(aReason, false);
    }

    void setCategory(String aCat)
    {
        iCategory = aCat;
        assertTrue("", true);
    }

    void testAddGetDeleteAndUpdateLandmarks()
    {
        try
        {
            if (iCategory != null)
            {
                ls = getDefaultLandmarkStore();
                ls.addCategory(iCategory);
            }

            testAddLandmarkBadArguments();
            testAddDeleteLandmark1();
            testAddDeleteLandmark2();
            testAddDeleteLandmark3();
            testAddDeleteLandmark4();
            testAddLandmarkTwice();
            testAddLandmarkNoUpdate();
            testAddLandmarkSameName();

            testAddDeleteAddLandmark();
            testAddLandmarkTruncate();
            testAddLandmarkEmptyStrings();

            testUpdateLandmarkBadArguments();
            testUpdateLandmark1();
            testUpdateLandmark2();

            testUpdateLandmarkTruncate();
            testAddDeleteUpdateLandmark();

            testGetAllLandmarks();

            if (iCategory != null)
            {
                ls = getDefaultLandmarkStore();
                ls.deleteCategory(iCategory);
            }

            assertTrue("", true);
        }
        catch (Exception e)
        {
            assertTrue(e.getMessage(), false);
        }
    }

    public void testGetInstance()
    {

        // Get default store
        ls = LandmarkStore.getInstance(null);
        assertTrue("Instance was null for default store", ls != null);
    }

    public void testAddLandmarkBadArguments()
    {
        try
        {
            ls = getDefaultLandmarkStore();

            try
            {
                // Add with null Landmark parameter
                ls.addLandmark(null, iCategory);
                assertContinue("NullPointerException not thrown", false);
            }
            catch (NullPointerException npe)
            {
                // Exception was thrown correctly
            }

            Landmark lm = new Landmark("test", null, null, null);

            try
            {
                String badCategory = "This category does not exist";
                ls.addLandmark(lm, badCategory); // Add with nonexisting
                // category
                assertContinue("IllegalArgumentException not thrown 1", false);
            }
            catch (IllegalArgumentException iae)
            {
                // Exception was thrown correctly
            }

            try
            {
                lm.setName(new String(new char[MAX_NAME_LENGTH + 1]));
                ls.addLandmark(lm, iCategory); // Add Landmark with too long
                // name
                assertContinue("IllegalArgumentException not thrown 2", false);
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

    public void testDeleteLandmarkBadArguments()
    {
        try
        {
            ls = getDefaultLandmarkStore();

            try
            {
                ls.deleteLandmark(null); // Delete with null Landmark
                // parameter
                assertContinue("NullPointerException not thrown", false);
            }
            catch (NullPointerException npe)
            {
                // Exception was thrown correctly
            }

            try
            {
                Landmark lm = new Landmark("test", null, null, null);
                ls.deleteLandmark(lm); // Try to delete a Landmark that was
                // never added
                assertContinue("LandmarkException not thrown", false);
            }
            catch (LandmarkException le)
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

    public void testGetLandmarksEmptyStore()
    {
        try
        {
            ls = getDefaultLandmarkStore();
            Enumeration e = ls.getLandmarks();
            assertTrue("Expected getLandmarks to return null", e == null);
        }
        catch (Exception e)
        {
            assertTrue(e.getMessage(), false);
        }
    }

    // Create a Landmark object, store it in the LandmarkStore, check
    // that it can be found again and then delete it
    public void testAddDeleteLandmark1()
    {
        try
        {

            String name = "Nowhere";

            // Create a new Landmark object with only name set
            Landmark lm = new Landmark(name, null, null, null);

            saveRetrieveDelete(lm);

            assertTrue("", true);
        }
        catch (Exception e)
        {
            assertTrue(e.getMessage(), false);
        }
    }

    public void testAddDeleteLandmark2()
    {
        try
        {
            String name = "Somewhere";
            String desc = "It's somewhere but noone knows where it really is";

            // Create a new Landmark object with only name set
            Landmark lm = new Landmark(name, desc, null, null);

            saveRetrieveDelete(lm);

            assertTrue("", true);
        }
        catch (Exception e)
        {
            assertTrue(e.getMessage(), false);
        }
    }

    public void testAddDeleteLandmark3()
    {
        try
        {
            String name = "North pole";
            String desc = "Santas workshop, whats that address again?";

            QualifiedCoordinates coords = new QualifiedCoordinates(90.0f, 0.0f,
                    0.0f, 10.0f, 10.0f);

            // Create a new Landmark object with only name set
            Landmark lm = new Landmark(name, desc, coords, null);

            saveRetrieveDelete(lm);

            assertTrue("", true);
        }
        catch (Exception e)
        {
            assertTrue(e.getMessage(), false);
        }
    }

    public void testAddDeleteLandmark4()
    {
        try
        {
            String name = "Office";
            String desc = "Where you work";

            QualifiedCoordinates coords = new QualifiedCoordinates(57.0f,
                    17.0f, 34.0f, 50.0f, 80.0f);

            AddressInfo address = new AddressInfo();
            address.setField(AddressInfo.EXTENSION, extension);
            address.setField(AddressInfo.STREET, street);
            address.setField(AddressInfo.POSTAL_CODE, postal_code);
            address.setField(AddressInfo.CITY, city);
            address.setField(AddressInfo.COUNTY, county);
            address.setField(AddressInfo.DISTRICT, district);
            address.setField(AddressInfo.STATE, state);
            address.setField(AddressInfo.COUNTRY, country);
            address.setField(AddressInfo.COUNTRY_CODE, country_code);
            address.setField(AddressInfo.BUILDING_NAME, building_name);
            address.setField(AddressInfo.BUILDING_FLOOR, building_floor);
            address.setField(AddressInfo.BUILDING_ROOM, building_room);
            address.setField(AddressInfo.BUILDING_ZONE, building_zone);
            address.setField(AddressInfo.CROSSING1, crossing1);
            address.setField(AddressInfo.CROSSING2, crossing2);
            address.setField(AddressInfo.URL, url);
            address.setField(AddressInfo.PHONE_NUMBER, phone_number);

            // Create a new Landmark object
            Landmark lm = new Landmark(name, desc, coords, address);

            saveRetrieveDelete(lm);

            assertTrue("", true);
        }
        catch (Exception e)
        {
            assertTrue(e.getMessage(), false);
        }
    }

    // Tests that a Landmark can be added twice.
    public void testAddLandmarkTwice()
    {
        try
        {
            String name = "Add this landmark twice";
            Landmark lm = new Landmark(name, null, null, null);

            ls = getDefaultLandmarkStore();
            ls.addLandmark(lm, iCategory);
            ls.addLandmark(lm, iCategory);

            Enumeration e = ls.getLandmarks();
            int count = findLandmarks(e, new String[] { name });
            assertContinue("Wrong number of landmarks found", count == 1);

            ls.deleteLandmark(lm);

            assertTrue("", true);
        }
        catch (Exception e)
        {
            assertTrue(e.getMessage(), false);
        }
    }

    // Tests that addLandmark can not be used as updateLandmark.
    public void testAddLandmarkNoUpdate() throws Exception
    {
        String name = "Add this landmark twice";
        Landmark lm = new Landmark(name, null, null, null);

        ls = getDefaultLandmarkStore();
        ls.addLandmark(lm, iCategory);

        // Check that addLandmark is not doing what updateLandmark should do
        lm.setDescription("Whatever");
        lm.setQualifiedCoordinates(new QualifiedCoordinates(0, 0, 0, 0, 0));
        lm.setAddressInfo(new AddressInfo());
        ls.addLandmark(lm, iCategory);

        Enumeration e = ls.getLandmarks();
        Landmark fetchedLm = findLandmark(e, name);
        checkLandmark(fetchedLm, name, null, null, null);

        ls.deleteLandmark(lm);

        assertTrue("", true);
    }

    // Tests that Landmarks with the same name can be added.
    public void testAddLandmarkSameName()
    {
        try
        {
            String name = "Glenn";
            Landmark lm1 = new Landmark(name, null, null, null);
            String desc = "Another landmark with name=Glenn";
            Landmark lm2 = new Landmark(name, desc, null, null);

            ls = getDefaultLandmarkStore();
            ls.addLandmark(lm1, iCategory);
            ls.addLandmark(lm2, iCategory);

            Enumeration e = ls.getLandmarks();
            int count = findLandmarks(e, new String[] { name, name });
            assertContinue("Wrong number of landmarks found", count == 2);

            ls.deleteLandmark(lm1);
            ls.deleteLandmark(lm2);

            assertTrue("", true);
        }
        catch (Exception e)
        {
            assertTrue(e.getMessage(), false);
        }
    }

    // Tests that a Landmark can be added, then deleted and added again.
    public void testAddDeleteAddLandmark()
    {
        try
        {
            ls = getDefaultLandmarkStore();

            String name = "LandmarkToBeAddedDeletedAndAddedAgain";
            Landmark lm = new Landmark(name, null, null, null);
            Enumeration e;

            ls.addLandmark(lm, iCategory);

            e = ls.getLandmarks();
            Landmark fetchedLm = findLandmark(e, name);
            ls.deleteLandmark(fetchedLm);

            ls.addLandmark(lm, iCategory);

            e = ls.getLandmarks();
            int count = findLandmarks(e, new String[] { name });
            assertContinue("Wrong number of landmarks found", count == 1);

            ls.deleteLandmark(lm);

            assertTrue("", true);
        }
        catch (Exception e)
        {
            assertTrue(e.getMessage(), false);
        }
    }

    // Tests that description and address are truncated if they are too long
    public void testAddLandmarkTruncate()
    {
        try
        {
            ls = getDefaultLandmarkStore();

            String name = "Longest address in the world :)";

            String maxDescription = new String(new char[MAX_DESCRIPTION_LENGTH]);
            String tooLongDescription = maxDescription + "could be a lot more";

            String maxAddrField = new String(new char[MAX_ADDRESSFIELD_LENGTH]);
            String tooLongAddrField = maxAddrField + "some more";
            AddressInfo tooLongAddr = new AddressInfo();
            for (int i = 0; i < 17; ++i)
            {
                tooLongAddr.setField(i + 1, tooLongAddrField);
            }

            // Add the landmark that has info with too long length
            Landmark lm = new Landmark(name, tooLongDescription, null,
                                       tooLongAddr);
            ls.addLandmark(lm, iCategory);

            // All fields should be truncated to max length
            AddressInfo expectedAddr = new AddressInfo();
            for (int i = 0; i < 17; ++i)
            {
                expectedAddr.setField(i + 1, maxAddrField);
            }

            Enumeration e = ls.getLandmarks();
            Landmark fetchedLm = findLandmark(e, name);
            checkLandmark(fetchedLm, name, maxDescription, null, expectedAddr);

            ls.deleteLandmark(lm);

            assertTrue("", true);
        }
        catch (Exception e)
        {
            assertTrue(e.getMessage(), false);
        }
    }

    public void testUpdateLandmarkBadArguments()
    {
        try
        {
            ls = getDefaultLandmarkStore();

            try
            {
                ls.updateLandmark(null); // Update with null Landmark
                // parameter
                assertContinue("NullPointerException not thrown", false);
            }
            catch (NullPointerException npe)
            {
                // Exception was thrown correctly
            }

            Landmark lm = new Landmark("test", null, null, null);
            try
            {
                ls.updateLandmark(lm); // Update without adding first
                assertContinue("LandmarkException not thrown 1", false);
            }
            catch (LandmarkException iae)
            {
                // Exception was thrown correctly
            }

            try
            {
                ls.addLandmark(lm, iCategory);
                ls.deleteLandmark(lm);
                ls.updateLandmark(lm); // Landmark no longer belongs to store
                assertContinue("LandmarkException not thrown 2", false);
            }
            catch (LandmarkException iae)
            {
                // Exception was thrown correctly
            }

            try
            {
                ls.addLandmark(lm, iCategory);
                lm.setName(new String(new char[MAX_NAME_LENGTH + 1]));
                ls.updateLandmark(lm); // Update Landmark with too long name
                assertContinue("IllegalArgumentException not thrown 3", false);
            }
            catch (IllegalArgumentException iae)
            {
                // Exception was thrown correctly
            }

            ls.deleteLandmark(lm);

            assertTrue("", true);
        }
        catch (Exception e)
        {
            assertTrue(e.getMessage(), false);
        }
    }

    // Add a landmark to the LandmarkStore and update with more and more
    // information.
    public void testUpdateLandmark1()
    {
        try
        {
            // Create a new Landmark object
            String name = "Home";
            Landmark lm = new Landmark(name, null, null, null);

            ls = getDefaultLandmarkStore();

            // Save the landmark
            ls.addLandmark(lm, iCategory);

            Landmark fetchedLm = null;

            fetchedLm = findLandmark(ls.getLandmarks(), name);
            checkLandmark(fetchedLm, name, null, null, null);

            // Update the landmark with a decription
            String description = "There is no place like it";
            lm.setDescription(description);
            ls.updateLandmark(lm);

            fetchedLm = findLandmark(ls.getLandmarks(), name);
            checkLandmark(fetchedLm, name, description, null, null);

            // Update the landmark with QualifiedCoordinates and change the name
            QualifiedCoordinates coords = new QualifiedCoordinates(-10.2f,
                    -107.8f, -4.5f, 11.9f, 5.1f);

            lm.setQualifiedCoordinates(coords);
            name = "My place";
            lm.setName(name);

            ls.updateLandmark(lm);

            fetchedLm = findLandmark(ls.getLandmarks(), name);
            checkLandmark(fetchedLm, name, description, coords, null);

            // Update the landmark with AddressInfo
            AddressInfo address = new AddressInfo();
            address.setField(AddressInfo.EXTENSION, extension);
            address.setField(AddressInfo.STREET, street);
            address.setField(AddressInfo.POSTAL_CODE, postal_code);
            address.setField(AddressInfo.CITY, city);
            address.setField(AddressInfo.COUNTY, county);
            address.setField(AddressInfo.DISTRICT, district);
            address.setField(AddressInfo.STATE, state);
            address.setField(AddressInfo.COUNTRY, country);
            address.setField(AddressInfo.COUNTRY_CODE, country_code);
            address.setField(AddressInfo.BUILDING_NAME, building_name);
            address.setField(AddressInfo.BUILDING_FLOOR, building_floor);
            address.setField(AddressInfo.BUILDING_ROOM, building_room);
            address.setField(AddressInfo.BUILDING_ZONE, building_zone);
            address.setField(AddressInfo.CROSSING1, crossing1);
            address.setField(AddressInfo.CROSSING2, crossing2);
            address.setField(AddressInfo.URL, url);
            address.setField(AddressInfo.PHONE_NUMBER, phone_number);

            lm.setAddressInfo(address);
            ls.updateLandmark(lm);

            fetchedLm = findLandmark(ls.getLandmarks(), name);
            checkLandmark(fetchedLm, name, description, coords, address);

            // Finally, remove the landmark
            ls.deleteLandmark(lm);

            assertTrue("", true);
        }
        catch (Exception e)
        {
            assertTrue(e.getMessage(), false);
        }
    }

    // Add a landmark to the LandmarkStore with all the information set and
    // update by removing information.
    public void testUpdateLandmark2()
    {
        try
        {
            // Create a new Landmark object
            String name = "Home";
            String desc = "There is no place like it";
            QualifiedCoordinates coords = new QualifiedCoordinates(-10.2f,
                    -107.8f, -4.5f, 11.9f, 5.1f);
            AddressInfo address = new AddressInfo();
            address.setField(AddressInfo.EXTENSION, extension);
            address.setField(AddressInfo.STREET, street);
            address.setField(AddressInfo.POSTAL_CODE, postal_code);
            address.setField(AddressInfo.CITY, city);
            address.setField(AddressInfo.COUNTY, county);
            address.setField(AddressInfo.DISTRICT, district);
            address.setField(AddressInfo.STATE, state);
            address.setField(AddressInfo.COUNTRY, country);
            address.setField(AddressInfo.COUNTRY_CODE, country_code);
            address.setField(AddressInfo.BUILDING_NAME, building_name);
            address.setField(AddressInfo.BUILDING_FLOOR, building_floor);
            address.setField(AddressInfo.BUILDING_ROOM, building_room);
            address.setField(AddressInfo.BUILDING_ZONE, building_zone);
            address.setField(AddressInfo.CROSSING1, crossing1);
            address.setField(AddressInfo.CROSSING2, crossing2);
            address.setField(AddressInfo.URL, url);
            address.setField(AddressInfo.PHONE_NUMBER, phone_number);
            Landmark lm = new Landmark(name, desc, coords, address);

            ls = getDefaultLandmarkStore();

            // Save the landmark
            ls.addLandmark(lm, iCategory);

            Landmark fetchedLm = null;

            fetchedLm = findLandmark(ls.getLandmarks(), name);
            checkLandmark(fetchedLm, name, desc, coords, address);

            // Remove decription
            lm.setDescription(null);
            ls.updateLandmark(lm);

            fetchedLm = findLandmark(ls.getLandmarks(), name);
            checkLandmark(fetchedLm, name, null, coords, address);

            // Remove QualifiedCoordinates
            lm.setQualifiedCoordinates(null);
            ls.updateLandmark(lm);

            fetchedLm = findLandmark(ls.getLandmarks(), name);
            checkLandmark(fetchedLm, name, null, null, address);

            // Remove AddressInfo
            lm.setAddressInfo(null);
            ls.updateLandmark(lm);

            fetchedLm = findLandmark(ls.getLandmarks(), name);
            checkLandmark(fetchedLm, name, null, null, null);

            // Finally, remove the landmark
            ls.deleteLandmark(lm);
            assertTrue("", true);
        }
        catch (Exception e)
        {
            assertTrue(e.getMessage(), false);
        }
    }

    public void testUpdateLandmarkTruncate()
    {
        try
        {
            ls = getDefaultLandmarkStore();

            String name = "Update with lots of info later";
            Landmark lm = new Landmark(name, null, null, null);
            ls.addLandmark(lm, iCategory);

            String maxDescription = new String(new char[MAX_DESCRIPTION_LENGTH]);
            String tooLongDescription = maxDescription + "could be a lot more";

            String maxAddrField = new String(new char[MAX_ADDRESSFIELD_LENGTH]);
            String tooLongAddrField = maxAddrField + "some more";
            AddressInfo tooLongAddr = new AddressInfo();
            for (int i = 0; i < 17; ++i)
            {
                tooLongAddr.setField(i + 1, tooLongAddrField);
            }

            // Update the landmark with info that has length longer than max
            lm.setDescription(tooLongDescription);
            lm.setAddressInfo(tooLongAddr);
            ls.updateLandmark(lm);

            // All fields should be truncated to max length
            AddressInfo expectedAddr = new AddressInfo();
            for (int i = 0; i < 17; ++i)
            {
                expectedAddr.setField(i + 1, maxAddrField);
            }

            Enumeration e = ls.getLandmarks();
            Landmark fetchedLm = findLandmark(e, name);
            checkLandmark(fetchedLm, name, maxDescription, null, expectedAddr);

            ls.deleteLandmark(lm);

            assertTrue("", true);
        }
        catch (Exception e)
        {
            assertTrue(e.getMessage(), false);
        }
    }

    public void testAddDeleteUpdateLandmark()
    {
        try
        {
            ls = getDefaultLandmarkStore();

            String name = "AddDeleteUpdateLandmark";
            Landmark lm = new Landmark(name, null, null, null);

            // Add, get and delete the landmark
            ls.addLandmark(lm, iCategory);
            Enumeration e = ls.getLandmarks();
            Landmark fetchedLm = findLandmark(e, name);
            ls.deleteLandmark(lm);

            // Try to update the landmark we just deleted
            try
            {
                fetchedLm.setDescription("Description");
                ls.updateLandmark(fetchedLm);
                assertContinue("LandmarkException not thrown", false);
            }
            catch (LandmarkException le)
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

    // List all landmarks.
    public void testGetAllLandmarks()
    {
        try
        {
            final String BASE_NAME = "GetLandmarksTest";
            final int NUM_LANDMARKS = 10;
            String[] names = new String[NUM_LANDMARKS];

            ls = getDefaultLandmarkStore();

            for (int i = 0; i < NUM_LANDMARKS; ++i)
            {
                names[i] = BASE_NAME + i;
                Landmark lm = new Landmark(names[i], null, null, null);
                ls.addLandmark(lm, iCategory);
            }

            // Retrieve the enumeration of landmarks
            Enumeration e = ls.getLandmarks();

            int count = findLandmarks(e, names);
            assertContinue("Wrong number of Landmarks found",
                           count == NUM_LANDMARKS);

            // Delete all the added landmarks
            e = ls.getLandmarks();
            while (e.hasMoreElements())
            {
                ls.deleteLandmark((Landmark) e.nextElement());
            }

            assertTrue("", true);
        }
        catch (Exception e)
        {
            assertTrue(e.getMessage(), false);
        }
    }

    public void testAddCategoryBadArguments()
    {
        try
        {
            ls = getDefaultLandmarkStore();

            try
            {
                ls.addCategory(null); // Add category with null parameter
                assertContinue("NullPointerException not thrown", false);
            }
            catch (NullPointerException npe)
            {
                // Exception was thrown correctly
            }

            try
            {
                ls.addCategory(""); // Add category with empty string parameter
                assertContinue("IllegalArgumentException not thrown", false);
            }
            catch (IllegalArgumentException iae)
            {
                // Exception was thrown correctly
            }

            String category = "Try to add this category twice";
            ls.addCategory(category);

            try
            {
                ls.addCategory(category); // Category already exists
                assertContinue("IllegalArgumentException not thrown", false);
            }
            catch (IllegalArgumentException iae)
            {
                // Exception was thrown correctly
            }

            ls.deleteCategory(category);

            assertTrue("", true);
        }
        catch (Exception e)
        {
            assertTrue(e.getMessage(), false);
        }
    }

    public void testDeleteCategoryBadArguments()
    {
        try
        {
            ls = getDefaultLandmarkStore();

            try
            {
                ls.deleteCategory(null); // Delete category with null
                // parameter
                assertContinue("NullPointerException not thrown", false);
            }
            catch (NullPointerException npe)
            {
                // Exception was thrown correctly
            }

            String category = "Nonexisting category";
            ls.deleteCategory(category); // This should be a silent failure
            ls.deleteCategory(category); // Try again just to make sure :)

            ls.deleteCategory(""); // Empty string should fail silently

            // Silent failure if category name is too long
            ls.deleteCategory(new String(new char[MAX_CATEGORY_LENGTH + 1]));

            assertTrue("", true);
        }
        catch (Exception e)
        {
            assertTrue(e.getMessage(), false);
        }
    }

    // Tests that new categories can be added and deleted.
    public void testAddDeleteCategory()
    {
        try
        {
            String category = "AddCategoryTest Category";

            ls = getDefaultLandmarkStore();

            // Add category
            ls.addCategory(category);

            // Check that the category was added
            Enumeration e = ls.getCategories();
            findCategory(e, category);

            // Delete the category
            ls.deleteCategory(category);

            // Check that category has been removed
            e = ls.getCategories();
            while (e.hasMoreElements())
            {
                String s = (String) e.nextElement();
                assertContinue("Category was not removed", !s.equals(category));
            }

            assertTrue("", true);
        }
        catch (Exception e)
        {
            assertTrue(e.getMessage(), false);
        }
    }

    public void testAddDeleteManyCategories()
    {
        int c = 0;
        try
        {
            // A variety of valid category names
            String[] categories = new String[] { "q", "Dots ...",
                                                 "Exclamation!", "(parentesis)", "percentage%", "hashmark#",
                                                 "at@", "et&", "commas,,,"
                                               };

            ls = getDefaultLandmarkStore();

            // Quick Add and Delete of categories
            for (int i = 0; i < categories.length; ++i)
            {
                ls.addCategory(categories[i]);
                ls.deleteCategory(categories[i]);
            }

            // Add many categories
            String category = "Test_Category_";

            // Add categories
            for (int i = 0; i < 100; ++i)
            {
                ls.addCategory(category + i);
                c++;
            }

            // Delete categories
            for (int i = 0; i < 100; ++i)
            {
                ls.deleteCategory(category + i);
            }

            assertTrue("", true);
        }
        catch (Exception e)
        {
            assertTrue(e.getMessage() + "@" + c, false);
        }
    }

    public void testAddCategoryTruncate()
    {
        try
        {
            ls = getDefaultLandmarkStore();

            String maxCategoryName = new String(new char[MAX_CATEGORY_LENGTH]);

            // Add category with too long name
            String tooLongCategoryName = maxCategoryName + "extra";
            ls.addCategory(tooLongCategoryName);

            // Check that the category was added
            Enumeration e = ls.getCategories();
            // Category name should be truncated to max length
            findCategory(e, maxCategoryName);

            // Try to delete with the too long category name
            ls.deleteCategory(tooLongCategoryName);
            // Make sure the category is still there
            e = ls.getCategories();
            findCategory(e, maxCategoryName);

            // Delete the category
            ls.deleteCategory(maxCategoryName);

            // Check that category has been removed
            e = ls.getCategories();
            while (e.hasMoreElements())
            {
                String s = (String) e.nextElement();
                assertContinue("Category was not removed", !maxCategoryName
                               .equals(s));
            }

            assertTrue("", true);
        }
        catch (Exception e)
        {
            assertTrue(e.getMessage(), false);
        }
    }

    public void testAddLandmarkToCategory1()
    {
        try
        {
            ls = getDefaultLandmarkStore();

            // Create a category
            String category = "ViperTest Sightseeing";
            ls.addCategory(category);

            // Create a new Landmark in the category
            String name = "Eiffel Tower";
            Landmark lm = new Landmark(name, null, null, null);
            ls.addLandmark(lm, category);

            // Get Landmarks by category
            Enumeration e = ls.getLandmarks(category, null);
            int count = findLandmarks(e, new String[] { name });
            assertContinue("Wrong number of Landmarks found", count == 1);

            ls.deleteLandmark(lm);
            ls.deleteCategory(category);

            assertTrue("", true);
        }
        catch (Exception e)
        {
            assertTrue(e.getMessage(), false);
        }
    }

    public void testAddLandmarkToCategory2()
    {
        try
        {
            ls = getDefaultLandmarkStore();

            // Add a new Landmark to default category
            String name = "Liseberg";
            Landmark lm = new Landmark(name, null, null, null);
            ls.addLandmark(lm, null);

            // Create a category
            String category = "Amuzement Parks";
            ls.addCategory(category);

            // Get Landmarks by category
            Enumeration e = ls.getLandmarks(category, null);
            assertContinue("Category should have been empty", e == null);

            // Add the Landmark to the category
            ls.addLandmark(lm, category);

            // Get Landmarks by category
            Enumeration e2 = ls.getLandmarks(category, null);
            int count = findLandmarks(e2, new String[] { name });
            assertContinue("Wrong number of Landmarks found", count == 1);

            ls.deleteLandmark(lm);
            ls.deleteCategory(category);

            assertTrue("", true);
        }
        catch (Exception e)
        {
            assertTrue(e.getMessage(), false);
        }
    }

    public void testAddLandmarkToCategory3()
    {
        try
        {
            ls = getDefaultLandmarkStore();

            // Add category1
            String category1 = "Food";
            ls.addCategory(category1);
            findCategory(ls.getCategories(), category1);

            // Add category2
            String category2 = "Restuarants";
            ls.addCategory(category2);
            findCategory(ls.getCategories(), category2);

            // Create a new Landmark in category1
            String name = "Da Pizza place";
            Landmark lm = new Landmark(name, null, null, null);
            ls.addLandmark(lm, category1);

            // Add the Landmark to category2
            ls.addLandmark(lm, category2);

            // Get Landmarks by category1
            Enumeration e1 = ls.getLandmarks(category1, null);
            int count1 = findLandmarks(e1, new String[] { name });
            assertContinue("Wrong number of Landmarks found", count1 == 1);

            // Get Landmarks by category2
            Enumeration e2 = ls.getLandmarks(category2, null);
            int count2 = findLandmarks(e2, new String[] { name });
            assertContinue("Wrong number of Landmarks found", count2 == 1);

            ls.deleteLandmark(lm);
            ls.deleteCategory(category1);
            ls.deleteCategory(category2);

            assertTrue("", true);
        }
        catch (Exception e)
        {
            assertTrue(e.getMessage(), false);
        }
    }

    public void testAddLandmarkEmptyStrings()
    {
        try
        {
            ls = getDefaultLandmarkStore();

            try
            {
                Landmark lm = new Landmark("name", null, null, null);
                ls.addLandmark(lm, ""); // Try to use empty string for category
                assertContinue("IllegalArgumentException not thrown", false);
            }
            catch (IllegalArgumentException iae)
            {
                // Exception thrown correctly
            }

            // Create AddressInfo with all fields set to empty strings
            AddressInfo addr = new AddressInfo();
            for (int i = 0; i < 17; ++i)
            {
                addr.setField(i + 1, "");
            }

            String name = "";
            String description = " ";

            Landmark lm = new Landmark(name, description, null, addr);
            ls.addLandmark(lm, iCategory); // Should be ok

            // Get Landmarks
            Enumeration e = ls.getLandmarks();
            Landmark fetchedLm = findLandmark(e, name);
            checkLandmark(fetchedLm, name, description, null, addr);

            ls.deleteLandmark(lm);

            assertTrue("", true);
        }
        catch (Exception e)
        {
            assertTrue(e.getMessage(), false);
        }
    }

    public void testAddLandmarkToCategoryDeleteCategory()
    {
        try
        {
            ls = getDefaultLandmarkStore();

            // Create a category
            String category = "Museums";
            ls.addCategory(category);

            // Create a new Landmark in the category
            String name = "Guggenheim";
            Landmark lm = new Landmark(name, null, null, null);
            ls.addLandmark(lm, category);

            // Delete the category
            ls.deleteCategory(category);

            // Get Landmarks by category
            Enumeration e = ls.getLandmarks(null, category);
            assertContinue("Expected getLandmarks to return null", e == null);

            // Make sure Landmark still exists
            Landmark fetchedLm = findLandmark(ls.getLandmarks(), name);
            checkLandmark(fetchedLm, name, null, null, null);

            ls.deleteLandmark(lm);

            assertTrue("", true);
        }
        catch (Exception e)
        {
            assertTrue(e.getMessage(), false);
        }
    }

    public void testAddLandmarkSameNameDifferentCategory()
    {
        try
        {
            ls = getDefaultLandmarkStore();

            // Create categories
            String category1 = "category1";
            String category2 = "category2";
            ls.addCategory(category1);
            ls.addCategory(category2);

            String name = "Same name";

            // Create a new Landmark in category1
            String desc1 = "This landmark has a description";
            Landmark lm1 = new Landmark(name, desc1, null, null);
            ls.addLandmark(lm1, category1);

            // Create a new Landmark in category2
            Landmark lm2 = new Landmark(name, null, null, null);
            ls.addLandmark(lm2, category2);

            // Get Landmarks by category1
            Enumeration e = ls.getLandmarks(category1, null);
            Landmark fetchedLm = findLandmark(e, name);
            checkLandmark(fetchedLm, name, desc1, null, null);

            // Get Landmarks by category2
            e = ls.getLandmarks(category2, null);
            fetchedLm = findLandmark(e, name);
            checkLandmark(fetchedLm, name, null, null, null);

            // Get Landmarks by name
            e = ls.getLandmarks(null, name);
            int count = findLandmarks(e, new String[] { name, name });
            assertContinue("Wrong number of landmarks found", count == 2);

            ls.deleteLandmark(lm1);
            ls.deleteLandmark(lm2);
            ls.deleteCategory(category1);
            ls.deleteCategory(category2);

            assertTrue("", true);
        }
        catch (Exception e)
        {
            assertTrue(e.getMessage(), false);
        }
    }

    public void testRemoveLandmarkFromCategoryBadArguments()
    {
        try
        {
            ls = getDefaultLandmarkStore();

            String category1 = "Cat1";
            String category2 = "Cat2";
            ls.addCategory(category1);
            ls.addCategory(category2);

            // Add a landmark to category1
            Landmark lm = new Landmark("name", null, null, null);
            ls.addLandmark(lm, category1);

            try
            {
                // Remove landmark from category with null Landmark parameter
                ls.removeLandmarkFromCategory(null, category1);
                assertContinue("NullPointerException not thrown", false);
            }
            catch (NullPointerException npe)
            {
                // Exception was thrown correctly
            }

            try
            {
                // Remove landmark from category with null category parameter
                ls.removeLandmarkFromCategory(lm, null);
                assertContinue("NullPointerException not thrown", false);
            }
            catch (NullPointerException npe)
            {
                // Exception was thrown correctly
            }

            // Silent failure if Landmark does not belong to category
            ls.removeLandmarkFromCategory(lm, category2);

            // Silent failure if category does not exist
            ls.removeLandmarkFromCategory(lm, "NonexistingCategory");

            // Silent failure if category is empty string
            ls.removeLandmarkFromCategory(lm, "");

            // Silent failure if category name is too long
            String tooLongCategory = new String(
                new char[MAX_CATEGORY_LENGTH + 1]);
            ls.removeLandmarkFromCategory(lm, tooLongCategory);

            // Silent failure if Landmark does not belong to LandmarkStore
            Landmark newLm = new Landmark("Not added", null, null, null);
            ls.removeLandmarkFromCategory(newLm, category1);

            ls.deleteLandmark(lm);
            ls.deleteCategory(category1);
            ls.deleteCategory(category2);

            assertTrue("", true);
        }
        catch (Exception e)
        {
            assertTrue(e.getMessage(), false);
        }
    }

    public void testRemoveLandmarkFromCategory()
    {
        try
        {
            String category = "Category";
            String name = "lm";
            Landmark lm = new Landmark(name, null, null, null);

            ls = getDefaultLandmarkStore();
            ls.addCategory(category);
            ls.addLandmark(lm, category);

            Enumeration e = ls.getLandmarks(category, name);
            Landmark fetchedLm = findLandmark(e, name);
            checkLandmark(fetchedLm, name, null, null, null);

            ls.removeLandmarkFromCategory(lm, category);

            Enumeration e2 = ls.getLandmarks(category, name);
            assertContinue("Landmark not removed from category", e2 == null);

            // Delete the category and Landmark from the store
            ls.deleteCategory(category);
            ls.deleteLandmark(lm);

            assertTrue("", true);
        }
        catch (Exception e)
        {
            assertTrue(e.getMessage(), false);
        }
    }

    public void testRemoveLandmarkFromCategory2()
    {
        try
        {
            ls = getDefaultLandmarkStore();

            // Add categories
            String category1 = "category1";
            String category2 = "category2";
            ls.addCategory(category1);
            ls.addCategory(category2);

            String name = "lm";
            Landmark lm = new Landmark(name, null, null, null);

            // Add landmark to category1 and category2
            ls.addLandmark(lm, category1);
            ls.addLandmark(lm, category2);

            // Remove landmark from category1
            ls.removeLandmarkFromCategory(lm, category1);

            Enumeration e = ls.getLandmarks(category1, name);
            assertContinue("Landmark not removed from category", e == null);

            // Make sure landmark still belongs to category2
            Enumeration e2 = ls.getLandmarks(category2, name);
            Landmark fetchedLm = findLandmark(e2, name);
            checkLandmark(fetchedLm, name, null, null, null);

            // Remove landmark from category2
            ls.removeLandmarkFromCategory(lm, category2);

            Enumeration e3 = ls.getLandmarks(category2, name);
            assertContinue("Landmark not removed from category", e3 == null);

            // Delete the categories and Landmark from the store
            ls.deleteCategory(category1);
            ls.deleteCategory(category2);
            ls.deleteLandmark(lm);

            assertTrue("", true);
        }
        catch (Exception e)
        {
            assertTrue(e.getMessage(), false);
        }
    }

    public void testGetLandmarksByNameBadArguments()
    {
        try
        {
            String maxName = new String(new char[MAX_NAME_LENGTH]);

            ls = getDefaultLandmarkStore();
            Landmark lm = new Landmark(maxName, null, null, null);
            ls.addLandmark(lm, null);

            // Sanity check, find landmark by name
            Enumeration e = ls.getLandmarks(null, maxName);
            int count = findLandmarks(e, new String[] { maxName });
            assertContinue("Wrong number of landmarks found", count == 1);

            // Search with too long name
            e = ls.getLandmarks(null, maxName + "x");
            assertContinue("Expected getLandmarks to return null (1)",
                           e == null);

            // Search with empty string for name
            e = ls.getLandmarks(null, "");
            assertContinue("Expected getLandmarks to return null (2)",
                           e == null);

            ls.deleteLandmark(lm);

            assertTrue("", true);
        }
        catch (Exception e)
        {
            assertTrue(e.getMessage(), false);
        }
    }

    public void testGetLandmarksByName()
    {
        try
        {

            ls = getDefaultLandmarkStore();

            addLandmarksAndCategoriesForSearch();

            Enumeration e;
            int count;

            // Search for landmark that does not exist
            e = ls.getLandmarks(null, searchName0);
            assertContinue("Expected getLandmarks to return null", e == null);

            // Search for landmarks 1
            e = ls.getLandmarks(null, searchName1);
            count = findLandmarks(e, new String[] { searchName1 });
            assertContinue("Wrong number of landmarks found", count == 1);

            // Search for landmarks 2
            e = ls.getLandmarks(null, searchName2);
            count = findLandmarks(e, new String[] { searchName2, searchName2 });
            assertContinue("Wrong number of landmarks found", count == 2);

            // Search for landmarks 3
            e = ls.getLandmarks(null, searchName3);
            count = findLandmarks(e, new String[] { searchName3, searchName3,
                                                    searchName3
                                                  });
            assertContinue("Wrong number of landmarks found", count == 3);

            deleteAllLandmarksAndCategories();

            assertTrue("", true);
        }
        catch (Exception e)
        {
            assertTrue(e.getMessage(), false);
        }
    }

    public void testGetLandmarksByNameUncategorized()
    {
        try
        {
            ls = getDefaultLandmarkStore();

            addLandmarksAndCategoriesForSearch();

            Enumeration e;
            int count;

            // Find all landmarks that do not belong to a category
            e = ls.getLandmarks("", null);
            count = findLandmarks(e, new String[] { searchName1, searchName2 });
            assertContinue("Wrong number of landmarks found", count == 2);

            // Search for landmarks 1
            e = ls.getLandmarks("", searchName1);
            count = findLandmarks(e, new String[] { searchName1 });
            assertContinue("Wrong number of landmarks found", count == 1);

            // Search for landmarks 3
            e = ls.getLandmarks("", searchName3);
            assertContinue("Expected getLandmarks to return null", e == null);

            deleteAllLandmarksAndCategories();

            assertTrue("", true);
        }
        catch (Exception e)
        {
            assertTrue(e.getMessage(), false);
        }
    }

    public void testGetLandmarksByNameWildcards()
    {
        try
        {
            ls = getDefaultLandmarkStore();

            String[] names = { "Hepa", "Huupa", "Lupa", "upo", "upa", "Lu?a",
                               "*"
                             };
            int length = names.length;
            Landmark[] lms = new Landmark[length];

            for (int i = 0; i < length; ++i)
            {
                lms[i] = new Landmark(names[i], null, null, null);
                ls.addLandmark(lms[i], null);
            }

            searchAndMatch("H*", new String[] { "Hepa", "Huupa" });
            searchAndMatch("*o", new String[] { "upo" });
            searchAndMatch("*upa", new String[] { "Huupa", "Lupa", "upa" });
            searchAndMatch("L*a", new String[] { "Lupa", "Lu?a" });
            searchAndMatch("*e*", new String[] { "Hepa" });

            searchAndMatch("?", new String[] { "*" });
            searchAndMatch("up?", new String[] { "upa", "upo" });
            searchAndMatch("?upa", new String[] { "Lupa" });
            searchAndMatch("H?pa", new String[] { "Hepa" });
            searchAndMatch("?u??", new String[] { "Lupa", "Lu?a" });
            searchAndMatch("???", new String[] { "upo", "upa" });
            searchAndMatch("?????", new String[] { "Huupa" });

            // Some complicated searches
            searchAndMatch("*??pa", new String[] { "Hepa", "Huupa", "Lupa" });
            searchAndMatch("*u?a", new String[] { "Huupa", "Lupa", "upa",
                                                  "Lu?a"
                                                });
            searchAndMatch("*up?",
                           new String[] { "Huupa", "Lupa", "upo", "upa" });
            searchAndMatch("?*up?", new String[] { "Huupa", "Lupa" });
            searchAndMatch("*?*o*", new String[] { "upo" });

            // Find all landmarks
            searchAndMatch("*", names);
            searchAndMatch("?*", names);
            searchAndMatch("*?", names);
            searchAndMatch("*?*", names);

            for (int i = 0; i < length; ++i)
            {
                ls.deleteLandmark(lms[i]);
            }

            assertTrue("", true);
        }
        catch (Exception e)
        {
            assertTrue(e.getMessage(), false);
        }
    }

    public void testGetLandmarksByCategoryBadArguments()
    {
        try
        {
            ls = getDefaultLandmarkStore();

            String maxCategoryName = new String(new char[MAX_CATEGORY_LENGTH]);
            ls.addCategory(maxCategoryName);

            String name = "Wherever";
            Landmark lm = new Landmark(name, null, null, null);
            ls.addLandmark(lm, maxCategoryName);

            // Sanity check, find landmark by category
            Enumeration e = ls.getLandmarks(maxCategoryName, null);
            int count = findLandmarks(e, new String[] { name });
            assertContinue("Wrong number of landmarks found", count == 1);

            // Search with too long category name
            e = ls.getLandmarks(maxCategoryName + "x", null);
            assertContinue("Expected getLandmarks to return null (1)",
                           e == null);

            // Search with empty string for category name
            e = ls.getLandmarks("", null);
            assertContinue("Expected getLandmarks to return null (2)",
                           e == null);

            ls.deleteLandmark(lm);
            ls.deleteCategory(maxCategoryName);

            assertTrue("", true);
        }
        catch (Exception e)
        {
            assertTrue(e.getMessage(), false);
        }
    }

    public void testGetLandmarksByCategory()
    {
        try
        {
            ls = getDefaultLandmarkStore();

            addLandmarksAndCategoriesForSearch();

            Enumeration e;
            int count;

            // Search for category that does not exist
            e = ls.getLandmarks(searchCategory0, null);
            assertContinue("Expected getLandmarks to return null", e == null);

            // Search for landmarks 1
            e = ls.getLandmarks(searchCategory1, null);
            count = findLandmarks(e, new String[] { searchName3 });
            assertContinue("Wrong number of landmarks found", count == 1);

            // Search for landmarks 2
            e = ls.getLandmarks(searchCategory2, null);
            count = findLandmarks(e, new String[] { searchName2, searchName3,
                                                    searchName3
                                                  });
            assertContinue("Wrong number of landmarks found", count == 3);

            // Delete the categories and Landmarks from the store
            deleteAllLandmarksAndCategories();
            assertTrue("", true);
        }
        catch (Exception e)
        {
            assertTrue(e.getMessage(), false);
        }
    }

    public void testGetLandmarksByNameAndCategory()
    {
        try
        {
            ls = getDefaultLandmarkStore();

            addLandmarksAndCategoriesForSearch();

            Enumeration e;
            int count;

            // Search with a category that does not exist
            e = ls.getLandmarks(searchCategory0, searchName1);
            assertContinue("Expected getLandmarks to return null", e == null);

            // Search with a name that does not exist
            e = ls.getLandmarks(searchCategory2, searchName0);
            assertContinue("Expected getLandmarks to return null", e == null);

            // Search with a name that does not exist in the category
            e = ls.getLandmarks(searchCategory1, searchName1);
            assertContinue("Expected getLandmarks to return null", e == null);

            // Search for landmarks 1
            e = ls.getLandmarks(searchCategory1, searchName3);
            count = findLandmarks(e, new String[] { searchName3 });
            assertContinue("Wrong number of landmarks found", count == 1);

            // Search for landmarks 2
            e = ls.getLandmarks(searchCategory2, searchName3);
            count = findLandmarks(e, new String[] { searchName3, searchName3 });
            assertContinue("Wrong number of landmarks found", count == 2);

            // Search for landmarks 3
            e = ls.getLandmarks(searchCategory2, searchName2);
            Landmark fetchedLm = findLandmark(e, searchName2);
            checkLandmark(fetchedLm, searchName2, searchCategory2, null, null);

            // Delete the categories and Landmarks from the store
            deleteAllLandmarksAndCategories();
            assertTrue("", true);
        }
        catch (Exception e)
        {
            assertTrue(e.getMessage(), false);
        }
    }

    public void testGetLandmarksByAreaBadArguments()
    {

        try
        {
            ls = getDefaultLandmarkStore();

            double minLat = -10.0d;
            double maxLat = 10.0d;
            double minLon = -110.0d;
            double maxLon = 110.0d;

            // Bad minLat values
            assertBadGetLandmarksByArea(-90.1d, maxLat, minLon, maxLon);
            assertBadGetLandmarksByArea(90.1d, maxLat, minLon, maxLon);

            // Bad maxLat values
            assertBadGetLandmarksByArea(minLat, -90.1d, minLon, maxLon);
            assertBadGetLandmarksByArea(minLat, 90.1d, minLon, maxLon);

            // minLat > maxLat
            assertBadGetLandmarksByArea(40.0d, 30.0d, minLon, maxLon);

            // Bad minLon values
            assertBadGetLandmarksByArea(minLat, maxLat, -180.1d, maxLon);
            assertBadGetLandmarksByArea(minLat, maxLat, 180.0d, maxLon);

            // Bad maxLon values
            assertBadGetLandmarksByArea(minLat, maxLat, minLon, -180.1d);
            assertBadGetLandmarksByArea(minLat, maxLat, minLon, 180.0d);

            try
            {
                Enumeration e = ls.getLandmarks(null, minLat, minLon, maxLat,
                                                maxLon);
                assertContinue(
                    "IllegalArgumentException not thrown for long category name",
                    false);
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

    public void testGetLandmarksByArea()
    {
        try
        {
            ls = getDefaultLandmarkStore();

            addLandmarksAndCategoriesForSearch();

            // -180 0 180
            // |---------|- 90 1 = searchName1
            // | | | 2 = searchName2
            // | 1 | | 3 = searchName3, searchCategory1
            // |----+----|- 0 4 = searchName3, searchCategory2
            // |2 |4 5| 5 = searchName3, searchCategory2
            // | 3 | |
            // |---------|- -90

            Enumeration e;
            int count;

            // Search an area where there are no landmarks
            e = ls.getLandmarks(null, 0.0d, 90.0d, 0.0d, 179.9d);
            assertContinue("Expected getLandmarks to return null", e == null);

            // Search for landmarks 1
            e = ls.getLandmarks(null, 0.0d, 90.0d, -180.0d, 0.0d);
            count = findLandmarks(e, new String[] { searchName1 });
            assertContinue("Wrong number of landmarks found", count == 1);

            // Search for landmarks 2
            e = ls.getLandmarks(null, -90.0d, 0.0d, -180.0d, 0.0d);
            count = findLandmarks(e, new String[] { searchName2, searchName3 });
            assertContinue("Wrong number of landmarks found", count == 2);

            // Search for landmarks 3
            e = ls.getLandmarks(null, -90.0d, 0.0d, 0.0d, 179.9d);
            count = findLandmarks(e, new String[] { searchName3, searchName3 });
            assertContinue("Wrong number of landmarks found", count == 2);

            // Search for landmarks - find lm2, smallest possible area
            e = ls.getLandmarks(null, -5.0d, -5.0d, -180.0d, -180.0d);
            count = findLandmarks(e, new String[] { searchName2 });
            assertContinue("Wrong number of landmarks found", count == 1);

            // Search for landmarks - search lon [-180, 0] and [100, 180)
            e = ls.getLandmarks(null, -90.0d, 90.0d, 100.0d, 0.0d);
            count = findLandmarks(e, new String[] { searchName1, searchName2,
                                                    searchName3, searchName3
                                                  });
            assertContinue("Wrong number of landmarks found", count == 4);

            deleteAllLandmarksAndCategories();

            assertTrue("", true);
        }
        catch (Exception e)
        {
            assertTrue(e.getMessage(), false);
        }
    }

    public void testGetLandmarksByArea2()
    {
        try
        {
            ls = getDefaultLandmarkStore();
            Landmark lm1 = new Landmark("Landmark1", "description",
                                        new QualifiedCoordinates(10.0, 20.0, Float.NaN, Float.NaN,
                                                                 Float.NaN), null);
            ls.addLandmark(lm1, null);
            ls.addLandmark(new Landmark("Landmark2", null,
                                        new QualifiedCoordinates(40.0, 30.0, 0, 0, 0), null), null);
            ls
            .addLandmark(new Landmark("Landmark3", null,
                                      new QualifiedCoordinates(30.0, -10.0, 0, 0, 0),
                                      null), null);
            ls
            .addLandmark(new Landmark("Landmark4", null,
                                      new QualifiedCoordinates(30.0, 165.0, 0, 0, 0),
                                      null), null);
            ls.addLandmark(new Landmark("Landmark5", null,
                                        new QualifiedCoordinates(30.0, -165.0, 0, 0, 0), null),
                           null);

            Enumeration e = ls.getLandmarks(null, -30.0, 50.0, 5.0, 25.0);
            Landmark gotLandmark = findLandmark(e, "Landmark1");
            checkLandmark(gotLandmark, lm1.getName(), lm1.getDescription(), lm1
                          .getQualifiedCoordinates(), lm1.getAddressInfo());
            assertContinue("Wrong number of landmarks found", !e
                           .hasMoreElements());

            deleteAllLandmarksAndCategories();

            assertTrue("", true);
        }
        catch (Exception e)
        {
            assertTrue(e.getMessage(), false);
        }
    }

    public void testGetLandmarksByAreaAndCategory()
    {
        try
        {
            ls = getDefaultLandmarkStore();

            addLandmarksAndCategoriesForSearch();

            // -180 0 180
            // |---------|- 90 1 = searchName1
            // | | | 2 = searchName2
            // | 1 | | 3 = searchName3, searchCategory1
            // |----+----|- 0 4 = searchName3, searchCategory2
            // |2 |4 5| 5 = searchName3, searchCategory2
            // | 3 | |
            // |---------|- -90

            Enumeration e;
            int count;

            // Search a category that does not contain any landmarks
            e = ls
                .getLandmarks(searchCategory0, -90.0d, 90.0d, -180.0d,
                              179.9d);
            assertContinue("Expected getLandmarks to return null", e == null);

            // Search an area that does not contain any landmarks in the
            // category
            e = ls.getLandmarks(searchCategory2, -90.0d, 90.0d, -180.0d, 0.0d);
            assertContinue("Expected getLandmarks to return null", e == null);

            // Search for landmarks 1
            e = ls
                .getLandmarks(searchCategory1, -90.0d, 90.0d, -180.0d,
                              179.9d);
            count = findLandmarks(e, new String[] { searchName3 });
            assertContinue("Wrong number of landmarks found", count == 1);

            // Search for landmarks 2
            e = ls.getLandmarks(searchCategory2, -90.0d, 0.0d, -180.0d, 179.9d);
            count = findLandmarks(e, new String[] { searchName3, searchName3 });
            assertContinue("Wrong number of landmarks found", count == 2);

            // Search for landmarks 3
            e = ls.getLandmarks(searchCategory2, -90.0d, 0.0d, 100.0d, 179.9d);
            count = findLandmarks(e, new String[] { searchName3 });
            assertContinue("Wrong number of landmarks found", count == 1);

            // Search for landmarks 4 - smallest possible search area
            e = ls
                .getLandmarks(searchCategory1, -90.0d, -90.0d, -21.0d,
                              -21.0d);
            count = findLandmarks(e, new String[] { searchName3 });
            assertContinue("Wrong number of landmarks found", count == 1);

            deleteAllLandmarksAndCategories();

            assertTrue("", true);
        }
        catch (Exception e)
        {
            assertTrue(e.getMessage(), false);
        }
    }

    public void testEnumerationDeleteLandmark1()
    {
        try
        {
            ls = getDefaultLandmarkStore();

            Landmark lm1 = new Landmark("lm1", null, null, null);
            Landmark lm2 = new Landmark("lm2", null, null, null);
            Landmark lm3 = new Landmark("lm3", null, null, null);

            ls.addLandmark(lm1, null);
            ls.addLandmark(lm2, null);
            ls.addLandmark(lm3, null);

            Enumeration e = ls.getLandmarks();
            assertContinue("Enumeration is empty", e.hasMoreElements());
            ls.deleteLandmark(lm1);
            ls.deleteLandmark(lm2);

            // Expect all landmarks since the enumeration should remain
            // unchanged
            int count = findLandmarks(e, new String[] { "lm1", "lm2", "lm3" });
            assertContinue("Wrong number of landmarks found", count == 3);

            ls.deleteLandmark(lm3);

            assertTrue("", true);
        }
        catch (Exception e)
        {
            assertTrue(e.getMessage(), false);
        }
    }

    public void testEnumerationDeleteLandmark2()
    {
        try
        {
            ls = getDefaultLandmarkStore();

            Landmark lm1 = new Landmark("lm1", null, null, null);
            Landmark lm2 = new Landmark("lm2", null, null, null);
            Landmark lm3 = new Landmark("lm3", null, null, null);

            ls.addLandmark(lm1, null);
            ls.addLandmark(lm2, null);
            ls.addLandmark(lm3, null);

            Enumeration e = ls.getLandmarks();
            ls.deleteLandmark(lm1);
            ls.deleteLandmark(lm2);
            ls.deleteLandmark(lm3);

            assertContinue("Landmark lm1 was not found!", findLandmarks(e,
                           new String[] { "lm1", "lm2", "lm3" }) == 3);
            assertContinue("Store is not empty!", ls.getLandmarks() == null);
        }
        catch (Exception e)
        {
            assertTrue(e.getMessage(), false);
        }
    }

    public void testGetInstance2()
    {
        try
        {
            String[] stores = LandmarkStore.listLandmarkStores();
            if (stores != null)
            {
                for (int i = 0; i < stores.length; i++)
                {
                    LandmarkStore.deleteLandmarkStore(stores[i]);
                }
            }
            for (int i = 0; i < 10; i++)
            {
                LandmarkStore.createLandmarkStore("mystore" + i);
            }

            stores = LandmarkStore.listLandmarkStores();
            for (int i = 0; i < stores.length; i++)
            {
                LandmarkStore.getInstance(stores[i]);
            }

            for (int i = 0; i < stores.length; i++)
            {
                LandmarkStore.deleteLandmarkStore(stores[i]);
            }
        }
        catch (Exception e)
        {
            assertTrue("Exception thrown when testing getInstance() !!!", false);
        }
    }

    public void testGetInstanceWithGc()
    {
        try
        {
            String[] stores = LandmarkStore.listLandmarkStores();
            if (stores != null)
            {
                for (int i = 0; i < stores.length; i++)
                {
                    LandmarkStore.deleteLandmarkStore(stores[i]);
                }
            }
            for (int i = 0; i < 10; i++)
            {
                LandmarkStore.createLandmarkStore("mystore" + i);
            }

            stores = LandmarkStore.listLandmarkStores();
            for (int i = 0; i < stores.length; i++)
            {
                LandmarkStore.getInstance(stores[i]);
            }

            for (int i = 0; i < stores.length; i++)
            {
                LandmarkStore.deleteLandmarkStore(stores[i]);
            }

            for (int i = 0; i < 10; i++)
            {
                LandmarkStore.createLandmarkStore("mystore" + i);
                System.gc(); // collect some garbages...
            }

            // Cleanup
            stores = LandmarkStore.listLandmarkStores();
            for (int i = 0; i < stores.length; i++)
            {
                LandmarkStore.deleteLandmarkStore(stores[i]);
            }
        }
        catch (Exception e)
        {
            assertTrue("Exception thrown when testing getInstance() !!!", false);
        }
    }

    public void testCreateLandmarkStore1()
    {
        try
        {
            String[] stores = LandmarkStore.listLandmarkStores();
            if (stores != null)
            {
                for (int i = 0; i < stores.length; i++)
                {
                    LandmarkStore.deleteLandmarkStore(stores[i]);
                }
            }

            try
            {
                // Check for '\' character
                java.lang.Character ch = new java.lang.Character((char) 0x005C);
                LandmarkStore.createLandmarkStore("mystore" + ch.charValue());
            }
            catch (IllegalArgumentException iae)
            {
                // Exception was thrown correctly
            }
            try
            {
                // Check for '/' character
                LandmarkStore.createLandmarkStore("mystore/");
            }
            catch (IllegalArgumentException iae)
            {
                // Exception was thrown correctly
            }
            try
            {
                // Check for ':' character
                LandmarkStore.createLandmarkStore("mystore:");
            }
            catch (IllegalArgumentException iae)
            {
                // Exception was thrown correctly
            }
            try
            {
                // Check for '*' character
                LandmarkStore.createLandmarkStore("mystore*");
            }
            catch (IllegalArgumentException iae)
            {
                // Exception was thrown correctly
            }
            try
            {
                // Check for '?' character
                LandmarkStore.createLandmarkStore("mystore?");
            }
            catch (IllegalArgumentException iae)
            {
                // Exception was thrown correctly
            }
            try
            {
                // Check for '|' character
                LandmarkStore.createLandmarkStore("mystore" + '"');
            }
            catch (IllegalArgumentException iae)
            {
                // Exception was thrown correctly
            }
            try
            {
                // Check for '<' character
                LandmarkStore.createLandmarkStore("mystore<");
            }
            catch (IllegalArgumentException iae)
            {
                // Exception was thrown correctly
            }
            try
            {
                // Check for '>' character
                LandmarkStore.createLandmarkStore("mystore>");
            }
            catch (IllegalArgumentException iae)
            {
                // Exception was thrown correctly
            }
            try
            {
                // Check for '|' character
                LandmarkStore.createLandmarkStore("mystore|");
            }
            catch (IllegalArgumentException iae)
            {
                // Exception was thrown correctly
            }
            try
            {
                // Check for Byte-order-mark character
                java.lang.Character ch = new java.lang.Character((char) 0xFEFF);
                LandmarkStore.createLandmarkStore("mystore" + ch.charValue());
            }
            catch (IllegalArgumentException iae)
            {
                // Exception was thrown correctly
            }

            // Cleanup
            stores = LandmarkStore.listLandmarkStores();
            if (stores != null)
            {
                for (int i = 0; i < stores.length; i++)
                {
                    LandmarkStore.deleteLandmarkStore(stores[i]);
                }
            }
        }
        catch (Exception e)
        {
            assertTrue("Exception thrown when testing createLandmarkStore !!!",
                       false);

        }

        assertTrue("", true);
    }

    // 0x0000...0x001F control characters are not allowed
    public void testCreateLandmarkStore2()
    {
        try
        {
            String[] stores = LandmarkStore.listLandmarkStores();
            if (stores != null)
            {
                for (int i = 0; i < stores.length; i++)
                {
                    LandmarkStore.deleteLandmarkStore(stores[i]);
                }
            }

            for (int i = 0x0000; i <= 0x001F; i++)
            {
                java.lang.Character ch = new java.lang.Character((char) i);
                try
                {
                    LandmarkStore.createLandmarkStore("mystore"
                                                      + ch.charValue());
                }
                catch (IllegalArgumentException e)
                {
                    // Exception was thrown correctly
                }
            }

            // Cleanup
            stores = LandmarkStore.listLandmarkStores();
            if (stores != null)
            {
                for (int i = 0; i < stores.length; i++)
                {
                    LandmarkStore.deleteLandmarkStore(stores[i]);
                }
            }
        }
        catch (Exception e)
        {
            assertTrue("Exception thrown when testing createLandmarkStore !!!",
                       false);
        }

        assertTrue("", true);
    }

    // 0x007F...0x009F control characters are not allowed
    public void testCreateLandmarkStore3()
    {
        try
        {
            String[] stores = LandmarkStore.listLandmarkStores();
            if (stores != null)
            {
                for (int i = 0; i < stores.length; i++)
                {
                    LandmarkStore.deleteLandmarkStore(stores[i]);
                }
            }

            for (int i = 0x007F; i <= 0x009F; i++)
            {
                java.lang.Character ch = new java.lang.Character((char) i);
                try
                {
                    LandmarkStore.createLandmarkStore("mystore"
                                                      + ch.charValue());
                }
                catch (IllegalArgumentException e)
                {
                    // Exception was thrown correctly
                }
            }

            // Cleanup
            stores = LandmarkStore.listLandmarkStores();
            if (stores != null)
            {
                for (int i = 0; i < stores.length; i++)
                {
                    LandmarkStore.deleteLandmarkStore(stores[i]);
                }
            }
        }
        catch (Exception e)
        {
            assertTrue("Exception thrown when testing createLandmarkStore !!!",
                       false);
        }

        assertTrue("", true);
    }

    // 0xFFF0...0xFFFF are not allowed
    public void testCreateLandmarkStore4()
    {
        try
        {
            String[] stores = LandmarkStore.listLandmarkStores();
            if (stores != null)
            {
                for (int i = 0; i < stores.length; i++)
                {
                    LandmarkStore.deleteLandmarkStore(stores[i]);
                }
            }

            for (int i = 0xFFF0; i <= 0xFFFF; i++)
            {
                java.lang.Character ch = new java.lang.Character((char) i);
                try
                {
                    LandmarkStore.createLandmarkStore("mystore"
                                                      + ch.charValue());
                }
                catch (IllegalArgumentException e)
                {
                    // Exception was thrown correctly
                }
            }

            // Cleanup
            stores = LandmarkStore.listLandmarkStores();
            if (stores != null)
            {
                for (int i = 0; i < stores.length; i++)
                {
                    LandmarkStore.deleteLandmarkStore(stores[i]);
                }
            }
        }
        catch (Exception e)
        {
            assertTrue("Exception thrown when testing createLandmarkStore !!!",
                       false);
        }

        assertTrue("", true);
    }

    //------------------------ Helper methods -----------------------

    private void searchAndMatch(String aPattern, String[] aNames)
    throws Exception
    {

        Enumeration e = ls.getLandmarks(null, aPattern);
        int count = findLandmarks(e, aNames);
        assertContinue("Wrong number of landmarks found",
                       count == aNames.length);
    }

    // Add landmarks for testing getLandmarks
    private void addLandmarksAndCategoriesForSearch() throws Exception
    {
        // -180   0   180
        //   |---------|-  90        1 = searchName1
        //   |    |    |             2 = searchName2
        //   |  1 |    |             3 = searchName3, searchCategory1
        //   |----+----|-  0         4 = searchName3, searchCategory2
        //   |2   |4  5|             5 = searchName3, searchCategory2
        //   |  3 |    |
        //   |---------|- -90

        float alt = 0.0f;
        float hacc = 100.0f;
        float vacc = 100.0f;

        QualifiedCoordinates qc1 = new QualifiedCoordinates(27.0d, -17.0d, alt,
                hacc, vacc);
        QualifiedCoordinates qc2 = new QualifiedCoordinates(-5.0d, -180.0d,
                alt, hacc, vacc);
        QualifiedCoordinates qc3 = new QualifiedCoordinates(-90.0d, -21.0d,
                alt, hacc, vacc);
        QualifiedCoordinates qc4 = new QualifiedCoordinates(-5.0d, 5.0d, alt,
                hacc, vacc);
        QualifiedCoordinates qc5 = new QualifiedCoordinates(-5.0d, 179.0d, alt,
                hacc, vacc);

        // |     Category    | Landmarks
        // |-----------------|--------------------------------------
        // |       null      | searchName1, searchName2
        // | searchCategory1 | searchName3
        // | searchCategory2 | searchName2, searchName3, searchName3

        // Add categories
        ls.addCategory(searchCategory1);
        ls.addCategory(searchCategory2);

        // Add landmarks to default category
        Landmark lm1 = new Landmark(searchName1, null, qc1, null);
        Landmark lm2 = new Landmark(searchName2, null, qc2, null);
        ls.addLandmark(lm1, null);
        ls.addLandmark(lm2, null);

        // Add landmarks to searchCategory1 (description = category)
        Landmark lm3 = new Landmark(searchName3, searchCategory1, qc3, null);
        ls.addLandmark(lm3, searchCategory1);

        // Add landmark to searchCategory2 (description = category)
        Landmark lm4 = new Landmark(searchName3, searchCategory2, qc4, null);
        Landmark lm5 = new Landmark(searchName3, searchCategory2, qc5, null);
        Landmark lm6 = new Landmark(searchName2, searchCategory2, null, null);
        ls.addLandmark(lm4, searchCategory2);
        ls.addLandmark(lm5, searchCategory2);
        ls.addLandmark(lm6, searchCategory2);
    }

    private LandmarkStore getDefaultLandmarkStore()
    {
        LandmarkStore store = LandmarkStore.getInstance(null);
        assertContinue("LandmarkStore is null", store != null);
        return store;
    }

    private void assertEmptyLandmarkStore()
    {
        try
        {
            ls = getDefaultLandmarkStore();
            Enumeration le = ls.getLandmarks();
            assertContinue("Landmarks exist in LandmarkStore", le == null);

            Enumeration ce = ls.getCategories();
            assertContinue("Categories exist in LandmarkStore", !ce
                           .hasMoreElements());
            assertTrue("", true);
        }
        catch (Exception e)
        {
            assertTrue(e.getMessage(), false);
        }
    }

    private void saveRetrieveDelete(Landmark aLm) throws Exception
    {
        String name = aLm.getName();
        String description = aLm.getDescription();
        QualifiedCoordinates coords = aLm.getQualifiedCoordinates();
        AddressInfo address = aLm.getAddressInfo();

        // Get the LandmarkStore
        ls = LandmarkStore.getInstance(null);
        assertContinue("LandmarkStore is null", ls != null);

        // Save the landmark
        ls.addLandmark(aLm, iCategory);

        // Get all landmarks and search for the one we added
        Enumeration e = ls.getLandmarks();
        Landmark fetchedLm = findLandmark(e, name);

        ls.deleteLandmark(aLm);
        assertContinue("No landmarks found in database", e != null);
        checkLandmark(fetchedLm, name, description, coords, address);

        ls.deleteLandmark(aLm); // Try to delete again, should be silently ignored
    }

    private Landmark findLandmark(Enumeration e, String aName)
    {
        Landmark foundLm = null;

        if (e != null)
        {
            // Search for the saved one
            while (e.hasMoreElements())
            {
                Landmark aLandmark = (Landmark) e.nextElement();
                assertContinue("Enumeration contains a null", aLandmark != null);

                if (aName.equals(aLandmark.getName()))
                {
                    assertContinue("More that one landmark called " + aName,
                                   foundLm == null);
                    foundLm = aLandmark;
                    break;
                }
            }
        }

        return foundLm;
    }

    // Returns number of Landmarks in the enumeration
    // Also checks that Landmarks with correct names are found
    private int findLandmarks(Enumeration e, String[] aNames)
    {
        int length = aNames != null ? aNames.length : 0;

        boolean[] found = new boolean[length];
        int numLandmarks = 0;

        if (e != null)
        {
            // Search for the saved one
            while (e.hasMoreElements())
            {
                Landmark aLandmark = (Landmark) e.nextElement();
                assertContinue("Enumeration contains a null", aLandmark != null);
                ++numLandmarks;

                for (int i = 0; i < length; ++i)
                {
                    if (aNames[i].equals(aLandmark.getName()))
                    {
                        found[i] = true;
                    }
                }
            }
        }

        for (int i = 0; i < length; ++i)
        {
            assertContinue("Landmark '" + aNames[i] + "' not found", found[i]);
        }

        return numLandmarks;
    }

    private void findCategory(Enumeration e, String aCategory)
    {
        assertContinue("Categories enumeration is null", e != null);
        assertContinue("Categories enumeration is empty", e.hasMoreElements());

        // Check that both category is present
        boolean found = false;
        while (e.hasMoreElements())
        {
            String s = (String) e.nextElement();
            if (aCategory.equals(s))
            {
                found = true;
            }
        }

        assertContinue("Category " + aCategory + " not found", found);
    }

    private void assertEqual(String errorMsg, String dis, String dat)
    {
        if (dis == null)
        {
            assertContinue(errorMsg + ": " + dat + " != null", dat == null);
        }
        else
        {
            assertContinue(errorMsg + ": " + dis + " != " + dat, dis
                           .equals(dat));
        }
    }

    private void assertBadGetLandmarksByArea(double aMinLat, double aMaxLat,
            double aMinLon, double aMaxLon) throws Exception
    {
        try
        {
            Enumeration e = ls.getLandmarks(null, aMinLat, aMinLon, aMaxLat,
                                            aMaxLon);
            String args = "getLandmarks(null, " + aMinLat + ", " + aMaxLat
                          + ", " + aMinLon + ", " + aMaxLon + ")";

            assertContinue("IllegalArgumentException not thrown for " + args,
                           false);
        }
        catch (IllegalArgumentException iae)
        {
            // Exception was thrown correctly
        }
    }

    private void checkLandmark(Landmark aLm, String aName, String aDescription,
                               QualifiedCoordinates aCoords, AddressInfo aAddress)
    {

        assertContinue("Landmark is null", aLm != null);

        //        echo("name = " + aLm.getName() + ", desc = " + aLm.getDescription());

        // Check the values
        String incorrect = "Wrong value retrieved from LandmarkStore: ";
        assertEqual(incorrect + "name", aName, aLm.getName());
        assertEqual(incorrect + "description", aDescription, aLm
                    .getDescription());

        checkCoordinates(aCoords, aLm.getQualifiedCoordinates());
        checkAddressInfo(aAddress, aLm.getAddressInfo());
    }

    private void checkCoordinates(QualifiedCoordinates aCorrectCoords,
                                  QualifiedCoordinates aThatCoords)
    {
        if (aCorrectCoords == null)
        {
            assertContinue("Coordinates should have been null",
                           aThatCoords == null);
            return;
        }

        assertContinue("Coordinate null when retrieved from LandmarkStore",
                       aThatCoords != null);

        //        echo("lat = " + aThatCoords.getLatitude() +
        //             ", lon = " + aThatCoords.getLongitude() +
        //             ", alt = " + aThatCoords.getAltitude() +
        //             ", hacc = " + aThatCoords.getHorizontalAccuracy() +
        //             ", vacc = " + aThatCoords.getVerticalAccuracy());

        assertContinue("Latitude incorrect when retrieved from LandmarkStore",
                       aThatCoords.getLatitude() == aCorrectCoords.getLatitude());
        assertContinue("Longitude incorrect when retrieved from LandmarkStore",
                       aThatCoords.getLongitude() == aCorrectCoords.getLongitude());
        assertContinue("Altitude incorrect when retrieved from LandmarkStore",
                       isSameFloat(aThatCoords.getAltitude(), aCorrectCoords
                                   .getAltitude()));
        assertContinue(
            "HorizontalAccuracy incorrect when retrieved from LandmarkStore",
            isSameFloat(aThatCoords.getHorizontalAccuracy(), aCorrectCoords
                        .getHorizontalAccuracy()));
        assertContinue(
            "VerticalAccuracy incorrect when retrieved from LandmarkStore",
            isSameFloat(aThatCoords.getVerticalAccuracy(), aCorrectCoords
                        .getVerticalAccuracy()));
    }

    private void checkAddressInfo(AddressInfo aCorrectAddress,
                                  AddressInfo aThatAddress)
    {
        if (aCorrectAddress == null)
        {
            assertContinue("AddressInfo should have been null",
                           aThatAddress == null);
            return;
        }

        assertContinue("AddressInfo null when retrieved from LandmarkStore",
                       aThatAddress != null);

        // Check that address strings are the same as the input ones
        for (int i = 1; i <= 17; ++i)
        {
            String original = aCorrectAddress.getField(i);
            String stored = aThatAddress.getField(i);
            if (original != null)
            {
                assertContinue("AddressInfo incorrect (" + i + "): " + original
                               + " != " + stored, original.equals(stored));
            }
            else
            {
                assertContinue("AddressInfo incorrect: (" + i + "): " + stored
                               + " != null", stored == null);
            }
        }
    }

    private boolean isSameFloat(float aF1, float aF2)
    {
        if (new Float(aF1).equals(new Float(aF2)))
        {
            return true;
        }
        return false;
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

}
