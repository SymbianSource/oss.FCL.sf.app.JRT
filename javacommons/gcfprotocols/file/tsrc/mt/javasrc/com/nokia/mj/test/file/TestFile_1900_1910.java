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

import com.nokia.mj.test.file.utils.FileTestUtils;
import com.nokia.mj.test.file.utils.FileTestBase;
import com.nokia.mj.test.file.utils.TestFailedException;
import com.nokia.mj.impl.installer.utils.InstallerMain;

public class TestFile_1900_1910 extends FileTestBase
{
    public TestFile_1900_1910()
    {
    }

    public TestFile_1900_1910(String aTestName, TestMethod aTestMethod)
    {
        super(aTestName, aTestMethod);
    }

    public void assertFalse(String aMsg, boolean aCondition)
    {
        assertTrue(aMsg, !aCondition);
    }

    public static void registerTests(TestSuite suite)
    {
        suite.addTest(new TestFile_1900_1910("run_1900", new TestMethod()
        {
            public void run(TestCase tc)
            {
                ((TestFile_1900_1910) tc).run_1900();
            }
        }));

        suite.addTest(new TestFile_1900_1910("run_1910", new TestMethod()
        {
            public void run(TestCase tc)
            {
                ((TestFile_1900_1910) tc).run_1910();
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

    private void testList(String aPath, String aFilter) throws IOException
    {
        FileConnection conn1 = null;
        // Filter argument is empty.
        conn1 = FileTestUtils.openExistsURI(aPath, Connector.READ);
        if (conn1 == null)
            throw new TestFailedException();
        Enumeration fileList = conn1.list(aFilter, true);
        if (false == fileList.hasMoreElements())
            throw new TestFailedException();
        int elementCounter = 0;
        boolean failedFlag = false;
        while (fileList.hasMoreElements())
        {
            String obj = (String) fileList.nextElement();
            if (obj.equals("file100.txt"))
                ++elementCounter;
            else if (obj.equals("file100b.txt"))
                ++elementCounter;
            else if (obj.equals("file100d.txt"))
                ++elementCounter;
            else if (obj.equals("file100c.txt"))
                ++elementCounter;
            else if (obj.equals("subDir/"))
                ++elementCounter;
            else if (obj.equals("hiddenDir/"))
                ++elementCounter;
            else
                failedFlag = true;
        }// end while
        if (elementCounter != 6)
            throw new TestFailedException("elementCounter: " + elementCounter);
        if (failedFlag == true)
            throw new TestFailedException();
        conn1.close();
    }

    //
    //
    //
    //
    public void run_1900()
    {
        int counter = 0;
        try
        {
            testList("file:///" + iRoot + "mainTestDir/test100/", "");
            System.out.println(" Countter 1 ");
            counter = 1;
            testList("file:///" + iRoot + "mainTestDir/test100/", "*");
            System.out.println(" Countter 2 ");
            counter = 2;
            assertTrue(" run_1900() ok ", true);
            System.out.println(" run_1900() ok \n");
        }
        catch (Exception ex)
        {
            append(ex.toString());
            append("TEST FAILED!!! Case: run_1900() counter: " + counter + "\n");
            assertTrue(" run_1900() FAILED!!! Counter: " + counter, false);
        }
    }// end run_1900()
    //
    // //
    // //
    // //
    // //

    public void run_1910()
    {
        FileConnection conn1 = null;
        int counter = 0;
        try
        {
            conn1 = FileTestUtils.openExistsURI("file:///" + iRoot
                                                + "mainTestDir/test191/", Connector.READ);
            if (conn1 == null)
                throw new TestFailedException();
            Enumeration fileList = conn1.list("tem*", false);
            if (false == fileList.hasMoreElements())
                throw new TestFailedException();
            int elementCounter = 0;
            boolean failedFlag = false;
            while (fileList.hasMoreElements())
            {
                String obj = (String) fileList.nextElement();
                System.out.println("run_1910 Listed String: " + obj);
                if (obj.equals("tempora/"))
                    ++elementCounter;
                else if (obj.equals("temporar/"))
                    ++elementCounter;
                else if (obj.equals("temp.txt"))
                    ++elementCounter;
                else if (obj.equals("tempo.java"))
                    ++elementCounter;
                else if (obj.equals("tempor.cxx"))
                    ++elementCounter;
                else
                    failedFlag = true;
            }//end while
            if (elementCounter != 5)
                throw new TestFailedException();
            if (failedFlag == true)
                throw new TestFailedException();
            conn1.close();

            assertTrue(" run_1910() ok ", true);
            System.out.println(" run_1910() ok \n");
        }
        catch (Exception ex)
        {
            append(ex.toString());
            append("TEST FAILED!!! Case: run_1910() counter: " + counter + "\n");
            assertTrue(" run_1910() FAILED!!! Counter: " + counter, false);
        }
    }//end run_1910()

}
