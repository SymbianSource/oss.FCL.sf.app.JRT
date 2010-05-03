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
package com.nokia.mj.test.pim.automatic.RepeatRule.RRdates2;
//IMPORTS
import java.io.IOException;
import java.util.*;
import javax.microedition.pim.RepeatRule;
import com.nokia.mj.test.pim.adaptation.*;
// CLASS DEFINITION
/**
 * This class tests javax.microedition.pim.RepeatRule.dates().
 */
public class RRdates2 extends MIDPTest
{

    public RRdates2(
        String sTestName, j2meunit.framework.TestMethod rTestMethod)
    {
        super(sTestName, rTestMethod);
    }


    public void runTest() throws TestEndThrowable
    {
        Enumeration e;
        int count;

        long hourInMillis = 60 * 60 * 1000;
        RepeatRule rr = new RepeatRule();
        String[] stringDates = { "050101:080000", "050106:080000", "050111:080000",
                                 "050116:080000", "050121:080000", "050126:080000"
                               };

        long[] dates = TimeTool.getTimeInMillis(stringDates);

        rr.setInt(RepeatRule.FREQUENCY, RepeatRule.DAILY);
        rr.setDate(RepeatRule.END, dates[4] + hourInMillis);

        // test that rr.dates() return empty enumeration when range is after END
        e = rr.dates(dates[0], dates[4] + hourInMillis*2, dates[5]);
        if (e.hasMoreElements())
        {
            failTest("rr.dates() does not return empty enumeration when range is after END!");
        }

        // test that rr.dates() return empty enumeration when starting date is after END
        e = rr.dates(dates[4] + hourInMillis*2, dates[0], dates[5]);
        if (e.hasMoreElements())
        {
            failTest("rr.dates() does not return empty enumeration when starting date is after END!");
        }

        // test that rr.dates() return empty enumeration when range is before starting date
        e = rr.dates(dates[3], dates[0], dates[2]);
        if (e.hasMoreElements())
        {
            failTest("rr.dates() does not return empty enumeration when range is before starting date!");
        }

        // test that rr.dates() returns correct enumeration when range starts before starting date
        e = rr.dates(dates[0], dates[0] - hourInMillis, dates[2] + hourInMillis);
        count = 0;
        while (e.hasMoreElements())
        {
            e.nextElement();
            count++;
        }
        if (count != 11)
        {
            failTest("rr.dates() does not return correct enumeration when range starts before starting date!" +
                     "Count: "+count+", should be 11.");
        }

        // test that rr.dates() returns correct enumeration when range starts after starting date
        e = rr.dates(dates[0], dates[1] - hourInMillis, dates[3] + hourInMillis);
        count = 0;
        while (e.hasMoreElements())
        {
            e.nextElement();
            count++;
        }
        if (count != 11)
        {
            failTest("rr.dates() does not return correct enumeration when range starts after starting date!" +
                     "Count: "+count+", should be 11.");
        }

        // test that rr.dates() returns correct enumeration when range ends after END date
        e = rr.dates(dates[0], dates[3] - hourInMillis, dates[5] + hourInMillis);
        count = 0;
        while (e.hasMoreElements())
        {
            e.nextElement();
            count++;
        }
        if (count != 6)
        {
            failTest("rr.dates() does not return correct enumeration when range ends after END date!" +
                     "Count: "+count+", should be 6.");
        }

        // move END date with one month
        rr.setDate(RepeatRule.END, dates[0] + 31 * 24 * hourInMillis);

        // test that .... something :)
        e = rr.dates(dates[0], dates[4] - hourInMillis, dates[5] + hourInMillis);
        count = 0;
        while (e.hasMoreElements())
        {
            e.nextElement();
            count++;
        }
        if (count != 6)
        {
            failTest("rr.dates() does not return correct enumeration ..." +
                     "Count: "+count+", should be 6.");
        }

        // all is well since we get this far
        passTest();
    }

}