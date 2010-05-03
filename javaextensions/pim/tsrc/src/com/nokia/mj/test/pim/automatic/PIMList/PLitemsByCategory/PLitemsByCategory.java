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
package com.nokia.mj.test.pim.automatic.PIMList.PLitemsByCategory;
//IMPORTS
import java.io.IOException;
import java.util.*;
import javax.microedition.pim.*;
import com.nokia.mj.test.pim.adaptation.*;

// CLASS DEFINITION
/**
 * This class tests javax.microedition.pim.PIMList.itemsByCategory()
 */
public class PLitemsByCategory extends MIDPTest
{

    public PLitemsByCategory(
        String sTestName, j2meunit.framework.TestMethod rTestMethod)
    {
        super(sTestName, rTestMethod);
    }

    private Vector lists = null;
    private String testCategory = "Test1";
    private int itemCount = 0;

    public void initTest() throws TestEndThrowable
    {
        // preconditions
        openLists(PIM.READ_WRITE);

        // count items in lists
        itemCount = countItems();

        // add test items & categories
        addItems();
    }


    private int countItems() throws TestEndThrowable
    {
        int count = 0;

        try
        {
            for (int i=0; i< lists.size(); i++)
            {
                PIMList pl = (PIMList) lists.elementAt(i);
                Enumeration e = pl.items();
                count += count(e);
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
            for (int i=0; i < todoListNames.length; i++)
            {
                lists.addElement(p.openPIMList(PIM.TODO_LIST,
                                               aMode, todoListNames[i]));
            }

            String[] eventListNames = p.listPIMLists(PIM.EVENT_LIST);
            for (int i=0; i < eventListNames.length; i++)
            {
                lists.addElement(p.openPIMList(PIM.EVENT_LIST,
                                               aMode, eventListNames[i]));
            }

            String[] contactListNames = p.listPIMLists(PIM.CONTACT_LIST);
            for (int i=0; i < contactListNames.length; i++)
            {
                lists.addElement(p.openPIMList(PIM.CONTACT_LIST,
                                               aMode, contactListNames[i]));
            }
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
            // add test items and categories to those lists
            // that support categories
            for (int i=0; i< lists.size(); i++)
            {
                PIMList pl = (PIMList) lists.elementAt(i);
                if (catSupported(pl))
                {
                    pl.addCategory(testCategory);
                    if (pl instanceof EventList)
                    {
                        Event e1 = ((EventList)pl).createEvent();
                        e1.addString(Event.SUMMARY, PIMItem.ATTR_NONE, "test1");
                        e1.addDate(Event.START, PIMItem.ATTR_NONE, 1010101010L);
                        e1.addToCategory(testCategory);
                        e1.commit();
                    }
                    else if (pl instanceof ContactList)
                    {
                        Contact c1 = ((ContactList)pl).createContact();
                        c1.addString(Contact.EMAIL, PIMItem.ATTR_NONE, "test1");
                        c1.addToCategory(testCategory);
                        c1.commit();
                    }
                    else if (pl instanceof ToDoList)
                    {
                        ToDo t1 = ((ToDoList)pl).createToDo();
                        t1.addString(ToDo.SUMMARY, PIMItem.ATTR_NONE, "test1");
                        t1.addToCategory(testCategory);
                        t1.commit();
                    }
                }
            }
        }
        catch (Throwable e)
        {
            failTest("Adding test items & categories failed: " + e);
        }
    }

    private void closeLists()
    {
        try
        {
            // delete categories and test items
            for (int i=0; i< lists.size(); i++)
            {
                PIMList pl = (PIMList) lists.elementAt(i);
                if (pl != null)
                {
                    // also deletes items in category
                    if (pl.maxCategories() != 0)
                    {
                        pl.deleteCategory(testCategory, true);
                    }
                }
            }

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
        // test steps 1-3
        testCategories();

        // test steps 4-6
        testSpecialCategories();

        // test steps 7-8
        closeLists();
        testPIMException();

        // test step 9
        openLists(PIM.WRITE_ONLY);
        testSecurityException();

        passTest();
        cleanupTest();
    }

    public void cleanupTest()
    {
        try
        {
            closeLists();
        }
        catch (Throwable t)
        {
            log("error while cleaning up test: " +t);
        }
    }

    private void testSpecialCategories() throws TestEndThrowable
    {
        try
        {
            for (int i=0; i< lists.size(); i++)
            {
                PIMList pl = (PIMList) lists.elementAt(i);

                // get number of uncategorized items
                int uncatCount = count(
                                     pl.itemsByCategory(PIMList.UNCATEGORIZED));

                // get number of null-categorized items
                log("testing itemsByCategory( null )");
                int nullcatCount = count(pl.itemsByCategory(null));

                if (nullcatCount != uncatCount)
                {
                    failTest(uncatCount+" uncategorized items and "+
                             nullcatCount+ " null-categorized items. " +
                             "Should be equal amount.");
                }

                log("testing itemsByCategory( \"\" )");
                String[] cats = pl.getCategories();
                boolean emptyCategory = false;

                for (int j = 0; j < cats.length; j++)
                {
                    if (cats[j].equals(""))
                    {
                        emptyCategory = true;
                    }
                }
                if (!emptyCategory)
                {
                    if (count(pl.itemsByCategory("")) != 0)
                    {
                        failTest("Calling itemsByCategory(\"\") must "+
                                 "return empty enumeration!");

                    }
                }

                log("testing itemsByCategory( <existing name in lower/uppercase> )");
                int testCount = count(pl.itemsByCategory(testCategory));

                if (testCount != 0)
                {
                    int lcCount = count(pl.itemsByCategory(testCategory.toLowerCase()));
                    if (lcCount == testCount)
                    {
                        failTest("Calling itemsByCategory( testCategory.toLowerCase() ) "+
                                 "must return empty enumeration!");
                    }

                    int ucCount = count(pl.itemsByCategory(testCategory.toUpperCase()));
                    if (ucCount == testCount)
                    {
                        failTest("Calling itemsByCategory( testCategory.toUpperCase() ) "+
                                 "must return empty enumeration!");
                    }
                }
            }
        }
        catch (Throwable t)
        {
            failTest("Error while testing special categories: " + t);
        }
    }

    private void testCategories() throws TestEndThrowable
    {
        try
        {
            for (int i=0; i< lists.size(); i++)
            {
                PIMList pl = (PIMList) lists.elementAt(i);
                // get number of items belonging to some category
                String[] cats = pl.getCategories();
                Vector items = new Vector();
                for (int j = 0; j < cats.length; j++)
                {
                    Enumeration e = pl.itemsByCategory(cats[j]);
                    addTo(items, e);
                }
                // get number of uncategorized items
                int uncatCount = count(
                                     pl.itemsByCategory(PIMList.UNCATEGORIZED));
                // get number of items in list
                int listCount = count(pl.items());

                if (uncatCount + items.size() != listCount)
                {
                    failTest(listCount+" items in list "+  pl.getName() + ". "+
                             uncatCount+" uncategorized items and "+ items.size()+
                             " categorized items. (items != uncat + cat).");

                }
            }
        }
        catch (Throwable t)
        {
            failTest("Error while testing categories: " + t);
        }
    }

    private void addTo(Vector aV, Enumeration aE)
    {
        while (aE.hasMoreElements())
        {
            Object o = aE.nextElement();
            if (!aV.contains(o))
            {
                aV.addElement(o);
            }
        }
    }

    // test that closed lists throw PIMException
    // when calling PIMList.itemsByCategory("")
    private void testPIMException() throws TestEndThrowable
    {

        for (int i=0; i< lists.size(); i++)
        {
            try
            {
                PIMList pl = (PIMList) lists.elementAt(i);
                pl.itemsByCategory("");
                failTest("Calling itemsByCategory() on a closed "+
                         "list did not throw PIMException!");
            }
            catch (PIMException pe)
            {
                // ok
            }
            catch (Exception ex)
            {
                failTest("Calling itemsByCategory() on a closed "+
                         "list did not throw PIMException but " + ex);
            }
        }
    }

    // test that write_only lists throw SecurityException
    // when calling PIMList.itemsByCategory("")
    private void testSecurityException() throws TestEndThrowable
    {

        for (int i=0; i< lists.size(); i++)
        {
            try
            {
                PIMList pl = (PIMList) lists.elementAt(i);
                pl.itemsByCategory("asfdasdf");
                failTest("Calling itemsByCategory() on a write_only "+
                         "list did not throw SecurityException!");
            }
            catch (SecurityException se)
            {
                // ok
            }
            catch (Exception ex)
            {
                failTest("Calling itemsByCategory() on a write_only "+
                         "list did not throw SecurityException but " + ex);
            }
        }
    }

    private boolean catSupported(PIMList aPL)
    {
        if (aPL.maxCategories() == 0)
        {
            return false;
        }
        return true;
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
