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
package com.nokia.mj.test.pim.automatic.ContactList.CLmaxValues;
// IMPORTS
import javax.microedition.pim.*;
import java.util.Hashtable;
import java.util.Vector;
import com.nokia.mj.test.pim.automatic.*;
import com.nokia.mj.test.pim.adaptation.*;

// CLASS DEFINITION
/**
 * Tests the maximum values of contact list items
 *
 * Reference for supported field values in this point wwas
 * the source code because the specification wasn't approved
 * yet and it had misinformation about some fields.
 *
 * ATTENTION: Update maximum values when the specification
 *            is updated
 */

public class CLmaxValues
        extends MIDPTest
{

    public CLmaxValues(
        String sTestName, j2meunit.framework.TestMethod rTestMethod)
    {
        super(sTestName, rTestMethod);
    }

    /* PIM List */
    private PIMList iPIMList;

    /* Hash table including fields and maximum values */
    private Hashtable iMaximumValues = new Hashtable();

    /* Unlimited values */
    private static int UNLIMITED = -1;
    /**
     * Initializes the test
     */
    public void initTest()
    throws TestEndThrowable
    {
        // Initialize maximum values. -1 means unlimiited value
        iMaximumValues.put(new Integer(Contact.ADDR), new Integer(3));
        iMaximumValues.put(new Integer(Contact.BIRTHDAY), new Integer(1));
        iMaximumValues.put(new Integer(Contact.CLASS), new Integer(1));
        iMaximumValues.put(new Integer(Contact.EMAIL), new Integer(UNLIMITED));
        iMaximumValues.put(new Integer(Contact.FORMATTED_ADDR), new Integer(0));
        iMaximumValues.put(new Integer(Contact.FORMATTED_NAME), new Integer(0));
        iMaximumValues.put(new Integer(Contact.NAME), new Integer(1));
        iMaximumValues.put(new Integer(Contact.NICKNAME), new Integer(1));
        iMaximumValues.put(new Integer(Contact.NOTE), new Integer(UNLIMITED));
        iMaximumValues.put(new Integer(Contact.ORG), new Integer(1));
        iMaximumValues.put(new Integer(Contact.PHOTO), new Integer(1));
        iMaximumValues.put(new Integer(Contact.PHOTO_URL), new Integer(0));
        iMaximumValues.put(new Integer(Contact.PUBLIC_KEY), new Integer(0));
        iMaximumValues.put(new Integer(Contact.PUBLIC_KEY_STRING), new Integer(0));
        iMaximumValues.put(new Integer(Contact.REVISION), new Integer(1));
        iMaximumValues.put(new Integer(Contact.TEL), new Integer(UNLIMITED));
        iMaximumValues.put(new Integer(Contact.TITLE), new Integer(UNLIMITED));
        iMaximumValues.put(new Integer(Contact.UID), new Integer(1));
        iMaximumValues.put(new Integer(Contact.URL), new Integer(UNLIMITED));

        // Extended fields
        iMaximumValues.put(new Integer(0x1005001), new Integer(1));                                  // WV ID
        iMaximumValues.put(new Integer(0x1005002), new Integer(UNLIMITED));                          // SIP
        iMaximumValues.put(new Integer(0x1005003), new Integer(UNLIMITED));                          // DTMF
        iMaximumValues.put(new Integer(0x1005004), new Integer(1));                                  // Department
        iMaximumValues.put(new Integer(0x1005005), new Integer(1));                                  // Assistant name
        iMaximumValues.put(new Integer(0x1005006), new Integer(1));                                  // Children
        iMaximumValues.put(new Integer(0x1005007), new Integer(1));                                  // Spouse
        iMaximumValues.put(new Integer(0x1005008), new Integer(1));                                  // Anniversary
        iMaximumValues.put(new Integer(0x1005009), new Integer(UNLIMITED));                          // VOIP

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
        String err = PLitemTester.testMaximumValues(
                         iPIMList,
                         iMaximumValues);
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

