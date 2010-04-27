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
package com.nokia.mj.test.pim.automatic.ToDoList.TLimportToDo;
//IMPORTS
import java.io.IOException;
import java.io.InputStream;
import java.io.UnsupportedEncodingException;
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
 * This class tests javax.microedition.pim.ToDoList.importToDo()
 *
 */
public class TLimportToDo
        extends MIDPTest
{

    public TLimportToDo(
        String sTestName, j2meunit.framework.TestMethod rTestMethod)
    {
        super(sTestName, rTestMethod);
    }

    private ToDoList tl = null;

    public void runTest() throws TestEndThrowable
    {
        // preconditions
        PIM p = null;
        String[] todoListNames = null;
        ToDo t = null;
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

        // read ToDo from serial format
        InputStream is = null;
        try
        {
            is = getClass().getResourceAsStream("/res/todoPriority1.vcs");
            PIMItem[] items = p.fromSerialFormat(is, "UTF-8");
            t = (ToDo) items[0];
        }
        catch (Exception e)
        {
            e.printStackTrace();
            failTest("Failed to read a ToDo from file: " + e);
        }

        log("ToDoList has "+ count + " items.");

        log("Test that importing new ToDo item does not increase item count in ToDoList.");
        ToDo t2 = null;
        try
        {
            t2 = tl.importToDo(t);

            if (countItems(tl) != count)
            {
                failTest("Importing new ToDo changed ToDoList item count!");
            }
            // test taht imported item is ok.
            int pri = t2.getInt(ToDo.PRIORITY, 0);
            if (pri != 1)
            {
                failTest("ToDo item import failed to set priority to 1.");
            }
        }
        catch (Throwable e)
        {
            failTest("Failed to test ToDo item import: " + e);
        } // end of try-catch

        log("Test that commiting the imported ToDo item increases item count in ToDoList.");
        try
        {
            t2.commit();
            if (countItems(tl) != (count + 1))
            {
                failTest("Commiting imported ToDo did not increase ToDoList item count by one!");
            }
        }
        catch (Exception e)
        {
            failTest("Failed to commit imported ToDo to list: " + e);
        }

        log("Test that importing new ToDo item again does not increase item count in ToDoList.");
        ToDo t3 = null;
        try
        {
            t3 = tl.importToDo(t);
            if (countItems(tl) != (count + 1))
            {
                failTest("Importing new ToDo changed ToDoList item count!");
            }
        }
        catch (Throwable e)
        {
            failTest("Failed to test ToDo item import: " + e);
        } // end of try-catch

        log("Test that commiting the second imported ToDo item increases item count in ToDoList.");
        try
        {
            t3.commit();
            if (countItems(tl) != (count + 2))
            {
                failTest("Commiting second imported ToDo did not increase ToDoList item count by one!");
            }
        }
        catch (Exception e)
        {
            failTest("Failed to commit second imported ToDo to list: " + e);
        }

        log("Test that removing both imported ToDo items decreases item count in ToDoList by two.");
        try
        {
            tl.removeToDo(t2);
            tl.removeToDo(t3);
            if (countItems(tl) != count)
            {
                failTest("Removing both imported ToDos did not decrease ToDoList item count by two!");
            }
            tl.close();
        }
        catch (Exception e)
        {
            failTest("Failed to remove imported ToDos from list: " + e);
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
