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
package com.nokia.mj.test.pim.automatic.PIM.PfromSerialFormat.PfromSerialFormat;
//IMPORTS
import java.io.InputStream;
import java.io.UnsupportedEncodingException;
import java.util.*;
import javax.microedition.pim.PIM;
import javax.microedition.pim.PIMException;
import javax.microedition.pim.PIMItem;
import javax.microedition.pim.Contact;
import javax.microedition.pim.Event;
import javax.microedition.pim.ToDo;
import com.nokia.mj.test.pim.automatic.*;
import com.nokia.mj.test.pim.adaptation.*;

// CLASS DEFINITION
/**
 * Tests javax.microedition.pim.PIM.fromSerialFormat()
 */
public class PfromSerialFormatTest extends PfromSerialFormat
{

    public PfromSerialFormatTest(
        String sTestName, j2meunit.framework.TestMethod rTestMethod)
    {
        super(sTestName, rTestMethod);
    }

    public void runTest() throws TestEndThrowable
    {
        // Preconditions
        PIM p = null;
        InputStream ise = null;
        InputStream isc = null;
        InputStream ist = null;
        debug("pfrmserialformat");

        try
        {
            p = PIM.getInstance();
            ise = getClass().getResourceAsStream("/res/timed-appointment.vcs");
            isc = getClass().getResourceAsStream("/res/3.vcf");
            ist = getClass().getResourceAsStream("/res/todoPriority2.vcs");
        }
        catch (Throwable e)
        {
            failTest("preconditions failed: " + e.toString());
        } // end of try-catch

        // Test steps

        // s1 and s2
        testEvent(p, ise);

        // s3 and s4
        testContact(p, isc);

        // s5 and s6
        testToDo(p, ist);

        passTest();
    }
}



