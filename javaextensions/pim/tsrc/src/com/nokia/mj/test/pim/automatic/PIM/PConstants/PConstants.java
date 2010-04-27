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
package com.nokia.mj.test.pim.automatic.PIM.PConstants;
//IMPORTS
import java.io.IOException;
import java.util.*;
import javax.microedition.pim.PIM;
import com.nokia.mj.test.pim.adaptation.*;

// CLASS DEFINITION
/**
 * Tests that PIM class constants equal specification.
 *
 */
public class PConstants
        extends MIDPTest
{

    public PConstants(
        String sTestName, j2meunit.framework.TestMethod rTestMethod)
    {
        super(sTestName, rTestMethod);
    }


    public void runTest() throws TestEndThrowable
    {
        if (PIM.CONTACT_LIST != 1)
        {
            failTest("PIM.CONTACT_LIST != 1");
        } // end of if (PIM.CONTACT_LIST != 1)
        if (PIM.EVENT_LIST != 2)
        {
            failTest("PIM.EVENT_LIST != 2");
        }
        if (PIM.READ_ONLY != 1)
        {
            failTest("PIM.READ_ONLY != 1");
        } // end of if (PIM.READ_ONLY != 1)
        if (PIM.READ_WRITE != 3)
        {
            failTest("PIM.READ_WRITE != 3");
        }
        if (PIM.TODO_LIST != 3)
        {
            failTest("PIM.TODO_LIST != 3");
        } // end of if (PIM.TODO_LIST != 3)
        if (PIM.WRITE_ONLY != 2)
        {
            failTest("PIM.WRITE_ONLY != 2");
        } // end of if (PIM.WRITE_ONLY != 2)

        passTest();
    }
}
