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
package com.nokia.mj.test.pim.automatic.PIMItem.PIremoveValue;
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
 * Tests javax.microedition.pim.PIMItem.removeValue()
 */
public class PIremoveValue
        extends MIDPTest
{

    public PIremoveValue(
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
        // step s1-s4
        testGetFields(contactItems, contactLists);
        testGetFields(eventItems, eventLists);
        testGetFields(todoItems, todoLists);

        // step s5
        closeLists(contactLists);
        closeLists(eventLists);
        closeLists(todoLists);

        passTest();
    }

    /**
     * Test steps s1-s4
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
     * Test steps s1-s4
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
                int field = fields[ i ];
                if ((field == Contact.UID) ||
                        (field == Event.UID) ||
                        (field == ToDo.UID) ||
                        (field == Contact.REVISION) ||
                        (field == Event.REVISION) ||
                        (field == ToDo.REVISION))
                {
                    continue;
                }
                // test step s2
                int valueCount = testCountValues(item, field);
                // test step s3
                testRemoveValue(item, field, valueCount);
                // test step s4
                testIsModified(item, true);
            } // end of for (int i = 0; i < fieldCount; i++)
        } // end of while (aItems.hasMoreElements())
    }

    /**
     * Test steps s4
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
     * Test step s3
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
        int valueCount = testCountValues(aItem, aField);
        if (valueCount != (aValues - 1))
        {
            failTest("removeValue did not decrement value count");
        } // end of if (valueCount != ( aValues -1 ))
    }

    /**
     * test step s2
     */
    private int testCountValues(PIMItem aItem, int aField)
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
        return valueCount;
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
