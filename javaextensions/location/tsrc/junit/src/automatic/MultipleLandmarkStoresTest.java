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

import java.util.*;
import java.io.IOException;

/**
 * <b>TEST CASE SPECIFICATION</b>
 *
 */

public class MultipleLandmarkStoresTest extends TestCase
{
    private static final String STORE_NAME = "store1";

    public MultipleLandmarkStoresTest()
    {
    }

    public MultipleLandmarkStoresTest(String sTestName, TestMethod rTestMethod)
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

        aSuite.addTest(new MultipleLandmarkStoresTest(
                           "testGetInstanceDefaultOnly", new TestMethod()
        {
            public void run(TestCase tc)
            {
                ((MultipleLandmarkStoresTest) tc)
                .testGetInstanceDefaultOnly();
            }
        }));

        aSuite.addTest(new MultipleLandmarkStoresTest(
                           "testListLandmarkStoresOnlyDefault", new TestMethod()
        {
            public void run(TestCase tc)
            {
                ((MultipleLandmarkStoresTest) tc)
                .testListLandmarkStoresOnlyDefault();
            }
        }));

        aSuite.addTest(new MultipleLandmarkStoresTest(
                           "testCreateLandmarkStoreBadArguments", new TestMethod()
        {
            public void run(TestCase tc)
            {
                ((MultipleLandmarkStoresTest) tc)
                .testCreateLandmarkStoreBadArguments();
            }
        }));

        aSuite.addTest(new MultipleLandmarkStoresTest(
                           "testDeleteLandmarkStoreBadArguments", new TestMethod()
        {
            public void run(TestCase tc)
            {
                ((MultipleLandmarkStoresTest) tc)
                .testDeleteLandmarkStoreBadArguments();
            }
        }));

        aSuite.addTest(new MultipleLandmarkStoresTest(
                           "testCreateListDeleteLandmarkStore", new TestMethod()
        {
            public void run(TestCase tc)
            {
                ((MultipleLandmarkStoresTest) tc)
                .testCreateListDeleteLandmarkStore();
            }
        }));

        aSuite.addTest(new MultipleLandmarkStoresTest(
                           "testCreateLandmarkStoreAlreadyExists", new TestMethod()
        {
            public void run(TestCase tc)
            {
                ((MultipleLandmarkStoresTest) tc)
                .testCreateLandmarkStoreAlreadyExists();
            }
        }));

        aSuite.addTest(new MultipleLandmarkStoresTest(
                           "testCreateGetInstanceSameObject", new TestMethod()
        {
            public void run(TestCase tc)
            {
                ((MultipleLandmarkStoresTest) tc)
                .testCreateGetInstanceSameObject();
            }
        }));

        aSuite.addTest(new MultipleLandmarkStoresTest("testCaseInsensitivity",
                       new TestMethod()
        {
            public void run(TestCase tc)
            {
                ((MultipleLandmarkStoresTest) tc)
                .testCaseInsensitivity();
            }
        }));

        aSuite.addTest(new MultipleLandmarkStoresTest("testCreateDeleteUse",
                       new TestMethod()
        {
            public void run(TestCase tc)
            {
                ((MultipleLandmarkStoresTest) tc).testCreateDeleteUse();
            }
        }));

        aSuite.addTest(new MultipleLandmarkStoresTest(
                           "testAddLandmarkAndCategoryToStore", new TestMethod()
        {
            public void run(TestCase tc)
            {
                ((MultipleLandmarkStoresTest) tc)
                .testAddLandmarkAndCategoryToStore();
            }
        }));

        return aSuite;

    }

    private void assertContinue(String aReason, boolean aCond)
    {
        if (!aCond)
            assertTrue(aReason, false);
    }

    // Test getInstance when only the default store exists
    public void testGetInstanceDefaultOnly()
    {
        try
        {
            removeExistingStores();

            LandmarkStore ls = null;

            // Try to get nonexisting store
            ls = LandmarkStore.getInstance("Apa");
            assertContinue("Got an instance for nonexisting store1", ls == null);

            ls = LandmarkStore.getInstance("c:Heppa");
            assertContinue("Got an instance for nonexisting store", ls == null);

            ls = LandmarkStore.getInstance("Huppa (d:)");
            assertContinue("Got an instance for nonexisting store", ls == null);

            // Get default store
            ls = LandmarkStore.getInstance(null);
            assertContinue("Instance was null for default store", ls != null);

            assertTrue("", true);
        }
        catch (Exception e)
        {
            assertTrue(e.getMessage(), false);
        }
    }

    public void testListLandmarkStoresOnlyDefault()
    {

        try
        {
            // List all landmark stores, should return null
            String[] stores = LandmarkStore.listLandmarkStores();
            assertContinue("Landmarkstore found, should have been null",
                           stores == null);

            assertTrue("", true);
        }
        catch (Exception e)
        {
            assertTrue(e.getMessage(), false);
        }
    }

    public void testCreateLandmarkStoreBadArguments()
    {
        try
        {
            try
            {
                // Create with null
                LandmarkStore.createLandmarkStore(null);
                assertContinue(
                    "No Exception thrown for createLandmarkStore(null)",
                    false);
            }
            catch (NullPointerException npe)
            {
                // Exception was thrown correctly
            }

            assertBadCreateStore("file://Helloworld");
            assertBadCreateStore("c:Heppa");
            assertBadCreateStore("Heppa (d:)");
            assertBadCreateStore("x:Heppa");
            assertBadCreateStore("/Bad \\Name %&#");
            assertBadCreateStore("..\\Heppa");
            assertBadCreateStore("../Heppa");
            assertBadCreateStore("");

            assertTrue("", true);
        }
        catch (Exception e)
        {
            assertTrue(e.getMessage(), false);
        }
    }

    public void testDeleteLandmarkStoreBadArguments()
    {
        try
        {
            try
            {
                // Delete with null
                LandmarkStore.deleteLandmarkStore(null);
                assertContinue("No exception thrown for DeleteLandmarkStore()",
                               false);
            }
            catch (NullPointerException npe)
            {
                // Exception was thrown correctly
            }

            LandmarkStore.deleteLandmarkStore("Nonexisting"); // Silently fail
            LandmarkStore.deleteLandmarkStore("x:Nonexisting"); // Silently fail
            LandmarkStore.deleteLandmarkStore("/Bad \\Name %&#"); // Silently
            // fail

            assertTrue("", true);
        }
        catch (Exception e)
        {
            assertTrue(e.getMessage(), false);
        }
    }

    public void testCreateListDeleteLandmarkStore()
    {
        try
        {
            String name = "store";

            // Create the store
            LandmarkStore.createLandmarkStore(name);

            // Check that store is returned in list
            assertContinue("Created store not found in list", storeExists(name));

            // Delete the store
            LandmarkStore.deleteLandmarkStore(name);
            assertContinue("Deleted store found in list", !storeExists(name));

            assertTrue("", true);
        }
        catch (Exception e)
        {
            assertTrue(e.getMessage(), false);
        }
    }

    public void testCreateLandmarkStoreAlreadyExists()
    {
        try
        {
            String name = "Create me twice";

            // Create the store
            LandmarkStore.createLandmarkStore(name);
            assertContinue("Created store not found in list", storeExists(name));

            // Create again with the same name
            try
            {
                LandmarkStore.createLandmarkStore(name);
                assertContinue("No exception thrown for createLandmarkStore!",
                               false);
            }
            catch (IllegalArgumentException iae)
            {
                // Exception thrown correctly
            }

            // Delete the store
            LandmarkStore.deleteLandmarkStore(name);
            assertContinue("Deleted store found in list", !storeExists(name));

            assertTrue("", true);
        }
        catch (Exception e)
        {
            assertTrue(e.getMessage(), false);
        }
    }

    public void testCreateGetInstanceSameObject()
    {
        try
        {

            // Create the store
            LandmarkStore.createLandmarkStore(STORE_NAME);
            assertContinue("Created store not found in list",
                           storeExists(STORE_NAME));

            LandmarkStore ls1 = LandmarkStore.getInstance(STORE_NAME);
            LandmarkStore ls2 = LandmarkStore.getInstance(STORE_NAME);

            assertContinue("Not the same instance!", ls1.equals(ls2));

            // Delete the store
            LandmarkStore.deleteLandmarkStore(STORE_NAME);
            assertContinue("Deleted store found in list",
                           !storeExists(STORE_NAME));

            assertTrue("", true);
        }
        catch (Exception e)
        {
            assertTrue(e.getMessage(), false);
        }
    }

    public void testCaseInsensitivity()
    {
        try
        {
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
            assertContinue("Instance was null for store: " + upperName1,
                           ls1 != null);

            LandmarkStore ls2 = LandmarkStore.getInstance(lowerName2);
            assertContinue("Instance was null for store: " + lowerName2,
                           ls2 != null);

            LandmarkStore ls3 = LandmarkStore.getInstance(mixedName3);
            assertContinue("Instance was null for store: " + mixedName3,
                           ls3 != null);

            // Delete
            LandmarkStore.deleteLandmarkStore(mixedName1);
            LandmarkStore.deleteLandmarkStore(upperName2);
            LandmarkStore.deleteLandmarkStore(lowerName3);

            assertTrue("", true);
        }
        catch (Exception e)
        {
            assertTrue(e.getMessage(), false);
        }
    }

    public void testCreateDeleteUse()
    {
        try
        {

            // Create LandmarkStore
            LandmarkStore.createLandmarkStore(STORE_NAME);
            LandmarkStore ls = LandmarkStore.getInstance(STORE_NAME);

            // Add a Landmark
            Landmark lm = new Landmark("Hepp", null, null, null);
            ls.addLandmark(lm, null);

            // Get landmarks enumeration
            Enumeration eLms = ls.getLandmarks();
            assertContinue("getLandmarks returned null", eLms != null);

            // Get categories enumeration
            Enumeration eCats = ls.getCategories();
            assertContinue("getCategories returned null", eCats != null);

            // Delete the store
            LandmarkStore.deleteLandmarkStore(STORE_NAME);

            assertContinue("eLms should have elements", eLms.hasMoreElements());
            assertContinue("eLms should have lements",
                           eLms.nextElement() != null);

            assertContinue("eCats should have elements", eCats
                           .hasMoreElements());
            assertContinue("eCats should have lements",
                           eCats.nextElement() != null);

            for (int i = 0; i <= 8; ++i)
            {
                try
                {
                    failWithIOException(ls, lm, i);
                    assertContinue("No IOException thrown: " + i, false);
                }
                catch (IOException ioe)
                {
                    // Exception thrown correctly
                }
                catch (Exception e)
                {
                    assertContinue("Wrong Exception thrown: " + i + ", " + e,
                                   false);
                }
            }

            assertContinue("Expected getCategories to return null", ls
                           .getCategories() == null);

            assertTrue("", true);
        }
        catch (Exception e)
        {
            assertTrue(e.getMessage(), false);
        }
    }

    public void testAddLandmarkAndCategoryToStore()
    {
        try
        {

            removeExistingStores();

            LandmarkStore.createLandmarkStore(STORE_NAME);
            LandmarkStore ls = LandmarkStore.getInstance(STORE_NAME);

            // add category to store
            String category = "test category";
            ls.addCategory(category);

            // add Landmark in category to store
            Landmark landmark = new Landmark("test landmark",
                                             "test description", new QualifiedCoordinates(80d, 45d,
                                                     250f, 20f, 30f), null);

            addLandmarkToStore(ls, landmark, category);

            // delete landmark and category
            ls.deleteLandmark(landmark);
            ls.deleteCategory(category);

            assertTrue("", true);
        }
        catch (Exception e)
        {
            assertTrue(e.getMessage(), false);
        }
    }

    // ------------------------ Helper methods -----------------------

    private void assertBadCreateStore(String aStoreName) throws Exception
    {
        try
        {
            LandmarkStore.createLandmarkStore(aStoreName);
            assertContinue("No exception thrown for createLandmarkStore()",
                           false);
        }
        catch (IllegalArgumentException e)
        {
            // Exception was thrown correctly
        }
        catch (Exception e)
        {
            assertContinue("Wrong exception thrown for createLandmarkStore(\""
                           + aStoreName + "\"): " + e, false);
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
            assertContinue("Null found in listLandmarkStores",
                           stores[i] != null);
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
            assertContinue("No test for index: " + aIndex, false);
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
        assertContinue("Landmarks enumeration is null", e != null);

        int numLandmarksAfter = 0;
        while (e.hasMoreElements())
        {
            ++numLandmarksAfter;
            Object o = e.nextElement();
        }

        assertContinue("Expected only one landmark to be added",
                       numLandmarksAfter - numLandmarksBefore == 1);
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
