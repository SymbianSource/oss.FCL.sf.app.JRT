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
package com.nokia.mj.test.pim.automatic.ToDo.TConstants;
//IMPORTS
import java.io.IOException;
import java.util.*;
import javax.microedition.pim.ToDo;
import com.nokia.mj.test.pim.adaptation.*;

// CLASS DEFINITION
/**
 * This class tests javax.microedition.pim.ToDo constants.
 */
public class TConstants
        extends MIDPTest
{

    public TConstants(
        String sTestName, j2meunit.framework.TestMethod rTestMethod)
    {
        super(sTestName, rTestMethod);
    }


    public void runTest() throws TestEndThrowable
    {
        int[] constants = { ToDo.CLASS, 100,
                            ToDo.CLASS_CONFIDENTIAL, 200,
                            ToDo.CLASS_PRIVATE, 201,
                            ToDo.CLASS_PUBLIC, 202,
                            ToDo.COMPLETED, 101,
                            ToDo.COMPLETION_DATE, 102,
                            ToDo.DUE, 103,
                            ToDo.NOTE, 104,
                            ToDo.PRIORITY, 105,
                            ToDo.REVISION, 106,
                            ToDo.SUMMARY, 107,
                            ToDo.UID, 108
                          };

        for (int i=0; i < constants.length; i += 2)
        {
            if (constants[i] != constants[i+1])
            {
                failTest("Class field (" + (i+1) + ") is not correct. ");
            }
        }

        passTest();
    }
}