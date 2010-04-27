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

import javax.microedition.io.*;
import javax.microedition.io.file.*;

import com.nokia.mj.impl.installer.utils.InstallerMain;

import com.nokia.mj.test.file.utils.TestFailedException;
import com.nokia.mj.impl.rt.support.ApplicationInfo;
import com.nokia.mj.test.file.utils.FileTestUtils;
import com.nokia.mj.test.file.utils.FileTestBase;
import com.nokia.mj.test.file.utils.FileSecurityUtils;

/**
 * Test File Security
 *
 */
public class TestFile_Security_Manufacturer extends FileTestBase
{
    public TestFile_Security_Manufacturer()
    {
    }

    public TestFile_Security_Manufacturer(String aTestName, TestMethod aTestMethod)
    {
        super(aTestName, aTestMethod);
    }

    public void assertFalse(String aMsg, boolean aCondition)
    {
        assertTrue(aMsg, !aCondition);
    }

    public static void registerTests(TestSuite suite)
    {
        suite.addTest(new TestFile_Security_Manufacturer("testForbiddenPaths", new TestMethod()
        {
            public void run(TestCase tc)
            {
                ((TestFile_Security_Manufacturer) tc).testForbiddenPaths();
            }
        }));

        suite.addTest(new TestFile_Security_Manufacturer("testIllegalAccessToRestrictedPath", new TestMethod()
        {
            public void run(TestCase tc)
            {
                ((TestFile_Security_Manufacturer) tc).testIllegalAccessToRestrictedPath();
            }
        }));

        suite.addTest(new TestFile_Security_Manufacturer("testCreateInRestrictedPath", new TestMethod()
        {
            public void run(TestCase tc)
            {
                ((TestFile_Security_Manufacturer) tc).testCreateInRestrictedPath();
            }
        }));

        suite.addTest(new TestFile_Security_Manufacturer("testCreateAboveRestrictedPath", new TestMethod()
        {
            public void run(TestCase tc)
            {
                ((TestFile_Security_Manufacturer) tc).testCreateAboveRestrictedPath();
            }
        }));

    }

    public Test suite()
    {
        TestSuite aSuite = new TestSuite();
        registerTests(aSuite);
        return aSuite;
    }

    // Begin j2meunit test framework setup
    public void installerMain(String[] args)
    {
        TestSuite suite = new TestSuite(this.getClass().getName());
        registerTests(suite);
        com.nokia.mj.impl.utils.OmjTestRunner.run(suite);
    }

    /**
     * Description: Tries to open a connection to one of the forbidden paths.
     * Attempt to open should fail with a security exception.
     *
     */
    public void testForbiddenPaths()
    {
        Vector pathList = FilePathList.getForbiddenPathList();
        for (int i = 0; i < pathList.size(); i++)
        {
            String fileUrl = "file:///" + pathList.elementAt(i);
            try
            {
                FileSecurityUtils.checkSecurityExceptionOnOpen(fileUrl);
            }
            catch (Exception ex)
            {
                assertTrue("Security test on forbidden path failed. URL: "
                           + fileUrl, false);
            }
        }
    }

    /**
     * Description: Tries to open a connection to one of the restricted paths in
     * read-write mode and perform write operation. <br/> Connection should open
     * but any attempt to perform a write operation must throw a security
     * exception.
     *
     */
    public void testIllegalAccessToRestrictedPath()
    {
        Vector pathList = FilePathList.getRestrictedPathList();
        for (int i = 0; i < pathList.size(); i++)
        {
            String fileUrl = "file:///" + pathList.elementAt(i);
            try
            {
                // Check setReadable
                FileSecurityUtils
                .checkSecurityExceptionOnSetReadableOperation(fileUrl);

                // Check setWritable
                FileSecurityUtils
                .checkSecurityExceptionOnSetWritableOperation(fileUrl);

                // Check setWritable
                FileSecurityUtils
                .checkSecurityExceptionOnSetHiddenOperation(fileUrl);

                // Check rename
                FileSecurityUtils
                .checkSecurityExceptionOnRenameOperation(fileUrl);

                // Check delete
                FileSecurityUtils
                .checkSecurityExceptionOnDeleteOperation(fileUrl);
            }
            catch (TestFailedException ex)
            {
                assertTrue("Illegal Access to Restricted path failed. Path: "
                           + fileUrl + " " + ex, false);
            }
        }
    }

    /**
     * Description: Tries to open a connection to one of the restricted paths in
     * read-write mode and create a file within it.
     *
     */
    public void testCreateInRestrictedPath()
    {
        Vector pathList = FilePathList.getRestrictedPathList();
        for (int i = 0; i < pathList.size(); i++)
        {
            String fileUrl = "file:///" + pathList.elementAt(i) + "dummyFile";
            try
            {
                FileConnection conn = (FileConnection) Connector.open(fileUrl);
                conn.create();
                // Create should succeed.

                System.out.println("Creating " + fileUrl);
                assertTrue("File not created: " + fileUrl, conn.exists());

                conn.delete();
                // Delete should also succeed.

                assertFalse("File not deleted: " + fileUrl, conn.exists());
                conn.close();
            }
            catch (Exception ex)
            {
                assertTrue("Unexpected Exception: URL: " + fileUrl, false);
            }
        }
    }

    /**
     * Tries to create files in place where it is not allowed.
     *
     * Manufacturer domain midlets are allowed to create.
     *
     */
    public void testCreateAboveRestrictedPath()
    {
        ApplicationInfo info = ApplicationInfo.getInstance();
        String domain = info.getProtectionDomain();

        boolean expectSecurityException = !(domain
                                            .equals(ApplicationInfo.MANUFACTURER_DOMAIN));
        Vector fileList = FilePathList.getUpperRestrictedPathList();

        for (int i = 0; i < fileList.size(); i++)
        {
            String fileUrl = "file:///" + fileList.elementAt(i)
                             + "notToBeCreated";
            try
            {
                FileConnection conn = (FileConnection) Connector.open(fileUrl);
                conn.create();
                System.out.println("create done...");
                // It should not come here. If it has, delete the file
                // created.
                conn.delete();
                System.out.println("delete done...");

                if (expectSecurityException)
                {
                    assertTrue("Security test failed. Create succeeded. "
                               + fileUrl, false);
                }
            }
            catch (SecurityException ex)
            {
                System.out.println("Security exception thrown :-)");
                if (!expectSecurityException)
                {
                    assertTrue("Security test failed. Create failed. "
                               + fileUrl, false);
                }
            }
            catch (Exception ex)
            {
                ex.printStackTrace();
                assertTrue("Security test failed on: " + fileUrl, false);
            }
        }
    }
}

class FilePathList
{
    public static Vector getForbiddenPathList()
    {
        Vector list = new Vector();
        // list.addElement("C:/System/");
        list.addElement("D:/System/");
        list.addElement("Z:/System/");
        return list;
    }

    public static Vector getUpperRestrictedPathList()
    {
        Vector list = new Vector();
        list.addElement("C:/");
        list.addElement("C:/Data/");
        return list;
    }

    public static Vector getRestrictedPathList()
    {
        Vector list = new Vector();
        list.addElement("C:/Data/Images/");
        list.addElement("C:/Data/Images/Pictures/");
        //        list.addElement("C:/Data/Images/Presence/");
        //        list.addElement("C:/Data/Images/Backgrounds/");
        list.addElement("C:/Data/Others/");
        list.addElement("C:/Data/Videos/");
        list.addElement("C:/Data/Installs/");
        list.addElement("C:/Data/Games/");
        list.addElement("C:/Data/Sounds/");
        list.addElement("C:/Data/Sounds/Digital/");
        list.addElement("C:/Data/Sounds/Simple/");
        return list;
    }
}