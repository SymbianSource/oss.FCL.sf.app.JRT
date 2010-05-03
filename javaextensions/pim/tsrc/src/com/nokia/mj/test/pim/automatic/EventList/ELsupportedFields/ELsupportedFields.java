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
package com.nokia.mj.test.pim.automatic.EventList.ELsupportedFields;
// IMPORTS
import javax.microedition.pim.*;
import java.util.Hashtable;
import java.util.Vector;
import com.nokia.mj.test.pim.automatic.*;
import com.nokia.mj.test.pim.adaptation.*;

// CLASS DEFINITION
/**
 * Tests the event list's supported fields
 * Remember to test all event lists separately
 * (for 3.0 there are Appoinments, Events and Anniversaries)
 *
 * Reference for supported field values in this point wwas
 * the source code because the specification wasn't approved
 * yet and it had misinformation about some fields.
 *
 * ATTENTION: Update supported and unsupported fields when
 * the PIM API specification is approved
 */

public class ELsupportedFields
        extends MIDPTest
{

    public ELsupportedFields(
        String sTestName, j2meunit.framework.TestMethod rTestMethod)
    {
        super(sTestName, rTestMethod);
    }

    /* PIM lists */
    private String[] iListNames;
    private PIMList iAptList = null;
    private PIMList iEventList = null;
    private PIMList iAnvList = null;

    /* Test data including supported field values */
    private SupportTestData iSupportedAptFields = new SupportTestData();
    private SupportTestData iSupportedEventFields = new SupportTestData();
    private SupportTestData iSupportedAnvFields = new SupportTestData();

    /**
     * Initializes the test
     */
    public void initTest()
    throws TestEndThrowable
    {
        // Initialize supported fields. Appointments
        iSupportedAptFields.addSupportedField(Event.SUMMARY);
        iSupportedAptFields.addSupportedField(Event.NOTE);
        iSupportedAptFields.addSupportedField(Event.LOCATION);
        iSupportedAptFields.addSupportedField(Event.START);
        iSupportedAptFields.addSupportedField(Event.END);
        iSupportedAptFields.addSupportedField(Event.ALARM);
        iSupportedAptFields.addSupportedField(Event.UID);
        iSupportedAptFields.addSupportedField(Event.CLASS);
        iSupportedAptFields.addSupportedField(Event.REVISION);

        // Memo (Event) event's fields
        iSupportedEventFields.addSupportedField(Event.SUMMARY);
        iSupportedEventFields.addSupportedField(Event.NOTE);
        iSupportedEventFields.addSupportedField(Event.START);
        iSupportedEventFields.addSupportedField(Event.END);
        iSupportedEventFields.addSupportedField(Event.ALARM);
        iSupportedEventFields.addSupportedField(Event.UID);
        iSupportedEventFields.addSupportedField(Event.CLASS);
        iSupportedEventFields.addSupportedField(Event.REVISION);
        // Unsupported fields
        iSupportedEventFields.addUnsupportedField(Event.LOCATION);
        // Anniversary event's fields
        iSupportedAnvFields.addSupportedField(Event.SUMMARY);
        iSupportedAnvFields.addSupportedField(Event.NOTE);
        iSupportedAnvFields.addSupportedField(Event.START);
        iSupportedAnvFields.addSupportedField(Event.ALARM);
        iSupportedAnvFields.addSupportedField(Event.UID);
        iSupportedAnvFields.addSupportedField(Event.CLASS);
        iSupportedAnvFields.addSupportedField(Event.REVISION);
        // Unsupported fields
        iSupportedAnvFields.addUnsupportedField(Event.LOCATION);
        iSupportedAnvFields.addUnsupportedField(Event.END);

        // Initialize event lists
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
        String err = null;
        log("Running test.");

        // Test anniversary's fields
        log("Testing " + iListNames[ 2 ] + " list ");
        err = PLsupportTester.testSupportedFields(
                  iAnvList,
                  iSupportedAnvFields);
        CheckError(err);
        log(iListNames[ 2 ] + " list tested ");

        // Test appointment's fields
        log("Testing " + iListNames[ 0 ] + " list ");
        err = PLsupportTester.testSupportedFields(
                  iAptList,
                  iSupportedAptFields);
        CheckError(err);
        log(iListNames[ 0 ] + " list tested ");

        // Test event's fields
        log("Testing " + iListNames[ 1 ] + " list ");
        err = PLsupportTester.testSupportedFields(
                  iEventList,
                  iSupportedEventFields);
        CheckError(err);
        log(iListNames[ 1 ] + " list tested ");

        log("Test has been run");
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

