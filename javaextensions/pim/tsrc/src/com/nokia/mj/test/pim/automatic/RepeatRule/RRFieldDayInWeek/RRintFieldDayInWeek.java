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
package com.nokia.mj.test.pim.automatic.RepeatRule.RRFieldDayInWeek;
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
public class RRintFieldDayInWeek extends RRintField
{

    public RRintFieldDayInWeek(
        String sTestName, j2meunit.framework.TestMethod rTestMethod)
    {
        super(sTestName, rTestMethod);
    }
    public void initTest() throws TestEndThrowable
    {
        field = 2;
        invalids = new int[] { -1, 0, 1, 16, 32, 100, 1000, 1025, 1300481 };
        valids = new int[] { 1024, 2048, 4096, 8192, 16384, 32768, 65536, 130048 };
    }
}