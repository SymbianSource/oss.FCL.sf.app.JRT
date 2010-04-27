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
package com.nokia.mj.test.pim.automatic.ToDoList.TLpriorityMappings;
// IMPORTS
import javax.microedition.pim.*;
import com.nokia.mj.test.pim.adaptation.*;
import com.nokia.mj.test.pim.automatic.*;

// CLASS DEFINITION
/**
 * Tests the ToDo list's priority mappings
 *
 * Reference for priority values was the internal
 * specification of PIM API. Check TLpriorityTester
 * which provides the test functions for priority testing
 * This class only has to initialize ToDo list, item and
 * test data and provide those to TLpriorityTester class
 *
 */

public class TLpriorityMappings
        extends MIDPTest
{

    public TLpriorityMappings(
        String sTestName, j2meunit.framework.TestMethod rTestMethod)
    {
        super(sTestName, rTestMethod);
    }

    /* PIM List */
    private PIMList iPIMList;

    /* ToDo item */
    private ToDo iToDoItem;

    /* Priorities */
    private int[] iTestPriorityValues = null;
    private int[] iExpectedPriorityValues = null;

    /**
     * Initializes the test
     */
    public void initTest()
    throws TestEndThrowable
    {
        // Load test data
        iTestPriorityValues = new int[] { 0, 1, 2, 3, 4, 5, 6, 7, 8, 9 };
        iExpectedPriorityValues = new int[] { 4, 1, 1, 1, 4, 4, 4, 7, 7, 7 };
        // Initialize ToDo list
        try
        {
            PIM pim = PIM.getInstance();

            iPIMList = pim.openPIMList(
                           PIM.TODO_LIST,
                           PIM.READ_WRITE);
            iToDoItem = ((ToDoList) iPIMList).createToDo();
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

        // Test priority mappings
        error = PLpriorityTester.testPriorityMappings(
                    iToDoItem,
                    iTestPriorityValues,
                    iExpectedPriorityValues,
                    true);

        // Check if the test failed or not
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
            ((ToDoList) iPIMList).removeToDo(iToDoItem);
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

