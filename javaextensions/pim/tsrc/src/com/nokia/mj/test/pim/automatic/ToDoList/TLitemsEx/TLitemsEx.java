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
package com.nokia.mj.test.pim.automatic.ToDoList.TLitemsEx;
//IMPORTS
import java.io.IOException;
import java.util.*;
import javax.microedition.pim.*;
import com.nokia.mj.test.pim.adaptation.*;

// CLASS DEFINITION
/**
 * This class tests javax.microedition.pim.ToDoList.items()
 */
public class TLitemsEx extends MIDPTest
{

    public TLitemsEx(
        String sTestName, j2meunit.framework.TestMethod rTestMethod)
    {
        super(sTestName, rTestMethod);
    }

    ToDoList tl = null;

    public void initTest() throws TestEndThrowable
    {
        // preconditions
        PIM p = null;
        String[] todoListNames = null;

        // open first todo list
        try
        {
            p = PIM.getInstance();
            todoListNames = p.listPIMLists(PIM.TODO_LIST);
            tl = (ToDoList)p.openPIMList(PIM.TODO_LIST,
                                         PIM.READ_WRITE, todoListNames[0]);

        }
        catch (Throwable e)
        {
            failTest("preconditions failed: " + e);
        }
    }

    public void runTest() throws TestEndThrowable
    {
        initTest();
        String[] validDates = { "050122:080000", "050126:080000" };
        long[] dates = TimeTool.getTimeInMillis(validDates);

        // test bad types
        for (int i=-1; i < 300; i+=2)
        {
            if ((i != ToDo.DUE) && (i != ToDo.COMPLETION_DATE))
            {
                try
                {
                    Enumeration e = tl.items(i, dates[0], dates[1]);
                    failTest("Bad field did not throw IAE!");
                }
                catch (IllegalArgumentException iae)
                {
                    //ok
                }
                catch (UnsupportedFieldException iae)
                {
                    log("Note! Field "+i+" threw UFE when calling "+
                        "TL.items(field, date1, date2).");
                }
                catch (Exception ex)
                {
                    failTest("Bad field did not throw IAE, but " +ex);
                }
            }
        }
        // test valid dates in wrong order
        try
        {
            Enumeration e = tl.items(ToDo.DUE, dates[1], dates[0]);
            failTest("Dates in wrong order did not throw IAE!");
        }
        catch (IllegalArgumentException iae)
        {
            //ok
        }
        catch (Exception ex)
        {
            failTest("Dates in wrong order did not throw IAE, but " +ex);
        }

        // test with bad dates
        long[] badDates = { Long.MIN_VALUE, Long.MIN_VALUE,
                            Long.MIN_VALUE, Long.MIN_VALUE+1L,
                            0L, 0L,
                            Long.MAX_VALUE-1L, Long.MAX_VALUE,
                            Long.MAX_VALUE, Long.MAX_VALUE
                          };

        for (int i=0; i < badDates.length; i+=2)
        {
            try
            {
                Enumeration e = tl.items(ToDo.DUE, badDates[i], badDates[i+1]);
                log("TL.items(DUE, "+badDates[i]+", "+badDates[i+1]+") did not throw any exception");
            }
            catch (IllegalArgumentException iae)
            {
                log("TL.items(DUE, "+badDates[i]+", "+badDates[i+1]+") threw IAE");
            }
            catch (PIMException pime)
            {
                log("TL.items(DUE, "+badDates[i]+", "+badDates[i+1]+") threw PIME");
            }
            catch (Exception ex)
            {
                failTest("TL.items(DUE, "+badDates[i]+", "+badDates[i+1]+") threw "+ex);
            }
        }

        // test with WRITE_ONLY list
        try
        {
            tl.close();

            PIM p = PIM.getInstance();
            String[] todoListNames = p.listPIMLists(PIM.TODO_LIST);
            tl = (ToDoList)p.openPIMList(PIM.TODO_LIST,
                                         PIM.WRITE_ONLY, todoListNames[0]);

        }
        catch (Throwable e)
        {
            failTest("error while testing with WRITE_ONLY list: " + e);
        }
        try
        {
            Enumeration e = tl.items(ToDo.DUE, dates[1], dates[0]);
            failTest("TL.items() with WRITE_ONLY list did not throw SecurityException!");
        }
        catch (SecurityException se)
        {
            //ok
        }
        catch (Exception ex)
        {
            failTest("TL.items() with WRITE_ONLY list did not throw SecurityException, but " +ex);
        }

        passTest();
        cleanupTest();
    }

    public void cleanupTest()
    {
        try
        {
            // close list
            tl.close();
        }
        catch (Throwable t)
        {
            log("error while cleaning up test: " +t);
        }
    }
}
