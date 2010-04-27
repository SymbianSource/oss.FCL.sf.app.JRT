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
package com.nokia.mj.test.pim.automatic.Event.EConstants;
//IMPORTS
import java.io.IOException;
import java.util.*;
import javax.microedition.pim.Event;
import com.nokia.mj.test.pim.adaptation.*;

// CLASS DEFINITION
/**
 * Tests that Contact class constants equal specification.
 */
public class EConstants
        extends MIDPTest
{

    public EConstants(
        String sTestName, j2meunit.framework.TestMethod rTestMethod)
    {
        super(sTestName, rTestMethod);
    }


    public void runTest() throws TestEndThrowable
    {
        if (Event.ALARM != 100)
        {
            failTest("Event.ALARM != 100");
        }
        if (Event.CLASS != 101)
        {
            failTest("Event.CLASS != 101");
        }
        if (Event.END != 102)
        {
            failTest("Event.END != 102");
        }
        if (Event.LOCATION != 103)
        {
            failTest("Event.LOCATION != 103");
        }
        if (Event.NOTE != 104)
        {
            failTest("Event.NOTE != 104");
        }
        if (Event.REVISION != 105)
        {
            failTest("Event.REVISION != 105");
        }
        if (Event.START != 106)
        {
            failTest("Event.START != 106");
        }
        if (Event.SUMMARY != 107)
        {
            failTest("Event.SUMMARY != 107");
        }
        if (Event.UID != 108)
        {
            failTest("Event.UID != 108");
        }
        if (Event.CLASS_CONFIDENTIAL != 200)
        {
            failTest("Event.CLASS_CONFIDENTIAL != 200");
        }
        if (Event.CLASS_PRIVATE != 201)
        {
            failTest("Event.CLASS_PRIVATE != 201");
        }
        if (Event.CLASS_PUBLIC != 202)
        {
            failTest("Event.CLASS_PUBLIC != 202");
        }

        passTest();
    }
}
