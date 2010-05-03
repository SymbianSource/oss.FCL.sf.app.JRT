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
package com.nokia.mj.test.pim.automatic.RepeatRule.RRFieldInterval;
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
public class RRintFieldInterval extends RRintField
{

    public RRintFieldInterval(
        String sTestName, j2meunit.framework.TestMethod rTestMethod)
    {
        super(sTestName, rTestMethod);
    }

    public void initTest() throws TestEndThrowable
    {
        field = 128;
        invalids = new int[] { -100, -1, 0 };
        valids = new int[] { 1, 2, 3, 4, 100, 1000, 4096 };
    }
}