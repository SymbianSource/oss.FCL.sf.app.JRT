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
package com.nokia.mj.test.pim.automatic.PIMItem.PIgetPIMList;
//IMPORTS
import java.io.IOException;
import java.io.InputStream;
import java.util.*;
import javax.microedition.pim.PIM;
import javax.microedition.pim.PIMItem;
import javax.microedition.pim.Contact;
import javax.microedition.pim.ContactList;
import javax.microedition.pim.PIMList;
import javax.microedition.pim.PIMException;
import com.nokia.mj.test.pim.adaptation.*;

// CLASS DEFINITION
/**
 * Tests javax.microedition.pim.PIMItem.getCategories(),
 * javax.microedition.pim.PIMItem.maxCategories(),
 * javax.microedition.pim.PIMItem.removeFromCategory() and
 * javax.microedition.pim.PIMItem.addToCategory()
 *
 */
public class PIgetPIMList
        extends MIDPTest
{

    public PIgetPIMList(
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
        PIMItem serialItem = null;
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
            InputStream is = getClass().getResourceAsStream("/res/3.vcf");
            PIMItem item[] = p.fromSerialFormat(is, "UTF-8");
            serialItem = item[ 0 ];
        }
        catch (java.lang.OutOfMemoryError e)
        {
            failTest("preconditions threw OOME:" + e.getMessage());
        } // end of catch
        catch (Throwable e)
        {
            failTest("preconditions failed: " + e.toString());
        } // end of try-catch

        // test steps
        // step s1
        testGetPIMList(contactItems, contactLists);
        testGetPIMList(eventItems, eventLists);
        testGetPIMList(todoItems, todoLists);

        // step s2
        testNullList(serialItem);

        // step s3
        Contact importedContact =
            testAddToList(serialItem, contactLists[ 0 ]);

        // step s4
        testRemoveFromList(importedContact, contactLists[ 0 ]);

        // step s5
        closeLists(contactLists);
        closeLists(eventLists);
        closeLists(todoLists);

        passTest();
    }

    /**
     * Test step s1
     */
    private void testGetPIMList(Enumeration[] aItems,
                                PIMList[] aLists)
    throws TestEndThrowable
    {
        int listCount = aItems.length;
        for (int i = 0; i < listCount; i++)
        {
            testGetPIMList(aItems[ i ], aLists[ i ]);
        } // end of for (int i = 0; i < listCount; i++)
    }

    private void testGetPIMList(Enumeration aItems, PIMList aList)
    throws TestEndThrowable
    {
        while (aItems.hasMoreElements())
        {
            PIMItem item = (PIMItem)aItems.nextElement();
            PIMList itemList = null;
            try
            {
                itemList = item.getPIMList();
            }
            catch (Throwable e)
            {
                failTest("getPIMList threw: " + e.toString());
            } // end of try-catch
            if (aList != itemList)
            {
                failTest("getPIMList returned wrong list");
            } // end of if (aList != itemList)
        } // end of while (aItems.hasMoreElements())
    }

    /**
     * Test step s2
     */
    private void testNullList(PIMItem aItem)
    throws TestEndThrowable
    {
        PIMList itemList = null;
        try
        {
            itemList = aItem.getPIMList();
        }
        catch (Throwable e)
        {
            failTest("s2: getPIMList threw: " + e.toString());
        } // end of try-catch
        if (itemList != null)
        {
            failTest("getPIMList returned something when expecting null");
        } // end of if (itemList != null)
    }

    /**
     * Test step s3
     */
    private Contact testAddToList(PIMItem aItem, PIMList aList)
    throws TestEndThrowable
    {
        Contact importedContact = null;
        try
        {
            ContactList cList = (ContactList)aList;
            importedContact = cList.importContact((Contact)aItem);
            importedContact.commit();
        }
        catch (Throwable e)
        {
            failTest("importing item to a list failed: " + e.toString());
        } // end of try-catch

        PIMList itemList = null;
        try
        {
            itemList = importedContact.getPIMList();
        }
        catch (Throwable e)
        {
            failTest("s3: getPIMList threw: " + e.toString());
        } // end of try-catch
        if (itemList != aList)
        {
            failTest("imported getPIMList returns wrong list");
        } // end of if (itemList != aList)
        return importedContact;
    }

    /**
     * Test step s4
     */
    private void testRemoveFromList(PIMItem aItem, PIMList aList)
    throws TestEndThrowable
    {
        try
        {
            ContactList cList = (ContactList)aList;
            cList.removeContact((Contact)aItem);
        }
        catch (Throwable e)
        {
            failTest("removing item from a list failed: " + e.toString());
        } // end of try-catch

        PIMList itemList = null;
        try
        {
            itemList = aItem.getPIMList();
        }
        catch (Throwable e)
        {
            failTest("s4: getPIMList threw: " + e.toString());
        } // end of try-catch
        if (itemList != null)
        {
            failTest("removed getPIMList returns something");
        } // end of if (itemList != aList)
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
