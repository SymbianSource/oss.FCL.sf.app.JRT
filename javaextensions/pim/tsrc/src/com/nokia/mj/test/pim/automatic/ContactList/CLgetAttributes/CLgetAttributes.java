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
package com.nokia.mj.test.pim.automatic.ContactList.CLgetAttributes;
// IMPORTS
import javax.microedition.pim.*;
import java.util.Hashtable;
import java.util.Vector;
import com.nokia.mj.test.pim.automatic.*;
import com.nokia.mj.test.pim.adaptation.*;
// CLASS DEFINITION
/**
 * Tests attribute combinations of contact list fields
 *
 * Reference for supported field values in this point was
 * the source code because the specification wasn't approved
 * yet and it had misinformation about some fields.
 *
 * ATTENTION: Update test when the specification is valid
 *
 */

public class CLgetAttributes
        extends MIDPTest
{

    public CLgetAttributes(
        String sTestName, j2meunit.framework.TestMethod rTestMethod)
    {
        super(sTestName, rTestMethod);
    }

    /* PIM List and item */
    private PIMList iPIMList;
    private PIMItem iPIMItem;

    /* Test data including fields, attributes and field values */
    private SupportTestData iTestData = new SupportTestData();

    /**
     * Initializes the test
     */
    public void initTest()
    throws TestEndThrowable
    {
        String[] nameArrayValues = { "Pre", "Suf", "First", "Last", "Middle", "Read", "Read" };
        String[] addrArrayValues = { "P.O box", "LPR", null, null, null, null, null };
        String binaryString = "R0lGODdhAQABAPYAAAAAAP////8AAP8UAP8sAP9FAP9dAP91AP+OAP+mAP++AP/XAP/vAPf/\r\nAN//AMf/AK7/AJb/AH3/AGX/AE3/ADT/AAT/AAD/DAD/JAD/PAD/VQD/bQD/hgD/ngD/tgD/\r\nzwD/5wD//wDn/wDP/wC2/wCe/wCG/wBt/wBV/wA8/wAk/wAM/wQA/xwA/zQA/00A/2UA/30A\r\n/5YA/64A/8cA/98A//cA//8A7/8A1/8Avv8Apv8Ajv8Adf8AXf8ARf8ALP8AFMPDw89dXX19\r\nfXV1dQAAgoKCgv//AFFR+/sUQWFhYZqamjz7NAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA\r\nAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA\r\nAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA\r\nAAAAAAAAAAAAAAAAAAAAAAAAACwAAAAAAQABAAAHA4AFgQA7";
        String testString = "TestString";

        // Contact.EMAIL field. 1st field type, 2nd attributes, 3rd value, 4th versit conversion result
        iTestData.addFieldData(Contact.EMAIL, 0,                 "testing@domain.com", null);
        iTestData.addFieldData(Contact.EMAIL, Contact.ATTR_HOME, "testing@home.com",   null);
        iTestData.addFieldData(Contact.EMAIL, Contact.ATTR_WORK, "testing@work.com",   null);

        // Contact.TEL field
        iTestData.addFieldData(Contact.TEL, 0,                     "1234567890", null);
        iTestData.addFieldData(Contact.TEL, Contact.ATTR_HOME,     "1234567890", null);
        iTestData.addFieldData(Contact.TEL, Contact.ATTR_MOBILE,   "1234567890", null);
        iTestData.addFieldData(Contact.TEL, Contact.ATTR_FAX,      "1234567890", null);
        iTestData.addFieldData(Contact.TEL, Contact.ATTR_PAGER,    "1234567890", null);

        iTestData.addFieldData(Contact.TEL, Contact.ATTR_MOBILE |
                               Contact.ATTR_WORK,     "1234567890", null);

        iTestData.addFieldData(Contact.TEL, Contact.ATTR_MOBILE |
                               Contact.ATTR_HOME,     "1234567890", null);

        iTestData.addFieldData(Contact.TEL, Contact.ATTR_FAX |
                               Contact.ATTR_HOME,     "1234567890", null);

        iTestData.addFieldData(Contact.TEL, Contact.ATTR_FAX |
                               Contact.ATTR_WORK,     "1234567890", null);

        // Video call attributes
        iTestData.addFieldData(Contact.TEL, 16777216,              "1234567890", null);

        iTestData.addFieldData(Contact.TEL, 16777216 |
                               Contact.ATTR_HOME,     "1234567890", null);

        iTestData.addFieldData(Contact.TEL, 16777216 |
                               Contact.ATTR_WORK,     "1234567890", null);

        // Assistant phone
        iTestData.addFieldData(Contact.TEL, 1,                     "1234567890", null);
        // Car phone
        iTestData.addFieldData(Contact.TEL, 2,                     "1234567890", null);
        // Preferred SMS
        iTestData.addFieldData(Contact.TEL, Contact.ATTR_SMS,      "1234567890", null);
        // VOIP number
        iTestData.addFieldData(0x1005009, 0,                 testString, null);
        iTestData.addFieldData(0x1005009, Contact.ATTR_HOME, testString, null);
        iTestData.addFieldData(0x1005009, Contact.ATTR_WORK, testString, null);

        // Fields that have no attributes
        iTestData.addFieldData(Contact.URL,      0, testString,      null);
        iTestData.addFieldData(Contact.NOTE,     0, testString,      null);
        iTestData.addFieldData(Contact.NAME,     0, nameArrayValues, null);
        iTestData.addFieldData(Contact.ORG,      0, testString,      null);
        iTestData.addFieldData(Contact.TITLE,    0, testString,      null);
        iTestData.addFieldData(Contact.BIRTHDAY, 0, 130293923,       null);
        iTestData.addFieldData(Contact.ADDR,     0, addrArrayValues, null);
        iTestData.addFieldData(Contact.PHOTO,    0, binaryString,    null);
        iTestData.addFieldData(Contact.NICKNAME, 0, testString,      null);
        iTestData.addFieldData(0x1005002,        0, testString,      null);
        iTestData.addFieldData(0x1005003,        0, testString,      null);
        iTestData.addFieldData(0x1005004,        0, testString,      null);
        iTestData.addFieldData(0x1005005,        0, testString,      null);
        iTestData.addFieldData(0x1005006,        0, testString,      null);
        iTestData.addFieldData(0x1005007,        0, testString,      null);
        iTestData.addFieldData(0x1005008,        0, 123455555,       null);

        // Initialize contact list and contact item
        try
        {
            PIM pim = PIM.getInstance();
            iPIMList = pim.openPIMList(PIM.CONTACT_LIST, PIM.READ_WRITE);
            iPIMItem = ((ContactList) iPIMList).createContact();
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
        log("Running test CLgetAttributes");
        error = PLitemTester.testAttributeCombinations(
                    iPIMList,
                    iPIMItem,
                    iTestData);
        if (error != null)
        {
            failTest(error);
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
            ((ContactList) iPIMList).removeContact(
                (Contact) iPIMItem);
            iPIMList.close();
        }
        catch (PIMException pe)
        {
            // cannot call failTest
            log("Unexpected PIMException in cleanupTest().");
            pe.printStackTrace();
        }
    }

    /**
     * Helper function
     */
    private void checkError(String aError)
    throws TestEndThrowable
    {
        if (aError != null)
        {
            failTest(aError);
        }
    }
}

// End of file

