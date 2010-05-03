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
package com.nokia.mj.test.pim.automatic.PIMList.PLfields;
//IMPORTS
import java.io.IOException;
import java.util.*;
import javax.microedition.pim.*;
import com.nokia.mj.test.pim.adaptation.*;

// CLASS DEFINITION
/**
 * This class tests javax.microedition.pim.PIMList
 *
 */
public class PLfields
        extends MIDPTest
{

    public PLfields(
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

    private Hashtable supportedFields = new Hashtable();
    private Hashtable unsupportedFields = new Hashtable();


    public void runTest() throws TestEndThrowable
    {
        // preconditions
        PIMList[] allLists = openAllLists(PIM.READ_WRITE);
        // test step 1
        classifyFields(allLists);

        // Test steps 2 & 3
        log("Testing field support");
        testIsSupportedField(allLists);

        // Test steps 4
        log("Testing field maxValues");
        testMaxValues(allLists);

        // Test steps 5, 6, 7
        log("Testing field labels");
        testGetFieldLabel(allLists);

        // test steps 8, 9, 10
        log("Testing field data types");
        testGetFieldDatatype(allLists);

        closeLists(allLists);

        passTest();
    }

    private void testGetFieldDatatype(PIMList[] lists) throws TestEndThrowable
    {
        // first test for fields that are supported
        try
        {
            for (int i = 0; i < lists.length; i++)
            {
                int[] fields = (int[]) supportedFields.get(lists[i].getName());

                for (int j = 0; j<fields.length; j++)
                {
                    int dtype = lists[i].getFieldDataType(fields[j]);
                }
            }
        }
        catch (Exception e)
        {
            failTest("Error while calling getFieldDataType(<supportedfield>): " + e);
        }

        // test also fields that are not supported
        for (int i = 0; i < lists.length; i++)
        {
            int[] fields = (int[]) unsupportedFields.get(lists[i].getName());

            for (int j = 0; j<fields.length; j++)
            {
                try
                {
                    int dtype = lists[i].getFieldDataType(fields[j]);
                    failTest(lists[i].getName()+".getFieldDataType(" + fields[j] +
                             ") returned "+dtype+". Field not supported and should throw UFE.");
                }
                catch (UnsupportedFieldException ufe)
                {
                    // ok
                }
                catch (IllegalArgumentException iae)
                {
                    // ok
                }
                catch (Exception e)
                {
                    failTest("Error while calling getFieldDatatype(<unsupportedfield>): " + e);
                }
            }
        }

        // test also invalid fields
        for (int i = 0; i < lists.length; i++)
        {
            for (int j = 0; j<invalidFields.length; j++)
            {
                try
                {
                    int dtype = lists[i].getFieldDataType(invalidFields[j]);
                    failTest(lists[i].getName()+".getFieldDataType(" + invalidFields[j] +
                             ") returned "+dtype+". Field is invalid and should throw IAE.");
                }
                catch (IllegalArgumentException iae)
                {
                    //ok
                }
                catch (Exception e)
                {
                    failTest("Error while calling getFieldDataType(<invalidfield>): " + e);
                }
            }
        }
    }

    private void testGetFieldLabel(PIMList[] lists) throws TestEndThrowable
    {
        // first test for fields that are supported
        try
        {
            for (int i = 0; i < lists.length; i++)
            {
                int[] fields = (int[]) supportedFields.get(lists[i].getName());

                for (int j = 0; j<fields.length; j++)
                {
                    log("Testing: " + lists[i].getName()+".field:(" + fields[j] +")");
                    String label = lists[i].getFieldLabel(fields[j]);
                    log(lists[i].getName()+".field:(" + fields[j] +") has label: "+label);
                    if (label == null || label.length() < 1)
                    {
                        failTest(lists[i].getName()+".getFieldLabel(" + fields[j] +") returned null/empty String");
                    }
                }
            }
        }
        catch (Exception e)
        {
            failTest("Error while calling getFieldLabel(<supportedfield>): " + e);
        }

        // test also fields that are not supported
        for (int i = 0; i < lists.length; i++)
        {
            int[] fields = (int[]) unsupportedFields.get(lists[i].getName());

            for (int j = 0; j<fields.length; j++)
            {
                try
                {
                    log("Testing: " + lists[i].getName()+".field:(" + fields[j] +")");
                    String label = lists[i].getFieldLabel(fields[j]);
                    failTest(lists[i].getName()+".getFieldLabel(" + fields[j] +
                             ") returned "+label+". Rield is not supported. Should throw UFE.");
                }
                catch (UnsupportedFieldException ufe)
                {
                    // ok
                }
                catch (IllegalArgumentException iae)
                {
                    // ok
                }
                catch (Exception e)
                {
                    failTest("Error while calling getFieldLabel(<unsupportedfield>): " + e);
                }
            }
        }

        // test also invalid fields
        for (int i = 0; i < lists.length; i++)
        {
            for (int j = 0; j<invalidFields.length; j++)
            {
                try
                {
                    String label = lists[i].getFieldLabel(invalidFields[j]);
                    failTest(lists[i].getName()+".getFieldLabel(" + invalidFields[j] +
                             ") returned "+label+", even though the field is invalid and should throw IAE.");
                }
                catch (IllegalArgumentException iae)
                {
                    //ok
                }
                catch (Exception e)
                {
                    failTest("Error while calling getFieldLabel(<invalidfield>): " + e);
                }
            }
        }
    }

    private void testMaxValues(PIMList[] lists) throws TestEndThrowable
    {
        // first test for fields that are supported
        try
        {
            for (int i = 0; i < lists.length; i++)
            {
                int[] fields = (int[]) supportedFields.get(lists[i].getName());

                for (int j = 0; j<fields.length; j++)
                {
                    int val = lists[i].maxValues(fields[j]);
                    if (val < -1 || val == 0)
                    {
                        failTest(lists[i].getName()+".maxValue(" + fields[j] +") returned "+val);
                    }
                }
            }
        }
        catch (Exception e)
        {
            failTest("Error while calling maxValue(<supportedfield>): " + e);
        }

        // test also fields that are not supported
        try
        {
            for (int i = 0; i < lists.length; i++)
            {
                int[] fields = (int[]) unsupportedFields.get(lists[i].getName());

                for (int j = 0; j<fields.length; j++)
                {
                    int val = lists[i].maxValues(fields[j]);
                    if (val != 0)
                    {
                        failTest(lists[i].getName()+".maxValue(" + fields[j] +
                                 ") returned "+val+", even though the field is not supported and should return 0.");
                    }
                }
            }
        }
        catch (IllegalArgumentException iae)
        {
            // Ok
        }
        catch (Exception e)
        {
            failTest("Error while calling maxValue(<unsupportedfield>): " + e);
        }

        // test also invalid fields
        for (int i = 0; i < lists.length; i++)
        {
            for (int j = 0; j<invalidFields.length; j++)
            {
                try
                {
                    int val = lists[i].maxValues(invalidFields[j]);
                    failTest(lists[i].getName()+".maxValue(" + invalidFields[j] +
                             ") returned "+val+", even though it should throw IAE!");
                }
                catch (IllegalArgumentException iae)
                {
                    //ok
                }
                catch (Exception e)
                {
                    failTest("Error while calling maxValue(<invalidfield>): " + e);
                }
            }
        }
    }


    private void testIsSupportedField(PIMList[] lists) throws TestEndThrowable
    {
        // first test for fields that are supported
        try
        {
            for (int i = 0; i < lists.length; i++)
            {
                int[] fields = (int[]) supportedFields.get(lists[i].getName());

                for (int j = 0; j<fields.length; j++)
                {
                    if (!lists[i].isSupportedField(fields[j]))
                    {
                        failTest(lists[i].getName()+".isSupportedField(" + fields[j] +
                                 ") returned false, even though .getSupportedFields() returned it.");
                    }
                }
            }
        }
        catch (Exception e)
        {
            failTest("Error while calling isSupportedField(<supportedfield>): " + e);
        }

        // test also fields that are not supported
        try
        {
            for (int i = 0; i < lists.length; i++)
            {
                int[] fields = (int[]) unsupportedFields.get(lists[i].getName());

                for (int j = 0; j<fields.length; j++)
                {
                    if (lists[i].isSupportedField(fields[j]))
                    {
                        failTest(lists[i].getName()+".isSupportedField(" + fields[j] +
                                 ") returned true, even though .getSupportedFields() did not return it.");
                    }
                }
            }
        }
        catch (Exception e)
        {
            failTest("Error while calling isSupportedField(<unsupportedfield>): " + e);
        }

        // test also invalid fields
        try
        {
            for (int i = 0; i < lists.length; i++)
            {
                for (int j = 0; j<invalidFields.length; j++)
                {
                    if (lists[i].isSupportedField(invalidFields[j]))
                    {
                        failTest(lists[i].getName()+".isSupportedField(" + invalidFields[j] +
                                 ") returned true, even though .getSupportedFields() did not return it.");
                    }
                }
            }
        }
        catch (Exception e)
        {
            failTest("Error while calling isSupportedField(<invalidfield>): " + e);
        }
    }

    private void classifyFields(PIMList[] lists) throws TestEndThrowable
    {
        for (int i=0; i < lists.length; i++)
        {
            try
            {
                int[] fields = lists[i].getSupportedFields();
                String name = lists[i].getName();
                supportedFields.put(name, fields);

                log("supported fields for list '"+name+"': "+ asString(fields));

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

                log("unsupported fields for list '"+name+"': "+ asString(tmpFields));
            }
            catch (Exception ex)
            {
                failTest("Error while classifying fields: " + ex);
            }
        }
    }

    private String asString(int[] values)
    {
        String ret = "";
        for (int i=0; i < values.length-1; i++)
        {
            ret += values[i] +", ";
        }
        if (values.length > 0)
        {
            ret += values[values.length-1];
        }
        return ret;
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
