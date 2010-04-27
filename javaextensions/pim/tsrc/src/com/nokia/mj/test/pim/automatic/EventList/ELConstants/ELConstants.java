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
package com.nokia.mj.test.pim.automatic.EventList.ELConstants;
//IMPORTS
import java.io.IOException;
import java.util.*;
import javax.microedition.pim.EventList;
import com.nokia.mj.test.pim.adaptation.*;

// CLASS DEFINITION
/**
 * This class tests javax.microedition.pim.EventList constants.
 */
public class ELConstants
        extends MIDPTest
{

    public ELConstants(
        String sTestName, j2meunit.framework.TestMethod rTestMethod)
    {
        super(sTestName, rTestMethod);
    }


    public void runTest() throws TestEndThrowable
    {
        int[] constants = { EventList.STARTING, 0,
                            EventList.ENDING, 1,
                            EventList.OCCURRING, 2
                          };

        for (int i=0; i < constants.length; i += 2)
        {
            if (constants[i] != constants[i+1])
            {
                failTest("Class field (" + (i+1) + ") is not correct.");
            }
        }
        // all is well
        passTest();
    }
}