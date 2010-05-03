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
package com.nokia.mj.test.pim.automatic.EventList.ELfieldsOrder;
// IMPORTS
import javax.microedition.pim.*;
import java.util.Vector;
import com.nokia.mj.test.pim.automatic.*;
import com.nokia.mj.test.pim.adaptation.*;

// CLASS DEFINITION
/**
 * Tests that fields of the event lists are in right order
 *
 * Reference for supported field values in this point wwas
 * the source code because the specification wasn't approved
 * yet and it had misinformation about some fields. Order was
 * also checked from the emulator
 *
 * ATTENTION: Update fields if needed when the API specification
 * is approved
 */

public class ELfieldsOrder
        extends MIDPTest
{

    public ELfieldsOrder(
        String sTestName, j2meunit.framework.TestMethod rTestMethod)
    {
        super(sTestName, rTestMethod);
    }

    /* PIM lists */
    private PIMList iAptList = null;
    private PIMList iEventList = null;
    private PIMList iAnvList = null;

    /* List names */
    private String[] iListNames = null;

    /* Test data containing supported field values in right order */
    private SupportTestData iOrderedAptFields = new SupportTestData();
    private SupportTestData iOrderedEventFields = new SupportTestData();
    private SupportTestData iOrderedAnvFields = new SupportTestData();

    /**
     * Initializes the test
     */
    public void initTest()
    throws TestEndThrowable
    {
        // Initialize supported fields list. Check that the
        // order is the same as in the native side event list
        iOrderedAptFields.addSupportedField(Event.SUMMARY);
        iOrderedAptFields.addSupportedField(Event.NOTE);
        iOrderedAptFields.addSupportedField(Event.LOCATION);
        iOrderedAptFields.addSupportedField(Event.START);
        iOrderedAptFields.addSupportedField(Event.END);
        iOrderedAptFields.addSupportedField(Event.ALARM);
        iOrderedAptFields.addSupportedField(Event.CLASS);
        iOrderedAptFields.addSupportedField(Event.UID);
        iOrderedAptFields.addSupportedField(Event.REVISION);

        // Ordered event fields
        iOrderedEventFields.addSupportedField(Event.SUMMARY);
        iOrderedEventFields.addSupportedField(Event.NOTE);
        iOrderedEventFields.addSupportedField(Event.START);
        iOrderedEventFields.addSupportedField(Event.END);
        iOrderedEventFields.addSupportedField(Event.ALARM);
        iOrderedEventFields.addSupportedField(Event.CLASS);
        iOrderedEventFields.addSupportedField(Event.UID);
        iOrderedEventFields.addSupportedField(Event.REVISION);

        // Ordered anniversary fields
        iOrderedAnvFields.addSupportedField(Event.SUMMARY);
        iOrderedAnvFields.addSupportedField(Event.NOTE);
        iOrderedAnvFields.addSupportedField(Event.START);
        iOrderedAnvFields.addSupportedField(Event.ALARM);
        iOrderedAnvFields.addSupportedField(Event.CLASS);
        iOrderedAnvFields.addSupportedField(Event.UID);
        iOrderedAnvFields.addSupportedField(Event.REVISION);

        // Initialize contact list
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
        log("Running test.");
        // Test appointment's fields
        log("Testing " + iAptList.getName() + " list ");
        String err = PLsupportTester.testFieldsOrder(
                         iAptList,
                         iOrderedAptFields);
        CheckError(err);
        log(iAptList.getName() + " list tested ");

        // Test event's fields
        log("Testing " + iEventList.getName() + " list ");
        err = PLsupportTester.testFieldsOrder(
                  iEventList,
                  iOrderedEventFields);
        CheckError(err);
        log(iEventList.getName() + " list tested ");

        // Test anniversary's fields
        log("Testing " + iAnvList.getName() + " list ");
        err = PLsupportTester.testFieldsOrder(
                  iAnvList,
                  iOrderedAnvFields);
        CheckError(err);
        log(iAnvList.getName() + " list tested ");

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
            iAptList.close();
            iEventList.close();
            iAnvList.close();
        }
        catch (PIMException pe)
        {
            // cannot call failTest
            log("Unexpected PIMException in cleanupTest().");
            pe.printStackTrace();
        }
    }
    /**
     *  Helper function
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

