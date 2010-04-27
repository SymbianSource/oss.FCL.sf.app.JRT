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


// PACKAGE

package com.nokia.mj.test.pim.automatic.EventList.ELsupportedRRFields2;
// IMPORTS
import javax.microedition.pim.*;
import java.util.Hashtable;
import com.nokia.mj.test.pim.automatic.*;
import com.nokia.mj.test.pim.adaptation.*;

// CLASS DEFINITION
/**
 * Tests the event list's supported repeat rule fields
 * for all three event lists
 *
 * Reference for supported field values in this point was
 * the source code because the specification wasn't approved
 * yet and it had misinformation about some fields.
 *
 * ATTENTION: Update supported and unsupported fields when
 * the PIM API specification is approved
 *
 */

public class ELsupportedRRFields2
        extends MIDPTest
{

    public ELsupportedRRFields2(
        String sTestName, j2meunit.framework.TestMethod rTestMethod)
    {
        super(sTestName, rTestMethod);
    }

    /* PIM lists */
    private String[] iListNames;
    private PIMList iAptList = null;
    private PIMList iEventList = null;
    private PIMList iAnvList = null;

    /* Test dataincluding supported repeat rule field values */
    private SupportTestData iAptTestData = new SupportTestData();
    private SupportTestData iAnvTestData = new SupportTestData();
    private SupportTestData iEventTestData = new SupportTestData();

    /**
     * Initializes the test
     */
    public void initTest()
    throws TestEndThrowable
    {
        // NOTE: We can use supported attribute functions, thus they provide the same
        // data structure that repeat rule fields
        iAptTestData.addSupportedAttributes(RepeatRule.DAILY  , RepeatRule.FREQUENCY);
        iAptTestData.addSupportedAttributes(RepeatRule.MONTHLY, RepeatRule.FREQUENCY);
        iAptTestData.addSupportedAttributes(RepeatRule.YEARLY , RepeatRule.FREQUENCY);
        iAptTestData.addSupportedAttributes(RepeatRule.WEEKLY , RepeatRule.FREQUENCY |
                                            RepeatRule.INTERVAL);
        // -1 means no repeat rules
        iEventTestData.addSupportedAttributes(RepeatRule.DAILY  , -1);
        iEventTestData.addSupportedAttributes(RepeatRule.MONTHLY, -1);
        iEventTestData.addSupportedAttributes(RepeatRule.YEARLY , -1);
        iEventTestData.addSupportedAttributes(RepeatRule.WEEKLY , -1);

        iAnvTestData.addSupportedAttributes(RepeatRule.DAILY  , -1);
        iAnvTestData.addSupportedAttributes(RepeatRule.MONTHLY, -1);
        iAnvTestData.addSupportedAttributes(RepeatRule.YEARLY , RepeatRule.FREQUENCY);
        iAnvTestData.addSupportedAttributes(RepeatRule.WEEKLY , -1);

        // Initialize Event list
        try
        {
            PIM pim = PIM.getInstance();
            iListNames = pim.listPIMLists(PIM.EVENT_LIST);
            // Open appointments lists
            iAptList = pim.openPIMList(
                           PIM.EVENT_LIST,
                           PIM.READ_WRITE,
                           iListNames[ 0 ]);
            // Open events list
            iEventList = pim.openPIMList(
                             PIM.EVENT_LIST,
                             PIM.READ_WRITE,
                             iListNames[ 1 ]);
            // Open anniversaries list
            iAnvList = pim.openPIMList(
                           PIM.EVENT_LIST,
                           PIM.READ_WRITE,
                           iListNames[ 2 ]);

        }
        catch (PIMException pe)
        {
            failTest("Unexpected PIMException in initTest().");
            pe.printStackTrace();
        }
    }
    /**
     * Runs the test
     */
    public void runTest()
    throws TestEndThrowable
    {
        initTest();
        String error = null;
        log("Running test.");

        // Test appointment's fields
        log("Testing " + iListNames[ 0 ] + " list ");
        error = PLsupportTester.testSupportedRRFields(
                    iAptList,
                    iAptTestData);
        CheckError(error);
        log(iListNames[ 0 ] + " list tested ");

        // Test event's fields
        log("Testing " + iListNames[ 1 ] + " list ");
        error = PLsupportTester.testSupportedRRFields(
                    iEventList,
                    iEventTestData);
        CheckError(error);
        log(iListNames[ 1 ] + " list tested ");

        // Test anniversary's fields
        log("Testing " + iListNames[ 2 ] + " list ");
        error = PLsupportTester.testSupportedRRFields(
                    iAnvList,
                    iAnvTestData);
        CheckError(error);
        log(iListNames[ 2 ] + " list tested ");

        log("Test finished.");
        passTest();
        cleanupTest();
    }
    /**
     * Removes the test items.
     */
    public void cleanupTest()
    {
        try
        {
            iAnvList.close();
            iAptList.close();
            iEventList.close();
        }
        catch (PIMException pe)
        {
            // cannot call failTest
            log("Unexpected PIMException in cleanupTest().");
            pe.printStackTrace();
        }
    }

    /**
     *  Assistance method
     */
    private void CheckError(String aError)
    throws TestEndThrowable
    {
        // Fail if error string was returned
        if (aError != null)
        {
            failTest(aError);
        }
    }
}

// End of file

