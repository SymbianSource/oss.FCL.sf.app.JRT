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
package com.nokia.mj.test.pim.automatic.PIMList.PLgetName;
//IMPORTS
import java.io.IOException;
import java.util.*;
import javax.microedition.pim.PIM;
import javax.microedition.pim.PIMException;
import javax.microedition.pim.PIMList;
import com.nokia.mj.test.pim.adaptation.*;

// CLASS DEFINITION
/**
 * This class tests javax.microedition.pim.PIMList.getName()
 */
public class PLgetName
        extends MIDPTest
{

    public PLgetName(
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
        try
        {
            p = PIM.getInstance();
            contactListNames = p.listPIMLists(PIM.CONTACT_LIST);
            eventListNames = p.listPIMLists(PIM.EVENT_LIST);
            todoListNames = p.listPIMLists(PIM.TODO_LIST);
            contactLists = openLists(p, PIM.CONTACT_LIST, contactListNames);
            eventLists = openLists(p, PIM.EVENT_LIST, eventListNames);
            todoLists = openLists(p, PIM.TODO_LIST, todoListNames);
        }
        catch (Throwable e)
        {
            failTest("preconditions failed: " + e.toString());
        } // end of try-catch

        // Test steps
        // step s1
        testListNames(contactListNames, contactLists);
        testListNames(eventListNames, eventLists);
        testListNames(todoListNames, todoLists);

        passTest();
    }

    /**
     * Test step s1
     */
    private void testListNames(String[] aListNames, PIMList[] aLists)
    throws TestEndThrowable
    {
        int listCount = aLists.length;
        for (int i = 0; i < listCount; i++)
        {
            String listName = null;
            try
            {
                listName = aLists[ i ].getName();
            }
            catch (Throwable e)
            {
                failTest("getName threw: " + e.toString());
            } // end of try-catch
            if (listName.compareTo(aListNames[ i ]) != 0)
            {
                failTest("list " + aListNames[ i ] + " returned " +
                         listName);
            } // end of if (listName.compareTo( aListNames[ i ] ) != 0)
        } // end of for (int i = 0; i < listCount; i++)
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


}
