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
package com.nokia.mj.test.pim.automatic.PIMList.PLdeleteCategory;
//IMPORTS
import java.io.IOException;
import java.util.*;
import javax.microedition.pim.*;
import com.nokia.mj.test.pim.adaptation.*;

// CLASS DEFINITION
/**
 * This class tests javax.microedition.pim.PIMList.deleteCategory()
 */
public class PLdeleteCategory extends MIDPTest
{

    public PLdeleteCategory(
        String sTestName, j2meunit.framework.TestMethod rTestMethod)
    {
        super(sTestName, rTestMethod);
    }

    private Hashtable categories = new Hashtable();
    private Hashtable newItems1 = new Hashtable();
    private Hashtable newItems2 = new Hashtable();
    private String[] testCategories = { "test1", "test2", "test3", "test4" };

    public void runTest() throws TestEndThrowable
    {
        // preconditions
        PIMList[] allLists = openAllLists(PIM.READ_WRITE);

        if (allLists.length > 0)
        {
            // test step 1
            addNewCategories(allLists);

            // test step 2
            addNewItems(allLists);

            // test step 3
            testDeleteCategory(0, allLists, false, false);

            // test step 4
            testDeleteCategory(1, allLists, true, false);

            // test step 5
            testDeleteCategory(2, allLists, true, true);

            // test step 6
            testDeleteCategory2(3, allLists, false, false);
            removeAddedItems(allLists);
        }
        // test step 7, 8, 9
        testExceptions(allLists);

        passTest();
    }

    private void testExceptions(PIMList[] lists) throws TestEndThrowable
    {
        for (int i = 0; i < lists.length; i++)
        {
            // test invalid parameter
            if (lists[i].maxCategories() == 0)
            {
                try
                {
                    lists[i].deleteCategory("gawuoeagsoufso", true);
                    failTest("deleteCategory(<non-existing>, true) did not throw PIMException, even though categories are not supported by list.");
                }
                catch (PIMException pime)
                {
                    //ok
                }
                catch (Exception e)
                {
                    failTest("categories are not supported by list and deleteCategory(<non-existing>, true) threw : "+e);
                }
            }
            else
            {
                try
                {
                    lists[i].deleteCategory("gawuoeagsoufso", true);
                }
                catch (Exception e)
                {
                    failTest("deleteCategory(<non-existing>, true) threw : "+e);
                }
            }
            // test null parameter
            try
            {
                lists[i].deleteCategory(null, true);
                failTest("deleteCategory(null, true) did not throw NullPointerException.");
            }
            catch (NullPointerException npe)
            {
                //ok
            }
            catch (Exception e)
            {
                failTest("deleteCategory(<null>, true) threw : "+e);
            }

            // close list
            try
            {
                lists[i].close();
            }
            catch (Exception e)
            {
                failTest("Error while closing list: " +e);
            }

            // test closed list parameter
            try
            {
                lists[i].deleteCategory("oguguo", true);
                failTest("<closed list>.deleteCategory(name, true) did not throw PIMException.");
            }
            catch (PIMException npe)
            {
                //ok
            }
            catch (Exception e)
            {
                failTest("<closed list>.deleteCategory(name, true) threw : "+e);
            }
        }
    }

    private void removeAddedItems(PIMList[] lists) throws TestEndThrowable
    {
        try
        {
            for (int i = 0; i < lists.length; i++)
            {
                if (lists[i] instanceof EventList)
                {
                    ((EventList)lists[i]).removeEvent((Event)newItems2.get(lists[i]));
                }
                else if (lists[i] instanceof ContactList)
                {
                    ((ContactList)lists[i]).removeContact((Contact)newItems2.get(lists[i]));
                }
                else if (lists[i] instanceof ToDoList)
                {
                    ((ToDoList)lists[i]).removeToDo((ToDo)newItems2.get(lists[i]));
                }
            }
        }
        catch (Exception e)
        {
            failTest("Error while removing added items from list: "+e);
        }
    }

    private void testDeleteCategory2(int index, PIMList[] lists, boolean delUnassigned, boolean shouldBeDeleted) throws TestEndThrowable
    {
        String cat = testCategories[index];
        try
        {
            for (int i = 0; i < lists.length; i++)
            {
                log("itemcount before: "+ itemCount(lists[i]));
                lists[i].deleteCategory(cat, delUnassigned);
                log("itemcount after: "+ itemCount(lists[i]));

                String[] cats = lists[i].getCategories();
                if (find(cats, cat))
                {
                    failTest("Category: "+cat+" was not delete from list "+ lists[i].getName());
                }
                PIMItem item = (PIMItem) newItems2.get(lists[i]);
                // test if item is still in list
                boolean found = find(lists[i], item);
                if (found == shouldBeDeleted)
                {
                    if (shouldBeDeleted)
                    {
                        failTest("Item2 was NOT removed from the list when category was deleted!");
                    }
                    failTest("Item2 was removed from the list when category was deleted!");
                }

                if (find(lists[i], item, cat))
                {
                    failTest("Category: "+cat+" was deleted from list "+ lists[i].getName() +
                             ", but item2 "+item+" is still associated with category.");
                }
            }
        }
        catch (Exception e)
        {
            failTest("Error while testing deleteCategory(): "+e);
        }
    }

    private void testDeleteCategory(int index, PIMList[] lists, boolean delUnassigned, boolean shouldBeDeleted) throws TestEndThrowable
    {
        String cat = testCategories[index];
        try
        {
            for (int i = 0; i < lists.length; i++)
            {
                lists[i].deleteCategory(cat, delUnassigned);

                String[] cats = lists[i].getCategories();
                if (find(cats, cat))
                {
                    failTest("Category: "+cat+" was not delete from list "+ lists[i].getName());
                }
                PIMItem item = (PIMItem) newItems1.get(lists[i]);
                // test if item is still in list
                if (find(lists[i], item) == shouldBeDeleted)
                {
                    if (shouldBeDeleted)
                    {
                        failTest("Item1 was NOT removed from the list when category was deleted!");
                    }
                    failTest("Item1 was removed from the list when category was deleted!");
                }

                if (find(lists[i], item, cat))
                {
                    failTest("Category: "+cat+" was deleted from list "+ lists[i].getName() +
                             ", but item1 "+item+" is still associated with category.");
                }
            }
        }
        catch (Exception e)
        {
            failTest("Error while testing deleteCategory(): "+e);
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
                if (lists[i] instanceof EventList)
                {
                    Event e1 = ((EventList)lists[i]).createEvent();
                    e1.addString(Event.SUMMARY, PIMItem.ATTR_NONE, "test1");
                    e1.addDate(Event.START, PIMItem.ATTR_NONE, 1010101010L);
                    e1.addToCategory(testCategories[0]);
                    e1.addToCategory(testCategories[1]);
                    e1.addToCategory(testCategories[2]);
                    e1.commit();
                    newItems1.put(lists[i], e1);

                    Event e2 = ((EventList)lists[i]).createEvent();
                    e2.addString(Event.SUMMARY, PIMItem.ATTR_NONE, "test2");
                    e2.addDate(Event.START, PIMItem.ATTR_NONE, 1010101010L);
                    e2.addToCategory(testCategories[3]);
                    e2.commit();
                    newItems2.put(lists[i], e2);
                }
                else if (lists[i] instanceof ContactList)
                {
                    Contact c1 = ((ContactList)lists[i]).createContact();
                    c1.addString(Contact.EMAIL, PIMItem.ATTR_NONE, "test1");
                    c1.addToCategory(testCategories[0]);
                    c1.addToCategory(testCategories[1]);
                    c1.addToCategory(testCategories[2]);
                    c1.commit();
                    newItems1.put(lists[i], c1);

                    Contact c2 = ((ContactList)lists[i]).createContact();
                    c2.addString(Contact.EMAIL, PIMItem.ATTR_NONE, "test2");
                    c2.addToCategory(testCategories[3]);
                    c2.commit();
                    newItems2.put(lists[i], c2);
                }
                else if (lists[i] instanceof ToDoList)
                {
                    ToDo t1 = ((ToDoList)lists[i]).createToDo();
                    t1.addString(ToDo.SUMMARY, PIMItem.ATTR_NONE, "test1");
                    t1.addToCategory(testCategories[0]);
                    t1.addToCategory(testCategories[1]);
                    t1.addToCategory(testCategories[2]);
                    t1.commit();
                    newItems1.put(lists[i], t1);

                    ToDo t2 = ((ToDoList)lists[i]).createToDo();
                    t2.addString(ToDo.SUMMARY, PIMItem.ATTR_NONE, "test2");
                    t2.addToCategory(testCategories[3]);
                    t2.commit();
                    newItems2.put(lists[i], t2);
                }
            }
        }
        catch (Exception e)
        {
            failTest("Error while adding new items to lists: "+e);
        }
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
                if (max2 != -1 && (max2-max1 != 4))
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
                if (max > -1 && (max-names.length < 4))
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
                    log(pl.getName()+ " can be tested. Max categories = "+max+".");
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
