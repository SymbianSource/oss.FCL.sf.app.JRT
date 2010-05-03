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
package com.nokia.mj.test.pim.automatic;
// IMPORTS
import java.lang.String;
import javax.microedition.pim.*;
import java.util.Hashtable;
import java.util.Vector;
import java.util.Enumeration;

// CLASS DEFINITION
/**
 * Base that tests the items of a PIM list
 *
 * Tests the following features:
 '    - Maximum values of items
 *    - Different attribute combinations
 *
 * This class is common for all item test and
 * must not be changed unless you are implementing
 * new common test for all platforms or updating
 * some tests.
 *
 */

public class PLitemTester
{

    /**
     * Constructor
     */
    public void PLitemTester()
    {
    }

    /**
     * Tests maximum values of the PIM list fields
     *
     * @param aList PIM API list
     * @param aFieldsData hash table containing the test data
     * @return String containing the error decription. null if no errors occured
     */
    public static String testMaximumValues(
        PIMList aList,
        Hashtable aFieldsData)
    {
        // Check that test is ready to run
        if (aList == null ||
                aFieldsData == null)
        {
            return "Test has not been initialized";
        }

        Enumeration keys = aFieldsData.keys();

        for (int i = 0; i < aFieldsData.size(); i++)
        {
            // Get field id and its maximum value
            int field = ((Integer) keys.nextElement()).intValue();
            int maximumValues = ((Integer) aFieldsData.get(new Integer(field))).intValue();

            // Check that values match
            if (maximumValues !=  aList.maxValues(field))
            {
                return "Field " + field + " has wrong number of maximum values "
                       + aList.maxValues(field);
            }
        }
        // Test was ok
        return null;
    }

    /**
     * Tests attribute combinations of the specific PIM list
     *
     * @param aList PIM API list
     * @param aItem PIM API list item
     * @param aTestData Class containing test data
     */
    public static String testAttributeCombinations(
        PIMList aList,
        PIMItem aItem,
        SupportTestData aTestData)
    {
        if (aItem == null ||
                aList == null ||
                aTestData == null)
        {
            return "Test has not been initialized";
        }
        Vector fields = aTestData.getSupportedAttributeFields();

        for (int i = 0; i < fields.size(); i++)
        {
            // Get test data
            int field = ((Integer) fields.elementAt(i)).intValue();
            Vector attributeCombinations = aTestData.getFieldAttributeCombinations(field);
            Vector fieldValues = aTestData.getFieldValue(field);

            // Test all combinations
            for (int j = 0; j < attributeCombinations.size(); j++)
            {
                int attributes = ((Integer) attributeCombinations.elementAt(j)).intValue();
                Object value = fieldValues.elementAt(j);
                System.out.println("Log: PLitemTester: Testing field: " + field +
                                   ". Attributes: " + attributes + ". Value: " + value.toString());
                // Commit item with specific attributes and value
                if (!commitItem(aList, aItem, field, attributes, value))
                {
                    return "Could not commit item field: " + field;
                }
                // Check that valid attributes are returned
                if (attributes != aItem.getAttributes(field, 0))
                {
                    return "Bitmask of the attributes didn't match for the field " + field
                           + ". Expected: " + attributes + " Returned: " + aItem.getAttributes(field, 0);
                }
                if (!removeField(aItem, field , 0))
                {
                    return "Could not remove field data for field: " + field;
                }
            }
        }

        // Test was ok
        return null;
    }

    /**
     * Commits the PIM item
     */
    private static boolean commitItem(
        PIMList aList,
        PIMItem aItem,
        int aField,
        int aAttributes,
        Object aValue)
    {
        int dataType = 0;
        // Get data type of the current field so
        // we know which type of date we have to add
        // to the item
        try
        {
            dataType = aList.getFieldDataType(aField);
            System.out.println("Log: PLItemTester:commitItem: Data type of the field is: " + dataType);
        }
        catch (UnsupportedFieldException ufe)
        {
            ufe.printStackTrace();
            return false;
        }
        try
        {
            // Add corresponding data to the PIM list item
            switch (dataType)
            {
            case PIMItem.INT:
            {
                aItem.addInt(
                    aField,
                    aAttributes,
                    ((Integer) aValue).intValue());
                break;
            }
            case PIMItem.BOOLEAN:
            {
                aItem.addBoolean(
                    aField,
                    aAttributes,
                    ((Boolean) aValue).booleanValue());
                break;
            }
            case PIMItem.STRING:
            {
                aItem.addString(
                    aField,
                    aAttributes,
                    ((String) aValue).toString());
                break;
            }
            case PIMItem.BINARY:
            {
                byte[] byteArray = ((String) aValue).toString().getBytes();
                aItem.addBinary(
                    aField,
                    aAttributes,
                    byteArray,
                    0,
                    byteArray.length);
                break;
            }
            case PIMItem.STRING_ARRAY:
            {
                String[] stringArray = (String[]) aValue;
                aItem.addStringArray(
                    aField,
                    aAttributes,
                    stringArray);
                break;
            }
            case PIMItem.DATE:
            {
                aItem.addDate(
                    aField,
                    aAttributes,
                    ((Long) aValue).longValue());
                break;
            }
            default:
            {
                System.out.println("Unknown field data type: " + dataType);
                return false;
            }
            }
        }
        catch (FieldFullException ff)
        {
            ff.printStackTrace();
            return false;
        }

        // Save the item so that valid attributes are
        // left into the item
        try
        {
            aItem.commit();
        }
        catch (PIMException pe)
        {
            pe.printStackTrace();
            return false;
        }
        // Success
        return true;
    }

    /**
     * Removes specific field from PIM item
     */
    private static boolean removeField(
        PIMItem aItem,
        int aField,
        int aIndex)
    {
        try
        {
            aItem.removeValue(aField, aIndex);
        }
        catch (UnsupportedFieldException ufe)
        {
            ufe.printStackTrace();
            return false;
        }
        // Success
        return true;
    }
}

// End of file

