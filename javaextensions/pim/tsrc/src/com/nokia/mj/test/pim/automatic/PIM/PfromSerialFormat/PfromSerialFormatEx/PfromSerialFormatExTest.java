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
package com.nokia.mj.test.pim.automatic.PIM.PfromSerialFormat.PfromSerialFormatEx;
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
public class PfromSerialFormatExTest extends PfromSerialFormat
{

    public PfromSerialFormatExTest(
        String sTestName, j2meunit.framework.TestMethod rTestMethod)
    {
        super(sTestName, rTestMethod);
    }

    public void runTest() throws TestEndThrowable
    {
        // Preconditions
        PIM p = null;
        InputStream is = null; // valid
        InputStream is1 = null; // these are invalid
        InputStream is2 = null;
        InputStream is3 = null;

        try
        {
            p = PIM.getInstance();
            is = getClass().getResourceAsStream("/res/3.vcf");
            // 1.vcf is separated by CRCRLF instead of CRLF
            is1 = getClass().getResourceAsStream("/res/1.vcf");
            // 6.vcf is VCARD 3.0
            is2 = getClass().getResourceAsStream("/res/6.vcf");
            // broken.cvf is not a vcard at all
            is3 = getClass().getResourceAsStream("/res/broken.vcf");
        }
        catch (Throwable e)
        {
            failTest("preconditions failed: " + e.toString());
        } // end of try-catch

        // Test steps

        testNullPointer(p);
        testUEE(p, is, "bug");
        testPIME1(p, is1, "vCard with CRCRLF");
        testPIME1(p, is2, "vCard 3.0");
        testPIME2(p, is3);

        passTest();
    }
}



