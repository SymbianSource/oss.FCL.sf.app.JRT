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
package com.nokia.mj.test.pim.automatic.FieldFullException.FFEgetField;
//IMPORTS
import java.io.IOException;
import java.util.*;
import javax.microedition.pim.FieldFullException;
import com.nokia.mj.test.pim.adaptation.*;

// CLASS DEFINITION
/**
 * This class tests javax.microedition.pim.FieldFullException.getField().
 */
public class FFEgetField extends MIDPTest
{

    public FFEgetField(
        String sTestName, j2meunit.framework.TestMethod rTestMethod)
    {
        super(sTestName, rTestMethod);
    }


    public void runTest() throws TestEndThrowable
    {

        FieldFullException ffe1 = new FieldFullException();
        if (ffe1.getField() != -1)
        {
            failTest("Default field value returned by getField() was not -1!");
        }

        FieldFullException ffe2 = new FieldFullException("test");
        if (!(ffe2.getMessage().equals("test")))
        {
            failTest("Field given in constructor parameter was not returned by getField()!");
        }
        if (ffe2.getField() != -1)
        {
            failTest("Default field value returned by getField() was not -1!");
        }

        for (int i=0; i < 10; i++)
        {
            FieldFullException ffe = new FieldFullException("Testing Field:" + i, i);

            if (ffe.getField() != i)
            {
                failTest("Field given in constructor parameter was not returned by getField()!");
            }
            if (!(ffe.getMessage()).equals("Testing Field:" + i))
            {
                failTest("Field given in constructor parameter was not returned by getField()!");
            }
        }

        passTest();
    }
}