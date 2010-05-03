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
package com.nokia.mj.test.pim.automatic.PIMException.PEconstants;
//IMPORTS
import javax.microedition.pim.PIMException;
import com.nokia.mj.test.pim.adaptation.*;

// CLASS DEFINITION
/**
 * This class tests javax.microedition.pim.UnsupportedFieldException.getField().
 */
public class PEconstants
        extends MIDPTest
{

    public PEconstants(
        String sTestName, j2meunit.framework.TestMethod rTestMethod)
    {
        super(sTestName, rTestMethod);
    }

    // Expected constants
    private static final int TEST_FEATURE_NOT_SUPPORTED   = 0;
    private static final int TEST_GENERAL_ERROR           = 1;
    private static final int TEST_LIST_CLOSED             = 2;
    private static final int TEST_LIST_NOT_ACCESSIBLE     = 3;
    private static final int TEST_MAX_CATEGORIES_EXCEEDED = 4;
    private static final int TEST_UNSUPPORTED_VERSION     = 5;
    private static final int TEST_UPDATE_ERROR            = 6;


    public void runTest() throws TestEndThrowable
    {
        assertTest(
            PIMException.FEATURE_NOT_SUPPORTED == TEST_FEATURE_NOT_SUPPORTED,
            "FEATURE_NOT_SUPPORTED is " + TEST_FEATURE_NOT_SUPPORTED);

        assertTest(
            PIMException.GENERAL_ERROR == TEST_GENERAL_ERROR,
            "GENERAL_ERROR is " + TEST_GENERAL_ERROR);

        assertTest(
            PIMException.LIST_CLOSED == TEST_LIST_CLOSED,
            "LIST_CLOSED is " + TEST_LIST_CLOSED);

        assertTest(
            PIMException.LIST_NOT_ACCESSIBLE == TEST_LIST_NOT_ACCESSIBLE,
            "LIST_NOT_ACCESSIBLE is " + TEST_LIST_NOT_ACCESSIBLE);

        assertTest(
            PIMException.MAX_CATEGORIES_EXCEEDED == TEST_MAX_CATEGORIES_EXCEEDED,
            "MAX_CATEGORIES_EXCEEDED is " + TEST_MAX_CATEGORIES_EXCEEDED);

        assertTest(
            PIMException.UNSUPPORTED_VERSION == TEST_UNSUPPORTED_VERSION,
            "UNSUPPORTED_VERSION is " + TEST_UNSUPPORTED_VERSION);

        assertTest(
            PIMException.UPDATE_ERROR == TEST_UPDATE_ERROR,
            "UPDATE_ERROR is " + TEST_UPDATE_ERROR);

        passTest();
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