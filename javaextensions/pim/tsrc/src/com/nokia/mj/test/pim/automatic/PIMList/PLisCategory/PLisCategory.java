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
package com.nokia.mj.test.pim.automatic.PIMList.PLisCategory;
//IMPORTS
import java.io.IOException;
import java.util.*;
import javax.microedition.pim.PIM;
import javax.microedition.pim.PIMException;
import javax.microedition.pim.PIMList;
import com.nokia.mj.test.pim.adaptation.*;

// CLASS DEFINITION
/**
 * This class tests javax.microedition.pim.PIMList.close()
 */
public class PLisCategory extends MIDPTest
{

    public PLisCategory(
        String sTestName, j2meunit.framework.TestMethod rTestMethod)
    {
        super(sTestName, rTestMethod);
    }

    private String iTestCategoryName = "PLisCategory";
    private String iTestCategoryNewName = "PLisCategoryNew";

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
        }
        catch (Throwable e)
        {
            failTest("preconditions failed: " + e.toString());
        } // end of try-catch

        // Test steps
        // step s1
        testIsCategory(cl, cc);
        testIsCategory(el, ec);
        testIsCategory(tl, tc);

        // step s2
        testRenameCategory(cl, cc);
        testRenameCategory(el, ec);
        testRenameCategory(tl, tc);

        // steps s3, s4
        testAddCategory(cl);
        testAddCategory(el);
        testAddCategory(tl);

        // step s5
        testNull(cl);
        testNull(el);
        testNull(tl);

        // step s6
        testPIME(cl);
        testPIME(el);
        testPIME(tl);

        passTest();
    }

    private void testIsCategory(PIMList aList, String aCategory,
                                boolean aExpected)
    throws TestEndThrowable
    {
        boolean isCategory = false;
        try
        {
            isCategory = aList.isCategory(aCategory);
        }
        catch (Throwable e)
        {
            failTest("isCategory threw: " + e.toString());
        } // end of try-catch

        if (isCategory != aExpected)
        {
            if (aExpected)
            {
                failTest("isCategory returned false for " + aCategory);
            } // end of if (aExpected)
            failTest("isCategory returned true for " + aCategory);
        } // end of if (isCategory != aExpected)
    }

    /**
     * Test step s1
     */
    private void testIsCategory(PIMList[] aLists, String[][] aCategories)
    throws TestEndThrowable
    {
        int listCount = aLists.length;
        for (int i = 0; i < listCount; i++)
        {
            testIsCategory(aLists[ i ], aCategories[ i ]);
        } // end of for (int i = 0; i < listCount; i++)
    }

    private void testIsCategory(PIMList aList, String[] aCategories)
    throws TestEndThrowable
    {
        int categoryCount = aCategories.length;
        for (int i = 0; i < categoryCount; i++)
        {
            testIsCategory(aList, aCategories[ i ], true);
        } // end of for (int = 0; i < categoryCount; i++)
    }

    /**
     * test step s2
     */
    private void testRenameCategory(PIMList[] aLists, String[][] aCategories)
    throws TestEndThrowable
    {
        int listCount = aLists.length;
        for (int i = 0; i < listCount; i++)
        {
            testRenameCategory(aLists[ i ], aCategories[ i ]);
        } // end of for (int i = 0; i < listCount; i++)
    }

    private void testRenameCategory(PIMList aList, String[] aCategories)
    throws TestEndThrowable
    {
        if (aCategories.length == 0)
        {
            // we can't test
            String listName = aList.getName();
            log("list " + listName + " does not have any categories");
            return;
        } // end of if (aCategories.length == 0)

        int categoryIndex = 0;
        String oldCategoryName = aCategories[ categoryIndex ];
        while (iTestCategoryName.equals(oldCategoryName))
        {
            if (++categoryIndex == aCategories.length)
            {
                log("No suitable categories found from " + aList.getName());
                return;
            } // end of if (++categoryIndex == aCategories.length)
            oldCategoryName = aCategories[ categoryIndex ];
        } // end of while (newCategoryName.equals( oldCategoryName ))

        try
        {
            log("aList.renameCategory( \"" + oldCategoryName +
                "\", \"" + iTestCategoryName + "\" );");
            aList.renameCategory(oldCategoryName, iTestCategoryName);
        }
        catch (Throwable e)
        {
            failTest("renameCategory threw " + e.toString());
        } // end of try-catch

        testIsCategory(aList, oldCategoryName, false);
        testIsCategory(aList, iTestCategoryName, true);
    }

    /**
     * test steps s3 and s4
     */
    private void testAddCategory(PIMList[] aLists)
    throws TestEndThrowable
    {
        int listCount = aLists.length;
        for (int i = 0; i < listCount; i++)
        {
            testAddCategory(aLists[ i ]);
            testDeleteCategory(aLists[ i ]);
        } // end of for (int i = 0; i < listCount; i++)
    }

    /**
     * test step s3
     */
    private void testAddCategory(PIMList aList)
    throws TestEndThrowable
    {
        try
        {
            aList.addCategory(iTestCategoryNewName);
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
        } // end of catch
        testIsCategory(aList, iTestCategoryNewName, true);
    }

    /**
     * test step s4
     */
    private void testDeleteCategory(PIMList aList)
    throws TestEndThrowable
    {
        try
        {
            aList.deleteCategory(iTestCategoryNewName, false);
        }
        catch (PIMException e)
        {
            // categories may be unsupported, which is not an error
            int reason = e.getReason();
            if (reason == PIMException.FEATURE_NOT_SUPPORTED)
            {
                return;
            }
            failTest("deleteCategory threw PIME: " + e.getMessage());
        } // end of try-catch
        catch (Throwable e)
        {
            failTest("deleteCategory threw: " + e.toString());
        } // end of catch
        testIsCategory(aList, iTestCategoryNewName, false);
    }

    /**
     * test step s5
     */
    private void testNull(PIMList[] aLists)
    throws TestEndThrowable
    {
        int listCount = aLists.length;
        for (int i = 0; i < listCount; i++)
        {
            testNull(aLists[ i ]);
        } // end of for (int i = 0; i < listCount; i++)
    }

    private void testNull(PIMList aList)
    throws TestEndThrowable
    {
        boolean exceptionThrown = false;
        try
        {
            aList.isCategory(null);
        }
        catch (NullPointerException e)
        {
            exceptionThrown = true;
        } // end of try-catch
        catch (Throwable e)
        {
            failTest("not NPE: " + e.toString());
        } // end of catch

        if (!exceptionThrown)
        {
            failTest("NPE not thrown");
        } // end of if (!exceptionThrown)

    }

    /**
     * test step s6
     */
    private void testPIME(PIMList[] aLists)
    throws TestEndThrowable
    {
        int listCount = aLists.length;
        for (int i = 0; i < listCount; i++)
        {
            testPIME(aLists[ i ]);
        } // end of for (int i = 0; i < listCount; i++)
    }

    private void testPIME(PIMList aList)
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
            aList.isCategory("testCategory");
        }
        catch (PIMException e)
        {
            exceptionThrown = true;
        } // end of try-catch
        catch (Throwable e)
        {
            failTest("not PIMException: " + e.toString());
        } // end of catch

        if (!exceptionThrown)
        {
            failTest("PIMException not thrown");
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

}
