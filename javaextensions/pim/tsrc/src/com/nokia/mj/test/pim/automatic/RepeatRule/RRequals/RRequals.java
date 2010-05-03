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
package com.nokia.mj.test.pim.automatic.RepeatRule.RRequals;
//IMPORTS
import java.io.IOException;
import java.util.*;
import javax.microedition.pim.RepeatRule;
import com.nokia.mj.test.pim.adaptation.*;


// CLASS DEFINITION
/**
 * This class tests javax.microedition.pim.RepeatRule.dates().
 */
public class RRequals
        extends MIDPTest
{

    public RRequals(
        String sTestName, j2meunit.framework.TestMethod rTestMethod)
    {
        super(sTestName, rTestMethod);
    }


    public void runTest() throws TestEndThrowable
    {
        long hourInMillis = 60 * 60 * 1000;
        RepeatRule rr1 = new RepeatRule();
        RepeatRule rr2 = new RepeatRule();
        long date = TimeTool.getTimeInMillis("040101:080000");

        log("Test that empty rrs are equal.");
        if (!rr1.equals(rr2))
        {
            failTest("Two empty RepeatRules are not equal!");
        }

        log("Test int fields.");
        rr1.setInt(RepeatRule.FREQUENCY, RepeatRule.DAILY);
        if (rr1.equals(rr2))
        {
            failTest("Two different RepeatRules are equal!");
        }

        rr2.setInt(RepeatRule.FREQUENCY, RepeatRule.DAILY);
        if (!rr1.equals(rr2))
        {
            failTest("Two equal RepeatRules are not equal!");
        }

        log("Test exception dates.");
        rr1.addExceptDate(date);
        if (rr1.equals(rr2))
        {
            failTest("Two different RepeatRules are equal!");
        }

        rr2.addExceptDate(date);
        if (!rr1.equals(rr2))
        {
            failTest("Two equal RepeatRules are notequal!");
        }

        log("Test that equal does not care about hours&minutes in dates.");

        log("Replace date with date + hour.");
        Date d = (Date) rr2.getExceptDates().nextElement();
        rr2.removeExceptDate(d.getTime());
        rr2.addExceptDate(d.getTime() + hourInMillis);

        if (!rr1.equals(rr2))
        {
            failTest("Two equal RepeatRules are not equal!");
        }

        log("Test that two different dates do not make rrs equals.");
        Date d2 = (Date) rr2.getExceptDates().nextElement();
        rr2.removeExceptDate(d2.getTime());
        rr2.addExceptDate(d2.getTime() + hourInMillis * 100);

        if (rr1.equals(rr2))
        {
            failTest("Two different RepeatRules are equal!");
        }

        log("Removing exception dates.");
        rr1.removeExceptDate(date);
        rr2.removeExceptDate(d2.getTime() + hourInMillis * 100);

        log("Test END.");
        rr1.setDate(RepeatRule.END, date);
        if (rr1.equals(rr2))
        {
            failTest("Two different RepeatRules are equal!");
        }
        rr2.setDate(RepeatRule.END, date);
        if (!rr1.equals(rr2))
        {
            failTest("Two equal RepeatRules are notequal!");
        }

        log("Test passed.");
        passTest();
    }
}