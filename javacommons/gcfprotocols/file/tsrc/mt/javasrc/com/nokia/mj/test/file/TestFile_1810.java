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

/**
 * problem with test init framework. Creates files with wrong names
 */
public class TestFile_1810 extends FileTestBase
{
    public TestFile_1810()
    {
    }

    public TestFile_1810(String aTestName, TestMethod aTestMethod)
    {
        super(aTestName, aTestMethod);
    }

    public void assertFalse(String aMsg, boolean aCondition)
    {
        assertTrue(aMsg, !aCondition);
    }

    public static void registerTests(TestSuite suite)
    {
        suite.addTest(new TestFile_1810("run_1810", new TestMethod()
        {
            public void run(TestCase tc)
            {
                ((TestFile_1810) tc).run_1810();
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

    public void run_1810()
    {
        FileConnection conn1 = null;
        int counter = 0;
        try
        {
            conn1 = FileTestUtils.openExistsURI("file:///"+iRoot+"mainTestDir/test100/",Connector.READ);
            counter = 1;
            if (conn1 == null)
                throw new TestFailedException();
            counter = 2;
            Enumeration fileList = conn1.list();
            counter = 3;
            if (false == fileList.hasMoreElements())
                throw new TestFailedException();
            counter = 4;
            int elementCounter = 0;
            boolean failedFlag = false;
            while (fileList.hasMoreElements())
            {
                String obj = (String)fileList.nextElement();
                if (0 == obj.compareTo("file100.txt"))
                    ++elementCounter;
                else if (0 == obj.compareTo("file100b.txt"))
                    ++elementCounter;
                else if (0 == obj.compareTo("file100d.txt"))
                    ++elementCounter;
                else if (0 == obj.compareTo("subDir/"))
                    ++elementCounter;
                else
                    failedFlag = true;
            }//end while
            if (elementCounter != 4)
                throw new TestFailedException("elementCounter: " + elementCounter);
            counter = 5;
            if (failedFlag == true)
                throw new TestFailedException();
            conn1.close();

            System.out.println(" run_1810 #2 ");

            // 2
            conn1 = FileTestUtils.openExistsURI("file:///"+iRoot+"mainTestDir/test181/",Connector.READ);
            counter = 6;
            if (conn1 == null)
                throw new TestFailedException();
            counter = 7;
            fileList = conn1.list();
            System.out.println(" run_1810 Counter 7 after list");
            counter = 8;
            if (false == fileList.hasMoreElements())
                throw new TestFailedException();
            counter = 9;
            elementCounter = 0;
            failedFlag = false;
            while (fileList.hasMoreElements())
            {
                String obj = (String)fileList.nextElement();
                if (0 == obj.compareTo("someData"))
                    ++elementCounter;
                else if (0 == obj.compareTo("p��.���"))
                    ++elementCounter;
                else if (0 == obj.compareTo("p��/"))
                    ++elementCounter;
                else
                    failedFlag = true;
            }//end while
            if (elementCounter != 3)
                throw new TestFailedException("elementCounter: " + elementCounter);
            counter = 10;
            if (failedFlag == true)
                throw new TestFailedException();
            conn1.close();
            System.out.println(" run_1810 #3 ");
            // 3
            //conn1 = openExistsURI("file:///"+iRoot+"mainTestDir/test103/",Connector.READ);
            conn1 = FileTestUtils.openExistsURI("file:///"+iRoot+"data/Videos/drmFiles/testDir001",Connector.READ);
            if (conn1 == null)
                throw new TestFailedException();
            counter = 11;
            fileList = conn1.list();
            counter = 12;
            if (false == fileList.hasMoreElements())
                throw new TestFailedException();
            counter = 13;
            elementCounter = 0;
            failedFlag = false;
            while (fileList.hasMoreElements())
            {
                String obj = (String)fileList.nextElement();
                if (0 == obj.compareTo("drmfile015_001DecPlayCount3.dcf"))
                    ++elementCounter;
                else if (0 == obj.compareTo("drmfile015_001DecPlayCount3.dr"))
                    ++elementCounter;
                else if (0 == obj.compareTo("something.jad"))
                    ++elementCounter;
                else if (0 == obj.compareTo("something.jar"))
                    ++elementCounter;
                else
                    failedFlag = true;
            }//end while
            if (elementCounter != 4)
                throw new TestFailedException("elementCounter: " + elementCounter);
            counter = 14;
            if (failedFlag == true)
                throw new TestFailedException();
            conn1.close();

            assertTrue(" run_1810() ok ", true);
            System.out.println(" run_1810() ok \n");
        }
        catch (Exception ex)
        {
            append(ex.toString());
            append("TEST FAILED!!! Case: run_1810() counter: " + counter + "\n");
            assertTrue(" run_1810() FAILED!!! Counter: "+counter, false);
        }
    }//end run_1810()

}
