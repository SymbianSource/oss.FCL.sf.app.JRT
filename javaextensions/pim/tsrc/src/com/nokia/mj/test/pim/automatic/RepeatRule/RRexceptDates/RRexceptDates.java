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
package com.nokia.mj.test.pim.automatic.RepeatRule.RRexceptDates;
//IMPORTS
import java.io.IOException;
import java.util.*;
import javax.microedition.pim.RepeatRule;
import com.nokia.mj.test.pim.adaptation.*;
// CLASS DEFINITION
/**
 */
public class RRexceptDates
        extends MIDPTest
{

    public RRexceptDates(
        String sTestName, j2meunit.framework.TestMethod rTestMethod)
    {
        super(sTestName, rTestMethod);
    }


    public void runTest() throws TestEndThrowable
    {

        RepeatRule rr = new RepeatRule();
        RepeatRule rr2 = new RepeatRule();

        // test that calling rr.getExceptDates() returns empty enumeration
        Enumeration e = rr.getExceptDates();

        if (e.hasMoreElements())
        {
            failTest("rr.getExceptDates() returned a non-empty Enumeration, when no except date is set!");
        }

        String[] stringDates = { "040101:080000", "040128:080000", "040801:080000", "050218:080000"  };
        long[] dates = TimeTool.getTimeInMillis(stringDates);

        // test that rr.getExceptDates() returns all dates that were set using rr.addExceptDate()
        long[] dates2 = new long[dates.length];

        for (int i=0; i < dates.length; i++)
        {
            rr.addExceptDate(dates[i]);
        }

        Enumeration e2 = rr.getExceptDates();
        int count = 0;
        while (e2.hasMoreElements())
        {
            dates2[count] = ((Date)e2.nextElement()).getTime();
            count++;
        }
        if (count != dates.length)
        {
            failTest("rr.getExceptDates() did not return as many dates as was set using rr.addExceptDate()!");
        }

        // use dates2 to test that date is not modified during add&get
        for (int i=0; i < dates2.length; i++)
        {
            rr2.addExceptDate(dates2[i]);
        }

        Enumeration e3 = rr2.getExceptDates();
        int count2 = 0;
        while (e3.hasMoreElements())
        {
            long tmp = ((Date)e3.nextElement()).getTime();
            count2++;
            if (!containsLong(dates2, tmp))
            {
                failTest("rr2.getExceptDates() did not return the exact date as was set using rr2.addExceptDate()!");
            }
        }
        if (count2 != dates2.length)
        {
            failTest("rr2.getExceptDates() did not return as many dates as was set using rr2.addExceptDate()!");
        }

        // test that after calling rr.removeExceptDate(), date is removed
        Enumeration e4 = rr2.getExceptDates();

        while (e4.hasMoreElements())
        {
            long tmp = ((Date)e4.nextElement()).getTime();
            rr2.removeExceptDate(tmp);

            if (containsExceptDate(rr2, tmp))
            {
                failTest("rr2.removeExceptDate() did not remove the correct date!");
            }
        }

        if (rr2.getExceptDates().hasMoreElements())
        {
            failTest("rr2.removeExceptDate() did not remove all dates!");
        }

        // test that adding same date several times will result the date being added only once
        for (int i=0; i < 10; i++)
        {
            rr2.addExceptDate(dates2[0]);
        }
        Enumeration e5 = rr2.getExceptDates();
        e5.nextElement();   // just skip the first element...

        if (e5.hasMoreElements())
        {
            failTest("rr2.addExceptDate() can be used to add the same date several times!");
        }

        // all is well since we get this far
        passTest();
    }

    private boolean containsLong(long[] array, long val)
    {
        for (int i=0; i<array.length; i++)
        {
            if (array[i] == val)
            {
                return true;
            }
        }
        return false;
    }

    private boolean containsExceptDate(RepeatRule rr, long val)
    {
        Enumeration e = rr.getExceptDates();
        while (e.hasMoreElements())
        {
            long tmp = ((Date)e.nextElement()).getTime();
            if (val == tmp)
            {
                return true;
            }
        }
        return false;
    }
}