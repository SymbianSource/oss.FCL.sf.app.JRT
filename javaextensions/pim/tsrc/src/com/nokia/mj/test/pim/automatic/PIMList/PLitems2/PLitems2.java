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
package com.nokia.mj.test.pim.automatic.PIMList.PLitems2;
//IMPORTS
import java.io.IOException;
import java.util.*;
import javax.microedition.pim.*;
import com.nokia.mj.test.pim.adaptation.*;

// CLASS DEFINITION
/**
 * This class tests javax.microedition.pim.PIMList.items(String)
 */
public class PLitems2 extends MIDPTest
{

    public PLitems2(
        String sTestName, j2meunit.framework.TestMethod rTestMethod)
    {
        super(sTestName, rTestMethod);
    }

    private Vector lists = null;
    private Event e1 = null;
    private Contact c1 = null;
    private ToDo t1 = null;

    private String[] testStrings = { "Test String #1", "tEst sTrInG #1",
                                     "est", "eSt", " ", ""
                                   };

    // these are for testing matching for strings inside stringarrays
    private String[] testStrings2 = { "UniqueArray", "eArray", "EaRrAy" };

    private int[][] itemCounts = null;
    private int[][] arrayItemCounts = null;

    public void initTest() throws TestEndThrowable
    {
        // preconditions
        openLists(PIM.READ_WRITE);

        // count items in lists
        itemCounts = countItems(testStrings);
        arrayItemCounts = countItems(testStrings2);

        // add test items & categories
        addItems();
    }


    private int[][] countItems(String[] aTestStrings) throws TestEndThrowable
    {

        int[][] count = new int[lists.size()][aTestStrings.length];

        try
        {
            for (int i=0; i< lists.size(); i++)
            {
                PIMList pl = (PIMList) lists.elementAt(i);
                for (int j=0; j< aTestStrings.length; j++)
                {
                    Enumeration e = pl.items(aTestStrings[j]);
                    count[i][j] = count(e);
                }
            }
        }
        catch (Throwable t)
        {
            failTest("Item count failed: " + t);
        }

        return count;
    }

    private void openLists(int aMode) throws TestEndThrowable
    {
        // open first lists of each type
        try
        {
            PIM p = PIM.getInstance();
            lists = new Vector();

            String[] todoListNames = p.listPIMLists(PIM.TODO_LIST);
            lists.addElement(p.openPIMList(PIM.TODO_LIST,
                                           aMode, todoListNames[0]));

            String[] eventListNames = p.listPIMLists(PIM.EVENT_LIST);
            lists.addElement(p.openPIMList(PIM.EVENT_LIST,
                                           aMode, eventListNames[0]));

            String[] contactListNames = p.listPIMLists(PIM.CONTACT_LIST);
            lists.addElement(p.openPIMList(PIM.CONTACT_LIST,
                                           aMode, contactListNames[0]));
        }
        catch (Throwable e)
        {
            failTest("Opening lists failed: " + e);
        }
    }

    private void addItems() throws TestEndThrowable
    {
        try
        {
            // add test items
            for (int i=0; i< lists.size(); i++)
            {
                PIMList pl = (PIMList) lists.elementAt(i);

                if (pl instanceof EventList)
                {
                    e1 = ((EventList)pl).createEvent();
                    e1.addString(Event.SUMMARY, PIMItem.ATTR_NONE, testStrings[0]);
                    e1.addDate(Event.START, PIMItem.ATTR_NONE, 1000000000000L);
                    e1.commit();
                }
                else if (pl instanceof ContactList)
                {
                    c1 = ((ContactList)pl).createContact();
                    c1.addString(Contact.EMAIL, PIMItem.ATTR_NONE, testStrings[0]);

                    // add strings for stringarray testing also
                    String[] addr = new String[((ContactList)pl).stringArraySize(Contact.ADDR)];
                    addr[Contact.ADDR_COUNTRY] = testStrings2[0];
                    c1.addStringArray(Contact.ADDR, PIMItem.ATTR_NONE, addr);
                    c1.commit();
                }
                else if (pl instanceof ToDoList)
                {
                    t1 = ((ToDoList)pl).createToDo();
                    t1.addString(ToDo.SUMMARY, PIMItem.ATTR_NONE, testStrings[0]);
                    t1.commit();
                }
            }
        }
        catch (Throwable e)
        {
            failTest("Adding test items failed: " + e);
        }
    }

    private void deleteItems() throws TestEndThrowable
    {
        try
        {
            // delete test items
            for (int i=0; i< lists.size(); i++)
            {
                PIMList pl = (PIMList) lists.elementAt(i);

                if (pl instanceof EventList)
                {
                    ((EventList)pl).removeEvent(e1);
                }
                else if (pl instanceof ContactList)
                {
                    ((ContactList)pl).removeContact(c1);
                }
                else if (pl instanceof ToDoList)
                {
                    ((ToDoList)pl).removeToDo(t1);
                }
            }
        }
        catch (Throwable e)
        {
            failTest("Deleting test items failed: " + e);
        }
    }

    private void closeLists()
    {
        try
        {
            // close lists
            for (int i=0; i< lists.size(); i++)
            {
                PIMList pl = (PIMList) lists.elementAt(i);
                if (pl != null)
                {
                    pl.close();
                    pl = null;
                }
            }
        }
        catch (Throwable t)
        {
            log("error while closing lists: " +t);
        }
    }

    public void runTest() throws TestEndThrowable
    {
        initTest();
        int[][] testCount = countItems(testStrings);

        // test step1
        int[] allItems = countAllItems();
        for (int i = 0; i < allItems.length; i++)
        {
            if (allItems[i] != testCount[i][testStrings.length-1])
            {
                PIMList pl = (PIMList) lists.elementAt(i);
                log("Testing list " + pl.getName());
                failTest("items(\"\") did not return same amount "+
                         "of items that items() returned.");
            }
        }

        // test steps 2, 3
        for (int i = 0; i < lists.size(); i++)
        {
            for (int j = 0; j < testStrings.length; j++)
            {
                if (itemCounts[i][j] != (testCount[i][j] - 1))
                {
                    PIMList pl = (PIMList) lists.elementAt(i);
                    log("Testing list " + pl.getName());
                    failTest("items("+testStrings[j]+") did not return one more item "+
                             "than items() returned before adding the test item.");
                }
            }
        }

        // test step 4
        for (int i=0; i< lists.size(); i++)
        {
            PIMList pl = (PIMList) lists.elementAt(i);

            if (pl instanceof ContactList)
            {
                for (int j=0; j< testStrings2.length; j++)
                {
                    try
                    {
                        Enumeration e = pl.items(testStrings2[j]);
                        int ascount = count(e);
                        log("Found "+ascount+" matches.");
                        if (arrayItemCounts[i][j] != (ascount - 1))
                        {
                            log("Testing(stringarray) list " + pl.getName());
                            failTest("items("+testStrings2[j]+") did not return one more item "+
                                     "than items() returned before adding the test stringarrayitem.");
                        }
                    }
                    catch (PIMException pime)
                    {
                        failTest("Could not test items("+testStrings2[j]+"): "+pime);
                    }
                }
            }
        }


        passTest(); // does step5
        cleanupTest();
    }

    public void cleanupTest()
    {
        try
        {
            // delete test items
            deleteItems();

            // close lists
            closeLists();
        }
        catch (Throwable t)
        {
            log("error while cleaning up test: " +t);
        }
    }

    private int[] countAllItems() throws TestEndThrowable
    {
        int[] counts = new int[lists.size()];
        try
        {
            for (int i=0; i< lists.size(); i++)
            {
                PIMList pl = (PIMList) lists.elementAt(i);
                counts[i] = count(pl.items());
            }
        }
        catch (Throwable t)
        {
            log("error while counting all items: " +t);
        }
        return counts;
    }


    private int count(Enumeration e)
    {
        int count = 0;

        while (e.hasMoreElements())
        {
            e.nextElement();
            count++;
        }

        return count;
    }
}
