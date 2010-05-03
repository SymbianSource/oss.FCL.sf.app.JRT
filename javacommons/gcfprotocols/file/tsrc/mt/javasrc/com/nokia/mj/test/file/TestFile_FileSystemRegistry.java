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

import com.nokia.mj.test.file.utils.TestFailedException;
import com.nokia.mj.test.file.utils.FileTestUtils;
import com.nokia.mj.test.file.utils.FileTestBase;
import com.nokia.mj.impl.installer.utils.InstallerMain;

/**
 * Test FileSystemRegistry class.
 *
 */
public class TestFile_FileSystemRegistry extends FileTestBase
{
    public TestFile_FileSystemRegistry()
    {
    }

    public TestFile_FileSystemRegistry(String aTestName, TestMethod aTestMethod)
    {
        super(aTestName, aTestMethod);
    }

    public void assertFalse(String aMsg, boolean aCondition)
    {
        assertTrue(aMsg, !aCondition);
    }

    public static void registerTests(TestSuite suite)
    {
        suite.addTest(new TestFile_FileSystemRegistry("testListRoots", new TestMethod()
        {
            public void run(TestCase tc)
            {
                ((TestFile_FileSystemRegistry) tc).testListRoots();
            }
        }));

        suite.addTest(new TestFile_FileSystemRegistry("testAddListener", new TestMethod()
        {
            public void run(TestCase tc)
            {
                ((TestFile_FileSystemRegistry) tc).testAddListener();
            }
        }));

        suite.addTest(new TestFile_FileSystemRegistry("testRemoveListener", new TestMethod()
        {
            public void run(TestCase tc)
            {
                ((TestFile_FileSystemRegistry) tc).testRemoveListener();
            }
        }));

        suite.addTest(new TestFile_FileSystemRegistry("testAddListenerNull", new TestMethod()
        {
            public void run(TestCase tc)
            {
                ((TestFile_FileSystemRegistry) tc).testAddListenerNull();
            }
        }));

        suite.addTest(new TestFile_FileSystemRegistry("testRemoveListenerNull", new TestMethod()
        {
            public void run(TestCase tc)
            {
                ((TestFile_FileSystemRegistry) tc).testRemoveListenerNull();
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

    class TestListener implements FileSystemListener
    {
        public void rootChanged(int state, String rootName)
        {
            System.out.println("Root Changed: "+rootName+" State: "+state);
        }
    }

    public void testListRoots()
    {
        Enumeration fileList = FileSystemRegistry.listRoots();
        assertTrue("List roots returned empty", fileList.hasMoreElements());
    }

    public void testAddListener()
    {
        TestListener listener = new TestListener();
        assertTrue("Registering listener failed.", FileSystemRegistry.addFileSystemListener(listener));
    }

    public void testRemoveListener()
    {
        TestListener listener = new TestListener();
        assertTrue("Registering listener failed.", FileSystemRegistry.addFileSystemListener(listener));
        assertTrue("Unregistering listener failed.", FileSystemRegistry.removeFileSystemListener(listener));
    }

    public void testAddListenerNull()
    {
        try
        {
            assertTrue("Registering listener failed.",
                       FileSystemRegistry.addFileSystemListener(null));
        }
        catch (NullPointerException e)
        {
            // System.out.println("Good. Nullpointer Exception thrown");
            // Test Passed
        }
        catch (Exception ex)
        {
            assertTrue("Registering Listener threw unexpected exception: "+ex,
                       false);
        }
    }

    public void testRemoveListenerNull()
    {
        try
        {
            assertTrue("Unregistering listener failed.",
                       FileSystemRegistry.removeFileSystemListener(null));
        }
        catch (NullPointerException e)
        {
            // System.out.println("Good. Nullpointer Exception thrown");
            // Test Passed
        }
        catch (Exception ex)
        {
            assertTrue("Registering Listener threw unexpected exception: "+ex,
                       false);
        }
    }
}