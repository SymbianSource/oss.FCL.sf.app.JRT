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


//PACKAGE
package com.nokia.mj.test.pim.automatic.PIMException.PEgetReason;
//IMPORTS
import javax.microedition.pim.PIMException;
import com.nokia.mj.test.pim.adaptation.*;

// CLASS DEFINITION
/**
 * This class tests javax.microedition.pim.UnsupportedFieldException.getField().
 */
public class PEgetReason
        extends MIDPTest
{

    public PEgetReason(
        String sTestName, j2meunit.framework.TestMethod rTestMethod)
    {
        super(sTestName, rTestMethod);
    }

    // Test messages for PIMException
    private static final String TEST_MESSAGE = "Test message.";
    private static final String[] TEST_MESSAGES = { null, TEST_MESSAGE };

    // Test reasons for PIMException
    private static final int[] TEST_REASONS =
        { 0, 1, 2, 3, 4, 5, 6, 10, 100, -1 };

    public void runTest() throws TestEndThrowable
    {
        testDefaultConstructor();
        testMessageWithDefaultReason();
        testMessageAndReason();

        passTest();
    }


    private void testDefaultConstructor()
    throws TestEndThrowable
    {
        PIMException pe = new PIMException();

        assertTest(
            pe.getMessage() == null,
            "Message is null with default constructor.");

        assertTest(
            pe.getReason() == PIMException.GENERAL_ERROR,
            "Reason is GENERAL_ERROR with default constructor.");
    }


    private void testMessageWithDefaultReason()
    throws TestEndThrowable
    {
        for (int i = 0; i < TEST_MESSAGES.length; i++)
        {
            PIMException pe = new PIMException(TEST_MESSAGES[ i ]);

            assertTest(
                pe.getMessage() == TEST_MESSAGES[ i ],
                "Message is set correctly with PIMException( String ) " +
                "constructor.");

            assertTest(
                pe.getReason() == PIMException.GENERAL_ERROR,
                "Reason is GENERAL_ERROR with PIMException( String ) "+
                "constructor.");
        }
    }


    private void testMessageAndReason()
    throws TestEndThrowable
    {
        for (int mi = 0; mi < TEST_MESSAGES.length; mi++)  // message index
        {
            for (int ri = 0; ri < TEST_REASONS.length; ri++)  // reason index
            {
                PIMException pe = new PIMException(
                    TEST_MESSAGES[ mi ],
                    TEST_REASONS[ ri ]);

                assertTest(
                    pe.getMessage() == TEST_MESSAGES[ mi ],
                    "Message is set correctly with " +
                    "PIMException( String, int ) constructor.");

                assertTest(
                    pe.getReason() == TEST_REASONS[ ri ],
                    "Reason is set correctly with " +
                    "PIMException( String, int ) constructor. ");
            }
        }
    }


    private void assertTest(boolean aCondition, String aDescription)
    throws TestEndThrowable
    {
        if (!aCondition)
        {
            failTest("False assertTestion: " + aDescription);
        }
    }
}