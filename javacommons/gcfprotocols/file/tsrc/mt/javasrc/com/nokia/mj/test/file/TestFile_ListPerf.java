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

package com.nokia.mj.test.file;

import j2meunit.framework.Test;
import j2meunit.framework.TestCase;
import j2meunit.framework.TestMethod;
import j2meunit.framework.TestSuite;

import java.util.*;
import java.io.*;
import java.io.IOException;

import javax.microedition.io.file.*;
import javax.microedition.io.*;

import com.nokia.mj.test.file.utils.TestFailedException;
import com.nokia.mj.test.file.utils.FileTestUtils;
import com.nokia.mj.test.file.utils.FileTestBase;
import com.nokia.mj.impl.installer.utils.InstallerMain;

/**
 * Not to be run currently. Implementation for DRM file access is currently not
 * done. So all tests will fail.
 *
 */
public class TestFile_ListPerf extends FileTestBase
{
    public TestFile_ListPerf()
    {
    }

    public TestFile_ListPerf(String aTestName, TestMethod aTestMethod)
    {
        super(aTestName, aTestMethod);
    }

    public void assertFalse(String aMsg, boolean aCondition)
    {
        assertTrue(aMsg, !aCondition);
    }

    public static void registerTests(TestSuite suite)
    {
        suite.addTest(new TestFile_ListPerf("testListPerf", new TestMethod()
        {
            public void run(TestCase tc)
            {
                ((TestFile_ListPerf) tc).testListPerf();
            }
        }));
    }


    // Begin j2meunit test framework setup
    public void installerMain(String[] args)
    {
        TestSuite suite = new TestSuite(this.getClass().getName());
        registerTests(suite);
        com.nokia.mj.impl.utils.OmjTestRunner.run(suite);
    }

    public void testListPerf()
    {
        try
        {
            FileConnection conn = (FileConnection) Connector.open("file:///C:/Data/Images/results/");
            System.out.println("Calling List At: "+System.currentTimeMillis());
            conn.list();
            System.out.println("List rreturned At: "+System.currentTimeMillis());
            conn.close();
        }
        catch (Exception ex)
        {
            ex.printStackTrace();
        }
    }
}