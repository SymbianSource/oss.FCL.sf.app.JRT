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
package com.nokia.mj.test.pim.automatic.EventList.ELitems;
//IMPORTS
import java.io.IOException;
import java.util.*;
import javax.microedition.pim.*;
import com.nokia.mj.test.pim.adaptation.*;
// CLASS DEFINITION
/**
 * This class tests javax.microedition.pim.EventList.items().
 */
public class ELitems
        extends MIDPTest
{

    public ELitems(
        String sTestName, j2meunit.framework.TestMethod rTestMethod)
    {
        super(sTestName, rTestMethod);
    }

    private PIM p = null;
    private String[] eventListNames = null;
    private EventList el = null;
    private long[] dates;

    public void initTest() throws TestEndThrowable
    {
        log("Initializing test.");
        System.out.println("initTest +");
        long hourInMillis = 60 * 60 * 1000;
        String[] stringDates = { "040427:080000", "040429:080000", "040501:080000",
                                 "040506:080000", "040106:080000", "041231:080000",
                                 "040430:080000", "040502:080000", "040430:180000",
                                 "040503:080000", "040428:080000", "040505:080000"
                               };
        dates = TimeTool.getTimeInMillis(stringDates);

        // setup test preconditions
        try
        {
            p = PIM.getInstance();
            eventListNames = p.listPIMLists(PIM.EVENT_LIST);

            // just get first eventlist
            el= (EventList) p.openPIMList(PIM.EVENT_LIST, PIM.READ_WRITE, eventListNames[0]);

            // count how many events between 27.4.2004-6.5.2004
            Enumeration e = el.items(EventList.OCCURRING, dates[0], dates[3], false);
            if (countItems(e) != 0)
            {
                failTest("preconditions failed. Eventlist " + eventListNames[0] +
                         " has event(s) during 27.4.2004-6.5.2004. Cannot execute tests.");
            }

            // create event e1 for 29.4.2004-1.5.2004
            Event e1 = el.createEvent();
            e1.addString(Event.SUMMARY, PIMItem.ATTR_NONE, "Test Event 1");
            e1.addDate(Event.START, PIMItem.ATTR_NONE, dates[1]);
            e1.addDate(Event.END, PIMItem.ATTR_NONE, dates[2]);
            log("Created Event with START: " + e1.getDate(Event.START, 0)
                + " and END: " + e1.getDate(Event.END, 0));

            // create event e2, which is repeated every tuesday during year 2004 (6.1.2004 is a tuesday)
            Event e2 = el.createEvent();
            e2.addString(Event.SUMMARY, PIMItem.ATTR_NONE, "Test Event 2");
            e2.addDate(Event.START, PIMItem.ATTR_NONE, dates[4]);
            e2.addDate(Event.END, PIMItem.ATTR_NONE, dates[4] + hourInMillis*6);
            RepeatRule rr = new RepeatRule();
            rr.setInt(RepeatRule.FREQUENCY, RepeatRule.WEEKLY);
            rr.setDate(RepeatRule.END, dates[5]);
            e2.setRepeat(rr);
            log("Created Event with START: " + e2.getDate(Event.START, 0)
                + " and END: " + e2.getDate(Event.END, 0)
                + " repeating weekly until: " + rr.getDate(RepeatRule.END));

            // commit events to the list
            e1.commit();
            e2.commit();
        }
        catch (Throwable e)
        {
            failTest("preconditions failed: " + e);
        } // end of try-catch
        log("Initialization OK.");
    }

    public void runTest() throws TestEndThrowable
    {
        initTest();
        System.out.println("runtest +");
        testItems(EventList.STARTING, dates[0], dates[3], true, 1);
        System.out.println("runtest ++");
        testItems(EventList.STARTING, dates[0], dates[3], false, 2);

        testItems(EventList.OCCURRING, dates[0], dates[3], false, 2);

        testItems(EventList.ENDING, dates[0], dates[3], false, 2);

        testItems(EventList.STARTING, dates[0], dates[6], true, 1);

        testItems(EventList.STARTING, dates[6], dates[7], true, 0);

        testItems(EventList.OCCURRING, dates[6], dates[8], true, 1);

        testItems(EventList.ENDING, dates[6], dates[9], true, 1);

        testItems(EventList.ENDING, dates[0], dates[10], true, 0);

        testItems(EventList.ENDING, dates[7], dates[11], true, 0);

        // all is well
        passTest();
        cleanupTest();
    }

    public void cleanupTest()
    {
        try
        {
            // remove "Test Event" items
            Enumeration e = el.items();
            while (e.hasMoreElements())
            {
                Event ev = (Event)e.nextElement();
                if (ev.getString(Event.SUMMARY, 0).startsWith("Test Event"))
                {
                    el.removeEvent(ev);
                }
            }
            // close list
            el.close();
        }
        catch (Throwable t)
        {
            log("error while cleaning up test: " +t);
        }
    }

    private void testItems(int searchType, long startDate, long endDate, boolean initialOnly, int okCount) throws TestEndThrowable
    {
        log("Test that el.items( "+searchType+", "+startDate+", "+endDate+", "+initialOnly+" ) returns "+okCount+" items");
        try
        {
            Enumeration e = el.items(searchType, startDate, endDate, initialOnly);

            if (countItems(e) != okCount)
            {
                failTest("EventList.items( "+searchType+", "+startDate+", "+endDate+", "+initialOnly+" ) failed!");
            }
        }
        catch (Exception e)
        {
            failTest("EventList.items( "+searchType+", "+startDate+", "+endDate+", "+initialOnly+" ) failed!" +e);
        }
    }

    private int countItems(Enumeration e)
    {
        int val = 0;

        while (e.hasMoreElements())
        {
            e.nextElement();
            val++;
        }
        return val;
    }
}