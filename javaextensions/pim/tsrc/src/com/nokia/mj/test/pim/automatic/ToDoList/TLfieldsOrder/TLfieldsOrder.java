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
package com.nokia.mj.test.pim.automatic.ToDoList.TLfieldsOrder;
// IMPORTS
import javax.microedition.pim.*;
import java.util.Vector;
import com.nokia.mj.test.pim.adaptation.*;
import com.nokia.mj.test.pim.automatic.*;

// CLASS DEFINITION
/**
 * Tests that fields of the to-do list are in right order
 *
 * Reference for supported field values in this point wwas
 * the source code because the specification wasn't approved
 * yet and it had misinformation about some fields. Order was
 * also checked from the emulator
 *
 * ATTENTION: Update fields if needed when the API specification
 * is approved
 */

public class TLfieldsOrder
        extends MIDPTest
{

    public TLfieldsOrder(
        String sTestName, j2meunit.framework.TestMethod rTestMethod)
    {
        super(sTestName, rTestMethod);
    }

    /* PIM List */
    private PIMList iPIMList;

    /* Test data containing supported field values in right order */
    private SupportTestData iOrderedFields = new SupportTestData();

    /**
     * Initializes the test
     */
    public void initTest()
    throws TestEndThrowable
    {
        // Initialize supported fields list. Check that the
        // order is the same as in the native side to-do list
        iOrderedFields.addSupportedField(ToDo.SUMMARY);
        iOrderedFields.addSupportedField(ToDo.NOTE);
        iOrderedFields.addSupportedField(ToDo.DUE);
        iOrderedFields.addSupportedField(0x1005001);                // Extended alarm field
        iOrderedFields.addSupportedField(ToDo.PRIORITY);
        iOrderedFields.addSupportedField(ToDo.COMPLETED);
        iOrderedFields.addSupportedField(ToDo.COMPLETION_DATE);
        iOrderedFields.addSupportedField(ToDo.CLASS);
        iOrderedFields.addSupportedField(ToDo.UID);
        iOrderedFields.addSupportedField(ToDo.REVISION);

        // Initialize contact list
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
        log("Running test.");
        String err = PLsupportTester.testFieldsOrder(
                         iPIMList,
                         iOrderedFields);
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

