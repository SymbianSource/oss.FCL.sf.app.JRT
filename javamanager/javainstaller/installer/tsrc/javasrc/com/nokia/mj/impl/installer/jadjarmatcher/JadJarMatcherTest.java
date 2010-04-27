/*
* Copyright (c) 2008-2009 Nokia Corporation and/or its subsidiary(-ies).
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


package com.nokia.mj.impl.installer.jadjarmatcher;

import com.nokia.mj.impl.installer.integrityservice.IntegrityService;
import com.nokia.mj.impl.installer.utils.FileUtils;
import com.nokia.mj.impl.installer.utils.InstallerMain;
import com.nokia.mj.impl.installer.utils.Platform;
import com.nokia.mj.impl.utils.Attribute;

import java.util.Hashtable;

import j2meunit.framework.Test;
import j2meunit.framework.TestCase;
import j2meunit.framework.TestMethod;
import j2meunit.framework.TestSuite;

/**
 * JadJarMatcher unit tests.
 */
public class JadJarMatcherTest extends TestCase implements InstallerMain
{
    // Folder where test applications are located.
    private static final String TESTAPPS_ROOT;
    // File names for test applications.
    private static final String TESTAPP1 = "HelloWorld1";
    private static final String TESTAPP2 = "HelloWorld2";
    // File extensions for test applications.
    private static final String JAD = ".jad";
    private static final String JAR = ".jar";
    // Attributes in Jad files.
    private static final String MIDLETNAME_ATTR = "MIDlet-Name";
    private static final String MIDLETVENDOR_ATTR = "MIDlet-Vendor";
    private static final String TESTINFO_ATTR = "Nokia-Test-Info";

    static
    {
        TESTAPPS_ROOT = FileUtils.getInstallerRoot() +
                        "jiutdata" + FileUtils.pathSeparator() +
                        "jadjarmatcher" + FileUtils.pathSeparator() +
                        "testapps" + FileUtils.pathSeparator();
    }

    // Begin j2meunit test framework setup

    public void installerMain(String[] args)
    {
        TestSuite suite = new TestSuite(this.getClass().getName());

        suite.addTest(new JadJarMatcherTest("testFolderFindMidletMessage", new TestMethod()
        {
            public void run(TestCase tc)
            {
                ((JadJarMatcherTest)tc).testFolderFindMidletMessage();
            }
        }));

        suite.addTest(new JadJarMatcherTest("testFolderFindJad1", new TestMethod()
        {
            public void run(TestCase tc)
            {
                ((JadJarMatcherTest)tc).testFolderFindJad1();
            }
        }));

        suite.addTest(new JadJarMatcherTest("testFolderFindJad2", new TestMethod()
        {
            public void run(TestCase tc)
            {
                ((JadJarMatcherTest)tc).testFolderFindJad2();
            }
        }));

        suite.addTest(new JadJarMatcherTest("testFolderFindJar1", new TestMethod()
        {
            public void run(TestCase tc)
            {
                ((JadJarMatcherTest)tc).testFolderFindJar1();
            }
        }));

        suite.addTest(new JadJarMatcherTest("testFolderFindJar2", new TestMethod()
        {
            public void run(TestCase tc)
            {
                ((JadJarMatcherTest)tc).testFolderFindJar2();
            }
        }));

        if (Platform.isS60())
        {
            suite.addTest(new JadJarMatcherTest("testInboxFindMidletMessage", new TestMethod()
            {
                public void run(TestCase tc)
                {
                    ((JadJarMatcherTest)tc).testInboxFindMidletMessage();
                }
            }));

            suite.addTest(new JadJarMatcherTest("testInboxFindJad1", new TestMethod()
            {
                public void run(TestCase tc)
                {
                    ((JadJarMatcherTest)tc).testInboxFindJad1();
                }
            }));

            suite.addTest(new JadJarMatcherTest("testInboxFindJad2", new TestMethod()
            {
                public void run(TestCase tc)
                {
                    ((JadJarMatcherTest)tc).testInboxFindJad2();
                }
            }));

            suite.addTest(new JadJarMatcherTest("testInboxFindJar1", new TestMethod()
            {
                public void run(TestCase tc)
                {
                    ((JadJarMatcherTest)tc).testInboxFindJar1();
                }
            }));

            suite.addTest(new JadJarMatcherTest("testInboxFindJar2", new TestMethod()
            {
                public void run(TestCase tc)
                {
                    ((JadJarMatcherTest)tc).testInboxFindJar2();
                }
            }));
        }

        suite.addTest(new JadJarMatcherTest("testFindJadNoMatch", new TestMethod()
        {
            public void run(TestCase tc)
            {
                ((JadJarMatcherTest)tc).testFindJadNoMatch();
            }
        }));

        suite.addTest(new JadJarMatcherTest("testFindJarNoMatch", new TestMethod()
        {
            public void run(TestCase tc)
            {
                ((JadJarMatcherTest)tc).testFindJarNoMatch();
            }
        }));

        suite.addTest(new JadJarMatcherTest("testFindJadNull", new TestMethod()
        {
            public void run(TestCase tc)
            {
                ((JadJarMatcherTest)tc).testFindJadNull();
            }
        }));

        suite.addTest(new JadJarMatcherTest("testFindJarNull", new TestMethod()
        {
            public void run(TestCase tc)
            {
                ((JadJarMatcherTest)tc).testFindJarNull();
            }
        }));

        com.nokia.mj.impl.utils.OmjTestRunner.run(suite);
    }

    public JadJarMatcherTest()
    {
    }

    public JadJarMatcherTest(String aTestName, TestMethod aTestMethod)
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
    }

    protected void tearDown()
    {
    }

    public void testFolderFindMidletMessage()
    {
        String isRoot = FileUtils.getIntegrityServiceRoot();
        IntegrityService is = IntegrityService.getInstance(isRoot);
        JadJarMatcher.setIntegrityService(is);
        try
        {
            String jarFile = addPath("helloworldmm.jar");
            JadJarFile jadJarFile = JadJarMatcher.findJad(jarFile);
            assertTrue("Jad filename is null", jadJarFile.iJadFilename != null);
            assertTrue("Jar filename is null", jadJarFile.iJarFilename != null);
            assertTrue("Incorrect Jad " + jadJarFile.iJadFilename,
                       jadJarFile.iJadFilename.startsWith(isRoot));
            assertTrue("Incorrect Jar " + jadJarFile.iJarFilename,
                       jadJarFile.iJarFilename.startsWith(isRoot));
            assertTrue("Jar attributes not null",
                       jadJarFile.iJarAttributes == null);
            assertTrue("Jad attributes not null",
                       jadJarFile.iJadAttributes == null);
        }
        catch (Exception ex)
        {
            ex.printStackTrace();
            assertTrue("Unexpected exception: " + ex, false);
        }
    }

    public void testFolderFindJad1()
    {
        testFolderFindJad(TESTAPP1);
    }
    public void testFolderFindJad2()
    {
        testFolderFindJad(TESTAPP2);
    }
    public void testFolderFindJad(String aStartFile)
    {
        try
        {
            String jarFile = addPath(aStartFile+JAR);
            JadJarFile jadJarFile = JadJarMatcher.findJad(jarFile);
            assertTrue("Jad filename is null", jadJarFile.iJadFilename != null);
            assertTrue("Jar filename is null", jadJarFile.iJarFilename != null);
            assertTrue("Incorrect Jad " + jadJarFile.iJadFilename,
                       jadJarFile.iJadFilename.equalsIgnoreCase(addPath(aStartFile+JAD)));
            assertTrue("Incorrect Jar " + jadJarFile.iJarFilename,
                       jadJarFile.iJarFilename.equalsIgnoreCase(jarFile));
            assertJadAttributeValue(jadJarFile, TESTINFO_ATTR, aStartFile);
            assertJadAttributeValue(jadJarFile, MIDLETNAME_ATTR, aStartFile);
            assertJarAttributeValue(jadJarFile, MIDLETNAME_ATTR, aStartFile);
        }
        catch (Exception ex)
        {
            ex.printStackTrace();
            assertTrue("Unexpected exception: " + ex, false);
        }
    }

    public void testFolderFindJar1()
    {
        testFolderFindJar(TESTAPP1);
    }
    public void testFolderFindJar2()
    {
        testFolderFindJar(TESTAPP2);
    }
    public void testFolderFindJar(String aStartFile)
    {
        try
        {
            String jadFile = addPath(aStartFile+JAD);
            JadJarFile jadJarFile = JadJarMatcher.findJar(jadFile, null);
            assertTrue("Jad filename is null", jadJarFile.iJadFilename != null);
            assertTrue("Jar filename is null", jadJarFile.iJarFilename != null);
            assertTrue("Incorrect Jad " + jadJarFile.iJadFilename,
                       jadJarFile.iJadFilename.equalsIgnoreCase(jadFile));
            assertTrue("Incorrect Jar " + jadJarFile.iJarFilename,
                       jadJarFile.iJarFilename.equalsIgnoreCase(addPath(aStartFile+JAR)));
            assertJadAttributeValue(jadJarFile, TESTINFO_ATTR, aStartFile);
            assertJadAttributeValue(jadJarFile, MIDLETNAME_ATTR, aStartFile);
            assertJarAttributeValue(jadJarFile, MIDLETNAME_ATTR, aStartFile);
        }
        catch (Exception ex)
        {
            ex.printStackTrace();
            assertTrue("Unexpected exception: " + ex, false);
        }
    }

    public void testInboxFindMidletMessage()
    {
        String isRoot = FileUtils.getIntegrityServiceRoot();
        IntegrityService is = IntegrityService.getInstance(isRoot);
        JadJarMatcher.setIntegrityService(is);
        try
        {
            JadJarFile jadJarFile = JadJarMatcher.findJad(
                                        "c:\\private\\1000484b\\Mail2\\00001001_S\\1\\00100011_F\\hwmm.jar");
            assertTrue("Jad filename is null", jadJarFile.iJadFilename != null);
            assertTrue("Jar filename is null", jadJarFile.iJarFilename != null);
            assertTrue("Incorrect Jad " + jadJarFile.iJadFilename,
                       jadJarFile.iJadFilename.startsWith(isRoot));
            assertTrue("Incorrect Jar " + jadJarFile.iJarFilename,
                       jadJarFile.iJarFilename.startsWith(isRoot));
            assertTrue("Jad attributes not null",
                       jadJarFile.iJadAttributes == null);
            assertTrue("Jar attributes not null",
                       jadJarFile.iJarAttributes == null);
        }
        catch (Exception ex)
        {
            ex.printStackTrace();
            assertTrue("Unexpected exception: " + ex, false);
        }
        finally
        {
            is.commit();
        }
    }

    public void testInboxFindJad1()
    {
        String startFile = TESTAPP1;
        String startPath = "c:\\private\\1000484b\\Mail2\\00001001_S\\9\\00100009_F\\"+startFile+JAR;
        testInboxFindJad(startFile, startPath);
    }
    public void testInboxFindJad2()
    {
        String startFile = TESTAPP2;
        String startPath = "c:\\private\\1000484b\\Mail2\\00001001_S\\d\\0010000d_F\\"+startFile+JAR;
        testInboxFindJad(startFile, startPath);
    }
    public void testInboxFindJad(String aStartFile, String aStartPath)
    {
        String isRoot = FileUtils.getIntegrityServiceRoot();
        IntegrityService is = IntegrityService.getInstance(isRoot);
        JadJarMatcher.setIntegrityService(is);
        try
        {
            JadJarFile jadJarFile = JadJarMatcher.findJad(aStartPath);
            assertTrue("Jad filename is null", jadJarFile.iJadFilename != null);
            assertTrue("Jar filename is null", jadJarFile.iJarFilename != null);
            assertTrue("Incorrect Jad " + jadJarFile.iJadFilename,
                       jadJarFile.iJadFilename.startsWith(isRoot));
            assertTrue("Incorrect Jar " + jadJarFile.iJarFilename,
                       jadJarFile.iJarFilename.startsWith(isRoot));
            assertJadAttributeValue(jadJarFile, TESTINFO_ATTR, aStartFile+"_Inbox");
            assertJadAttributeValue(jadJarFile, MIDLETNAME_ATTR, aStartFile);
            assertJarAttributeValue(jadJarFile, MIDLETNAME_ATTR, aStartFile);
        }
        catch (Exception ex)
        {
            ex.printStackTrace();
            assertTrue("Unexpected exception: " + ex, false);
        }
        finally
        {
            is.commit();
        }
    }

    public void testInboxFindJar1()
    {
        String startFile = TESTAPP1;
        String startPath = "c:\\private\\1000484b\\Mail2\\00001001_S\\7\\00100007_F\\"+startFile+JAD;
        testInboxFindJar(startFile, startPath);
    }
    public void testInboxFindJar2()
    {
        String startFile = TESTAPP2;
        String startPath = "c:\\private\\1000484b\\Mail2\\00001001_S\\b\\0010000b_F\\"+startFile+JAD;
        testInboxFindJar(startFile, startPath);
    }
    public void testInboxFindJar(String aStartFile, String aStartPath)
    {
        String isRoot = FileUtils.getIntegrityServiceRoot();
        IntegrityService is = IntegrityService.getInstance(isRoot);
        JadJarMatcher.setIntegrityService(is);
        try
        {
            JadJarFile jadJarFile = JadJarMatcher.findJar(aStartPath, null);
            assertTrue("Jad filename is null", jadJarFile.iJadFilename != null);
            assertTrue("Jar filename is null", jadJarFile.iJarFilename != null);
            assertTrue("Incorrect Jad " + jadJarFile.iJadFilename,
                       jadJarFile.iJadFilename.startsWith(isRoot));
            assertTrue("Incorrect Jar " + jadJarFile.iJarFilename,
                       jadJarFile.iJarFilename.startsWith(isRoot));
            assertJadAttributeValue(jadJarFile, TESTINFO_ATTR, aStartFile+"_Inbox");
            assertJadAttributeValue(jadJarFile, MIDLETNAME_ATTR, aStartFile);
            assertJarAttributeValue(jadJarFile, MIDLETNAME_ATTR, aStartFile);
        }
        catch (Exception ex)
        {
            ex.printStackTrace();
            assertTrue("Unexpected exception: " + ex, false);
        }
        finally
        {
            is.rollback();
        }
    }

    public void testFindJadNoMatch()
    {
        try
        {
            String jarFile = addPath(TESTAPP1+"_no_match"+JAR);
            JadJarFile jadJarFile = JadJarMatcher.findJad(jarFile);
            assertTrue("Jad filename is not null (" + jadJarFile.iJadFilename + ")",
                       jadJarFile.iJadFilename == null);
            assertTrue("Jar filename is null", jadJarFile.iJarFilename != null);
            assertTrue("Jad attributes are not null", jadJarFile.iJadAttributes == null);
            assertTrue("Incorrect Jar " + jadJarFile.iJarFilename,
                       jadJarFile.iJarFilename.equalsIgnoreCase(jarFile));
            assertJarAttributeValue(jadJarFile, TESTINFO_ATTR, TESTAPP1+"_no_match");
            assertJarAttributeValue(jadJarFile, MIDLETNAME_ATTR, TESTAPP1);
        }
        catch (Exception ex)
        {
            ex.printStackTrace();
            assertTrue("Unexpected exception: " + ex, false);
        }
    }

    public void testFindJarNoMatch()
    {
        try
        {
            String jadFile = addPath(TESTAPP1+"_no_match"+JAD);
            JadJarFile jadJarFile = JadJarMatcher.findJar(jadFile, null);
            assertTrue("Jad filename is null", jadJarFile.iJadFilename != null);
            assertTrue("Jar filename is not null (" + jadJarFile.iJarFilename + ")",
                       jadJarFile.iJarFilename == null);
            assertTrue("Jar attributes are not null", jadJarFile.iJarAttributes == null);
            assertTrue("Incorrect Jad " + jadJarFile.iJadFilename,
                       jadJarFile.iJadFilename.equalsIgnoreCase(jadFile));
            assertJadAttributeValue(jadJarFile, TESTINFO_ATTR, TESTAPP1+"_no_match");
            assertJadAttributeValue(jadJarFile, MIDLETNAME_ATTR, TESTAPP1);
        }
        catch (Exception ex)
        {
            ex.printStackTrace();
            assertTrue("Unexpected exception: " + ex, false);
        }
    }

    public void testFindJadNull()
    {
        try
        {
            String jarFile = null;
            JadJarFile jadJarFile = JadJarMatcher.findJad(jarFile);
            assertTrue("Returned non-null JadJarFile", jadJarFile == null);
            //assertTrue("Jad filename is not null (" + jadJarFile.iJadFilename + ")",
            //    jadJarFile.iJadFilename == null);
            //assertTrue("Jar filename is not null (" + jadJarFile.iJarFilename + ")",
            //    jadJarFile.iJarFilename == null);
            //assertTrue("Jad attributes are not null", jadJarFile.iJadAttributes == null);
            //assertTrue("Jar attributes are not null", jadJarFile.iJarAttributes == null);
        }
        catch (Exception ex)
        {
            ex.printStackTrace();
            assertTrue("Unexpected exception: " + ex, false);
        }
    }

    public void testFindJarNull()
    {
        try
        {
            String jadFile = null;
            JadJarFile jadJarFile = JadJarMatcher.findJar(jadFile, null);
            assertTrue("Returned non-null JadJarFile", jadJarFile == null);
            //assertTrue("Jad filename is not null (" + jadJarFile.iJadFilename + ")",
            //    jadJarFile.iJadFilename == null);
            //assertTrue("Jar filename is not null (" + jadJarFile.iJarFilename + ")",
            //    jadJarFile.iJarFilename == null);
            //assertTrue("Jad attributes are not null", jadJarFile.iJadAttributes == null);
            //assertTrue("Jar attributes are not null", jadJarFile.iJarAttributes == null);
        }
        catch (Exception ex)
        {
            ex.printStackTrace();
            assertTrue("Unexpected exception: " + ex, false);
        }
    }

    private String addPath(String aFilename)
    {
        return TESTAPPS_ROOT + aFilename;
    }

    private String removePath(String aFullPath)
    {
        assertTrue("Path is null", aFullPath != null);
        assertTrue("Path is too short (" + aFullPath + ")",
                   aFullPath.length() > TESTAPPS_ROOT.length());
        return aFullPath.substring(TESTAPPS_ROOT.length());
    }

    private void assertJadAttributeValue(JadJarFile aJadJarFile, String aName, String aValue)
    {
        String value = getAttributeValue(aJadJarFile.iJadAttributes, aName);
        assertTrue("Incorrect " + aName + " value " + value +
                   " in " + aJadJarFile.iJadFilename +
                   ", expected " + aValue, aValue.equals(value));
    }

    private void assertJarAttributeValue(JadJarFile aJadJarFile, String aName, String aValue)
    {
        String value = getAttributeValue(aJadJarFile.iJarAttributes, aName);
        assertTrue("Incorrect " + aName + " value " + value +
                   " in " + aJadJarFile.iJarFilename +
                   ", expected " + aValue, aValue.equals(value));
    }

    private String getAttributeValue(Hashtable aAttributes, String aName)
    {
        Attribute attr = (Attribute)aAttributes.get(aName);
        if (attr != null)
        {
            return attr.getValue();
        }
        return null;
    }
}
