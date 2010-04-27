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
package com.nokia.mj.test.pim.automatic.ToDoList.TLremoveToDo;
//IMPORTS
import java.io.IOException;
import java.util.*;
import javax.microedition.pim.PIM;
import javax.microedition.pim.PIMItem;
import javax.microedition.pim.PIMException;
import javax.microedition.pim.PIMList;
import javax.microedition.pim.ToDo;
import javax.microedition.pim.ToDoList;
import com.nokia.mj.test.pim.adaptation.*;

// CLASS DEFINITION
/**
 * This class tests javax.microedition.pim.ToDoList.removeToDo()
 *
 */
public class TLremoveToDo
        extends MIDPTest
{

    public TLremoveToDo(
        String sTestName, j2meunit.framework.TestMethod rTestMethod)
    {
        super(sTestName, rTestMethod);
    }

    // preconditions
    PIM p = null;
    String[] todoListNames = null;
    ToDoList tl = null;
    int count = 0;

    public void runTest() throws TestEndThrowable
    {

        // open first todo list
        try
        {
            p = PIM.getInstance();
            todoListNames = p.listPIMLists(PIM.TODO_LIST);
            tl = (ToDoList)p.openPIMList(PIM.TODO_LIST, PIM.READ_WRITE, todoListNames[0]);

            count = countItems(tl);
        }
        catch (Throwable e)
        {
            failTest("preconditions failed: " + e);
        } // end of try-catch

        log("ToDoList has "+ count + " items.");

        log("Test that creating new ToDo item and adding it to the list does increase item count in ToDoList.");

        ToDo t = null;

        try
        {
            t = tl.createToDo();
            if (tl.isSupportedField(ToDo.SUMMARY))
            {
                t.addString(ToDo.SUMMARY, PIMItem.ATTR_NONE, "Buy going away present for Judy");
            }
            else if (tl.isSupportedField(ToDo.DUE))
            {
                t.addDate(ToDo.DUE, PIMItem.ATTR_NONE, new Date().getTime());
            }
            else
            {
                failTest("Error while creating ToDo item!");
            }
            t.commit();

            if (countItems(tl) != count+1)
            {
                failTest("Creating new ToDo did not increase ToDoList item count by one!");
            }

        }
        catch (Throwable e)
        {
            failTest("Failed to test ToDo item creation: " + e);
        } // end of try-catch

        log("Test that removing ToDo item decreases ToDoList item count by one.");

        try
        {
            tl.removeToDo(t);
        }
        catch (Throwable e)
        {
            failTest("Failed to test ToDo item removal: " + e);
        } // end of try-catch

        if (countItems(tl) != count)
        {
            failTest("Removing ToDo did not decrease ToDoList item count by one!");
        }

        log("Test that removing ToDo item not in list does not change ToDoList item count.");

        try
        {
            tl.removeToDo(t);
            failTest("Removing ToDo not in list did not throw any exception!");
        }
        catch (PIMException pime)
        {
            // ok.
        }
        catch (Exception e)
        {
            failTest("Removing ToDo not in list threw wrong exception: "+e);
        }

        if (countItems(tl) != count)
        {
            failTest("Removing ToDo not in list changed ToDoList item count!");
        }

        log("Test that removing null ToDo item does not change ToDoList item count.");
        try
        {
            tl.removeToDo(null);
            failTest("Removing null ToDo did not throw any exception!");
        }
        catch (NullPointerException npe)
        {
            // ok.
        }
        catch (Exception e)
        {
            failTest("Removing null ToDo threw wrong exception: "+e);
        }

        if (countItems(tl) != count)
        {
            failTest("Removing null ToDo changed ToDoList item count!");
        }

        try
        {
            tl.close();
        }
        catch (Exception e)
        {
            failTest("Error while closing ToDoList: "+e);
        }

        passTest();
    }

    private int countItems(PIMList pl) throws TestEndThrowable
    {
        int tmp = 0;
        try
        {
            Enumeration e = pl.items();
            while (e.hasMoreElements())
            {
                e.nextElement();
                tmp++;
            }
        }
        catch (PIMException pime)
        {
            failTest("Error while getting items in list!");
        }

        return tmp;
    }
}
