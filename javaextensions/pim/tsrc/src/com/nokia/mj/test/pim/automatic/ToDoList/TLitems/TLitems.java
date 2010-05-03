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
package com.nokia.mj.test.pim.automatic.ToDoList.TLitems;
//IMPORTS
import java.io.IOException;
import java.util.*;
import javax.microedition.pim.*;
import com.nokia.mj.test.pim.adaptation.*;

// CLASS DEFINITION
/**
 * This class tests javax.microedition.pim.ToDoList.items()
 */
public class TLitems extends MIDPTest
{

    public TLitems(
        String sTestName, j2meunit.framework.TestMethod rTestMethod)
    {
        super(sTestName, rTestMethod);
    }

    /**
     * Set of dates for item ranges in ascending order.
     */
    private final String[] RANGE_DATE_STRINGS =
        { "050101:080000",
          "050107:080000",
          "050110:080000",
          "050113:080000",
          "050127:080000"
        };

    private long[] iRangeDates = null;

    ToDoList iToDoList = null;

    private int iDueItems0to4 = 0;
    private int iDueItems1to2 = 0;
    private int iDueItems2to3 = 0;

    private int iCompletedItems0to4 = 0;
    private int iCompletedItems1to2 = 0;
    private int iCompletedItems2to3 = 0;

    public TLitems()
    {
    }

    public void initTest() throws TestEndThrowable
    {
        // preconditions
        iRangeDates = TimeTool.getTimeInMillis(RANGE_DATE_STRINGS);
        // open first todo list
        try
        {
            PIM p = PIM.getInstance();
            String[] todoListNames = p.listPIMLists(PIM.TODO_LIST);
            String toDoListName = todoListNames[ 0 ];
            iToDoList = (ToDoList)p.openPIMList(PIM.TODO_LIST,
                                                PIM.READ_WRITE, toDoListName);

            iDueItems0to4 = countItems(iToDoList, ToDo.DUE,
                                       iRangeDates[ 0 ],
                                       iRangeDates[ 4 ]);

            iDueItems1to2 = countItems(iToDoList, ToDo.DUE,
                                       iRangeDates[ 1 ],
                                       iRangeDates[ 2 ]);

            iDueItems2to3 = countItems(iToDoList, ToDo.DUE,
                                       iRangeDates[ 2 ],
                                       iRangeDates[ 3 ]);

            iCompletedItems0to4 = countItems(iToDoList, ToDo.COMPLETION_DATE,
                                             iRangeDates[ 0 ],
                                             iRangeDates[ 4 ]);

            iCompletedItems1to2 = countItems(iToDoList, ToDo.COMPLETION_DATE,
                                             iRangeDates[ 1 ],
                                             iRangeDates[ 2 ]);

            iCompletedItems2to3 = countItems(iToDoList, ToDo.COMPLETION_DATE,
                                             iRangeDates[ 2 ],
                                             iRangeDates[ 3 ]);

            log(iDueItems0to4 + " DUE items in range "
                + iRangeDates[ 0 ] + "-" + iRangeDates[ 4 ]
                + " before adding.");

            log(iCompletedItems0to4 + " COMPLETION_DATE items in range "
                + iRangeDates[ 0 ] + "-" + iRangeDates[ 4 ]
                + " before adding.");

            addItems(iToDoList);
        }
        catch (Throwable e)
        {
            failTest("preconditions failed: " + e);
        }
    }

    public void runTest() throws TestEndThrowable
    {
        initTest();
        int count = 0;

        if (iToDoList.isSupportedField(ToDo.DUE))
        {
            count = countItems(iToDoList, ToDo.DUE,
                               iRangeDates[0], iRangeDates[4]);

            if (count != (iDueItems0to4 + 4))
            {
                log(count+" DUE items in range "
                    +iRangeDates[0]+"-"+iRangeDates[4]);

                failTest("#1: ToDoList.items() did not return "
                         +(iDueItems0to4+4) + " items");
            }

            count = countItems(iToDoList, ToDo.DUE,
                               iRangeDates[1], iRangeDates[2]);

            if (count != (iDueItems1to2 + 1))
            {
                log(count+" DUE items in range "
                    +iRangeDates[1]+"-"+iRangeDates[2]);

                failTest("#2: ToDoList.items() did not return "
                         +(iDueItems0to4+1) + " items");
            }

            count = countItems(iToDoList, ToDo.DUE,
                               iRangeDates[2], iRangeDates[3]);

            if (count != iDueItems2to3)
            {
                log(count+" DUE items in range "
                    +iRangeDates[2]+"-"+iRangeDates[3]);

                failTest("#3: ToDoList.items() did not return "
                         + iDueItems0to4 + " items");
            }
        }

        if (iToDoList.isSupportedField(ToDo.COMPLETION_DATE))
        {
            count = countItems(iToDoList, ToDo.COMPLETION_DATE,
                               iRangeDates[0], iRangeDates[4]);

            if (count != (iCompletedItems0to4 + 4))
            {
                log(count+" COMPLETION_DATE items in range "
                    +iRangeDates[0]+"-"+iRangeDates[4]);

                failTest("#4: ToDoList.items() did not return "
                         +(iCompletedItems0to4+4) + " items");
            }

            count = countItems(iToDoList, ToDo.COMPLETION_DATE,
                               iRangeDates[1], iRangeDates[2]);

            if (count !=  iCompletedItems1to2)
            {
                log(count+" COMPLETION_DATE items in range "
                    +iRangeDates[1]+"-"+iRangeDates[2]);

                failTest("#5: ToDoList.items() did not return "
                         +iCompletedItems0to4 + " items");
            }

            count = countItems(iToDoList, ToDo.COMPLETION_DATE,
                               iRangeDates[2], iRangeDates[3]);

            if (count != (iCompletedItems2to3 + 1))
            {
                log(count+" COMPLETION_DATE items in range "
                    +iRangeDates[2]+"-"+iRangeDates[3]);

                failTest("#6: ToDoList.items() did not return "
                         +(iCompletedItems0to4+1) + " items");
            }
        }
        passTest();
        cleanupTest();
    }

    public void cleanupTest()
    {
        try
        {
            // remove "TestToDo" items
            Enumeration e = iToDoList.items();
            while (e.hasMoreElements())
            {
                ToDo t = (ToDo)e.nextElement();
                if (t.getString(ToDo.SUMMARY, 0).equals("TestToDo"))
                {
                    iToDoList.removeToDo(t);
                }
            }
            // close list
            iToDoList.close();
        }
        catch (Throwable t)
        {
            log("error while cleaning up test: " +t);
        }
    }

    private void addItems(ToDoList aToDoList) throws TestEndThrowable
    {
        try
        {
            String[] stringDates =
                { "050122:080000", "050126:080000", "050115:080000",
                  "050118:080000", "050108:080000", "050112:080000",
                  "050102:080000", "050104:080000"
                };
            long[] dates = TimeTool.getTimeInMillis(stringDates);

            for (int i=0; i < dates.length; i++)
            {
                ToDo t = aToDoList.createToDo();
                t.addString(ToDo.SUMMARY, PIMItem.ATTR_NONE, "TestToDo");
                if (i%2 == 0)
                {
                    if (aToDoList.isSupportedField(ToDo.DUE))
                    {
                        t.addDate(ToDo.DUE, PIMItem.ATTR_NONE, dates[i]);
                    }
                }
                else
                {
                    if (aToDoList.isSupportedField(ToDo.COMPLETION_DATE))
                    {
                        t.addDate(ToDo.COMPLETION_DATE, PIMItem.ATTR_NONE, dates[i]);
                        t.addBoolean(ToDo.COMPLETED, PIMItem.ATTR_NONE, true);
                    }
                }
                t.commit();
            }
        }
        catch (Exception e)
        {
            failTest("Error while adding TestToDos: " + e);
        }
    }

    private int countItems(ToDoList aToDoList, int aType, long aBegin, long aEnd)
    throws TestEndThrowable
    {
        int count = 0;
        long prevDate = 0;

        if (!aToDoList.isSupportedField(aType))
        {
            return 0;
        }

        try
        {
            Enumeration e = aToDoList.items(aType, aBegin, aEnd);
            while (e.hasMoreElements())
            {
                ToDo t = (ToDo) e.nextElement();
                int[] fields = t.getFields();
                long tmp = 0;

                tmp = t.getDate(aType, 0);

                if (tmp < prevDate)
                {
                    log("previous date: " +prevDate+", current date: "+ tmp);
                    failTest("Dates returned by items() are not in ascending order!");
                }
                prevDate = tmp;
                count++;
            }
        }
        catch (Throwable t)
        {
            failTest("Error while counting ToDos: " + t);
        }
        return count;
    }

    private boolean contains(int[] array, int value)
    {
        for (int i=0; i < array.length; i++)
        {
            if (array[i] == value)
            {
                return true;
            }
        }
        return false;
    }
}
