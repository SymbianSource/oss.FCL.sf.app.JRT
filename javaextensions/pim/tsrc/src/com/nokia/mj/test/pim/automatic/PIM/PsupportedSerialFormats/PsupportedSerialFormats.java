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
package com.nokia.mj.test.pim.automatic.PIM.PsupportedSerialFormats;
//IMPORTS
import java.io.IOException;
import java.util.*;
import javax.microedition.pim.PIM;
import com.nokia.mj.test.pim.adaptation.*;

// CLASS DEFINITION
/**
 * Tests javax.microedition.pim.PIM.supportedSerialFormats()
 */
public class PsupportedSerialFormats
        extends MIDPTest
{

    public PsupportedSerialFormats(
        String sTestName, j2meunit.framework.TestMethod rTestMethod)
    {
        super(sTestName, rTestMethod);
    }


    public void runTest() throws TestEndThrowable
    {
        PIM p = PIM.getInstance();

        TestSupportedFormats(p, PIM.CONTACT_LIST, "VCARD/2.1");
        TestSupportedFormats(p, PIM.EVENT_LIST, "VCALENDAR/1.0");
        TestSupportedFormats(p, PIM.TODO_LIST, "VCALENDAR/1.0");

        TestException(p, -1);
        TestException(p, 0);
        TestException(p, 4);

        passTest();
    }

    private void TestSupportedFormats(PIM aP, int aListType, String aFormat)
    throws TestEndThrowable
    {
        String[] supportedFormats = aP.supportedSerialFormats(aListType);
        if (supportedFormats == null)
        {
            failTest("supportedFormats == null");
        }
        boolean found = false;
        for (int i = 0; i < supportedFormats.length; i++)
        {
            if (supportedFormats[ i ].compareTo(aFormat) == 0)
            {
                found = true;
            }
        }
        if (!found)
        {
            failTest(aFormat + " not found");
        }
    }

    private void TestException(PIM aP, int aListType)
    throws TestEndThrowable
    {
        boolean exceptionThrown = false;
        try
        {
            aP.supportedSerialFormats(aListType);
        }
        catch (IllegalArgumentException e)
        {
            exceptionThrown = true;
        } // end of try-catch
        if (!exceptionThrown)
        {
            failTest("exception not thrown( listtype -1 )");
        } // end of if (!exceptionThrown)
    }

}
