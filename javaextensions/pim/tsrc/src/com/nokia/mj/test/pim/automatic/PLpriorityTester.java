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
package com.nokia.mj.test.pim.automatic ;
// IMPORTS
import java.lang.String;
import javax.microedition.pim.*;

// CLASS DEFINITION
/**
 * Base that tests the support functionality
 *
 * This class is common for all support test and
 * must not be changed unless you are implementing
 * new common test or updating some tests
 */

public class PLpriorityTester
{

    /**
     * Constructor
     */
    public void PLpriorityTester()
    {
    }

    /**
     * Tests that item of ToDo list returns right mapped priorities
     * Tested operations: ToDo.setInt(...) ToDo.getInt(...)
     *
     * NOTE: The arrays must be equal length. The first array has values which
     *       are to be tested. The second array has the values correspond to
     *       the values of the first array but they are mapped values. E.g
     *       in PIM API 3.0 priority value 2 is high and it must be mapped
     *       to value 3.
     *
     * @param aToDoItem Newly created ToDo item
     * @param aTestPriorityValues Tested priority values
     * @param aMappedPriorityValues Mapped priority values
     * @return String containing the error decription. null if no errors occured
     */
    public static String testPriorityMappings(
        ToDo aToDoItem,
        int[] aTestPriorityValues,
        int[] aMappedPriorityValues,
        boolean aDoCommit)
    {
        int priority = -1;

        // Check that test has been initialized
        if (aToDoItem == null ||
                aTestPriorityValues == null ||
                aMappedPriorityValues == null)
        {
            return "Test has not been initialized";
        }
        // Add first priority
        aToDoItem.addInt(ToDo.PRIORITY, PIMItem.ATTR_NONE, aTestPriorityValues[ 0 ]);
        // Do first commit
        if (aDoCommit)
        {
            try
            {
                aToDoItem.commit();
            }
            catch (PIMException pe)
            {
                pe.printStackTrace();
                return "ToDo.commit() threw an unexpected exception: " + pe.toString();
            }
        }
        // Check that added priority matches with expected priority
        // and is mapped right
        priority = aToDoItem.getInt(ToDo.PRIORITY, 0);
        if (priority != aMappedPriorityValues[ 0 ])
        {
            return "Priority value " + aTestPriorityValues[ 0 ] + " was mapped "
                   + "to value " + priority + ". Should have been "
                   + aMappedPriorityValues[ 0 ];
        }

        // Do this for the rest of the priorities
        for (int i = 1; i < aTestPriorityValues.length; i++)
        {
            // Set new priority to index 0 with no attributes
            aToDoItem.setInt(ToDo.PRIORITY, 0, PIMItem.ATTR_NONE, aTestPriorityValues[ i ]);
            // Do commit if needed
            if (aDoCommit)
            {
                try
                {
                    aToDoItem.commit();
                }
                catch (PIMException pe)
                {
                    pe.printStackTrace();
                    return "ToDo.commit() threw an unexpected exception: " + pe.toString();
                }
            }
            // Check that added priority matches with expected priority
            // and is mapped right
            priority = aToDoItem.getInt(ToDo.PRIORITY, 0);
            if (priority != aMappedPriorityValues[ i ])
            {
                return "Priority value " + aTestPriorityValues[ i ] + " was mapped "
                       + "to value " + priority + ". Should have been "
                       + aMappedPriorityValues[ i ];
            }
        }

        // Test was ok
        return null;
    }
}

// End of file

