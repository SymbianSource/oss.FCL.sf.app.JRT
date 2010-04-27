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


package com.nokia.mj.impl.installer.integrityservice;

import com.nokia.mj.impl.fileutils.FileUtility;
import com.nokia.mj.impl.installer.utils.FileUtils;
import com.nokia.mj.impl.installer.utils.InstallerMain;
import com.nokia.mj.impl.installer.utils.Log;

import java.io.File;
import java.io.InputStream;
import java.io.IOException;
import java.io.OutputStream;
import java.util.Vector;

import j2meunit.framework.Test;
import j2meunit.framework.TestCase;
import j2meunit.framework.TestMethod;
import j2meunit.framework.TestSuite;

/**
 * IntegrityService unit tests.
 */
public class IntegrityServiceTest extends TestCase implements InstallerMain
{
    /**
     * Directory for IntegrityService tests.
     */
    private static final String iTestRoot =
        FileUtils.getInstallerRoot() + "istest";

    /**
     * Directory for IntegrityService journal and temp files.
     */
    private static final String iIsRoot = iTestRoot + FileUtils.pathSeparator() + "is" + FileUtils.pathSeparator();
    private static final String iIsRoot2 = iTestRoot + FileUtils.pathSeparator() + "is2" + FileUtils.pathSeparator();

    // Begin j2meunit test framework setup

    public void installerMain(String[] args)
    {
        TestSuite suite = new TestSuite(this.getClass().getName());

        suite.addTest(new IntegrityServiceTest("testStaticRollback", new TestMethod()
        {
            public void run(TestCase tc)
            {
                ((IntegrityServiceTest)tc).testStaticRollback();
            }
        }));
        suite.addTest(new IntegrityServiceTest("testStaticRollbackNop", new TestMethod()
        {
            public void run(TestCase tc)
            {
                ((IntegrityServiceTest)tc).testStaticRollbackNop();
            }
        }));
        suite.addTest(new IntegrityServiceTest("testNopCommit", new TestMethod()
        {
            public void run(TestCase tc)
            {
                ((IntegrityServiceTest)tc).testNopCommit();
            }
        }));
        suite.addTest(new IntegrityServiceTest("testNopRollback", new TestMethod()
        {
            public void run(TestCase tc)
            {
                ((IntegrityServiceTest)tc).testNopRollback();
            }
        }));
        suite.addTest(new IntegrityServiceTest("testGetInstanceTwiceInRow", new TestMethod()
        {
            public void run(TestCase tc)
            {
                ((IntegrityServiceTest)tc).testGetInstanceTwiceInRow();
            }
        }));
        suite.addTest(new IntegrityServiceTest("testGetInstanceTwice", new TestMethod()
        {
            public void run(TestCase tc)
            {
                ((IntegrityServiceTest)tc).testGetInstanceTwice();
            }
        }));
        suite.addTest(new IntegrityServiceTest("testGetTwoInstances", new TestMethod()
        {
            public void run(TestCase tc)
            {
                ((IntegrityServiceTest)tc).testGetTwoInstances();
            }
        }));
        suite.addTest(new IntegrityServiceTest("testCommitTwice", new TestMethod()
        {
            public void run(TestCase tc)
            {
                ((IntegrityServiceTest)tc).testCommitTwice();
            }
        }));
        suite.addTest(new IntegrityServiceTest("testRollbackTwice", new TestMethod()
        {
            public void run(TestCase tc)
            {
                ((IntegrityServiceTest)tc).testRollbackTwice();
            }
        }));
        suite.addTest(new IntegrityServiceTest("testCreateCommit", new TestMethod()
        {
            public void run(TestCase tc)
            {
                ((IntegrityServiceTest)tc).testCreateCommit();
            }
        }));
        suite.addTest(new IntegrityServiceTest("testCreateRollback", new TestMethod()
        {
            public void run(TestCase tc)
            {
                ((IntegrityServiceTest)tc).testCreateRollback();
            }
        }));
        suite.addTest(new IntegrityServiceTest("testCreateDirCommit", new TestMethod()
        {
            public void run(TestCase tc)
            {
                ((IntegrityServiceTest)tc).testCreateDirCommit();
            }
        }));
        suite.addTest(new IntegrityServiceTest("testCreateDirRollback", new TestMethod()
        {
            public void run(TestCase tc)
            {
                ((IntegrityServiceTest)tc).testCreateDirRollback();
            }
        }));
        suite.addTest(new IntegrityServiceTest("testCreateTempFileCommit", new TestMethod()
        {
            public void run(TestCase tc)
            {
                ((IntegrityServiceTest)tc).testCreateTempFileCommit();
            }
        }));
        suite.addTest(new IntegrityServiceTest("testCreateTempFileRollback", new TestMethod()
        {
            public void run(TestCase tc)
            {
                ((IntegrityServiceTest)tc).testCreateTempFileRollback();
            }
        }));
        suite.addTest(new IntegrityServiceTest("testDeleteCommit", new TestMethod()
        {
            public void run(TestCase tc)
            {
                ((IntegrityServiceTest)tc).testDeleteCommit();
            }
        }));
        suite.addTest(new IntegrityServiceTest("testDeleteRollback", new TestMethod()
        {
            public void run(TestCase tc)
            {
                ((IntegrityServiceTest)tc).testDeleteRollback();
            }
        }));
        suite.addTest(new IntegrityServiceTest("testCopyCommit", new TestMethod()
        {
            public void run(TestCase tc)
            {
                ((IntegrityServiceTest)tc).testCopyCommit();
            }
        }));
        suite.addTest(new IntegrityServiceTest("testCopyRollback", new TestMethod()
        {
            public void run(TestCase tc)
            {
                ((IntegrityServiceTest)tc).testCopyRollback();
            }
        }));
        suite.addTest(new IntegrityServiceTest("testMoveCommit", new TestMethod()
        {
            public void run(TestCase tc)
            {
                ((IntegrityServiceTest)tc).testMoveCommit();
            }
        }));
        suite.addTest(new IntegrityServiceTest("testMoveRollback", new TestMethod()
        {
            public void run(TestCase tc)
            {
                ((IntegrityServiceTest)tc).testMoveRollback();
            }
        }));
        suite.addTest(new IntegrityServiceTest("testUpdateCommit", new TestMethod()
        {
            public void run(TestCase tc)
            {
                ((IntegrityServiceTest)tc).testUpdateCommit();
            }
        }));
        suite.addTest(new IntegrityServiceTest("testUpdateRollback", new TestMethod()
        {
            public void run(TestCase tc)
            {
                ((IntegrityServiceTest)tc).testUpdateRollback();
            }
        }));
        suite.addTest(new IntegrityServiceTest("testOpSeqCommit", new TestMethod()
        {
            public void run(TestCase tc)
            {
                ((IntegrityServiceTest)tc).testOpSeqCommit();
            }
        }));
        suite.addTest(new IntegrityServiceTest("testOpSeqRollback", new TestMethod()
        {
            public void run(TestCase tc)
            {
                ((IntegrityServiceTest)tc).testOpSeqRollback();
            }
        }));

        com.nokia.mj.impl.utils.OmjTestRunner.run(suite);
    }

    public IntegrityServiceTest()
    {
    }

    public IntegrityServiceTest(String aTestName, TestMethod aTestMethod)
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
        // Clean up test folder before test case execution
        // to ensure that failing test cases do not cause
        // remaining test cases to fail.
        delete(iTestRoot);
    }

    protected void tearDown()
    {
        // Clean up test folder before next test case execution
        // to ensure that failing test cases do not cause
        // remaining test cases to fail.
        delete(iTestRoot);
    }

    public void testStaticRollback()
    {
        // Generate journal and temp files to iIsRoot.
        IntegrityService is = IntegrityService.getInstance(iIsRoot);
        assertExists(iIsRoot, "after getInstance");
        // Create test files.
        String f = getTestFilename("f.txt");
        String f2 = getTestFilename("f2.txt");
        String f3 = getTestFilename("f3.txt");
        String f4 = getTestFilename("f4.txt");
        assertTrue("is.create failed", is.create(f));
        assertTrue("f creation failed", create(f));
        assertTrue("is.copy failed", is.copy(f, f2));
        assertTrue("is.copy2 failed", is.copy(f, f2));
        assertTrue("is.move failed", is.move(f2, f3));
        assertTrue("is.copy3 failed", is.copy(f3, f4));
        assertTrue("is.delete failed", is.delete(f4));

        // Take a copy of iTestRoot.
        String isTmpRoot = FileUtils.getInstallerRoot() +
                           FileUtils.pathSeparator() + "isTmp";
        // FileOperations.copy() cannot copy file if the file
        // has been opened by another FileUtility instance, so
        // let's do copying using streams.
        //assertTrue("Copying iTestRoot failed",
        //           FileOps.copyAll(iTestRoot, isTmpRoot));
        assertTrue("Copying iTestRoot failed",
                   streamCopyAll(new File(iTestRoot),
                                 new File(isTmpRoot)));

        // Finalize IntegrityService session.
        assertExists(iIsRoot, "before rollback");
        assertTrue("is.rollback failed", is.rollback());
        assertNotExists(iIsRoot, "after rollback");
        assertNotExists(f, "after rollback");
        assertNotExists(f2, "after rollback");
        assertNotExists(f3, "after rollback");
        assertNotExists(f4, "after rollback");

        // Put back the copy of iTestRoot.
        listFiles(isTmpRoot);
        listFiles(iTestRoot);
        assertTrue("Moving iTestRoot failed",
                   FileOps.move(isTmpRoot, iTestRoot));
        //FileOps.copyAll(isTmpRoot, iTestRoot);

        // Do static rollback.
        assertExists(iIsRoot, "before static rollback");
        assertExists(f, "before static rollback");
        assertNotExists(f2, "before static rollback");
        assertExists(f3, "before static rollback");
        assertNotExists(f4, "before static rollback");
        assertTrue("static rollback failed", IntegrityService.rollback(iIsRoot));
        assertNotExists(iIsRoot, "after static rollback");
        assertNotExists(f, "after static rollback");
        assertNotExists(f2, "after static rollback");
        assertNotExists(f3, "after static rollback");
        assertNotExists(f4, "after static rollback");
    }

    public void testStaticRollbackNop()
    {
        // Do static rollback when there is no journal file.
        assertNotExists(iIsRoot, "before rollback");
        assertTrue(IntegrityService.rollback(iIsRoot));
        assertNotExists(iIsRoot, "after rollback");
    }

    public void testNopCommit()
    {
        IntegrityService is = IntegrityService.getInstance(iIsRoot);
        assertExists(iIsRoot, "after getInstance");
        assertTrue("is.commit failed", is.commit());
        assertNotExists(iIsRoot, "after commit");
    }

    public void testNopRollback()
    {
        IntegrityService is = IntegrityService.getInstance(iIsRoot);
        assertExists(iIsRoot, "after getInstance");
        assertTrue("is.rollback failed", is.rollback());
        assertNotExists(iIsRoot, "after rollback");
    }

    public void testGetInstanceTwiceInRow()
    {
        IntegrityService is = IntegrityService.getInstance(iIsRoot);
        assertTrue("is.commit failed", is.commit());
        assertNotExists(iIsRoot, "after commit");
        is = IntegrityService.getInstance(iIsRoot);
        assertTrue("2nd is.commit failed", is.commit());
        assertNotExists(iIsRoot, "after 2nd commit");
    }

    public void testGetInstanceTwice()
    {
        IntegrityService is = IntegrityService.getInstance(iIsRoot);
        assertExists(iIsRoot, "after getInstance");
        IntegrityService is2 = IntegrityService.getInstance(iIsRoot);
        assertExists(iIsRoot, "after 2nd getInstance");
        assertTrue("is != is2", is == is2);
        assertTrue("is.commit failed", is.commit());
        assertNotExists(iIsRoot, "after commit");
        assertFalse("2nd is.commit succeeded", is.commit());
        assertNotExists(iIsRoot, "after 2nd commit");
    }

    public void testGetTwoInstances()
    {
        IntegrityService is = IntegrityService.getInstance(iIsRoot);
        assertExists(iIsRoot, "after getInstance");
        IntegrityService is2 = IntegrityService.getInstance(iIsRoot2);
        assertExists(iIsRoot2, "after getInstance2");
        assertTrue("is == is2", is != is2);
        assertTrue("is2 == null", is2 != null);
        assertTrue("is.commit failed", is.commit());
        assertNotExists(iIsRoot, "after commit");
        assertTrue("is2.commit failed", is2.commit());
        assertNotExists(iIsRoot2, "after commit2");
    }

    public void testCommitTwice()
    {
        IntegrityService is = IntegrityService.getInstance(iIsRoot);
        assertTrue("is.commit failed", is.commit());
        assertNotExists(iIsRoot, "after commit");
        assertFalse("2nd is.commit succeeded", is.commit());
        assertNotExists(iIsRoot, "after 2nd commit");
    }

    public void testRollbackTwice()
    {
        IntegrityService is = IntegrityService.getInstance(iIsRoot);
        assertExists(iIsRoot, "after getInstance");
        assertTrue("is.rollback failed", is.rollback());
        assertNotExists(iIsRoot, "after rollback");
        assertFalse("2nd is.rollback succeeded", is.rollback());
        assertNotExists(iIsRoot, "after 2nd rollback");
    }

    public void testCreateCommit()
    {
        IntegrityService is = IntegrityService.getInstance(iIsRoot);
        assertExists(iIsRoot, "after getInstance");
        String tmpFile = getTestFilename("a.txt");
        assertTrue("is.create failed", is.create(tmpFile));
        assertNotExists(tmpFile, "after is.create");
        assertTrue("file creation failed", create(tmpFile));
        assertExists(tmpFile, "after creation");
        assertTrue("is.commit failed", is.commit());
        assertExists(tmpFile, "after commit");
        assertTrue("file deletion failed", delete(tmpFile));
        assertNotExists(tmpFile, "after deletion");
        assertNotExists(iIsRoot, "in the end of test case");
    }

    public void testCreateRollback()
    {
        IntegrityService is = IntegrityService.getInstance(iIsRoot);
        assertExists(iIsRoot, "after getInstance");
        String tmpFile = getTestFilename("longlonglonglonglonglonglonglonglonglongfilename.txt");
        assertTrue("is.create failed", is.create(tmpFile));
        assertNotExists(tmpFile, "after is.create");
        assertTrue("file creation failed", create(tmpFile));
        assertExists(tmpFile, "after creation");
        assertTrue("is.rollback failed", is.rollback());
        assertNotExists(tmpFile, "after rollback");
        assertNotExists(iIsRoot, "in the end of test case");
    }

    public void testCreateDirCommit()
    {
        IntegrityService is = IntegrityService.getInstance(iIsRoot);
        assertExists(iIsRoot, "after getInstance");
        String tmpFile = getTestFilename("abc");
        assertTrue("is.createDir failed", is.createDir(tmpFile));
        assertExists(tmpFile, "after is.createDir");
        assertTrue("is.commit failed", is.commit());
        assertExists(tmpFile, "after commit");
        assertTrue("file deletion failed", delete(tmpFile));
        assertNotExists(tmpFile, "after deletion");
        assertNotExists(iIsRoot, "in the end of test case");
    }

    public void testCreateDirRollback()
    {
        IntegrityService is = IntegrityService.getInstance(iIsRoot);
        assertExists(iIsRoot, "after getInstance");
        String tmpFile = getTestFilename("abc");
        assertTrue("is.createDir failed", is.createDir(tmpFile));
        assertExists(tmpFile, "after is.createDir");
        assertTrue("is.rollback", is.rollback());
        assertNotExists(tmpFile, "after rollback");
        assertNotExists(iIsRoot, "in the end of test case");
    }

    public void testCreateTempFileCommit()
    {
        IntegrityService is = IntegrityService.getInstance(iIsRoot);
        assertExists(iIsRoot, "after getInstance");
        String tmpFile = is.createTempFile(-1);
        assertNotExists(tmpFile, "after createTempFile");
        assertTrue("tmpFile creation failed", create(tmpFile));
        assertExists(tmpFile, "after creation");
        assertTrue("is commit failed", is.commit());
        assertNotExists(tmpFile, "after commit");
        assertTrue("tmpFile deletion failed", delete(tmpFile));
        assertNotExists(tmpFile, "after deletion");
        assertNotExists(iIsRoot, "in the end of test case");
    }

    public void testCreateTempFileRollback()
    {
        IntegrityService is = IntegrityService.getInstance(iIsRoot);
        assertExists(iIsRoot, "after getInstance");
        String tmpFile = is.createTempFile(-1);
        assertNotExists(tmpFile, "after createTempFile");
        assertTrue("tmpFile creation failed", create(tmpFile));
        assertExists(tmpFile, "after creation");
        assertTrue("is.rollback failed", is.rollback());
        assertNotExists(tmpFile, "after rollback");
        assertNotExists(iIsRoot, "in the end of test case");
    }

    public void testDeleteCommit()
    {
        IntegrityService is = IntegrityService.getInstance(iIsRoot);
        assertExists(iIsRoot, "after getInstance");
        String tmpFile = getTestFilename("a.txt");
        assertTrue("file creation failed", create(tmpFile));
        assertExists(tmpFile, "after creation");
        assertTrue("is.delete failed", is.delete(tmpFile));
        assertNotExists(tmpFile, "after is.delete");
        assertTrue("is.commit failed", is.commit());
        assertNotExists(tmpFile, "after is.commit");
        assertNotExists(iIsRoot, "in the end of test case");
    }

    public void testDeleteRollback()
    {
        IntegrityService is = IntegrityService.getInstance(iIsRoot);
        assertExists(iIsRoot, "after getInstance");
        String tmpFile = getTestFilename("a.txt");
        assertTrue("file creation failed", create(tmpFile));
        assertExists(tmpFile, "after creation");
        assertTrue("is.delete failed", is.delete(tmpFile));
        assertNotExists(tmpFile, "after is.delete");
        assertTrue("is.rollback failed", is.rollback());
        assertExists(tmpFile, "after is.rollback");
        assertTrue("file deletion", delete(tmpFile));
        assertNotExists(tmpFile, "after deletion");
        assertNotExists(iIsRoot, "in the end of test case");
    }

    public void testCopyCommit()
    {
        IntegrityService is = IntegrityService.getInstance(iIsRoot);
        assertExists(iIsRoot, "after getInstance");
        String tmpFile = getTestFilename("a.txt");
        String tmpFile2 = getTestFilename("b.txt");
        assertTrue("file creation failed", create(tmpFile));
        assertExists(tmpFile, "after creation");
        assertNotExists(tmpFile2, "before is.copy");
        assertTrue("is.copy failed", is.copy(tmpFile, tmpFile2));
        assertExists(tmpFile, "after is.copy");
        assertExists(tmpFile2, "after is.copy");
        assertTrue("is.commit failed", is.commit());
        assertExists(tmpFile, "after commit");
        assertExists(tmpFile2, "after commit");
        assertNotExists(iIsRoot, "in the end of test case");
    }

    public void testCopyRollback()
    {
        IntegrityService is = IntegrityService.getInstance(iIsRoot);
        assertExists(iIsRoot, "after getInstance");
        String tmpFile = getTestFilename("a.txt");
        String tmpFile2 = getTestFilename("b.txt");
        assertTrue("file creation failed", create(tmpFile));
        assertExists(tmpFile, "after creation");
        assertNotExists(tmpFile2, "before is.copy");
        assertTrue("is.copy failed", is.copy(tmpFile, tmpFile2));
        assertExists(tmpFile, "after is.copy");
        assertExists(tmpFile2, "after is.copy");
        assertTrue("is.rollback failed", is.rollback());
        assertExists(tmpFile, "after rollback");
        assertNotExists(tmpFile2, "after rollback");
        assertNotExists(iIsRoot, "in the end of test case");
    }

    public void testMoveCommit()
    {
        IntegrityService is = IntegrityService.getInstance(iIsRoot);
        assertExists(iIsRoot, "after getInstance");
        String tmpFile = getTestFilename("a.txt");
        String tmpFile2 = getTestFilename("b.txt");
        assertTrue("file creation failed", create(tmpFile));
        assertExists(tmpFile, "after creation");
        assertNotExists(tmpFile2, "before is.move");
        assertTrue("is.move failed", is.move(tmpFile, tmpFile2));
        assertNotExists(tmpFile, "after is.move");
        assertExists(tmpFile2, "after is.move");
        assertTrue("is.commit failed", is.commit());
        assertNotExists(tmpFile, "after commit");
        assertExists(tmpFile2, "after commit");
        assertNotExists(iIsRoot, "in the end of test case");
    }

    public void testMoveRollback()
    {
        IntegrityService is = IntegrityService.getInstance(iIsRoot);
        assertExists(iIsRoot, "after getInstance");
        String tmpFile = getTestFilename("a.txt");
        String tmpFile2 = getTestFilename("b.txt");
        assertTrue("file creation failed", create(tmpFile));
        assertExists(tmpFile, "after creation");
        assertNotExists(tmpFile2, "before is.move");
        assertTrue("is.move failed", is.move(tmpFile, tmpFile2));
        assertNotExists(tmpFile, "after is.move");
        assertExists(tmpFile2, "after is.move");
        assertTrue("is.rollback failed", is.rollback());
        assertExists(tmpFile, "after rollback");
        assertNotExists(tmpFile2, "after rollback");
        assertNotExists(iIsRoot, "in the end of test case");
    }

    public void testUpdateCommit()
    {
        IntegrityService is = IntegrityService.getInstance(iIsRoot);
        assertExists(iIsRoot, "after getInstance");
        String tmpFile = getTestFilename("a.txt");
        assertTrue("file creation failed", create(tmpFile));
        assertExists(tmpFile, "after creation");
        assertTrue("is.update failed", is.update(tmpFile));
        assertExists(tmpFile, "after is.update");
        assertTrue("is.commit failed", is.commit());
        assertExists(tmpFile, "after commit");
        assertNotExists(iIsRoot, "in the end of test case");
    }

    public void testUpdateRollback()
    {
        IntegrityService is = IntegrityService.getInstance(iIsRoot);
        assertExists(iIsRoot, "after getInstance");
        String tmpFile = getTestFilename("a.txt");
        assertTrue("file creation failed", create(tmpFile));
        assertExists(tmpFile, "after creation");
        assertTrue("is.update failed", is.update(tmpFile));
        assertExists(tmpFile, "after is.update");
        assertTrue("is.rollback failed", is.rollback());
        assertExists(tmpFile, "after rollback");
        assertNotExists(iIsRoot, "in the end of test case");
    }

    public void testOpSeqCommit()
    {
        IntegrityService is = IntegrityService.getInstance(iIsRoot);
        assertExists(iIsRoot, "after getInstance");

        // Create test file structure
        String f = "f.txt";
        String f2 = "f2.txt";
        String f3 = "f3.txt";
        String f4 = "f4.txt";
        String d = getTestFilename("d");
        String d2 = getTestFilename("d2");
        String d3 = getTestFilename("d3"+FileUtils.pathSeparator()+"sd1"+FileUtils.pathSeparator()+"ssd1");
        String d4 = getTestFilename("d4"+FileUtils.pathSeparator()+"sd2"+FileUtils.pathSeparator()+"ssd1");
        String d5 = getTestFilename("d5");
        assertTrue("dir creation failed", createDir(d));
        assertExists(d, "after creation");
        assertTrue("dir creation failed", createDir(d3));
        assertExists(d3, "after creation");
        assertTrue("dir creation failed", createDir(d4));
        assertExists(d4, "after creation");
        assertTrue("file creation failed", create(d+FileUtils.pathSeparator()+f));
        assertExists(d+FileUtils.pathSeparator()+f, "after creation");
        assertTrue("file creation failed", create(d3+FileUtils.pathSeparator()+f3));
        assertExists(d3+FileUtils.pathSeparator()+f3, "after creation");
        assertTrue("file creation failed", create(d4+FileUtils.pathSeparator()+f4));
        assertExists(d4+FileUtils.pathSeparator()+f4, "after creation");

        // Do some file operations through IntegrityService.
        assertTrue("is.copy failed",
                   is.copy(d+FileUtils.pathSeparator()+f, d3+FileUtils.pathSeparator()+f));
        assertExists(d+FileUtils.pathSeparator()+f, "after copy");
        assertExists(d3+FileUtils.pathSeparator()+f, "after copy");

        assertTrue("is.copy2 failed",
                   is.copy(d+FileUtils.pathSeparator()+f, d+FileUtils.pathSeparator()+f2));
        assertExists(d+FileUtils.pathSeparator()+f, "after copy2");
        assertExists(d+FileUtils.pathSeparator()+f2, "after copy2");

        assertExists(d3, "before copy3");
        listFiles(d3);
        assertExists(d4, "before copy3");
        listFiles(d4);
        assertTrue("is.copy3 failed",is.copy(d3, d4));
        assertExists(d3, "after copy3");
        listFiles(d3);
        assertExists(d4, "after copy3");
        listFiles(d4);
        assertExists(d3+FileUtils.pathSeparator()+f, "after copy3");
        assertNotExists(d3+FileUtils.pathSeparator()+f2, "after copy3");
        assertExists(d3+FileUtils.pathSeparator()+f3, "after copy3");
        assertNotExists(d3+FileUtils.pathSeparator()+f4, "after copy3");
        assertExists(d4+FileUtils.pathSeparator()+f, "after copy3");
        assertNotExists(d4+FileUtils.pathSeparator()+f2, "after copy3");
        assertExists(d4+FileUtils.pathSeparator()+f3, "after copy3");
        assertNotExists(d4+FileUtils.pathSeparator()+f4, "after copy3");

        assertTrue("is.copy4 failed", is.copy(d, d2));
        assertExists(d+FileUtils.pathSeparator()+f, "after copy4");
        assertExists(d+FileUtils.pathSeparator()+f2, "after copy4");
        assertNotExists(d+FileUtils.pathSeparator()+f3, "after copy4");
        assertNotExists(d+FileUtils.pathSeparator()+f4, "after copy4");
        assertExists(d2+FileUtils.pathSeparator()+f, "after copy4");
        assertExists(d2+FileUtils.pathSeparator()+f2, "after copy4");
        assertNotExists(d2+FileUtils.pathSeparator()+f3, "after copy4");
        assertNotExists(d2+FileUtils.pathSeparator()+f4, "after copy4");

        assertTrue("is.move1 failed",
                   is.move(d2+FileUtils.pathSeparator()+f2, d2+FileUtils.pathSeparator()+f3));
        assertExists(d2+FileUtils.pathSeparator()+f, "after move1");
        assertNotExists(d2+FileUtils.pathSeparator()+f2, "after move1");
        assertExists(d2+FileUtils.pathSeparator()+f3, "after move1");
        assertNotExists(d2+FileUtils.pathSeparator()+f4, "after move1");

        assertTrue("is.move2 failed", is.move(d2, d5));
        assertNotExists(d2, "after move2");
        assertExists(d5+FileUtils.pathSeparator()+f, "after move2");
        assertNotExists(d5+FileUtils.pathSeparator()+f2, "after move2");
        assertExists(d5+FileUtils.pathSeparator()+f3, "after move2");
        assertNotExists(d5+FileUtils.pathSeparator()+f4, "after move2");

        assertTrue("is.delete1 failed", is.delete(d5+FileUtils.pathSeparator()+f));
        assertNotExists(d5+FileUtils.pathSeparator()+f, "after delete1");
        assertNotExists(d5+FileUtils.pathSeparator()+f2, "after delete1");
        assertExists(d5+FileUtils.pathSeparator()+f3, "after delete1");
        assertNotExists(d5+FileUtils.pathSeparator()+f4, "after delete1");

        // Commit the operations.
        assertTrue("is.commit failed", is.commit());

        // Check that operations are committed to the disk.
        assertExists(d+FileUtils.pathSeparator()+f, "after commit");
        assertExists(d+FileUtils.pathSeparator()+f2, "after commit");
        assertNotExists(d+FileUtils.pathSeparator()+f3, "after commit");
        assertNotExists(d2, "after commit");
        assertExists(d3+FileUtils.pathSeparator()+f3, "after commit");
        assertNotExists(d3+FileUtils.pathSeparator()+f4, "after commit");
        assertExists(d4+FileUtils.pathSeparator()+f3, "after commit");
        assertNotExists(d4+FileUtils.pathSeparator()+f4, "after commit");
        assertNotExists(d5+FileUtils.pathSeparator()+f, "after commit");
        assertNotExists(d5+FileUtils.pathSeparator()+f2, "after commit");
        assertExists(d5+FileUtils.pathSeparator()+f3, "after commit");
        assertNotExists(d5+FileUtils.pathSeparator()+f4, "after commit");

        assertNotExists(iIsRoot, "in the end of test case");
    }

    public void testOpSeqRollback()
    {
        IntegrityService is = IntegrityService.getInstance(iIsRoot);
        assertExists(iIsRoot, "after getInstance");

        // Create test file structure
        String f = "f.txt";
        String f2 = "f2.txt";
        String f3 = "f3.txt";
        String f4 = "f4.txt";
        String d = getTestFilename("d");
        String d2 = getTestFilename("d2");
        String d3 = getTestFilename("d3"+FileUtils.pathSeparator()+"sd1"+FileUtils.pathSeparator()+"ssd1");
        String d4 = getTestFilename("d4"+FileUtils.pathSeparator()+"sd2"+FileUtils.pathSeparator()+"ssd1");
        String d5 = getTestFilename("d5");
        assertTrue("dir creation failed", createDir(d));
        assertExists(d, "after creation");
        assertTrue("dir creation failed", createDir(d3));
        assertExists(d3, "after creation");
        assertTrue("dir creation failed", createDir(d4));
        assertExists(d4, "after creation");
        assertTrue("file creation failed", create(d+FileUtils.pathSeparator()+f));
        assertExists(d+FileUtils.pathSeparator()+f, "after creation");
        assertTrue("file creation failed", create(d3+FileUtils.pathSeparator()+f3));
        assertExists(d3+FileUtils.pathSeparator()+f3, "after creation");
        assertTrue("file creation failed", create(d4+FileUtils.pathSeparator()+f4));
        assertExists(d4+FileUtils.pathSeparator()+f4, "after creation");

        // Do some file operations through IntegrityService.
        assertTrue("is.copy failed",
                   is.copy(d+FileUtils.pathSeparator()+f, d3+FileUtils.pathSeparator()+f));
        assertExists(d+FileUtils.pathSeparator()+f, "after copy");
        assertExists(d3+FileUtils.pathSeparator()+f, "after copy");

        assertTrue("is.copy2 failed",
                   is.copy(d+FileUtils.pathSeparator()+f, d+FileUtils.pathSeparator()+f2));
        assertExists(d+FileUtils.pathSeparator()+f, "after copy2");
        assertExists(d+FileUtils.pathSeparator()+f2, "after copy2");

        assertTrue("is.copy3 failed",is.copy(d3, d4));
        assertExists(d3+FileUtils.pathSeparator()+f, "after copy3");
        assertNotExists(d3+FileUtils.pathSeparator()+f2, "after copy3");
        assertExists(d3+FileUtils.pathSeparator()+f3, "after copy3");
        assertNotExists(d3+FileUtils.pathSeparator()+f4, "after copy3");
        assertExists(d4+FileUtils.pathSeparator()+f, "after copy3");
        assertNotExists(d4+FileUtils.pathSeparator()+f2, "after copy3");
        assertExists(d4+FileUtils.pathSeparator()+f3, "after copy3");
        assertNotExists(d4+FileUtils.pathSeparator()+f4, "after copy3");

        assertTrue("is.copy4 failed", is.copy(d, d2));
        assertExists(d+FileUtils.pathSeparator()+f, "after copy4");
        assertExists(d+FileUtils.pathSeparator()+f2, "after copy4");
        assertNotExists(d+FileUtils.pathSeparator()+f3, "after copy4");
        assertNotExists(d+FileUtils.pathSeparator()+f4, "after copy4");
        assertExists(d2+FileUtils.pathSeparator()+f, "after copy4");
        assertExists(d2+FileUtils.pathSeparator()+f2, "after copy4");
        assertNotExists(d2+FileUtils.pathSeparator()+f3, "after copy4");
        assertNotExists(d2+FileUtils.pathSeparator()+f4, "after copy4");

        assertTrue("is.move1 failed",
                   is.move(d2+FileUtils.pathSeparator()+f2, d2+FileUtils.pathSeparator()+f3));
        assertExists(d2+FileUtils.pathSeparator()+f, "after move1");
        assertNotExists(d2+FileUtils.pathSeparator()+f2, "after move1");
        assertExists(d2+FileUtils.pathSeparator()+f3, "after move1");
        assertNotExists(d2+FileUtils.pathSeparator()+f4, "after move1");

        assertTrue("is.move2 failed", is.move(d2, d5));
        assertNotExists(d2, "after move2");
        assertExists(d5+FileUtils.pathSeparator()+f, "after move2");
        assertNotExists(d5+FileUtils.pathSeparator()+f2, "after move2");
        assertExists(d5+FileUtils.pathSeparator()+f3, "after move2");
        assertNotExists(d5+FileUtils.pathSeparator()+f4, "after move2");

        assertTrue("is.delete1 failed", is.delete(d5+FileUtils.pathSeparator()+f));
        assertNotExists(d5+FileUtils.pathSeparator()+f, "after delete1");
        assertNotExists(d5+FileUtils.pathSeparator()+f2, "after delete1");
        assertExists(d5+FileUtils.pathSeparator()+f3, "after delete1");
        assertNotExists(d5+FileUtils.pathSeparator()+f4, "after delete1");

        // Rollback the operations.
        assertTrue("is.rollback failed", is.rollback());

        // Check that operations are rolled back.
        assertExists(d, "after rollback");
        assertNotExists(d2, "after rollback");
        assertExists(d3, "after rollback");
        assertExists(d4, "after rollback");
        assertExists(d+FileUtils.pathSeparator()+f, "after rollback");
        assertExists(d3+FileUtils.pathSeparator()+f3, "after rollback");
        assertExists(d4+FileUtils.pathSeparator()+f4, "after rollback");
        assertNotExists(d5, "after rollback");

        assertNotExists(iIsRoot, "in the end of test case");
    }

    /**
     * Asserts that given file exists.
     */
    private void assertExists(String aFilename, String aMsg)
    {
        if (!exists(aFilename))
        {
            Log.logError("ERR: " + aFilename + " should exist " + aMsg);
        }
        else
        {
            Log.log("OK: " + aFilename + " exists " + aMsg);
        }
        assertTrue(aFilename + " should exist " + aMsg, exists(aFilename));
    }

    /**
     * Asserts that given file does not exist.
     */
    private void assertNotExists(String aFilename, String aMsg)
    {
        if (exists(aFilename))
        {
            Log.logError("ERR: " + aFilename + " should not exist " + aMsg);
        }
        else
        {
            Log.log("OK: " + aFilename + " does not exist " + aMsg);
        }
        assertFalse(aFilename + " should not exist " + aMsg, exists(aFilename));
    }

    /**
     * Prefixes filename with current test root folder.
     */
    private static String getTestFilename(String aFilename)
    {
        return iTestRoot + FileUtils.pathSeparator() + aFilename;
    }

    /**
     * Returns true if file or directory with given filename
     * exists, false otherwise.
     */
    private boolean exists(String aFilename)
    {
        return FileUtils.exists(aFilename);
    }

    /**
     * Creates an empty file. Returns true if operation
     * succeeded, false otherwise.
     */
    private static boolean create(String aFilename)
    {
        boolean result = true;
        try
        {
            result = new FileUtility(aFilename).createNewFile();
        }
        catch (Throwable t)
        {
            System.err.println(t);
        }
        return result;
    }

    /**
     * Creates a new directory. Returns true if operation
     * succeeded, false otherwise.
     */
    private static boolean createDir(String aDirname)
    {
        boolean result = false;
        try
        {
            result = new FileUtility(aDirname).mkdirs();
        }
        catch (Throwable t)
        {
            System.err.println(t);
        }
        return result;
    }

    /**
     * Deletes given file or directory. Returns true if operation
     * succeeded, false otherwise.
     */
    private static boolean delete(String aFilename)
    {
        boolean result = false;
        try
        {
            result = deleteAll(new FileUtility(aFilename));
        }
        catch (Throwable t)
        {
            System.err.println(t);
        }
        return result;
    }

    /**
     * Deletes given file or directory. Returns true if operation
     * succeeded, false otherwise.
     */
    private static boolean deleteAll(FileUtility aFile) throws IOException
    {
        boolean result = true;
        if (aFile == null)
        {
            result = false;
        }
        if (result && aFile.isDirectory())
        {
            FileUtility[] files = aFile.listFiles();
            if (files != null)
            {
                for (int i = 0; i < files.length && result; i++)
                {
                    result = deleteAll(files[i]);
                }
            }
        }
        if (result && aFile.exists())
        {
            result = aFile.delete();
        }
        return result;
    }

    /**
     * Lists files from specifed path to installer log.
     */
    private static void listFiles(String aPath)
    {
        if (aPath == null || aPath.length() == 0)
        {
            return;
        }
        FileUtility f = new FileUtility(aPath);
        if (!f.exists())
        {
            Log.log("does not exist: " + aPath);
        }
        else if (f.isDirectory())
        {
            Log.log("dir exists: " + aPath);
            try
            {
                FileUtility files[] = f.listFiles();
                for (int i = 0; i < files.length; i++)
                {
                    listFiles(files[i].getAbsolutePath());
                }
            }
            catch (IOException ioe)
            {
                Log.logError("Exception while listing files", ioe);
            }
        }
        else
        {
            Log.log("file exists: " + aPath);
        }
    }

    // Begin helper methods which use java.io.File

    /**
     * Copies file or directory using java.io.File.
     */
    private static boolean streamCopyAll(File aFrom, File aTo)
    {
        boolean result = true;
        if (aFrom == null || !aFrom.exists() || aTo == null)
        {
            result = false;
        }
        if (result)
        {
            if (aFrom.isDirectory())
            {
                if (aTo.exists())
                {
                    // Target dir exists, remove it before copying.
                    result = delete(aTo.getAbsolutePath());
                }
                if (result)
                {
                    result = aTo.mkdirs();
                    if (!result)
                    {
                        Log.logWarning
                        ("IntegrityServiceTest.copyAll: mkdirs failed: " +
                         aTo.toString());
                    }
                }
                else
                {
                    Log.logWarning
                    ("IntegrityServiceTest.copyAll: delete target dir failed: " +
                     aTo.toString());
                }
                File[] files = aFrom.listFiles();
                if (files != null)
                {
                    for (int i = 0; i < files.length && result; i++)
                    {
                        String newTo = aTo.getAbsolutePath() +
                                       File.separator + files[i].getName();
                        result = streamCopyAll(files[i], new File(newTo));
                    }
                }
            }
            else
            {
                try
                {
                    streamCopy(aFrom, aTo);
                }
                catch (IOException ioe)
                {
                    result = false;
                    Log.logWarning
                    ("IntegrityServiceTest.copyAll: copy failed: " + ioe);
                }
            }
        }
        if (!result)
        {
            Log.logWarning("IntegrityServiceTest.copyAll failed: [" +
                           aFrom + "], [" + aTo + "]");
        }
        return result;
    }

    /**
     * Copies file using java.io.File and InputStream/OutputStream.
     */
    private static void streamCopy(File aFrom, File aTo) throws IOException
    {
        if (aFrom == null || !aFrom.exists())
        {
            throw new IOException("Cannot copy, file does not exist: " + aFrom);
        }
        File toParent = aTo.getParentFile();
        if (!toParent.exists())
        {
            // Directory for target file does not exist, let's create it.
            toParent.mkdirs();
        }
        InputStream fis  = FileUtils.getInputStream(aFrom.getCanonicalPath());
        OutputStream fos = FileUtils.getOutputStream(aTo.getCanonicalPath());
        try
        {
            byte[] buf = new byte[16384];
            int i = 0;
            while ((i = fis.read(buf)) != -1)
            {
                fos.write(buf, 0, i);
            }
        }
        catch (IOException ioe)
        {
            throw ioe;
        }
        finally
        {
            if (fis != null)
            {
                fis.close();
            }
            if (fos != null)
            {
                fos.close();
            }
        }
    }

    // End helper methods which use java.io.File
}
