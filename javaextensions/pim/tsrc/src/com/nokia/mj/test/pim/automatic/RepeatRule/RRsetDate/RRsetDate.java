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
package com.nokia.mj.test.pim.automatic.RepeatRule.RRsetDate;
//IMPORTS
import java.io.IOException;
import java.util.*;
import javax.microedition.pim.RepeatRule;
import com.nokia.mj.test.pim.adaptation.*;

// CLASS DEFINITION
/**
 * This class tests javax.microedition.pim.RepeatRule.setdate().
 */
public class RRsetDate extends MIDPTest
{

    public RRsetDate(
        String sTestName, j2meunit.framework.TestMethod rTestMethod)
    {
        super(sTestName, rTestMethod);
    }

    private static long iMillis2000 = 946677600000L;

    public void runTest() throws TestEndThrowable
    {
        RepeatRule r = new RepeatRule();

        try
        {
            r.setDate(RepeatRule.END, iMillis2000);
        }
        catch (Exception e)
        {
            failTest("rr.setDate( RepeatRule.END, "+iMillis2000+
                     " ) threw Exception: " + e);
        }

        for (int i = -5; i < 4098; i++)  // range should be enough
        {
            // skip RepeatRule.END since it must not throw IAException
            if (i == RepeatRule.END)
            {
                i++;
            }

            try
            {
                r.setDate(i, iMillis2000);
                failTest("setDate("+i+","+iMillis2000+
                         ") did not throw IllegalArgumentException!");
            }
            catch (IllegalArgumentException e)
            {
                // ok
            }
            catch (Exception e)
            {
                failTest("rr.setDate( "+i+", "+iMillis2000+
                         " ) threw wrong Exception: " + e);
            }
        }

        passTest();
    }
}