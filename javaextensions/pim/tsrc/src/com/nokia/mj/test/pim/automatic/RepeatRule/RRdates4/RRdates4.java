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
package com.nokia.mj.test.pim.automatic.RepeatRule.RRdates4;
//IMPORTS
import java.io.IOException;
import java.util.*;
import javax.microedition.pim.RepeatRule;
import com.nokia.mj.test.pim.adaptation.*;
// CLASS DEFINITION
/**
 * This class tests javax.microedition.pim.RepeatRule.dates().
 */
public class RRdates4 extends MIDPTest
{

    public RRdates4(
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
        String[] stringDates = { "040101:080000", "040128:080000" };

        long[] dates = TimeTool.getTimeInMillis(stringDates);

        rr.setInt(RepeatRule.FREQUENCY, RepeatRule.DAILY);

        log("test that rr.dates() FREQUENCY=DAILY return 28 dates");
        e = rr.dates(dates[0], dates[0], dates[1]);

        count = 0;
        while (e.hasMoreElements())
        {
            e.nextElement();
            count++;
        }

        if (count != 28)
        {
            failTest("rr.dates() does not return 28 days but " + count);
        }

        // all is well since we get this far
        passTest();
    }

}