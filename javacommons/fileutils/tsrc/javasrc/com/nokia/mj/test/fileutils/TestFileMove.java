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
import java.util.Vector;

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
public class TestFileMove extends TestCase implements InstallerMain
{
    String path;

    // Begin j2meunit test framework setup
    public void installerMain(String[] args)
    {
        TestSuite suite = new TestSuite(this.getClass().getName());

        suite.addTest(new TestFileMove("testFileMove", new TestMethod()
        {
            public void run(TestCase tc)
            {
                ((TestFileMove) tc).testFileMove();
            }
        }));

        suite.addTest(new TestFileMove("testDirMove", new TestMethod()
        {
            public void run(TestCase tc)
            {
                ((TestFileMove) tc).testDirMove();
            }
        }));

        com.nokia.mj.impl.utils.OmjTestRunner.run(suite);
    }

    public TestFileMove()
    {
    }

    public TestFileMove(String aTestName, TestMethod aTestMethod)
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
        path = TestEnvironment.Main_Test_Folder + "testfilemove/";
    }

    private void matchEnumWithVector(java.util.Enumeration source,
                                     Vector expectedList)
    {
        System.out.println("Starting compare");
        while (source.hasMoreElements())
        {
            String file = (String) source.nextElement();
            if (expectedList.contains(file))
            {
                System.out.println("File Found: "+file);
                expectedList.remove(file);
            }
            else
            {
                assertTrue("List contents wrong", false);
            }
        }
        assertTrue("All elements not returned: Elements Left: "+expectedList.size(), 0 == expectedList.size());
    }

    private void testDirMove()
    {
        String oldDir = path+"folderToMove";
        String newDir = path+"movedFolder";

        try
        {
            FileUtility oFile = new FileUtility(oldDir);
            FileUtility nFile = new FileUtility(newDir);

            String[] fList = { "folder1/", "folder2/", "file1.txt",
                               "file2.txt", "file3.txt", "file4.txt"
                             };

            Vector expectedList = new Vector();
            for (int i = 0; i < fList.length; i++)
            {
                expectedList.add(fList[i]);
            }

            assertTrue("Copy All Failed",
                       FileOperations.move(oldDir, newDir, true));

            assertTrue("Destination does not exist", nFile.exists());
            java.util.Enumeration destContents = nFile.list();
            System.out.println("Enumerating "+nFile.getAbsolutePath());
            matchEnumWithVector(destContents, expectedList);

        }
        catch (Exception e)
        {
            assertTrue("Unexpected exception: "+e, false);
        }
    }


    private void testFileMove()
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

            assertTrue("File Move Failed",
                       FileOperations.move(oldFile, newFile, false));

            FileUtility target1 = new FileUtility(path+"outputFile.txt");
            byte[] con = new byte[36];
            InputStream fis = target1.openInputStream();
            fis.read(con);
            String str = new String(con);
            fis.close();

            assertTrue("Contents differ", str.equals(contents));
            assertFalse("Old file existed", target.exists());
        }
        catch (Exception e)
        {
            assertTrue("Unexpected exception: "+e, false);
        }
    }
}
