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
package com.nokia.mj.test.pim.automatic.RepeatRule.RRdates;
//IMPORTS
import java.io.IOException;
import java.util.*;
import javax.microedition.pim.RepeatRule;
import com.nokia.mj.test.pim.adaptation.*;
// CLASS DEFINITION
/**
 * This class tests javax.microedition.pim.RepeatRule.dates().
 */
public class RRdates
        extends MIDPTest
{

    public RRdates(
        String sTestName, j2meunit.framework.TestMethod rTestMethod)
    {
        super(sTestName, rTestMethod);
    }


    public void runTest() throws TestEndThrowable
    {

        RepeatRule rr = new RepeatRule();
        String[] stringDates = { "050101:080000", "050106:080000", "050111:080000"};
        long[] dates = TimeTool.getTimeInMillis(stringDates);

        // test that calling rr.dates() returns date
        // for every day (freq=daily as default)
        Enumeration e = rr.dates(dates[0], dates[1], dates[2]);

        int count = 0;
        while (e.hasMoreElements())
        {
            e.nextElement();
            count++;
        }
        if (count != 6)
        {
            log(count+ " dates returned.");
            failTest("rr.dates() did not return every day as default!");
        }
        // test that setting subset beginning to bigger than subset ending causes IAE
        try
        {
            Enumeration e2 = rr.dates(dates[0], dates[2], dates[1]);
            failTest("rr.dates() with bigger subset beginning than ending did not throw Exception!");
        }
        catch (IllegalArgumentException iae)
        {
            // ok
        }
        catch (Exception ex)
        {
            failTest("rr.dates() with bigger subset beginning than ending threw wrong exception: " +ex);
        }

        // all is well since we get this far
        passTest();
    }
}