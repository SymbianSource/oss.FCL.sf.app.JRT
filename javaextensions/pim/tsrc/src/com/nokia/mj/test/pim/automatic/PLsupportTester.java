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
 * Base that tests the support functionality
 *
 * Provides test functions to:
 *    - supported fields
 *    - supported attributes
 *    - supported array elements
 *    - supported repeat rule fields
 *    - fields order validation
 *
 * This class is common for all support test and
 * must not be changed unless you are implementing
 * new common test for all platforms or updating
 * some tests.
 */

public class PLsupportTester
{

    private final static int FIELDS = 0;
    private final static int ARRAY_ELEMENTS = 1;

    /**
     * Constructor
     */
    public void PLsupportTester()
    {
    }

    /**
     * Test supported fields of the list
     *
     * @param aList PIM API list
     * @param aTestData Tested data
     * @return String containing the error decription. null if no errors occured
     */
    public static String testSupportedFields(
        PIMList aList,
        SupportTestData aTestData)
    {
        int supportedCount = 0;

        if (aList == null ||
                aTestData == null)
        {
            return "Test not initialized";
        }

        // Test supported fields
        Vector supportedFields = aTestData.getSupportedFields();
        // Check all elements initialized
        for (int i = 0; i < supportedFields.size(); i++)
        {
            int field = ((Integer) supportedFields.elementAt(i)).intValue();
            boolean isSupported = aList.isSupportedField(field);
            // Check if this field is supported or not
            System.out.println("Log: PLsupportTester: Testing supported field: " + field);
            if (!isSupported)
            {
                return "Requested field " + field + " is not supported as it should be" ;
            }
        }
        // Check that the count of the tested fields and supported fields match
        if (supportedFields.size() != aList.getSupportedFields().length)
        {
            return "Test has incomplete number of supported fields, "
                   + "or getSupportedFields() returns wrong amount of data";
        }

        // Test unsupported fields
        Vector unsupportedFields = aTestData.getUnsupportedFields();
        // Check all elements initialized
        for (int i = 0; i < unsupportedFields.size(); i++)
        {
            int field = ((Integer) unsupportedFields.elementAt(i)).intValue();
            boolean isSupported = aList.isSupportedField(field);
            // Check if this field is supported or not
            System.out.println("Log: PLsupportTester: Testing unsupported field: " + field);
            if (isSupported)
            {
                return "Requested field " + field + " is supported as it shouldn't be" ;
            }
        }
        // Check that getSupportedFields returns the right fields
        // Test was ok if checkSupportedFields returns null
        return checkSupportedElements(aList, supportedFields, 0, FIELDS);
    }

    /**
     * Test supported attributes of the list fields
     *
     * @param aList PIM API list
     * @param aTestData Tested data
     * @return String containing the error decription. null if no errors occured
     */
    public static String testSupportedAttributes(
        PIMList aList,
        SupportTestData aTestData)
    {
        boolean isSupported = false;
        String error = null;
        int singleBit = 1;  // 0001
        int attributeCount = 0;

        if (aList == null ||
                aTestData == null)
        {
            return "Test has not been initialized";
        }
        Vector fields = aTestData.getSupportedAttributeFields();

        for (int i = 0; i < fields.size(); i++)
        {
            int field = ((Integer) fields.elementAt(i)).intValue();
            // Get tested attributes of this field
            int attributes = aTestData.getSupportedAttributes(field);
            // Test that each attribute is supported
            while (singleBit <= PIMItem.EXTENDED_ATTRIBUTE_MIN_VALUE)
            {
                int singleAttribute = (attributes & singleBit);

                if (singleAttribute != 0)
                {
                    System.out.println("Testing attribute: " + singleAttribute
                                       + " for field: " + field);
                    if (!aList.isSupportedAttribute(field, singleAttribute))
                    {
                        return "Unsupported attribute:" + singleAttribute;
                    }
                    attributeCount++;
                }
                singleBit <<= 1;
            }
            // Check that the test count matches with the real count of attributes
            if (attributeCount != aList.getSupportedAttributes(field).length)
            {
                return "Tested field: " + field + " has incomplete number of attributes";
            }

            // Check that supported attributes can be found from the test data
            error = checkSupportedAttributes(aList.getSupportedAttributes(field),
                                             attributes);
            if (error != null)
            {
                return error;
            }

            // Initialize values fot the next field
            singleBit = 1;  // Back to 0001
            attributeCount = 0;
        }
        // Test was ok
        return null;
    }
    /**
     * Test supported array elements
     *
     * @param aList PIM API list
     * @param aFieldsData hash table containing the test data
     * @return String containing the error decription. null if no errors occured
     */
    public static String testSupportedArrayElements(
        PIMList aList,
        SupportTestData aTestData)
    {
        String error = null;

        if (aList == null ||
                aTestData == null)
        {
            return "Test has not been initialized";
        }
        Vector fields = aTestData.getSupportedArrayElementFields();

        for (int i = 0; i < fields.size(); i++)
        {
            int field = ((Integer) fields.elementAt(i)).intValue();
            // Get tested attributes of this field
            Vector arrayElements = aTestData.getSupportedArrayElements(field);
            // Test array elements
            System.out.println("Log: PLsupportTester: Testing field: " + field);
            for (int j = 0; j < arrayElements.size(); j++)
            {
                System.out.println("Log: PLsupportTester: Testing array element: " +
                                   arrayElements.elementAt(j));
                boolean isSupported = aList.isSupportedArrayElement(field,
                                      ((Integer) arrayElements.elementAt(j)).intValue());
                if (!isSupported)
                {
                    return "Requested array element " + arrayElements.elementAt(j) +
                           " is not supported in field " + field;
                }
            }
            // Check that the test count matches with the real count of array elements
            if (arrayElements.size() != aList.getSupportedArrayElements(field).length)
            {
                return "Tested field: " + field + " has incomplete number "
                       + "of array elements";
            }
            // Check that this field's attributes are found from the test data
            error = checkSupportedElements(aList, arrayElements,
                                           field, ARRAY_ELEMENTS);
            if (error != null)
            {
                return error;
            }
        }
        // Test was ok
        return null;
    }
    /**
     * Test supported repeat rule elements for specific PIM list
     *
     * @param aList PIM API list
     * @param iTestData Class containing the test data
     * @return String containing the error decription. null if no errors occured
     */
    public static String testSupportedRRFields(
        PIMList aList,
        SupportTestData aTestData)
    {
        int fieldCount = 0;

        if (aList == null ||
                aTestData == null)
        {
            return "Test has not been initialized";
        }
        // Attirbute field functions can be used, thus the
        // structure of the test data is similar to attribute tests
        Vector fields = aTestData.getSupportedAttributeFields();

        for (int i = 0; i < fields.size(); i++)
        {
            int frequency = ((Integer) fields.elementAt(i)).intValue();
            // Get tested repeat rules of this field. NOTE: attribute functions
            // are used due to similar test data structure
            int supportedRepeatRules = aTestData.getSupportedAttributes(frequency);
            // Test that each repeat rule is supported
            int[] apiSupportedFields = ((EventList)
                                        aList).getSupportedRepeatRuleFields(frequency);
            System.out.println("Log: PLsupportTester: Testing frequency: " + frequency);
            for (int j = 0; j < apiSupportedFields.length; j++)
            {
                fieldCount++;
                System.out.println("Log: PLsupportTester: Testing repeat rule: " +
                                   apiSupportedFields[ j ]);
                // Bitwise operations used to check support
                if (supportedRepeatRules != -1)
                {
                    if ((supportedRepeatRules & apiSupportedFields[ j ]) == 0 &&
                            (apiSupportedFields[ j ] != 0))
                    {
                        return "getSupportedRepeatRuleFields() returned unexpected "
                               + "value: " + apiSupportedFields[ j ];
                    }
                    // TODO fix test
                }
                // No repeat rules
                else
                {
                    fieldCount = 1;
                    if (apiSupportedFields.length != 1 &&
                            apiSupportedFields[ 0 ] != 0)
                    {
                        return "There is a repeat rule for frequency: " +
                               frequency + " as there shouldn't be";
                    }
                }
            }
            if (apiSupportedFields.length != fieldCount)
            {
                return "Returned wrong number of supported fields for frequency: "
                       + frequency + " Expected: " + fieldCount + " Returned: "
                       + apiSupportedFields.length;
            }
            fieldCount = 0;
        }

        // Test was ok
        return null;
    }

    /**
     * Tests that supported fields are returned in right order
     *
     * @param aList PIM API list
     * @param iTestData Class containing the test data
     * @return String containing the error decription. null if no errors occured
     */
    public static String testFieldsOrder(
        PIMList aList,
        SupportTestData aTestData)
    {
        int[] fieldsArray = null;

        // Check that test has been initialized
        if (aList == null ||
                aTestData == null)
        {
            return "Test has not been initialized";
        }

        fieldsArray = aList.getSupportedFields();
        Vector orderedFields = aTestData.getSupportedFields();

        // Array length and test data size must be equal
        if (orderedFields.size() != fieldsArray.length)
        {
            return "Tested fields and supported fields sizes doesn't match";
        }

        // Check that fields are in right order
        for (int i = 0; i < orderedFields.size(); i++)
        {
            System.out.println("Log: PLsupportTester: Testing order for field: " +
                               orderedFields.elementAt(i));
            if (((Integer) orderedFields.elementAt(i)).intValue() !=
                    fieldsArray[ i ])
            {
                return "Supported field " + fieldsArray[ i ] + " was not "
                       + "expected to be next field. Field should be "
                       + orderedFields.elementAt(i);
            }
        }
        // Test was ok
        return null;
    }

    /**
     * Checks that supported elements are found
     * from the test data. Fields or array elements can be used
     *
     * @param aList PIM API list
     * @param aTestedSupportedElements Vector containing the test data
     * @param aField Field which array elements are checked
     * @param aElementType Element types in the test data. Look at the
     *        top of the class for constants
     * @return String containing the error decription. null if no errors occured
     */
    private static String checkSupportedElements(
        PIMList aList,
        Vector aTestedSupportedElements,
        int aField,
        int aElementsType)
    {

        int[] supportedElements = null;

        if (aElementsType == ARRAY_ELEMENTS)
        {
            supportedElements = aList.getSupportedArrayElements(aField);
        }
        else if (aElementsType == FIELDS)
        {
            supportedElements = aList.getSupportedFields();
        }
        // Test that all supported elements are found
        // from test data
        for (int i = 0; i < supportedElements.length; i++)
        {
            System.out.println("Log: checkSupportedElements() Testing element: " +
                               supportedElements[ i ]);
            if (!aTestedSupportedElements.contains(new Integer(supportedElements[ i ])))
            {
                return "Supported element " + supportedElements[ i ] +
                       " was not found from the test's supported elements list";
            }
        }
        // Check was ok
        return null;
    }

    /**
     * Checks that supported elements are found
     * from the test data.
     *
     * @param aSupportedAttributes Array containing the supported fields
     *        that the PIM API list provides
     * @param aTestSupportedAttributes Integer containing supported attributes which
     *        the test expects to be supported
     * @return String containing the error decription. null if no errors occured
     */
    private static String checkSupportedAttributes(
        int[] aSupportedAttributes,
        int aTestSupportedAttributes)
    {
        boolean found = false;
        int singleBit = 1;  // 0001
        int j = 0;

        while (singleBit <= PIMItem.EXTENDED_ATTRIBUTE_MIN_VALUE)
        {
            int singleAttribute = (aTestSupportedAttributes & singleBit);

            if (singleAttribute != 0)
            {
                System.out.println("Log: Checking getSupportedAttributes() "
                                   + "attribute: " + singleAttribute);
                for (j = 0; j < aSupportedAttributes.length; j++)
                {
                    if (singleAttribute == aSupportedAttributes[ j ])
                    {
                        found = true;
                    }
                }
                // Element was not found
                if (!found)
                {
                    return "Supported element " + aSupportedAttributes[ j ] +
                           " was not found from the supported elements list";
                }
            }
            singleBit <<= 1;
            found = false;
        }
        return null;
    }
}

// End of file

