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
package com.nokia.mj.test.pim.automatic.ToDo.Tcommit;
//IMPORTS
import java.io.IOException;
import java.util.*;
import javax.microedition.pim.*;
import com.nokia.mj.test.pim.adaptation.*;

// CLASS DEFINITION
/**
 * Tests that ToDo.commit() persists values correctly
 */
public class Tcommit extends MIDPTest
{
    private String[] iToDoLists;
    private String iItemSubject;
    private Date iDueDate;
    private Date iCompletionDate;
    private int iAlarm = 1800;
    private int iDefaultAlarm = -43200;

    public Tcommit(String sTestName,
                   j2meunit.framework.TestMethod rTestMethod)
    {
        super(sTestName, rTestMethod);
    }

    public void runTest() throws TestEndThrowable
    {
        // Init test data
        init();
        // Do not close lists between commit
        doTest(false);
        // Close lists between commit
        doTest(true);
    }

    public void doTest(boolean aCloseList) throws TestEndThrowable
    {
        try
        {
            debug("Tcommit::doTest - testing Default ToDo list");
            ToDoList todos = openList(iToDoLists[ 0 ]);

            ToDo todo = createAndCommitItem(todos);

            if (aCloseList)
            {
                todos.close();
                todos = openList(iToDoLists[ 0 ]);
                todo  = getSpecificToDoItem(todos, iItemSubject);
            }

            // Check commited appointment item that data was scaled correctly
            testField(todos, todo, ToDo.SUMMARY, iItemSubject);
            testField(todos, todo, ToDo.DUE, getStartOfDay(iDueDate));
            testField(todos, todo, ToDo.COMPLETION_DATE, iCompletionDate);
            testField(todos, todo, ToDo.COMPLETED, new Boolean(true));
            assertString(todo.countValues(0x1005001) == 0, "Alarm should not be present in the item");

            // Remove completed and completion date since alarm is not supported when these fields are present
            todo.removeValue(ToDo.COMPLETION_DATE, 0);
            todo.removeValue(ToDo.COMPLETED, 0);
            todo.setDate(ToDo.DUE, 0, PIMItem.ATTR_NONE, iDueDate.getTime());
            todo.addInt(0x1005001, PIMItem.ATTR_NONE, iAlarm);

            todo.commit();
            if (aCloseList)
            {
                todos.close();
                todos = openList(iToDoLists[ 0 ]);
                todo  = getSpecificToDoItem(todos, iItemSubject);
            }

            // Date has been converted to start of day
            Date alarmDate = new Date((iDueDate.getTime() - iAlarm*1000));
            int alarmInt = getDifferenceInSeconds(getStartOfDay(iDueDate), alarmDate);
            testField(todos, todo, 0x1005001, new Integer(alarmInt));
            // Change alarm to future
            todo.setInt(0x1005001, 0, PIMItem.ATTR_NONE, -90000);
            todo.commit();
            if (aCloseList)
            {
                todos.close();
                todos = openList(iToDoLists[ 0 ]);
                todo  = getSpecificToDoItem(todos, iItemSubject);
            }

            // Alarm occurs at 12:00 o'clock. Due date is converted to 00:00
            alarmInt = getDifferenceInSeconds(iDueDate, alarmDate);
            testField(todos, todo, ToDo.DUE, getStartOfDay(iDueDate));
            testField(todos, todo, 0x1005001, new Integer(iDefaultAlarm));
            todos.removeToDo(todo);
            todos.close();
        }
        catch (Throwable e)
        {
            failTest("Tcommit::doTest - test failed: " + e.toString());
        }

        passTest();
    }

    private ToDoList openList(String aListName)
    {
        try
        {
            PIM pim = PIM.getInstance();
            return (ToDoList)pim.openPIMList(PIM.TODO_LIST,
                                             PIM.READ_WRITE,
                                             aListName);
        }
        catch (Throwable e)
        {
            failTest("Failed to open PIM list");
        }
        return null;
    }

    private ToDo getSpecificToDoItem(ToDoList aList, String aSubject) throws Exception
    {
        ToDo matchingItem = aList.createToDo();
        matchingItem.addString(ToDo.SUMMARY, PIMItem.ATTR_NONE, aSubject);
        Enumeration items = aList.items(matchingItem);
        // There should be at least one element
        return (ToDo)items.nextElement();
    }

    private ToDo createAndCommitItem(ToDoList aList)
    {
        ToDo todo = aList.createToDo();
        // Initialize data
        if (aList.isSupportedField(ToDo.SUMMARY))
        {
            todo.addString(ToDo.SUMMARY, PIMItem.ATTR_NONE, iItemSubject);
        }
        if (aList.isSupportedField(ToDo.DUE))
        {
            todo.addDate(ToDo.DUE, PIMItem.ATTR_NONE, iDueDate.getTime());
        }
        if (aList.isSupportedField(ToDo.COMPLETION_DATE))
        {
            todo.addDate(ToDo.COMPLETION_DATE, PIMItem.ATTR_NONE, iCompletionDate.getTime());
        }
        // Extended alarm
        if (aList.isSupportedField(0x1005001))
        {
            todo.addInt(0x1005001, PIMItem.ATTR_NONE, iAlarm);
        }
        try
        {
            todo.commit();
        }
        catch (Exception e)
        {
            failTest("Tcommit::createAndCommitItem() - Exception when creating item");
        }
        return todo;
    }

    private void init()
    {
        // Clean lists
        try
        {
            PIM pim = PIM.getInstance();
            iToDoLists = pim.listPIMLists(PIM.TODO_LIST);

            for (int i = 0; i < iToDoLists.length; i++)
            {
                ToDoList list = openList(iToDoLists[ i ]);
                Enumeration items = list.items();
                while (items.hasMoreElements())
                {
                    ToDo todo = (ToDo)items.nextElement();
                    list.removeToDo(todo);
                }
            }
        }
        catch (Exception e)
        {
            failTest("Tcommit::init() - Exception when cleaning lists");
        }

        iDueDate = new Date(System.currentTimeMillis());
        // End date to next day
        iCompletionDate = new Date(iDueDate.getTime() - 3600000);
        iItemSubject = "TestToDoItem " + iDueDate.toString();
    }

    private Date getStartOfDay(Date aDate)
    {
        Calendar cal = Calendar.getInstance();

        cal.setTime(aDate);
        cal.set(Calendar.HOUR_OF_DAY, 0);
        cal.set(Calendar.MINUTE, 0);
        cal.set(Calendar.SECOND, 0);
        cal.set(Calendar.MILLISECOND, 0);

        return cal.getTime();
    }

    private int getDifferenceInSeconds(Date aFirstDate, Date aSecondDate)
    {
        double diff = (aFirstDate.getTime() - aSecondDate.getTime());
        int diffSecs = (((int)diff) / 1000);
        // Round full seconds
        diffSecs /= 60;
        return diffSecs*60;
    }

    private void testField(ToDoList aList, ToDo aItem, int aField, Object aExpectedResult)
    throws TestEndThrowable
    {
        boolean result = false;
        int type = aList.getFieldDataType(aField);
        debug("Tcommit::testField - testing field: " + aField + " + with value: " + aExpectedResult);
        switch (type)
        {
        case PIMItem.STRING:
        {
            String value = ((String)aExpectedResult);
            result = value.equals(aItem.getString(aField, 0));
            debug("Tcommit::testField - returned value: " + aItem.getString(aField, 0));
            break;
        }
        case PIMItem.INT:
        {
            Integer value = ((Integer)aExpectedResult);
            result = value.intValue() == aItem.getInt(aField, 0);
            debug("Tcommit::testField - returned value: " + aItem.getInt(aField, 0));
            debug("ATTENTION!!! Note that Java may have problems handling daylight saving times. Check that when this case fails");
            break;
        }
        case PIMItem.DATE:
        {
            Date value = ((Date)aExpectedResult);
            result = value.getTime() == aItem.getDate(aField, 0);
            debug("Tcommit::testField - returned value: " + new Date(aItem.getDate(aField, 0)).toString());
            debug("ATTENTION!!! Note that Java may have problems handling daylight saving times. Check that when this case fails");
            break;
        }
        case PIMItem.BOOLEAN:
        {
            Boolean value = ((Boolean)aExpectedResult);
            result = value.booleanValue() == aItem.getBoolean(aField, 0);
            debug("Tcommit::testField - returned value: " + aItem.getBoolean(aField, 0));
            break;
        }
        case PIMItem.STRING_ARRAY:
        default:
            assertString(false, "Tcommit::testField - test does not support field type: " + type);
            break;
        }
        assertString(result, "Tcommit::testField - incorrect value returned from field: " + aField);
    }
}
