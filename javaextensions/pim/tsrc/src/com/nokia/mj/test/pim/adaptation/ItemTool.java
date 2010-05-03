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


//PACKAGE
package com.nokia.mj.test.pim.adaptation;
//IMPORTS
import java.io.ByteArrayInputStream;
import java.io.ByteArrayOutputStream;
import java.io.UnsupportedEncodingException;
import java.util.*;
import javax.microedition.pim.*;

// CLASS DEFINITION
/**
 * Tool for filling and comparing items.
 */
public class ItemTool
{
    // Constants
    /**
     * Constant indicating that only differences between two items
     * should be logged.
     */
    public static final int LOG_DIFF = 1;

    /**
     * Constant indicating that only equalities within two items
     * should be logged.
     */
    public static final int LOG_EQUAL = 2;

    /**
     * Constant indicating that both differences and equalities
     * within two items should be logged.
     * Same as LOG_DIFF | LOG_EQUAL.
     */
    public static final int LOG_ALL = 3;

    // Test data
    public static final boolean TEST_BOOLEAN = true;
    public static final long TEST_DATE = 946080000000L;
    public static final int TEST_INT_CLASS = 200; // Fits also to CLASS
    public static final int TEST_INT = 5; // Priority etc...
    public static final String TEST_STRING = "TestString";
    public static final String TEST_BINARY = "/9j/4AAQSkZJRgABAQEBLAEsAAD/2wBDAAgGBgcGBQgHBwcJCQgKDBQNDAsLDBkSEw8UHRof\nHh0aHBwgJC4nICIsIxwcKDcpLDAxNDQ0Hyc5PTgyPC4zNDL/2wBDAQkJCQwLDBgNDRgyIRwh\nMjIyMjIyMjIyMjIyMjIyMjIyMjIyMjIyMjIyMjIyMjIyMjIyMjIyMjIyMjIyMjIyMjL/wAAR\nCAAgACADASIAAhEBAxEB/8QAHwAAAQUBAQEBAQEAAAAAAAAAAAECAwQFBgcICQoL/8QAtRAA\nAgEDAwIEAwUFBAQAAAF9AQIDAAQRBRIhMUEGE1FhByJxFDKBkaEII0KxwRVS0fAkM2JyggkK\nFhcYGRolJicoKSo0NTY3ODk6Q0RFRkdISUpTVFVWV1hZWmNkZWZnaGlqc3R1dnd4eXqDhIWG\nh4iJipKTlJWWl5iZmqKjpKWmp6ipqrKztLW2t7i5usLDxMXGx8jJytLT1NXW19jZ2uHi4+Tl\n5ufo6erx8vP09fb3+Pn6/8QAHwEAAwEBAQEBAQEBAQAAAAAAAAECAwQFBgcICQoL/8QAtREA\nAgECBAQDBAcFBAQAAQJ3AAECAxEEBSExBhJBUQdhcRMiMoEIFEKRobHBCSMzUvAVYnLRChYk\nNOEl8RcYGRomJygpKjU2Nzg5OkNERUZHSElKU1RVVldYWVpjZGVmZ2hpanN0dXZ3eHl6goOE\nhYaHiImKkpOUlZaXmJmaoqOkpaanqKmqsrO0tba3uLm6wsPExcbHyMnK0tPU1dbX2Nna4uPk\n5ebn6Onq8vP09fb3+Pn6/9oADAMBAAIRAxEAPwD1fx940tvAfhd9Yng+0yGVIYLfeU812OSN\nwU7cKGbkfw46kV80+IPij4m1rcNQvr+380LKkdvOIo45Y5mZGCqu4BRkYLFiyqxY7VA9G/aX\nvriPTvDtgsmLWaWeaRNo+Z0CBTnrwJH/AD9hXimBPZbYV2qtrnzbckYIX51kXP8AEV698DrT\njiZUJKUUvmdeFwqxCkr6rb+v+GV2rnqdp8e9c0/Q4YWtbCe5MQ8hpJDLn96/LkSbhtRVQK2X\nb77OeA3S+F/jfqPiXVBYx6JAjNdWylllysMElyI3dskFmAlhQAD7xZjx8o8CsLXdZh/szyvK\nwEaKgJYqeW3FSFTkA9cn0Ga6/wCDgVPjPo0UcheKNrlYyWz8vky4/wAfxrd4ylNtKmr37smr\ng5UqUaknv/X5a/d0aZ9E/E7wj/wmnga902Jd19F/pNlzj98gOF+8B8wLJknA3Z7V8kvp0f7+\nCVDDqSx7BaMjK8bxkhwwxwx2Hg85J9jX3NXPeIfA3hvxVeW95rGmCe6t0KRzJNJE4U9iyMCR\n1wDnGTjqc8k4uS0dmGGrQpy/eR5ov/hrrzSbt0vr0Pj1rLZbrEbu4a2hZ1usNhAVwcKD3JOB\nnqecV7j8B/h9rGhahe+JdVt5bBJ4HtLezuEImILqxdgcbR8mACMnJPAxu9N8PfD7wr4VnM+j\n6PFBMW3CR3eVkOCuVLk7SQxB24yOua6WlTg47u5eLxEK1uSNred/Ky8v1bP/2Q=="; // an evil face, 32 x 32 pixels, base64-encoded, divided into multiple lines


    /**
     * Hidden constructor.
     */
    private ItemTool()
    {
    }


    /**
     * Fills given item with all supported fields and attributes,
     * single value per field. Unexportable fields are not filled.
     *
     * Notes:
     * @li RepeatRule is not filled for Event.
     */
    public static void fillItem(PIMItem aItem, PIMList aList)
    {
        int[] supportedFields = aList.getSupportedFields();
        for (int fieldIndex = 0;
                fieldIndex < supportedFields.length;
                fieldIndex++)
        {
            int field = supportedFields[ fieldIndex ];

            if (!isExportable(field, aItem))
            {
                // The field is not OK to export for this item - skip
                continue;
            }

            int fieldDataType = aList.getFieldDataType(field);
            int attributes = getSupportedAttributesCombined(field, aList);

            if (fieldDataType == PIMItem.BINARY)
            {
                byte[] binaryValue = createBinaryValue(field, aItem);
                aItem.addBinary(field,
                                attributes,
                                binaryValue,
                                0,
                                binaryValue.length);
            }
            else if (fieldDataType == PIMItem.BOOLEAN)
            {
                aItem.addBoolean(field, attributes, TEST_BOOLEAN);
            }
            else if (fieldDataType == PIMItem.DATE)
            {
                aItem.addDate(field, attributes, TEST_DATE);
            }
            else if (fieldDataType == PIMItem.INT)
            {
                if ((field == Contact.CLASS) ||
                        (field == Event.CLASS) ||
                        (field == ToDo.CLASS))
                {
                    aItem.addInt(field, attributes, TEST_INT_CLASS);
                }
                else
                {
                    aItem.addInt(field, attributes, TEST_INT);
                }
            }
            else if (fieldDataType == PIMItem.STRING)
            {
                aItem.addString(field, attributes, TEST_STRING);
            }
            else if (fieldDataType == PIMItem.STRING_ARRAY)
            {
                String[] stringArrayValue = createStringArrayValue(
                                                field,
                                                aList);

                aItem.addStringArray(field, attributes, stringArrayValue);
            }
            // else ..? assume sane types, we're not testing them here
        }
    }


    /**
     * Checks whether given field is ok to export for given item
     * (type), i.e. can it be transformed to serialized form and back
     * without loss of data, distortion etc.
     */
    public static boolean isExportable(int aField, PIMItem aItem)
    {
        // principle: pass by default, fail for specific reason

        boolean isOk = true;
        if (aItem instanceof Contact)
        {
            if (aField == Contact.UID)
            {
                isOk = false;
            }
        }
        else if (aItem instanceof Event)
        {
            if (aField == Event.UID)
            {
                isOk = false;
            }
        }
        else  // assume ToDo
        {
            if (aField == ToDo.UID)
            {
                isOk = false;
            }
        }
        return isOk;
    }


    /**
     * Works only on Contact.PHOTO which is the only supported
     * BINARY field in the system.
     */
    public static byte[] createBinaryValue(int aField, PIMItem aItem)
    {
        return TEST_BINARY.getBytes();
    }


    public static String[] createStringArrayValue(int aField, PIMList aList)
    {
        String[] stringArray = null;
        int numElems = aList.stringArraySize(aField);
        stringArray = new String[ numElems ];
        int[] supportedElems = aList.getSupportedArrayElements(aField);
        // iterate throught the supported elements and fill corresponding
        // indexes in the string array
        for (int i = 0; i < supportedElems.length; i++)
        {
            int elem = supportedElems[ i ];
            stringArray[ elem ] = TEST_STRING;
        }
        return stringArray;
    }


    public static int getSupportedAttributesCombined(
        int aField,
        PIMList aList)
    {
        int[] supportedAttributes = aList.getSupportedAttributes(aField);
        int supportedAttributesCombined = 0;
        for (int i = 0; i < supportedAttributes.length; i++)
        {
            supportedAttributesCombined += supportedAttributes[ i ];
        }
        return supportedAttributesCombined;
    }


    //@{
    /**
     * Compares two items for equality.
     *
     * If the \a aLog has been given, complete comparison is carried
     * out to some reasonable extent and all found differences are
     * logged (unless they are of completely different types).
     *
     * @param aItem1 First item to be compared.
     * @param aItem2 Second item to be compared.
     * @param aLog If not null, log messages about the comparison
     *        are written here.
     * @param aList If not null, labels are resolved from this list.
     * @param aLogOpt Logging options: LOG_DIFF, LOG_EQUAL or LOG_ALL
     *        or a bitwise combination of these.
     */
    public static boolean isEqualItems(
        PIMItem aItem1,
        PIMItem aItem2,
        StringBuffer aLog,
        PIMList aList)
    {
        return isEqualItems(
                   aItem1,
                   aItem2,
                   aLog,
                   aList,
                   LOG_ALL);
    }

    public static boolean isEqualItems(
        PIMItem aItem1,
        PIMItem aItem2,
        StringBuffer aLog,
        PIMList aList,
        int aLogOpt)
    {
        // Implementation note: The &= assignment for boolean results in
        // true if and only if both the old value and assigned value
        // are true. (I.e. the value of a boolean assigned with &= can only
        // change from true to false, not from false to true.)

        // Compares equality of two items
        // Logs differences

        logLn(aLog, "Comparison of two items started.");

        boolean isEqual = true;

        isEqual &= isEqualItemType(aItem1, aItem2, aLog);

        // Carry on with the comparison only if the items are of
        // same item type.
        if (isEqual)
        {
            isEqual &= isEqualContent(aItem1, aItem2, aLog, aList, aLogOpt);
        }

        if (isEqual)
        {
            logLn(aLog, "--- The items are EQUAL.");
        }
        else
        {
            logLn(aLog, "--- The items DIFFER.");
        }

        return isEqual;
    }
    // @}


    public static boolean isEqualItemType(
        PIMItem aItem1,
        PIMItem aItem2,
        StringBuffer aLog)
    {
        // it is essential that the initial values are:
        // 1) not the same
        // 2) not any of the valid item (list) types
        int itemType1 = -1;
        int itemType2 = -2;

        if (aItem1 instanceof Contact)
        {
            itemType1 = PIM.CONTACT_LIST;
            log(aLog, "  First item is Contact, ");
        }
        else if (aItem1 instanceof Event)
        {
            itemType1 = PIM.EVENT_LIST;
            log(aLog, "  First item is Event, ");
        }
        else if (aItem1 instanceof ToDo)
        {
            itemType1 = PIM.TODO_LIST;
            log(aLog, "  First item is ToDo, ");
        }
        else
        {
            log(aLog, "First item is of unknown type, ");
        }

        if (aItem2 instanceof Contact)
        {
            itemType2 = PIM.CONTACT_LIST;
            logLn(aLog, "second item is Contact.");
        }
        else if (aItem2 instanceof Event)
        {
            itemType2 = PIM.EVENT_LIST;
            logLn(aLog, "second item is Event.");
        }
        else if (aItem2 instanceof ToDo)
        {
            itemType2 = PIM.TODO_LIST;
            logLn(aLog, "second item is ToDo.");
        }
        else
        {
            logLn(aLog, "second item is of unknown type.");
        }

        return (itemType1 == itemType2);
    }


    public static boolean isEqualContent(
        PIMItem aItem1,
        PIMItem aItem2,
        StringBuffer aLog,
        PIMList aList,
        int aLogOpt)
    {
        boolean isEqual = true;

        int[] fields1 = aItem1.getFields();
        int[] fields2 = aItem2.getFields();

        if (fields1.length == fields2.length)
        {
            logLn(aLog, "    Equal amount of fields: " + fields1.length,
                  LOG_EQUAL, aLogOpt);
        }
        else
        {
            isEqual = false;
            logLn(aLog, "    - Different number of fields: "
                  + fields1.length + " vs " + fields2.length,
                  LOG_DIFF, aLogOpt);
        }

        // Iterate through fields in first item.
        // Log commonalities and fields only in first item.
        for (int fi1 = 0; fi1 < fields1.length; fi1++)
        {
            String fieldString = getFieldId(fields1[ fi1 ], aList);

            int fi2;
            for (fi2 = 0; fi2 < fields2.length; fi2++)
            {
                if (fields1[ fi1 ] == fields2[ fi2 ])
                {
                    logLn(aLog, "    Common field: " + fieldString,
                          LOG_EQUAL, aLogOpt);

                    isEqual &= isEqualValues(
                                   aItem1,
                                   aItem2,
                                   fields1[ fi1 ],
                                   aLog,
                                   aList,
                                   aLogOpt);

                    break; // next field from first item
                }
            }
            if (fi2 == fields2.length)
            {
                // no match in the second item
                isEqual = false;
                logLn(aLog,  "    - Field in first item only: "
                      + fieldString,
                      LOG_DIFF, aLogOpt);
            }
        }

        // Iterate through fields in second item.
        // Log fields only in second item.
        for (int fi2 = 0; fi2 < fields2.length; fi2++)
        {
            String fieldString = getFieldId(fields2[ fi2 ], aList);

            int fi1;
            for (fi1 = 0; fi1 < fields1.length; fi1++)
            {
                if (fields2[ fi2 ] == fields1[ fi1 ])
                {
                    break; // match - skip; it has been logged already
                }
            }
            if (fi1 == fields1.length)
            {
                isEqual = false;
                logLn(aLog, "    - Field in second item only: "
                      + fieldString,
                      LOG_DIFF, aLogOpt);
            }
        }

        return isEqual;
    }


    public static boolean isEqualValues(
        PIMItem aItem1,
        PIMItem aItem2,
        int aField,
        StringBuffer aLog,
        PIMList aList,
        int aLogOpt)
    {
        boolean isEqual = true;

        int numValues1 = aItem1.countValues(aField);
        int numValues2 = aItem2.countValues(aField);

        String fieldString = getFieldId(aField, aList);

        if (numValues1 == numValues2)
        {
            logLn(aLog, "    Equal number of values: " + numValues1,
                  LOG_EQUAL, aLogOpt);
        }
        else
        {
            isEqual = false;
            logLn(aLog, "    * Different number of values in field "
                  + fieldString + ": "
                  + numValues1 + " vs " + numValues2,
                  LOG_DIFF, aLogOpt);
        }

        if (aList != null)
        {
            // Values can be compared only if the list has been
            // given; without one we cannot determine field types

            int fieldType = aList.getFieldDataType(aField);

            for (int vi1 = 0; vi1 < numValues1; vi1++)
            {
                int vi2;
                for (vi2 = 0; vi2 < numValues2; vi2++)
                {
                    isEqual &= isEqualValue(
                                   aItem1,
                                   aItem2,
                                   aField,
                                   fieldType,
                                   vi1,
                                   vi2,
                                   aLog,
                                   aList,
                                   aLogOpt);

                    isEqual &= isEqualAttributes(
                                   aItem1,
                                   aItem2,
                                   aField,
                                   fieldType,
                                   vi1,
                                   vi2,
                                   aLog,
                                   aList,
                                   aLogOpt);
                }
            }
        }

        return isEqual;
    }


    public static boolean isEqualValue(
        PIMItem aItem1,
        PIMItem aItem2,
        int aField,
        int aFieldType,
        int aI1,
        int aI2,
        StringBuffer aLog,
        PIMList aList,
        int aLogOpt)
    {
        boolean isEqual = true;

        String fieldId = getFieldId(aField, aList);

        if (aFieldType == PIMItem.BINARY)
        {
            byte[] value1 = aItem1.getBinary(aField, aI1);
            byte[] value2 = aItem2.getBinary(aField, aI2);
            if (value1.length == value2.length)
            {
                for (int i = 0; i < value1.length; i++)
                {
                    if (value1[ i ] != value2[ i ])
                    {
                        isEqual = false;
                        break;
                    }
                }
            }
            else
            {
                isEqual = false;
            }

            if (isEqual == true)
            {
                logLn(aLog, "      Equal binary values.",
                      LOG_EQUAL, aLogOpt);
            }
            else
            {
                logLn(aLog, "      Binary values differ in field "
                      + fieldId + " value " + aI1,
                      LOG_DIFF, aLogOpt);
            }
        }
        else if (aFieldType == PIMItem.BOOLEAN)
        {
            boolean value1 = aItem1.getBoolean(aField, aI1);
            boolean value2 = aItem2.getBoolean(aField, aI2);
            if (value1 == value2)
            {
                logLn(aLog, "      Equal values: " +
                      booleanString(value1),
                      LOG_EQUAL, aLogOpt);
            }
            else
            {
                isEqual = false;
                logLn(aLog, "      Boolean values differ in field "
                      + fieldId + " value " + aI1 + ": " +
                      booleanString(value1) + " vs " +
                      booleanString(value2),
                      LOG_DIFF, aLogOpt);
            }
        }
        else if (aFieldType == PIMItem.DATE)
        {
            long value1 = aItem1.getDate(aField, aI1);
            long value2 = aItem2.getDate(aField, aI2);
            if (value1 == value2)
            {
                logLn(aLog, "      Equal values: " + value1,
                      LOG_EQUAL, aLogOpt);
            }
            else
            {
                isEqual = false;
                logLn(aLog, "      Date values differ in field "
                      + fieldId + " value " + aI1 + ": " +
                      value1 + " vs " + value2,
                      LOG_DIFF, aLogOpt);
            }
        }
        else if (aFieldType == PIMItem.INT)
        {
            int value1 = aItem1.getInt(aField, aI1);
            int value2 = aItem2.getInt(aField, aI2);
            if (value1 == value2)
            {
                logLn(aLog, "      Equal values: " + value1,
                      LOG_EQUAL, aLogOpt);
            }
            else
            {
                isEqual = false;
                logLn(aLog, "      Int values differ in field "
                      + fieldId + " value " + aI1 + ": " +
                      value1 + " vs " + value2,
                      LOG_DIFF, aLogOpt);
            }
        }
        else if (aFieldType == PIMItem.STRING)
        {
            String value1 = aItem1.getString(aField, aI1);
            String value2 = aItem2.getString(aField, aI2);
            if (value1.equals(value2))
            {
                logLn(aLog, "      Equal values: \"" + value1 + "\"",
                      LOG_EQUAL, aLogOpt);
            }
            else
            {
                isEqual = false;
                logLn(aLog, "      String values differ in field "
                      + fieldId + " value " + aI1 + ": \"" + value1
                      + "\" vs \"" + value2 + "\"",
                      LOG_DIFF, aLogOpt);
            }
        }
        else if (aFieldType == PIMItem.STRING_ARRAY)
        {
            String[] value1 = aItem1.getStringArray(aField, aI1);
            String[] value2 = aItem2.getStringArray(aField, aI2);
            // assume that we can only get string arrays of correct size
            for (int i = 0; i < value1.length; i++)
            {
                String elementString = "" + i;
                if (aList != null &&
                        aList.isSupportedArrayElement(aField, i))
                {
                    elementString = aList.getArrayElementLabel(aField, i)
                                    + "(" + i + ")";
                }

                String elem1 = value1[ i ];
                String elem2 = value2[ i ];
                if (elem1 == null)
                {
                    if (elem2 == null)
                    {
                        continue; // two null elements are not very interesting
                    }
                    else
                    {
                        isEqual = false;
                        logLn(aLog, "          > Field " + fieldId
                              + " value " + aI1
                              + " element " + elementString
                              + " \"" + elem2 + "\" "
                              + "only in the second item.",
                              LOG_DIFF, aLogOpt);
                    }
                }
                else
                {
                    if (elem2 == null)
                    {
                        isEqual = false;
                        logLn(aLog, "          < Field " + fieldId
                              + " value " + aI1
                              + " element " + elementString
                              + "\"" + elem1 + "\""
                              + "only in the first item.",
                              LOG_DIFF, aLogOpt);
                    }
                    else
                    {
                        if (elem1.equals(elem2))
                        {
                            logLn(aLog, "          Equal elements "
                                  + elementString + ": "
                                  + "\"" + elem1 + "\"",
                                  LOG_EQUAL, aLogOpt);
                        }
                        else
                        {
                            isEqual = false;
                            logLn(aLog, "          * Different elements in field "
                                  + fieldId + " value " + aI1
                                  + " element " + elementString + ": "
                                  + "\"" + elem1 + "\" vs \""
                                  + elem2 + "\"",
                                  LOG_DIFF, aLogOpt);
                        }
                    }
                }
            }
        }
        // else? assume that we'll get only sane field types

        return isEqual;
    }


    public static String booleanString(boolean aValue)
    {
        return (aValue ? "true" : "false");
    }


    public static boolean isEqualAttributes(
        PIMItem aItem1,
        PIMItem aItem2,
        int aField,
        int aFieldType,
        int aI1,
        int aI2,
        StringBuffer aLog,
        PIMList aList,
        int aLogOpt)
    {
        boolean isEqual = true;

        String fieldId = getFieldId(aField, aList);

        int attributesCombined1 = aItem1.getAttributes(aField, aI1);
        int attributesCombined2 = aItem2.getAttributes(aField, aI2);

        int[] attributes1 = extractAttributes(attributesCombined1);
        int[] attributes2 = extractAttributes(attributesCombined2);

        if (attributes1.length == attributes2.length)
        {
            logLn(aLog, "      Equal number of attributes: "
                  + attributes1.length,
                  LOG_EQUAL, aLogOpt);
        }
        else
        {
            isEqual = false;
            logLn(aLog, "      * Different number of attributes in field "
                  + fieldId + " value " + aI1 + ": "
                  + attributes1.length + " vs " + attributes2.length,
                  LOG_DIFF, aLogOpt);
        }

        // Iterate through attributes in the first set.
        // Log commonalities and attributes on in first set.
        for (int ai1 = 0; ai1 < attributes1.length; ai1++)
        {
            String attributeString = getAttributeId(
                                         attributes1[ ai1 ], aList);

            int ai2;
            for (ai2 = 0; ai2 < attributes2.length; ai2++)
            {
                if (attributes1[ ai1 ] == attributes2[ ai2 ])
                {
                    logLn(aLog, "        Equal attribute: "
                          + attributeString,
                          LOG_EQUAL, aLogOpt);

                    break; // next attribute from the first set
                }
            }
            if (ai2 == attributes2.length)
            {
                // no match in the second set
                isEqual = false;
                logLn(aLog, "        - Field " + fieldId
                      + " value " + aI1
                      + " attribute in first item only: "
                      + attributeString,
                      LOG_DIFF, aLogOpt);
            }
        }

        // Iterate through attributes in the second set.
        // Log attributes only in second set.
        for (int ai2 = 0; ai2 < attributes2.length; ai2++)
        {
            String attributeString = getAttributeId(
                                         attributes2[ ai2 ], aList);

            int ai1;
            for (ai1 = 0; ai1 < attributes1.length; ai1++)
            {
                if (attributes2[ ai2 ] == attributes1[ ai1 ])
                {
                    break; // match - skip; it has been logged already
                }
            }
            if (ai1 == attributes1.length)
            {
                isEqual = false;
                logLn(aLog, "        - Field " + fieldId
                      + " value " + aI1
                      + " attribute in second item only: "
                      + attributeString,
                      LOG_DIFF, aLogOpt);
            }
        }

        return isEqual;
    }


    public static int[] extractAttributes(int aAttributes)
    {
        Vector attributeArray = new Vector(); // contains Integer objects

        int attributes = aAttributes;
        int mask = 1;
        while (attributes > 0)
        {
            if ((attributes & mask) == mask)
            {
                attributeArray.addElement(new Integer(mask));
                attributes &= ~mask;
            }
            mask *= 2;
        }

        int[] retVal = new int[ attributeArray.size()];
        for (int i = 0; i < attributeArray.size(); i++)
        {
            retVal[ i ] = ((Integer)(attributeArray.elementAt(i)))
                          .intValue();
        }
        return retVal;
    }


    public static String getFieldId(
        int aField,
        PIMList aList)
    {
        return aList != null?
               aList.getFieldLabel(aField) + "(" + aField + ")" :
               "" + aField;
    }


    public static String getAttributeId(
        int aAttribute,
        PIMList aList)
    {
        return aList != null ?
               aList.getAttributeLabel(aAttribute) + "(" + aAttribute + ")" :
               "" + aAttribute;
    }


    // @{
    /**
     * Appends given string to \a aLog if \a aLog is non-null.
     * Otherwise does nothing.
     * @param aLog The log to write into.
     * @param aMsg The message to write.
     * @param aMsgType Type of the message, LOG_DIFF or LOG_EQUAL.
     * @param aLogOpt Logging options. If \a aMsgType is found among
     *        \a aLogOpt, the message is logged; otherwise it is
     *        discarded.
     */

    public static void log(
        StringBuffer aLog,
        String aMsg)
    {
        // Just write it without caring any options
        log(aLog, aMsg, LOG_ALL, LOG_ALL);
    }

    public static void log(
        StringBuffer aLog,
        String aMsg,
        int aMsgType,
        int aLogOpt)
    {
        if (aLog != null && ((aLogOpt & aMsgType) == aMsgType))
        {
            aLog.append(aMsg);
        }
    }
    // @}

    // @{
    /**
     * As log() but appends a newline to the end.
     */
    public static void logLn(
        StringBuffer aLog,
        String aMsg)
    {
        // Just write it without caring any options
        logLn(aLog, aMsg, LOG_ALL, LOG_ALL);
    }


    public static void logLn(
        StringBuffer aLog,
        String aMsg,
        int aMsgType,
        int aLogOpt)
    {
        if (aLog != null && ((aLogOpt & aMsgType) == aMsgType))
        {
            aLog.append(aMsg).append('\n');
        }
    }
    // @}

}
