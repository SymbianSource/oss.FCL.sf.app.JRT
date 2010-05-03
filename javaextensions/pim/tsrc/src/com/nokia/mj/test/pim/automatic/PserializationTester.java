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
import java.io.ByteArrayOutputStream;
import java.io.UnsupportedEncodingException;

// CLASS DEFINITION
/**
 * Base that tests the items of a PIM list
 *
 * Tests following features:
 '    - When item is exported the stream should coutain
 '      expected result
 *
 * This class is common for all item test and
 * must not be changed unless you are implementing
 * new common test for all platforms or updating
 * some tests.
 */

public class PserializationTester
{
    // Static constants
    public static String STRING_VCARD_FORMAT = "VCARD/2.1";
    public static String STRING_VCALENDAR_FORMAT = "VCALENDAR/1.0";

    // Private. Used in serialization check
    private static String STRING_VCARD_START_FIELD = "BEGIN:VCARD";
    private static String STRING_VCALENDAR_START_FIELD = "BEGIN:VCALENDAR";
    private static String STRING_VERSION_10 = "VERSION:1.0";
    private static String STRING_VERSION_21 = "VERSION:2.1";
    private static String STRING_VTODO_START = "BEGIN:VTODO";
    private static String STRING_VEVENT_START = "BEGIN:VEVENT";
    private static String STRING_VCARD_END_FIELD = "END:VCARD";
    private static String STRING_VCALENDAR_END_FIELD = "END:VCALENDAR";
    private static String STRING_VTODO_END = "END:VTODO";
    private static String STRING_VEVENT_END = "END:VEVENT";
    /**
     * Constructor. Prohibit construction
     */
    private void PserializationTester()
    {
    }

    /**
     * Tests PIM.toSerialFormat(). Tests that the result of the function corresponds
     * the test data (i.e the versit result includes the expected test string
     *
     * @param aList Tested PIM List
     * @param aItem Tested PIM item
     * @param aTestData Test data including fields, values and versit results
     * @param aEncoding Encoding used in conversion
     * @param aSerialFormat Serial format used in conversion
     */
    public static String testToSerialFormat(
        PIMList aList,
        PIMItem aItem,
        SupportTestData aTestData,
        String aEncoding,
        String aSerialFormat)
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
            Vector versitValues = aTestData.getVersitValue(field);

            // Test all combinations
            for (int j = 0; j < attributeCombinations.size(); j++)
            {
                int attributes = ((Integer) attributeCombinations.elementAt(j)).intValue();

                // Get test values
                Object value = fieldValues.elementAt(j);
                String versitValue = ((String) versitValues.elementAt(j)).toString();

                System.out.println("Log: PserializationTester: Testing field: " + field +
                                   ". Attributes: " + attributes + ". Value: " + value.toString());

                // Commit item with specific attributes and value
                if (!addFieldValue(aList, aItem, field, attributes, value))
                {
                    return "Could not add values to field: " + field;
                }
                // Serialize item
                ByteArrayOutputStream bs = new ByteArrayOutputStream();
                PIM pim = PIM.getInstance();

                try
                {
                    pim.toSerialFormat(aItem, bs, aEncoding, aSerialFormat);
                }
                catch (PIMException pe)
                {
                    pe.printStackTrace();
                    return "Error issuing PIM.toSerialFormat()";
                }
                catch (UnsupportedEncodingException uee)
                {
                    uee.printStackTrace();
                    return "Unsupported encoding exception when issuing "
                           + " PIM.toSerialFormat()";
                }
                // Check serialization
                if (!checkSerialization(aItem, bs, versitValue, aSerialFormat))
                {
                    System.out.println("Serialized item: " + bs.toString());
                    return "Serialization for field " + field + " failed";
                }
            }
        }
        // Success
        return null;
    }

    private static boolean addFieldValue(
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
            System.out.println("Log: PserializationTester::addFieldValue: Data type of the field is: " + dataType);
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
                float value = ((Long) aValue).longValue();
                Float floatValue = new Float(value);
                int intValue = floatValue.intValue();
                aItem.addInt(
                    aField,
                    aAttributes,
                    intValue);
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
        return true;
    }

    private static boolean checkSerialization(
        PIMItem aItem,
        ByteArrayOutputStream aByteStream,
        String aExpectedValue,
        String aSerialFormat)
    {
        if (aExpectedValue == null)
        {
            return false;
        }
        String stringStream = aByteStream.toString();

        // Test that required fields are in place
        if (aSerialFormat.equals(STRING_VCARD_FORMAT))
        {
            System.out.println("Log: Testing vCard standard fields");
            if ((stringStream.indexOf(STRING_VCARD_START_FIELD) == -1) ||
                    (stringStream.indexOf(STRING_VERSION_21) == -1) ||
                    (stringStream.indexOf(STRING_VCARD_END_FIELD) == -1))
            {
                System.out.println("Log: Failed!!!");
                return false;
            }
        }
        // vCalendar standard fields
        else if (aSerialFormat.equals(STRING_VCALENDAR_FORMAT))
        {
            System.out.println("Log: Testing vCalendar standard fields");
            if ((stringStream.indexOf(STRING_VCALENDAR_START_FIELD) == -1) ||
                    (stringStream.indexOf(STRING_VERSION_10) == -1) ||
                    (stringStream.indexOf(STRING_VCALENDAR_END_FIELD) == -1))
            {
                System.out.println("Log: Failed!!!");
                return false;
            }
            // Check specific item
            if (aItem instanceof Event)
            {
                System.out.println("Log: Testing Event standard fields");
                if ((stringStream.indexOf(STRING_VEVENT_START) == -1) ||
                        (stringStream.indexOf(STRING_VEVENT_END) == -1))
                {
                    System.out.println("Log: Failed!!!");
                    return false;
                }
            }
            else if (aItem instanceof ToDo)
            {
                System.out.println("Log: Testing ToDo standard fields");
                if ((stringStream.indexOf(STRING_VTODO_START) == -1) ||
                        (stringStream.indexOf(STRING_VTODO_END) == -1))
                {
                    System.out.println("Log: Failed!!!");
                    return false;
                }
            }
            else
            {
                System.out.println("Log: Error: Incorrect item converted using vCalendar conversion");
                return false;
            }
        }
        else
        {
            System.out.println("Log: Error: Incorrect serial format");
            return false;
        }

        System.out.println("Log: PSerializationTester::checkSerialization: Testing versit value "
                           + aExpectedValue);
        // Check that the specified string can be found from
        // the result stream
        if (stringStream.indexOf(aExpectedValue) != -1)
        {
            return true;
        }

        // Not ok
        return false;
    }
}

