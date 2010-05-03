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
package com.nokia.mj.test.pim.automatic.PIMItem.PIaddBinaryEx;
//IMPORTS
import java.io.IOException;
import java.io.InputStream;
import java.util.*;
import javax.microedition.pim.PIM;
import javax.microedition.pim.PIMItem;
import javax.microedition.pim.Contact;
import javax.microedition.pim.Event;
import javax.microedition.pim.ToDo;
import javax.microedition.pim.ContactList;
import javax.microedition.pim.PIMList;
import javax.microedition.pim.PIMException;
import javax.microedition.pim.UnsupportedFieldException;
import com.nokia.mj.test.pim.adaptation.*;

// CLASS DEFINITION
/**
 * Tests javax.microedition.pim.PIMItem.addBinary() and
 * javax.microedition.pim.PIMItem.getBinary()
 */
public class PIaddBinaryEx
        extends MIDPTest
{

    public PIaddBinaryEx(
        String sTestName, j2meunit.framework.TestMethod rTestMethod)
    {
        super(sTestName, rTestMethod);
    }


    public void runTest() throws TestEndThrowable
    {
        // preconditions
        PIM p = null;
        String[] contactListNames = null;
        String[] eventListNames = null;
        String[] todoListNames = null;
        PIMList[] contactLists = null;
        PIMList[] eventLists = null;
        PIMList[] todoLists = null;
        Enumeration[] contactItems = null;
        Enumeration[] eventItems = null;
        Enumeration[] todoItems = null;
        try
        {
            p = PIM.getInstance();
            contactListNames = p.listPIMLists(PIM.CONTACT_LIST);
            eventListNames = p.listPIMLists(PIM.EVENT_LIST);
            todoListNames = p.listPIMLists(PIM.TODO_LIST);
            contactLists = openLists(p, PIM.CONTACT_LIST, contactListNames);
            eventLists = openLists(p, PIM.EVENT_LIST, eventListNames);
            todoLists = openLists(p, PIM.TODO_LIST, todoListNames);
            contactItems = getItems(contactLists);
            eventItems = getItems(eventLists);
            todoItems = getItems(todoLists);
        }
        catch (Throwable e)
        {
            failTest("preconditions failed: " + e.toString());
        } // end of try-catch

        // test steps
        // step s1-s10
        testGetFields(contactItems, contactLists);
        testGetFields(eventItems, eventLists);
        testGetFields(todoItems, todoLists);

        // step s11 makes no sense

        // step s12
        closeLists(contactLists);
        closeLists(eventLists);
        closeLists(todoLists);

        passTest();
    }

    /**
     * Test steps s1-s10
     */
    private void testGetFields(Enumeration[] aItems,
                               PIMList[] aLists)
    throws TestEndThrowable
    {
        int listCount = aItems.length;
        for (int i = 0; i < listCount; i++)
        {
            testGetFields(aItems[ i ], aLists[ i ]);
        } // end of for (int i = 0; i < listCount; i++)
    }

    /**
     * Test steps s1-s10
     */
    private void testGetFields(Enumeration aItems, PIMList aList)
    throws TestEndThrowable
    {
        while (aItems.hasMoreElements())
        {
            PIMItem item = (PIMItem)aItems.nextElement();
            int[] fields = null;
            try
            {
                fields = item.getFields();
            }
            catch (Throwable e)
            {
                failTest("getFields threw: " + e.toString());
            } // end of try-catch
            int fieldCount = fields.length;
            for (int i = 0; i < fieldCount; i++)
            {
                // test steps s2-s11
                testFieldDataType(item, fields[ i ], aList);
            } // end of for (int i = 0; i < fieldCount; i++)
        } // end of while (aItems.hasMoreElements())
    }

    /**
     * Test steps s2-s10
     */
    private void testFieldDataType(PIMItem aItem, int aField, PIMList aList)
    throws TestEndThrowable
    {
        int dataType = 0;
        try
        {
            dataType = aList.getFieldDataType(aField);
        }
        catch (Throwable e)
        {
            failTest("getFieldDataType threw:" + e.toString());
        } // end of try-catch
        if (dataType == PIMItem.BINARY)
        {
            // test step s3
            byte[] binary = testGetBinary(aItem, aField);
            // test step s4
            testAddBinary(aItem, aField, null, binary.length, 0, 0, 1);
            // test step s5
            testAddBinary(aItem, 1, binary, binary.length, 0, 0, 2);
            // test step s6
            testUFE(aItem, aField, binary, binary.length, 0, 0, 3);
            // test step s7
            testAddBinary(aItem, aField, binary, binary.length, 0, -1, 2);
            testAddBinary(aItem, aField, binary, binary.length, 0,
                          binary.length, 2);
            // test step s8
            testAddBinary(aItem, aField, binary, -1, 0, 0, 2);
            testAddBinary(aItem, aField, binary, 0, 0, 0, 2);
            // test step s9
            testAddBinary(aItem, aField, new byte[ 0 ],
                          binary.length, 0, 0, 2);


            // test step s10
            // Minor deviation from test plan: isModified is not tested
            // for each field, as the method only tests the entire item.
            // however, we still run it only for thos items which we have
            // changed
            testIsModified(aItem);
        } // end of if (dataType = PIMItem.BINARY)
    }

    /**
     * Test step s3
     */
    private byte[] testGetBinary(PIMItem aItem, int aField)
    throws TestEndThrowable
    {
        byte[] data = null;
        try
        {
            data = aItem.getBinary(aField, 0);
        }
        catch (Throwable e)
        {
            failTest("getBinary threw: " + e.toString());
        } // end of try-catch
        return data;
    }

    /**
     * Test steps s4-s9
     * @param aExpected 1 = Null, 2 = IAE, 3 = UFE, 4 = IOOBE
     */
    private void testAddBinary(PIMItem aItem, int aField,
                               byte[] aData, int aDataLength,
                               int aAttributes, int aOffset, int aExpected)
    throws TestEndThrowable
    {
        int exceptionThrown = 0;
        try
        {
            aItem.addBinary(aField, aAttributes, aData, aOffset,
                            aDataLength);
        }
        catch (java.lang.NullPointerException e)
        {
            exceptionThrown = 1;
        } // end of catch
        catch (java.lang.IllegalArgumentException e)
        {
            exceptionThrown = 2;
        } // end of catch
        catch (javax.microedition.pim.UnsupportedFieldException e)
        {
            exceptionThrown = 3;
        } // end of catch
        catch (java.lang.IndexOutOfBoundsException e)
        {
            exceptionThrown = 4;
        } // end of catch
        catch (Throwable e)
        {
            failTest("addBinary threw: " + e.toString());
        } // end of try-catch
        if (exceptionThrown != aExpected)
        {
            failTest("addBinary threw " + exceptionThrown +
                     " instead of " + aExpected);
        } // end of if (exceptionThrown != aExpected)
    }

    /**
     * Test step s6
     */
    private void testUFE(PIMItem aItem, int aField,
                         byte[] aData, int aDataLength,
                         int aAttributes, int aOffset, int aExpected)
    throws TestEndThrowable
    {
        int field = 0;
        if (aItem instanceof Contact)
        {
            field = 112;
        } // end of if (aItem instanceof Contact)
        if (aItem instanceof Event)
        {
            field = 0;
        }
        if (aItem instanceof ToDo)
        {
            field = 0;
        }
        if (field != 0)
        {
            testAddBinary(aItem, field, aData, aDataLength,
                          aAttributes, aOffset, aExpected);
        } // end of if (field != 0)
    }

    /**
     * Test step s10
     */
    private void testIsModified(PIMItem aItem)
    throws TestEndThrowable
    {
        boolean isModified = true;
        try
        {
            isModified = aItem.isModified();
        }
        catch (Throwable e)
        {
            failTest("isModified threw: " + e.toString());
        } // end of try-catch
        if (isModified)
        {
            failTest("isModified returned true");
        } // end of if (!isModified)
    }


    private PIMList[] openLists(PIM aP, int aListType, String[] aListNames)
    throws PIMException
    {
        int listCount = aListNames.length;
        PIMList[] pimLists = new PIMList[ listCount ];
        for (int i = 0; i < listCount; i++)
        {
            pimLists[ i ] = aP.openPIMList(aListType, PIM.READ_WRITE,
                                           aListNames[ i ]);
        } // end of for (int i = 0; i < listCount; i++)
        return pimLists;
    }

    private Enumeration[] getItems(PIMList[] aLists)
    throws PIMException
    {
        int listCount = aLists.length;
        Enumeration[] items = new Enumeration[ listCount ];
        for (int i = 0; i < listCount; i++)
        {
            items[ i ] = aLists[ i ].items();
        } // end of for (int i = 0; i < listCount; i++)
        return items;
    }

    private void closeLists(PIMList[] aLists)
    throws TestEndThrowable
    {
        int listCount = aLists.length;
        for (int i = 0; i < listCount; i++)
        {
            try
            {
                aLists[ i ].close();
            }
            catch (Throwable e)
            {
                failTest("close threw: " + e.toString());
            } // end of try-catch
        } // end of for (int i = 0; i < listCount; i++)
    }

}
