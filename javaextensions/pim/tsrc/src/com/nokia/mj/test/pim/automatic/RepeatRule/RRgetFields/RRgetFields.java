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
package com.nokia.mj.test.pim.automatic.RepeatRule.RRgetFields;
//IMPORTS
import java.io.IOException;
import java.util.*;
import javax.microedition.pim.RepeatRule;
import com.nokia.mj.test.pim.adaptation.*;

// CLASS DEFINITION
/**
 * This class tests javax.microedition.pim.RepeatRule.getDate().
 */
public class RRgetFields extends MIDPTest
{

    public RRgetFields(
        String sTestName, j2meunit.framework.TestMethod rTestMethod)
    {
        super(sTestName, rTestMethod);
    }

    public void runTest() throws TestEndThrowable
    {
        // Tested fields
        int[] fields = { 0, 1, 2, 4, 8, 16, 32, 64, 128 };

        // Valid value for each field
        int[] valids = { 16, 1, 1024, 100, 131072, 256, 1, 9466776, 2 };

        // Invalid value for each field (note exception on field 64 (END))
        int[] invalids = { 15, -1, 1, 0, 4096, -1, -1, 0, -1 };

        // Create the test object
        RepeatRule rr = new RepeatRule();

        // Ensure that the test object is empty
        if (rr.getFields().length != 0)
        {
            failTest("rr.getFields() returned some fields from new, " +
                     "empty RepeatRule.");
        }

        // Ensure that no invalid values are accepted
        trySetInvalidValues(rr, fields, invalids);

        // Ensure that no invalid values were set
        if (rr.getFields().length != 0)
        {
            failTest("rr.getFields() returned some fields after trying to " +
                     "set invalid values.");
        }

        // Set a valid value for each field
        setValidValues(rr, fields, valids);

        // Ensure that each field has a value
        if (rr.getFields().length != fields.length)
        {
            failTest("rr.getFields() did not return all fields after " +
                     "setting valid values.");
        }

        // All clear
        passTest();
    }


    /**
     * Tries to set invalid values to RepeatRule fields.
     * IllegalArgumentException is expected for each field.
     * @param aFields The fields.
     * @param aInvalidValues Invalid value for each field.
     *        NOTE: value for field RepeatRule.END (64) is ignored.
     *        The value must be large negative long, and it is set
     *        internally.
     */
    protected void trySetInvalidValues(
        RepeatRule aRepeatRule,
        int[] aFields,
        int[] aInvalidValues)
    throws TestEndThrowable
    {
        for (int i=0; i < aFields.length; i++)
        {
            if (aFields[i] == RepeatRule.END)
            {
                try
                {
                    // Try to set invalid value.

                    // Here's something little that needs to be explained.
                    // We need a large negative to represent a date far
                    // beyond year 1900, which is the start of time
                    // for Symbian Agenda Model. Long.MIN_VALUE would be
                    // OK if it wasn't converted from milliseconds to
                    // microseconds in the native side, overflowing the
                    // variable. Thus we divide it with 10k to get a
                    // suitable number.
                    // Note that this overrides the 0 from properties.txt
                    // for field 64 (RepeatRule.END).
                    long largeNegative = Long.MIN_VALUE / 10000;
                    aRepeatRule.setDate(aFields[i], largeNegative);
                    failTest("aRepeatRule.setDate("
                             +aFields[i]+","+largeNegative+") on "+
                             "RepeatRule did not throw IAE!");
                }
                catch (IllegalArgumentException iae)
                {
                    // OK, as expected
                }
                catch (Exception e)
                {
                    // Unexpected exception
                    failTest("aRepeatRule.setDate("
                             +aFields[i]+","+aInvalidValues[i]+") on "+
                             "RepeatRule did not throw IAE but: "+e);
                }
            }
            else // != RepeatRule.END
            {
                try
                {
                    // Try to set invalid value
                    aRepeatRule.setInt(aFields[i], aInvalidValues[i]);
                    failTest("aRepeatRule.setInt("
                             +aFields[i]+","+aInvalidValues[i]+") on "+
                             "RepeatRule did not throw IAE!");
                }
                catch (IllegalArgumentException iae)
                {
                    // OK, as expected
                }
                catch (Exception e)
                {
                    // Unexpected exception
                    failTest("aRepeatRule.setInt("
                             +aFields[i]+","+aInvalidValues[i]+") on "+
                             "RepeatRule did not throw IAE but: "+e);
                }
            }
        }
    }

    /**
     * Sets valid values to fields in a RepeatRule. Fails on exception.
     * @param aRepeatRule The RepeatRule.
     * @param aFields The fields to set.
     * @param aValues The values to set for each field.
     */
    protected void setValidValues(
        RepeatRule aRepeatRule,
        int[] aFields,
        int[] aValues)
    throws TestEndThrowable
    {
        // test that after setting all fields to valid value
        // getFields() returns all fields
        for (int i=0; i < aFields.length; i++)
        {
            if (aFields[i] == RepeatRule.END)
            {
                // Make it long and large.
                long tmp = (long)(aValues[i] * 100000L);
                try
                {
                    aRepeatRule.setDate(aFields[i], tmp);
                }
                catch (Exception e)
                {
                    failTest("aRepeatRule.setDate("
                             +aFields[i]+","+tmp+") on "+
                             "RepeatRule threw: "+e);
                }
            }
            else
            {
                try
                {
                    aRepeatRule.setInt(aFields[i], aValues[i]);
                }
                catch (Exception e)
                {
                    failTest("aRepeatRule.setInt("
                             +aFields[i]+","+aValues[i]+") on "+
                             "RepeatRule threw: "+e);
                }
            }
        }
    }
}