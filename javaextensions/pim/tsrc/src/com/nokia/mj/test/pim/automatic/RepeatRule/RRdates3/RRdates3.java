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
package com.nokia.mj.test.pim.automatic.RepeatRule.RRdates3;
//IMPORTS
import java.io.IOException;
import java.util.*;
import javax.microedition.pim.RepeatRule;
import com.nokia.mj.test.pim.adaptation.*;
// CLASS DEFINITION
/**
 * This class tests javax.microedition.pim.RepeatRule.dates().
 */
public class RRdates3 extends MIDPTest
{

    public RRdates3(
        String sTestName, j2meunit.framework.TestMethod rTestMethod)
    {
        super(sTestName, rTestMethod);
    }


    public void runTest() throws TestEndThrowable
    {
        Enumeration e;
        int count;
        long tmp;

        long hourInMillis = 60 * 60 * 1000;
        RepeatRule rr = new RepeatRule();
        String[] stringDates = { "040101:080000", "041231:080000" };

        long[] dates = TimeTool.getTimeInMillis(stringDates);

        rr.setInt(RepeatRule.FREQUENCY, RepeatRule.DAILY);

        log("test that rr.dates() FREQUENCY=DAILY return 366 dates (2004 is a leap year)");
        e = rr.dates(dates[0], dates[0], dates[1]);

        count = 0;
        while (e.hasMoreElements())
        {
            e.nextElement();
            count++;
        }

        if (count != 366)
        {
            failTest("rr.dates() does not return 366 days!");
        }

        log("test that rr.dates() FREQUENCY=DAILY & INTERVAL=2 return 183 dates (2004 is a leap year)");
        rr.setInt(RepeatRule.INTERVAL, 2);
        e = rr.dates(dates[0], dates[0], dates[1]);

        count = 0;
        tmp = 0;
        while (e.hasMoreElements())
        {
            Date d = (Date) e.nextElement();
            // test that every other day is skipped
            if ((count > 1) && (d.getTime() - tmp < hourInMillis * 47))
            {
                failTest("rr.dates() with FREQUENCY=DAILY & INTERVAL=2 does not skip every other day!");
            }
            tmp = d.getTime();
            count++;
        }

        if (count != 183)
        {
            failTest("rr.dates() does not return 183 days!");
        }

        log("test that rr.dates() FREQUENCY=DAILY & INTERVAL=2 & COUNT=2 return 2 dates");
        rr.setInt(RepeatRule.COUNT, 2);
        e = rr.dates(dates[0], dates[0], dates[1]);

        count = 0;
        tmp = 0;
        while (e.hasMoreElements())
        {
            Date d = (Date) e.nextElement();
            // test that every other day is skipped
            if ((count > 1) && (d.getTime() - tmp < hourInMillis * 47))
            {
                failTest("rr.dates() with FREQUENCY=DAILY & INTERVAL=2 & COUNT=2 does not skip every other day!");
            }
            tmp = d.getTime();
            count++;
        }

        if (count != 2)
        {
            failTest("rr.dates() does not return 2 days!");
        }

        // all is well since we get this far
        passTest();
    }

}