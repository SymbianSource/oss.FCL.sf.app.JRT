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

public class LandmarkStoreTest extends ViperUnitTest
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

    private LandmarkStore ls;

    private String iCategory;

    public LandmarkStoreTest()
    {
        super("LandmarkStoreTest");
    }

    // These tests assume the default Landmark store is empty to begin with
    protected void runTest() throws java.lang.Throwable
    {
        testGetInstance();

        testDeleteLandmarkBadArguments();
        testGetLandmarksEmptyStore();

        // ---------------------------------------------------
        // Test addLandmark(Landmark, String category = null),
        // deleteLandmark(Landmark), updateLandmark(Landmark)
        // and getLandmarks()
        // ---------------------------------------------------
        iCategory = null;
        testAddGetDeleteAndUpdateLandmarks();

        // ---------------------------------------------------
        // Test addCategory(String), deleteCategory(String)
        // and getCategories()
        // ---------------------------------------------------
        testAddCategoryBadArguments();
        testDeleteCategoryBadArguments();
        testAddDeleteCategory();
        testAddDeleteManyCategories();
        testAddCategoryTruncate();

        testAddLandmarkToCategory1();
        testAddLandmarkToCategory2();
        testAddLandmarkToCategory3();
        testAddLandmarkToCategoryDeleteCategory();
        testAddLandmarkSameNameDifferentCategory();

        // Test addLandmark(Landmark, String category = "Work")
        iCategory = "Work";
        testAddGetDeleteAndUpdateLandmarks();

        testRemoveLandmarkFromCategoryBadArguments();
        testRemoveLandmarkFromCategory();
        testRemoveLandmarkFromCategory2();

        testGetLandmarksByNameBadArguments();
        testGetLandmarksByName();
        testGetLandmarksByNameUncategorized();
        testGetLandmarksByNameWildcards();
        testGetLandmarksByCategoryBadArguments();
        testGetLandmarksByCategory();
        testGetLandmarksByNameAndCategory();
        testGetLandmarksByAreaBadArguments();
        testGetLandmarksByArea();
        testGetLandmarksByArea2();
        testGetLandmarksByAreaAndCategory();

        testEnumerationDeleteLandmark1();
        testEnumerationDeleteLandmark2();

        // Make sure the tests cleaned the store
        assertEmptyLandmarkStore();
    }

    void testAddGetDeleteAndUpdateLandmarks() throws Exception
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
    }

    void testGetInstance()
    {
        setCurrentTest("testGetInstance()");

        // Get default store
        ls = LandmarkStore.getInstance(null);
        assertTrue(ls != null, "Instance was null for default store");
    }

    void testAddLandmarkBadArguments() throws Exception
    {
        setCurrentTest("testAddLandmarkBadArguments()");

        ls = getDefaultLandmarkStore();

        try
        {
            // Add with null Landmark parameter
            ls.addLandmark(null, iCategory);
            assertTrue(false, "NullPointerException not thrown");
        }
        catch (NullPointerException npe)
        {
            // Exception was thrown correctly
            assertNoMessage(npe);
        }

        Landmark lm = new Landmark("test", null, null, null);

        try
        {
            String badCategory = "This category does not exist";
            ls.addLandmark(lm, badCategory); // Add with nonexisting category
            assertTrue(false, "IllegalArgumentException not thrown 1");
        }
        catch (IllegalArgumentException iae)
        {
            // Exception was thrown correctly
            assertNoMessage(iae);
        }

        try
        {
            lm.setName(new String(new char[MAX_NAME_LENGTH + 1]));
            ls.addLandmark(lm, iCategory); // Add Landmark with too long name
            assertTrue(false, "IllegalArgumentException not thrown 2");
        }
        catch (IllegalArgumentException iae)
        {
            // Exception was thrown correctly
            assertNoMessage(iae);
        }
    }

    void testDeleteLandmarkBadArguments() throws Exception
    {
        setCurrentTest("testDeleteLandmarkBadArguments");

        ls = getDefaultLandmarkStore();

        try
        {
            ls.deleteLandmark(null); // Delete with null Landmark parameter
            assertTrue(false, "NullPointerException not thrown");
        }
        catch (NullPointerException npe)
        {
            // Exception was thrown correctly
            assertNoMessage(npe);
        }

        try
        {
            Landmark lm = new Landmark("test", null, null, null);
            ls.deleteLandmark(lm); // Try to delete a Landmark that was never
            // added
            assertTrue(false, "LandmarkException not thrown");
        }
        catch (LandmarkException le)
        {
            // Exception was thrown correctly
            assertNoMessage(le);
        }

    }

    void testGetLandmarksEmptyStore() throws Exception
    {
        setCurrentTest("testGetLandmarksEmptyStore()");
        ls = getDefaultLandmarkStore();
        Enumeration e = ls.getLandmarks();
        assertTrue(e == null, "Expected getLandmarks to return null");
    }

    // Create a Landmark object, store it in the LandmarkStore, check
    // that it can be found again and then delete it
    void testAddDeleteLandmark1() throws Exception
    {
        setCurrentTest("testAddDeleteLandmark1()");

        String name = "Nowhere";

        // Create a new Landmark object with only name set
        Landmark lm = new Landmark(name, null, null, null);

        saveRetrieveDelete(lm);
    }

    void testAddDeleteLandmark2() throws Exception
    {
        setCurrentTest("testAddDeleteLandmark2()");

        String name = "Somewhere";
        String desc = "It's somewhere but noone knows where it really is";

        // Create a new Landmark object with only name set
        Landmark lm = new Landmark(name, desc, null, null);

        saveRetrieveDelete(lm);
    }

    void testAddDeleteLandmark3() throws Exception
    {
        setCurrentTest("testAddDeleteLandmark3()");

        String name = "North pole";
        String desc = "Santas workshop, whats that address again?";

        QualifiedCoordinates coords = new QualifiedCoordinates(90.0f, 0.0f,
                0.0f, 10.0f, 10.0f);

        // Create a new Landmark object with only name set
        Landmark lm = new Landmark(name, desc, coords, null);

        saveRetrieveDelete(lm);
    }

    void testAddDeleteLandmark4() throws Exception
    {
        setCurrentTest("testAddDeleteLandmark4()");

        String name = "Office";
        String desc = "Where you work";

        QualifiedCoordinates coords = new QualifiedCoordinates(57.0f, 17.0f,
                34.0f, 50.0f, 80.0f);

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
    }

    // Tests that a Landmark can be added twice.
    void testAddLandmarkTwice() throws Exception
    {
        setCurrentTest("testAddLandmarkTwice()");

        String name = "Add this landmark twice";
        Landmark lm = new Landmark(name, null, null, null);

        ls = getDefaultLandmarkStore();
        ls.addLandmark(lm, iCategory);
        ls.addLandmark(lm, iCategory);

        Enumeration e = ls.getLandmarks();
        int count = findLandmarks(e, new String[] { name });
        assertTrue(count == 1, "Wrong number of landmarks found");

        ls.deleteLandmark(lm);
    }

    // Tests that addLandmark can not be used as updateLandmark.
    void testAddLandmarkNoUpdate() throws Exception
    {
        setCurrentTest("testAddLandmarkNoUpdate()");
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
    }

    // Tests that Landmarks with the same name can be added.
    void testAddLandmarkSameName() throws Exception
    {
        setCurrentTest("testAddLandmarkSameName()");

        String name = "Glenn";
        Landmark lm1 = new Landmark(name, null, null, null);
        String desc = "Another landmark with name=Glenn";
        Landmark lm2 = new Landmark(name, desc, null, null);

        ls = getDefaultLandmarkStore();
        ls.addLandmark(lm1, iCategory);
        ls.addLandmark(lm2, iCategory);

        Enumeration e = ls.getLandmarks();
        int count = findLandmarks(e, new String[] { name, name });
        assertTrue(count == 2, "Wrong number of landmarks found");

        ls.deleteLandmark(lm1);
        ls.deleteLandmark(lm2);
    }

    // Tests that a Landmark can be added, then deleted and added again.
    void testAddDeleteAddLandmark() throws Exception
    {
        setCurrentTest("testAddDeleteAddLandmark()");

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
        assertTrue(count == 1, "Wrong number of landmarks found");

        ls.deleteLandmark(lm);
    }

    // Tests that description and address are truncated if they are too long
    void testAddLandmarkTruncate() throws Exception
    {
        setCurrentTest("testAddLandmarkTruncate()");

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
        Landmark lm = new Landmark(name, tooLongDescription, null, tooLongAddr);
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
    }

    void testUpdateLandmarkBadArguments() throws Exception
    {
        setCurrentTest("testUpdateLandmarkBadArguments");

        ls = getDefaultLandmarkStore();

        try
        {
            ls.updateLandmark(null); // Update with null Landmark parameter
            assertTrue(false, "NullPointerException not thrown");
        }
        catch (NullPointerException npe)
        {
            // Exception was thrown correctly
        }

        Landmark lm = new Landmark("test", null, null, null);
        try
        {
            ls.updateLandmark(lm); // Update without adding first
            assertTrue(false, "LandmarkException not thrown 1");
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
            assertTrue(false, "LandmarkException not thrown 2");
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
            assertTrue(false, "IllegalArgumentException not thrown 3");
        }
        catch (IllegalArgumentException iae)
        {
            // Exception was thrown correctly
        }

        ls.deleteLandmark(lm);
    }

    // Add a landmark to the LandmarkStore and update with more and more
    // information.
    void testUpdateLandmark1() throws Exception
    {
        setCurrentTest("testUpdateLandmark1()");

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
        QualifiedCoordinates coords = new QualifiedCoordinates(-10.2f, -107.8f,
                -4.5f, 11.9f, 5.1f);

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
    }

    // Add a landmark to the LandmarkStore with all the information set and
    // update by removing information.
    void testUpdateLandmark2() throws Exception
    {
        setCurrentTest("testUpdateLandmark2()");

        // Create a new Landmark object
        String name = "Home";
        String desc = "There is no place like it";
        QualifiedCoordinates coords = new QualifiedCoordinates(-10.2f, -107.8f,
                -4.5f, 11.9f, 5.1f);
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
    }

    void testUpdateLandmarkTruncate() throws Exception
    {
        setCurrentTest("testUpdateLandmarkTruncate()");

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
    }

    void testAddDeleteUpdateLandmark() throws Exception
    {
        setCurrentTest("testAddDeleteUpdateLandmark()");

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
            assertTrue(false, "LandmarkException not thrown");
        }
        catch (LandmarkException le)
        {
            // Exception was thrown correctly
            assertNoMessage(le);
        }
    }

    // List all landmarks.
    void testGetAllLandmarks() throws Exception
    {
        setCurrentTest("testGetAllLandmarks()");

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
        assertTrue(count == NUM_LANDMARKS, "Wrong number of Landmarks found");

        // Delete all the added landmarks
        e = ls.getLandmarks();
        while (e.hasMoreElements())
        {
            ls.deleteLandmark((Landmark) e.nextElement());
        }
    }

    void testAddCategoryBadArguments() throws Exception
    {
        setCurrentTest("testAddCategoryBadArguments()");

        ls = getDefaultLandmarkStore();

        try
        {
            ls.addCategory(null); // Add category with null parameter
            assertTrue(false, "NullPointerException not thrown");
        }
        catch (NullPointerException npe)
        {
            // Exception was thrown correctly
        }

        try
        {
            ls.addCategory(""); // Add category with empty string parameter
            assertTrue(false, "IllegalArgumentException not thrown");
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
            assertTrue(false, "IllegalArgumentException not thrown");
        }
        catch (IllegalArgumentException iae)
        {
            // Exception was thrown correctly
        }

        ls.deleteCategory(category);
    }

    void testDeleteCategoryBadArguments() throws Exception
    {
        setCurrentTest("testDeleteCategoryBadArguments()");

        ls = getDefaultLandmarkStore();

        try
        {
            ls.deleteCategory(null); // Delete category with null parameter
            assertTrue(false, "NullPointerException not thrown");
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
    }

    // Tests that new categories can be added and deleted.
    void testAddDeleteCategory() throws Exception
    {
        setCurrentTest("testAddDeleteCategory()");

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
            assertTrue(!s.equals(category), "Category was not removed");
        }
    }

    void testAddDeleteManyCategories() throws Exception
    {
        setCurrentTest("testAddDeleteManyCategories()");

        // A variety of valid category names
        String[] categories = new String[] { "q", "Dots ...", "Exclamation!",
                                             "(parentesis)", "percentage%", "hashmark#", "at@", "et&",
                                             "commas,,,"
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
        }

        // Delete categories
        for (int i = 0; i < 100; ++i)
        {
            ls.deleteCategory(category + i);
        }
    }

    void testAddCategoryTruncate() throws Exception
    {
        setCurrentTest("testAddCategoryTruncate()");

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
            assertTrue(!maxCategoryName.equals(s), "Category was not removed");
        }
    }

    void testAddLandmarkToCategory1() throws Exception
    {
        setCurrentTest("testAddLandmarkToCategory1()");

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
        assertTrue(count == 1, "Wrong number of Landmarks found");

        ls.deleteLandmark(lm);
        ls.deleteCategory(category);
    }

    void testAddLandmarkToCategory2() throws Exception
    {
        setCurrentTest("testAddLandmarkToCategory2()");

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
        assertTrue(e == null, "Category should have been empty");

        // Add the Landmark to the category
        ls.addLandmark(lm, category);

        // Get Landmarks by category
        Enumeration e2 = ls.getLandmarks(category, null);
        int count = findLandmarks(e2, new String[] { name });
        assertTrue(count == 1, "Wrong number of Landmarks found");

        ls.deleteLandmark(lm);
        ls.deleteCategory(category);
    }

    void testAddLandmarkToCategory3() throws Exception
    {
        setCurrentTest("testAddLandmarkToCategory3()");

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
        assertTrue(count1 == 1, "Wrong number of Landmarks found");

        // Get Landmarks by category2
        Enumeration e2 = ls.getLandmarks(category2, null);
        int count2 = findLandmarks(e2, new String[] { name });
        assertTrue(count2 == 1, "Wrong number of Landmarks found");

        ls.deleteLandmark(lm);
        ls.deleteCategory(category1);
        ls.deleteCategory(category2);
    }

    void testAddLandmarkEmptyStrings() throws Exception
    {
        setCurrentTest("testAddLandmarkEmptyStrings()");

        ls = getDefaultLandmarkStore();

        try
        {
            Landmark lm = new Landmark("name", null, null, null);
            ls.addLandmark(lm, ""); // Try to use empty string for category
            assertTrue(false, "IllegalArgumentException not thrown");
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
    }

    void testAddLandmarkToCategoryDeleteCategory() throws Exception
    {
        setCurrentTest("testAddLandmarkToCategoryDeleteCategory()");

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
        assertTrue(e == null, "Expected getLandmarks to return null");

        // Make sure Landmark still exists
        Landmark fetchedLm = findLandmark(ls.getLandmarks(), name);
        checkLandmark(fetchedLm, name, null, null, null);

        ls.deleteLandmark(lm);
    }

    void testAddLandmarkSameNameDifferentCategory() throws Exception
    {
        setCurrentTest("testAddLandmarkSameNameDifferentCategory()");

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
        assertTrue(count == 2, "Wrong number of landmarks found");

        ls.deleteLandmark(lm1);
        ls.deleteLandmark(lm2);
        ls.deleteCategory(category1);
        ls.deleteCategory(category2);
    }

    void testRemoveLandmarkFromCategoryBadArguments() throws Exception
    {
        setCurrentTest("testRemoveLandmarkFromCategoryBadArguments");

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
            assertTrue(false, "NullPointerException not thrown");
        }
        catch (NullPointerException npe)
        {
            // Exception was thrown correctly
        }

        try
        {
            // Remove landmark from category with null category parameter
            ls.removeLandmarkFromCategory(lm, null);
            assertTrue(false, "NullPointerException not thrown");
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
        String tooLongCategory = new String(new char[MAX_CATEGORY_LENGTH + 1]);
        ls.removeLandmarkFromCategory(lm, tooLongCategory);

        // Silent failure if Landmark does not belong to LandmarkStore
        Landmark newLm = new Landmark("Not added", null, null, null);
        ls.removeLandmarkFromCategory(newLm, category1);

        ls.deleteLandmark(lm);
        ls.deleteCategory(category1);
        ls.deleteCategory(category2);
    }

    void testRemoveLandmarkFromCategory() throws Exception
    {
        setCurrentTest("testRemoveLandmarkFromCategory");

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
        assertTrue(e2 == null, "Landmark not removed from category");

        // Delete the category and Landmark from the store
        ls.deleteCategory(category);
        ls.deleteLandmark(lm);
    }

    void testRemoveLandmarkFromCategory2() throws Exception
    {
        setCurrentTest("testRemoveLandmarkFromCategory2()");

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
        assertTrue(e == null, "Landmark not removed from category");

        // Make sure landmark still belongs to category2
        Enumeration e2 = ls.getLandmarks(category2, name);
        Landmark fetchedLm = findLandmark(e2, name);
        checkLandmark(fetchedLm, name, null, null, null);

        // Remove landmark from category2
        ls.removeLandmarkFromCategory(lm, category2);

        Enumeration e3 = ls.getLandmarks(category2, name);
        assertTrue(e3 == null, "Landmark not removed from category");

        // Delete the categories and Landmark from the store
        ls.deleteCategory(category1);
        ls.deleteCategory(category2);
        ls.deleteLandmark(lm);
    }

    void testGetLandmarksByNameBadArguments() throws Exception
    {
        setCurrentTest("testGetLandmarksByNameBadArguments()");

        String maxName = new String(new char[MAX_NAME_LENGTH]);

        ls = getDefaultLandmarkStore();
        Landmark lm = new Landmark(maxName, null, null, null);
        ls.addLandmark(lm, null);

        // Sanity check, find landmark by name
        Enumeration e = ls.getLandmarks(null, maxName);
        int count = findLandmarks(e, new String[] { maxName });
        assertTrue(count == 1, "Wrong number of landmarks found");

        // Search with too long name
        e = ls.getLandmarks(null, maxName + "x");
        assertTrue(e == null, "Expected getLandmarks to return null (1)");

        // Search with empty string for name
        e = ls.getLandmarks(null, "");
        assertTrue(e == null, "Expected getLandmarks to return null (2)");

        ls.deleteLandmark(lm);
    }

    void testGetLandmarksByName() throws Exception
    {
        setCurrentTest("testGetLandmarksByName()");

        ls = getDefaultLandmarkStore();

        addLandmarksAndCategoriesForSearch();

        Enumeration e;
        int count;

        // Search for landmark that does not exist
        e = ls.getLandmarks(null, searchName0);
        assertTrue(e == null, "Expected getLandmarks to return null");

        // Search for landmarks 1
        e = ls.getLandmarks(null, searchName1);
        count = findLandmarks(e, new String[] { searchName1 });
        assertTrue(count == 1, "Wrong number of landmarks found");

        // Search for landmarks 2
        e = ls.getLandmarks(null, searchName2);
        count = findLandmarks(e, new String[] { searchName2, searchName2 });
        assertTrue(count == 2, "Wrong number of landmarks found");

        // Search for landmarks 3
        e = ls.getLandmarks(null, searchName3);
        count = findLandmarks(e, new String[] { searchName3, searchName3,
                                                searchName3
                                              });
        assertTrue(count == 3, "Wrong number of landmarks found");

        deleteAllLandmarksAndCategories();
    }

    void testGetLandmarksByNameUncategorized() throws Exception
    {
        setCurrentTest("testGetLandmarksByNameUncategorized()");

        ls = getDefaultLandmarkStore();

        addLandmarksAndCategoriesForSearch();

        Enumeration e;
        int count;

        // Find all landmarks that do not belong to a category
        e = ls.getLandmarks("", null);
        count = findLandmarks(e, new String[] { searchName1, searchName2 });
        assertTrue(count == 2, "Wrong number of landmarks found");

        // Search for landmarks 1
        e = ls.getLandmarks("", searchName1);
        count = findLandmarks(e, new String[] { searchName1 });
        assertTrue(count == 1, "Wrong number of landmarks found");

        // Search for landmarks 3
        e = ls.getLandmarks("", searchName3);
        assertTrue(e == null, "Expected getLandmarks to return null");

        deleteAllLandmarksAndCategories();
    }

    void testGetLandmarksByNameWildcards() throws Exception
    {
        setCurrentTest("testGetLandmarksByNameWildcards()");

        ls = getDefaultLandmarkStore();

        String[] names = { "Hepa", "Huupa", "Lupa", "upo", "upa", "Lu?a", "*" };
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
        searchAndMatch("*u?a", new String[] { "Huupa", "Lupa", "upa", "Lu?a" });
        searchAndMatch("*up?", new String[] { "Huupa", "Lupa", "upo", "upa" });
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
    }

    void testGetLandmarksByCategoryBadArguments() throws Exception
    {
        setCurrentTest("testGetLandmarksByCategoryBadArguments()");

        ls = getDefaultLandmarkStore();

        String maxCategoryName = new String(new char[MAX_CATEGORY_LENGTH]);
        ls.addCategory(maxCategoryName);

        String name = "Wherever";
        Landmark lm = new Landmark(name, null, null, null);
        ls.addLandmark(lm, maxCategoryName);

        // Sanity check, find landmark by category
        Enumeration e = ls.getLandmarks(maxCategoryName, null);
        int count = findLandmarks(e, new String[] { name });
        assertTrue(count == 1, "Wrong number of landmarks found");

        // Search with too long category name
        e = ls.getLandmarks(maxCategoryName + "x", null);
        assertTrue(e == null, "Expected getLandmarks to return null (1)");

        // Search with empty string for category name
        e = ls.getLandmarks("", null);
        assertTrue(e == null, "Expected getLandmarks to return null (2)");

        ls.deleteLandmark(lm);
        ls.deleteCategory(maxCategoryName);
    }

    void testGetLandmarksByCategory() throws Exception
    {
        setCurrentTest("testGetLandmarksByCategory()");

        ls = getDefaultLandmarkStore();

        addLandmarksAndCategoriesForSearch();

        Enumeration e;
        int count;

        // Search for category that does not exist
        e = ls.getLandmarks(searchCategory0, null);
        assertTrue(e == null, "Expected getLandmarks to return null");

        // Search for landmarks 1
        e = ls.getLandmarks(searchCategory1, null);
        count = findLandmarks(e, new String[] { searchName3 });
        assertTrue(count == 1, "Wrong number of landmarks found");

        // Search for landmarks 2
        e = ls.getLandmarks(searchCategory2, null);
        count = findLandmarks(e, new String[] { searchName2, searchName3,
                                                searchName3
                                              });
        assertTrue(count == 3, "Wrong number of landmarks found");

        // Delete the categories and Landmarks from the store
        deleteAllLandmarksAndCategories();
    }

    void testGetLandmarksByNameAndCategory() throws Exception
    {
        setCurrentTest("testGetLandmarksByNameAndCategory()");

        ls = getDefaultLandmarkStore();

        addLandmarksAndCategoriesForSearch();

        Enumeration e;
        int count;

        // Search with a category that does not exist
        e = ls.getLandmarks(searchCategory0, searchName1);
        assertTrue(e == null, "Expected getLandmarks to return null");

        // Search with a name that does not exist
        e = ls.getLandmarks(searchCategory2, searchName0);
        assertTrue(e == null, "Expected getLandmarks to return null");

        // Search with a name that does not exist in the category
        e = ls.getLandmarks(searchCategory1, searchName1);
        assertTrue(e == null, "Expected getLandmarks to return null");

        // Search for landmarks 1
        e = ls.getLandmarks(searchCategory1, searchName3);
        count = findLandmarks(e, new String[] { searchName3 });
        assertTrue(count == 1, "Wrong number of landmarks found");

        // Search for landmarks 2
        e = ls.getLandmarks(searchCategory2, searchName3);
        count = findLandmarks(e, new String[] { searchName3, searchName3 });
        assertTrue(count == 2, "Wrong number of landmarks found");

        // Search for landmarks 3
        e = ls.getLandmarks(searchCategory2, searchName2);
        Landmark fetchedLm = findLandmark(e, searchName2);
        checkLandmark(fetchedLm, searchName2, searchCategory2, null, null);

        // Delete the categories and Landmarks from the store
        deleteAllLandmarksAndCategories();
    }

    void testGetLandmarksByAreaBadArguments() throws Exception
    {
        setCurrentTest("testGetLandmarksByAreaBadArguments()");
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
            assertTrue(false,
                       "IllegalArgumentException not thrown for long category name");
        }
        catch (IllegalArgumentException iae)
        {
            // Exception was thrown correctly
            assertTrue(iae.getMessage() == null,
                       "Message not allowed for exception: " + iae);
        }

    }

    void testGetLandmarksByArea() throws Exception
    {
        setCurrentTest("testGetLandmarksByArea()");

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
        assertTrue(e == null, "Expected getLandmarks to return null");

        // Search for landmarks 1
        e = ls.getLandmarks(null, 0.0d, 90.0d, -180.0d, 0.0d);
        count = findLandmarks(e, new String[] { searchName1 });
        assertTrue(count == 1, "Wrong number of landmarks found");

        // Search for landmarks 2
        e = ls.getLandmarks(null, -90.0d, 0.0d, -180.0d, 0.0d);
        count = findLandmarks(e, new String[] { searchName2, searchName3 });
        assertTrue(count == 2, "Wrong number of landmarks found");

        // Search for landmarks 3
        e = ls.getLandmarks(null, -90.0d, 0.0d, 0.0d, 179.9d);
        count = findLandmarks(e, new String[] { searchName3, searchName3 });
        assertTrue(count == 2, "Wrong number of landmarks found");

        // Search for landmarks - find lm2, smallest possible area
        e = ls.getLandmarks(null, -5.0d, -5.0d, -180.0d, -180.0d);
        count = findLandmarks(e, new String[] { searchName2 });
        assertTrue(count == 1, "Wrong number of landmarks found");

        // Search for landmarks - search lon [-180, 0] and [100, 180)
        e = ls.getLandmarks(null, -90.0d, 90.0d, 100.0d, 0.0d);
        count = findLandmarks(e, new String[] { searchName1, searchName2,
                                                searchName3, searchName3
                                              });
        assertTrue(count == 4, "Wrong number of landmarks found");

        deleteAllLandmarksAndCategories();
    }

    void testGetLandmarksByArea2() throws Exception
    {
        setCurrentTest("testGetLandmarksByArea2()");

        ls = getDefaultLandmarkStore();
        Landmark lm1 = new Landmark("Landmark1", "description",
                                    new QualifiedCoordinates(10.0, 20.0, Float.NaN, Float.NaN,
                                                             Float.NaN), null);
        ls.addLandmark(lm1, null);
        ls.addLandmark(new Landmark("Landmark2", null,
                                    new QualifiedCoordinates(40.0, 30.0, 0, 0, 0), null), null);
        ls.addLandmark(new Landmark("Landmark3", null,
                                    new QualifiedCoordinates(30.0, -10.0, 0, 0, 0), null), null);
        ls.addLandmark(new Landmark("Landmark4", null,
                                    new QualifiedCoordinates(30.0, 165.0, 0, 0, 0), null), null);
        ls.addLandmark(new Landmark("Landmark5", null,
                                    new QualifiedCoordinates(30.0, -165.0, 0, 0, 0), null), null);

        Enumeration e = ls.getLandmarks(null, -30.0, 50.0, 5.0, 25.0);
        Landmark gotLandmark = findLandmark(e, "Landmark1");
        checkLandmark(gotLandmark, lm1.getName(), lm1.getDescription(), lm1
                      .getQualifiedCoordinates(), lm1.getAddressInfo());
        assertTrue(!e.hasMoreElements(), "Wrong number of landmarks found");

        deleteAllLandmarksAndCategories();
    }

    void testGetLandmarksByAreaAndCategory() throws Exception
    {
        setCurrentTest("testGetLandmarksByAreaAndCategory()");

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
        e = ls.getLandmarks(searchCategory0, -90.0d, 90.0d, -180.0d, 179.9d);
        assertTrue(e == null, "Expected getLandmarks to return null");

        // Search an area that does not contain any landmarks in the category
        e = ls.getLandmarks(searchCategory2, -90.0d, 90.0d, -180.0d, 0.0d);
        assertTrue(e == null, "Expected getLandmarks to return null");

        // Search for landmarks 1
        e = ls.getLandmarks(searchCategory1, -90.0d, 90.0d, -180.0d, 179.9d);
        count = findLandmarks(e, new String[] { searchName3 });
        assertTrue(count == 1, "Wrong number of landmarks found");

        // Search for landmarks 2
        e = ls.getLandmarks(searchCategory2, -90.0d, 0.0d, -180.0d, 179.9d);
        count = findLandmarks(e, new String[] { searchName3, searchName3 });
        assertTrue(count == 2, "Wrong number of landmarks found");

        // Search for landmarks 3
        e = ls.getLandmarks(searchCategory2, -90.0d, 0.0d, 100.0d, 179.9d);
        count = findLandmarks(e, new String[] { searchName3 });
        assertTrue(count == 1, "Wrong number of landmarks found");

        // Search for landmarks 4 - smallest possible search area
        e = ls.getLandmarks(searchCategory1, -90.0d, -90.0d, -21.0d, -21.0d);
        count = findLandmarks(e, new String[] { searchName3 });
        assertTrue(count == 1, "Wrong number of landmarks found");

        deleteAllLandmarksAndCategories();
    }

    void testEnumerationDeleteLandmark1() throws Exception
    {
        setCurrentTest("testEnumerationDeleteLandmark1()");

        ls = getDefaultLandmarkStore();

        Landmark lm1 = new Landmark("lm1", null, null, null);
        Landmark lm2 = new Landmark("lm2", null, null, null);
        Landmark lm3 = new Landmark("lm3", null, null, null);

        ls.addLandmark(lm1, null);
        ls.addLandmark(lm2, null);
        ls.addLandmark(lm3, null);

        Enumeration e = ls.getLandmarks();
        assertTrue(e.hasMoreElements(), "Enumeration is empty");
        ls.deleteLandmark(lm1);
        ls.deleteLandmark(lm2);

        // Expect lm1 because hasMoreElements should cache a Landmark
        int count = findLandmarks(e, new String[] { "lm1", "lm3" });
        assertTrue(count == 2, "Wrong number of landmarks found");

        ls.deleteLandmark(lm3);
    }

    void testEnumerationDeleteLandmark2() throws Exception
    {
        setCurrentTest("testEnumerationDeleteLandmark2()");

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

        assertTrue(!e.hasMoreElements(), "Wrong number of landmarks found");
    }

    // ------------------------ Helper methods -----------------------

    private void searchAndMatch(String aPattern, String[] aNames)
    throws Exception
    {

        Enumeration e = ls.getLandmarks(null, aPattern);
        int count = findLandmarks(e, aNames);
        assertTrue(count == aNames.length, "Wrong number of landmarks found");
    }

    // Add landmarks for testing getLandmarks
    private void addLandmarksAndCategoriesForSearch() throws Exception
    {
        // -180 0 180
        // |---------|- 90 1 = searchName1
        // | | | 2 = searchName2
        // | 1 | | 3 = searchName3, searchCategory1
        // |----+----|- 0 4 = searchName3, searchCategory2
        // |2 |4 5| 5 = searchName3, searchCategory2
        // | 3 | |
        // |---------|- -90

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

        // | Category | Landmarks
        // |-----------------|--------------------------------------
        // | null | searchName1, searchName2
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
        assertTrue(store != null, "LandmarkStore is null");
        return store;
    }

    private void assertEmptyLandmarkStore() throws Exception
    {
        setCurrentTest("assertEmptyLandmarkStore");
        ls = getDefaultLandmarkStore();
        Enumeration le = ls.getLandmarks();
        assertTrue(le == null, "Landmarks exist in LandmarkStore");

        Enumeration ce = ls.getCategories();
        assertTrue(!ce.hasMoreElements(), "Categories exist in LandmarkStore");
    }

    private void saveRetrieveDelete(Landmark aLm) throws Exception
    {
        String name = aLm.getName();
        String description = aLm.getDescription();
        QualifiedCoordinates coords = aLm.getQualifiedCoordinates();
        AddressInfo address = aLm.getAddressInfo();

        // Get the LandmarkStore
        ls = LandmarkStore.getInstance(null);
        assertTrue(ls != null, "LandmarkStore is null");

        // Save the landmark
        ls.addLandmark(aLm, iCategory);

        // Get all landmarks and search for the one we added
        Enumeration e = ls.getLandmarks();
        Landmark fetchedLm = findLandmark(e, name);

        ls.deleteLandmark(aLm);
        assertTrue(e != null, "No landmarks found in database");
        checkLandmark(fetchedLm, name, description, coords, address);

        ls.deleteLandmark(aLm); // Try to delete again, should be silently
        // ignored
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
                assertTrue(aLandmark != null, "Enumeration contains a null");

                if (aName.equals(aLandmark.getName()))
                {
                    assertTrue(foundLm == null,
                               "More that one landmark called " + aName);
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
                assertTrue(aLandmark != null, "Enumeration contains a null");
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
            assertTrue(found[i], "Landmark '" + aNames[i] + "' not found");
        }

        return numLandmarks;
    }

    private void findCategory(Enumeration e, String aCategory)
    {
        assertTrue(e != null, "Categories enumeration is null");
        assertTrue(e.hasMoreElements(), "Categories enumeration is empty");

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

        assertTrue(found, "Category " + aCategory + " not found");
    }

    private void assertEqual(String errorMsg, String dis, String dat)
    {
        if (dis == null)
        {
            assertTrue(dat == null, errorMsg + ": " + dat + " != null");
        }
        else
        {
            assertTrue(dis.equals(dat), errorMsg + ": " + dis + " != " + dat);
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

            assertTrue(false, "IllegalArgumentException not thrown for " + args);
        }
        catch (IllegalArgumentException iae)
        {
            // Exception was thrown correctly
            assertTrue(iae.getMessage() == null,
                       "Message not allowed for exception: " + iae);
        }
    }

    private void checkLandmark(Landmark aLm, String aName, String aDescription,
                               QualifiedCoordinates aCoords, AddressInfo aAddress)
    {

        assertTrue(aLm != null, "Landmark is null");

        // echo("name = " + aLm.getName() + ", desc = " + aLm.getDescription());

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
            assertTrue(aThatCoords == null, "Coordinates should have been null");
            return;
        }

        assertTrue(aThatCoords != null,
                   "Coordinate null when retrieved from LandmarkStore");

        // echo("lat = " + aThatCoords.getLatitude() +
        // ", lon = " + aThatCoords.getLongitude() +
        // ", alt = " + aThatCoords.getAltitude() +
        // ", hacc = " + aThatCoords.getHorizontalAccuracy() +
        // ", vacc = " + aThatCoords.getVerticalAccuracy());

        assertTrue(aThatCoords.getLatitude() == aCorrectCoords.getLatitude(),
                   "Latitude incorrect when retrieved from LandmarkStore");
        assertTrue(aThatCoords.getLongitude() == aCorrectCoords.getLongitude(),
                   "Longitude incorrect when retrieved from LandmarkStore");
        assertTrue(isSameFloat(aThatCoords.getAltitude(), aCorrectCoords
                               .getAltitude()),
                   "Altitude incorrect when retrieved from LandmarkStore");
        assertTrue(isSameFloat(aThatCoords.getHorizontalAccuracy(),
                               aCorrectCoords.getHorizontalAccuracy()),
                   "HorizontalAccuracy incorrect when retrieved from LandmarkStore");
        assertTrue(isSameFloat(aThatCoords.getVerticalAccuracy(),
                               aCorrectCoords.getVerticalAccuracy()),
                   "VerticalAccuracy incorrect when retrieved from LandmarkStore");
    }

    private void checkAddressInfo(AddressInfo aCorrectAddress,
                                  AddressInfo aThatAddress)
    {
        if (aCorrectAddress == null)
        {
            assertTrue(aThatAddress == null,
                       "AddressInfo should have been null");
            return;
        }

        assertTrue(aThatAddress != null,
                   "AddressInfo null when retrieved from LandmarkStore");

        // Check that address strings are the same as the input ones
        for (int i = 1; i <= 17; ++i)
        {
            String original = aCorrectAddress.getField(i);
            String stored = aThatAddress.getField(i);
            if (original != null)
            {
                assertTrue(original.equals(stored), "AddressInfo incorrect ("
                           + i + "): " + original + " != " + stored);
            }
            else
            {
                assertTrue(stored == null, "AddressInfo incorrect: (" + i
                           + "): " + stored + " != null");
            }
        }
    }

    private boolean isSameFloat(float aF1, float aF2)
    {
        if (new Float(aF1).equals(new Float(aF2)))
        {
            return true;
        }
        echo("Floats are different: " + aF1 + " != " + aF2);
        echo("IntBits1 = " + Float.floatToIntBits(aF1));
        echo("IntBits2 = " + Float.floatToIntBits(aF2));
        return false;
    }

}
