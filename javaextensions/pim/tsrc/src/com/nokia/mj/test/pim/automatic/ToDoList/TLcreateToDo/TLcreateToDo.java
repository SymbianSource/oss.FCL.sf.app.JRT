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
package com.nokia.mj.test.pim.automatic.ToDoList.TLcreateToDo;
//IMPORTS
import java.io.IOException;
import java.util.*;
import javax.microedition.pim.PIM;
import javax.microedition.pim.PIMException;
import javax.microedition.pim.PIMList;
import javax.microedition.pim.ToDo;
import javax.microedition.pim.ToDoList;
import com.nokia.mj.test.pim.adaptation.*;

// CLASS DEFINITION
/**
 * This class tests javax.microedition.pim.ToDoList.create()
 */
public class TLcreateToDo
        extends MIDPTest
{

    public TLcreateToDo(
        String sTestName, j2meunit.framework.TestMethod rTestMethod)
    {
        super(sTestName, rTestMethod);
    }


    public void runTest() throws TestEndThrowable
    {
        // preconditions
        PIM p = null;
        String[] todoListNames = null;
        ToDoList tl = null;
        int count = 0;

        // open first todo list
        try
        {
            p = PIM.getInstance();
            todoListNames = p.listPIMLists(PIM.TODO_LIST);
            tl = (ToDoList)p.openPIMList(PIM.TODO_LIST, PIM.READ_WRITE, todoListNames[0]);

            Enumeration e = tl.items();
            while (e.hasMoreElements())
            {
                e.nextElement();
                count++;
            }
        }
        catch (Throwable e)
        {
            failTest("preconditions failed: " + e.toString());
        } // end of try-catch

        log("ToDoList has "+ count + " items.");

        log("Test that creating new ToDo item does not increase item count in ToDoList.");

        ToDo t = null;

        try
        {
            t = tl.createToDo();

            Enumeration e = tl.items();
            tl.close();
            int tmp = 0;
            while (e.hasMoreElements())
            {
                e.nextElement();
                tmp++;
            }
            if (tmp != count)
            {
                failTest("Creating new ToDo changed ToDoList item count!");
            }

        }
        catch (Throwable e)
        {
            failTest("Failed to test ToDo item creation: " + e.toString());
        } // end of try-catch

        log("Test that created ToDo tiem is empty.");
        int[] fields = t.getFields();
        if (fields.length != 0)
        {
            failTest("Created new ToDo item has not got all fields empty!");
        }

        passTest();
    }

}
