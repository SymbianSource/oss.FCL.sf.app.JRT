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
package com.nokia.mj.test.pim.automatic.PIM.PlistPIMlists;
//IMPORTS
import java.io.IOException;
import java.util.*;
import javax.microedition.pim.PIM;
import com.nokia.mj.test.pim.adaptation.*;

// CLASS DEFINITION
/**
 * Tests javax.microedition.pim.PIM.listPIMlists()
 *

 */
public class PlistPIMlists
        extends MIDPTest
{

    public PlistPIMlists(
        String sTestName, j2meunit.framework.TestMethod rTestMethod)
    {
        super(sTestName, rTestMethod);
    }


    public void runTest() throws TestEndThrowable
    {
        PIM p = PIM.getInstance();

        TestAndLog(p, PIM.CONTACT_LIST);
        TestAndLog(p, PIM.EVENT_LIST);
        TestAndLog(p, PIM.TODO_LIST);

        TestException(p, -1);
        TestException(p, 0);
        TestException(p, 4);

        passTest();
    }

    private void TestAndLog(PIM aP, int aListType)
    throws TestEndThrowable
    {
        String[] listNames = null;
        try
        {
            listNames = aP.listPIMLists(aListType);
        }
        catch (SecurityException e)
        {
            log("SecurityException:" + e.getMessage());
        } // end of try-catch

        if (listNames == null)
        {
            failTest("listNames == null");
        } // end of if (listNames == null)

        log("list names:");
        for (int i = 0; i < listNames.length; i++)
        {
            log(listNames[ i ]);
        } // end of for (int i = 0; i < listNames.length; i++)
    }

    private void TestException(PIM aP, int aListType)
    throws TestEndThrowable
    {
        boolean exceptionThrown = false;
        try
        {
            aP.listPIMLists(aListType);
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
