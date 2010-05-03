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
package com.nokia.mj.test.pim.automatic.ContactList.CLcategoryTests;
// IMPORTS
import javax.microedition.pim.*;
import java.lang.String;
import com.nokia.mj.test.pim.automatic.*;
import com.nokia.mj.test.pim.adaptation.*;

// CLASS DEFINITION
/**
 * Tests the contact list's category support
 *
 * Class PLcategoryTester tests the actual
 * functions. This class only initializes the
 * data and calls the right testing fuction
 *
 */

public class CLcategoryTests
        extends MIDPTest
{

    public CLcategoryTests(
        String sTestName, j2meunit.framework.TestMethod rTestMethod)
    {
        super(sTestName, rTestMethod);
    }

    /* PIM List */
    private PIMList iPIMList;

    /**
     * Initializes the test
     */
    public void initTest()
    throws TestEndThrowable
    {
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
        log("Running test. CLcategoryTests");
        // Create category

        String error = PLcategoryTester.testMaxCategories(iPIMList,
                       -1, // unlimited categories
                       0, // No static categories
                       null);
        CheckError(error);

        // Test passed
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
    private void CheckError(String aError)
    throws TestEndThrowable
    {
        // Check and fail if error string has some data
        if (aError != null)
        {
            failTest(aError);
        }
    }
}

// End of file

