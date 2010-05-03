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
package com.nokia.mj.test.pim.automatic.EventList.ELcreateEvent;
//IMPORTS
import java.io.IOException;
import java.io.InputStream;
import java.io.UnsupportedEncodingException;
import java.util.*;
import javax.microedition.pim.*;
import com.nokia.mj.test.pim.adaptation.*;

// CLASS DEFINITION
/**
 * This class tests javax.microedition.pim.EventList.createEvent()
 */
public class ELcreateEvent
        extends MIDPTest
{

    public ELcreateEvent(
        String sTestName, j2meunit.framework.TestMethod rTestMethod)
    {
        super(sTestName, rTestMethod);
    }


    public void runTest() throws TestEndThrowable
    {
        // preconditions
        PIM p = null;
        String[] eventListNames = null;
        EventList el = null;
        int count = 0;

        // open first event list
        try
        {
            p = PIM.getInstance();
            eventListNames = p.listPIMLists(PIM.EVENT_LIST);
            el = (EventList)p.openPIMList(PIM.EVENT_LIST, PIM.READ_WRITE, eventListNames[0]);

            Enumeration e = el.items();
            count = itemCount(e);
        }
        catch (Throwable t)
        {
            failTest("preconditions failed: " + t);
        }

        log("EventList has "+ count + " items.");

        log("Test that creating new Event item does not increase item count in EventList.");

        Event e = null;

        try
        {
            e = el.createEvent();

            Enumeration en = el.items();
            el.close();

            if (itemCount(en) != count)
            {
                failTest("Creating new Event changed EventList item count!");
            }

        }
        catch (Throwable t)
        {
            failTest("Failed to test Event item creation: " + t);
        }

        log("Test that created Event item is empty.");
        int[] fields = e.getFields();
        if (fields.length != 0)
        {
            failTest("Created new Event item has not got all fields empty!");
        }

        passTest();
    }

    private int itemCount(Enumeration e)
    {
        int tmp = 0;
        while (e.hasMoreElements())
        {
            e.nextElement();
            tmp++;
        }
        return tmp;
    }
}
