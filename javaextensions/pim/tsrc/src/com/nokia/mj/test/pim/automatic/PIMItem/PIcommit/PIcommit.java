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
package com.nokia.mj.test.pim.automatic.PIMItem.PIcommit;
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
 * Tests javax.microedition.pim.PIMItem.commit()
 */
public class PIcommit
        extends MIDPTest
{

    public PIcommit(
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
            contactLists = openLists(p, PIM.CONTACT_LIST, PIM.READ_WRITE,
                                     contactListNames);
            eventLists = openLists(p, PIM.EVENT_LIST, PIM.READ_WRITE,
                                   eventListNames);
            todoLists = openLists(p, PIM.TODO_LIST, PIM.READ_WRITE,
                                  todoListNames);
            contactItems = getItems(contactLists);
            eventItems = getItems(eventLists);
            todoItems = getItems(todoLists);
        }
        catch (Throwable e)
        {
            failTest("preconditions failed: " + e.toString());
        } // end of try-catch

        // test steps
        // step s1
        testCommit(contactItems, contactLists);
        testCommit(eventItems, eventLists);
        testCommit(todoItems, todoLists);

        // step s2
        closeLists(contactLists);
        closeLists(eventLists);
        closeLists(todoLists);

        // step s3
        try
        {
            contactLists = openLists(p, PIM.CONTACT_LIST, PIM.READ_ONLY,
                                     contactListNames);
            eventLists = openLists(p, PIM.EVENT_LIST, PIM.READ_ONLY,
                                   eventListNames);
            todoLists = openLists(p, PIM.TODO_LIST, PIM.READ_ONLY,
                                  todoListNames);
            contactItems = getItems(contactLists);
            eventItems = getItems(eventLists);
            todoItems = getItems(todoLists);
        }
        catch (Throwable e)
        {
            failTest("s3: " + e.toString());
        } // end of try-catch

        // step s4
        testPIME(contactItems, contactLists);
        testPIME(eventItems, eventLists);
        testPIME(todoItems, todoLists);

        // step s5
        closeLists(contactLists);
        closeLists(eventLists);
        closeLists(todoLists);

        passTest();
    }

    /**
     * Test steps s1
     */
    private void testCommit(Enumeration[] aItems,
                            PIMList[] aLists)
    throws TestEndThrowable
    {
        int listCount = aItems.length;
        for (int i = 0; i < listCount; i++)
        {
            testCommit(aItems[ i ], aLists[ i ]);
        } // end of for (int i = 0; i < listCount; i++)
    }

    /**
     * Test steps s1
     */
    private void testCommit(Enumeration aItems, PIMList aList)
    throws TestEndThrowable
    {
        while (aItems.hasMoreElements())
        {
            PIMItem item = (PIMItem)aItems.nextElement();
            testCommit(item);
        } // end of while (aItems.hasMoreElements())
    }

    /**
     * Test step s1
     */
    private void testCommit(PIMItem aItem)
    throws TestEndThrowable
    {
        try
        {
            aItem.commit();
        }
        catch (Throwable e)
        {
            failTest("commit threw: " + e.toString());
        } // end of try-catch
    }

    /**
     * Test steps s4
     */
    private void testPIME(Enumeration[] aItems,
                          PIMList[] aLists)
    throws TestEndThrowable
    {
        int listCount = aItems.length;
        for (int i = 0; i < listCount; i++)
        {
            testPIME(aItems[ i ], aLists[ i ]);
        } // end of for (int i = 0; i < listCount; i++)
    }

    /**
     * Test steps s4
     */
    private void testPIME(Enumeration aItems, PIMList aList)
    throws TestEndThrowable
    {
        while (aItems.hasMoreElements())
        {
            PIMItem item = (PIMItem)aItems.nextElement();
            testPIME(item);
        } // end of while (aItems.hasMoreElements())
    }

    /**
     * Test step s4
     */
    private void testPIME(PIMItem aItem)
    throws TestEndThrowable
    {
        boolean exceptionThrown = false;
        try
        {
            aItem.commit();
        }
        catch (java.lang.SecurityException e)
        {
            exceptionThrown = true;
        } // end of catch
        catch (Throwable e)
        {
            failTest("not PIMException: " + e.toString());
        } // end of try-catch
        if (!exceptionThrown)
        {
            failTest("PIMException not thrown");
        } // end of if (!exceptionThrown)
    }

    private PIMList[] openLists(PIM aP, int aListType, int aMode,
                                String[] aListNames)
    throws PIMException
    {
        int listCount = aListNames.length;
        PIMList[] pimLists = new PIMList[ listCount ];
        for (int i = 0; i < listCount; i++)
        {
            pimLists[ i ] = aP.openPIMList(aListType, aMode,
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
