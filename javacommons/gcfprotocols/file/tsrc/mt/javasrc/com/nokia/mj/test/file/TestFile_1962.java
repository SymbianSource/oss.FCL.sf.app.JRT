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

public class TestFile_1962 extends FileTestBase
{
    public TestFile_1962()
    {
    }

    public TestFile_1962(String aTestName, TestMethod aTestMethod)
    {
        super(aTestName, aTestMethod);
    }

    public void assertFalse(String aMsg, boolean aCondition)
    {
        assertTrue(aMsg, !aCondition);
    }

    public static void registerTests(TestSuite suite)
    {

        suite.addTest(new TestFile_1962("run_1962", new TestMethod()
        {
            public void run(TestCase tc)
            {
                ((TestFile_1962) tc).run_1962();
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

    public void run_1962()
    {
        FileConnection conn1 = null;
        InputStream stream = null;
        int counter = 0;
        try
        {
            // 1
            conn1 =(FileConnection)Connector.open("file:///Internal/");
            Enumeration dirList = conn1.list("* ",true);
            if (false == dirList.hasMoreElements())
                throw new TestFailedException();
            counter = 2;
            int elementCounter = 0;
            boolean failedFlag = false;
            while (dirList.hasMoreElements())
            {
                String obj = (String)dirList.nextElement();
                obj = obj.toLowerCase();
                //append(obj);
                if (obj.equals("images/"))
                    ++elementCounter;
                else if (obj.equals("videos/"))
                    ++elementCounter;
                else if (obj.equals("graphics/"))
                    ++elementCounter;
                else if (obj.equals("sounds/"))
                    ++elementCounter;
                else if (obj.equals("music/"))
                    ++elementCounter;
                else if (obj.equals("recordings/"))
                    ++elementCounter;
                else if (obj.equals("private/"))
                    ++elementCounter;
                else if (obj.equals("data/"))
                    ++elementCounter;
                else if (obj.equals("system/"))
                    ++elementCounter;
                else if (obj.equals("maintestdir/"))
                    ++elementCounter;
            }//end while
            counter = 3;
            //There can be also other directories but it is enough to check
            //that at least these four ones can be found.
            if (elementCounter < 10)
                throw new TestFailedException("elementCounter: " + elementCounter);
            counter = 4;
            conn1.close();

            conn1 =(FileConnection)Connector.open("file:///Internal/");
            dirList = conn1.list("",true);
            if (false == dirList.hasMoreElements())
                throw new TestFailedException();
            counter = 20;
            elementCounter = 0;
            failedFlag = false;
            while (dirList.hasMoreElements())
            {
                String obj = (String)dirList.nextElement();
                obj = obj.toLowerCase();
                //append(obj);
                if (obj.equals("images/"))
                    ++elementCounter;
                else if (obj.equals("videos/"))
                    ++elementCounter;
                else if (obj.equals("graphics/"))
                    ++elementCounter;
                else if (obj.equals("sounds/"))
                    ++elementCounter;
                else if (obj.equals("music/"))
                    ++elementCounter;
                else if (obj.equals("recordings/"))
                    ++elementCounter;
                else if (obj.equals("private/"))
                    ++elementCounter;
                else if (obj.equals("data/"))
                    ++elementCounter;
                else if (obj.equals("system/"))
                    ++elementCounter;
                else if (obj.equals("maintestdir/"))
                    ++elementCounter;
            }//end while
            counter = 21;
            //There can be also other directories but it is enough to check
            //that at least these four ones can be found.
            if (elementCounter < 10)
                throw new TestFailedException("elementCounter: " + elementCounter);
            counter = 22;
            conn1.close();


            // 2
            conn1 =(FileConnection)Connector.open("file:///Internal/private/new dir");
            counter = 5;
            conn1.mkdir();
            counter = 6;
            conn1.setFileConnection("..");
            dirList = conn1.list("New%20*",true);
            if (false == dirList.hasMoreElements())
                throw new TestFailedException();
            counter = 7;
            elementCounter = 0;
            failedFlag = false;
            while (dirList.hasMoreElements())
            {
                String obj = (String)dirList.nextElement();
                obj = obj.toLowerCase();
                //append(obj);
                if (obj.equals("new dir/"))
                    ++elementCounter;
                else
                    failedFlag = true;
            }//end while
            counter = 8;
            //There can be also other directories but it is enough to check
            //that at least these four ones can be found.
            if (elementCounter != 1)
                throw new TestFailedException("elementCounter: " + elementCounter);
            if (failedFlag)
                throw new TestFailedException();
            counter = 9;
            conn1.setFileConnection("new DIR/");
            counter = 10;
            conn1.delete();
            conn1.close();

            assertTrue(" run_1962() ok \n", true);
        }
        catch (Exception ex)
        {
            append(ex.toString());
            append("TEST FAILED!!!: run_1962() counter: " + counter + "\n");
            assertTrue(" run_1962() FAILED!!! Counter: "+counter, false);
            return;
        }
    }//end run_1962()

}
