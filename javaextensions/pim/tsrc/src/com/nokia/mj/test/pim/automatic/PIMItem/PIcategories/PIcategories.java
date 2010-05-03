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
package com.nokia.mj.test.pim.automatic.PIMItem.PIcategories;
//IMPORTS
import java.io.IOException;
import java.util.*;
import javax.microedition.pim.PIM;
import javax.microedition.pim.PIMItem;
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
public class PIcategories extends MIDPTest
{

    public PIcategories(
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
        String[][] contactCategories = null;
        String[][] eventCategories = null;
        String[][] todoCategories = null;
        Enumeration[] contactItems = null;
        Enumeration[] eventItems = null;
        Enumeration[] todoItems = null;

        int contactMaxC = -1;
        int eventMaxC = 0;
        int todoMaxC = 0;

        try
        {
            p = PIM.getInstance();
            contactListNames = p.listPIMLists(PIM.CONTACT_LIST);
            eventListNames = p.listPIMLists(PIM.EVENT_LIST);
            todoListNames = p.listPIMLists(PIM.TODO_LIST);
            contactLists = openLists(p, PIM.CONTACT_LIST, contactListNames);
            eventLists = openLists(p, PIM.EVENT_LIST, eventListNames);
            todoLists = openLists(p, PIM.TODO_LIST, todoListNames);
            contactCategories = getCategories(contactLists);
            eventCategories = getCategories(eventLists);
            todoCategories = getCategories(todoLists);
            contactItems = getItems(contactLists);
            eventItems = getItems(eventLists);
            todoItems = getItems(todoLists);
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
        // steps s1-s9
        testAll(contactItems, contactCategories, contactMaxC);
        testAll(eventItems, eventCategories, eventMaxC);
        testAll(todoItems, todoCategories, todoMaxC);
        // step s10
        closeLists(contactLists);
        closeLists(eventLists);
        closeLists(todoLists);


        passTest();
    }

    private void testAll(Enumeration[] aItems, String[][] aCategories,
                         int aListMaxC)
    throws TestEndThrowable
    {
        int listCount = aItems.length;
        for (int i = 0; i < listCount; i++)
        {
            testAll(aItems[ i ], aCategories[ i ], aListMaxC);
        } // end of for (int i = 0; i < listCount; i++)
    }

    private void testAll(Enumeration aItems, String[] aCategories,
                         int aListMaxC)
    throws TestEndThrowable
    {
        while (aItems.hasMoreElements())
        {
            PIMItem item = (PIMItem)aItems.nextElement();
            // Test step s1
            String[] itemCategories = testCategories(item, aCategories);
            // Test step s2
            testMaxC(item, aListMaxC);
            // Test step s3
            testNullCategory(item);
            // Test step s4
            testInvalidCategory(item);
            // Test step s5
            String[] addedCategories = testAddCategories(item, itemCategories,
                                       aCategories);
            // Test step s6
            testDoubleAdd(item, addedCategories);
            // Test step s7
            testNullRemove(item);
            // Test step s8
            testRemoveCategories(item, addedCategories);
            // Test step s9
            testDoubleRemoveCategories(item, addedCategories);
        } // end of while (aItems.hasMoreElements())
    }

    /**
     * Test Step S1
     */
    private String[] testCategories(PIMItem aItem, String[] aCategories)
    throws TestEndThrowable
    {
        String[] categories = null;
        try
        {
            categories = aItem.getCategories();
        }
        catch (Throwable e)
        {
            failTest("aItem.getCategories threw: " + e.toString());
        } // end of try-catch

        int categoryCount = categories.length;
        int listCategoryCount = aCategories.length;
        for (int i = 0; i < categoryCount; i++)
        {
            boolean found = false;
            for (int j = 0; j < listCategoryCount; j++)
            {
                if (categories[ i ].compareTo(aCategories[ j ]) == 0)
                {
                    found = true;
                    break;
                }
            } // end of for (int j = 0; j < listCategoryCount; j++)
            if (!found)
            {
                failTest("item category not in list: " + categories[ i ]);
            } // end of if (!found)
        } // end of for (int i = 0; i < categoryCount; i++)
        return categories;
    }

    /**
     * Test Step S2
     */
    private void testMaxC(PIMItem aItem, int aListMaxC)
    throws TestEndThrowable
    {
        int maxCategories = 0;
        try
        {
            maxCategories = aItem.maxCategories();
        }
        catch (Throwable e)
        {
            failTest("aItem.maxCategories threw: " + e.toString());
        } // end of try-catch
        if (maxCategories != aListMaxC)
        {
            failTest("Item returned wrong maxCategories: " + maxCategories);
        } // end of if (maxCategories != aListMaxC)
    }

    /**
     * Test step S3
     */
    private void testNullCategory(PIMItem aItem)
    throws TestEndThrowable
    {
        int categoryCount = aItem.getCategories().length;
        boolean exceptionThrown = false;
        try
        {
            aItem.addToCategory(null);
        }
        catch (NullPointerException e)
        {
            exceptionThrown = true;
        } // end of try-catch
        catch (Throwable e)
        {
            failTest("addToCategory(null) threw:" + e.toString());
        } // end of catch

        if (!exceptionThrown)
        {
            failTest("addToCategory(null) did not throw NullPointer");
        } // end of if (!exceptionThrown)
        int newCategoryCount = aItem.getCategories().length;
        if (newCategoryCount != categoryCount)
        {
            failTest("addToCategory(null) changed category count");
        } // end of if (newCategoryCount != categoryCount)
    }

    /**
     * Test step S4
     */
    private void testInvalidCategory(PIMItem aItem)
    throws TestEndThrowable
    {
        int categoryCount = aItem.getCategories().length;
        boolean exceptionThrown = false;
        try
        {
            aItem.addToCategory("ThisIsNotACateogry");
        }
        catch (PIMException e)
        {
            exceptionThrown = true;
        } // end of try-catch
        if (!exceptionThrown)
        {
            failTest("addToCategory(<invalid>) did not throw PIMException");
        } // end of if (!exceptionThrown)
        int newCategoryCount = aItem.getCategories().length;
        if (newCategoryCount != categoryCount)
        {
            failTest("addToCategory(<invalid>) changed category count");
        } // end of if (newCategoryCount != categoryCount)
    }

    /**
     * Test step s5
     */
    private String[] testAddCategories(PIMItem aItem,
                                       String[] aItemCategories,
                                       String[] aListCategories)
    throws TestEndThrowable
    {
        int itemCategoryCount = aItemCategories.length;
        int itemCategoryListCount = aItemCategories.length;
        int listCategoryCount = aListCategories.length;
        int maxCategories = aItem.maxCategories();
        String[] addedCategories =
            new String[ listCategoryCount - itemCategoryCount ];
        int addCount = 0;
        for (int i = 0; i < listCategoryCount; i++)
        {
            boolean found = false;
            for (int j = 0; j < itemCategoryListCount; j++)
            {
                log("comparing: \"" + aListCategories[ i ] + "\" and \"" + aItemCategories[ j ] + "\"");
                if (aListCategories[ i ].compareTo(aItemCategories[ j ])
                        == 0)
                {
                    log("found category:" + aItemCategories[ j ]);
                    found = true;
                    break;
                }
            }
            for (int j = 0; j < addCount; j++)
            {
                if (aListCategories[ i ].compareTo(addedCategories[ j ])
                        == 0)
                {
                    found = true;
                    break;
                }
            }
            if (!found)
            {
                // we must add it
                boolean exceptionThrown = false;
                try
                {
                    aItem.addToCategory(aListCategories[ i ]);
                }
                catch (PIMException e)
                {
                    exceptionThrown = true;
                    if (itemCategoryCount < maxCategories)
                    {
                        failTest("addToCategory threw PIMException");
                    } // end of if (itemCategoryCount < maxCategories)
                } // end of try-catch
                catch (Throwable e)
                {
                    failTest("addToCategory threw: " + e.toString());
                } // end of catch
                itemCategoryCount++;
                if ((maxCategories != -1) &&
                        (itemCategoryCount > maxCategories))
                {
                    if (!exceptionThrown)
                    {
                        log("itemCategoryCount = " + itemCategoryCount);
                        log("maxCategories = " + maxCategories);
                        failTest("addToCategory did not throw PIMException");
                    } // end of if (!exceptionThrown)
                }
                int newCategoryCount = aItem.getCategories().length;
                if (newCategoryCount != itemCategoryCount)
                {
                    failTest("category was not added");
                } // end of if (newCategoryCount != itemCategoryCount)
                addedCategories[ addCount++ ] = aListCategories[ i ];
            } // end of if (!found)
        } // end of for (int i = 0; i < listCategoryCount; i++)
        String[] retVal = new String[ addCount ];
        for (int i = 0; i < addCount; i++)
        {
            retVal[ i ] = addedCategories[ i ];
        } // end of for (int i = 0; i < addCount; i++)

        return retVal;
    }

    /**
     * Test step s6
     */
    private void testAddCategories2(PIMItem aItem,
                                    String[] aAddedCategories)
    throws TestEndThrowable
    {
        int addedCategoryCount = aAddedCategories.length;
        for (int i = 0; i < addedCategoryCount; i++)
        {
            try
            {
                aItem.addToCategory(aAddedCategories[ i ]);
            }
            catch (PIMException e)
            {
                failTest("S6: addToCategory threw PIMException");
            } // end of try-catch
            catch (Throwable e)
            {
                failTest("S6: addToCategory threw: " + e.toString());
            } // end of catch
        }
        return;
    }


    private void testDoubleAdd(PIMItem aItem, String[] aAddedCategories)
    throws TestEndThrowable
    {
        String[] itemOldCategories = null;
        String[] itemNewCategories = null;
        try
        {
            itemOldCategories = aItem.getCategories();
            testAddCategories2(aItem, aAddedCategories);
            itemNewCategories = aItem.getCategories();

        }
        catch (TestEndThrowable e)
        {
            throw e;
        } // end of try-catch
        catch (Throwable e)
        {
            failTest("Test step s6: " + e.toString());
        } // end of catch
        if (itemOldCategories.length != itemNewCategories.length)
        {
            failTest("repeating step 5 changed category count");
        }
    }

    /**
     * Test step S7
     */
    private void testNullRemove(PIMItem aItem)
    throws TestEndThrowable
    {
        int categoryCount = aItem.getCategories().length;
        boolean exceptionThrown = false;
        try
        {
            aItem.removeFromCategory(null);
        }
        catch (NullPointerException e)
        {
            exceptionThrown = true;
        } // end of try-catch
        catch (Throwable e)
        {
            failTest("removeFromCategory(null) threw:" + e.toString());
        } // end of catch

        if (!exceptionThrown)
        {
            failTest("removeFromCategory(null) did not throw NullPointer");
        } // end of if (!exceptionThrown)
        int newCategoryCount = aItem.getCategories().length;
        if (newCategoryCount != categoryCount)
        {
            failTest("removeFromCategory(null) changed category count");
        } // end of if (newCategoryCount != categoryCount)
    }

    /**
     * Test step s8
     */
    private void testRemoveCategories(PIMItem aItem, String[] aAddedCategories)
    throws TestEndThrowable
    {
        String[] itemCategories = null;
        try
        {
            itemCategories = aItem.getCategories();
        }
        catch (Throwable e)
        {
            failTest("S8: getCategories threw:" + e.toString());
        } // end of try-catch
        int itemCategoryCount = itemCategories.length;
        int addedCategoryCount = aAddedCategories.length;
        for (int i = 0; i < addedCategoryCount; i++)
        {
            try
            {
                aItem.removeFromCategory(aAddedCategories[ i ]);
                itemCategories = aItem.getCategories();
            }
            catch (Throwable e)
            {
                failTest("S8: " + e.toString());
            } // end of try-catch
            itemCategoryCount--;
            if (itemCategoryCount != itemCategories.length)
            {
                failTest("removeFromCategory did not remove from category");
            }
        }
    }

    /**
     * Test step s9
     */
    private void testDoubleRemoveCategories(PIMItem aItem,
                                            String[] aAddedCategories)
    throws TestEndThrowable
    {
        String[] itemCategories = null;
        try
        {
            itemCategories = aItem.getCategories();
        }
        catch (Throwable e)
        {
            failTest("S9: getCategories threw:" + e.toString());
        } // end of try-catch
        int itemCategoryCount = itemCategories.length;
        int addedCategoryCount = aAddedCategories.length;
        for (int i = 0; i < addedCategoryCount; i++)
        {
            try
            {
                aItem.removeFromCategory(aAddedCategories[ i ]);
                itemCategories = aItem.getCategories();
            }
            catch (Throwable e)
            {
                failTest("S8: " + e.toString());
            } // end of try-catch
            if (itemCategoryCount != itemCategories.length)
            {
                failTest("double removeFromCategory removed from category");
            }
        }
    }

    /**
     * Test step s10
     */
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


    private String[][] getCategories(PIMList[] aLists)
    throws PIMException
    {
        int listCount = aLists.length;
        String[][] categories= new String[ listCount ][];
        for (int i = 0; i < listCount; i++)
        {
            categories[ i ] = aLists[ i ].getCategories();
        } // end of for (int i = 0; i < listCount; i++)
        return categories;
    }


    private PIMList[] openLists(PIM aP, int aListType, String[] aListNames)
    throws PIMException
    {
        int listCount = aListNames.length;
        PIMList[] pimLists = new PIMList[ listCount ];
        for (int i = 0; i < listCount; i++)
        {
            pimLists[ i ] = aP.openPIMList(aListType, PIM.READ_ONLY,
                                           aListNames[ i ]);
        } // end of for (int i = 0; i < listCount; i++)
        return pimLists;
    }
}

