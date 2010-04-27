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
package com.nokia.mj.test.pim.automatic.ContactList.CLfieldsOrder;
// IMPORTS
import javax.microedition.pim.*;
import java.util.Vector;
import com.nokia.mj.test.pim.automatic.*;
import com.nokia.mj.test.pim.adaptation.*;

// CLASS DEFINITION
/**
 * Tests that fields of the contact list are in right order
 *
 * Reference for supported field values in this point wwas
 * the source code because the specification wasn't approved
 * yet and it had misinformation about some fields. Order was
 * also checked from the emulator
 *
 * ATTENTION: Update fields if needed when the API specification
 * is approved
 *
 */

public class CLfieldsOrder
        extends MIDPTest
{

    public CLfieldsOrder(
        String sTestName, j2meunit.framework.TestMethod rTestMethod)
    {
        super(sTestName, rTestMethod);
    }

    /* PIM List */
    private PIMList iPIMList;

    /* Test data containing supported field values in right order */
    private SupportTestData iOrderedFieldsData = new SupportTestData();

    /**
     * Initializes the test
     */
    public void initTest()
    throws TestEndThrowable
    {
        // Initialize supported fields list. Check that the
        // order is the same as in the native side contact list
        iOrderedFieldsData.addSupportedField(Contact.PHOTO);
        iOrderedFieldsData.addSupportedField(Contact.NAME);
        iOrderedFieldsData.addSupportedField(Contact.NICKNAME);
        iOrderedFieldsData.addSupportedField(Contact.TEL);
        iOrderedFieldsData.addSupportedField(Contact.EMAIL);
        iOrderedFieldsData.addSupportedField(0x1005003);                // DTMF
        iOrderedFieldsData.addSupportedField(0x1005009);                // VOIP
        iOrderedFieldsData.addSupportedField(0x100500A);                // PTT
        iOrderedFieldsData.addSupportedField(0x100500B);                // SWIS
        iOrderedFieldsData.addSupportedField(0x1005002);                // SIP
        iOrderedFieldsData.addSupportedField(Contact.ORG);
        iOrderedFieldsData.addSupportedField(0x1005004);                // Department
        iOrderedFieldsData.addSupportedField(Contact.TITLE);
        iOrderedFieldsData.addSupportedField(0x1005005);                // Assistant name
        iOrderedFieldsData.addSupportedField(0x1005007);                // Spouse
        iOrderedFieldsData.addSupportedField(0x1005006);                // Children
        iOrderedFieldsData.addSupportedField(Contact.URL);
        iOrderedFieldsData.addSupportedField(Contact.ADDR);
        iOrderedFieldsData.addSupportedField(Contact.BIRTHDAY);
        iOrderedFieldsData.addSupportedField(0x1005008);                // Anniversary
        iOrderedFieldsData.addSupportedField(Contact.NOTE);
        iOrderedFieldsData.addSupportedField(Contact.CLASS);
        iOrderedFieldsData.addSupportedField(0x1005001);                // WV ID
        iOrderedFieldsData.addSupportedField(Contact.UID);
        iOrderedFieldsData.addSupportedField(Contact.REVISION);

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
        String err = PLsupportTester.testFieldsOrder(
                         iPIMList,
                         iOrderedFieldsData);
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

