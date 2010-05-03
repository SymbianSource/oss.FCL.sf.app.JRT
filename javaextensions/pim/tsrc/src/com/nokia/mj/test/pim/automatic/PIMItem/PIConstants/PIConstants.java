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
package com.nokia.mj.test.pim.automatic.PIMItem.PIConstants;
//IMPORTS
import java.io.IOException;
import java.util.*;
import javax.microedition.pim.PIMItem;
import com.nokia.mj.test.pim.adaptation.*;

// CLASS DEFINITION
/**
 * Tests that PIMItem class constants equal specification.
 */
public class PIConstants
        extends MIDPTest
{

    public PIConstants(
        String sTestName, j2meunit.framework.TestMethod rTestMethod)
    {
        super(sTestName, rTestMethod);
    }


    public void runTest() throws TestEndThrowable
    {
        if (PIMItem.ATTR_NONE != 0)
        {
            failTest("PIMItem.ATTR_NONE != 0");
        }
        if (PIMItem.BINARY != 0)
        {
            failTest("PIMItem.BINARY != 0");
        }
        if (PIMItem.BOOLEAN != 1)
        {
            failTest("PIMItem.BOOLEAN != 1");
        }
        if (PIMItem.DATE != 2)
        {
            failTest("PIMItem.DATE != 2");
        }
        if (PIMItem.EXTENDED_ATTRIBUTE_MIN_VALUE != 16777216)
        {
            failTest("PIMItem.EXTENDED_ATTRIBUTE_MIN_VALUE != 16777216");
        }
        if (PIMItem.EXTENDED_FIELD_MIN_VALUE != 16777216)
        {
            failTest("PIMItem.EXTENDED_FIELD_MIN_VALUE != 16777216");
        }
        if (PIMItem.INT != 3)
        {
            failTest("PIMItem.INT != 3");
        }
        if (PIMItem.STRING != 4)
        {
            failTest("PIMItem.STRING != 4");
        }
        if (PIMItem.STRING_ARRAY != 5)
        {
            failTest("PIMItem.STRING_ARRAY != 5");
        }
        passTest();
    }
}
