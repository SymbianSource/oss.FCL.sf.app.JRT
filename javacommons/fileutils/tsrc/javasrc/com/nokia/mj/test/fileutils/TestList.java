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

import j2meunit.framework.Test;
import j2meunit.framework.TestCase;
import j2meunit.framework.TestMethod;
import j2meunit.framework.TestSuite;

import java.util.Vector;
import java.util.Enumeration;

import com.nokia.mj.impl.installer.utils.InstallerMain;
import com.nokia.mj.impl.fileutils.*;
import com.nokia.mj.test.fileutils.TestEnvironment;

/**
 * Checks cases related to isFile()
 */
public class TestList extends TestCase implements InstallerMain
{
    String path;

    // Begin j2meunit test framework setup
    public void installerMain(String[] args)
    {
        TestSuite suite = new TestSuite(this.getClass().getName());
        /*
                suite.addTest(new TestList("testListEnum", new TestMethod() {
                    public void run(TestCase tc) {
                        ((TestList) tc).testListEnum();
                    }
                }));

                suite.addTest(new TestList("testListEnumWithHidden", new TestMethod() {
                    public void run(TestCase tc) {
                        ((TestList) tc).testListEnumWithHidden();
                    }
                }));

                suite.addTest(new TestList("testListEnumWithFilter", new TestMethod() {
                    public void run(TestCase tc) {
                        ((TestList) tc).testListEnumWithFilter();
                    }
                }));

                suite.addTest(new TestList("testListEnumWithFilterInBetween", new TestMethod() {
                    public void run(TestCase tc) {
                        ((TestList) tc).testListEnumWithFilterInBetween();
                    }
                }));

                suite.addTest(new TestList("testListEnumWithFilterAndHidden",
                        new TestMethod() {
                            public void run(TestCase tc) {
                                ((TestList) tc).testListEnumWithFilterAndHidden();
                            }
                        }));

                suite.addTest(new TestList("testListVectorWithFilterAndHidden",
                        new TestMethod() {
                            public void run(TestCase tc) {
                                ((TestList) tc).testListVectorWithFilterAndHidden();
                            }
                        }));

                suite.addTest(new TestList("testFileUtilityList", new TestMethod() {
                    public void run(TestCase tc) {
                        ((TestList) tc).testFileUtilityList();
                    }
                }));

                suite.addTest(new TestList("testInvalidArgumentInList",
                        new TestMethod() {
                            public void run(TestCase tc) {
                                ((TestList) tc).testInvalidArgumentInList();
                            }
                        }));
        */
        suite.addTest(new TestList("temporaryTest",
                                   new TestMethod()
        {
            public void run(TestCase tc)
            {
                ((TestList) tc).temporaryTest();
            }
        }));


        com.nokia.mj.impl.utils.OmjTestRunner.run(suite);
    }

    public TestList()
    {
    }

    public TestList(String aTestName, TestMethod aTestMethod)
    {
        super(aTestName, aTestMethod);
    }

    public void assertFalse(String aMsg, boolean aCondition)
    {
        assertTrue(aMsg, !aCondition);
    }

    protected void setUp()
    {
        path = TestEnvironment.Main_Test_Folder + "testlist/";
    }

    private void matchEnumWithVector(java.util.Enumeration source,
                                     Vector expectedList)
    {
        while (source.hasMoreElements())
        {
            String file = (String) source.nextElement();
            System.out.println("Got: "+file);
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

    private void matchVectorWithVector(Vector source, Vector expectedList)
    {
        assertTrue("Result does not match expected",
                   source.size() == expectedList.size());

        for (int i = 0; i < source.size(); i++)
        {
            String file = (String) source.elementAt(i);
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

    private void matchFileUtilityListWithVector(FileUtility[] source,
            Vector expectedList)
    {
        assertTrue("Result does not match expected",
                   source.length == expectedList.size());

        for (int i = 0; i < source.length; i++)
        {
            String file = source[i].getName();
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

    public void testFileUtilityList()
    {
        try
        {
            FileUtility check = new FileUtility(path);
            FileUtility[] files = check.listFiles();

            String[] List = { "folder1/", "folder2/", "folder3/",
                              "inputFile.txt", "inputFile1.txt", "inputFile2.txt",
                              "inputFile3.txt", "pää.txt"
                            };

            Vector expectedList = new Vector();
            for (int i = 0; i < List.length; i++)
            {
                expectedList.add(List[i]);
            }

            matchFileUtilityListWithVector(files, expectedList);
        }
        catch (Exception e)
        {
            assertTrue(" TestList failed " + e, false);
        }
    }

    public void temporaryTest()
    {
        try
        {
            FileUtility target = new FileUtility("c:/data/videos/tcktemp/filef");
            if (!target.exists())
            {
                target.createNewFile();
            }

            target = new FileUtility("c:/data/videos/tcktemp/filefoo");
            if (!target.exists())
            {
                target.createNewFile();
            }

            target = new FileUtility("c:/data/videos/tcktemp/ff");
            if (!target.exists())
            {
                target.createNewFile();
            }

            target = new FileUtility("c:/data/videos/tcktemp/afilefoo");
            if (!target.exists())
            {
                target.createNewFile();
            }


            target = new FileUtility("c:/data/videos/tcktemp/aflefo");
            if (!target.exists())
            {
                target.mkdir();
            }

            target = new FileUtility("c:/data/videos/tcktemp/fefo");
            if (!target.exists())
            {
                target.mkdir();
            }

            target = new FileUtility("c:/data/videos/tcktemp/");

            Enumeration files = target.list("f*f", false);
            String[] fList = new String[] {"ff", "filef"};
            Vector expectedList = new Vector();
            for (int i = 0; i < fList.length; i++)
            {
                expectedList.add(fList[i]);
            }

            matchEnumWithVector(files, expectedList);
        }
        catch (Exception e)
        {
            assertFalse("testfailed"+e, true);
        }


    }


    public void testListVectorWithFilterAndHidden()
    {
        try
        {
            FileUtility check = new FileUtility(path);
            Vector files = check.listFiles("*ol*", true);

            String[] List;
            if (TestEnvironment.Platform.equals("s60"))
            {
                String[] platformList = { "folder1/", "folder2/", "folder3/",
                                          "folder4/"
                                        };
                List = platformList;
            }
            else
            {
                String[] platformList = { "folder1/", "folder2/", "folder3/",
                                          ".folder4/"
                                        };
                List = platformList;
            }

            Vector expectedList = new Vector();
            for (int i = 0; i < List.length; i++)
            {
                expectedList.add(List[i]);
            }

            matchVectorWithVector(files, expectedList);
        }

        catch (Exception e)
        {
            assertTrue(" TestList failed " + e, false);
        }
    }

    public void testListEnumWithFilterAndHidden()
    {
        try
        {
            FileUtility check = new FileUtility(path);
            java.util.Enumeration files = check.list("*ol*", true);

            String[] List;
            if (TestEnvironment.Platform.equals("s60"))
            {
                String[] platformList = { "folder1/", "folder2/", "folder3/",
                                          "folder4/"
                                        };
                List = platformList;
            }
            else
            {
                String[] platformList = { "folder1/", "folder2/", "folder3/",
                                          ".folder4/"
                                        };
                List = platformList;
            }

            Vector expectedList = new Vector();
            for (int i = 0; i < List.length; i++)
            {
                expectedList.add(List[i]);
            }

            matchEnumWithVector(files, expectedList);
        }

        catch (Exception e)
        {
            assertTrue(" TestList failed " + e, false);
        }
    }

    public void testListEnumWithFilterInBetween()
    {
        try
        {
            FileUtility check = new FileUtility(path);
            java.util.Enumeration files = check.list("f*o*", false);

            String[] List;
            if (TestEnvironment.Platform.equals("s60"))
            {
                String[] platformList = { "folder1/", "folder2/", "folder3/" };
                List = platformList;
            }
            else
            {
                String[] platformList = { "folder1/", "folder2/", "folder3/" };
                List = platformList;
            }

            Vector expectedList = new Vector();
            for (int i = 0; i < List.length; i++)
            {
                expectedList.add(List[i]);
            }

            matchEnumWithVector(files, expectedList);
        }

        catch (Exception e)
        {
            assertTrue(" TestList failed " + e, false);
        }
    }


    public void testListEnumWithFilter()
    {
        try
        {
            FileUtility check = new FileUtility(path);
            java.util.Enumeration files = check.list("*ol*", false);

            String[] List;
            if (TestEnvironment.Platform.equals("s60"))
            {
                String[] platformList = { "folder1/", "folder2/", "folder3/" };
                List = platformList;
            }
            else
            {
                String[] platformList = { "folder1/", "folder2/", "folder3/" };
                List = platformList;
            }

            Vector expectedList = new Vector();
            for (int i = 0; i < List.length; i++)
            {
                expectedList.add(List[i]);
            }

            matchEnumWithVector(files, expectedList);
        }

        catch (Exception e)
        {
            assertTrue(" TestList failed " + e, false);
        }
    }

    public void testListEnumWithHidden()
    {
        try
        {
            FileUtility check = new FileUtility(path);
            java.util.Enumeration files = check.list("", true);

            String[] List;
            if (TestEnvironment.Platform.equals("s60"))
            {
                String[] platformList = { "folder1/", "folder2/", "folder3/",
                                          "inputFile.txt", "inputFile1.txt", "inputFile2.txt",
                                          "inputFile3.txt", "pää.txt", "hidden.txt", "folder4/"
                                        };
                List = platformList;
            }
            else
            {
                String[] platformList = { "folder1/", "folder2/", "folder3/",
                                          "inputFile.txt", "inputFile1.txt", "inputFile2.txt",
                                          "inputFile3.txt", "pää.txt", ".hidden.txt",
                                          ".folder4/"
                                        };
                List = platformList;
            }

            Vector expectedList = new Vector();
            for (int i = 0; i < List.length; i++)
            {
                expectedList.add(List[i]);
            }

            matchEnumWithVector(files, expectedList);
        }
        catch (Exception e)
        {
            assertTrue(" TestList failed " + e, false);
        }
    }

    public void testListEnum()
    {
        try
        {
            FileUtility check = new FileUtility(path);
            java.util.Enumeration files = check.list();

            String[] List = { "folder1/", "folder2/", "folder3/",
                              "inputFile.txt", "inputFile1.txt", "inputFile2.txt",
                              "inputFile3.txt", "pää.txt"
                            };

            Vector expectedList = new Vector();
            for (int i = 0; i < List.length; i++)
            {
                expectedList.add(List[i]);
            }

            matchEnumWithVector(files, expectedList);
        }
        catch (Exception e)
        {
            assertTrue(" TestList failed " + e, false);
        }
    }

    private void checkIllegalArgumentExceptionInList(String dir, String filter,
            boolean hidden)
    {
        try
        {
            FileUtility check = new FileUtility(dir);
            Enumeration files = check.list(filter, hidden);
        }
        catch (IllegalArgumentException ex)
        {
            // Expected
        }
        catch (Exception e)
        {
            assertTrue(" Improper Exception thrown: " + e, false);
        }

    }

    public void testInvalidArgumentInList()
    {
        // '?' not allowed
        checkIllegalArgumentExceptionInList(path, "?a", false);

        // '/' not allowed
        checkIllegalArgumentExceptionInList(path, "a/", false);

        // '\\' not allowed
        checkIllegalArgumentExceptionInList(path, "\\a", false);

        // '|' not allowed
        checkIllegalArgumentExceptionInList(path, "a|", false);

        // '>' not allowed
        checkIllegalArgumentExceptionInList(path, "a>a", false);

        // '<' not allowed
        checkIllegalArgumentExceptionInList(path, "a<a", false);

        // ':' not allowed
        checkIllegalArgumentExceptionInList(path, "a:a", false);
    }

}
