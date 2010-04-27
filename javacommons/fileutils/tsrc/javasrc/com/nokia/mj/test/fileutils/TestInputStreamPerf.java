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

import java.io.InputStreamReader;
import java.io.ByteArrayInputStream;
import java.io.File;
import java.io.IOException;

import com.nokia.mj.impl.installer.utils.InstallerMain;
import com.nokia.mj.impl.fileutils.*;

import j2meunit.framework.Test;
import j2meunit.framework.TestCase;
import j2meunit.framework.TestMethod;
import j2meunit.framework.TestSuite;

/**
 * FileURL unit tests.
 */
public class TestInputStreamPerf extends TestCase implements InstallerMain
{
    String separator;

    // Begin j2meunit test framework setup
    public void installerMain(String[] args)
    {
        TestSuite suite = new TestSuite(this.getClass().getName());

        suite.addTest(new TestInputStreamPerf("testTimingByteArray",
                                              new TestMethod()
        {
            public void run(TestCase tc)
            {
                ((TestInputStreamPerf) tc).testTimingByteArray();
            }
        }));

        com.nokia.mj.impl.utils.OmjTestRunner.run(suite);
    }

    public TestInputStreamPerf()
    {
        separator = System.getProperty("file.separator");
    }

    public TestInputStreamPerf(String aTestName, TestMethod aTestMethod)
    {
        super(aTestName, aTestMethod);
        separator = System.getProperty("file.separator");
    }

    public void assertFalse(String aMsg, boolean aCondition)
    {
        assertTrue(aMsg, !aCondition);
    }

    // End j2meunit test framework setup

    protected void setUp()
    {
    }

    protected void tearDown()
    {
    }

    public void testTimingByteArray()
    {
        File fileCdc = new File("C:\\Data\\Images\\readFile.txt");

        byte[] bArray = new byte[100];

        long cdcRead=0;


        long cdcStart = 0;
        long cdcEnd = 0;
        try
        {
            java.io.FileInputStream fis = new java.io.FileInputStream("C:\\Data\\Images\\readFile.txt");
            cdcStart = System.currentTimeMillis();

            int ch = 0;
            while (ch != -1)
            {
                ch = fis.read(bArray);
                cdcRead+=ch;
            }
            cdcEnd = System.currentTimeMillis();
            fis.close();
        }
        catch (Exception e)
        {
            assertTrue("Failed to measure cdc file", false);
        }

        long cldcStart = 0;
        long cldcEnd = 0;
        long cldcRead=0;
        try
        {
            FileUtility fex = new FileUtility("C:\\Data\\Images\\readFile.txt");
            java.io.InputStream fileInput = fex.openInputStream();
            cldcStart = System.currentTimeMillis();

            int ch = 0;
            while (ch != -1)
            {
                ch = fileInput.read(bArray);
                cldcRead+= ch;
            }
            cldcEnd = System.currentTimeMillis();
            fileInput.close();

        }
        catch (Exception e)
        {
            assertTrue("Failed to measure file extended", false);
        }

        System.out.println(" TOTAL CDC TIME: "+(cdcEnd-cdcStart)+" CDC Read: "+cdcRead);
        System.out.println(" TOTAL CLDC TIME: "+(cldcEnd-cldcStart)+ " CLDC Read: "+ cldcRead);

    }


    public void testTimingSingleByte()
    {
        File fileCdc = new File("C:\\Data\\Images\\readFile.txt");

        long cdcStart = 0;
        long cdcEnd = 0;
        try
        {
            java.io.FileInputStream fis = new java.io.FileInputStream("C:\\Data\\Images\\readFile.txt");
            cdcStart = System.currentTimeMillis();

            int ch = 0;
            while (ch != -1)
            {
                ch = fis.read();
            }
            cdcEnd = System.currentTimeMillis();
            fis.close();
        }
        catch (Exception e)
        {
            assertTrue("Failed to measure cdc file", false);
        }

        long cldcStart = 0;
        long cldcEnd = 0;
        try
        {
            FileUtility fex = new FileUtility("C:\\Data\\Images\\readFile.txt");
            java.io.InputStream fileInput = fex.openInputStream();
            cldcStart = System.currentTimeMillis();

            int ch = 0;
            while (ch != -1)
            {
                ch = fileInput.read();
            }
            cldcEnd = System.currentTimeMillis();
            fileInput.close();

        }
        catch (Exception e)
        {
            assertTrue("Failed to measure file extended", false);
        }

        System.out.println(" TOTAL CDC TIME: "+(cdcEnd-cdcStart));
        System.out.println(" TOTAL CLDC TIME "+(cldcEnd-cldcStart));

    }

    public void testCanonicalPath()
    {
        FileUtility fileEx = new FileUtility("C:\\Data\\Images\\readFile.txt");

        try
        {
            java.io.DataInputStream stream = fileEx.openDataInputStream();

            int ch = 0;
            while (ch!=-1)
            {
                ch = stream.read();
            }
        }
        catch (Exception e)
        {
            e.printStackTrace();
            assertTrue("Test failed: "+e, false);
        }
    }

}