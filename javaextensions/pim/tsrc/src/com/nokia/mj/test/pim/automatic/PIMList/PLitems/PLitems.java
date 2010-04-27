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
package com.nokia.mj.test.pim.automatic.PIMList.PLitems;
//IMPORTS
import java.io.IOException;
import java.util.*;
import javax.microedition.pim.PIM;
import javax.microedition.pim.PIMException;
import javax.microedition.pim.PIMList;
import com.nokia.mj.test.pim.adaptation.*;

// CLASS DEFINITION
/**
 * This class tests javax.microedition.pim.PIMList.items()
 */
public class PLitems
        extends MIDPTest
{

    public PLitems(
        String sTestName, j2meunit.framework.TestMethod rTestMethod)
    {
        super(sTestName, rTestMethod);
    }


    public void runTest() throws TestEndThrowable
    {
        // preconditions
        PIMList[] allLists = openAllLists(PIM.READ_WRITE);

        // Test steps
        // step s1
        testItems(allLists, true, false);

        // step s2
        closeLists(allLists);

        // step s3
        testItems(allLists, false, false);

        // step s4
        allLists = openAllLists(PIM.WRITE_ONLY);

        // step s5
        testItems(allLists, true, false);

        // step s6
        closeLists(allLists);

        passTest();
    }

    private void testItems(PIMList[] lists, boolean noPIMException, boolean noSecurityException)
    throws TestEndThrowable
    {
        for (int i = 0; i < lists.length; i++)
        {
            try
            {
                Enumeration e = lists[i].items();
                if (!noPIMException)
                {
                    failTest("Calling list.items() did not throw PIMException!");
                }
            }
            catch (PIMException pime)
            {
                if (noPIMException)
                {
                    failTest("PIMException thrown while calling list.items(): " + pime);
                }
            }
            catch (SecurityException se)
            {
                if (noSecurityException)
                {
                    failTest("SecurityException thrown while calling list.items(): " + se);
                }
            }
            catch (Exception ex)
            {
                failTest("Error while calling list.items(): " + ex);
            }
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
    throws PIMException
    {
        PIMList[] pimLists = new PIMList[ aNames.length ];
        for (int i = 0; i < aNames.length; i++)
        {
            pimLists[ i ] = aP.openPIMList(aListType, aMode, aNames[ i ]);
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
