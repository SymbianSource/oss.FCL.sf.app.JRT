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
package com.nokia.mj.test.pim.automatic.RepeatRule.RRConstants;
//IMPORTS
import java.io.IOException;
import java.util.*;
import javax.microedition.pim.RepeatRule;
import com.nokia.mj.test.pim.adaptation.*;

// CLASS DEFINITION
/**
 * This class tests javax.microedition.pim.EventList constants.
 */
public class RRConstants
        extends MIDPTest
{

    public RRConstants(
        String sTestName, j2meunit.framework.TestMethod rTestMethod)
    {
        super(sTestName, rTestMethod);
    }


    public void runTest() throws TestEndThrowable
    {
        int[] constants = { RepeatRule.DAY_IN_MONTH, 1,
                            RepeatRule.DAY_IN_WEEK, 2,
                            RepeatRule.DAY_IN_YEAR, 4,
                            RepeatRule.MONTH_IN_YEAR, 8,
                            RepeatRule.WEEK_IN_MONTH, 16,
                            RepeatRule.COUNT, 32,
                            RepeatRule.END, 64,
                            RepeatRule.FREQUENCY, 0,
                            RepeatRule.INTERVAL, 128,
                            RepeatRule.DAILY, 16,
                            RepeatRule.WEEKLY, 17,
                            RepeatRule.MONTHLY, 18,
                            RepeatRule.YEARLY, 19,
                            RepeatRule.FIRST, 1,
                            RepeatRule.SECOND, 2,
                            RepeatRule.THIRD, 4,
                            RepeatRule.FOURTH, 8,
                            RepeatRule.FIFTH, 16,
                            RepeatRule.LAST, 32,
                            RepeatRule.SECONDLAST, 64,
                            RepeatRule.THIRDLAST, 128,
                            RepeatRule.FOURTHLAST, 256,
                            RepeatRule.FIFTHLAST, 512,
                            RepeatRule.SATURDAY, 1024,
                            RepeatRule.FRIDAY, 2048,
                            RepeatRule.THURSDAY, 4096,
                            RepeatRule.WEDNESDAY, 8192,
                            RepeatRule.TUESDAY, 16384,
                            RepeatRule.MONDAY, 32768,
                            RepeatRule.SUNDAY, 65536,
                            RepeatRule.JANUARY, 131072,
                            RepeatRule.FEBRUARY, 262144,
                            RepeatRule.MARCH, 524288,
                            RepeatRule.APRIL, 1048576,
                            RepeatRule.MAY, 2097152,
                            RepeatRule.JUNE, 4194304,
                            RepeatRule.JULY, 8388608,
                            RepeatRule.AUGUST, 16777216,
                            RepeatRule.SEPTEMBER, 33554432,
                            RepeatRule.OCTOBER, 67108864,
                            RepeatRule.NOVEMBER, 134217728,
                            RepeatRule.DECEMBER, 268435456
                          };

        for (int i=0; i < constants.length; i += 2)
        {
            if (constants[i] != constants[i+1])
            {
                failTest("Class field (" + (i+1) + ") is not correct.");
            }
        }
        // all is well
        passTest();
    }
}