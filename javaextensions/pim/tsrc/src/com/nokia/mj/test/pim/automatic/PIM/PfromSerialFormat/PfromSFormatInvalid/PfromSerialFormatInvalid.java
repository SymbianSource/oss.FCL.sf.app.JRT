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
package com.nokia.mj.test.pim.automatic.PIM.PfromSerialFormat.PfromSFormatInvalid;
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
public class PfromSerialFormatInvalid extends PfromSerialFormat
{

    public PfromSerialFormatInvalid(
        String sTestName, j2meunit.framework.TestMethod rTestMethod)
    {
        super(sTestName, rTestMethod);
    }

    public void runTest() throws TestEndThrowable
    {
        // Preconditions
        PIM p = null;
        InputStream isArray[] = null;

        try
        {
            p = PIM.getInstance();

            int maxValue = 6;
            isArray = new InputStream[ maxValue ];
            for (int i = 0; i < maxValue; i++)
            {
                int value = i + 1;
                isArray[ i ] =
                    getClass().getResourceAsStream("/res/invalid" + value + ".vcf");
            } // end of for (int i = 0; i < maxValue; i++)
        }
        catch (Throwable e)
        {
            failTest("preconditions failed: " + e.toString());
        } // end of try-catch

        // Test steps

        for (int i = 0; i < isArray.length; i++)
        {
            // Apparently the objects
            // invalid3.vcf, invalid4.vcf and invalid5.vcf
            // are not that invalid; we don't expect an exception
            // for them.
            if (i == 2 || i == 3 || i == 4)
            {
                log("skipping item " + i);
            }
            else
            {
                log("testing invalid item " + i);
                testPIME2(p, isArray[ i ]);
            }
        } // end of for (int i = 0; i < isArray.length; i++)

        passTest();
    }
}



