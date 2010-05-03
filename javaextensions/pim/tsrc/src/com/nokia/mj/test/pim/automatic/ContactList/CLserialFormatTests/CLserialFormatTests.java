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
package com.nokia.mj.test.pim.automatic.ContactList.CLserialFormatTests;
// IMPORTS
import javax.microedition.pim.*;
import java.util.Hashtable;
import java.util.Vector;
import com.nokia.mj.test.pim.automatic.*;
import com.nokia.mj.test.pim.adaptation.*;
// CLASS DEFINITION
/**
 * This class initializes the test data for PIM serialization tests
 * Only Contact List specific are tested in this class
 *
 * Tested functions:
 * javax.microedition.pim.PIM.toSerialFormat()
 * javax.microedition.pim.PIM.fromSerialFormat()
 *
 */

public class CLserialFormatTests
        extends MIDPTest
{

    public CLserialFormatTests(
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
        String[] nameArrayValues = { "Last", "First", "Middle", "Pre", "Suf", "LNR", "FNR" };
        String[] addrArrayValues = { "P.O box", "Extra", "Street", "Locality", "Region", "Postal", "Country" };
        // Initialize test data. the final string value indicates what string must be
        // found from the data after versit conversion
        iTestData.addFieldData(Contact.NICKNAME, 0, "nickname", "X-NICKNAME:nickname");

        // Contact.EMAIL
        iTestData.addFieldData(Contact.EMAIL, 0,                 "testing@none.com", "EMAIL;ENCODING=QUOTED-PRINTABLE:testing=40none.com");
        iTestData.addFieldData(Contact.EMAIL, Contact.ATTR_HOME, "testing@home.com", "EMAIL;HOME;ENCODING=QUOTED-PRINTABLE:testing=40home.com");
        iTestData.addFieldData(Contact.EMAIL, Contact.ATTR_WORK, "testing@work.com", "EMAIL;WORK;ENCODING=QUOTED-PRINTABLE:testing=40work.com");

        // Contact.TEL
        iTestData.addFieldData(Contact.TEL, 0,                     "Tel",        "TEL;VOICE:Tel");
        iTestData.addFieldData(Contact.TEL, Contact.ATTR_HOME,     "TelHome",    "TEL;VOICE;HOME:TelHome");
        iTestData.addFieldData(Contact.TEL, Contact.ATTR_MOBILE,   "TelMobile",  "TEL;CELL;VOICE:TelMobile");
        iTestData.addFieldData(Contact.TEL, Contact.ATTR_FAX,      "Fax",        "TEL;FAX:Fax");
        iTestData.addFieldData(Contact.TEL, Contact.ATTR_PAGER,    "Pager",      "TEL;PAGER:Pager");

        iTestData.addFieldData(Contact.TEL, Contact.ATTR_MOBILE |
                               Contact.ATTR_WORK,     "MobileWork", "TEL;CELL;VOICE;WORK:MobileWork");

        iTestData.addFieldData(Contact.TEL, Contact.ATTR_MOBILE |
                               Contact.ATTR_HOME,     "MobileHome", "TEL;CELL;VOICE;HOME:MobileHome");

        iTestData.addFieldData(Contact.TEL, Contact.ATTR_FAX |
                               Contact.ATTR_HOME,     "FaxHome",    "TEL;FAX;HOME:FaxHome");

        iTestData.addFieldData(Contact.TEL, Contact.ATTR_FAX |
                               Contact.ATTR_WORK,     "FaxWork",    "TEL;FAX;WORK:FaxWork");

        // Video call attributes
        iTestData.addFieldData(Contact.TEL, 16777216,              "VideoCall",  "TEL;VIDEO:VideoCall");

        iTestData.addFieldData(Contact.TEL, 16777216 |
                               Contact.ATTR_HOME,     "VidHome",    "TEL;VIDEO;HOME:VidHome");

        iTestData.addFieldData(Contact.TEL, 16777216 |
                               Contact.ATTR_WORK,     "VidWork",    "TEL;VIDEO;WORK:VidWork");

        // Assistant phone
        iTestData.addFieldData(Contact.TEL, 1,                     "Assistant",  "X-ASSISTANT-TEL:Assistant");
        // Car phone
        iTestData.addFieldData(Contact.TEL, 2,                     "CarPhone",   "TEL;CAR:CarPhone");

        // VOIP number
        iTestData.addFieldData(0x1005009, 0,                 "VOIPNone", "X-SIP;VOIP:VOIPNone");
        iTestData.addFieldData(0x1005009, Contact.ATTR_HOME, "VOIPHome", "X-SIP;VOIP;HOME:VOIPHome");
        iTestData.addFieldData(0x1005009, Contact.ATTR_WORK, "VOIPWork", "X-SIP;VOIP;WORK:VOIPWork");

        // Fields that have no attributes
        iTestData.addFieldData(Contact.URL,      0, "www.url.com",  "URL:www.url.com");
        iTestData.addFieldData(Contact.NOTE,     0, "Note",         "NOTE:Note");
        iTestData.addFieldData(Contact.ORG,      0, "Organisation", "ORG:Organisation");
        iTestData.addFieldData(Contact.TITLE,    0, "Title",        "TITLE:Title");

        // String arrays
        iTestData.addFieldData(Contact.ADDR,     0, addrArrayValues,
                               "ADR:P.O box;Extra;Street;Locality;Region;Postal;Country");
        iTestData.addFieldData(Contact.ADDR, Contact.ATTR_HOME, addrArrayValues,
                               "ADR;HOME:P.O box;Extra;Street;Locality;Region;Postal;Country");
        iTestData.addFieldData(Contact.ADDR, Contact.ATTR_WORK, addrArrayValues,
                               "ADR;WORK:P.O box;Extra;Street;Locality;Region;Postal;Country");
        iTestData.addFieldData(Contact.NAME,     0, nameArrayValues,
                               "N:Last;First;Middle;Pre;Suf");

        // Extended fields
        iTestData.addFieldData(0x1005002, 0, "SIP",            "X-SIP:SIP");
        iTestData.addFieldData(0x1005003, 0, "DTMF",           "X-DTMF:DTMF");
        iTestData.addFieldData(0x1005004, 0, "Department",     "ORG:;Department");
        iTestData.addFieldData(0x1005005, 0, "Assistant name", "X-ASSISTANT:Assistant name");
        iTestData.addFieldData(0x1005006, 0, "Children",       "X-CHILDREN:Children");
        iTestData.addFieldData(0x1005007, 0, "Spouse",         "X-SPOUSE:Spouse");
        iTestData.addFieldData(0x100500A, 0, "POC",            "X-SIP;POC:POC");
        iTestData.addFieldData(0x100500B, 0, "SWIS",           "X-SIP;SWIS:SWIS");

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
        log("Running CLserialFormatTests. Testing vCard 2.1");
        error = PserializationTester.testToSerialFormat(
                    iPIMList,
                    iPIMItem,
                    iTestData,
                    "UTF-8",
                    "VCARD/2.1");

        if (error != null)
        {
            failTest(error);
        }
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
            // NOTE: The test class doesn't commit the item so
            // we don't have to remove it
            //( ( ContactList ) iPIMList ).removeContact(
            //    ( Contact ) iPIMItem );
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