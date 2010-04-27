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
import java.util.Enumeration;

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
public class TestFileCopyAll extends TestCase implements InstallerMain
{
    String path;

    // Begin j2meunit test framework setup
    public void installerMain(String[] args)
    {
        TestSuite suite = new TestSuite(this.getClass().getName());

//        suite.addTest(new TestFileCopyAll("testFileCopyAll", new TestMethod() {
//            public void run(TestCase tc) {
//                ((TestFileCopyAll) tc).testFileCopyAll();
//            }
//        }));

//        suite.addTest(new TestFileCopyAll("testFileCopyAllFail", new TestMethod() {
//            public void run(TestCase tc) {
//                ((TestFileCopyAll) tc).testFileCopyAllFail();
//            }
//        }));

        suite.addTest(new TestFileCopyAll("testCopyAllDestExistsOverride", new TestMethod()
        {
            public void run(TestCase tc)
            {
                ((TestFileCopyAll) tc).testCopyAllDestExistsOverride();
            }
        }));

        suite.addTest(new TestFileCopyAll("testCopyAllDestExistsOverrideSubDir", new TestMethod()
        {
            public void run(TestCase tc)
            {
                ((TestFileCopyAll) tc).testCopyAllDestExistsOverrideSubDir();
            }
        }));

        suite.addTest(new TestFileCopyAll("testCopyAllDestExistsOverrideOtherContents", new TestMethod()
        {
            public void run(TestCase tc)
            {
                ((TestFileCopyAll) tc).testCopyAllDestExistsOverrideOtherContents();
            }
        }));

        com.nokia.mj.impl.utils.OmjTestRunner.run(suite);
    }

    public TestFileCopyAll()
    {
    }

    public TestFileCopyAll(String aTestName, TestMethod aTestMethod)
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
        path = TestEnvironment.Main_Test_Folder + "testfilecopyall/";
    }

    private void matchEnumWithVector(java.util.Enumeration source,
                                     Vector expectedList)
    {
        while (source.hasMoreElements())
        {
            String file = (String) source.nextElement();
            if (expectedList.contains(file))
            {
                expectedList.remove(file);
            }
            else
            {
                assertTrue("List contents wrong", false);
            }
        }
        assertTrue("All elements not returned", 0 == expectedList.size());
    }

    private boolean copyAll(String aSource, String aDestination, boolean aOverWrite, boolean aTestLinux) throws IOException
    {
        if (!aTestLinux)
        {
            return FileOperations.copyAll(aSource, aDestination, aOverWrite);
        }
        else
        {
            FileUtility source = new FileUtility(aSource);
            FileUtility destination = new FileUtility(aDestination);

            if (!source.isDirectory())
            {
                System.out.println("SOURCE NOT DIRECTOY: "+aSource);
                throw new IOException("SOURCE_NOT_DIRECTORY");
            }

            if (!aOverWrite)
            {
                if (destination.exists())
                {
                    return false;
                }
            }
            return linuxCopyAll(aSource, aDestination, aOverWrite);
        }
    }

    private boolean linuxCopyAll(String aSource, String aDestination, boolean aOverWrite) throws IOException
    {
        System.out.println("Linux Copy All: Source: "+aSource);
        System.out.println("Linux Copy All: Destination"+aDestination);

        boolean result = true;
        FileUtility aFrom = new FileUtility(aSource);
        FileUtility aTo = new FileUtility(aDestination);

        if (aFrom == null || !aFrom.exists() || aTo == null)
        {
            result = false;
        }

        if (result)
        {
            if (aFrom.isDirectory())
            {
//                if (aTo.exists())
//                {
//                    // Target dir exists, remove it before copying.
//                    result = linuxDeleteAll(aTo.getAbsolutePath());
//                }

                if (result)
                {
                    result = aTo.mkdirs();
                    if (!result)
                    {
                        System.out.println(
                            "FileOps.copyAll: mkdirs failed: "
                            + aTo.toString());
                    }
                }
                else
                {
                    System.out.println(
                        "FileOps.copyAll: delete target dir failed: "
                        + aTo.toString());
                }
                FileUtility[] files = aFrom.listFiles();

                if (files != null)
                {
                    for (int i = 0; i < files.length && result; i++)
                    {

                        String newTo = aTo.getAbsolutePath()
                                       + files[i].getName();
                        result = linuxCopyAll(files[i].getAbsolutePath(), newTo,
                                              aOverWrite);
                    }
                }
            }
            else
            {
                try
                {
                    linuxCopy(aFrom.getAbsolutePath(), aTo.getAbsolutePath(),
                              aOverWrite);
                }
                catch (IOException ioe)
                {
                    result = false;
                    System.out.println(
                        "FileOps.copyAll: copy failed: " + ioe);
                }
            }
        }
        if (!result)
        {
            System.out.println("FileOps.copyAll failed: [" + aFrom
                               + "], [" + aTo + "]");
        }
        return result;
    }

    public static boolean linuxDeleteAll(String aTargetFile) throws IOException
    {
        boolean result = true;
        FileUtility aFile = new FileUtility(aTargetFile);
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
                    result = linuxDeleteAll(files[i].getAbsolutePath());
                }
            }
        }

        if (result && aFile.exists())
        {
            result = aFile.delete();
        }

        if (!result)
        {
            System.out.println("FileOps.deleteAll failed: [" + aFile + "]");
        }
        return result;
    }

    public static boolean linuxCopy(String aSourceFile, String aDestFile,
                                    boolean aOverwrite) throws IOException
    {
        FileUtility aFrom = new FileUtility(aSourceFile);
        FileUtility aTo = new FileUtility(aDestFile);



        if (!aFrom.exists())
        {
            throw new IOException("Cannot copy, file does not exist: " + aFrom);
        }

        FileUtility toParent = aTo.getParentFile();
        if (!toParent.exists())
        {
            // Directory for target file does not exist, let's create it.
            toParent.mkdirs();
        }

        if (!aTo.exists())
        {
            aTo.createNewFile();
        }
        else if (!aOverwrite)
        {
            throw new IOException("Target already present");
        }

        InputStream fis = aFrom.openInputStream();
        OutputStream fos = aTo.openOutputStream();

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

        return true;
    }


    private void testFileCopyAll()
    {
        try
        {
            String oldFile = path+"source";
            String newFile = path+"destination";

            String[] fList = { "folder1/", "folder2/", "file1.txt",
                               "file2.txt", "file3.txt", "file4.txt"
                             };

            Vector expectedList = new Vector();
            for (int i = 0; i < fList.length; i++)
            {
                expectedList.add(fList[i]);
            }

            FileUtility target = new FileUtility(newFile);

            assertTrue("Copy All Failed",
                       FileOperations.copyAll(oldFile, newFile, false));

            assertTrue("Destination does not exist", target.exists());
            assertTrue("Destinate not shown as dir", target.isDirectory());
            java.util.Enumeration destContents = target.list();
            matchEnumWithVector(destContents, expectedList);
        }
        catch (Exception e)
        {
            assertTrue("Unexpected exception: "+e, false);
        }
    }


    private void testFileCopyAllFail()
    {
        try
        {
            String oldFile = path+"source1/";
            String newFile = "destination1/";

            String[] fList = { "folder1/", "folder2/", "file1.txt",
                               "file2.txt", "file3.txt", "file4.txt"
                             };

            Vector expectedList = new Vector();
            for (int i = 0; i < fList.length; i++)
            {
                expectedList.add(fList[i]);
            }

            FileUtility target = new FileUtility(newFile);

            assertFalse("Copy All Failed",
                        FileOperations.copyAll(oldFile, newFile, false));

            assertFalse("Destination does not exist", target.exists());
            assertFalse("Destinate not shown as dir", target.isDirectory());
            java.util.Enumeration destContents = target.list();
            matchEnumWithVector(destContents, expectedList);
        }
        catch (Exception e)
        {
            assertTrue("Unexpected exception: "+e, false);
        }
    }

    /**
     * Tests to see if copy all works fine in case the destination directory exists.
     * And is not empty.
     *
     */
    public void testCopyAllDestExistsOverrideOtherContents()
    {
        try
        {
            String testPath = "test5/";
            String file1 = "file1";
            String file2 = "file2";
            String file3 = "file3";
            String file4 = "file4";
            String file5 = "file5";

            FileUtility util = new FileUtility(path + testPath);
            if (util.exists())
            {
                util.delete();
            }
            util.mkdir();

            util = new FileUtility(path + testPath + file1);
            util.createNewFile();

            util = new FileUtility(path + testPath + file2);
            util.createNewFile();

            util = new FileUtility(path + testPath + file3);
            util.createNewFile();

            util = new FileUtility(path + testPath + file4);
            util.createNewFile();

            util = new FileUtility(path + testPath + file5);
            util.createNewFile();

            String destDir = path + "test5Dest/";
            util = new FileUtility(destDir);
            util.mkdir();

            util = new FileUtility(destDir+"miscfile");
            util.createNewFile();

            util = new FileUtility(destDir);

            assertTrue("FileOperations Copy All failed", copyAll(path + testPath, destDir, true, true));
            Enumeration en = util.list();

            int count = 0;
            while (en.hasMoreElements())
            {
                count++;
                String name = (String) en.nextElement();
                System.out.println("FileOperations: Copy All: File: "
                                   + util.getAbsolutePath() + name);
            }
            assertTrue("FileOperation Copy ALl failed. All files not present.",
                       (count == 6)); // all contents plus misc file
        }
        catch (Exception e)
        {
            assertTrue("FileOperation Copy All failed. exception: " + e, false);
        }
    }


    /**
     * Tests to see if copy all works fine in case the destination directory exists.
     * And is not empty.
     *
     */
    public void testCopyAllDestExistsOverride()
    {
        try
        {
            String testPath = "test3/";
            String file1 = "file1";
            String file2 = "file2";
            String file3 = "file3";
            String file4 = "file4";
            String file5 = "file5";

            FileUtility util = new FileUtility(path + testPath);
            if (util.exists())
            {
                util.delete();
            }
            util.mkdir();

            util = new FileUtility(path + testPath + file1);
            util.createNewFile();

            util = new FileUtility(path + testPath + file2);
            util.createNewFile();

            util = new FileUtility(path + testPath + file3);
            util.createNewFile();

            util = new FileUtility(path + testPath + file4);
            util.createNewFile();

            util = new FileUtility(path + testPath + file5);
            util.createNewFile();

            String destDir = path + "test3Dest/";
            util = new FileUtility(destDir);
            util.mkdir();

            assertTrue("FileOperations Copy All failed", copyAll(path + testPath, destDir, true, true));
            Enumeration en = util.list();

            int count = 0;
            while (en.hasMoreElements())
            {
                count++;
                String name = (String) en.nextElement();
                System.out.println("FileOperations: Copy All: File: "
                                   + util.getAbsolutePath() + name);
            }
            assertTrue("FileOperation Copy ALl failed. All files not present.",
                       (count == 5));
        }
        catch (Exception e)
        {
            assertTrue("FileOperation Copy All failed. exception: " + e, false);
        }
    }

    /**
     * Tests to see if copy all works fine in case the destination directory exists.
     * And is not empty. Also, contains sub directories. Check if it recurses
     *
     */
    public void testCopyAllDestExistsOverrideSubDir()
    {
        try
        {
            String testPath = "test4/";
            String file1 = "file1";
            String file2 = "file2";
            String file3 = "file3";
            String file4 = "file4";
            String file5 = "file5";
            String dir1 = "dir1/";
            String subDir1 = "subDir1/";

            FileUtility util = new FileUtility(path + testPath);
            if (util.exists())
            {
                util.delete();
            }
            util.mkdir();

            util = new FileUtility(path + testPath + file1);
            util.createNewFile();

            util = new FileUtility(path + testPath + file2);
            util.createNewFile();

            util = new FileUtility(path + testPath + file3);
            util.createNewFile();

            util = new FileUtility(path + testPath + file4);
            util.createNewFile();

            util = new FileUtility(path + testPath + file5);
            util.createNewFile();

            util = new FileUtility(path + testPath + dir1);
            util.mkdir();

            util = new FileUtility(path + testPath + dir1 + subDir1);
            util.mkdir();

            String destDir = path + "test4Dest/";
            util = new FileUtility(destDir);
            util.mkdir();

            assertTrue("FileOperations Copy All failed", copyAll(path + testPath, destDir, true, true));
            Enumeration en = util.list();

            int count = 0;
            while (en.hasMoreElements())
            {
                count++;
                String name = (String) en.nextElement();
                System.out.println("FileOperations: Copy All: File: "
                                   + util.getAbsolutePath() + name);
            }
            assertTrue("FileOperation Copy All failed. All files not present.",
                       (count == 6));
            assertTrue("FileOperation did not copy directory ", (new FileUtility(destDir+dir1)).exists());
            assertTrue("FileOperation did not copy sub directory ", (new FileUtility(destDir+dir1+subDir1)).exists());
        }
        catch (Exception e)
        {
            e.printStackTrace();
            assertTrue("FileOperation Copy All failed. exception: " + e, false);

        }
    }

}
