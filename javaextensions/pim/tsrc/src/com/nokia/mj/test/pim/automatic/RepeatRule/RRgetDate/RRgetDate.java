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
package com.nokia.mj.test.pim.automatic.RepeatRule.RRgetDate;
//IMPORTS
import java.io.IOException;
import java.util.*;
import javax.microedition.pim.RepeatRule;
import javax.microedition.pim.FieldEmptyException;
import com.nokia.mj.test.pim.adaptation.*;

// CLASS DEFINITION
/**
 * This class tests javax.microedition.pim.RepeatRule.getDate().
 */
public class RRgetDate extends MIDPTest
{

    public RRgetDate(
        String sTestName, j2meunit.framework.TestMethod rTestMethod)
    {
        super(sTestName, rTestMethod);
    }

    private static long iMillis2000 = 946677600000L;

    public void runTest() throws TestEndThrowable
    {
        long date=0;
        RepeatRule r = new RepeatRule();

        // test that getting unset date causes FieldEmptyException
        try
        {
            date = r.getDate(RepeatRule.END);
            failTest("rr.getDate( RepeatRule.END ) returned a "+
                     "date when no date is set!");
        }
        catch (FieldEmptyException e)
        {
            // ok
        }
        catch (Exception e)
        {
            failTest("rr.getDate( RepeatRule.END ) threw Exception: " + e);
        }

        // test that using invalid parameter field causes IAE when date is not set
        for (int i = -5; i < 4098; i++)  // range should be enough
        {
            // skip RepeatRule.END since it must not throw IAException
            if (i == RepeatRule.END)
            {
                i++;
            }

            try
            {
                long val = r.getDate(i);
                failTest("getDate("+i+") did not throw IllegalArgumentException!");
            }
            catch (IllegalArgumentException e)
            {
                // ok
            }
            catch (Exception e)
            {
                failTest("rr.getDate( "+i+", 1 ) threw wrong Exception: " + e);
            }
        }

        for (long i = iMillis2000-1000000L; i < iMillis2000+1000000L; i+=100000L)
        {
            // set date to have some value
            try
            {
                r.setDate(RepeatRule.END, i);
            }
            catch (Exception e)
            {
                failTest("rr.setDate( RepeatRule.END, "+i+" ) threw Exception: " + e);
            }

            // test that getting date when it is set does not cause FieldEmptyException
            try
            {
                date = r.getDate(RepeatRule.END);
            }
            catch (FieldEmptyException e)
            {
                failTest("rr.getDate( RepeatRule.END ) threw FFE when date is set!");
            }
            catch (Exception e)
            {
                failTest("rr.getDate( RepeatRule.END ) threw Exception: " + e);
            }

            // test that returned date is as set, do it this way because of possible rounding...
            r.setDate(RepeatRule.END, date);
            long date2 = r.getDate(RepeatRule.END);
            if (date != date2)
            {
                failTest("rr.getDate( RepeatRule.END ) did not return the date that was set. ");
            }
        }

        // test that using invalid parameter field causes IAE when date is set
        for (int i = -5; i < 4098; i++)  // range should be enough
        {
            // skip RepeatRule.END since it must not throw IAException
            if (i == RepeatRule.END)
            {
                i++;
            }

            try
            {
                long val = r.getDate(i);
                failTest("getDate("+i+") did not throw IllegalArgumentException!");
            }
            catch (IllegalArgumentException e)
            {
                // ok
            }
            catch (Exception e)
            {
                failTest("rr.getDate( "+i+") threw wrong Exception: " + e);
            }
        }

        passTest();
    }
}