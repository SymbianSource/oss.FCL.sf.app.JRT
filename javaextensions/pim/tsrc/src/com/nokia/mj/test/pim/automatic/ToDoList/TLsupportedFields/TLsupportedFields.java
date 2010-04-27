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
package com.nokia.mj.test.pim.automatic.ToDoList.TLsupportedFields;
// IMPORTS
import javax.microedition.pim.*;
import java.util.Hashtable;
import java.util.Vector;
import com.nokia.mj.test.pim.adaptation.*;
import com.nokia.mj.test.pim.automatic.*;

// CLASS DEFINITION
/**
 * Tests the ToDo list's supported fields
 *
 * Reference for supported field values in this point wwas
 * the source code because the specification wasn't approved
 * yet and it had misinformation about some fields.
 *
 * ATTENTION: Update supported and unsupported fields when
 * the PIM API specification is approved
 */

public class TLsupportedFields
        extends MIDPTest
{

    public TLsupportedFields(
        String sTestName, j2meunit.framework.TestMethod rTestMethod)
    {
        super(sTestName, rTestMethod);
    }

    /* PIM List */
    private PIMList iPIMList;

    /* Test data including supported field values */
    private SupportTestData iSupportedFields = new SupportTestData();

    /**
     * Initializes the test
     */
    public void initTest()
    throws TestEndThrowable
    {
        // Initialize supported fields. Key indicates the field and value indicates
        // if the field is supported or not in thist list
        iSupportedFields.addSupportedField(ToDo.SUMMARY);
        iSupportedFields.addSupportedField(ToDo.DUE);
        iSupportedFields.addSupportedField(ToDo.PRIORITY);
        iSupportedFields.addSupportedField(ToDo.COMPLETED);
        iSupportedFields.addSupportedField(ToDo.COMPLETION_DATE);
        iSupportedFields.addSupportedField(ToDo.CLASS);
        iSupportedFields.addSupportedField(ToDo.UID);
        iSupportedFields.addSupportedField(ToDo.REVISION);
        iSupportedFields.addSupportedField(ToDo.NOTE);
        iSupportedFields.addSupportedField(0x1005001);                  // Extended alarm field

        // Initialize ToDo list
        try
        {
            PIM pim = PIM.getInstance();

            iPIMList = pim.openPIMList(
                           PIM.TODO_LIST,
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
        log("Running test TLsupportedFields");
        // Test supported fields
        String err = PLsupportTester.testSupportedFields(
                         iPIMList,
                         iSupportedFields);
        // Fail if error string was returned
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

