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
package com.nokia.mj.test.pim.automatic.ContactList.CLsupportedFields;
// IMPORTS
import javax.microedition.pim.*;
import com.nokia.mj.test.pim.automatic.*;
import com.nokia.mj.test.pim.adaptation.*;
// CLASS DEFINITION
/**
 * Tests the supported fields of the contact list
 *
 * Reference for supported field values in this point wwas
 * the source code because the specification wasn't approved
 * yet and it had misinformation about some fields.
 *
 * ATTENTION: Update supported and unsupported fields when
 * the PIM API specification is approved
 *
 */

public class CLsupportedFields
        extends MIDPTest
{

    public CLsupportedFields(
        String sTestName, j2meunit.framework.TestMethod rTestMethod)
    {
        super(sTestName, rTestMethod);
    }

    /* PIM List */
    private PIMList iPIMList;

    /* Support test data */
    private SupportTestData iTestData = new SupportTestData();

    /**
     * Initializes the test
     */
    public void initTest()
    throws TestEndThrowable
    {

        // Initialize supported fields
        iTestData.addSupportedField(Contact.NICKNAME);
        iTestData.addSupportedField(Contact.NAME);
        iTestData.addSupportedField(Contact.ORG);
        iTestData.addSupportedField(Contact.TITLE);
        iTestData.addSupportedField(Contact.ADDR);
        iTestData.addSupportedField(Contact.TEL);
        iTestData.addSupportedField(Contact.EMAIL);
        iTestData.addSupportedField(Contact.URL);
        iTestData.addSupportedField(Contact.BIRTHDAY);
        iTestData.addSupportedField(Contact.NOTE);
        iTestData.addSupportedField(Contact.PHOTO);
        iTestData.addSupportedField(Contact.UID);
        iTestData.addSupportedField(Contact.REVISION);
        iTestData.addSupportedField(0x1005002);                // SIP
        iTestData.addSupportedField(0x1005003);                // DTMF
        iTestData.addSupportedField(0x1005001);                // WV ID
        iTestData.addSupportedField(0x1005004);                // Department
        iTestData.addSupportedField(0x1005005);                // Assistant name
        iTestData.addSupportedField(0x1005006);                // Children
        iTestData.addSupportedField(0x1005007);                // Spouse
        iTestData.addSupportedField(0x1005008);                // Anniversary
        iTestData.addSupportedField(0x1005009);                // VOIP
        iTestData.addSupportedField(Contact.CLASS);
        iTestData.addSupportedField(0x100500A);                // PTT
        iTestData.addSupportedField(0x100500B);                // SWIS

        // Initialize contact list
        try
        {
            PIM pim = PIM.getInstance();

            iPIMList = pim.openPIMList(
                           PIM.CONTACT_LIST,
                           PIM.READ_WRITE);
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
        String err = PLsupportTester.testSupportedFields(
                         iPIMList,
                         iTestData);
        if (err != null)
        {
            failTest(err);
        }
        else
        {
            passTest();
        }
        cleanupTest();
    }
    /**
     * Removes the test items.
     */
    public void cleanupTest()
    {
        try
        {
            iPIMList.close();
        }
        catch (PIMException pe)
        {
            // cannot call failTest
            log("Unexpected PIMException in cleanupTest().");
            pe.printStackTrace();
        }
    }
}

// End of file

