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
package com.nokia.mj.test.pim.automatic.EventList.ELremoveEventEx;
//IMPORTS
import java.io.IOException;
import java.util.*;
import javax.microedition.pim.PIM;
import javax.microedition.pim.PIMItem;
import javax.microedition.pim.PIMException;
import javax.microedition.pim.PIMList;
import javax.microedition.pim.Event;
import javax.microedition.pim.EventList;
import com.nokia.mj.test.pim.adaptation.*;

// CLASS DEFINITION
/**
 * This class tests javax.microedition.pim.EventList.removeEvent()
 *
 */
public class ELremoveEventEx
        extends MIDPTest
{

    public ELremoveEventEx(
        String sTestName, j2meunit.framework.TestMethod rTestMethod)
    {
        super(sTestName, rTestMethod);
    }

    // preconditions
    PIM p = null;
    EventList el = null;
    int count = 0;

    public void runTest() throws TestEndThrowable
    {
        try
        {
            p = PIM.getInstance();
        }
        catch (Throwable t)
        {
            failTest("preconditions failed: " + t);
        }

        el = openEventList(PIM.READ_WRITE);
        count = countItems(el);

        log("EventList has "+ count + " items.");

        log("Test that creating new Event item and adding it to the list does increase item count in EventList.");

        Event e = null;

        try
        {
            e = el.createEvent();
            if (el.isSupportedField(Event.SUMMARY))
            {
                e.addString(Event.SUMMARY, PIMItem.ATTR_NONE, "testing");
            }
            else
            {
                failTest("Error while creating Event item!");
            }
            e.commit();

            if (countItems(el) != count+1)
            {
                failTest("Creating new Event did not increase EventList item count by one!");
            }

        }
        catch (Throwable t)
        {
            failTest("Failed to test Event item creation: " + t);
        }

        log("Test that removing Event item from a read only list causes SecurityException.");
        try
        {
            el.close();
        }
        catch (Exception ex)
        {
            failTest("Failed to close list: " + ex);
        }
        el = openEventList(PIM.READ_ONLY);

        try
        {
            e = find(el, e);
            el.removeEvent(e);
            failTest("SecurityException was not thrown when removing event from a read only list.");
        }
        catch (SecurityException se)
        {
            // ok.
        }
        catch (Exception ex)
        {
            failTest("Removing Event from a read only list threw wrong exception: "+ex);
        }

        if (countItems(el) != count+1)
        {
            failTest("Removing Event from a read only list decreased item count by one!");
        }

        try
        {
            el.close();
        }
        catch (Exception ex)
        {
            failTest("Failed to close list: " + ex);
        }
        el = openEventList(PIM.READ_WRITE);

        log("Test that removing null Event item does not change EventList item count.");
        try
        {
            el.removeEvent(null);
            failTest("Removing null Event did not throw any exception!");
        }
        catch (NullPointerException npe)
        {
            // ok.
        }
        catch (Exception ex)
        {
            failTest("Removing null Event threw wrong exception: "+ex);
        }

        if (countItems(el) != count+1)
        {
            failTest("Removing null Event changed EventList item count!");
        }

        log("Test that removing Event item decreases EventList item count by one.");

        try
        {
            e = find(el, e);
            el.removeEvent(e);
        }
        catch (Throwable t)
        {
            failTest("Failed to test Event item removal: " + t);
        }

        if (countItems(el) != count)
        {
            failTest("Removing Event did not decrease EventList item count by one!");
        }

        log("Test that removing Event item not in list does not change EventList item count.");

        try
        {
            el.removeEvent(e);
            failTest("Removing Event not in list did not throw any exception!");
        }
        catch (PIMException pime)
        {
            // ok.
        }
        catch (Exception ex)
        {
            failTest("Removing Event not in list threw wrong exception: "+ex);
        }

        if (countItems(el) != count)
        {
            failTest("Removing Event not in list changed EventList item count!");
        }

        log("Test that removing Event from a closed list throws PIMException.");
        try
        {
            el.close();
        }
        catch (Exception ex)
        {
            failTest("Failed to close list: " + ex);
        }

        try
        {
            el.removeEvent(e);
            failTest("Removing Event from a closed list did not throw any exception!");
        }
        catch (PIMException pime)
        {
            // ok.
        }
        catch (Exception ex)
        {
            failTest("Removing Event from a closed list threw wrong exception: "+ex);
        }
        el = openEventList(PIM.READ_WRITE);

        if (countItems(el) != count)
        {
            failTest("Removing Event from a closed list changed EventList item count!");
        }
        try
        {
            el.close();
        }
        catch (Exception ex)
        {
            failTest("Failed to close list: " + ex);
        }

        passTest();
    }

    private Event find(EventList el, Event e) throws TestEndThrowable

    {
        String id = e.getString(Event.UID, 0);
        try
        {
            Enumeration en = el.items();
            while (en.hasMoreElements())
            {
                Event tmp = (Event)en.nextElement();
                String tmpid = tmp.getString(Event.UID, 0);
                if (tmpid.equals(id))
                {
                    return tmp;
                }
            }
        }
        catch (IllegalArgumentException iae)
        {
            // item does not originate from the list
            return null;
        }
        catch (Exception ex)
        {
            failTest("Error while getting items from list!"+ex);
        }
        return null;
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

    private EventList openEventList(int mode) throws TestEndThrowable
    {
        // open first Event list
        try
        {
            String[] eventListNames = p.listPIMLists(PIM.EVENT_LIST);
            el = (EventList)p.openPIMList(PIM.EVENT_LIST, mode, eventListNames[0]);
            return el;
        }
        catch (Throwable e)
        {
            failTest("EventList opening failed: " + e);
        }
        return null;
    }
}
