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
package com.nokia.mj.test.pim.automatic.PIMList.PLrenameCategory;
//IMPORTS
import java.io.IOException;
import java.util.*;
import javax.microedition.pim.*;
import com.nokia.mj.test.pim.adaptation.*;

// CLASS DEFINITION
/**
 * This class tests javax.microedition.pim.PIMList.renameCategory()
 */
public class PLrenameCategory extends MIDPTest
{

    public PLrenameCategory(
        String sTestName, j2meunit.framework.TestMethod rTestMethod)
    {
        super(sTestName, rTestMethod);
    }

    private Hashtable categories = new Hashtable();
    private Hashtable newItems1 = new Hashtable();
    private String[] testCategories = { "test1cat", "test2cat", "newtest1cat" };

    public void runTest() throws TestEndThrowable
    {
        // preconditions
        PIMList[] allLists = openAllLists(PIM.READ_WRITE);

        if (allLists.length > 0)
        {
            try
            {
                // test step 1
                addNewCategories(allLists);
                addNewItems(allLists);

                // test step 2
                testRenameCategories(allLists);

                // test step 3
                testRenameCategories2(allLists);

                // test step 4
                testRenameCategoryException(allLists, "gsafgsuo", "guoawgeuf");

                // test step 5
                testRenameCategoryException(allLists, "gsafgsuo", testCategories[1]);

                // test step 6
                testRenameCategoryException(allLists, testCategories[1], testCategories[1]);

                // test step 7
                testRenameCategoryException(allLists, null, testCategories[1]);
                testRenameCategoryException(allLists, testCategories[1], null);
            }
            catch (TestEndThrowable tet)
            {
                // we only catch this to have the cleanup in finally...
                throw tet;
            }
            finally
            {
                cleanup(allLists);
            }
        }

        passTest();
    }


    private void removeAddedItems(PIMList[] lists) throws TestEndThrowable
    {
        try
        {
            for (int i = 0; i < lists.length; i++)
            {
                if (lists[i] instanceof EventList)
                {
                    Event e = (Event)newItems1.get(lists[i]);
                    if (e != null)
                    {
                        ((EventList)lists[i]).removeEvent(e);
                    }
                }
                else if (lists[i] instanceof ContactList)
                {
                    Contact c = (Contact)newItems1.get(lists[i]);
                    if (c != null)
                    {
                        ((ContactList)lists[i]).removeContact(c);
                    }
                }
                else if (lists[i] instanceof ToDoList)
                {
                    ToDo t = (ToDo)newItems1.get(lists[i]);
                    if (t != null)
                    {
                        ((ToDoList)lists[i]).removeToDo(t);
                    }
                }
            }
        }
        catch (Exception e)
        {
            failTest("Error while removing added items from list: "+e);
        }
    }

    private int itemCount(PIMList list) throws TestEndThrowable
    {
        int count = 0;
        try
        {
            Enumeration items = list.items();
            while (items.hasMoreElements())
            {
                PIMItem tmp = (PIMItem) items.nextElement();
                count++;
            }
        }
        catch (Exception e)
        {
            failTest("Error counting items in list: "+e);
        }
        return count;
    }

    private boolean find(PIMList list, PIMItem item, String category) throws TestEndThrowable
    {
        try
        {
            Enumeration items = list.items(item);
            while (items.hasMoreElements())
            {
                PIMItem tmp = (PIMItem) items.nextElement();
                String[] cats = tmp.getCategories();
                if (find(cats, category))
                {
                    return true;
                }
            }
        }
        catch (IllegalArgumentException iae)
        {
            // item does not originate from the list
            return false;
        }
        catch (Exception e)
        {
            failTest("Error finding item: "+e);
        }
        return false;
    }

    private boolean find(PIMList list, PIMItem item) throws TestEndThrowable
    {
        try
        {
            Enumeration items = list.items(item);
            if (items.hasMoreElements())
            {
                return true;
            }
        }
        catch (IllegalArgumentException iae)
        {
            // item does not originate from the list
            return false;
        }
        catch (Exception e)
        {
            failTest("Error finding item: "+e);
        }
        return false;
    }

    private boolean find(String[] array, String item)
    {
        for (int i = 0; i < array.length; i++)
        {
            if (item.equals(array[i]))
            {
                return true;
            }
        }
        return false;
    }

    private void addNewItems(PIMList[] lists) throws TestEndThrowable
    {
        try
        {
            for (int i = 0; i < lists.length; i++)
            {
                String[] cats = lists[i].getCategories();
                if (cats.length != 0)
                {
                    if (lists[i] instanceof EventList)
                    {
                        Event e1 = ((EventList)lists[i]).createEvent();
                        e1.addString(Event.SUMMARY, PIMItem.ATTR_NONE, testCategories[0]);
                        e1.addDate(Event.START, PIMItem.ATTR_NONE, 1010101010L);
                        e1.addToCategory(testCategories[0]);
                        e1.commit();
                        newItems1.put(lists[i], e1);
                    }
                    else if (lists[i] instanceof ContactList)
                    {
                        Contact c1 = ((ContactList)lists[i]).createContact();
                        c1.addString(Contact.EMAIL, PIMItem.ATTR_NONE, testCategories[0]);
                        c1.addToCategory(testCategories[0]);
                        c1.commit();
                        newItems1.put(lists[i], c1);
                    }
                    else if (lists[i] instanceof ToDoList)
                    {
                        ToDo t1 = ((ToDoList)lists[i]).createToDo();
                        t1.addString(ToDo.SUMMARY, PIMItem.ATTR_NONE, testCategories[0]);
                        t1.addToCategory(testCategories[0]);
                        t1.commit();
                        newItems1.put(lists[i], t1);
                    }
                }
            }
        }
        catch (Exception e)
        {
            failTest("Error while adding new items to lists: "+e);
        }
    }

    private void cleanup(PIMList[] lists) throws TestEndThrowable
    {
        try
        {
            for (int i = 0; i < lists.length; i++)
            {
                lists[i].deleteCategory(testCategories[0], false);
                lists[i].deleteCategory(testCategories[1], false);
                lists[i].deleteCategory(testCategories[2], false);
            }
            removeAddedItems(lists);
        }
        catch (Exception e)
        {
            failTest("error during cleanup: "+e);
        }
    }

    private void testRenameCategoryException(PIMList[] lists, String first, String second) throws TestEndThrowable
    {
        for (int i = 0; i < lists.length; i++)
        {
            String[] cats = null;
            try
            {
                cats = lists[i].getCategories();
                lists[i].renameCategory(first, second);
            }
            catch (PIMException pime)
            {
                if (find(cats, first))
                {
                    failTest("error in renaming category exceptions: "+pime);
                }
            }
            catch (NullPointerException npe)
            {
                if (first != null && second != null)
                {
                    failTest("error in renaming category exceptions: "+npe);
                }
            }
            catch (Exception e)
            {
                failTest("error in renaming category exceptions: "+e);
            }

            try
            {
                String[] cats2 = lists[i].getCategories();
                if (cats.length != cats2.length)
                {
                    log("Rename: "+ first +" to "+second);
                    log("Old Categories: "+ asString(cats));
                    log("Categories found: "+ asString(cats2));
                    failTest("Category size was changed!");
                }
            }
            catch (Exception e)
            {
                failTest("error in renaming category exceptions: "+e);
            }
        }
    }


    private void testRenameCategories2(PIMList[] lists) throws TestEndThrowable
    {
        try
        {
            for (int i = 0; i < lists.length; i++)
            {
                String[] cats = lists[i].getCategories();
                if (cats.length != 0)
                {

                    lists[i].renameCategory(testCategories[2], testCategories[1]);

                    String[] cats2 = lists[i].getCategories();

                    if (cats.length == cats2.length)
                    {
                        failTest("Category #1 was not removed!");
                    }
                    if (find(cats2, testCategories[2]))
                    {
                        log("Old Categories: "+ asString(cats));
                        log("Categories found: "+ asString(cats2));
                        failTest("Category was renamed but old name exists!");
                    }
                    Enumeration e = lists[i].itemsByCategory(testCategories[1]);
                    int count = 0;
                    while (e.hasMoreElements())
                    {
                        e.nextElement();
                        count++;
                    }
                    if (count != 1)
                    {
                        log("Old Categories: "+ asString(cats));
                        log("Categories found: "+ asString(cats2));
                        failTest("Category was renamed but item is not returned by list.itemsByCategory("+testCategories[1]+")!");
                    }
                }
            }
        }
        catch (Exception e)
        {
            failTest("error while renaming category #1 to #2: "+e);
        }
    }

    private void testRenameCategories(PIMList[] lists) throws TestEndThrowable
    {
        try
        {
            for (int i = 0; i < lists.length; i++)
            {
                String[] cats = lists[i].getCategories();
                if (cats.length != 0)
                {
                    if (!find(cats, testCategories[0]))
                    {
                        log("Categories found: "+ asString(cats));
                        failTest("Category #1 ("+testCategories[0]+") not in lists.");
                    }
                    lists[i].renameCategory(testCategories[0], testCategories[2]);

                    String[] cats2 = lists[i].getCategories();

                    if (!find(cats2, testCategories[2]))
                    {
                        failTest("Category #1 was not renamed!");
                    }
                    if (cats.length == cats2.length)
                    {
                        log("Old Categories: "+ asString(cats));
                        log("Categories found: "+ asString(cats2));
                        failTest("Category was renamed but old name exists!");
                    }
                }
            }
        }
        catch (Exception e)
        {
            failTest("error while renaming category #1 to lists: "+e);
        }
    }

    private String asString(String[] values)
    {
        String ret = "";
        if (values.length == 0)
        {
            return ret;
        }

        for (int i=0; i <values.length-1; i++)
        {
            ret += values[i]+", ";
        }
        ret += values[values.length-1];
        return ret;
    }

    private void addNewCategories(PIMList[] lists) throws TestEndThrowable
    {
        try
        {
            for (int i = 0; i < lists.length; i++)
            {
                int max1 = lists[i].maxCategories();
                for (int j = 0; j < testCategories.length; j++)
                {
                    lists[i].addCategory(testCategories[j]);
                }
                int max2 = lists[i].maxCategories();
                if (max2 != -1 && (max2-max1 != 2))
                {
                    log("max categories before adding: "+max1+", after adding: "+max2);
                    failTest("error while adding new categories to lists.");
                }
            }
        }
        catch (Exception e)
        {
            failTest("error while adding new categories to lists: "+e);
        }
    }


    private void closeLists(PIMList[] lists) throws TestEndThrowable
    {
        try
        {
            for (int i = 0; i < lists.length; i++)
            {
                lists[i].close();
            }
        }
        catch (Exception e)
        {
            failTest("Error while closing lists: " +e);
        }
    }

    private PIMList[] openAllLists(int mode) throws TestEndThrowable
    {
        try
        {
            PIM p = PIM.getInstance();
            String[] contactListNames = p.listPIMLists(PIM.CONTACT_LIST);
            String[] eventListNames = p.listPIMLists(PIM.EVENT_LIST);
            String[] todoListNames = p.listPIMLists(PIM.TODO_LIST);
            PIMList[] contactLists = openLists(p, PIM.CONTACT_LIST, contactListNames, mode);
            PIMList[] eventLists = openLists(p, PIM.EVENT_LIST, eventListNames, mode);
            PIMList[] todoLists = openLists(p, PIM.TODO_LIST, todoListNames, mode);

            return combineLists(contactLists, eventLists, todoLists);
        }
        catch (Throwable e)
        {
            failTest("preconditions failed: " + e);
        }
        return null;
    }

    private PIMList[] openLists(PIM aP, int aListType, String[] aNames, int aMode)
    throws TestEndThrowable
    {
        Vector v = new Vector();
        for (int i = 0; i < aNames.length; i++)
        {
            PIMList pl = null;
            try
            {
                pl = aP.openPIMList(aListType, aMode, aNames[ i ]);
                String[] names = pl.getCategories();
                int max = pl.maxCategories();
                if (max > -1 && (max-names.length < 2))
                {
                    log(pl.getName() + " cannot be tested. Too few categories ("+max+") left.");
                    pl.close();
                }
                else if (max < -1)
                {
                    failTest(pl.getName() +".maxCategories() returned "+max+"!");
                }
                else
                {
                    log(pl.getName()+ " can be tested. Max categories = "+max+". Categories found: "+ asString(names));
                    categories.put(pl.getName(), names);
                    v.addElement(pl);
                }
            }
            catch (Exception e)
            {
                failTest("error while opening lists: "+e);
            }
        }
        PIMList[] pimLists = new PIMList[ v.size()];
        for (int i = 0; i < v.size(); i++)
        {
            pimLists[i] = (PIMList) v.elementAt(i);
        }
        return pimLists;
    }


    private PIMList[] combineLists(PIMList[] pl1, PIMList[] pl2, PIMList[] pl3)
    {
        PIMList[] pimLists = new PIMList[ pl1.length + pl2.length + pl3.length ];
        int i = 0;
        for (int j=0; j < pl1.length; j++)
        {
            pimLists[ i++ ] = pl1[j];
        }
        for (int j=0; j < pl2.length; j++)
        {
            pimLists[ i++ ] = pl2[j];
        }
        for (int j=0; j < pl3.length; j++)
        {
            pimLists[ i++ ] = pl3[j];
        }
        return pimLists;
    }
}
