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
package com.nokia.mj.test.pim.automatic.ContactList.CLsupportedAttributes;
// IMPORTS
import javax.microedition.pim.*;
import java.util.Hashtable;
import com.nokia.mj.test.pim.automatic.*;
import com.nokia.mj.test.pim.adaptation.*;
// CLASS DEFINITION
/**
 * Tests supported attributes of the contact list
 *
 * Reference for supported attribute values was only
 * the source code because the specification wasn't approved
 * yet and it had misinformation about some attributes.
 *
 * ATTENTION: Update supported attributes when
 * the PIM API specification is approved
 *
 */

public class CLsupportedAttributes
        extends MIDPTest
{

    public CLsupportedAttributes(
        String sTestName, j2meunit.framework.TestMethod rTestMethod)
    {
        super(sTestName, rTestMethod);
    }

    /* PIM List */
    private PIMList iPIMList;

    /* Supported attributes test data */
    private SupportTestData iTestData = new SupportTestData();

    /**
     * Initializes the test
     */
    public void initTest()
    throws TestEndThrowable
    {
        // Initialize test data
        iTestData.addSupportedAttributes(Contact.NICKNAME       , Contact.ATTR_NONE);
        iTestData.addSupportedAttributes(Contact.NAME           , Contact.ATTR_NONE);
        iTestData.addSupportedAttributes(Contact.ORG            , Contact.ATTR_NONE);
        iTestData.addSupportedAttributes(Contact.TITLE          , Contact.ATTR_NONE);
        iTestData.addSupportedAttributes(Contact.BIRTHDAY       , Contact.ATTR_NONE);
        iTestData.addSupportedAttributes(Contact.NOTE           , Contact.ATTR_NONE);
        iTestData.addSupportedAttributes(Contact.PHOTO          , Contact.ATTR_NONE);
        iTestData.addSupportedAttributes(Contact.UID            , Contact.ATTR_NONE);
        iTestData.addSupportedAttributes(Contact.REVISION       , Contact.ATTR_NONE);
        iTestData.addSupportedAttributes(0x1005002              , Contact.ATTR_NONE);   // SIP
        iTestData.addSupportedAttributes(0x1005003              , Contact.ATTR_NONE);   // DTMF
        iTestData.addSupportedAttributes(0x1005001              , Contact.ATTR_NONE);   // WV ID
        iTestData.addSupportedAttributes(0x1005004              , Contact.ATTR_NONE);   // Department
        iTestData.addSupportedAttributes(0x1005005              , Contact.ATTR_NONE);   // Assistant name
        iTestData.addSupportedAttributes(0x1005006              , Contact.ATTR_NONE);   // Children
        iTestData.addSupportedAttributes(0x1005007              , Contact.ATTR_NONE);   // Spouse
        iTestData.addSupportedAttributes(0x1005008              , Contact.ATTR_NONE);   // Anniversary
        iTestData.addSupportedAttributes(Contact.CLASS          , Contact.ATTR_NONE);

        iTestData.addSupportedAttributes(Contact.ADDR,
                                         Contact.ATTR_HOME | Contact.ATTR_WORK);

        iTestData.addSupportedAttributes(Contact.TEL,
                                         Contact.ATTR_HOME   | Contact.ATTR_WORK |
                                         Contact.ATTR_MOBILE | Contact.ATTR_FAX  |
                                         Contact.ATTR_PAGER  | Contact.ATTR_PREFERRED |
                                         16777216            | 1 |   // VIDEO_CALL, ASST, AUTO
                                         2 | Contact.ATTR_OTHER  | Contact.ATTR_SMS);

        iTestData.addSupportedAttributes(Contact.EMAIL,
                                         Contact.ATTR_HOME | Contact.ATTR_WORK | Contact.ATTR_PREFERRED);

        iTestData.addSupportedAttributes(Contact.URL,
                                         Contact.ATTR_HOME | Contact.ATTR_WORK);

        // VOIP extended field
        iTestData.addSupportedAttributes(0x1005009,
                                         Contact.ATTR_HOME | Contact.ATTR_WORK | Contact.ATTR_PREFERRED);

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
        String err = PLsupportTester.testSupportedAttributes(
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

