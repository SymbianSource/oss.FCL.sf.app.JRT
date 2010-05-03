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
package com.nokia.mj.test.pim.automatic.PIMItem.PIaddDateEx;
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
public class PIaddDateEx
        extends MIDPTest
{

    public PIaddDateEx(
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
        // step s1-s8
        testGetFields(contactItems, contactLists);
        testGetFields(eventItems, eventLists);
        testGetFields(todoItems, todoLists);

        // step s11
        closeLists(contactLists);
        closeLists(eventLists);
        closeLists(todoLists);

        passTest();
    }

    /**
     * Test steps s1-s8
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
     * Test steps s1-s8
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
                // test steps s2-s9
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
        if (dataType == PIMItem.DATE)
        {
            // test step s3
            long date = testGetDate(aItem, aField);
            // test step s4 is not done because negative values may be valid
            // test step s5
            testAddDate(aItem, 1, date, 0, 2);
            // test step s6
            testUFE(aItem, aField, date, 0, 3);
            // test step s7
            testIsModified(aItem);
            testFFE(aList, aItem, aField, date, 0);
        }
    }

    /**
     * Test step s3
     */
    private long testGetDate(PIMItem aItem, int aField)
    throws TestEndThrowable
    {
        long data = 0;
        try
        {
            data = aItem.getDate(aField, 0);
        }
        catch (Throwable e)
        {
            failTest("getDate threw: " + e.toString());
        } // end of try-catch
        return data;
    }

    /**
     * Test steps s4-s6
     * @param aExpected 1 = Null, 2 = IAE, 3 = UFE, 4 = IOOBE
     */
    private void testAddDate(PIMItem aItem, int aField,
                             long aData,
                             int aAttributes, int aExpected)
    throws TestEndThrowable
    {
        int exceptionThrown = 0;
        try
        {
            aItem.addDate(aField, aAttributes, aData);
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
            failTest("addDate threw: " + e.toString());
        } // end of try-catch
        if (exceptionThrown != aExpected)
        {
            failTest("addDate threw " + exceptionThrown +
                     " instead of " + aExpected);
        } // end of if (exceptionThrown != aExpected)
    }

    /**
     * Test step s6
     */
    private void testUFE(PIMItem aItem, int aField,
                         long aData,
                         int aAttributes, int aExpected)
    throws TestEndThrowable
    {
        int field = 0;
        if (aItem instanceof Contact)
        {
            field = 0;
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
            testAddDate(aItem, field, aData,
                        aAttributes, aExpected);
        } // end of if (field != 0)
    }

    /**
     * Test step s7
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

    /**
     * Test step s8
     */
    private void testFFE(PIMList aList, PIMItem aItem, int aField, long aData,
                         int aAttributes)
    throws TestEndThrowable
    {
        int maxValues = aList.maxValues(aField);
        if (maxValues == -1)
        {
            // unlimited
            return;
        }
        if (maxValues == 0)
        {
            failTest("maxValues returned 0, field has values");
        } // end of if (maxValues == 0)
        int valueCount = aItem.countValues(aField);
        // This many adds should not fail
        int needToAdd = maxValues - valueCount;
        for (int i = 0; i < needToAdd; i++)
        {
            try
            {
                aItem.addDate(aField, aAttributes, aData);
            }
            catch (Throwable e)
            {
                failTest("S8: addDate threw: " + e.toString());
            } // end of try-catch
        } // end of for (int i = 0; i < needToAdd; i++)
        boolean exceptionThrown = false;
        try
        {
            aItem.addDate(aField, aAttributes, aData);
        }
        catch (javax.microedition.pim.FieldFullException e)
        {
            exceptionThrown = true;
        } // end of try-catch
        catch (Throwable e)
        {
            failTest("S8: last addDate threw: " + e.toString());
        } // end of catch
        if (!exceptionThrown)
        {
            failTest("FFE not thrown");
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
