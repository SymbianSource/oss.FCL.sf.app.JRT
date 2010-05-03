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
package com.nokia.mj.test.pim.automatic.EventList.ELremoveEvent;
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
public class ELremoveEvent
        extends MIDPTest
{

    public ELremoveEvent(
        String sTestName, j2meunit.framework.TestMethod rTestMethod)
    {
        super(sTestName, rTestMethod);
    }

    // preconditions
    PIM p = null;
    String[] eventListNames = null;
    EventList el = null;
    int count = 0;

    public void runTest() throws TestEndThrowable
    {

        // open first Event list
        try
        {
            p = PIM.getInstance();
            eventListNames = p.listPIMLists(PIM.EVENT_LIST);
            el = (EventList)p.openPIMList(PIM.EVENT_LIST, PIM.READ_WRITE, eventListNames[0]);

            count = countItems(el);
        }
        catch (Throwable t)
        {
            failTest("preconditions failed: " + t);
        } // end of try-catch

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

        log("Test that removing Event item decreases EventList item count by one.");

        try
        {
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
