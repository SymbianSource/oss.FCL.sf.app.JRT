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
package com.nokia.mj.test.pim.automatic.PIMList.PLaddCategory;
//IMPORTS
import java.io.IOException;
import java.util.*;
import javax.microedition.pim.PIM;
import javax.microedition.pim.PIMException;
import javax.microedition.pim.PIMList;
import com.nokia.mj.test.pim.adaptation.*;

// CLASS DEFINITION
/**
 * This class tests javax.microedition.pim.PIMList.addCategory() and
 * javax.microedition.pim.PIMList.maxCategories()
 */
public class PLaddCategory extends MIDPTest
{

    public PLaddCategory(
        String sTestName, j2meunit.framework.TestMethod rTestMethod)
    {
        super(sTestName, rTestMethod);
    }

    private String iTestCategoryName = "PLaddCategory";

    public void runTest() throws TestEndThrowable
    {
        // preconditions
        PIM p = null;
        String[] contactListNames = null;
        String[] eventListNames = null;
        String[] todoListNames = null;
        PIMList[] cl = null;
        PIMList[] el = null;
        PIMList[] tl = null;
        String[][] cc = null;
        String[][] ec = null;
        String[][] tc = null;
        int[] cMax = null;
        int[] eMax = null;
        int[] tMax = null;

        try
        {
            p = PIM.getInstance();
            contactListNames = p.listPIMLists(PIM.CONTACT_LIST);
            eventListNames = p.listPIMLists(PIM.EVENT_LIST);
            todoListNames = p.listPIMLists(PIM.TODO_LIST);
            cl = openLists(p, PIM.CONTACT_LIST, contactListNames);
            el = openLists(p, PIM.EVENT_LIST, eventListNames);
            tl = openLists(p, PIM.TODO_LIST, todoListNames);
            cc = getCategories(cl);
            ec = getCategories(el);
            tc = getCategories(tl);
            cMax = getMax(cl);
            eMax = getMax(el);
            tMax = getMax(tl);
        }
        catch (Throwable e)
        {
            failTest("preconditions failed: " + e.toString());
        } // end of try-catch

        // Test steps
        // step s1

        testAddCategory(cl, cc, iTestCategoryName);
        testAddCategory(el, ec, iTestCategoryName);
        testAddCategory(tl, tc, iTestCategoryName);

        // step s2

        testAddCategory(cl, cc, iTestCategoryName);
        testAddCategory(el, ec, iTestCategoryName);
        testAddCategory(tl, tc, iTestCategoryName);

        // steps s3-s5

        testMaxC(cl, cMax);
        testMaxC(el, eMax);
        testMaxC(tl, tMax);

        // steps s6-s8

        testExceptions(cl);
        testExceptions(el);
        testExceptions(tl);


        passTest();
    }

    private void testGetCategories(PIMList aList, String aCategory)
    throws TestEndThrowable
    {
        String[] categories = null;
        try
        {
            categories = aList.getCategories();
        }
        catch (Throwable e)
        {
            failTest("getCategories threw: " + e.toString());
        } // end of try-catch
        int categoryCount = categories.length;
        int foundCount = 0;
        for (int i = 0; i < categoryCount; i++)
        {
            if (categories[ i ].compareTo(aCategory) == 0)
            {
                foundCount++;
            } // end of if (categories[ i ].compareTo( aCategory ) == 0)
        } // end of for (int = 0; i < categoryCount; i++)
        if (foundCount == 0)
        {
            failTest("getCategories did not return: " + aCategory);
        } // end of if (foundCount == 0)
        if (foundCount > 1)
        {
            failTest("getCategories returned " + aCategory +
                     " " + foundCount + " times");
        } // end of if (foundCount > 1)
    }

    private String makeNewCategory(String aCategory, String[] aOldCategories)
    {
        int suffix = 0;
        String newCategory = aCategory;
        int categoryCount = aOldCategories.length;
        while (true)
        {
            boolean found = false;
            for (int i = 0; i < categoryCount; i++)
            {
                if (aOldCategories[ i ].compareTo(newCategory) == 0)
                {
                    found = true;
                    break;
                }
            } // end of for (int i = 0; i < categoryCount; i++)
            if (!found)
            {
                return newCategory;
            } // end of if (!found)
            newCategory = aCategory + suffix++;
        } // end of while (true)
    }

    private void testAddCategory(PIMList aList, String[] aOldCategories,
                                 String aCategory)
    throws TestEndThrowable
    {
        // figure out a new category name
        String newCategoryName = makeNewCategory(aCategory, aOldCategories);
        try
        {
            aList.addCategory(newCategoryName);
        }
        catch (PIMException e)
        {
            // categories may be unsupported, which is not an error
            int reason = e.getReason();
            if ((reason == PIMException.FEATURE_NOT_SUPPORTED) ||
                    (reason == PIMException.MAX_CATEGORIES_EXCEEDED))
            {
                return;
            }
            failTest("addCategory threw PIME: " + e.getMessage());
        } // end of try-catch
        catch (Throwable e)
        {
            failTest("addCategory threw: " + e.toString());
        } // end of try-catch
        testGetCategories(aList, aCategory);
    }

    /**
     * Test steps s1,s2
     */
    private void testAddCategory(PIMList[] aLists, String[][] aOldCategories,
                                 String aCategory)
    throws TestEndThrowable
    {
        int listCount = aLists.length;
        for (int i = 0; i < listCount; i++)
        {
            testAddCategory(aLists[ i ], aOldCategories[ i ], aCategory);
        } // end of for (int i = 0; i < listCount; i++)
    }

    /**
     * Test steps s3-s5
     */
    private void testMaxC(PIMList[] aLists, int[] aMax)
    throws TestEndThrowable
    {
        int listCount = aLists.length;
        for (int i = 0; i < listCount; i++)
        {
            testMaxC(aLists[ i ], aMax[ i ]);
        } // end of for (int i = 0; i < listCount; i++)
    }

    private void testMaxC(PIMList aList, int aMax)
    throws TestEndThrowable
    {

        int categoryCount = 0;
        try
        {
            String[] categories = aList.getCategories();

            categoryCount = categories.length;
        }
        catch (Throwable e)
        {
            failTest("getCategories threw: " + e.toString());
        } // end of try-catch
        int addCount = 0;
        if (aMax == -1)
        {
            addCount = 10;
        } // end of if (aMax == -1)
        else
        {
            addCount = aMax - categoryCount;
        } // end of else

        String prefix = iTestCategoryName;

        String[] addedCategories = new String[ addCount ];
        for (int i = 0; i < addCount; i++)
        {
            String[] categories = null;
            try
            {
                categories = aList.getCategories();

            }
            catch (Throwable e)
            {
                failTest("getCategories threw: " + e.toString());
            } // end of try-catch
            String newCategoryName = makeNewCategory(prefix, categories);
            try
            {
                aList.addCategory(newCategoryName);
            }
            catch (Throwable e)
            {
                failTest("addCategory threw: " + e.toString());
            } // end of try-catch
            addedCategories[ i ] = newCategoryName;
        } // end of for (int i = 0; i < addCount; i++)

        // step s4
        if (aMax != -1)
        {
            String[] categories = null;
            try
            {
                categories = aList.getCategories();
            }
            catch (Throwable e)
            {
                failTest("getCategories threw: " + e.toString());
            }
            String newCategoryName = makeNewCategory(prefix, categories);
            testPIME(aList, newCategoryName);
        } // end of if (aMax != -1)

        // step s5
        for (int i = 0; i < addCount; i++)
        {
            try
            {
                aList.deleteCategory(addedCategories[ i ], false);
            }
            catch (Throwable e)
            {
                failTest("deleteCategory threw: " + e.toString());
            } // end of try-catch
        } // end of for (int i = 0; i < addCount; i++)

    }

    /**
     * test step s4
     */
    private void testPIME(PIMList aList, String aCategory)
    throws TestEndThrowable
    {
        boolean exceptionThrown = false;
        try
        {
            aList.addCategory(aCategory);
        }
        catch (PIMException e)
        {
            exceptionThrown = true;
        } // end of try-catch
        if (!exceptionThrown)
        {
            failTest("PIMException not thrown");
        } // end of if (!exceptionThrown)
    }

    /**
     * test step s6-s8
     */
    private void testExceptions(PIMList[] aLists)
    throws TestEndThrowable
    {
        int listCount = aLists.length;
        for (int i = 0; i < listCount; i++)
        {
            testInvalid(aLists[ i ]);
            testNull(aLists[ i ]);
            testClose(aLists[ i ]);
        } // end of for (int i = 0; i < listCount; i++)
    }

    private void testInvalid(PIMList aList)
    throws TestEndThrowable
    {
        tryInvalid(aList, "");
        tryInvalid(aList, "AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA");
        tryInvalid(aList, "\b\t\n\f\r");
    }

    private void tryInvalid(PIMList aList, String aCategory)
    throws TestEndThrowable
    {
        boolean exceptionThrown = false;
        try
        {
            aList.addCategory(aCategory);
        }
        catch (PIMException e)
        {
            exceptionThrown = true;
        } // end of try-catch
        catch (Throwable e)
        {
            failTest("addCategory threw: " + e.toString());
        } // end of catch
        if (!exceptionThrown)
        {
            try
            {
                aList.deleteCategory(aCategory, false);
            }
            catch (Throwable e)
            {
                failTest("deleteCategory threw: " + e.toString());
            } // end of try-catch
        } // end of if (!exceptionThrown)
    }

    /**
     * Test step s7
     */
    private void testNull(PIMList aList)
    throws TestEndThrowable
    {
        boolean exceptionThrown = false;
        try
        {
            aList.addCategory(null);
        }
        catch (NullPointerException e)
        {
            exceptionThrown = true;
        } // end of try-catch
        catch (Throwable e)
        {
            failTest("Not NPE: " + e.toString());
        } // end of catch
        if (!exceptionThrown)
        {
            failTest("NullPointer not thrown");
        } // end of if (!exceptionThrown)
    }

    /**
     * Test step s8
     */
    private void testClose(PIMList aList)
    throws TestEndThrowable
    {
        try
        {
            aList.close();
        }
        catch (Throwable e)
        {
            failTest("close threw: " + e.toString());
        } // end of try-catch
        boolean exceptionThrown = false;
        try
        {
            aList.addCategory("test");
        }
        catch (PIMException e)
        {
            exceptionThrown = true;
        } // end of try-catch
        catch (Throwable e)
        {
            failTest("not PIME: " + e.toString());
        } // end of catch
        if (!exceptionThrown)
        {
            failTest("PIMException not thrown for closed list");
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

    private String[][] getCategories(PIMList[] aLists)
    throws PIMException
    {
        int listCount = aLists.length;
        String[][] categories = new String[ listCount ][];
        for (int i = 0; i < listCount; i++)
        {
            categories[ i ] = aLists[ i ].getCategories();
        } // end of for (int = 0; i < listCount; i++)
        return categories;
    }

    private int[] getMax(PIMList[] aLists)
    {
        int listCount = aLists.length;
        int[] maxC = new int[ listCount ];
        for (int i = 0; i < listCount; i++)
        {
            maxC[ i ] = aLists[ i ].maxCategories();
        } // end of for (int = 0; i < listCount; i++)
        return maxC;
    }

}
