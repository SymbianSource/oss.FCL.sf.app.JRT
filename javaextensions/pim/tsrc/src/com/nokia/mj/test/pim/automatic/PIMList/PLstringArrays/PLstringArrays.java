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
package com.nokia.mj.test.pim.automatic.PIMList.PLstringArrays;
//IMPORTS
import java.io.IOException;
import java.util.*;
import javax.microedition.pim.*;
import com.nokia.mj.test.pim.adaptation.*;

// CLASS DEFINITION
/**
 * This class tests javax.microedition.pim.PIMList.items()
 */
public class PLstringArrays
        extends MIDPTest
{

    public PLstringArrays(
        String sTestName, j2meunit.framework.TestMethod rTestMethod)
    {
        super(sTestName, rTestMethod);
    }

    private int[] contactFields = { Contact.ADDR,
                                    Contact.BIRTHDAY,
                                    Contact.CLASS,
                                    Contact.EMAIL,
                                    Contact.FORMATTED_ADDR,
                                    Contact.FORMATTED_NAME,
                                    Contact.NAME,
                                    Contact.NICKNAME,
                                    Contact.NOTE,
                                    Contact.ORG,
                                    Contact.PHOTO,
                                    Contact.PHOTO_URL,
                                    Contact.PUBLIC_KEY,
                                    Contact.PUBLIC_KEY_STRING,
                                    Contact.REVISION,
                                    Contact.TEL,
                                    Contact.TITLE,
                                    Contact.UID,
                                    Contact.URL,
                                    0x1005001,  // Extended fiels
                                    0x1005002,
                                    0x1005003,
                                    0x1005004,
                                    0x1005005,
                                    0x1005006,
                                    0x1005007,
                                    0x1005008,
                                    0x1005009,
                                    0x100500A,
                                    0x100500B
                                  };

    private int[] eventFields = { Event.ALARM,
                                  Event.CLASS,
                                  Event.END,
                                  Event.LOCATION,
                                  Event.NOTE,
                                  Event.REVISION,
                                  Event.START,
                                  Event.SUMMARY,
                                  Event.UID
                                };

    private int[] todoFields = { ToDo.CLASS,
                                 ToDo.COMPLETED,
                                 ToDo.COMPLETION_DATE,
                                 ToDo.DUE,
                                 ToDo.NOTE,
                                 ToDo.PRIORITY,
                                 ToDo.REVISION,
                                 ToDo.SUMMARY,
                                 ToDo.UID,
                                 0x1005001  // Extended fiels
                               };

    private int[] invalidFields = { -100, -1, 0, 666, 2147483647 };
    private int[] invalidIndexes = { -100, -1, 666, 2147483647 };
    private Hashtable supportedFields = new Hashtable();
    private Hashtable unsupportedFields = new Hashtable();


    public void runTest() throws TestEndThrowable
    {
        // preconditions
        PIMList[] allLists = openAllLists(PIM.READ_WRITE);
        // test step1
        classifyFields(allLists);
        // test steps 2, 3, 4, 5, 6, 8, 9
        testStringArrays(allLists);
        // test step 7
        log("Testing isSupportedArrayElementFalse");
        testIsSupportedArrayElementFalse(allLists);

        // test steps 10, 11
        log("Testing getArrayElementLabel: IAE & UFE");
        testGetArrayElementLabelIAE(allLists);
        testGetArrayElementLabelUFE(allLists);

        // test step 12
        log("Testing stringArraySize exceptions");
        testStringArraySizeExceptions(allLists);

        closeLists(allLists);

        passTest();
    }
    private void testGetArrayElementLabelUFE(PIMList[] lists) throws TestEndThrowable
    {
        for (int i = 0; i < lists.length; i++)
        {
            // test that supported fields and unsupported index throw UFE
            int[] fields = (int[]) supportedFields.get(lists[i].getName());
            int testIndex=0;

            for (int j = 0; j<fields.length; j++)
            {
                try
                {
                    int dataType = lists[i].getFieldDataType(fields[j]);
                    if (dataType == PIMItem.STRING_ARRAY)
                    {
                        int[] indexes = lists[i].getSupportedArrayElements(fields[j]);
                        int size = lists[i].stringArraySize(fields[j]);
                        if (size != indexes.length)
                        {
                            log("arraySize: "+size+", indexes: "+asString(indexes));
                            for (int k = 0; k<indexes.length; k++)
                            {
                                // only one unsupported index is tested but should be enough
                                if (k != indexes[k])
                                {
                                    testIndex = indexes[k];
                                    lists[i].getArrayElementLabel(fields[j], indexes[k]);
                                    failTest(lists[i].getName()+".getArrayElementLabel("+fields[j]+
                                             ", <unsupportedIndex="+testIndex+">) did not throw IAE!");
                                }
                            }
                        }
                    }
                }
                catch (UnsupportedFieldException iae)
                {
                    //ok
                }
                catch (Exception e)
                {
                    failTest(lists[i].getName()+".getArrayElementLabel("+fields[j]+
                             ", <unsupportedIndex="+testIndex+">) did not throw IAE but "+e);
                }
            }
            // test that unsupported fields and valid index throw UFE
            int[] ufields = (int[]) unsupportedFields.get(lists[i].getName());

            for (int j = 0; j<ufields.length; j++)
            {
                for (int k = 0; k<invalidIndexes.length; k++)
                {
                    try
                    {
                        if (isUnsupportedSA(lists[i], ufields[j]))
                        {
                            // assumption that 0 is alway valid field...
                            lists[i].getArrayElementLabel(ufields[j], 0);
                        }
                    }
                    catch (UnsupportedFieldException ufe)
                    {
                        //ok
                    }
                    catch (Exception e)
                    {
                        failTest(lists[i].getName()+".getArrayElementLabel("+ufields[j]+
                                 ", "+invalidIndexes[k]+") did not throw IAE but "+e);
                    }
                }
            }
        }
    }

    private void testGetArrayElementLabelIAE(PIMList[] lists) throws TestEndThrowable
    {
        for (int i = 0; i < lists.length; i++)
        {
            // test that supported SA fields and invalid index throw IAE
            int[] fields = (int[]) supportedFields.get(lists[i].getName());

            for (int j = 0; j<fields.length; j++)
            {
                for (int k = 0; k<invalidIndexes.length; k++)
                {
                    try
                    {
                        int dataType = lists[i].getFieldDataType(fields[j]);
                        if (dataType == PIMItem.STRING_ARRAY)
                        {
                            lists[i].getArrayElementLabel(fields[j], invalidIndexes[k]);
                        }
                    }
                    catch (IllegalArgumentException iae)
                    {
                        //ok
                    }
                    catch (Exception e)
                    {
                        failTest(lists[i].getName()+".getArrayElementLabel("+fields[j]+
                                 ", "+invalidIndexes[k]+") did not throw IAE but "+e);
                    }
                }
            }
            // test that unsupported SA fields and invalid index throw IAE
            int[] ufields = (int[]) unsupportedFields.get(lists[i].getName());

            for (int j = 0; j<ufields.length; j++)
            {
                for (int k = 0; k<invalidIndexes.length; k++)
                {
                    try
                    {
                        if (isUnsupportedSA(lists[i], ufields[j]))
                        {
                            lists[i].getArrayElementLabel(ufields[j], invalidIndexes[k]);
                        }
                    }
                    catch (IllegalArgumentException iae)
                    {
                        //ok
                    }
                    catch (Exception e)
                    {
                        failTest(lists[i].getName()+".getArrayElementLabel("+ufields[j]+
                                 ", "+invalidIndexes[k]+") did not throw IAE but "+e);
                    }
                }
            }
            // test that invalid fields and invalid index throw IAE
            for (int j = 0; j<invalidFields.length; j++)
            {
                for (int k = 0; k<invalidIndexes.length; k++)
                {
                    try
                    {
                        // assumption that 0 is alway valid field...
                        lists[i].getArrayElementLabel(invalidFields[j], 0);
                    }
                    catch (IllegalArgumentException iae)
                    {
                        //ok
                    }
                    catch (Exception e)
                    {
                        failTest(lists[i].getName()+".getArrayElementLabel("+invalidFields[j]+
                                 ", "+invalidIndexes[k]+") did not throw IAE but "+e);
                    }
                }
            }
        }
    }

    private void testStringArraySizeExceptions(PIMList[] lists) throws TestEndThrowable
    {
        for (int i = 0; i < lists.length; i++)
        {
            // test that supported non-STRING_ARRAY fields throw IAE
            int[] fields = (int[]) supportedFields.get(lists[i].getName());

            for (int j = 0; j<fields.length; j++)
            {
                try
                {
                    int dataType = lists[i].getFieldDataType(fields[j]);
                    if (dataType != PIMItem.STRING_ARRAY)
                    {
                        lists[i].stringArraySize(fields[j]);
                        failTest(lists[i].getName()+".stringArraySize("+fields[j]+") did not throw IAE!");
                    }
                }
                catch (IllegalArgumentException iae)
                {
                    //ok
                }
                catch (Exception e)
                {
                    failTest(lists[i].getName()+".stringArraySize("+fields[j]+") did not throw IAE but "+e);
                }
            }
            // test that invalid fields throw IAE
            for (int j = 0; j<invalidFields.length; j++)
            {
                try
                {
                    int dataType = lists[i].getFieldDataType(invalidFields[j]);
                    if (dataType != PIMItem.STRING_ARRAY)
                    {
                        lists[i].stringArraySize(invalidFields[j]);
                        failTest(lists[i].getName()+".stringArraySize("+invalidFields[j]+") did not throw IAE!");
                    }
                }
                catch (IllegalArgumentException iae)
                {
                    //ok
                }
                catch (Exception e)
                {
                    failTest(lists[i].getName()+".stringArraySize("+invalidFields[j]+") did not throw IAE but "+e);
                }
            }

            // test unsupported fields
            int[] ufields = (int[]) unsupportedFields.get(lists[i].getName());

            for (int j = 0; j<ufields.length; j++)
            {
                try
                {
                    lists[i].stringArraySize(ufields[j]);
                    if (!isUnsupportedSA(lists[i], ufields[j]))
                    {
                        failTest(lists[i].getName()+".stringArraySize("+ufields[j]+") did not throw IAE!");
                    }
                }
                catch (IllegalArgumentException iae)
                {
                    //ok
                    if (isUnsupportedSA(lists[i], ufields[j]))
                    {
                        failTest(lists[i].getName()+".stringArraySize("+ufields[j]+") threw IAE!");
                    }
                }
                catch (Exception e)
                {
                    failTest(lists[i].getName()+".stringArraySize("+ufields[j]+") did not throw IAE but "+e);
                }
            }
        }
    }
    private boolean isUnsupportedSA(PIMList list, int field)
    {
        if (list instanceof ContactList)
        {
            if (field == Contact.ADDR || field == Contact.NAME)
            {
                return true;
            }
        }
        return false;
    }
    private String asString(int[] values)
    {
        String ret = "";
        for (int i=0; i <values.length-1; i++)
        {
            ret += values[i]+", ";
        }
        ret += values[values.length-1];
        return ret;
    }

    private void testIsSupportedArrayElementFalse(PIMList[] lists) throws TestEndThrowable
    {
        for (int i = 0; i < lists.length; i++)
        {
            int[] fields = (int[]) supportedFields.get(lists[i].getName());

            for (int j = 0; j<fields.length; j++)
            {
                int dataType = lists[i].getFieldDataType(fields[j]);
                if (dataType == PIMItem.STRING_ARRAY)
                {
                    int arraySize = lists[i].stringArraySize(fields[j]);
                    int[] arrayElements = lists[i].getSupportedArrayElements(fields[j]);
//                  log( "arraySize: "+arraySize+", indexes: "+asString( arrayElements ) );

                    // test that unsupported arrayelements return false in isSupportedArrayElement()
                    for (int k=0; k< arraySize; k++)
                    {
                        if (!contains(arrayElements, k))
                        {
                            try
                            {
                                boolean supported = lists[i].isSupportedArrayElement(fields[j], k);
                                if (supported)
                                {
                                    String info = "list "+lists[i].getName()+", field "+fields[j]+ " arrayelement "+k;
                                    failTest("isSupportedArrayElement() for "+ info + "returned true!");
                                }
                            }
                            catch (Exception e)
                            {
                                String info = "list "+lists[i].getName()+", field "+fields[j]+ " arrayelement "+k;
                                failTest("isSupportedArrayElement() for "+ info + " threw: " +e);
                            }
                        }
                    }
                    // test that invalid arrayelements return false in isSupportedArrayElement()
                    int[] invalids = { -1000, -128, -5, -1, arraySize, 2147483647 };
                    for (int k=0; k< invalids.length; k++)
                    {
                        if (!contains(arrayElements, invalids[k]))
                        {
                            try
                            {
                                boolean supported = lists[i].isSupportedArrayElement(fields[j], invalids[k]);
                                if (supported)
                                {
                                    String info = "list "+lists[i].getName()+", field "+fields[j]+ " arrayelement "+invalids[k];
                                    failTest("isSupportedArrayElement() for "+ info + "returned true!");
                                }
                            }
                            catch (Exception e)
                            {
                                String info = "list "+lists[i].getName()+", field "+fields[j]+ " arrayelement "+invalids[k];
                                failTest("isSupportedArrayElement() for "+ info + " threw: " +e);
                            }
                        }
                    }
                }
            }
        }
    }

    private boolean contains(int[] array, int value)
    {
        for (int i=0; i < array.length; i++)
        {
            if (array[i] == value)
            {
                return true;
            }
        }
        return false;
    }

    private void testStringArrays(PIMList[] lists) throws TestEndThrowable
    {
        log("testing stringArrays");
        for (int i = 0; i < lists.length; i++)
        {
            // test supported fields
            int[] fields = (int[]) supportedFields.get(lists[i].getName());

            for (int j = 0; j<fields.length; j++)
            {
                int dataType = lists[i].getFieldDataType(fields[j]);
                if (dataType == PIMItem.STRING_ARRAY)
                {
                    int arraySize = lists[i].stringArraySize(fields[j]);
                    int[] arrayElements = lists[i].getSupportedArrayElements(fields[j]);
                    for (int k=0; k< arrayElements.length; k++)
                    {
                        String label = lists[i].getArrayElementLabel(fields[j], arrayElements[k]);
                        String info = "list "+lists[i].getName()+", field "+fields[j]+
                                      " arrayelement "+arrayElements[k] +"(label: "+label+")";
                        log(info);
                        if (arrayElements[k] >= arraySize)
                        {
                            failTest("stringArraySize() for "+ info + "is smaller than some "+
                                     "of the indexes returned by getSupportedArrayElements()");
                        }
                        if (!lists[i].isSupportedArrayElement(fields[j], arrayElements[k]))
                        {
                            failTest("isSupportedArrayElement() for "+ info + "returned false!");
                        }
                        if (label == null)
                        {
                            failTest("getArrayElementLabel() for "+ info + "returned null!");
                        }
                        else if (label.length() == 0)
                        {
                            failTest("getArrayElementLabel() for " + info + " returned empty string!");
                        }
                    }
                }
                else
                {
                    String info = "list "+lists[i].getName()+", field "+fields[j];
                    try
                    {
                        int[] arrayElements = lists[i].getSupportedArrayElements(fields[j]);
                        failTest("getSupportedArrayElements() for "+ info + "did not throw IAE!");
                    }
                    catch (IllegalArgumentException iae)
                    {
                        //ok
                    }
                    catch (Exception e)
                    {
                        failTest("getSupportedArrayElements() for "+ info + "did not throw IAE but "+e);
                    }
                }
            }
            // test unsupported fields
            fields = (int[]) unsupportedFields.get(lists[i].getName());

            for (int j = 0; j<fields.length; j++)
            {
                if (isUnsupportedSA(lists[i], fields[j]))
                {
                    String info = "list "+lists[i].getName()+", field "+fields[j];
                    try
                    {
                        int[] arrayElements = lists[i].getSupportedArrayElements(fields[j]);
                        failTest("getSupportedArrayElements() for "+ info + "did not throw UFE!");
                    }
                    catch (UnsupportedFieldException ufe)
                    {
                        //ok
                    }
                    catch (Exception e)
                    {
                        failTest("getSupportedArrayElements() for "+ info + "did not throw UFE but "+e);
                    }
                }
            }
        }
    }

    private void classifyFields(PIMList[] lists) throws TestEndThrowable
    {
        for (int i=0; i < lists.length; i++)
        {
            try
            {
                int[] fields = lists[i].getSupportedFields();
                supportedFields.put(lists[i].getName(), fields);

                int[] tmpFields = null;
                if (lists[i] instanceof ContactList)
                {
                    tmpFields = valuesNotInSecondList(contactFields, fields);
                }
                else if (lists[i] instanceof EventList)
                {
                    tmpFields = valuesNotInSecondList(eventFields, fields);
                }
                else if (lists[i] instanceof ToDoList)
                {
                    tmpFields = valuesNotInSecondList(todoFields, fields);
                }
                unsupportedFields.put(lists[i].getName(), tmpFields);
            }
            catch (Exception ex)
            {
                failTest("Error while classifying fields: " + ex);
            }
        }
    }
    // returns array containing those values in the first list
    // that are not contained in the  second list
    private int[] valuesNotInSecondList(int[] first, int[] second)
    {
        int[] ret = new int[ first.length - second.length ];
        int index = 0;

        for (int i=0; i<first.length; i++)
        {
            boolean found = false;
            for (int j=0; j<second.length; j++)
            {
                if (first[i] == second[j])
                {
                    found = true;
                }
            }
            if (!found)
            {
                ret[index++] = first[i];
            }
        }
        return ret;
    }

    private void closeLists(PIMList[] lists) throws TestEndThrowable
    {
        try
        {
            for (int i = 0; i < lists.length; i++)
            {
                lists[i].close();
            }
        }
        catch (Exception e)
        {
            failTest("Error while closing lists: " +e);
        }
    }

    private PIMList[] openAllLists(int mode) throws TestEndThrowable
    {
        try
        {
            PIM p = PIM.getInstance();
            String[] contactListNames = p.listPIMLists(PIM.CONTACT_LIST);
            String[] eventListNames = p.listPIMLists(PIM.EVENT_LIST);
            String[] todoListNames = p.listPIMLists(PIM.TODO_LIST);
            PIMList[] contactLists = openLists(p, PIM.CONTACT_LIST, contactListNames, mode);
            PIMList[] eventLists = openLists(p, PIM.EVENT_LIST, eventListNames, mode);
            PIMList[] todoLists = openLists(p, PIM.TODO_LIST, todoListNames, mode);

            return combineLists(contactLists, eventLists, todoLists);
        }
        catch (Throwable e)
        {
            failTest("preconditions failed: " + e);
        }
        return null;
    }

    private PIMList[] openLists(PIM aP, int aListType, String[] aNames, int aMode)
    throws PIMException
    {
        PIMList[] pimLists = new PIMList[ aNames.length ];
        for (int i = 0; i < aNames.length; i++)
        {
            pimLists[ i ] = aP.openPIMList(aListType, aMode, aNames[ i ]);
        }
        return pimLists;
    }

    private PIMList[] combineLists(PIMList[] pl1, PIMList[] pl2, PIMList[] pl3)
    {
        PIMList[] pimLists = new PIMList[ pl1.length + pl2.length + pl3.length ];
        int i = 0;
        for (int j=0; j < pl1.length; j++)
        {
            pimLists[ i++ ] = pl1[j];
        }
        for (int j=0; j < pl2.length; j++)
        {
            pimLists[ i++ ] = pl2[j];
        }
        for (int j=0; j < pl3.length; j++)
        {
            pimLists[ i++ ] = pl3[j];
        }
        return pimLists;
    }
}
