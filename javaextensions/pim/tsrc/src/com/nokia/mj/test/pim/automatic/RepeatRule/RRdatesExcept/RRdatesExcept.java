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
package com.nokia.mj.test.pim.automatic.RepeatRule.RRdatesExcept;
//IMPORTS
import java.io.IOException;
import java.util.*;
import javax.microedition.pim.RepeatRule;
import com.nokia.mj.test.pim.adaptation.*;

// CLASS DEFINITION
/**
 * This class tests javax.microedition.pim.RepeatRule.dates().
 */
public class RRdatesExcept
        extends MIDPTest
{

    public RRdatesExcept(
        String sTestName, j2meunit.framework.TestMethod rTestMethod)
    {
        super(sTestName, rTestMethod);
    }


    public void runTest() throws TestEndThrowable
    {
        Enumeration e, e2;
        int count;

        long hourInMillis = 60 * 60 * 1000;
        RepeatRule rr = new RepeatRule();
        String[] stringDates = { "040101:080000", "040131:080000" };

        long[] dates = TimeTool.getTimeInMillis(stringDates);

        rr.setInt(RepeatRule.FREQUENCY, RepeatRule.DAILY);

        // verify that every day of the range is returned (or at least count is correct)
        e = rr.dates(dates[0], dates[0] - hourInMillis, dates[1] + hourInMillis);

        count = 0;
        while (e.hasMoreElements())
        {
            count++;
            Date d = (Date) e.nextElement();
            // add every other (=odd) day to except dates
            if (count % 2 != 0)
            {
                rr.addExceptDate(d.getTime());
            }
        }
        if (count != 31)
        {
            failTest("Correct number of dates was not returned. Was supposed to return 31, returned " +count);
        }

        // verify that every other day of the range is returned (or at least count is correct)
        e = rr.dates(dates[0], dates[0] - hourInMillis, dates[1] + hourInMillis);
        count = 0;
        while (e.hasMoreElements())
        {
            count++;
            e.nextElement();
        }
        if (count != 15)
        {
            failTest("Correct number of dates was not returned. Was supposed to return 15, returned " +count);
        }

        // tests ok
        passTest();
    }

}