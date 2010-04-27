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
package com.nokia.mj.test.pim.automatic.PIM.PtoSerialFormat.PtoSerialFormatEx;
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
public class PtoSerialFormatEx extends PtoSerialFormat
{

    public PtoSerialFormatEx(
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
        OutputStream os = null;

        try
        {
            p = PIM.getInstance();
            c1 = createContact(p);
            e1 = createEvent(p);
            t1 = createToDo(p);
            cFormats = p.supportedSerialFormats(PIM.CONTACT_LIST);
            eFormats = p.supportedSerialFormats(PIM.EVENT_LIST);
            tFormats = p.supportedSerialFormats(PIM.TODO_LIST);

            os = new ByteArrayOutputStream();
        }
        catch (Throwable e)
        {
            failTest("preconditions failed: " + e);
        } // end of try-catch

        // Test steps
        // s1
        testNullPointer(p, c1, cFormats, os);
        testNullPointer(p, e1, eFormats, os);
        testNullPointer(p, t1, tFormats, os);

        // s2
        testUEE(p, c1, cFormats, os);
        testUEE(p, e1, eFormats, os);
        testUEE(p, t1, tFormats, os);

        // s3
        testIAE(p, c1, os);
        testIAE(p, e1, os);
        testIAE(p, t1, os);

        passTest();
    }
}



