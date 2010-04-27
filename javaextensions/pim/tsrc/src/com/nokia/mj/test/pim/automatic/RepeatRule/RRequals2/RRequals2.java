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
package com.nokia.mj.test.pim.automatic.RepeatRule.RRequals2;
//IMPORTS
import java.io.IOException;
import java.util.*;
import javax.microedition.pim.RepeatRule;
import com.nokia.mj.test.pim.adaptation.*;

// CLASS DEFINITION
/**
 * This class tests javax.microedition.pim.RepeatRule.equals().
 */
public class RRequals2 extends MIDPTest
{

    public RRequals2(
        String sTestName, j2meunit.framework.TestMethod rTestMethod)
    {
        super(sTestName, rTestMethod);
    }


    public void runTest() throws TestEndThrowable
    {
        RepeatRule rr1 = new RepeatRule();
        boolean bool = true;

        log("Test that rr.equals(null) returns false");
        try
        {
            bool = rr1.equals(null);
        }
        catch (Exception e)
        {
            failTest("RepeatRule.equals(null) threw: "+e);
        }

        if (bool)
        {
            failTest("RepeatRule can not be equal with null!");
        }

        log("Test that empty rr.equals(<object of some other "+
            "class>) returns false");

        try
        {
            bool = rr1.equals(new Integer(352));
        }
        catch (Exception e)
        {
            failTest("RepeatRule.equals(new Integer(352)) threw: "+e);
        }

        if (bool)
        {
            failTest("RepeatRule can not be equal with Integer object!");
        }

        passTest();
    }
}