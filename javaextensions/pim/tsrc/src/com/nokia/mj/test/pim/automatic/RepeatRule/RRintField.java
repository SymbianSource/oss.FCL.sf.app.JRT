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
package com.nokia.mj.test.pim.automatic.RepeatRule;
//IMPORTS
import java.io.IOException;
import java.util.*;
import javax.microedition.pim.RepeatRule;
import javax.microedition.pim.FieldEmptyException;
import com.nokia.mj.test.pim.adaptation.*;

// CLASS DEFINITION
/**
 * This class tests javax.microedition.pim.RepeatRule.getInt().
 */
public class RRintField extends MIDPTest
{

    public RRintField(
        String sTestName, j2meunit.framework.TestMethod rTestMethod)
    {
        super(sTestName, rTestMethod);
    }

    private RepeatRule rr = new RepeatRule();

    protected int field = 0;
    protected int defaultVal = -1;
    protected int[] valids = null;
    protected int[] invalids = null;

    public void runTest() throws TestEndThrowable
    {
        initTest();
        // test that created repeatrule has not the given field already set
        if (!contains(rr.getFields(), field))
        {
            // test that getting field value when it is not set causes
            // FieldEmptyException to be thrown
            try
            {
                int val = rr.getInt(field);
                failTest("rr.getInt( "+field+" ) returned a value when no value is set!");
            }
            catch (FieldEmptyException e)
            {
                // ok
            }
            catch (Exception e)
            {
                failTest("rr.getInt( "+field+" ) threw Exception: " + e);
            }
        }
        // getFields() returned tested field in constructed RR, this
        // may be a special case, which has a default value set
        else
        {
            if (defaultVal != -1)
            {
                // test that the default value it is correct
                testDefaultValue(field, defaultVal);
            }
            else
            {
                failTest("Constructed RR has "+field+" field set!");
            }
        }

        // test that setting field to invalid value throws IAE
        testInvalidValues(field, invalids);


        // test that setting field to valid value works ok
        testValidValues(field, valids);

        passTest();
        cleanupTest();
    }

    private void testDefaultValue(int field, int defaultVal) throws TestEndThrowable
    {
        try
        {
            int val = rr.getInt(field);

            if (val != defaultVal)
            {
                failTest("rr.getInt( "+field+" ) should return "+
                         defaultVal + " but returned "+val+"!");
            }
        }
        catch (FieldEmptyException e)
        {
            failTest("rr.getInt( "+field+" ) threw FEE when it should"+
                     "return value "+defaultVal+"!");
        }
        catch (Exception e)
        {
            failTest("rr.getInt( "+field+" ) threw Exception: "+e +
                     " when it should return value "+defaultVal+"!");
        }
    }

    private void testInvalidValues(int field, int[] values) throws TestEndThrowable
    {
        for (int i = 0; i < values.length; i++)
        {
            try
            {
                rr.setInt(field, values[i]);
                failTest("rr.setInt( "+field+", "+values[i]+" ) did not throw any Exception!");
            }
            catch (IllegalArgumentException e)
            {
                // ok
            }
            catch (Exception e)
            {
                failTest("rr.setInt( "+field+", "+values[i]+" ) threw Exception: "+e);
            }
        }
    }

    private void testValidValues(int field, int[] values) throws TestEndThrowable
    {
        for (int i = 0; i < values.length; i++)
        {
            try
            {
                rr.setInt(field, values[i]);
            }
            catch (Exception e)
            {
                failTest("rr.setInt( "+field+", "+values[i]+" ) threw Exception: "+e);
            }
            try
            {
                int val = rr.getInt(field);

                if (val != values[i])
                {
                    failTest("rr.getInt("+field+") did not return the value that was set!");
                }
            }
            catch (Exception e)
            {
                failTest("rr.getInt( "+field+") threw Exception: " + e);
            }
        }
    }

    // returns true if field is one of values in fields[]
    private boolean contains(int[] fields, int field)
    {
        for (int i=0; i < fields.length; i++)
        {
            if (field == fields[i])
            {
                return true;
            }
        }
        return false;
    }

    protected void initTest() throws TestEndThrowable
    {
        log("RRIntField::initTest()");
    }

    protected void cleanupTest()
    {
        log("RRIntField::cleanupTest()");
    }
}