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
package com.nokia.mj.test.pim.automatic.PIM.PtoSerialFormat.PtoSerialFormat;
//IMPORTS
import java.io.*;
import java.util.*;
import javax.microedition.pim.*;
import com.nokia.mj.test.pim.automatic.*;
import com.nokia.mj.test.pim.adaptation.*;

// CLASS DEFINITION
/**
 * Tests javax.microedition.pim.PIM.toSerialFormat()
 */
public class PtoSerialFormatTest extends PtoSerialFormat
{

    public PtoSerialFormatTest(
        String sTestName, j2meunit.framework.TestMethod rTestMethod)
    {
        super(sTestName, rTestMethod);
    }

    public void runTest() throws TestEndThrowable
    {
        // Preconditions
        PIM p = null;
        Contact c1 = null;
        Event e1 = null;
        ToDo t1 = null;
        String[] cFormats = null;
        String[] eFormats = null;
        String[] tFormats = null;

        try
        {
            p = PIM.getInstance();
            c1 = createContact(p);
            e1 = createEvent(p);
            t1 = createToDo(p);
            cFormats = p.supportedSerialFormats(PIM.CONTACT_LIST);
            eFormats = p.supportedSerialFormats(PIM.EVENT_LIST);
            tFormats = p.supportedSerialFormats(PIM.TODO_LIST);
        }
        catch (Throwable e)
        {
            failTest("preconditions failed: " + e);
        } // end of try-catch

        // Test steps

        // s1 and s2
        testSerialFormats(p, e1, eFormats);
        // s3 and s4

        testSerialFormats(p, c1, cFormats);
        // s5 and s6
        testSerialFormats(p, t1, tFormats);

        passTest();
    }
}



