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


package com.nokia.mj.test.file.utils;

import j2meunit.framework.Test;
import j2meunit.framework.TestCase;
import j2meunit.framework.TestMethod;
import j2meunit.framework.TestSuite;

import com.nokia.mj.impl.installer.utils.InstallerMain;

public abstract class FileTestBase extends TestCase implements InstallerMain
{
    public FileTestBase()
    {
    }

    public FileTestBase(String aTestName, TestMethod aTestMethod)
    {
        super(aTestName, aTestMethod);
    }

    protected String iRoot = "C:/";

    protected boolean iMultiDriveSupportTests = false;

    protected void append(String msg)
    {
        System.out.println(msg);
    }

    public void assertTrue(String aMsg, boolean aCondition)
    {
        if ((aMsg != null) && (aCondition == false))
        {
            System.out.println(aMsg + " " + aCondition);
        }

        super.assertTrue(aMsg, aCondition);
    }

    protected void tearDown()
    {
        //Empty
    }
}
