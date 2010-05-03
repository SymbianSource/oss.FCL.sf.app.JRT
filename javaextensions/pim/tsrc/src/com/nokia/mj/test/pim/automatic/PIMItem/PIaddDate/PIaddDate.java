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
package com.nokia.mj.test.pim.automatic.PIMItem.PIaddDate;
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
 * Tests javax.microedition.pim.PIMItem.addDate() and
 * javax.microedition.pim.PIMItem.getDate()
 */
public class PIaddDate
        extends MIDPTest
{

    public PIaddDate(
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
        // step s1-s6
        testGetFields(contactItems, contactLists);
        testGetFields(eventItems, eventLists);
        testGetFields(todoItems, todoLists);

        // step s7
        closeLists(contactLists);
        closeLists(eventLists);
        closeLists(todoLists);

        passTest();
    }

    /**
     * Test steps s1-s6
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
     * Test steps s1-s6
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
            // this is not in (original) test plan
            testIsModified(item, false);
            int fieldCount = fields.length;
            for (int i = 0; i < fieldCount; i++)
            {
                if ((fields[ i ] == Contact.REVISION) ||
                        (fields[ i ] == Event.REVISION) ||
                        (fields[ i ] == ToDo.REVISION))
                {
                    continue;
                }
                // test steps s2-s7
                testFieldDataType(item, fields[ i ], aList);
            } // end of for (int i = 0; i < fieldCount; i++)
        } // end of while (aItems.hasMoreElements())
    }

    /**
     * Test steps s2-s6
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
        if (dataType == PIMItem.DATE)
        {
            // test step s3
            long[] date = new long[ 1 ];
            int valueCount = testGetDate(aItem, aField, date);

            // test step s4
            testRemoveValue(aItem, aField, valueCount);
            // these is not in (original) test plan
            testCountValues(aItem, aField, valueCount - 1);
            testIOOBE(aItem, aField, valueCount - 1);
            // test step s5
            testAddDate(aItem, aField, date[ 0 ], valueCount);
            // test step s6
            testIsModified(aItem, true);
        }
    }

    /**
     * Test step s3
     */
    private int testGetDate(PIMItem aItem, int aField, long[] aData)
    throws TestEndThrowable
    {
        long data = 0;
        int values = 0;
        try
        {
            values = aItem.countValues(aField);
            data = aItem.getDate(aField, values - 1);
        }
        catch (Throwable e)
        {
            failTest("getDate threw: " + e.toString());
        } // end of try-catch
        aData[ 0 ] = data;
        return values;
    }

    private void compareData(PIMItem aItem, int aField, long aData,
                             int aIndex)
    throws TestEndThrowable
    {
        long newData = 0;
        try
        {
            newData = aItem.getDate(aField, aIndex);
        }
        catch (Throwable e)
        {
            failTest("S4: getDate threw:" + e.toString());
        } // end of try-catch
        if (newData != aData)
        {
            failTest("data has changed, old data: " + aData +
                     ", new data: " + newData);
        }
    }

    /**
     * Test step s5
     */
    private void testAddDate(PIMItem aItem, int aField, long aData,
                             int aValueCount)
    throws TestEndThrowable
    {
        try
        {
            aItem.addDate(aField, PIMItem.ATTR_NONE, aData);
        }
        catch (Throwable e)
        {
            failTest("addDate threw: " + e.toString());
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
    private void testRemoveValue(PIMItem aItem, int aField, int aValues)
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

    private void testIOOBE(PIMItem aItem, int aField, int aIndex)
    throws TestEndThrowable
    {
        boolean exceptionThrown = false;
        try
        {
            aItem.getDate(aField, aIndex);
        }
        catch (java.lang.IndexOutOfBoundsException e)
        {
            exceptionThrown = true;
        }
        catch (Throwable e)
        {
            failTest("S4: getDate threw: " + e.toString());
        } // end of try-catch
        if (!exceptionThrown)
        {
            failTest("getDate did not throw IOOBE");
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
