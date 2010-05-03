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
package com.nokia.mj.test.pim.automatic.UnsupportedFieldException.UFEgetField;
//IMPORTS
import java.io.IOException;
import java.util.*;
import javax.microedition.pim.UnsupportedFieldException;
import com.nokia.mj.test.pim.adaptation.*;

// CLASS DEFINITION
/**
 * This class tests javax.microedition.pim.UnsupportedFieldException.getField().
 */
public class UFEgetField extends MIDPTest
{

    public UFEgetField(
        String sTestName, j2meunit.framework.TestMethod rTestMethod)
    {
        super(sTestName, rTestMethod);
    }


    public void runTest() throws TestEndThrowable
    {
        UnsupportedFieldException ufe1 = new UnsupportedFieldException();
        if (ufe1.getField() != -1)
        {
            failTest("Default field value returned by getField() was not -1!");
        }

        String testMessage = "test ";

        UnsupportedFieldException ufe2 = new UnsupportedFieldException(testMessage);
        if (!(ufe2.getMessage().equals(testMessage)))
        {
            failTest("Field given in constructor parameter was not returned by getField()!");
        }
        if (ufe2.getField() != -1)
        {
            failTest("Default field value returned by getField() was not -1!");
        }

        // test that getField returns value set in constructor parameter
        for (int i=0; i < 10; i++)
        {
            UnsupportedFieldException ufe = new UnsupportedFieldException(testMessage + i, i);

            if (ufe.getField() != i)
            {
                failTest("Field given in constructor parameter was not returned by getField()!");
            }

            if (!(ufe.getMessage()).equals(testMessage + i))
            {
                failTest("Field given in constructor parameter was not returned by getField()!");
            }
        }

        passTest();
    }
}