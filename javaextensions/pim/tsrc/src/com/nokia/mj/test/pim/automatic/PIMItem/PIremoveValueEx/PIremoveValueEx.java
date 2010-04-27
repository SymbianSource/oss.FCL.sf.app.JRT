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
package com.nokia.mj.test.pim.automatic.PIMItem.PIremoveValueEx;
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
import javax.microedition.pim.EventList;
import javax.microedition.pim.ToDoList;
import javax.microedition.pim.PIMList;
import javax.microedition.pim.PIMException;
import javax.microedition.pim.UnsupportedFieldException;
import com.nokia.mj.test.pim.adaptation.*;

// CLASS DEFINITION
/**
 * Tests javax.microedition.pim.PIMItem.removeValue()
 */
public class PIremoveValueEx
        extends MIDPTest
{

    public PIremoveValueEx(
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
        int unsupportedField = 0;
        if (aList instanceof ContactList)
        {
            unsupportedField = 112;
        } // end of if (aList instanceof ContactList)
        if (aList instanceof EventList)
        {
            unsupportedField = 0;
        }
        if (aList instanceof ToDoList)
        {
            unsupportedField = 0;
        }

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
                // test step s2
                int valueCount = testCountValues(item, field);
                // test step s3
                testIOOBE(item, field, valueCount);
                testIOOBE(item, field, -1);
            } // end of for (int i = 0; i < fieldCount; i++)

            // test step s4
            testIAE(item, 1, 0);
            // test step s5
            if (unsupportedField != 0)
                testUFE(item, unsupportedField, 0);
            // test step s6
            testIsModified(item, false);
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
    private void testIOOBE(PIMItem aItem, int aField, int aIndex)
    throws TestEndThrowable
    {
        boolean exceptionThrown = false;
        try
        {
            aItem.removeValue(aField, aIndex);
        }
        catch (IndexOutOfBoundsException e)
        {
            exceptionThrown =true;
        } // end of catch
        catch (Throwable e)
        {
            failTest("removeValue threw instead of IOOBE: " + e.toString());
        } // end of try-catch
        if (!exceptionThrown)
        {
            failTest("IOOBE not thrown");
        } // end of if (!exceptionThrown)
    }

    /**
     * Test step s4
     */
    private void testIAE(PIMItem aItem, int aField, int aIndex)
    throws TestEndThrowable
    {
        boolean exceptionThrown = false;
        try
        {
            aItem.removeValue(aField, aIndex);
        }
        catch (IllegalArgumentException e)
        {
            exceptionThrown =true;
        } // end of catch
        catch (Throwable e)
        {
            failTest("removeValue threw instead of IAE: " + e.toString());
        } // end of try-catch
        if (!exceptionThrown)
        {
            failTest("IAE not thrown");
        } // end of if (!exceptionThrown)
    }

    /**
     * Test step s5
     */
    private void testUFE(PIMItem aItem, int aField, int aIndex)
    throws TestEndThrowable
    {
        boolean exceptionThrown = false;
        try
        {
            aItem.removeValue(aField, aIndex);
        }
        catch (UnsupportedFieldException e)
        {
            exceptionThrown =true;
        } // end of catch
        catch (Throwable e)
        {
            failTest("removeValue threw instead of UFE: " + e.toString());
        } // end of try-catch
        if (!exceptionThrown)
        {
            failTest("UFE not thrown");
        } // end of if (!exceptionThrown)
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
