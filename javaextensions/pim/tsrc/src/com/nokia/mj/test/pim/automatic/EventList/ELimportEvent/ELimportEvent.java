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
package com.nokia.mj.test.pim.automatic.EventList.ELimportEvent;
//IMPORTS
import java.io.IOException;
import java.io.InputStream;
import java.io.UnsupportedEncodingException;
import java.util.*;
import javax.microedition.pim.*;
import com.nokia.mj.test.pim.adaptation.*;

// CLASS DEFINITION
/**
 * This class tests javax.microedition.pim.ToDoList.importToDo()
 */
public class ELimportEvent
        extends MIDPTest
{

    public ELimportEvent(
        String sTestName, j2meunit.framework.TestMethod rTestMethod)
    {
        super(sTestName, rTestMethod);
    }

    private EventList el = null;
    private String[] stringDates = { "040101:000000", "040113:000000", "040201:000000"};
    private long[] dates = TimeTool.getTimeInMillis(stringDates);

    public void runTest() throws TestEndThrowable
    {
        // preconditions
        PIM p = null;
        String[] eventListNames = null;
        Event e1 = null;
        int count = 0;
        int countR = 0;

        // open first todo list
        try
        {
            p = PIM.getInstance();
            eventListNames = p.listPIMLists(PIM.EVENT_LIST);
            el = (EventList)p.openPIMList(PIM.EVENT_LIST, PIM.READ_WRITE, eventListNames[0]);

            Enumeration e = el.items(EventList.OCCURRING, dates[0], dates[1], true);
            while (e.hasMoreElements())
            {
                e.nextElement();
                count++;
            }
            Enumeration e2 = el.items(EventList.OCCURRING, dates[1], dates[2], false);
            while (e2.hasMoreElements())
            {
                e2.nextElement();
                countR++;
            }
        }
        catch (Throwable e)
        {
            failTest("preconditions failed: " + e.toString());
        } // end of try-catch

        // read Event from serial format
        InputStream is = null;
        try
        {
            is = getClass().getResourceAsStream("/res/appointment.vcs");
            PIMItem[] items = p.fromSerialFormat(is, "UTF-8");
            e1 = (Event) items[0];
        }
        catch (Exception e)
        {
            failTest("Failed to read an Event from file: " + e);
        }

        log("EventList has "+ count + " (initial only) items at range: "+dates[0]+"-"+dates[1]);
        log("EventList has "+ countR + " (repeating also) items at range: "+dates[1]+"-"+dates[2]);

        log("Test that importing new Event item does not increase item count in EventList.");
        Event e2 = null;
        try
        {
            e2 = el.importEvent(e1);

            if (countItems(el, dates[0], dates[2], true) != count)
            {
                failTest("Importing new Event changed EventList item count!");
            }
            // test that imported item is ok.
            String summary = e2.getString(Event.SUMMARY, 0);
            if (!summary.equals("timed appointment"))
            {
                failTest("Event item import failed to set summary to timed appointment, was: "+summary);
            }
        }
        catch (Throwable e)
        {
            failTest("Failed to test Event item import: " + e);
        } // end of try-catch

        log("Test that commiting the imported Event item increases item count in EventList.");
        try
        {
            e2.commit();
            log("Event added: " +e2.getDate(Event.START, 0)+"-"+e2.getDate(Event.END, 0));

            if (countItems(el, dates[0], dates[2], true) != (count + 1))
            {
                log("Range: "+dates[0]+"-"+dates[2]);
                cleanup(e2);
                failTest("Commiting imported Event did not increase EventList item count by one!");
            }
            if (countItems(el, dates[1], dates[2], true) != countR)
            {
                log("Range: "+dates[1]+"-"+dates[2]);
                cleanup(e2);
                failTest("Commiting imported Event increased EventList item count by one when checking for initial event only and range out of imported event!");
            }
            if (countItems(el, dates[1], dates[2], false) != (countR + 1))
            {
                log("Range: "+dates[1]+"-"+dates[2]);
                cleanup(e2);
                failTest("Commiting imported Event did not increase EventList item count for other than initial event only!");
            }
        }
        catch (Exception e)
        {
            cleanup(e2);
            failTest("Failed to commit imported Event to list: " + e);
        }

        log("Test that removing imported Event item decreases item count.");
        try
        {
            el.removeEvent(e2);
            if (countItems(el, dates[0], dates[2], true) != count)
            {
                failTest("Removing imported Event did not decrease EventList item count by one!");
            }
            if (countItems(el, dates[1], dates[2], false) != countR)
            {
                failTest("Removing imported Event did not decrease EventList item&repeat count by one!");
            }
            el.close();
        }
        catch (Exception e)
        {
            failTest("Failed to remove imported Event from list: " + e);
        }


        passTest();
    }

    private void cleanup(Event e2)
    {
        try
        {
            el.removeEvent(e2);
            el.close();
        }
        catch (Exception e)
        {
            log("Error while cleaning up: "+e);
        }
    }

    private int countItems(EventList ael, long date1, long date2, boolean initialOnly) throws TestEndThrowable
    {
        int tmp = 0;
        try
        {
            Enumeration e = ael.items(EventList.OCCURRING, date1, date2, initialOnly);
            while (e.hasMoreElements())
            {
                Event ev = (Event)e.nextElement();
                log("Event found at: " +ev.getDate(Event.START, 0)+"-"+ev.getDate(Event.END, 0));
                tmp++;
            }
        }
        catch (PIMException pime)
        {
            failTest("Error while getting items in list!");
        }
        log(tmp+" Events found at: "+date1+"-"+date2+", initialOnly="+initialOnly);
        return tmp;
    }

}
