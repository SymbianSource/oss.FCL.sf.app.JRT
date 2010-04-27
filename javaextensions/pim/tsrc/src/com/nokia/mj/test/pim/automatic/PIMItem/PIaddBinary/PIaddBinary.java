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
package com.nokia.mj.test.pim.automatic.PIMItem.PIaddBinary;
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
public class PIaddBinary
        extends MIDPTest
{

    public PIaddBinary(
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
        // step s1-s9
        testGetFields(contactItems, contactLists);
        testGetFields(eventItems, eventLists);
        testGetFields(todoItems, todoLists);

        // step s10
        closeLists(contactLists);
        closeLists(eventLists);
        closeLists(todoLists);

        passTest();
    }

    /**
     * Test steps s1 -s9
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
     * Test steps s1-s9
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
                // test steps s2-s7
                testFieldDataType(item, fields[ i ], aList);
            } // end of for (int i = 0; i < fieldCount; i++)
        } // end of while (aItems.hasMoreElements())
    }

    /**
     * Test steps s2-s9
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
            byte[][] binary = new byte[1][];
            int valueCount = testGetBinary(aItem, aField, binary);
            // test step s4
            testIsModified(aItem, false);
            // test step s5
            testRemoveBinary(aItem, aField, valueCount);
            // test step s6
            testCountValues(aItem, aField, valueCount - 1);
            // test step s7
            testIOOBE(aItem, aField, valueCount - 1);
            // test step s8
            testAddBinary(aItem, aField, binary[0], valueCount);
            // test step s9
            testIsModified(aItem, true);
        } // end of if (dataType = PIMItem.BINARY)
    }

    /**
     * Test step s3
     */
    private int testGetBinary(PIMItem aItem, int aField, byte[][] aData)
    throws TestEndThrowable
    {
        byte[] data = null;
        int values = 0;
        try
        {
            values = aItem.countValues(aField);
            data = aItem.getBinary(aField, values - 1);
        }
        catch (Throwable e)
        {
            failTest("getBinary threw: " + e.toString());
        } // end of try-catch
        aData[0] = data;
        return values;
    }

    private void compareData(PIMItem aItem, int aField, byte[] aData,
                             int aIndex)
    throws TestEndThrowable
    {
        byte[] newData = null;
        try
        {
            newData = aItem.getBinary(aField, aIndex);
        }
        catch (Throwable e)
        {
            failTest("S4: getBinary threw:" + e.toString());
        } // end of try-catch
        int dataLength = aData.length;
        if (newData.length != dataLength)
        {
            failTest("data has changed, datalength:" + newData.length);
        } // end of if (newData.length != dataLength)
        for (int i = 0; i < dataLength; i++)
        {
            if (newData[ i ] != aData[ i ])
            {
                failTest("data has changed at offset:" + i);
            } // end of if (newData[ i ] != aData[ i ])
        } // end of for (int i = 0; i < dataLength; i++)
    }

    /**
     * Test step s8
     */
    private void testAddBinary(PIMItem aItem, int aField, byte[] aData,
                               int aValueCount)
    throws TestEndThrowable
    {
        try
        {
            aItem.addBinary(aField, PIMItem.ATTR_NONE, aData, 0,
                            aData.length);
        }
        catch (Throwable e)
        {
            failTest("addBinary threw: " + e.toString());
        } // end of try-catch
        compareData(aItem, aField, aData, aValueCount - 1);
    }

    /**
     * Test steps s4, s9
     */
    private void testIsModified(PIMItem aItem, boolean aExpected)
    throws TestEndThrowable
    {
        boolean isModified = false;
        try
        {
            isModified = aItem.isModified();
        }
        catch (Throwable e)
        {
            failTest("isModified threw: " + e.toString());
        } // end of try-catch
        if (isModified != aExpected)
        {
            if (aExpected)
            {
                failTest("isModified returned false");
            } // end of if (aExpected)
            else
            {
                failTest("isModified returned true");
            } // end of else
        } // end of if (!isModified)
    }

    /**
     * Test step s5
     */
    private void testRemoveBinary(PIMItem aItem, int aField, int aValues)
    throws TestEndThrowable
    {
        try
        {
            aItem.removeValue(aField, aValues - 1);
        }
        catch (Throwable e)
        {
            failTest("removeValue threw: " + e.toString());
        } // end of try-catch
    }

    /**
     * Test step s6
     */
    private void testCountValues(PIMItem aItem, int aField,
                                 int aExpected)
    throws TestEndThrowable
    {
        int valueCount = 0;
        try
        {
            valueCount = aItem.countValues(aField);
        }
        catch (Throwable e)
        {
            failTest("countValues threw: " + e.toString());
        } // end of try-catch
        if (valueCount != aExpected)
        {
            failTest("countValues returned:" + valueCount);
        } // end of if (valueCount != aExpected)
    }

    /**
     * Test step s7
     */
    private void testIOOBE(PIMItem aItem, int aField, int aIndex)
    throws TestEndThrowable
    {
        boolean exceptionThrown = false;
        try
        {
            aItem.getBinary(aField, aIndex);
        }
        catch (java.lang.IndexOutOfBoundsException e)
        {
            exceptionThrown = true;
        }
        catch (Throwable e)
        {
            failTest("S7: getBinary threw: " + e.toString());
        } // end of try-catch
        if (!exceptionThrown)
        {
            failTest("getBinary did not throw IOOBE");
        } // end of if (!exceptionThrown)
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
