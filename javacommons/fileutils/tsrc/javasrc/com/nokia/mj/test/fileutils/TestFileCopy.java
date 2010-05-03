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


package com.nokia.mj.test.fileutils;

import java.io.IOException;
import java.io.OutputStream;
import java.io.InputStream;

import com.nokia.mj.impl.installer.utils.InstallerMain;
import com.nokia.mj.impl.fileutils.*;
import com.nokia.mj.test.fileutils.TestEnvironment;

import j2meunit.framework.Test;
import j2meunit.framework.TestCase;
import j2meunit.framework.TestMethod;
import j2meunit.framework.TestSuite;

/**
 * Tests FileUtility.createNewFile(). Ensure that you run after exists. Test
 * case assumes that exists() works fine.
 */
public class TestFileCopy extends TestCase implements InstallerMain
{
    String path;

    // Begin j2meunit test framework setup
    public void installerMain(String[] args)
    {
        TestSuite suite = new TestSuite(this.getClass().getName());

        suite.addTest(new TestFileCopy("testFileCopy", new TestMethod()
        {
            public void run(TestCase tc)
            {
                ((TestFileCopy) tc).testFileCopy();
            }
        }));

        com.nokia.mj.impl.utils.OmjTestRunner.run(suite);
    }

    public TestFileCopy()
    {
    }

    public TestFileCopy(String aTestName, TestMethod aTestMethod)
    {
        super(aTestName, aTestMethod);
    }

    public void assertFalse(String aMsg, boolean aCondition)
    {
        assertTrue(aMsg, !aCondition);
    }

    // End j2meunit test framework setup

    protected void setUp()
    {
        path = TestEnvironment.Main_Test_Folder + "testfilecopy/";
    }

    private void testFileCopy()
    {
        String oldFile = path+"inputFile.txt";
        String newFile = path+"outputFile.txt";
        try
        {
            FileUtility target = new FileUtility(oldFile);
            String contents = "abcdefghijklmnopqrstuvwxyz1234567890";

            if (!target.exists())
            {
                target.createNewFile();
            }

            OutputStream fos = target.openOutputStream();
            fos.write(contents.getBytes());
            fos.close();

            assertTrue("File Copy Failed",
                       FileOperations.copy(oldFile, newFile, false));

            FileUtility target1 = new FileUtility(path+"outputFile.txt");
            byte[] con = new byte[36];
            InputStream fis = target1.openInputStream();
            fis.read(con);
            String str = new String(con);

            assertTrue("Contents differ", str.equals(contents));
        }
        catch (Exception e)
        {
            assertTrue("Unexpected exception: "+e, false);
        }
    }
}
