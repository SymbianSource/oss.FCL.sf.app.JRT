/*
* Copyright (c) 2008 Nokia Corporation and/or its subsidiary(-ies).
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


// PACKAGE
package com.nokia.mj.test.pim.automatic ;
// IMPORTS
import java.lang.String;
import javax.microedition.pim.*;
import java.util.Hashtable;
import java.util.Vector;
import java.util.Enumeration;

// CLASS DEFINITION
/**
 * Base that tests the category functionality
 *
 * This class is common for all category tests and
 * it must not be changed unless you are implementing
 * new common test or updating some old tests
 */

public abstract class PLcategoryTester
{

    /**
     * Constructor
     */
    public void PLcategoryTester()
    {
    }

    /**
     * Tries to create a category to the specified PIM API list
     *
     * @param aList PIM list where to create the category
     * @param aCategoryName Category name which will be created
     * @return true if category was successfully created, false if not
     */
    public static boolean testCreateCategory(
        PIMList aList,
        String aCategoryName)
    {

        if (aList == null)
        {
            // Test not initialized
            return false;
        }
        // Try creating category
        try
        {
            aList.addCategory(aCategoryName);
        }
        catch (PIMException pe)
        {
            // Categories not supported
            return false;
        }
        // Category was created
        return true;
    }

    /**
     * Tries to delete a category from the specified PIM API list
     *
     * @param aList PIM list from where the category is deleted
     * @param aCategoryName Category name which will be deleted
     * @return true if category was successfully deleted, false if not
     */
    public static boolean testDeleteCategory(
        PIMList aList,
        String aCategoryName)
    {

        if (aList == null)
        {
            // Test not initialized
            return false;
        }
        // Try creating category
        try
        {
            aList.deleteCategory(aCategoryName, true);
        }
        catch (PIMException pe)
        {
            return false;
        }
        // Category was deleted
        return true;
    }

    /**
     * Tries to rename a category from the specified PIM API list
     *
     * @param aList PIM list which category will be renamed
     * @param aOldName Category's old name
     * @param aNewname Category's new name
     * @return true if category was successfully renamed, false if not
     */
    public static boolean testRenameCategory(
        PIMList aList,
        String aOldName,
        String aNewName)
    {

        if (aList == null)
        {
            // Test not initialized
            return false;
        }
        // Try creating category
        try
        {
            aList.renameCategory(aOldName, aNewName);
        }
        catch (PIMException pe)
        {
            return false;
        }
        // Category was renamed
        return true;
    }

    /**
     * Test that PIMList.maxCategories() returns right amount
     * of maximun categories
     *
     * @param aList PIM list from where to ask maximum number of categories
     * @param aMaxCategories Expected number of maximum categories
     * @param aIsStaticCategories 0 or 1 indication static categories or unlimited
     * @param aStaticCategories The names of the static categories
     * @return null string if the test was OK, otherwise a string with
     *         error description
     */
    public static String testMaxCategories(
        PIMList aList,
        int aMaxCategories,
        int aIsStaticCategories,
        String[] aStaticCategories)
    {
        String error = null;
        int maxCategories = -2; // -2 not supported by the API
        String testCategory = "testCategory12344321";
        String renamedTestCategory = "renamedTestCategory12344321";

        if (aList == null)
        {
            return "Test has not been initialized";
        }
        // Get number of categories and compare
        // it to expected number of categories
        maxCategories = aList.maxCategories();
        if (maxCategories != aMaxCategories)
        {
            return "Number of categories didn't match. maxCategoires() returned: " +
                   maxCategories + " Expected number was: " + aMaxCategories;
        }
        // If this PIM API version supports only static
        // categories, test that the static categories
        // are found
        if (aIsStaticCategories == 1)
        {
            error = testGetCategories(aList, aStaticCategories);
            if (error != null)
            {
                return error;
            }
        }
        // If unlimited number of categories is supported
        // test creation, deletion and renaming a category
        if (aMaxCategories == -1)
        {
            // Create
            System.out.println("Log::PLcategoryTester - Creating category");
            if (!testCreateCategory(aList, testCategory))
            {
                return "Unable to create category: " + testCategory;
            }
            // Rename
            System.out.println("Log::PLcategoryTester - Renaming category");
            if (!testRenameCategory(aList, testCategory, renamedTestCategory))
            {
                return "Unable to rename category: " + testCategory;
            }
            // Delete
            System.out.println("Log::PLcategoryTester - Deleting category");
            if (!testDeleteCategory(aList, renamedTestCategory))
            {
                return "Unable to delete category: " + renamedTestCategory;
            }
        }

        // Test was ok
        return null;
    }

    /**
     * Tests that getCategories returns the right names of
     * categories in the PIM list
     *
     * @param aList PIM list from where to get categories
     * @param aTestCategories Category names which should be found from
     *        the categories list
     * @return null string if the test was OK, otherwise a string with
     *         error description
     */
    public static String testGetCategories(
        PIMList aList,
        String[] aTestCategories)
    {
        String[] categories = null;
        boolean found = false;

        // Check that test is initialized
        if (aList == null)
        {
            return "Test has not been initalized";
        }

        // Get categories
        try
        {
            categories = aList.getCategories();
        }
        catch (PIMException pe)
        {
            pe.printStackTrace();
            return "Cannot get categories. PIMList is not accessible";
        }

        // Check that the test categories match
        // with the categories that the API returns
        for (int i = 0; i < aTestCategories.length; i++)
        {
            for (int j = 0; j < categories.length; j++)
            {
                if (aTestCategories[ i ].equals(categories[ j ]))
                {
                    found = true;
                }
            }
            // Check if the category was found
            if (!found)
            {
                return "Category name " + aTestCategories[ i ] +
                       " was not found from the contact list categories";
            }
            found = false;
        }
        // Test was ok
        return null;
    }
}

// End of file

