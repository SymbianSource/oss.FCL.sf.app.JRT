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
package com.nokia.mj.test.pim.automatic.EventList.ELitemsEx;
//IMPORTS
import java.io.IOException;
import java.util.*;
import javax.microedition.pim.*;
import com.nokia.mj.test.pim.adaptation.*;
// CLASS DEFINITION
/**
 * This class tests javax.microedition.pim.EventList.items().
 */
public class ELitemsEx
        extends MIDPTest
{

    public ELitemsEx(
        String sTestName, j2meunit.framework.TestMethod rTestMethod)
    {
        super(sTestName, rTestMethod);
    }

    private PIM p = null;
    private String[] eventListNames = null;
    private EventList el = null;

    public void runTest() throws TestEndThrowable
    {
        String[] stringDates = { "040101:080000", "041231:080000" };
        long[] dates = TimeTool.getTimeInMillis(stringDates);

        // setup test preconditions
        try
        {
            p = PIM.getInstance();
            eventListNames = p.listPIMLists(PIM.EVENT_LIST);

            // just get first eventlist
            el= (EventList) p.openPIMList(PIM.EVENT_LIST, PIM.READ_WRITE, eventListNames[0]);
        }
        catch (Throwable e)
        {
            failTest("preconditions failed: " + e);
        } // end of try-catch

        log("Test that calling el.items() using invalid searchtype causes IAE");
        int[] invalids = { -1, 3, 4 };
        for (int i = 0; i < invalids.length; i++)
        {
            try
            {
                el.items(invalids[i], dates[0], dates[1], true);
                failTest("Invalid searchtype did not cause IAE!");
            }
            catch (IllegalArgumentException iae)
            {
                // ok
            }
            catch (Exception e)
            {
                failTest("Invalid searchtype did not cause IAE, but: " +e);
            }
        }

        log("Test that calling el.items() using bigger startdate than enddate causes IAE");
        try
        {
            el.items(EventList.OCCURRING, dates[1], dates[0], true);
            failTest("bigger startdate than enddate did not cause IAE!");
        }
        catch (IllegalArgumentException iae)
        {
            // ok
        }
        catch (Exception e)
        {
            failTest("bigger startdate than enddate did not cause IAE, but: " +e);
        }

        log("Test that calling el.items() when el is closed causes PIME");
        try
        {
            el.close();
            el.items(EventList.OCCURRING, dates[0], dates[1], true);
            failTest("closed list did not cause PIME!");
        }
        catch (PIMException pime)
        {
            // ok
        }
        catch (Exception e)
        {
            failTest("closed list did not cause PIME, but: " +e);
        }

        log("Open eventlist in WRITE_ONLY mode and test that calling items() causes SecurityException.");
        try
        {
            // just get first eventlist
            el= (EventList) p.openPIMList(PIM.EVENT_LIST, PIM.WRITE_ONLY, eventListNames[0]);
            el.items(EventList.OCCURRING, dates[0], dates[1], true);
            failTest("calling items() using WRITE_ONLY list did not cause SecurityException!");
        }
        catch (SecurityException se)
        {
            // ok
        }
        catch (Exception e)
        {
            failTest("calling items() using WRITE_ONLY list did not cause SecurityException, but: " + e);
        } // end of try-catch

        // all is well
        passTest();
    }
}