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
package com.nokia.mj.test.pim.automatic.ContactList.CLsupportedArrayElements;
// IMPORTS
import javax.microedition.pim.*;
import java.util.Hashtable;
import java.util.Vector;
import com.nokia.mj.test.pim.automatic.*;
import com.nokia.mj.test.pim.adaptation.*;
// CLASS DEFINITION
/**
 * Tests the contact list's supported array elements
 *
 * Reference for supported field values in this point wwas
 * the source code because the specification wasn't approved
 * yet and it had misinformation about some fields.
 *
 * ATTENTION: Update supported and unsupported fields when
 * the PIM API specification is approved
 *
 */

public class CLsupportedArrayElements
        extends MIDPTest
{

    public CLsupportedArrayElements(
        String sTestName, j2meunit.framework.TestMethod rTestMethod)
    {
        super(sTestName, rTestMethod);
    }

    /* PIM List */
    private PIMList iPIMList;

    /* Test data including supported field values */
    private SupportTestData iTestData = new SupportTestData();

    /**
     * Initializes the test
     */
    public void initTest()
    throws TestEndThrowable
    {
        // Initialize test data. Contact.NAME field
        iTestData.addSupportedArrayElement(Contact.NAME, Contact.NAME_GIVEN);
        iTestData.addSupportedArrayElement(Contact.NAME, Contact.NAME_FAMILY);
        iTestData.addSupportedArrayElement(Contact.NAME, Contact.NAME_PREFIX);
        iTestData.addSupportedArrayElement(Contact.NAME, Contact.NAME_SUFFIX);
        iTestData.addSupportedArrayElement(Contact.NAME, Contact.NAME_OTHER);
        iTestData.addSupportedArrayElement(Contact.NAME, 5);    // Last name reading
        iTestData.addSupportedArrayElement(Contact.NAME, 6);    // First name reading
        // Contact.ADDR field
        iTestData.addSupportedArrayElement(Contact.ADDR, Contact.ADDR_POBOX);
        iTestData.addSupportedArrayElement(Contact.ADDR, Contact.ADDR_STREET);
        iTestData.addSupportedArrayElement(Contact.ADDR, Contact.ADDR_POSTALCODE);
        iTestData.addSupportedArrayElement(Contact.ADDR, Contact.ADDR_LOCALITY);
        iTestData.addSupportedArrayElement(Contact.ADDR, Contact.ADDR_REGION);
        iTestData.addSupportedArrayElement(Contact.ADDR, Contact.ADDR_COUNTRY);
        iTestData.addSupportedArrayElement(Contact.ADDR, Contact.ADDR_EXTRA);
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
        String err = PLsupportTester.testSupportedArrayElements(
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

