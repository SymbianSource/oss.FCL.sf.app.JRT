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

import java.util.*;
import java.io.IOException;

public class MultipleLandmarkStoresTest extends ViperUnitTest
{
    private static final String STORE_NAME = "store1";

    public MultipleLandmarkStoresTest()
    {
        super("MultipleLandmarkStoresTest");
    }

    // These tests assume that you can create/delete LandmarkStores
    protected void runTest() throws java.lang.Throwable
    {
        testGetInstanceDefaultOnly();
        testListLandmarkStoresOnlyDefault();
        testCreateLandmarkStoreBadArguments();
        testDeleteLandmarkStoreBadArguments();
        testCreateListDeleteLandmarkStore();
        testCreateLandmarkStoreAlreadyExists();
        testCreateGetInstanceSameObject();
        testCaseInsensitivity();
        testCreateDeleteUse();
        testAddLandmarkAndCategoryToStore();
    }

    // Test getInstance when only the default store exists
    void testGetInstanceDefaultOnly() throws Exception
    {
        setCurrentTest("testGetInstanceDefaultOnly()");
        removeExistingStores();

        LandmarkStore ls = null;

        // Try to get nonexisting store
        ls = LandmarkStore.getInstance("Apa");
        assertTrue(ls == null, "Got an instance for nonexisting store1");

        ls = LandmarkStore.getInstance("c:Heppa");
        assertTrue(ls == null, "Got an instance for nonexisting store");

        ls = LandmarkStore.getInstance("Huppa (d:)");
        assertTrue(ls == null, "Got an instance for nonexisting store");

        // Get default store
        ls = LandmarkStore.getInstance(null);
        assertTrue(ls != null, "Instance was null for default store");
    }

    void testListLandmarkStoresOnlyDefault() throws Exception
    {
        setCurrentTest("testListLandmarkStoresOnlyDefault()");

        // List all landmark stores, should return null
        String[] stores = LandmarkStore.listLandmarkStores();
        assertTrue(stores == null, "Landmarkstore found, should have been null");
    }

    void testCreateLandmarkStoreBadArguments() throws Exception
    {
        setCurrentTest("testCreateLandmarkStoreBadArguments()");

        try
        {
            // Create with null
            LandmarkStore.createLandmarkStore(null);
            assertTrue(false,
                       "No Exception thrown for createLandmarkStore(null)");
        }
        catch (NullPointerException npe)
        {
            // Exception was thrown correctly
            assertNoMessage(npe);
        }

        assertBadCreateStore("file://Helloworld");
        assertBadCreateStore("c:Heppa");
        assertBadCreateStore("Heppa (d:)");
        assertBadCreateStore("x:Heppa");
        assertBadCreateStore("/Bad \\Name %&#");
        assertBadCreateStore("..\\Heppa");
        assertBadCreateStore("../Heppa");
        assertBadCreateStore("");
    }

    void testDeleteLandmarkStoreBadArguments() throws Exception
    {
        setCurrentTest("testDeleteLandmarkStoreBadArguments()");

        try
        {
            // Delete with null
            LandmarkStore.deleteLandmarkStore(null);
            assertTrue(false, "No exception thrown for DeleteLandmarkStore()");
        }
        catch (NullPointerException npe)
        {
            // Exception was thrown correctly
            assertNoMessage(npe);
        }

        LandmarkStore.deleteLandmarkStore("Nonexisting"); // Silently fail
        LandmarkStore.deleteLandmarkStore("x:Nonexisting"); // Silently fail
        LandmarkStore.deleteLandmarkStore("/Bad \\Name %&#"); // Silently fail
    }

    void testCreateListDeleteLandmarkStore() throws Exception
    {
        setCurrentTest("testCreateListDeleteLandmarkStore()");
        String name = "store";

        // Create the store
        LandmarkStore.createLandmarkStore(name);

        // Check that store is returned in list
        assertTrue(storeExists(name), "Created store not found in list");

        // Delete the store
        LandmarkStore.deleteLandmarkStore(name);
        assertTrue(!storeExists(name), "Deleted store found in list");
    }

    void testCreateLandmarkStoreAlreadyExists() throws Exception
    {
        setCurrentTest("testCreateLandmarkStoreAlreadyExists()");
        String name = "Create me twice";

        // Create the store
        LandmarkStore.createLandmarkStore(name);
        assertTrue(storeExists(name), "Created store not found in list");

        // Create again with the same name
        try
        {
            LandmarkStore.createLandmarkStore(name);
            assertTrue(false, "No exception thrown for createLandmarkStore!");
        }
        catch (IllegalArgumentException iae)
        {
            // Exception thrown correctly
        }

        // Delete the store
        LandmarkStore.deleteLandmarkStore(name);
        assertTrue(!storeExists(name), "Deleted store found in list");
    }

    void testCreateGetInstanceSameObject() throws Exception
    {
        setCurrentTest("testCreateGetInstanceSameObject()");

        // Create the store
        LandmarkStore.createLandmarkStore(STORE_NAME);
        assertTrue(storeExists(STORE_NAME), "Created store not found in list");

        LandmarkStore ls1 = LandmarkStore.getInstance(STORE_NAME);
        LandmarkStore ls2 = LandmarkStore.getInstance(STORE_NAME);

        assertTrue(ls1.equals(ls2), "Not the same instance!");

        // Delete the store
        LandmarkStore.deleteLandmarkStore(STORE_NAME);
        assertTrue(!storeExists(STORE_NAME), "Deleted store found in list");
    }

    void testCaseInsensitivity() throws Exception
    {
        setCurrentTest("testCaseInsensitivity()");
        String lowerName1 = "store";
        String upperName1 = "STORE";
        String mixedName1 = "sToRe";

        String lowerName2 = "smores";
        String upperName2 = "SMORES";
        String mixedName2 = "SmoRES";

        String lowerName3 = "da store";
        String upperName3 = "DA STORE";
        String mixedName3 = "dA sToRe";

        // Create
        LandmarkStore.createLandmarkStore(lowerName1);
        LandmarkStore.createLandmarkStore(mixedName2);
        LandmarkStore.createLandmarkStore(upperName3);

        // Open
        LandmarkStore ls1 = LandmarkStore.getInstance(upperName1);
        assertTrue(ls1 != null, "Instance was null for store: " + upperName1);

        LandmarkStore ls2 = LandmarkStore.getInstance(lowerName2);
        assertTrue(ls2 != null, "Instance was null for store: " + lowerName2);

        LandmarkStore ls3 = LandmarkStore.getInstance(mixedName3);
        assertTrue(ls3 != null, "Instance was null for store: " + mixedName3);

        // Delete
        LandmarkStore.deleteLandmarkStore(mixedName1);
        LandmarkStore.deleteLandmarkStore(upperName2);
        LandmarkStore.deleteLandmarkStore(lowerName3);
    }

    void testCreateDeleteUse() throws Exception
    {
        setCurrentTest("testCreateDeleteUse()");

        // Create LandmarkStore
        LandmarkStore.createLandmarkStore(STORE_NAME);
        LandmarkStore ls = LandmarkStore.getInstance(STORE_NAME);

        // Add a Landmark
        Landmark lm = new Landmark("Hepp", null, null, null);
        ls.addLandmark(lm, null);

        // Get landmarks enumeration
        Enumeration eLms = ls.getLandmarks();
        assertTrue(eLms != null, "getLandmarks returned null");

        // Get categories enumeration
        Enumeration eCats = ls.getCategories();
        assertTrue(eCats != null, "getCategories returned null");

        // Delete the store
        LandmarkStore.deleteLandmarkStore(STORE_NAME);

        // All operations should fail now
        try
        {
            assertTrue(!eLms.hasMoreElements(), "eLms should not have elements");
            Object o = eLms.nextElement();
            assertTrue(false, "No Exception thrown for eLms");
        }
        catch (NoSuchElementException nsee)
        {
            // Exception thrown correctly
        }
        catch (Exception e)
        {
            assertTrue(false, "Wrong Exception thrown for eLms: " + e);
        }

        try
        {
            assertTrue(!eCats.hasMoreElements(),
                       "eCats should not have elements");
            Object o = eCats.nextElement();
            assertTrue(false, "No Exception thrown for eCats");
        }
        catch (NoSuchElementException nsee)
        {
            // Exception thrown correctly
        }
        catch (Exception e)
        {
            assertTrue(false, "Wrong Exception thrown for eCats: " + e);
        }

        for (int i = 0; i <= 8; ++i)
        {
            try
            {
                failWithIOException(ls, lm, i);
                assertTrue(false, "No IOException thrown: " + i);
            }
            catch (IOException ioe)
            {
                // Exception thrown correctly
            }
            catch (Exception e)
            {
                assertTrue(false, "Wrong Exception thrown: " + i + ", " + e);
            }
        }

        assertTrue(ls.getCategories() == null,
                   "Expected getCategories to return null");
    }

    void testAddLandmarkAndCategoryToStore() throws Exception
    {
        setCurrentTest("addLandmarkAndCategoryToStore()");

        removeExistingStores();

        LandmarkStore.createLandmarkStore(STORE_NAME);
        LandmarkStore ls = LandmarkStore.getInstance(STORE_NAME);

        // add category to store
        String category = "test category";
        ls.addCategory(category);

        // add Landmark in category to store
        Landmark landmark = new Landmark("test landmark", "test description",
                                         new QualifiedCoordinates(80d, 45d, 250f, 20f, 30f), null);

        addLandmarkToStore(ls, landmark, category);

        // delete landmark and category
        ls.deleteLandmark(landmark);
        ls.deleteCategory(category);
    }

    // ------------------------ Helper methods -----------------------

    private void assertBadCreateStore(String aStoreName) throws Exception
    {
        try
        {
            LandmarkStore.createLandmarkStore(aStoreName);
            assertTrue(false, "No exception thrown for createLandmarkStore()");
        }
        catch (IllegalArgumentException e)
        {
            // Exception was thrown correctly
        }
        catch (Exception e)
        {
            assertTrue(false,
                       "Wrong exception thrown for createLandmarkStore(\""
                       + aStoreName + "\"): " + e);
        }
    }

    private boolean storeExists(String aStoreName) throws Exception
    {
        String[] stores = LandmarkStore.listLandmarkStores();
        if (stores == null)
        {
            return false;
        }

        String listName = null;
        for (int i = 0; i < stores.length; ++i)
        {
            assertTrue(stores[i] != null, "Null found in listLandmarkStores");
            if (aStoreName.equals(stores[i]))
            {
                listName = stores[i];
                break;
            }
        }

        return aStoreName.equals(listName);
    }

    private void failWithIOException(LandmarkStore aLs, Landmark aLm, int aIndex)
    throws Exception
    {
        Enumeration e = null;

        switch (aIndex)
        {
        case 0:
            aLs.addLandmark(aLm, null);
            break;
        case 1:
            e = aLs.getLandmarks(null, null);
            break;
        case 2:
            e = aLs.getLandmarks();
            break;
        case 3:
            e = aLs.getLandmarks(null, -90, 90, -180, 179);
            break;
        case 4:
            aLs.removeLandmarkFromCategory(aLm, "categoryName");
            break;
        case 5:
            aLs.updateLandmark(aLm);
            break;
        case 6:
            aLs.deleteLandmark(aLm);
            break;
        case 7:
            aLs.addCategory("categoryName");
            break;
        case 8:
            aLs.deleteCategory("categoryName");
            break;
        default:
            assertTrue(false, "No test for index: " + aIndex);
        }
    }

}
