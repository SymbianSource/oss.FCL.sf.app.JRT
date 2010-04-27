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
package com.nokia.mj.test.pim.automatic;
//IMPORTS
import java.io.IOException;
import java.io.InputStream;
import java.io.UnsupportedEncodingException;
import java.util.Hashtable;
import javax.microedition.pim.*;
import com.nokia.mj.test.pim.adaptation.*;

// CLASS DEFINITION
/**
 * Tests maximum lengths of string values and string array elements in
 * a PIMList.
 *
 * Derive the list-specific test classes from this class and implement
 * the initTest() and cleanupTest() functions.
 */
public class PLstringMaxLen
        extends MIDPTest
{
    protected static final int IGNORE = -1;
    protected static final int UNLIMITED = -2;
    /**
     * TEST DATA
     * Maximum lengths of string fields.
     * Keys: Integer (field)
     * Values: Integer (max length)
     */
    protected Hashtable iStringFieldMaxLengths = new Hashtable();

    /**
     * TEST DATA
     * Maximum lengths of string array elements.
     * Keys: Integer (field)
     * Values: int[] (max lengths)
     */
    protected Hashtable iStringArrayElemMaxLengths = new Hashtable();

    /**
     * Derived class must place the tested list here in initTest().
     */
    protected PIMList iPimList;

    /**
     * Derived class must create and place a test item here in initTest().
     */
    protected PIMItem iPimItem;

    public PLstringMaxLen(
        String sTestName,
        j2meunit.framework.TestMethod rTestMethod)
    {
        super(sTestName, rTestMethod);
    }
    /**
     * Test body.
     * Runs testStringMaxLen for string fields and
     * testStrArrElemMaxLen for string array fields.
     */
    public void runTest()
    throws TestEndThrowable
    {
        initTest();
        if (iStringFieldMaxLengths == null ||
                iStringArrayElemMaxLengths == null ||
                iPimList == null ||
                iPimItem == null)
        {
            failTest("The test has not been initialized. "
                     + "Implement initTest() and cleanupTest() in derived class");
        }

        //log( "CLstringMaxLen.runTest()" );
        int[] supportedFields = iPimList.getSupportedFields();

        for (int i = 0; i < supportedFields.length; i++)
        {
            int field = supportedFields[ i ];
            int fieldType = iPimList.getFieldDataType(field);

            if (fieldType == PIMItem.STRING)
            {
                log("string field: " + field);

                // determine maxLen, fiddling with the key and value objects
                Integer fieldObj = new Integer(field);

                Integer maxLengthObj =
                    (Integer) iStringFieldMaxLengths.get(fieldObj);

                if (maxLengthObj == null)
                {
                    failTest("Maximum length is not defined for field: "
                             + field);
                }

                int maxLength = maxLengthObj.intValue();
                if (maxLength > 0)
                {
                    log("string field maxLength: " + maxLength);
                    testStringMaxLen(iPimItem, field, maxLength);
                }
                else
                {
                    log("skipping field " + field);
                }
            }
            else if (fieldType == PIMItem.STRING_ARRAY)
            {
                log("string array field: " + field);

                // determine array of maxLens
                int[] maxLengths = (int[])
                                   iStringArrayElemMaxLengths.get(new Integer(field));

                if (maxLengths == null)
                {
                    failTest("Maximum lengths are not defined for field: "
                             + field);
                }

                int stringArraySize = iPimList.stringArraySize(field);

                int[] supportedArrayElements =
                    iPimList.getSupportedArrayElements(field);

                testStrArrElemMaxLen(
                    iPimItem,
                    field,
                    stringArraySize,
                    supportedArrayElements,
                    maxLengths);
            }
        }
        cleanupTest();
    }


    protected void testStringMaxLen(
        PIMItem aItem,
        int aField,
        int aMaxLength)
    throws TestEndThrowable
    {
        //log( "CLstringMaxLen.testStringMaxLen()" );
        // a string with aMaxLength chars is needed
        StringBuffer testChars = new StringBuffer(aMaxLength + 1);
        for (int i = 0; i < aMaxLength; i++)
        {
            testChars.append('a');
        }

        // test with correct value
        try
        {
            log("testing field " + aField + " with correct value.");

            while (aItem.countValues(aField) > 0)
            {
                aItem.removeValue(aField, 0);
            }

            aItem.addString(aField, PIMItem.ATTR_NONE, testChars.toString());
            aItem.commit();
            aItem.removeValue(aField, 0);
            aItem.commit();
        }
        catch (Exception e)
        {
            e.printStackTrace();
            failTest("unexpected exception when adding correct "
                     + "value. " + e);
        }

        // test with too long a value
        testChars.append('b');   // length now aMaxLength + 1
        try
        {


            while (aItem.countValues(aField) > 0)
            {
                aItem.removeValue(aField, 0);
            }

            aItem.addString(aField, PIMItem.ATTR_NONE, testChars.toString());

            aItem.commit();

            failTest("testStringMaxLen: a PIMException should have been "
                     + "thrown when committing with too long a value.");
        }
        catch (PIMException pe)
        {
            // OK, expected
        }

        try
        {
            aItem.removeValue(aField, 0);
            aItem.commit();
        }
        catch (PIMException pe)
        {
            pe.printStackTrace();
            failTest("Unexpected PIMException when removing test value: " + pe);
        }
    }


    protected void testStrArrElemMaxLen(
        PIMItem aItem,
        int aField,
        int aStringArraySize,
        int[] aSupportedArrayElements,
        int[] aMaxLengths)
    throws TestEndThrowable
    {
        //log( "CLstringMaxLen.testStrArrElemMaxLen()" );

        String[] stringArray = new String[ aStringArraySize ];

        for (int i = 0; i < aSupportedArrayElements.length; i++)
        {
            int elemIndex = aSupportedArrayElements[ i ];
            int maxLength = aMaxLengths[ elemIndex ];

            log("string array element: " + elemIndex);
            log("max length: " + maxLength);

            for (int j = 0; j < stringArray.length; j++)
            {
                stringArray[ j ] = "";
            }

            // a string with aMaxLength chars is needed
            StringBuffer testChars = new StringBuffer(maxLength + 1);
            for (int k = 0; k < maxLength; k++)
            {
                testChars.append('a');
            }

            // test with correct value
            try
            {
                log("testing field " + aField + " with correct value.");

                System.out.println("testing field " + aField + " with correct value. element " + elemIndex);

                stringArray[ elemIndex ] = testChars.toString();

                aItem.addStringArray(
                    aField,
                    PIMItem.ATTR_NONE,
                    stringArray);

                if (aItem.countValues(aField) != 1)
                {
                    failTest("testStrArrElemMaxLen: incorrect number of values in the item after addStringArray");
                }

                aItem.commit();

                if (aItem.countValues(aField) != 1)
                {
                    failTest("testStrArrElemMaxLen: incorrect number of values in the item after first commit");
                }

                String[] tempArray = aItem.getStringArray(aField, 0);
                for (int ii = 0; ii < tempArray.length; ii++)
                {
                    if (ii == elemIndex)
                    {
                        if (tempArray[ ii ].length() != testChars.length())
                        {
                            failTest("testStrArrElemMaxLen: incorrect length after commit in element " + i);
                        }
                    }
                    else
                    {
                        if (tempArray[ ii ] != null && tempArray[ ii ].length() != 0)
                        {
                            failTest("testStrArrElemMaxLen: incorrect length after commit in element " + i);
                        }
                    }
                }

                aItem.removeValue(aField, 0);

                if (aItem.countValues(aField) != 0)
                {
                    failTest("testStrArrElemMaxLen: incorrect number of values in the item after removeValue");
                }

                aItem.commit();

                if (aItem.countValues(aField) != 0)
                {
                    failTest("testStrArrElemMaxLen: incorrect number of values in the item after second commit");
                }
            }
            catch (Exception e)
            {
                e.printStackTrace();

                failTest("testStrArrElemMaxLen: unexpected exception when adding"
                         + "correct value. " + e);
            }

            // test with too long a value
            testChars.append('b');   // length now aMaxLength + 1
            try
            {
                log("testing field " + aField + " with incorrect value.");
                System.out.println("testing field " + aField + " with incorrect value. element " + elemIndex);
                stringArray[ elemIndex ] = testChars.toString();

                aItem.addStringArray(
                    aField,
                    PIMItem.ATTR_NONE,
                    stringArray);

                if (aItem.countValues(aField) != 1)
                {
                    failTest("testStrArrElemMaxLen: incorrect number of values in the item after addStringArray");
                }

                aItem.commit();

                failTest("testStrArrElemMaxLen: a PIMException should have "
                         + "been thrown when committing with too long a value.");
            }
            catch (PIMException pe)
            {
                // OK, expected
            }

            try
            {
                if (aItem.countValues(aField) != 1)
                {
                    failTest("testStrArrElemMaxLen: incorrect number of values in the item after addStringArray and commit");
                }

                String[] tempArray = aItem.getStringArray(aField, 0);
                for (int ii = 0; ii < tempArray.length; ii++)
                {
                    if (ii == elemIndex)
                    {
                        if (tempArray[ ii ].length() != testChars.length())
                        {
                            failTest("testStrArrElemMaxLen: incorrect length after failed commit in element " + i);
                        }
                    }
                    else
                    {
                        if (tempArray[ ii ] != null && tempArray[ ii ].length() != 0)
                        {
                            failTest("testStrArrElemMaxLen: incorrect length after failed commit in element " + i);
                        }
                    }
                }

                aItem.removeValue(aField, 0);

                if (aItem.countValues(aField) != 0)
                {
                    failTest("testStrArrElemMaxLen: incorrect number of values in the item after failed commit and removevalue");
                }

                aItem.commit();

                if (aItem.countValues(aField) != 0)
                {
                    failTest("testStrArrElemMaxLen: incorrect number of values in the item after ok commit and removevalue");
                }

            }
            catch (PIMException pe)
            {
                pe.printStackTrace();
                failTest("Unexpected PIMException when removing test value: " + pe);
            }
        }
    }

    protected void initTest() throws TestEndThrowable
    {
        log("PLstringMaxLen::initTest()");
    }

    protected void cleanupTest()
    {
        log("PLstringMaxLen::cleanupTest()");
    }
}

// End of Filé
