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
package com.nokia.mj.test.pim.automatic.RepeatRule.RRFieldFrequency;
//IMPORTS
import java.io.IOException;
import java.util.*;
import javax.microedition.pim.RepeatRule;
import javax.microedition.pim.FieldEmptyException;
import com.nokia.mj.test.pim.automatic.RepeatRule.*;
import com.nokia.mj.test.pim.adaptation.*;
// CLASS DEFINITION
/**
 * This class tests javax.microedition.pim.RepeatRule.getInt().
 */
public class RRintFieldFrequency extends RRintField
{

    public RRintFieldFrequency(
        String sTestName, j2meunit.framework.TestMethod rTestMethod)
    {
        super(sTestName, rTestMethod);
    }

    public void initTest() throws TestEndThrowable
    {
        defaultVal = 16;
        field = 0;
        invalids = new int[] { -100, -1, 0, 1, 2, 14, 15, 20, 21, 100, 1000, 4096 };
        valids = new int[] { 16, 17, 18, 19 };
    }
}