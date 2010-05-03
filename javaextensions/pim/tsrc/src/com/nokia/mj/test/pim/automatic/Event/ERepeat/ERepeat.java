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
package com.nokia.mj.test.pim.automatic.Event.ERepeat;
//IMPORTS
import java.io.IOException;
import java.util.*;
import javax.microedition.pim.PIM;
import javax.microedition.pim.PIMItem;
import javax.microedition.pim.Event;
import javax.microedition.pim.EventList;
import javax.microedition.pim.RepeatRule;
import com.nokia.mj.test.pim.adaptation.*;

// CLASS DEFINITION
/**
 * Tests javax.microedition.pim.Event.setRepeat() and
 * javax.microedition.pim.Event.getRepeat()
 *
 */
public class ERepeat
        extends MIDPTest
{

    public ERepeat(
        String sTestName, j2meunit.framework.TestMethod rTestMethod)
    {
        super(sTestName, rTestMethod);
    }


    public void runTest() throws TestEndThrowable
    {
        // preconditions
        RepeatRule rr1 = null;
        PIM p = null;
        EventList eventList = null;
        Event e = null;
        try
        {
            rr1 = new RepeatRule();
            rr1.setInt(RepeatRule.FREQUENCY, RepeatRule.DAILY);
            p = PIM.getInstance();
            eventList =
                (EventList)p.openPIMList(PIM.EVENT_LIST, PIM.READ_ONLY);
            e = eventList.createEvent();
        }
        catch (Throwable ex)
        {
            failTest("preconditions failed: " + ex.toString());
        } // end of try-catch


        // Test steps
        // step s1
        testGetRepeat(e, null);

        // step s2
        RepeatRule rr2 = testSetRepeat(e, rr1);

        // step s3
        rr2.setInt(RepeatRule.FREQUENCY, RepeatRule.WEEKLY);
        testSetRepeat(e, rr2);
        if (rr1.getInt(RepeatRule.FREQUENCY) != RepeatRule.DAILY)
        {
            failTest("rr1 has been modified");
        }

        // step s4
        testGetRepeat(e, rr2);

        // step s5
        testSetRepeat(e, null);

        passTest();
    }

    /**
     * Test steps s1, s2, s4, s5
     */
    private RepeatRule testGetRepeat(Event aEvent, RepeatRule aExpected)
    throws TestEndThrowable
    {
        RepeatRule repeat = null;
        try
        {
            repeat = aEvent.getRepeat();
        }
        catch (Throwable e)
        {
            failTest("getRepeat threw: " + e.toString());
        } // end of try-catch
        if (aExpected == null)
        {
            if (repeat != null)
            {
                failTest("getRepeat did not return null");
            } // end of if (repeat != null)
        } // end of if (aExpected == null)
        else
        {
            if (!aExpected.equals(repeat))
            {
                failTest("the repeat rules do not match");
            } // end of if (!aExpected.equals( repeat ))
        } // end of else
        return repeat;
    }

    /**
     * Test steps s2, s3, s5
     */
    private RepeatRule testSetRepeat(Event aEvent, RepeatRule aRepeat)
    throws TestEndThrowable
    {
        try
        {
            aEvent.setRepeat(aRepeat);
        }
        catch (Throwable e)
        {
            failTest("setRepeat threw:" + e.toString());
        } // end of try-catch
        return testGetRepeat(aEvent, aRepeat);
    }

}


