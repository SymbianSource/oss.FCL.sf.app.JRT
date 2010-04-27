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
package com.nokia.mj.test.pim.automatic.PIMList.PLitems2Ex;
//IMPORTS
import java.io.IOException;
import java.util.*;
import javax.microedition.pim.*;
import com.nokia.mj.test.pim.adaptation.*;

// CLASS DEFINITION
/**
 * This class tests javax.microedition.pim.PIMList.items(String)
 */
public class PLitems2Ex extends MIDPTest
{

    public PLitems2Ex(
        String sTestName, j2meunit.framework.TestMethod rTestMethod)
    {
        super(sTestName, rTestMethod);
    }

    private Vector lists = null;

    public void initTest() throws TestEndThrowable
    {
        // preconditions
        openLists(PIM.READ_WRITE);
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
        // test steps 1
        testNullPointerException();

        // test steps 2 & 3
        closeLists();
        testPIMException();

        // test step 4 & 5
        openLists(PIM.WRITE_ONLY);
        testSecurityException();

        passTest(); // will also close lists (step 6)
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

    // test that lists throw NullPointerException
    // when calling PIMList.items(null)
    private void testNullPointerException() throws TestEndThrowable
    {
        for (int i=0; i< lists.size(); i++)
        {
            try
            {
                PIMList pl = (PIMList) lists.elementAt(i);
                String str = null;
                pl.items(str);
                failTest("Calling items(null) on a closed "+
                         "list did not throw NullPointerException!");
            }
            catch (NullPointerException npe)
            {
                // ok
            }
            catch (Exception ex)
            {
                failTest("Calling items(null) on a closed "+
                         "list did not throw NullPointerException but " + ex);
            }
        }
    }

    // test that closed lists throw PIMException
    // when calling PIMList.items("")
    private void testPIMException() throws TestEndThrowable
    {
        for (int i=0; i< lists.size(); i++)
        {
            try
            {
                PIMList pl = (PIMList) lists.elementAt(i);
                pl.items("");
                failTest("Calling items(\"\") on a closed "+
                         "list did not throw PIMException!");
            }
            catch (PIMException pe)
            {
                // ok
            }
            catch (Exception ex)
            {
                failTest("Calling items(\"\") on a closed "+
                         "list did not throw PIMException but " + ex);
            }
        }
    }

    // test that write_only lists throw SecurityException
    // when calling PIMList.items("")
    private void testSecurityException() throws TestEndThrowable
    {
        for (int i=0; i< lists.size(); i++)
        {
            try
            {
                PIMList pl = (PIMList) lists.elementAt(i);
                pl.items("");
                failTest("Calling items(\"\") on a write_only "+
                         "list did not throw SecurityException!");
            }
            catch (SecurityException se)
            {
                // ok
            }
            catch (Exception ex)
            {
                failTest("Calling items(\"\") on a write_only "+
                         "list did not throw SecurityException but " + ex);
            }
        }
    }

}
