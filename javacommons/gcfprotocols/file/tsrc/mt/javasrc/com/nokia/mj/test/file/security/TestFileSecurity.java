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


package com.nokia.mj.test.file.security;

import java.io.*;
import java.util.*;
import j2meunit.framework.*;
import com.nokia.mj.impl.installer.utils.InstallerMain;

import javax.microedition.io.file.*;
import javax.microedition.io.*;
import com.nokia.mj.test.file.utils.*;

public class TestFileSecurity extends FileTestBase
{
    private String iOtherApplicationId;

    private String iNonUsedMidletPath;

    public static void registerTests(TestSuite aSuite)
    {
        aSuite.addTest(new TestFileSecurity("TestFileSecurity_0070",
                                            new TestMethod()
        {
            public void run(TestCase tc)
            {
                ((TestFileSecurity) tc).run_0070();
            }
        }));

        aSuite.addTest(new TestFileSecurity("TestFileSecurity_0080",
                                            new TestMethod()
        {
            public void run(TestCase tc)
            {
                ((TestFileSecurity) tc).run_0080();
            }
        }));

        aSuite.addTest(new TestFileSecurity("TestFileSecurity_1120",
                                            new TestMethod()
        {
            public void run(TestCase tc)
            {
                ((TestFileSecurity) tc).run_1120();
            }
        }));

        aSuite.addTest(new TestFileSecurity("TestFileSecurity_1650",
                                            new TestMethod()
        {
            public void run(TestCase tc)
            {
                ((TestFileSecurity) tc).run_1650();
            }
        }));

        aSuite.addTest(new TestFileSecurity("TestFileSecurity_1730",
                                            new TestMethod()
        {
            public void run(TestCase tc)
            {
                ((TestFileSecurity) tc).run_1730();
            }
        }));

        aSuite.addTest(new TestFileSecurity("TestFileSecurity_1940",
                                            new TestMethod()
        {
            public void run(TestCase tc)
            {
                ((TestFileSecurity) tc).run_1940();
            }
        }));

        aSuite.addTest(new TestFileSecurity("TestFileSecurity_1950",
                                            new TestMethod()
        {
            public void run(TestCase tc)
            {
                ((TestFileSecurity) tc).run_1950();
            }
        }));

        aSuite.addTest(new TestFileSecurity("TestFileSecurity_2020",
                                            new TestMethod()
        {
            public void run(TestCase tc)
            {
                ((TestFileSecurity) tc).run_2020();
            }
        }));

        aSuite.addTest(new TestFileSecurity("TestFileSecurity_2120",
                                            new TestMethod()
        {
            public void run(TestCase tc)
            {
                ((TestFileSecurity) tc).run_2120();
            }
        }));

    }

    public void installerMain(String[] args)
    {
        TestSuite aSuite = new TestSuite(this.getClass().getName());
        registerTests(aSuite);
        com.nokia.mj.impl.utils.OmjTestRunner.run(aSuite);
    }

    public Test suite()
    {
        TestSuite aSuite = new TestSuite();
        registerTests(aSuite);
        return aSuite;
    }

    public TestFileSecurity()
    {
    }

    public TestFileSecurity(String sTestName, TestMethod rTestMethod)
    {
        super(sTestName, rTestMethod);
    }

    protected void setUp() throws Exception
    {

    }

    private class MyFileSystemListener implements FileSystemListener
    {
        private int iState = -1000;

        // private String iTestCase;

        public MyFileSystemListener()
        {
            // iTestCase = aTestCase;
        }

        public synchronized int getState()
        {
            return iState;
        }

        public synchronized void setState(int aState)
        {
            iState = aState;
        }

        public void rootChanged(int aState, String aRootName)
        {
            try
            {
                if (aState == FileSystemListener.ROOT_ADDED)
                {
                    if (!aRootName.equals("E:/"))
                        throw new TestFailedException("aRootName: " + aRootName);
                    setState(aState);
                    append("added: " + aRootName + "\n");
                }
                if (aState == FileSystemListener.ROOT_REMOVED)
                {
                    if (!aRootName.equals("E:/"))
                        throw new TestFailedException("aRootName: " + aRootName);
                    setState(aState);
                    append("removed: " + aRootName + "\n");
                }
            }
            catch (Exception ex)
            {
                append(ex.toString());
                setState(45);
            }
        }// end rootChanged()
    }// end FileSystemListener

    public void runTests()
    {
        try
        {
            // Update some other application's application id to the
            // 'iOtherApplicationId'
            // datamember. This 'other application' must be locates in the
            // c:/private/ directory.
            iOtherApplicationId = new String("10000882/");
            // run_0070();
            // run_0080();
            // run_0082(); // Invalid. VFS cases
            // run_0083(); // Invalid. VFS cases
            // run_0092(); //DRM
            // run_0093(); //DRM
            // run_0900(); // Invalid. VFS Cases
            // run_1120();
            // run_1650();
            // run_1730();

            // run_1830(); // list giving more issue
            // run_1840(); // list giving more issue
            // run_1841(); // need to change getcategory
            // run_1940();
            // run_1950();
            // run_1960(); // VFS Case
            // run_1961(); // VFS Case
            // run_2020();
            // run_2031(); // VFS case
            // run_2120();
            // run_2121(); // VFS case
            /*
             * run_2320(); run_2450(); run_2550(); run_2571(); run_2572();
             * run_2640(); run_2740(); run_2750(); run_2780(); run_2791();
             */
            // Below are non-automatic security testcases.
            // Testcases run_4000 - run_4021 can be executed together.
            // Install other than this midlet to the e: drive and update path of
            // this other
            // midlet to the 'iNonUsedMidletPath' datamember.
            iNonUsedMidletPath = new String(
                "file:///e:/Private/102033E6/MIDlets/[1012248c]/");
            // Update some other application's application id to the
            // 'iOtherApplicationId'
            // datamember. This 'other application' must be locates in the
            // e:/private/ directory.
            iOtherApplicationId = new String("10203636/");
            /*
             * run_4000(); run_4010(); run_4020(); run_4021();
             */
            // Testcase run_4030 have to be executed separately with run_4000
            // testcase.
            // run_4030();
            // Testcase run_4040 have to be executed alone.
            // run_4040();
            // Testcase run_4050 have to be executed alone.
            // run_4050();
            // Testcase run_4060 have to be executed alone.
            // run_4060();
            // Testcase run_4070 have to be executed alone.
            // run_4070();
            // assertTrue("all executed \n");
            // printResult();
        }
        catch (Exception e)
        {
            e.printStackTrace();
        }
    }// end run()

    //
    //
    //
    //
    private void run_0070()
    {
        FileConnection conn1 = null;
        int counter = 0;
        try
        {
            String url = null;
            // Making a connection to the allowed 'photos' directory.
            url = System.getProperty("fileconn.dir.photos");
            counter = 1;
            conn1 = (FileConnection) Connector.open(url, Connector.READ_WRITE);
            counter = 2;
            if (!conn1.isDirectory())
                throw new TestFailedException();
            conn1.close();
            counter = 3;

            // Making a connection to the allowed 'videos' directory.
            url = System.getProperty("fileconn.dir.videos");
            counter = 4;
            if (url == null)
                throw new TestFailedException("Uri is null");
            conn1 = (FileConnection) Connector.open(url);
            counter = 5;
            if (!conn1.isDirectory())
                throw new TestFailedException();
            conn1.close();
            counter = 6;

            // Making a connection to the allowed 'videos' directory.
            url = System.getProperty("fileconn.dir.graphics");
            counter = 41;
            conn1 = (FileConnection) Connector.open(url);
            counter = 51;
            if (!conn1.isDirectory())
                throw new TestFailedException();
            conn1.close();
            counter = 61;

            // Making a connection to the allowed 'tones' directory.
            url = System.getProperty("fileconn.dir.tones");
            counter = 7;
            conn1 = (FileConnection) Connector.open(url);
            counter = 8;
            if (!conn1.isDirectory())
                throw new TestFailedException();
            conn1.close();
            counter = 9;

            // Making a connection to the allowed 'tones' directory.
            url = System.getProperty("fileconn.dir.music");
            counter = 71;
            conn1 = (FileConnection) Connector.open(url);
            counter = 81;
            if (!conn1.isDirectory())
                throw new TestFailedException();
            conn1.close();
            counter = 91;

            // Making a connection to the allowed 'tones' directory.
            url = System.getProperty("fileconn.dir.recordings");
            counter = 92;
            conn1 = (FileConnection) Connector.open(url);
            counter = 93;
            if (!conn1.isDirectory())
                throw new TestFailedException();
            conn1.close();
            counter = 94;

            // Making a connection to the memory card.
            url = System.getProperty("fileconn.dir.memorycard");
            counter = 10;
            conn1 = (FileConnection) Connector.open(url);
            counter = 11;
            if (!conn1.exists())
                throw new TestFailedException();
            conn1.close();
            counter = 12;

            // Making a connection to the private directory of the midlet.
            url = System.getProperty("fileconn.dir.private");
            counter = 13;
            conn1 = (FileConnection) Connector.open(url);
            counter = 14;
            if (!conn1.isDirectory())
                throw new TestFailedException("url: " + url);
            conn1.close();
            counter = 15;

            // Making a connection to the MMC root.
            conn1 = (FileConnection) Connector.open("file:///e:/");
            counter = 16;
            if (!conn1.isDirectory())
                throw new TestFailedException();
            counter = 17;
            conn1.close();

            String property = System.getProperty("file.separator");
            counter = 18;
            // if(!property.equals("/"))
            // throw new TestFailedException();

            property = System
                       .getProperty("microedition.io.file.FileConnection.version");
            counter = 19;
            if (!property.equals("1.0"))
                throw new TestFailedException();

            counter = 20;
            String name = System.getProperty("fileconn.dir.photos.name");
            if (false == name.equals("Images"))
                throw new TestFailedException("Value: " + name + "\n");
            counter = 21;
            name = System.getProperty("fileconn.dir.videos.name");
            if (false == name.equals("Videos"))
                throw new TestFailedException("name: " + name + "\n");
            counter = 22;
            name = System.getProperty("fileconn.dir.graphics.name");
            if (false == name.equals("Graphics"))
                throw new TestFailedException();
            counter = 23;
            name = System.getProperty("fileconn.dir.tones.name");
            if (false == name.equals("Sounds"))
                throw new TestFailedException("Returned: " + name);
            counter = 24;
            name = System.getProperty("fileconn.dir.music.name");
            if (false == name.equals("Music"))
                throw new TestFailedException();
            counter = 25;
            name = System.getProperty("fileconn.dir.recordings.name");
            if (false == name.equals("Recordings"))
                throw new TestFailedException();
            counter = 26;
            name = System.getProperty("fileconn.dir.memorycard.name");
            if (false == name.equals("Memory card"))
                throw new TestFailedException();
            counter = 27;
            name = System.getProperty("fileconn.dir.private.name");
            if (false == name.equals("Private"))
                throw new TestFailedException();

            url = System.getProperty("fileconn.dir.private") + "myDir/";
            counter = 28;
            conn1 = (FileConnection) Connector.open(url);
            counter = 29;
            String newFile = System.getProperty("fileconn.dir.private")
                             + "myDir/myDir2/file";
            FileConnection conn2 = (FileConnection) Connector.open(newFile);
            counter = 30;
            conn1.mkdir();
            counter = 31;
            conn1.close();
            url = url + "myDir2/";
            conn1 = (FileConnection) Connector.open(url);
            counter = 32;
            conn1.mkdir();
            counter = 33;
            conn1.close();
            conn2.create();
            counter = 34;
            conn2.delete();
            counter = 35;
            conn2.setFileConnection("..");
            counter = 36;
            conn2.delete();
            counter = 37;
            conn2.setFileConnection("..");
            counter = 38;
            conn2.delete();
            conn2.close();

            /**
             * counter = 50; conn1 =(FileConnection)Connector.open
             * ("file:///c:/data/videos/drmFiles/testDir001/drmfile015_001DecPlayCount3.dr",Connector.READ);
             * counter = 51; InputStream inStream = conn1.openInputStream();
             * counter = 52; byte[] byteArray2 = new byte[1000]; int retValue =
             * inStream.read(byteArray2); if(retValue != 805) throw new
             * TestFailedException("retValue: " + retValue); inStream.close();
             * conn1.close(); counter = 53; if (iMultiDriveSupportTests) {
             * //Making a connection to the root of the second MMC card conn1
             * =(FileConnection)Connector.open("file:///g:/"); counter = 54;
             * if(!conn1.isDirectory()) throw new TestFailedException(); counter =
             * 55; conn1.close(); }
             */
            assertTrue(" run_0070() ok ", true);
        }
        catch (Exception ex)
        {
            append(ex.toString());
            append("TEST FAILED!!!: run_0070(): counter: " + counter + "\n");
            assertTrue(" run_0070() FAILED!!! Counter: " + counter, false);
        }
    }// end run_0070()

    //
    //
    //
    //
    private void run_0080()
    {
        FileConnection conn1 = null;
        int counter = 0;
        try
        {
            // Try to make a connection to the z:\ root.
            try
            {
                conn1 = (FileConnection) Connector.open("file:///z:/",
                                                        Connector.READ_WRITE);
                throw new TestFailedException();
            }
            catch (SecurityException ex)
            {
            }
            counter = 1;

            // Try to make a connection to the c:/data/games/ directory.
            // This fails because 'Operator' domain does not access to the
            // 'games/' directory.
            // try{
            // conn1 =(FileConnection)Connector.open
            // ("file:///c:/data/games/",Connector.READ_WRITE);
            // throw new TestFailedException();
            // }
            // catch(SecurityException ex){}
            counter = 2;

            // Try to make a connection to the existing file in the 'nokia'
            // directory.
            try
            {
                conn1 = (FileConnection) Connector.open(
                            "file:///c:/data/emptyFile", Connector.READ_WRITE);
                throw new TestFailedException();
            }
            catch (SecurityException ex)
            {
            }
            counter = 3;

            // Try to create a new file to the 'nokia' directory.
            try
            {
                conn1 = (FileConnection) Connector.open(
                            "file:///c:/data/newFile", Connector.READ_WRITE);
                throw new TestFailedException();
            }
            catch (SecurityException ex)
            {
            }
            counter = 4;
            /*
             * //Try to make a connection to the DRM file. try{ conn1
             * =(FileConnection)Connector.open
             * //("file:///c:/data/images/myDir/DRMFile",Connector.READ_WRITE);
             * ("file:///c:/data/videos/drmFiles/testDir001/drmfile015_001DecPlayCount3.dcf",Connector.READ);
             * throw new TestFailedException(); } catch(SecurityException ex){}
             * counter = 5;
             *
             * //Try to make a connection to the DRM file. try{ conn1
             * =(FileConnection)Connector.open
             * //("file:///c:/data/images/myDir/DRMFile",Connector.READ_WRITE);
             * ("file:///c:/data/videos/drmFiles/testDir001/drmfile015_001DecPlayCount3.dcf?dec=play",
             * Connector.READ); throw new TestFailedException(); }
             * catch(SecurityException ex){} counter = 6;
             *
             * try{ conn1 =(FileConnection)Connector.open
             * //("file:///c:/data/images/myDir/DRMFile",Connector.READ_WRITE);
             * ("file:///c:/data/videos/drmFiles/testDir001/drmfile015_001DecPlayCount3.dcf?dec=play+preview",
             * Connector.READ); throw new TestFailedException(); }
             * catch(SecurityException ex){}
             */
            counter = 7;

            // Try to make a connection to the 'system' directory
            // of the memorycard.
            try
            {
                String url = System.getProperty("fileconn.dir.memorycard")
                             + "system/";
                conn1 = (FileConnection) Connector.open(url);
                throw new TestFailedException();
            }
            catch (SecurityException ex)
            {
            }
            counter = 6;

            // Try to make a connection to the other midlet's private directory.
            try
            {
                conn1 = (FileConnection) Connector
                        .open("file:///c:/system/MIDlets/dgtt/");
                throw new TestFailedException();
            }
            catch (SecurityException ex)
            {
            }
            counter = 7;

            // Try to make a connection to the restricted folder.
            try
            {
                conn1 = (FileConnection) Connector.open("file:///c:/Logs");
                throw new TestFailedException();
            }
            catch (SecurityException ex)
            {
            }
            counter = 8;

            // Try to create a new directory to the 'nokia' directory.
            try
            {
                conn1 = (FileConnection) Connector.open(
                            "file:///c:/data/newDir34/", Connector.READ_WRITE);
                throw new TestFailedException();
            }
            catch (SecurityException ex)
            {
            }
            counter = 9;

            // Try to make a connection to the d:\ root.
            try
            {
                conn1 = (FileConnection) Connector.open("file:///d:/",
                                                        Connector.READ_WRITE);
                throw new TestFailedException();
            }
            catch (SecurityException ex)
            {
            }
            counter = 10;

            if (iMultiDriveSupportTests)
            {
                // Try to make a connection to the 'system' directory
                // of the second memorycard.
                try
                {
                    conn1 = (FileConnection) Connector
                            .open("file:///g:/system/");
                    throw new TestFailedException();
                }
                catch (SecurityException ex)
                {
                }
                counter = 11;
            }

            assertTrue(" run_0080() ok ", true);
        }
        catch (Exception ex)
        {
            append(ex.toString());
            append("TEST FAILED!!!: run_0080(): counter: " + counter + "\n");
            assertTrue(" run_0080() FAILED!!! Counter: " + counter, false);
        }
    }// end run_0080()

    private void validateNames(FileConnection aConn, String aName,
                               String aPath, String aUri) throws IOException
    {
        String name = aConn.getName();
        if (false == name.equals(aName))
            throw new TestFailedException("name: " + name);
        String path = aConn.getPath();
        if (false == path.equals(aPath))
            throw new TestFailedException("path: " + path);
        String uri = aConn.getURL();
        if (false == uri.equals(aUri))
            throw new TestFailedException("uri: " + uri);
    }

    //
    //
    //
    //
    private void testGetConnectionMethods(String aEndpoint, String aName,
                                          String aPath, String aUri) throws IOException
    {
        FileConnection conn1 = null;
        conn1 = (FileConnection) Connector
                .open(aEndpoint, Connector.READ_WRITE);
        validateNames(conn1, aName, aPath, aUri);
        conn1.close();
    }

    //
    //
    //
    //
    private void run_0082()
    {
        FileConnection conn1 = null;
        int counter = 0;
        try
        {
            testGetConnectionMethods("file:///Internal/", "", "/Internal/",
                                     "file:///Internal/");
            counter = 1;
            testGetConnectionMethods("file:///Memory%20caRd/", "",
                                     "/Memory caRd/", "file:///Memory%20caRd/");
            counter = 2;
            testGetConnectionMethods("file:///Memory caRd/  ", "",
                                     "/Memory caRd/", "file:///Memory%20caRd/");
            counter = 3;
            testGetConnectionMethods("file:///Internal/Private/  ", "Private/",
                                     "/Internal/", "file:///Internal/Private/");
            counter = 4;
            testGetConnectionMethods("file:///Internal/Private/newFile  ",
                                     "newFile", "/Internal/Private/",
                                     "file:///Internal/Private/newFile");
            counter = 5;
            conn1 = (FileConnection) Connector
                    .open("file:///Internal/Private/newDir/  ");
            counter = 51;
            conn1.mkdir();
            counter = 52;
            testGetConnectionMethods("file:///Internal/Private/newDir/ ",
                                     "newDir/", "/Internal/Private/",
                                     "file:///Internal/Private/newDir/");
            counter = 53;
            conn1.delete();
            conn1.close();
            counter = 6;
            testGetConnectionMethods("file:///Internal/Images/", "Images/",
                                     "/Internal/", "file:///Internal/Images/");
            counter = 7;
            testGetConnectionMethods("file:///Internal/Images/newFile",
                                     "newFile", "/Internal/Images/",
                                     "file:///Internal/Images/newFile");
            counter = 8;
            testGetConnectionMethods("file:///Internal/Videos/", "Videos/",
                                     "/Internal/", "file:///Internal/Videos/");
            counter = 9;
            testGetConnectionMethods("file:///Internal/Videos/newFile",
                                     "newFile", "/Internal/Videos/",
                                     "file:///Internal/Videos/newFile");
            counter = 10;
            testGetConnectionMethods("file:///Internal/Graphics/", "Graphics/",
                                     "/Internal/", "file:///Internal/Graphics/");
            counter = 11;
            testGetConnectionMethods("file:///Internal/Graphics/newFile",
                                     "newFile", "/Internal/Graphics/",
                                     "file:///Internal/Graphics/newFile");
            counter = 12;
            testGetConnectionMethods("file:///Internal/Sounds/", "Sounds/",
                                     "/Internal/", "file:///Internal/Sounds/");
            counter = 13;
            testGetConnectionMethods("file:///Internal/Sounds/newFile",
                                     "newFile", "/Internal/Sounds/",
                                     "file:///Internal/Sounds/newFile");
            counter = 14;
            testGetConnectionMethods("file:///Internal/Music/", "Music/",
                                     "/Internal/", "file:///Internal/Music/");
            counter = 15;
            testGetConnectionMethods("file:///Internal/Music/newFile",
                                     "newFile", "/Internal/Music/",
                                     "file:///Internal/Music/newFile");
            counter = 16;
            testGetConnectionMethods("file:///Internal/Recordings/",
                                     "Recordings/", "/Internal/", "file:///Internal/Recordings/");
            counter = 17;
            testGetConnectionMethods("file:///Internal/Recordings/newFile",
                                     "newFile", "/Internal/Recordings/",
                                     "file:///Internal/Recordings/newFile");
            counter = 18;
            testGetConnectionMethods("file:///Internal/RecorDings/newFile",
                                     "newFile", "/Internal/RecorDings/",
                                     "file:///Internal/RecorDings/newFile");
            counter = 19;
            testGetConnectionMethods("file:///Memory card/newFile", "newFile",
                                     "/Memory card/", "file:///Memory%20card/newFile");
            counter = 20;
            testGetConnectionMethods("file:///Internal/Music", "Music/",
                                     "/Internal/", "file:///Internal/Music/");
            counter = 21;

            if (iMultiDriveSupportTests)
            {
                testGetConnectionMethods("file:///Internal G/newFile",
                                         "newFile", "/Internal G/",
                                         "file:///Internal%20G/newFile");
                counter = 22;
            }

            assertTrue(" run_0082() ok ", true);
        }
        catch (Exception ex)
        {
            append(ex.toString());
            append("TEST FAILED!!!: run_0082(): counter: " + counter + "\n");
            assertTrue(" run_0082() FAILED!!! ", false);
        }
    }// end run_0082()

    //
    //
    //
    //
    private void run_0083()
    {
        FileConnection conn1 = null;
        int counter = 0;
        try
        {
            try
            {
                conn1 = (FileConnection) Connector.open("file:///Temporary/");
                throw new TestFailedException();
            }
            catch (SecurityException ex)
            {
            }
            counter = 1;

            try
            {
                conn1 = (FileConnection) Connector.open("file:///Rom/");
                throw new TestFailedException();
            }
            catch (SecurityException ex)
            {
            }
            counter = 2;

            try
            {
                conn1 = (FileConnection) Connector
                        .open("file:///Internal/newFile");
                throw new TestFailedException();
            }
            catch (SecurityException ex)
            {
            }
            counter = 3;

            try
            {
                conn1 = (FileConnection) Connector
                        .open("file:///Internal/newDir/");
                throw new TestFailedException();
            }
            catch (SecurityException ex)
            {
            }
            counter = 4;

            try
            {
                conn1 = (FileConnection) Connector
                        .open("file:///Internal/data/");
                throw new TestFailedException();
            }
            catch (SecurityException ex)
            {
            }
            counter = 5;

            try
            {
                conn1 = (FileConnection) Connector
                        .open("file:///Internal/System/Midlets/");
                throw new TestFailedException();
            }
            catch (SecurityException ex)
            {
            }
            counter = 6;

            try
            {
                String url = System.getProperty("fileconn.dir.private");
                // Following characters are skipped away: 'file:///c:/private/'
                String scratchWithoutRoot = url.substring(19);
                conn1 = (FileConnection) Connector.open("file:///Internal/"
                                                        + scratchWithoutRoot);
                throw new TestFailedException();
            }
            catch (SecurityException ex)
            {
            }
            counter = 7;

            /*
             * try{ conn1 = (FileConnection)
             * Connector.open("file:///c:/data/unProtectedFile.txt?drm=dec+play+preview");
             * throw new TestFailedException(); } catch(SecurityException ex){}
             * counter = 8;
             */
            assertTrue(" run_0083() ok ", true);
        }
        catch (Exception ex)
        {
            ex.printStackTrace();
            append(ex.toString());
            append("TEST FAILED!!!: run_0083(): counter: " + counter + "\n");
            assertTrue(" run_0083() FAILED!!! ", false);
        }
    }// end run_0083()

    //
    //
    //
    //
    private void run_0092()
    {
        FileConnection conn1 = null;
        int counter = 0;
        try
        {

            conn1 = (FileConnection) Connector
                    .open(
                        "file:///c:/data/videos/drmFiles/unprotectedFile.txt?drm=preview",
                        Connector.READ);
            counter = 1;
            counter = 2;
            InputStream stream = conn1.openInputStream();
            counter = 3;
            byte[] byteArray1 = new byte[1501];
            int retValue = stream.read(byteArray1);
            counter = 4;
            if (retValue != 1500)
                throw new TestFailedException();
            counter = 5;
            for (int i = 0; i < retValue; ++i)
            {
                if (byteArray1[i] != 75)
                    throw new TestFailedException();
            }
            stream.close();
            conn1.close();
            counter = 6;

            conn1 = (FileConnection) Connector
                    .open(
                        "file:///c:/data/videos/drmFiles/drmfile006_6DecPlayCount1.dcf?drm=enc",
                        Connector.READ);
            counter = 7;
            stream = conn1.openInputStream();
            byteArray1 = new byte[1501];
            counter = 8;
            retValue = stream.read(byteArray1);
            if (retValue != 1330)
                throw new TestFailedException();
            stream.close();
            conn1.close();
            counter = 10;
            conn1 = (FileConnection) Connector
                    .open(
                        "file:///c:/data/videos/drmFiles/drmfile006_6DecPlayCount1.dcf?drm=enc",
                        Connector.READ);
            counter = 11;
            stream = conn1.openInputStream();
            byteArray1 = new byte[1501];
            counter = 12;
            retValue = stream.read(byteArray1);
            if (retValue != 1330)
                throw new TestFailedException();
            stream.close();
            conn1.close();

            counter = 20;
            conn1 = (FileConnection) Connector
                    .open(
                        "file:///Memory card/drmfile006_61DecPlayCount1.dcf?drm=enc",
                        Connector.READ);
            stream = conn1.openInputStream();
            counter = 21;
            byteArray1 = new byte[1501];
            counter = 22;
            retValue = stream.read(byteArray1);
            if (retValue != 1330)
                throw new TestFailedException();
            stream.close();
            conn1.close();

            counter = 40;
            conn1 = (FileConnection) Connector.open(
                        "file:///Memory card/emptyFile.txt?drm=dec+play",
                        Connector.READ);
            conn1.close();
            counter = 41;

            counter = 50;
            conn1 = (FileConnection) Connector.open(
                        "file:///Memory card/emptyFile.txt?dec+play+preview",
                        Connector.READ);
            conn1.close();
            counter = 51;

            conn1 = (FileConnection) Connector
                    .open(
                        "file:///c:/data/videos/drmFiles/unprotectedFile.txt?drm=dec+play",
                        Connector.READ);
            counter = 61;
            counter = 62;
            stream = conn1.openInputStream();
            counter = 63;
            byteArray1 = new byte[1501];
            retValue = stream.read(byteArray1);
            counter = 64;
            if (retValue != 1500)
                throw new TestFailedException();
            counter = 65;
            for (int i = 0; i < retValue; ++i)
            {
                if (byteArray1[i] != 75)
                    throw new TestFailedException();
            }
            stream.close();
            conn1.close();
            counter = 66;

            assertTrue(" run_0092() ok ", true);
        }
        catch (Exception ex)
        {
            append(ex.toString());
            append("TEST FAILED!!!: run_0092(): counter: " + counter + "\n");
            assertTrue(" run_0092() FAILED!!! Counter: " + counter, false);
        }
    }// end run_0092()

    //
    //
    //
    //
    private void run_0093()
    {
        FileConnection conn1 = null;
        int counter = 0;
        try
        {

            try
            {
                conn1 = (FileConnection) Connector.open(
                            "file:///c:/data/unprotectedFile.txt?drm=preview",
                            Connector.READ);
                throw new TestFailedException();
            }
            catch (SecurityException ex)
            {
            }
            counter = 1;

            try
            {
                conn1 = (FileConnection) Connector.open(
                            "file:///Memory card/emptyFile.txt?drm=enc",
                            Connector.READ);
                throw new TestFailedException();
            }
            catch (SecurityException ex)
            {
            }
            counter = 2;

            try
            {
                conn1 = (FileConnection) Connector
                        .open(
                            "file:///Internal/Videos/drmFiles/drmfile006_7DecPlayCount10.dcf?drm=preview",
                            Connector.READ);
                throw new TestFailedException();
            }
            catch (SecurityException ex)
            {
            }
            counter = 5;

            try
            {
                conn1 = (FileConnection) Connector
                        .open(
                            "file:///Internal/Videos/drmFiles/drmfile006_7DecPlayCount10.dcf?drm=dec+play+preview",
                            Connector.READ);
                throw new TestFailedException();
            }
            catch (SecurityException ex)
            {
            }
            counter = 4;

            try
            {
                conn1 = (FileConnection) Connector
                        .open(
                            "file:///Internal/Videos/drmFiles/drmfile006_7DecPlayCount10.dcf?drm=dec+play",
                            Connector.READ);
                throw new TestFailedException();
            }
            catch (SecurityException ex)
            {
            }
            counter = 5;

            try
            {
                conn1 = (FileConnection) Connector
                        .open(
                            "file:///Internal/Videos/drmFiles/drmfile006_7DecPlayCount10.dcf?drm=dec+execute",
                            Connector.READ);
                throw new TestFailedException();
            }
            catch (SecurityException ex)
            {
            }
            counter = 6;

            assertTrue(" run_0093() ok ", true);
        }
        catch (Exception ex)
        {
            append(ex.toString());
            append("TEST FAILED!!!: run_0093(): counter: " + counter + "\n");
            assertTrue(" run_0093() FAILED!!! ", false);
        }
    }// end run_0093()

    //
    //
    //
    //
    private void run_0900()
    {
        // This test verifies that only allowed localized root names are
        // returned in the value of system property fileconn.dir.roots.names and
        // that they are in the same order as the roots returned by listRoots()
        // and the number of the localized root names is the same as the number
        // of enums returned by listRoots().
        //
        // Preconditions for this testcase:
        // 1. Add following line _EPOC_LocDrv_1 E: to the epoc.ini file.
        // epoc.ini file locates on the /epoc32/data/ directory.
        // This demonstrate that memory card is in the emulator.
        // 2. Use Trusted Third Party access domain in this test.
        // 3. Execute test.
        FileConnection conn1 = null;
        int counter = 0;
        try
        {
            Enumeration roots = FileSystemRegistry.listRoots();
            counter = 1;
            if (false == roots.hasMoreElements())
                throw new TestFailedException();
            counter = 2;
            String rootNames = System.getProperty("fileconn.dir.roots.names");
            counter = 3;
            int counterRoot = 0;
            boolean failedFlag = false;
            int indSeparator;
            String name;

            while (roots.hasMoreElements())
            {
                String obj = (String) roots.nextElement();
                if (obj.equals("C:/"))
                {
                    ++counterRoot;
                    if (!rootNames.startsWith("Internal"))
                        throw new TestFailedException(
                            "Wrong value of fileconn.dir.roots.names: "
                            + rootNames);
                    indSeparator = rootNames.indexOf(';');
                    rootNames = rootNames.substring(indSeparator + 1);
                    continue;
                }
                else if (obj.equals("Internal/"))
                {
                    ++counterRoot;
                    if (!rootNames.startsWith("Internal"))
                        throw new TestFailedException(
                            "Wrong value of fileconn.dir.roots.names: "
                            + rootNames);
                    indSeparator = rootNames.indexOf(';');
                    rootNames = rootNames.substring(indSeparator + 1);
                    continue;
                }
                if (obj.equals("E:/"))
                {
                    ++counterRoot;
                    if (!rootNames.startsWith("Memory card"))
                        throw new TestFailedException(
                            "Wrong value of fileconn.dir.roots.names: "
                            + rootNames);
                    indSeparator = rootNames.indexOf(';');
                    rootNames = rootNames.substring(indSeparator + 1);
                    continue;
                }
                else if (obj.equals("Memory card/"))
                {
                    ++counterRoot;
                    if (!rootNames.startsWith("Memory card"))
                        throw new TestFailedException(
                            "Wrong value of fileconn.dir.roots.names: "
                            + rootNames);
                    indSeparator = rootNames.indexOf(';');
                    rootNames = rootNames.substring(indSeparator + 1);
                    continue;
                }
                else
                {
                    if (!iMultiDriveSupportTests)
                        failedFlag = true;
                    else
                    {
                        if (obj.equals("G:/"))
                        {
                            ++counterRoot;
                            if (!rootNames.startsWith("Internal G"))
                                throw new TestFailedException(
                                    "Wrong value of fileconn.dir.roots.names: "
                                    + rootNames);
                            indSeparator = rootNames.indexOf(';');
                            rootNames = rootNames.substring(indSeparator + 1);
                            continue;
                        }
                        else if (obj.equals("Internal G/"))
                        {
                            ++counterRoot;
                            if (!rootNames.startsWith("Internal G"))
                                throw new TestFailedException(
                                    "Wrong value of fileconn.dir.roots.names: "
                                    + rootNames);
                            indSeparator = rootNames.indexOf(';');
                            rootNames = rootNames.substring(indSeparator + 1);
                            continue;
                        }
                    }
                }
            }// end while
            counter = 4;
            if (!iMultiDriveSupportTests)
            {
                if (4 != counterRoot)
                    throw new TestFailedException("counterRoot: " + counterRoot);
            }
            else
            {
                if (6 != counterRoot)
                    throw new TestFailedException("counterRoot: " + counterRoot);
            }

            if (true == failedFlag)
                throw new TestFailedException();
            counter = 5;

            assertTrue(" run_0900() ok ", true);
        }
        catch (Exception ex)
        {
            append(ex.toString());
            append("TEST FAILED!!!: run_0900(): counter: " + counter + "\n");
            assertTrue(" run_0900() FAILED!!! ", false);
        }
    } // end run_0900()

    //
    //
    //
    //
    private void run_1120()
    {
        FileConnection conn1 = null;
        int counter = 0;
        try
        {
            conn1 = (FileConnection) Connector.open("file:///c:/data/");
            counter = 1;
            try
            {
                conn1.fileSize();
                throw new TestFailedException();
            }
            catch (IOException ex)
            {
            }
            assertTrue(" run_1120() ok ", true);
        }
        catch (Exception ex)
        {
            append(ex.toString());
            append("TEST FAILED!!!: run_1120(): counter: " + counter + "\n");
            assertTrue(" run_1120() FAILED!!! ", false);
        }
    }// end run_1120()

    //
    //
    //
    //
    private void run_1650()
    {
        FileConnection conn1 = null;
        int counter = 0;
        try
        {
            conn1 = (FileConnection) Connector
                    .open("file:///c:/data/videos/mydir/file");
            counter = 1;
            if (!conn1.canWrite())
                throw new TestFailedException();
            counter = 2;
            conn1.setWritable(false);
            counter = 3;
            conn1.close();

            conn1 = (FileConnection) Connector
                    .open("file:///c:/data/videos/mydir/file");
            counter = 4;
            if (conn1.canWrite())
                throw new TestFailedException();
            counter = 5;
            conn1.close();

            conn1 = (FileConnection) Connector.open("file:///c:/");
            counter = 6;
            try
            {
                conn1.setWritable(false);
                throw new TestFailedException();
            }
            catch (SecurityException ex)
            {
            }
            conn1.close();

            assertTrue(" run_1650() ok ", true);
        }
        catch (Exception ex)
        {
            append(ex.toString());
            append("TEST FAILED!!!: run_1650(): counter: " + counter + "\n");
            assertTrue(" run_1650() FAILED!!! ", false);
        }
    }// end run_1650()

    //
    //
    //
    //
    private void run_1730()
    {
        FileConnection conn1 = null;
        int counter = 0;
        try
        {
            conn1 = (FileConnection) Connector
                    .open("file:///c:/data/videos/mydir/");
            counter = 1;
            if (conn1.isHidden())
                throw new TestFailedException();
            counter = 2;
            conn1.setHidden(true);
            counter = 3;
            conn1.close();

            conn1 = (FileConnection) Connector
                    .open("file:///c:/data/videos/mydir/");
            counter = 4;
            if (!conn1.isHidden())
                throw new TestFailedException();
            counter = 5;
            conn1.close();

            conn1 = (FileConnection) Connector.open("file:///c:/");
            counter = 6;
            try
            {
                conn1.setHidden(true);
                throw new TestFailedException();
            }
            catch (SecurityException ex)
            {
            }
            conn1.close();

            assertTrue(" run_1730() ok ", true);
        }
        catch (Exception ex)
        {
            append(ex.toString());
            append("TEST FAILED!!!: run_1730(): counter: " + counter + "\n");
            assertTrue(" run_1730() FAILED!!! ", false);
        }
    }// end run_1730()

    //
    //
    //
    //
    private void run_1830()
    {
        FileConnection conn1 = null;
        int counter = 0;
        try
        {
            // Making a new sub-directory under a 'tones' directory.
            String url = System.getProperty("fileconn.dir.tones")
                         + "my sub dir";
            counter = 1;
            conn1 = (FileConnection) Connector.open(url);
            counter = 2;
            conn1.mkdir();
            counter = 3;
            conn1.close();

            // Start to browse from the root to the above created directory.
            conn1 = (FileConnection) Connector.open("file:///c:/");
            counter = 4;
            Enumeration dirList = conn1.list();
            if (false == dirList.hasMoreElements())
                throw new TestFailedException();
            counter = 5;
            int elementCounter = 0;
            boolean failedFlag = false;
            while (dirList.hasMoreElements())
            {
                String obj = (String) dirList.nextElement();
                obj = obj.toLowerCase();
                // append(obj + "\n");
                // This is not returned in the 3.0.
                if (obj.equals("private/"))
                    ++elementCounter;
                else if (obj.equals("data/"))
                    ++elementCounter;
                else
                    failedFlag = true;
            }// end while
            if (elementCounter != 2)
                throw new TestFailedException("elementCounter: "
                                              + elementCounter);
            if (failedFlag == true)
                throw new TestFailedException();
            counter = 6;

            // Setting connection to the c:/data/ directory.
            conn1.setFileConnection("data/");
            counter = 61;
            try
            {
                conn1.delete();
                throw new TestFailedException();
            }
            catch (SecurityException ex)
            {
            }
            counter = 7;
            dirList = conn1.list();
            if (false == dirList.hasMoreElements())
                throw new TestFailedException();
            counter = 8;
            elementCounter = 0;
            failedFlag = false;
            while (dirList.hasMoreElements())
            {
                String obj = (String) dirList.nextElement();
                obj = obj.toLowerCase();
                if (obj.equals("images/"))
                    ++elementCounter;
                else if (obj.equals("sounds/"))
                    ++elementCounter;
                else if (obj.equals("videos/"))
                    ++elementCounter;
                else
                    failedFlag = true;
            }// end while
            if (elementCounter != 3)
                throw new TestFailedException("elementCounter: "
                                              + elementCounter);
            if (failedFlag == true)
                throw new TestFailedException();
            counter = 9;

            // Setting connection to the c:/data/Sounds directory.
            conn1.setFileConnection("Sounds/");
            counter = 10;
            dirList = conn1.list();
            if (false == dirList.hasMoreElements())
                throw new TestFailedException();
            counter = 11;
            elementCounter = 0;
            failedFlag = false;
            while (dirList.hasMoreElements())
            {
                String obj = (String) dirList.nextElement();
                obj = obj.toLowerCase();
                if (obj.equals("digital/"))
                    ++elementCounter;
                else if (obj.equals("simple/"))
                    ++elementCounter;
                else if (obj.equals("my sub dir/"))
                    ++elementCounter;
                else
                    failedFlag = true;
            }// end while
            if (elementCounter != 3)
                throw new TestFailedException("elementCounter: "
                                              + elementCounter);
            if (failedFlag == true)
                throw new TestFailedException();
            counter = 12;
            // Making connection to the c:/data/Sounds/my sub dir directory.
            conn1.setFileConnection("my sub dir/");
            counter = 13;
            dirList = conn1.list();
            counter = 14;
            if (true == dirList.hasMoreElements())
                throw new TestFailedException();
            counter = 15;
            if (false == conn1.isDirectory())
                throw new TestFailedException();
            conn1.close();

            assertTrue(" run_1830() ok ", true);
        }
        catch (Exception ex)
        {
            append(ex.toString());
            append("TEST FAILED!!!: run_1830(): counter: " + counter + "\n");
            assertTrue(" run_1830() FAILED!!! ", false);
        }
    }// end run_1830()

    //
    //
    //
    //
    private void run_1840()
    {
        FileConnection conn1 = null;
        int counter = 0;
        try
        {
            // Making a new sub-directory under a 'scratch' directory.
            String url = System.getProperty("fileconn.dir.private");
            counter = 1;
            conn1 = (FileConnection) Connector.open(url);
            counter = 2;
            if (!conn1.exists())
                throw new TestFailedException();
            counter = 3;
            conn1.close();

            // Creating file under a Midlet's 'scratch' directory.
            conn1 = (FileConnection) Connector.open(url + "new dir");
            counter = 31;
            if (conn1.exists())
                throw new TestFailedException();
            conn1.create();
            counter = 32;
            conn1.delete();
            conn1.close();

            // Creating directory under a Midlet's 'scratch' directory.
            conn1 = (FileConnection) Connector.open(url + "new dir");
            counter = 33;
            if (conn1.exists())
                throw new TestFailedException();
            conn1.mkdir();
            counter = 34;
            // conn1.delete();
            conn1.close();

            // Start to browse from the root to the above created directory.
            conn1 = (FileConnection) Connector.open("file:///c:/");
            counter = 4;
            Enumeration dirList = conn1.list();
            if (false == dirList.hasMoreElements())
                throw new TestFailedException();
            counter = 5;
            int elementCounter = 0;
            boolean failedFlag = false;
            while (dirList.hasMoreElements())
            {
                String obj = (String) dirList.nextElement();
                obj = obj.toLowerCase();
                // append(obj + "\n");
                if (obj.equals("private/"))
                    ++elementCounter;
                else if (obj.equals("data/"))
                    ++elementCounter;
                else
                    failedFlag = true;
            }// end while
            if (elementCounter != 2)
                throw new TestFailedException("elementCounter: "
                                              + elementCounter);
            if (failedFlag == true)
                throw new TestFailedException();
            counter = 6;

            // Setting connection to the c:/private/ directory.
            conn1.setFileConnection("private/");
            counter = 7;
            if (false == conn1.exists())
                throw new TestFailedException();
            counter = 71;
            if (false == conn1.isDirectory())
                throw new TestFailedException();
            counter = 72;
            if (-1 != conn1.directorySize(true))
                throw new TestFailedException("directory size: "
                                              + conn1.directorySize(true) + "\n");
            counter = 73;
            if (0 != conn1.lastModified())
                throw new TestFailedException("lastModified(): "
                                              + conn1.lastModified() + "\n");
            counter = 74;
            dirList = conn1.list();
            if (false == dirList.hasMoreElements())
                throw new TestFailedException();
            counter = 8;
            elementCounter = 0;
            failedFlag = false;
            while (dirList.hasMoreElements())
            {
                String obj = (String) dirList.nextElement();
                obj = obj.toLowerCase();
                // append("obj: " + obj + "\n");
                if (obj.equals("102033e6/"))
                    ++elementCounter;
                else
                    failedFlag = true;
            }// end while
            if (elementCounter != 1)
                throw new TestFailedException("elementCounter: "
                                              + elementCounter);
            if (failedFlag == true)
                throw new TestFailedException();
            counter = 9;

            try
            {
                conn1.setFileConnection(iOtherApplicationId);
                throw new TestFailedException(
                    "Connection to the forbidden directory succeed");
            }
            catch (SecurityException ex)
            {
            }
            counter = 900;

            // Setting connection to the c:/private/102033E6/ directory.
            conn1.setFileConnection("102033E6/");
            counter = 91;
            dirList = conn1.list();
            if (false == dirList.hasMoreElements())
                throw new TestFailedException();
            counter = 92;
            elementCounter = 0;
            failedFlag = false;
            while (dirList.hasMoreElements())
            {
                String obj = (String) dirList.nextElement();
                if (obj.equals("MIDlets/"))
                    ++elementCounter;
                else
                    failedFlag = true;
            }// end while
            if (elementCounter != 1)
                throw new TestFailedException("elementCounter: "
                                              + elementCounter);
            if (failedFlag == true)
                throw new TestFailedException();
            counter = 93;

            // Setting connection to the c:/private/102033E6/MIDlets directory.
            conn1.setFileConnection("MIDlets/");
            counter = 10;
            dirList = conn1.list();
            if (false == dirList.hasMoreElements())
                throw new TestFailedException();
            counter = 11;
            elementCounter = 0;
            failedFlag = false;
            url = System.getProperty("fileconn.dir.private");
            int startIndex = url.indexOf('[');
            int lastIndex = url.lastIndexOf(']');
            String uidDirStr = url.substring(startIndex, lastIndex + 1) + "/";
            // append("uidDirStr: " + uidDirStr);
            while (dirList.hasMoreElements())
            {
                String obj = (String) dirList.nextElement();
                // append("obj: " + obj);
                if (obj.equals(uidDirStr))
                    ++elementCounter;
                else
                    failedFlag = true;
            }// end while
            if (elementCounter != 1)
                throw new TestFailedException("elementCounter: "
                                              + elementCounter);
            if (failedFlag == true)
                throw new TestFailedException();
            counter = 12;

            // Setting connection to the c:/system/MIDlets/[#UID] directory.
            conn1.setFileConnection(uidDirStr);
            counter = 13;
            dirList = conn1.list();
            if (false == dirList.hasMoreElements())
                throw new TestFailedException();
            elementCounter = 0;
            failedFlag = false;
            while (dirList.hasMoreElements())
            {
                String obj = (String) dirList.nextElement();
                if (obj.equals("scratch/"))
                    ++elementCounter;
                else
                    failedFlag = true;
            }// end while
            if (elementCounter != 1)
                throw new TestFailedException("elementCounter: "
                                              + elementCounter);
            if (failedFlag == true)
                throw new TestFailedException();
            counter = 14;

            // Setting connection to the c:/system/MIDlets/[#UID]/scratch
            // directory.
            conn1.setFileConnection("scratch/");
            counter = 15;
            dirList = conn1.list();
            if (false == dirList.hasMoreElements())
                throw new TestFailedException();
            elementCounter = 0;
            failedFlag = false;
            while (dirList.hasMoreElements())
            {
                String obj = (String) dirList.nextElement();
                if (obj.equals("new dir/"))
                    ++elementCounter;
                else
                    failedFlag = true;
            }// end while
            if (elementCounter != 1)
                throw new TestFailedException("elementCounter: "
                                              + elementCounter);
            if (failedFlag == true)
                throw new TestFailedException();
            counter = 16;
            conn1.close();

            counter = 161;
            url = System.getProperty("fileconn.dir.private");
            conn1 = (FileConnection) Connector.open(url + "new dir");
            counter = 162;
            conn1.delete();
            counter = 163;
            conn1.close();

            // This test's part #2.
            // Make a connection to the file where DRM file locates.
            conn1 = (FileConnection) Connector
                    .open("file:///c:/data/images/MYDIR");
            counter = 17;
            dirList = conn1.list();
            counter = 18;
            if (false == dirList.hasMoreElements())
                throw new TestFailedException();
            counter = 19;
            elementCounter = 0;
            failedFlag = false;
            while (dirList.hasMoreElements())
            {
                String obj = (String) dirList.nextElement();
                obj = obj.toLowerCase();
                if (obj.equals("myimages/"))
                    ++elementCounter;
                else if (obj.equals("myimages2/"))
                    ++elementCounter;
                else if (obj.equals("drmfile026decplaycount50.dr"))
                    ++elementCounter;
                else if (obj.equals("drmfile026decplaycount50.dcf"))
                    ++elementCounter;
                else
                    failedFlag = true;
            }// end while
            if (elementCounter != 4)
                throw new TestFailedException("elementCounter: "
                                              + elementCounter);
            if (failedFlag == true)
                throw new TestFailedException();
            counter = 20;
            conn1.close();

            assertTrue(" run_1840() ok ", true);
        }
        catch (Exception ex)
        {
            append(ex.toString());
            append("TEST FAILED!!!: run_1840(): counter: " + counter + "\n");
            assertTrue(" run_1840() FAILED!!! ", false);
        }
    }// end run_1840()

    //
    //
    //
    //
    private void run_1841()
    {
        FileConnection conn1 = null;
        int counter = 0;
        try
        {
            try
            {
                FileConnection tmpConn2 = (FileConnection) Connector
                                          .open("file:///c:/private/" + iOtherApplicationId);
                tmpConn2.close();
                throw new TestFailedException(
                    "Connection to the forbidden directory succeed");
            }
            catch (SecurityException ex)
            {
            }
            counter = 160;

            conn1 = (FileConnection) Connector.open("file:///c:/private/");
            counter = 1;
            if (false == conn1.exists())
                throw new TestFailedException();
            counter = 71;
            if (false == conn1.isDirectory())
                throw new TestFailedException();
            counter = 72;
            if (-1 != conn1.directorySize(true))
                throw new TestFailedException("directory size: "
                                              + conn1.directorySize(true) + "\n");
            counter = 73;
            if (0 != conn1.lastModified())
                throw new TestFailedException("lastModified(): "
                                              + conn1.lastModified() + "\n");
            counter = 74;
            Enumeration dirList = conn1.list();
            if (false == dirList.hasMoreElements())
                throw new TestFailedException();
            counter = 8;
            int elementCounter = 0;
            boolean failedFlag = false;
            while (dirList.hasMoreElements())
            {
                String obj = (String) dirList.nextElement();
                obj = obj.toLowerCase();
                // append("obj: " + obj + "\n");
                if (obj.equals("102033e6/"))
                    ++elementCounter;
                else
                    failedFlag = true;
            }// end while
            if (elementCounter != 1)
                throw new TestFailedException("elementCounter: "
                                              + elementCounter);
            if (failedFlag == true)
                throw new TestFailedException();
            counter = 9;

            conn1.setFileConnection("102033e6/");
            counter = 10;
            conn1.setFileConnection("..");
            counter = 11;
            if (false == conn1.exists())
                throw new TestFailedException();
            counter = 12;
            if (false == conn1.isDirectory())
                throw new TestFailedException();
            counter = 13;
            if (-1 != conn1.directorySize(true))
                throw new TestFailedException("directory size: "
                                              + conn1.directorySize(true) + "\n");
            counter = 14;
            if (0 != conn1.lastModified())
                throw new TestFailedException("lastModified(): "
                                              + conn1.lastModified() + "\n");
            counter = 15;
            conn1.setFileConnection("102033e6/");
            if (-1 == conn1.directorySize(true))
                throw new TestFailedException(
                    "directorySize() operation returned -1");
            conn1.close();

            assertTrue(" run_1841() ok ", true);
        }
        catch (Exception ex)
        {
            append(ex.toString());
            append("TEST FAILED!!!: run_1841(): counter: " + counter + "\n");
            assertTrue(" run_1841() FAILED!!! ", false);
        }
    }// end run_1841()

    //
    //
    //
    //
    private void run_1940()
    {
        FileConnection conn1 = null;
        int counter = 0;
        try
        {
            conn1 = (FileConnection) Connector.open("file:///c:/data/");
            counter = 1;
            Enumeration dirList = conn1.list("video*", true);
            if (false == dirList.hasMoreElements())
                throw new TestFailedException();
            counter = 2;
            int elementCounter = 0;
            boolean failedFlag = false;
            while (dirList.hasMoreElements())
            {
                String obj = (String) dirList.nextElement();
                if (obj.equals("Videos/"))
                    ++elementCounter;
                else
                    failedFlag = true;
            }// end while
            if (elementCounter != 1)
                throw new TestFailedException("elementCounter: "
                                              + elementCounter);
            if (failedFlag == true)
                throw new TestFailedException();
            counter = 3;
            /*
             * //Re-calling list() operation using 'video?' parameter. dirList =
             * conn1.list("video?",true); if(false == dirList.hasMoreElements())
             * throw new TestFailedException(); counter = 4; elementCounter = 0;
             * failedFlag = false; while(dirList.hasMoreElements()){ String obj =
             * (String)dirList.nextElement(); if(obj.equals("Videos/"))
             * ++elementCounter; else failedFlag = true; }//end while
             * if(elementCounter != 1) throw new
             * TestFailedException("elementCounter: " + elementCounter);
             * if(failedFlag == true) throw new TestFailedException();
             */
            counter = 5;

            // Re-calling list() operation using 'videos' parameter.
            dirList = conn1.list("videos", true);
            if (false == dirList.hasMoreElements())
                throw new TestFailedException();
            counter = 6;
            elementCounter = 0;
            failedFlag = false;
            while (dirList.hasMoreElements())
            {
                String obj = (String) dirList.nextElement();
                if (obj.equals("Videos/"))
                    ++elementCounter;
                else
                    failedFlag = true;
            }// end while
            if (elementCounter != 1)
                throw new TestFailedException("elementCounter: "
                                              + elementCounter);
            if (failedFlag == true)
                throw new TestFailedException();
            counter = 7;
            conn1.close();

            assertTrue(" run_1940() ok ", true);
        }
        catch (Exception ex)
        {
            append(ex.toString());
            append("TEST FAILED!!!: run_1940(): counter: " + counter + "\n");
            assertTrue(" run_1940() FAILED!!! Counter: " + counter, false);
        }
    }// end run_1940()

    //
    //
    //
    //
    private void run_1950()
    {
        FileConnection conn1 = null;
        int counter = 0;
        try
        {
            // Making connection to the c:/data/Images/ directory and check
            // that 'Pictures' directory(System Property) is returned
            // by list() operation.
            String url = System.getProperty("fileconn.dir.photos");
            conn1 = (FileConnection) Connector.open(url);
            counter = 1;
            if (!conn1.exists())
                throw new TestFailedException();
            Enumeration dirList = conn1.list("*", true);
            if (false == dirList.hasMoreElements())
                throw new TestFailedException();
            counter = 2;
            int elementCounter = 0;
            boolean failedFlag = false;
            while (dirList.hasMoreElements())
            {
                String obj = (String) dirList.nextElement();
                obj = obj.toLowerCase();
                // if(obj.equals("_palbtn/"))
                // ++elementCounter;
                if (obj.equals("mydir/"))
                    ++elementCounter;
                else if (obj.equals("pictures/"))
                    ++elementCounter;
                // else if(obj.equals("presence/"))
                // ++elementCounter;
                // else
                // failedFlag = true;
            }// end while
            counter = 3;
            // There can be also other directories but it is enough to check
            // that at least these four ones can be found.
            if (elementCounter != 2)
                throw new TestFailedException("elementCounter: "
                                              + elementCounter);
            if (failedFlag == true)
                throw new TestFailedException();
            counter = 4;
            conn1.close();

            assertTrue(" run_1950() ok ", true);
        }
        catch (Exception ex)
        {
            append(ex.toString());
            append("TEST FAILED!!!: run_1950(): counter: " + counter + "\n");
            assertTrue(" run_1950() FAILED!!! ", false);
        }
    }// end run_1950()

    //
    //
    //
    //
    private void run_1960()
    {
        FileConnection conn1 = null;
        int counter = 0;
        try
        {

            conn1 = (FileConnection) Connector.open("file:///Internal/");
            Enumeration dirList = conn1.list("* ", true);
            if (false == dirList.hasMoreElements())
                throw new TestFailedException();
            counter = 2;
            int elementCounter = 0;
            boolean failedFlag = false;
            while (dirList.hasMoreElements())
            {
                String obj = (String) dirList.nextElement();
                obj = obj.toLowerCase();
                // append(obj);
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
                else
                    failedFlag = true;
            }// end while
            counter = 3;
            if (elementCounter != 7)
                throw new TestFailedException("elementCounter: "
                                              + elementCounter);
            if (failedFlag == true)
                throw new TestFailedException();
            counter = 4;

            dirList = conn1.list("*s", true);
            if (false == dirList.hasMoreElements())
                throw new TestFailedException();
            counter = 10;
            elementCounter = 0;
            failedFlag = false;
            while (dirList.hasMoreElements())
            {
                String obj = (String) dirList.nextElement();
                obj = obj.toLowerCase();
                // append(obj);
                if (obj.equals("images/"))
                    ++elementCounter;
                else if (obj.equals("videos/"))
                    ++elementCounter;
                else if (obj.equals("graphics/"))
                    ++elementCounter;
                else if (obj.equals("sounds/"))
                    ++elementCounter;
                else if (obj.equals("recordings/"))
                    ++elementCounter;
                else
                    failedFlag = true;
            }// end while
            counter = 11;
            if (elementCounter != 5)
                throw new TestFailedException("elementCounter: "
                                              + elementCounter);
            if (failedFlag == true)
                throw new TestFailedException();

            dirList = conn1.list("VIDEO?", true);
            if (false == dirList.hasMoreElements())
                throw new TestFailedException();
            counter = 20;
            elementCounter = 0;
            failedFlag = false;
            while (dirList.hasMoreElements())
            {
                String obj = (String) dirList.nextElement();
                obj = obj.toLowerCase();
                // append(obj);
                if (obj.equals("videos/"))
                    ++elementCounter;
                else
                    failedFlag = true;
            }// end while
            counter = 21;
            // There can be also other directories but it is enough to check
            // that at least these four ones can be found.
            if (elementCounter != 1)
                throw new TestFailedException("elementCounter: "
                                              + elementCounter);
            if (failedFlag == true)
                throw new TestFailedException();
            conn1.close();

            conn1 = (FileConnection) Connector.open("file:///Memory CARD/");
            counter = 22;
            dirList = conn1.list("", true);
            if (false == dirList.hasMoreElements())
                throw new TestFailedException();
            counter = 30;
            elementCounter = 0;
            while (dirList.hasMoreElements())
            {
                String obj = (String) dirList.nextElement();
                obj = obj.toLowerCase();
                // append(obj);
                if (obj.equals("emptyfile.txt"))
                    ++elementCounter;
                else if (obj.equals("system/"))
                    throw new TestFailedException(
                        "System dir shouldn't be returned");
            }// end while
            counter = 31;
            if (elementCounter != 1)
                throw new TestFailedException("elementCounter: "
                                              + elementCounter);
            conn1.close();

            if (iMultiDriveSupportTests)
            {
                counter = 32;
                conn1 = (FileConnection) Connector.open("file:///INTERNAL G/");
                counter = 33;
                dirList = conn1.list("", true);
                // secondary memory card should be empty
                if (true == dirList.hasMoreElements())
                    throw new TestFailedException("Internal G was not empty");
            }

            assertTrue(" run_1960() ok ", true);
        }
        catch (Exception ex)
        {
            append(ex.toString());
            append("TEST FAILED!!!: run_1960(): counter: " + counter + "\n");
            assertTrue(" run_1960() FAILED!!! ", false);
        }
    }// end run_1960()

    //
    //
    //
    //
    private void run_1961()
    {
        FileConnection conn1 = null;
        int counter = 0;
        try
        {
            conn1 = (FileConnection) Connector.open("file:///Internal/Videos/");
            Enumeration dirList = conn1.list("*", true);
            if (false == dirList.hasMoreElements())
                throw new TestFailedException();
            counter = 2;
            int elementCounter = 0;
            boolean failedFlag = false;
            while (dirList.hasMoreElements())
            {
                String obj = (String) dirList.nextElement();
                obj = obj.toLowerCase();
                // append(obj);
                if (obj.equals("mydir/"))
                    ++elementCounter;
                else if (obj.equals("drmfiles/"))
                    ++elementCounter;
                else
                    failedFlag = true;
            }// end while
            counter = 3;
            if (elementCounter != 2)
                throw new TestFailedException("elementCounter: "
                                              + elementCounter);
            if (failedFlag == true)
                throw new TestFailedException();
            counter = 4;

            conn1.setFileConnection("mydir/");
            counter = 10;
            dirList = conn1.list("*", true);
            if (false == dirList.hasMoreElements())
                throw new TestFailedException();
            elementCounter = 0;
            failedFlag = false;
            while (dirList.hasMoreElements())
            {
                String obj = (String) dirList.nextElement();
                obj = obj.toLowerCase();
                // append(obj);
                if (obj.equals("file"))
                    ++elementCounter;
                else
                    failedFlag = true;
            }// end while
            counter = 11;
            if (elementCounter != 1)
                throw new TestFailedException("elementCounter: "
                                              + elementCounter);
            if (failedFlag == true)
                throw new TestFailedException();
            counter = 12;
            conn1.close();

            assertTrue(" run_1961() ok ", true);
        }
        catch (Exception ex)
        {
            append(ex.toString());
            append("TEST FAILED!!!: run_1961(): counter: " + counter + "\n");
            assertTrue(" run_1961() FAILED!!! ", false);
        }
    }// end run_1961()

    //
    //
    //
    //
    private void run_2020()
    {
        FileConnection conn1 = null;
        int counter = 0;
        try
        {
            try
            {
                conn1 = (FileConnection) Connector
                        .open("file:///c:/data/newFile67");
                throw new TestFailedException();
            }
            catch (SecurityException ex)
            {
            }
            counter = 1;
            conn1 = (FileConnection) Connector
                    .open("file:///c:/data/sounds/newSound");
            counter = 4;
            if (conn1.exists())
                throw new TestFailedException();
            counter = 5;
            conn1.create();
            counter = 6;
            conn1.close();

            conn1 = (FileConnection) Connector
                    .open("file:///c:/data/sounds/newSound");
            counter = 7;
            if (!conn1.exists())
                throw new TestFailedException();
            conn1.delete();
            conn1.close();

            assertTrue(" run_2020() ok ", true);
        }
        catch (Exception ex)
        {
            append(ex.toString());
            append("TEST FAILED!!!: run_2020(): counter: " + counter + "\n");
            assertTrue(" run_2020() FAILED!!! ", false);
        }
    }// end run_2020()

    private void createFile(String aFileUri) throws IOException
    {
        FileConnection conn1 = (FileConnection) Connector.open(aFileUri);
        conn1.create();
        conn1.delete();
        conn1.close();
    }

    private void createDirAndFile(String aUri) throws IOException
    {
        createFile(aUri + "new test file");
        FileConnection conn1 = (FileConnection) Connector.open(aUri
                               + "new test dir");
        conn1.mkdir();
        createFile(aUri + "new test dir/new test file");
        conn1.delete();
        conn1.close();
    }

    //
    //
    //
    //
    private void run_2031()
    {
        FileConnection conn1 = null;
        int counter = 0;
        try
        {
            createDirAndFile("file:///Internal/images/");
            counter = 1;
            createDirAndFile("file:///Internal/Videos/");
            counter = 2;
            createDirAndFile("file:///Internal/graphics/");
            counter = 3;
            createDirAndFile("file:///Internal/SOUNDS/");
            counter = 4;
            createDirAndFile("file:///Internal/Music/");
            counter = 5;
            createDirAndFile("file:///Internal/Recordings/");
            counter = 6;
            createDirAndFile("file:///Internal/private/");
            counter = 7;

            assertTrue(" run_2031() ok ", true);
        }
        catch (Exception ex)
        {
            append(ex.toString());
            append("TEST FAILED!!!: run_2031(): counter: " + counter + "\n");
            assertTrue(" run_2031() FAILED!!! ", false);
        }
    }// end run_2031()

    //
    //
    //
    //
    private void run_2120()
    {
        FileConnection conn1 = null;
        int counter = 0;
        try
        {
            try
            {
                conn1 = (FileConnection) Connector
                        .open("file:///c:/data/newDir/");
                throw new TestFailedException();
            }
            catch (SecurityException ex)
            {
            }
            counter = 1;

            conn1 = (FileConnection) Connector
                    .open("file:///c:/data/sounds/newSoundDir/");
            counter = 4;
            if (conn1.exists())
                throw new TestFailedException();
            counter = 5;
            conn1.mkdir();
            counter = 6;
            conn1.close();

            conn1 = (FileConnection) Connector
                    .open("file:///c:/data/sounds/newSoundDir/");
            counter = 7;
            if (!conn1.exists())
                throw new TestFailedException();
            conn1.delete();
            conn1.close();

            assertTrue(" run_2120() ok ", true);
        }
        catch (Exception ex)
        {
            append(ex.toString());
            append("TEST FAILED!!!: run_2120(): counter: " + counter + "\n");
            assertTrue(" run_2120() FAILED!!! ", false);
        }
    }// end run_2120()

    private void createTwoSubDir(String aUri) throws IOException
    {
        String dir1 = aUri + "new test dir/";
        FileConnection conn1 = (FileConnection) Connector.open(dir1);
        conn1.mkdir();
        String dir2 = aUri + "new test dir/new dir2/";
        FileConnection conn2 = (FileConnection) Connector.open(dir2);
        conn2.mkdir();
        conn2.delete();
        conn1.delete();
        conn1.close();
        conn2.close();
    }

    //
    //
    //
    //
    private void run_2121()
    {
        FileConnection conn1 = null;
        int counter = 0;
        try
        {
            createTwoSubDir("file:///Internal/Images/");
            counter = 1;
            createTwoSubDir("file:///Internal/videos/");
            counter = 2;
            createTwoSubDir("file:///Internal/graphics/");
            counter = 3;
            createTwoSubDir("file:///Internal/SOUNDS/");
            counter = 4;
            createTwoSubDir("file:///Internal/music/");
            counter = 5;
            createTwoSubDir("file:///Internal/recordings/");
            counter = 6;

            assertTrue(" run_2121() ok ", true);
        }
        catch (Exception ex)
        {
            append(ex.toString());
            append("TEST FAILED!!!: run_2121(): counter: " + counter + "\n");
            assertTrue(" run_2121() FAILED!!! ", false);
        }
    }// end run_2121()

    //
    //
    //
    //
    private void run_2320()
    {
        FileConnection conn1 = null;
        int counter = 0;
        try
        {
            conn1 = (FileConnection) Connector.open("file:///c:/data/");
            counter = 1;
            if (!conn1.isDirectory())
                throw new TestFailedException();
            conn1.close();

            assertTrue(" run_2320() ok ", true);
        }
        catch (Exception ex)
        {
            append(ex.toString());
            append("TEST FAILED!!!: run_2320(): counter: " + counter + "\n");
            assertTrue(" run_2320() FAILED!!! ", false);
        }
    }// end run_2320()

    //
    //
    //
    //
    private void run_2450()
    {
        FileConnection conn1 = null;
        int counter = 0;
        try
        {
            conn1 = (FileConnection) Connector.open("file:///c:/data/images/");
            counter = 1;
            try
            {
                conn1.delete();
                throw new TestFailedException();
            }
            catch (SecurityException ex)
            {
            }
            counter = 2;
            conn1.close();

            conn1 = (FileConnection) Connector
                    .open("file:///c:/data/sounds/newSound23");
            counter = 3;
            conn1.create();
            counter = 4;
            conn1.close();

            conn1 = (FileConnection) Connector
                    .open("file:///c:/data/sounds/newSound23");
            counter = 5;
            conn1.delete();
            counter = 6;
            conn1.close();

            conn1 = (FileConnection) Connector.open("file:///c:/data/");
            counter = 7;
            try
            {
                conn1.delete();
                throw new TestFailedException();
            }
            catch (SecurityException ex)
            {
            }
            counter = 8;
            conn1.close();

            conn1 = (FileConnection) Connector.open("file:///c:/");
            counter = 9;
            try
            {
                conn1.delete();
                throw new TestFailedException();
            }
            catch (SecurityException ex)
            {
            }
            counter = 10;
            conn1.close();

            assertTrue(" run_2450() ok ", true);
        }
        catch (Exception ex)
        {
            append(ex.toString());
            append("TEST FAILED!!!: run_2450(): counter: " + counter + "\n");
            assertTrue(" run_2450() FAILED!!! ", false);
        }
    }// end run_2450()

    void createDirRenameItAndDestroyed(String aPath) throws IOException
    {
        FileConnection conn1 = null;
        String newDir = aPath + "newDir99";
        conn1 = (FileConnection) Connector.open(newDir);
        conn1.mkdir();
        conn1.close();
        conn1 = (FileConnection) Connector.open(newDir);
        conn1.rename("newDir98");
        conn1.close();
        newDir = aPath + "newDir98";
        conn1 = (FileConnection) Connector.open(newDir);
        if (!conn1.exists())
            throw new TestFailedException();
        conn1.delete();
        conn1.setFileConnection("../");
        // Trying to delete and renaming a restricted directory.
        try
        {
            conn1.rename("NewName Dir");
            throw new TestFailedException();
        }
        catch (SecurityException ex)
        {
        }
        try
        {
            conn1.delete();
            throw new TestFailedException();
        }
        catch (SecurityException ex)
        {
        }
        conn1.close();
    }

    //
    //
    //
    //
    private void run_2550()
    {
        FileConnection conn1 = null;
        int counter = 0;
        try
        {
            createDirRenameItAndDestroyed(System
                                          .getProperty("fileconn.dir.photos"));
            counter = 3;
            createDirRenameItAndDestroyed(System
                                          .getProperty("fileconn.dir.videos"));
            counter = 4;
            createDirRenameItAndDestroyed(System
                                          .getProperty("fileconn.dir.graphics"));
            counter = 5;
            createDirRenameItAndDestroyed(System
                                          .getProperty("fileconn.dir.tones"));
            counter = 6;
            createDirRenameItAndDestroyed(System
                                          .getProperty("fileconn.dir.music"));
            counter = 7;
            createDirRenameItAndDestroyed(System
                                          .getProperty("fileconn.dir.recordings"));
            counter = 8;
            createDirRenameItAndDestroyed(System
                                          .getProperty("fileconn.dir.private"));
            counter = 9;
            // The private directory of the midlet is a special case.
            // If is otherwise a restricted directory (cannot be renamed,
            // deleted etc.),
            // but setWritable() can be called for it.
            String url = System.getProperty("fileconn.dir.private");
            counter = 10;
            conn1 = (FileConnection) Connector.open(url);
            counter = 11;
            conn1.setWritable(false);
            counter = 12;
            conn1.setWritable(true);
            counter = 13;
            conn1.close();
            counter = 14;

            assertTrue(" run_2550() ok ", true);
        }
        catch (Exception ex)
        {
            append(ex.toString());
            append("TEST FAILED!!!: run_2550(): counter: " + counter + "\n");
            assertTrue(" run_2550() FAILED!!! ", false);
        }
    }// end run_2550()

    //
    //
    //
    //
    private void run_2571()
    {
        FileConnection conn1 = null;
        int counter = 0;
        try
        {

            conn1 = (FileConnection) Connector
                    .open("file:///Internal/Videos/newDir_2571");
            counter = 1;
            conn1.mkdir();
            counter = 2;
            validateNames(conn1, "newDir_2571/", "/Internal/Videos/",
                          "file:///Internal/Videos/newDir_2571/");
            counter = 3;
            conn1.rename("newDir_2571_new");
            counter = 4;
            validateNames(conn1, "newDir_2571_new/", "/Internal/Videos/",
                          "file:///Internal/Videos/newDir_2571_new/");
            counter = 5;
            conn1.delete();
            counter = 6;
            conn1.close();

            conn1 = (FileConnection) Connector
                    .open("file:///Internal/Videos/newFile_2571");
            counter = 10;
            conn1.create();
            counter = 11;
            validateNames(conn1, "newFile_2571", "/Internal/Videos/",
                          "file:///Internal/Videos/newFile_2571");
            counter = 12;
            conn1.rename("newFile_2571_new");
            counter = 13;
            validateNames(conn1, "newFile_2571_new", "/Internal/Videos/",
                          "file:///Internal/Videos/newFile_2571_new");
            counter = 14;
            conn1.delete();
            counter = 15;
            conn1.close();

            conn1 = (FileConnection) Connector
                    .open("file:///Memory card/newDir_2571");
            counter = 30;
            conn1.mkdir();
            counter = 31;
            validateNames(conn1, "newDir_2571/", "/Memory card/",
                          "file:///Memory%20card/newDir_2571/");
            counter = 32;
            conn1.rename("newDir_2571_new");
            counter = 33;
            validateNames(conn1, "newDir_2571_new/", "/Memory card/",
                          "file:///Memory%20card/newDir_2571_new/");
            counter = 34;
            conn1.delete();
            counter = 35;
            conn1.close();

            conn1 = (FileConnection) Connector
                    .open("file:///Memory card/newFile_2571");
            counter = 40;
            conn1.create();
            counter = 41;
            validateNames(conn1, "newFile_2571", "/Memory card/",
                          "file:///Memory%20card/newFile_2571");
            counter = 42;
            conn1.rename("newFile_2571_new");
            counter = 43;
            validateNames(conn1, "newFile_2571_new", "/Memory card/",
                          "file:///Memory%20card/newFile_2571_new");
            counter = 44;
            conn1.delete();
            counter = 45;
            conn1.close();

            conn1 = (FileConnection) Connector
                    .open("file:///Internal/Videos/newDir_2571");
            counter = 50;
            conn1.mkdir();
            counter = 51;
            validateNames(conn1, "newDir_2571/", "/Internal/Videos/",
                          "file:///Internal/Videos/newDir_2571/");
            counter = 52;
            conn1.close();
            conn1 = (FileConnection) Connector
                    .open("file:///Internal/Videos/newDir_2571/other new dir");
            conn1.mkdir();
            conn1.rename("other new dir 2");
            counter = 53;
            validateNames(conn1, "other new dir 2/",
                          "/Internal/Videos/newDir_2571/",
                          "file:///Internal/Videos/newDir_2571/other%20new%20dir%202/");
            counter = 54;
            conn1.delete();
            counter = 55;
            conn1.close();
            conn1 = (FileConnection) Connector
                    .open("file:///Internal/Videos/newDir_2571");
            counter = 56;
            conn1.delete();
            conn1.close();

            if (iMultiDriveSupportTests)
            {
                counter = 57;

                conn1 = (FileConnection) Connector
                        .open("file:///Internal G/newDir_2571");
                counter = 58;
                conn1.mkdir();
                counter = 59;
                validateNames(conn1, "newDir_2571/", "/Internal G/",
                              "file:///Internal%20G/newDir_2571/");
                counter = 60;
                conn1.rename("newDir_2571_new");
                counter = 61;
                validateNames(conn1, "newDir_2571_new/", "/Internal G/",
                              "file:///Internal%20G/newDir_2571_new/");
                counter = 62;
                conn1.delete();
                counter = 63;
                conn1.close();

                conn1 = (FileConnection) Connector
                        .open("file:///Internal G/newFile_2571");
                counter = 64;
                conn1.create();
                counter = 65;
                validateNames(conn1, "newFile_2571", "/Internal G/",
                              "file:///Internal%20G/newFile_2571");
                counter = 66;
                conn1.rename("newFile_2571_new");
                counter = 67;
                validateNames(conn1, "newFile_2571_new", "/Internal G/",
                              "file:///Internal%20G/newFile_2571_new");
                counter = 68;
                conn1.delete();
                counter = 69;
                conn1.close();
            }

            assertTrue(" run_2571() ok ", true);
        }
        catch (Exception ex)
        {
            append(ex.toString());
            append("TEST FAILED!!!: run_2571(): counter: " + counter + "\n");
            assertTrue(" run_2571() FAILED!!! ", false);
        }
    }// end run_2571()

    //
    //
    //
    //
    private void run_2572()
    {
        FileConnection conn1 = null;
        int counter = 0;
        try
        {

            conn1 = (FileConnection) Connector.open("file:///Memory card/");
            counter = 1;
            try
            {
                conn1.rename("NewMemoryCard");
                throw new TestFailedException();
            }
            catch (SecurityException ex)
            {
            }
            validateNames(conn1, "", "/Memory card/", "file:///Memory%20card/");
            conn1.close();
            counter = 2;

            conn1 = (FileConnection) Connector
                    .open("file:///Internal/Private/");
            counter = 3;
            try
            {
                conn1.rename("New Private");
                throw new TestFailedException();
            }
            catch (SecurityException ex)
            {
            }
            validateNames(conn1, "Private/", "/Internal/",
                          "file:///Internal/Private/");
            conn1.close();
            counter = 4;

            conn1 = (FileConnection) Connector.open("file:///Internal/Private");
            counter = 5;
            try
            {
                conn1.rename("New Private?");
                throw new TestFailedException();
            }
            catch (SecurityException ex)
            {
            }
            validateNames(conn1, "Private/", "/Internal/",
                          "file:///Internal/Private/");
            conn1.close();
            counter = 6;

            if (iMultiDriveSupportTests)
            {
                conn1 = (FileConnection) Connector.open("file:///Internal G/");
                counter = 7;
                try
                {
                    conn1.rename("NewInternalG");
                    throw new TestFailedException();
                }
                catch (SecurityException ex)
                {
                }
                validateNames(conn1, "", "/Internal G/",
                              "file:///Internal%20G/");
                conn1.close();

                counter = 8;
                try
                {
                    conn1 = (FileConnection) Connector
                            .open("file:///Internal G/Private/");
                    throw new TestFailedException(
                        "Internal G/Private/ could be opened");
                }
                catch (SecurityException ex)
                {
                }
                counter = 9;
                conn1.close();
                counter = 10;
            }

            assertTrue(" run_2572() ok ", true);
        }
        catch (Exception ex)
        {
            append(ex.toString());
            append("TEST FAILED!!!: run_2572(): counter: " + counter + "\n");
            assertTrue(" run_2572() FAILED!!! ", false);
        }
    }// end run_2572()

    //
    //
    //
    //
    private void run_2640()
    {
        FileConnection conn1 = null;
        int counter = 0;
        try
        {
            conn1 = (FileConnection) Connector
                    .open("file:///c:/data/sounds/newFile12");
            counter = 1;
            if (conn1.exists())
                throw new TestFailedException();
            conn1.create();
            counter = 2;
            OutputStream outStream = conn1.openOutputStream();
            counter = 3;
            byte[] byteArray1 = new byte[20];
            for (int i = 0; i < byteArray1.length; ++i)
            {
                byteArray1[i] = 75;
            }
            outStream.write(byteArray1);
            counter = 4;
            outStream.flush();
            outStream.close();
            conn1.close();

            conn1 = (FileConnection) Connector
                    .open("file:///c:/data/sounds/newFile12");
            counter = 5;
            conn1.truncate(10);
            conn1.close();
            counter = 6;

            conn1 = (FileConnection) Connector
                    .open("file:///c:/data/sounds/newFile12");
            counter = 7;
            if (!conn1.exists())
                throw new TestFailedException();
            counter = 8;
            InputStream inStream = conn1.openInputStream();
            counter = 9;
            byte[] byteArray2 = new byte[15];
            counter = 10;
            int retValue = inStream.read(byteArray2);
            if (retValue != 10)
                throw new TestFailedException("retValue: " + retValue);
            for (int i = 0; i < retValue; ++i)
            {
                if (byteArray2[i] != 75)
                    throw new TestFailedException();
            }
            inStream.close();
            counter = 11;
            conn1.delete();
            conn1.close();

            assertTrue(" run_2640() ok ", true);
        }
        catch (Exception ex)
        {
            append(ex.toString());
            append("TEST FAILED!!!: run_2640(): counter: " + counter + "\n");
            assertTrue(" run_2640() FAILED!!! ", false);
        }
    }// end run_2640()

    //
    //
    //
    //
    private void run_2740()
    {
        FileConnection conn1 = null;
        int counter = 0;
        try
        {
            // Making a connection to the 'restricted' directory.
            conn1 = (FileConnection) Connector.open("file:///c:/data/");
            counter = 1;
            try
            {
                conn1.setFileConnection("installs/");
                throw new TestFailedException();
            }
            catch (SecurityException ex)
            {
            }
            counter = 2;
            conn1.setFileConnection("images/");
            counter = 3;
            if (false == conn1.isDirectory())
                throw new TestFailedException();
            counter = 4;
            conn1.close();

            // Making a connection to the 'restricted' directory and re-setting
            // connection to other 'restriected' directory.
            conn1 = (FileConnection) Connector.open("file:///c:/data/images/");
            counter = 5;
            conn1.setFileConnection("../");
            counter = 6;
            if (false == conn1.getPath().equals("/c:/"))
                throw new TestFailedException("path: " + conn1.getPath());
            counter = 7;
            conn1.close();

            // Try to make a connection to the DRM file via setFileConnection
            // operation.
            conn1 = (FileConnection) Connector
                    .open("file:///c:/data/images/mydir/ ");
            counter = 8;
            try
            {
                conn1.setFileConnection("drmfile026DecPlayCount50.dcf");
                throw new TestFailedException();
            }
            catch (SecurityException ex)
            {
            }
            counter = 9;
            conn1.close();

            // Same test as above for .jar and .jad files.
            conn1 = (FileConnection) Connector
                    .open("file:///c:/data/images/mydir/");
            counter = 10;
            try
            {
                conn1.setFileConnection("something.jad");
                throw new TestFailedException();
            }
            catch (SecurityException ex)
            {
            }
            counter = 11;
            try
            {
                conn1.setFileConnection("something.jar");
                throw new TestFailedException();
            }
            catch (SecurityException ex)
            {
            }
            counter = 12;
            try
            {
                conn1.setFileConnection("something.jar  ");
                throw new TestFailedException();
            }
            catch (SecurityException ex)
            {
            }
            counter = 13;
            try
            {
                conn1.setFileConnection("something.jad  \n");
                throw new TestFailedException();
            }
            catch (SecurityException ex)
            {
            }
            counter = 14;
            try
            {
                conn1.setFileConnection("something.jad.jad");
                throw new TestFailedException();
            }
            catch (SecurityException ex)
            {
            }
            counter = 15;
            try
            {
                conn1.setFileConnection("something.jar.jad");
                throw new TestFailedException();
            }
            catch (SecurityException ex)
            {
            }
            counter = 16;
            conn1.close();

            // Testing that myTest.jad.File.jak can be created onto filesystem.
            conn1 = (FileConnection) Connector
                    .open("file:///c:/data/images/mydir/myTest.jad.File.jak");
            counter = 17;
            if (conn1.exists())
                throw new TestFailedException();
            conn1.create();
            counter = 18;
            conn1.close();

            // Testing that myTest.jar.File.jak can be created onto filesystem.
            conn1 = (FileConnection) Connector
                    .open("file:///c:/data/images/mydir/myTest.jar.File.jak");
            counter = 19;
            if (conn1.exists())
                throw new TestFailedException();
            conn1.create();
            conn1.close();

            assertTrue(" run_2740() ok ", true);
        }
        catch (Exception ex)
        {
            append(ex.toString());
            append("TEST FAILED!!!: run_2740(): counter: " + counter + "\n");
            assertTrue(" run_2740() FAILED!!! ", false);
        }
    }// end run_2740()

    //
    //
    //
    //
    private void run_2750()
    {
        FileConnection conn1 = null;
        int counter = 0;
        try
        {
            conn1 = (FileConnection) Connector
                    .open("file:///c:/data/images/myDir/myImages/");
            counter = 1;
            conn1.setWritable(false);
            counter = 11;
            conn1.setHidden(true);
            counter = 2;

            conn1.setFileConnection("../");
            counter = 3;
            String dirName = conn1.getName();
            if (!dirName.equals("myDir/"))
                throw new TestFailedException("dirName: " + dirName);
            counter = 3;
            conn1.setWritable(false);
            counter = 4;
            conn1.setHidden(true);
            counter = 44;

            conn1.setFileConnection("../");
            counter = 5;
            dirName = conn1.getName();
            if (!dirName.equals("images/"))
                throw new TestFailedException("dirName: " + dirName);
            counter = 6;
            try
            {
                conn1.setWritable(false);
                throw new TestFailedException();
            }
            catch (SecurityException ex)
            {
            }
            counter = 7;
            try
            {
                conn1.setHidden(true);
                throw new TestFailedException();
            }
            catch (SecurityException ex)
            {
            }
            counter = 71;

            conn1.setFileConnection("../");
            counter = 8;
            dirName = conn1.getName();
            if (!dirName.equals("data/"))
                throw new TestFailedException("dirName: " + dirName);
            counter = 9;
            try
            {
                conn1.setWritable(false);
                throw new TestFailedException();
            }
            catch (SecurityException ex)
            {
            }
            counter = 10;
            try
            {
                conn1.setHidden(true);
                throw new TestFailedException();
            }
            catch (SecurityException ex)
            {
            }
            counter = 100;

            conn1.setFileConnection("../");
            counter = 11;
            dirName = conn1.getName();
            if (!dirName.equals(""))
                throw new TestFailedException("dirName: " + dirName);
            counter = 12;
            try
            {
                conn1.setWritable(false);
                throw new TestFailedException();
            }
            catch (SecurityException ex)
            {
            }
            counter = 13;
            try
            {
                conn1.setHidden(true);
                throw new TestFailedException();
            }
            catch (SecurityException ex)
            {
            }
            counter = 130;
            conn1.close();

            assertTrue(" run_2750() ok ", true);
        }
        catch (Exception ex)
        {
            append(ex.toString());
            append("TEST FAILED!!!: run_2750(): counter: " + counter + "\n");
            assertTrue(" run_2750() FAILED!!! ", false);
        }
    }// end run_2750()

    private void createAndDeleteInPropertyDir(String aPropertyDir)
    throws IOException
    {
        FileConnection conn1 = null;
        int counter = 0;
        try
        {
            String dirUrl = aPropertyDir + "a/";
            counter = 1;
            conn1 = (FileConnection) Connector.open(dirUrl);
            counter = 2;
            conn1.mkdir();
            conn1.close();
            counter = 3;
            String fileName = dirUrl + "newFile";
            conn1 = (FileConnection) Connector.open(fileName);
            counter = 4;
            conn1.create();
            conn1.close();

            dirUrl = dirUrl + "a/";
            counter = 5;
            conn1 = (FileConnection) Connector.open(dirUrl);
            counter = 6;
            conn1.mkdir();
            conn1.close();
            counter = 7;
            fileName = dirUrl + "newFile";
            conn1 = (FileConnection) Connector.open(fileName);
            counter = 8;
            conn1.create();
            conn1.close();
            counter = 9;
            dirUrl = dirUrl + "a/";
            conn1 = (FileConnection) Connector.open(dirUrl);
            counter = 91;
            conn1.mkdir();
            conn1.close();
            counter = 92;

            dirUrl = aPropertyDir + "a/a/";
            conn1 = (FileConnection) Connector.open(dirUrl);
            counter = 10;
            conn1.setFileConnection("newFile");
            counter = 11;
            conn1.delete();
            conn1.close();

            dirUrl = aPropertyDir + "a/";
            counter = 12;
            conn1 = (FileConnection) Connector.open(dirUrl);
            counter = 13;
            conn1.setFileConnection("newFile");
            counter = 14;
            conn1.delete();
            conn1.close();

            dirUrl = aPropertyDir + "a/a/a/";
            counter = 15;
            conn1 = (FileConnection) Connector.open(dirUrl);
            counter = 16;
            conn1.delete();
            counter = 17;
            conn1.setFileConnection("..");
            counter = 18;
            conn1.delete();
            counter = 19;
            conn1.setFileConnection("..");
            counter = 20;
            conn1.delete();
            counter = 21;
            conn1.setFileConnection("..");
            counter = 22;
            try
            {
                conn1.delete();
                throw new TestFailedException();
            }
            catch (SecurityException ex)
            {
            }
            conn1.close();
        }
        catch (Exception ex)
        {
            throw new TestFailedException(ex.toString() + "\ncount#1: "
                                          + counter);
        }
        finally
        {
            if (conn1 != null)
                conn1.close();
        }
    }

    private void ExecuteCreateAndDeleteInPropertyDirTest() throws IOException
    {
        FileConnection conn1 = null;
        int counter = 0;
        try
        {
            String property = System.getProperty("fileconn.dir.tones");
            createAndDeleteInPropertyDir(property);
            counter = 1;
            conn1 = (FileConnection) Connector.open(property);
            counter = 2;
            conn1.setFileConnection("DiGital/");
            counter = 3;
            try
            {
                conn1.delete();
                throw new TestFailedException();
            }
            catch (SecurityException ex)
            {
            }
            conn1.close();
            counter = 4;

            property = System.getProperty("fileconn.dir.recordings");
            createAndDeleteInPropertyDir(property);
            counter = 5;

            property = System.getProperty("fileconn.dir.private");
            createAndDeleteInPropertyDir(property);
            counter = 6;

            property = System.getProperty("fileconn.dir.memorycard");
            createAndDeleteInPropertyDir(property);
            counter = 7;

            property = System.getProperty("fileconn.dir.photos");
            createAndDeleteInPropertyDir(property);
            counter = 8;

            property = System.getProperty("fileconn.dir.graphics");
            createAndDeleteInPropertyDir(property);
            counter = 9;

            property = System.getProperty("fileconn.dir.videos");
            createAndDeleteInPropertyDir(property);
            counter = 11;

            property = System.getProperty("fileconn.dir.music");
            createAndDeleteInPropertyDir(property);
            counter = 12;
        }
        catch (Exception ex)
        {
            throw new TestFailedException(ex.toString() + "\ncount#2: "
                                          + counter);
        }
        finally
        {
            if (conn1 != null)
                conn1.close();
        }
    }

    //
    //
    //
    //
    private void run_2780()
    {
        FileConnection conn1 = null;
        int counter = 0;
        try
        {
            ExecuteCreateAndDeleteInPropertyDirTest();

            assertTrue(" run_2780() ok ", true);
        }
        catch (Exception ex)
        {
            append(ex.toString());
            append("TEST FAILED!!!: run_2780(): counter: " + counter + "\n");
            assertTrue(" run_2780() FAILED!!! ", false);
        }
    }// end run_2780()

    //
    //
    //
    //
    private void run_2791()
    {
        FileConnection conn1 = null;
        int counter = 0;
        try
        {

            conn1 = (FileConnection) Connector.open("file:///Internal/");
            counter = 1;
            validateNames(conn1, "", "/Internal/", "file:///Internal/");
            counter = 2;
            conn1.setFileConnection("Images/");
            counter = 3;
            validateNames(conn1, "Images/", "/Internal/",
                          "file:///Internal/Images/");
            counter = 4;
            conn1.setFileConnection("Mydir/");
            counter = 5;
            validateNames(conn1, "Mydir/", "/Internal/Images/",
                          "file:///Internal/Images/Mydir/");
            counter = 6;
            conn1.setFileConnection("myImages2/");
            counter = 7;
            validateNames(conn1, "myImages2/", "/Internal/Images/Mydir/",
                          "file:///Internal/Images/Mydir/myImages2/");
            counter = 8;
            conn1.setFileConnection("myImagesFile.txt");
            counter = 9;
            validateNames(conn1, "myImagesFile.txt",
                          "/Internal/Images/Mydir/myImages2/",
                          "file:///Internal/Images/Mydir/myImages2/myImagesFile.txt");
            counter = 10;
            if (!conn1.canWrite())
                throw new TestFailedException();
            conn1.close();

            conn1 = (FileConnection) Connector
                    .open("file:///Internal/Images/Mydir/myImages2/");
            counter = 11;
            validateNames(conn1, "myImages2/", "/Internal/Images/Mydir/",
                          "file:///Internal/Images/Mydir/myImages2/");
            counter = 12;
            conn1.setFileConnection("../ ");
            counter = 13;
            validateNames(conn1, "Mydir/", "/Internal/Images/",
                          "file:///Internal/Images/Mydir/");
            counter = 14;
            conn1.setFileConnection(".. ");
            counter = 15;
            validateNames(conn1, "Images/", "/Internal/",
                          "file:///Internal/Images/");
            counter = 16;
            conn1.setFileConnection("..");
            counter = 17;
            validateNames(conn1, "", "/Internal/", "file:///Internal/");
            counter = 18;
            conn1.close();

            conn1 = (FileConnection) Connector.open("file:///Internal/Videos");
            counter = 19;
            validateNames(conn1, "Videos/", "/Internal/",
                          "file:///Internal/Videos/");
            counter = 20;
            conn1.setFileConnection("..");
            counter = 21;
            validateNames(conn1, "", "/Internal/", "file:///Internal/");
            conn1.close();

            conn1 = (FileConnection) Connector
                    .open("file:///Internal/Videos/newDir1976");
            counter = 22;
            validateNames(conn1, "newDir1976", "/Internal/Videos/",
                          "file:///Internal/Videos/newDir1976");
            counter = 23;
            conn1.setFileConnection("..");
            counter = 24;
            validateNames(conn1, "Videos/", "/Internal/",
                          "file:///Internal/Videos/");
            counter = 25;
            conn1.close();

            conn1 = (FileConnection) Connector.open("file:///Memory card/");
            counter = 26;
            try
            {
                conn1.setFileConnection("..");
                throw new TestFailedException();
            }
            catch (IOException ex)
            {
            }
            conn1.close();
            counter = 27;

            conn1 = (FileConnection) Connector
                    .open("file:///Memory card/new file");
            counter = 28;
            conn1.create();
            counter = 29;
            conn1.close();
            conn1 = (FileConnection) Connector
                    .open("file:///Memory card/new dir");
            counter = 30;
            conn1.mkdir();
            counter = 31;
            conn1.close();
            conn1 = (FileConnection) Connector.open("file:///Memory card/");
            counter = 32;
            conn1.setFileConnection("new file");
            counter = 33;
            conn1.delete();
            conn1.close();
            counter = 34;
            conn1 = (FileConnection) Connector.open("file:///Memory card/");
            counter = 35;
            conn1.setFileConnection("new dir/");
            counter = 36;
            conn1.delete();
            conn1.close();

            counter = 37;
            conn1 = (FileConnection) Connector.open("file:///Internal/");
            try
            {
                conn1.setFileConnection("logs/");
                throw new TestFailedException();
            }
            catch (SecurityException ex)
            {
            }
            conn1.close();
            counter = 38;

            if (iMultiDriveSupportTests)
            {
                conn1 = (FileConnection) Connector.open("file:///Internal G/");
                counter = 39;
                try
                {
                    conn1.setFileConnection("..");
                    throw new TestFailedException();
                }
                catch (IOException ex)
                {
                }
                conn1.close();
                counter = 40;

                conn1 = (FileConnection) Connector
                        .open("file:///Internal G/new file");
                counter = 41;
                conn1.create();
                counter = 42;
                conn1.close();
                conn1 = (FileConnection) Connector
                        .open("file:///Internal G/new dir");
                counter = 43;
                conn1.mkdir();
                counter = 44;
                conn1.close();
                conn1 = (FileConnection) Connector.open("file:///Internal G/");
                counter = 45;
                conn1.setFileConnection("new file");
                counter = 46;
                conn1.delete();
                conn1.close();
                counter = 47;
                conn1 = (FileConnection) Connector.open("file:///Internal G/");
                counter = 48;
                conn1.setFileConnection("new dir/");
                counter = 49;
                conn1.delete();
                conn1.close();
                counter = 50;
            }

            assertTrue(" run_2791() ok ", true);
        }
        catch (Exception ex)
        {
            append(ex.toString());
            append("TEST FAILED!!!: run_2791(): counter: " + counter + "\n");
            assertTrue(" run_2791() FAILED!!! ", false);
        }
    }// end run_2791()

    //
    //
    //
    //
    private void run_4000()
    {
        FileConnection conn1 = null;
        int counter = 0;
        try
        {
            // Preconditions for this testcase:
            // 1. Add following line into the \epoc32\data\epoc.ini file
            // in the emulator:
            // _EPOC_DRIVE_E \epoc32\wins\e
            // 2. Install two midlets to the memory card.
            // 3. Make a 'nokia' directory to the e:\ root.
            // 4. Remove 'scratch' directory from Midlet's directory if it
            // exists.

            conn1 = (FileConnection) Connector.open("file:///e:/");
            counter = 111;
            Enumeration dirList = conn1.list("*", true);
            counter = 112;
            if (false == dirList.hasMoreElements())
                throw new TestFailedException();
            int elementCounter = 0;
            while (dirList.hasMoreElements())
            {
                String obj = (String) dirList.nextElement();
                obj = obj.toLowerCase();
                if (obj.equals("private/"))
                    ++elementCounter;
                // In 3.0, system directory is not returned.
                // Reason why "if" statement below is that elementCounter check
                // causes
                // error if system/ directory for reason is returned.
                if (obj.equals("system/"))
                    ++elementCounter;
            }// end while
            counter = 113;
            if (elementCounter != 1)
                throw new TestFailedException("elementCounter: "
                                              + elementCounter);
            counter = 114;
            conn1.close();

            conn1 = (FileConnection) Connector.open("file:///Memory card/");
            counter = 200;
            dirList = conn1.list("*", true);
            counter = 201;
            if (false == dirList.hasMoreElements())
                throw new TestFailedException();
            elementCounter = 0;
            while (dirList.hasMoreElements())
            {
                String obj = (String) dirList.nextElement();
                obj = obj.toLowerCase();
                // append(obj + "\n");
                if (obj.equals("private/"))
                    ++elementCounter;
                // In 3.0, system directory is not returned.
                // Reason why "if" statement below is that elementCounter check
                // causes
                // error if system/ directory for reason is returned.
                if (obj.equals("system/"))
                    ++elementCounter;
            }// end while
            counter = 203;
            if (elementCounter != 1)
                throw new TestFailedException("elementCounter: "
                                              + elementCounter);
            counter = 204;
            conn1.close();

            conn1 = (FileConnection) Connector
                    .open("file:///e:/Private/102033E6/MIDlets/");
            counter = 1;
            dirList = conn1.list("*", true);
            if (false == dirList.hasMoreElements())
                throw new TestFailedException();
            counter = 2;
            elementCounter = 0;
            boolean failedFlag = false;
            String url = System.getProperty("fileconn.dir.private");
            int startIndex = url.indexOf('[');
            int lastIndex = url.lastIndexOf(']');
            String uidDirStr = url.substring(startIndex, lastIndex + 1) + "/";
            String midletDir = null;
            while (dirList.hasMoreElements())
            {
                midletDir = (String) dirList.nextElement();
                if (midletDir.equals(uidDirStr))
                    ++elementCounter;
                else
                    failedFlag = true;
            }// end while
            counter = 3;
            if (elementCounter != 1)
                throw new TestFailedException("elementCounter: "
                                              + elementCounter);
            if (failedFlag == true)
                throw new TestFailedException();
            counter = 31;
            try
            {
                conn1.setHidden(true);
                throw new TestFailedException();
            }
            catch (SecurityException ex)
            {
            }
            counter = 32;
            try
            {
                conn1.setWritable(false);
                throw new TestFailedException();
            }
            catch (SecurityException ex)
            {
            }
            counter = 33;
            try
            {
                conn1.rename("newMidletDir0");
                throw new TestFailedException();
            }
            catch (SecurityException ex)
            {
            }
            counter = 34;
            try
            {
                conn1.delete();
                throw new TestFailedException();
            }
            catch (SecurityException ex)
            {
            }
            counter = 4;

            // Making a connection to the MIDlet's directory.
            conn1.setFileConnection(midletDir);
            counter = 41;
            try
            {
                conn1.setHidden(true);
                throw new TestFailedException();
            }
            catch (SecurityException ex)
            {
            }
            counter = 42;
            try
            {
                conn1.setWritable(false);
                throw new TestFailedException();
            }
            catch (SecurityException ex)
            {
            }
            counter = 43;
            try
            {
                conn1.rename("newMidletDir");
                throw new TestFailedException();
            }
            catch (SecurityException ex)
            {
            }
            counter = 44;
            try
            {
                conn1.delete();
                throw new TestFailedException();
            }
            catch (SecurityException ex)
            {
            }
            counter = 45;
            dirList = conn1.list("*", true);
            if (false == dirList.hasMoreElements())
                throw new TestFailedException();
            counter = 5;
            elementCounter = 0;
            failedFlag = false;
            while (dirList.hasMoreElements())
            {
                String obj = (String) dirList.nextElement();
                if (obj.equals("scratch/"))
                    ++elementCounter;
                else
                    failedFlag = true;
            }// end while
            counter = 6;
            if (elementCounter != 1)
                throw new TestFailedException("elementCounter: "
                                              + elementCounter);
            if (failedFlag == true)
                throw new TestFailedException();
            counter = 7;
            conn1.close();

            // Trying to make a connection to other MIDlet's directory.
            try
            {
                conn1 = (FileConnection) Connector.open(iNonUsedMidletPath);
                counter = 15;
                throw new TestFailedException();
            }
            catch (SecurityException ex)
            {
            }

            assertTrue(" run_4000() ok ", true);
        }
        catch (Exception ex)
        {
            append(ex.toString());
            append("TEST FAILED!!!: run_4000(): counter: " + counter + "\n");
            assertTrue(" run_4000() FAILED!!! ", false);
        }
    }// end run_4000()

    //
    //
    //
    //
    private void run_4010()
    {
        FileConnection conn1 = null;
        int counter = 0;
        try
        {
            // This testcase can be executed with testcase run_4000.

            conn1 = (FileConnection) Connector.open("file:///e:/newDir");
            counter = 1;
            if (conn1.exists())
                throw new TestFailedException();
            conn1.mkdir();
            counter = 2;
            conn1.close();

            conn1 = (FileConnection) Connector
                    .open("file:///e:/newDIR/otherDir");
            counter = 3;
            if (conn1.exists())
                throw new TestFailedException();
            conn1.mkdir();
            counter = 4;
            conn1.close();

            // Creating a file to the root of the e:/newDir/ directory.
            conn1 = (FileConnection) Connector
                    .open("file:///e:/newDir/new file");
            counter = 5;
            if (conn1.exists())
                throw new TestFailedException();
            conn1.create();
            counter = 6;
            conn1.setHidden(true);
            counter = 7;
            conn1.setWritable(false);
            counter = 8;
            conn1.rename("new file2");
            counter = 9;
            conn1.close();

            conn1 = (FileConnection) Connector.open("file:///e:/newDir/");
            counter = 10;
            if (!conn1.exists())
                throw new TestFailedException();
            counter = 101;
            Enumeration dirList = conn1.list("*", true);
            if (false == dirList.hasMoreElements())
                throw new TestFailedException();
            counter = 11;
            int elementCounter = 0;
            boolean failedFlag = false;
            while (dirList.hasMoreElements())
            {
                String obj = (String) dirList.nextElement();
                if (obj.equals("new file2"))
                    ++elementCounter;
                else if (obj.equals("otherDir/"))
                    ++elementCounter;
                else
                    failedFlag = true;
            }// end while
            counter = 12;
            if (elementCounter != 2)
                throw new TestFailedException("elementCounter: "
                                              + elementCounter);
            if (failedFlag == true)
                throw new TestFailedException();
            counter = 13;
            conn1.setFileConnection("new file2");
            counter = 14;
            conn1.setWritable(true);
            counter = 141;
            conn1.delete();
            counter = 15;
            conn1.close();

            // Creating a file to the root of the e:/ drive.
            conn1 = (FileConnection) Connector.open("file:///e:/newFile.txt");
            counter = 16;
            if (conn1.exists())
                throw new TestFailedException();
            conn1.create();
            counter = 17;
            conn1.delete();
            conn1.close();

            conn1 = (FileConnection) Connector
                    .open("file:///e:/newDIR/otherDir");
            counter = 18;
            conn1.delete();
            counter = 19;
            conn1.setFileConnection("../");
            counter = 20;
            conn1.delete();
            counter = 21;
            conn1.close();

            // Creating a directory to the root of the e:/newDir/ directory.
            conn1 = (FileConnection) Connector.open("file:///e:/newDir/");
            counter = 22;
            if (conn1.exists())
                throw new TestFailedException();
            conn1.mkdir();
            counter = 23;
            conn1.setHidden(true);
            counter = 24;
            conn1.setWritable(false);
            counter = 25;
            conn1.rename("newDir2/");
            counter = 26;
            conn1.close();

            // Deleting a 'newDir2' directory.
            conn1 = (FileConnection) Connector.open("file:///e:/newDir2/");
            counter = 27;
            if (!conn1.exists())
                throw new TestFailedException();
            counter = 28;
            conn1.setWritable(true);
            counter = 29;
            conn1.delete();
            conn1.close();

            conn1 = (FileConnection) Connector
                    .open("file:///Memory card/private/new file");
            counter = 31;
            conn1.create();
            counter = 32;
            conn1.close();
            String url = System.getProperty("fileconn.dir.private");
            conn1 = (FileConnection) Connector.open(url + "new file");
            counter = 33;
            conn1.delete();
            counter = 34;
            conn1.close();

            conn1 = (FileConnection) Connector
                    .open("file:///Memory card/private/new dir/");
            counter = 35;
            conn1.mkdir();
            counter = 36;
            conn1.close();
            conn1 = (FileConnection) Connector.open(url + "new dir/");
            counter = 37;
            conn1.delete();
            counter = 38;
            conn1.close();

            assertTrue(" run_4010() ok ", true);
        }
        catch (Exception ex)
        {
            append(ex.toString());
            append("TEST FAILED!!!: run_4010(): counter: " + counter + "\n");
            assertTrue(" run_4010() FAILED!!! ", false);
        }
    }// end run_4010()

    //
    //
    //
    //
    private void run_4020()
    {
        FileConnection conn1 = null;
        int counter = 0;
        try
        {
            // This testcase can be executed with testcase run_4000.
            // Note: Update 'nonUsedMidletPath' string after installation of the
            // "other" midlet. See more information from test case run_4000.
            String nonUsedMidletPath = new String(
                "file:///e:/Private/102033E6/MIDlets/[10127026]/");

            // Making a connection to the e:/private/ directory.
            conn1 = (FileConnection) Connector
                    .open("file:///e:/private/102033E6/");
            if (!conn1.exists())
                throw new TestFailedException();
            counter = 1;
            Enumeration dirList = conn1.list("*", true);
            if (false == dirList.hasMoreElements())
                throw new TestFailedException();
            counter = 2;
            int elementCounter = 0;
            boolean failedFlag = false;
            while (dirList.hasMoreElements())
            {
                String obj = (String) dirList.nextElement();
                if (obj.equals("MIDlets/"))
                    ++elementCounter;
                else
                    failedFlag = true;
            }// end while
            counter = 3;
            if (elementCounter != 1)
                throw new TestFailedException("elementCounter: "
                                              + elementCounter);
            if (failedFlag == true)
                throw new TestFailedException();
            counter = 4;
            try
            {
                conn1.setHidden(true);
                throw new TestFailedException();
            }
            catch (SecurityException ex)
            {
            }
            counter = 5;
            try
            {
                conn1.setWritable(false);
                throw new TestFailedException();
            }
            catch (SecurityException ex)
            {
            }
            counter = 6;
            try
            {
                conn1.rename("newMidletDir");
                throw new TestFailedException();
            }
            catch (SecurityException ex)
            {
            }
            counter = 7;
            try
            {
                conn1.delete();
                throw new TestFailedException();
            }
            catch (SecurityException ex)
            {
            }
            counter = 8;
            conn1.close();

            // Trying to create a new dir to the e:/private/102033E6/ directory.
            try
            {
                conn1 = (FileConnection) Connector
                        .open("file:///e:/private/102033E6/new system dir/");
                throw new TestFailedException();
            }
            catch (SecurityException ex)
            {
            }
            counter = 9;

            // Trying to create a new file to the e:/private/102033E6/
            // directory.
            try
            {
                conn1 = (FileConnection) Connector
                        .open("file:///e:/private/102033E6/newSystemFILE");
                throw new TestFailedException();
            }
            catch (SecurityException ex)
            {
            }
            counter = 10;

            // Trying to make a connection to the e:/system/ directory.
            try
            {
                conn1 = (FileConnection) Connector.open("file:///e:/system/");
                throw new TestFailedException();
            }
            catch (SecurityException ex)
            {
            }
            counter = 11;

            // Trying to make a connection to the "other midlet's" directory.
            try
            {
                conn1 = (FileConnection) Connector.open(iNonUsedMidletPath);
                throw new TestFailedException();
            }
            catch (SecurityException ex)
            {
            }
            counter = 12;

            // Trying to delete e: root.
            conn1 = (FileConnection) Connector.open("file:///e:/");
            counter = 13;
            try
            {
                conn1.setHidden(true);
                throw new TestFailedException();
            }
            catch (SecurityException ex)
            {
            }
            counter = 14;
            try
            {
                conn1.setWritable(false);
                throw new TestFailedException();
            }
            catch (SecurityException ex)
            {
            }
            counter = 15;
            try
            {
                conn1.rename("newMidletDir");
                throw new TestFailedException();
            }
            catch (SecurityException ex)
            {
            }
            counter = 16;
            try
            {
                conn1.delete();
                throw new TestFailedException();
            }
            catch (SecurityException ex)
            {
            }
            conn1.close();
            counter = 17;

            conn1 = (FileConnection) Connector
                    .open("file:///e:/newMyDirectory/");
            if (conn1.exists())
                throw new TestFailedException();
            conn1.mkdir();
            counter = 18;
            conn1.setFileConnection("../");
            counter = 19;
            dirList = conn1.list("*", true);
            if (false == dirList.hasMoreElements())
                throw new TestFailedException();
            counter = 20;
            elementCounter = 0;
            failedFlag = false;
            while (dirList.hasMoreElements())
            {
                String obj = (String) dirList.nextElement();
                obj = obj.toLowerCase();
                if (obj.equals("newmydirectory/"))
                    ++elementCounter;
                else if (obj.equals("private/"))
                    ++elementCounter;
                // At least those two directories have to be found.
                // else
                // failedFlag = true;
            }// end while
            counter = 21;
            if (elementCounter != 2)
                throw new TestFailedException("elementCounter: "
                                              + elementCounter);
            if (failedFlag == true)
                throw new TestFailedException();
            counter = 22;
            conn1.setFileConnection("newMyDirectory/");
            counter = 23;
            conn1.delete();
            counter = 24;
            conn1.close();

            assertTrue(" run_4020() ok ", true);
        }
        catch (Exception ex)
        {
            append(ex.toString());
            append("TEST FAILED!!!: run_4020(): counter: " + counter + "\n");
            assertTrue(" run_4020() FAILED!!! ", false);
        }
    }// end run_4020()

    //
    //
    //
    //
    private void run_4021()
    {
        FileConnection conn1 = null;
        int counter = 0;
        try
        {
            // Making a new sub-directory under a 'scratch' directory.
            String url = System.getProperty("fileconn.dir.private");
            counter = 1;
            conn1 = (FileConnection) Connector.open(url);
            counter = 2;
            if (!conn1.exists())
                throw new TestFailedException();
            counter = 3;
            conn1.close();
            // Creating file under a Midlet's 'scratch' directory.
            conn1 = (FileConnection) Connector.open(url + "new dir");
            counter = 31;
            if (conn1.exists())
                throw new TestFailedException();
            conn1.mkdir();
            counter = 32;
            conn1.close();

            // Start to browse from the root to the above created directory.
            conn1 = (FileConnection) Connector.open("file:///e:/");
            counter = 4;
            Enumeration dirList = conn1.list();
            if (false == dirList.hasMoreElements())
                throw new TestFailedException();
            counter = 5;
            int elementCounter = 0;
            boolean failedFlag = false;
            while (dirList.hasMoreElements())
            {
                String obj = (String) dirList.nextElement();
                obj = obj.toLowerCase();
                // append(obj + "\n");
                if (obj.equals("private/"))
                    ++elementCounter;
            }// end while
            if (elementCounter != 1)
                throw new TestFailedException("elementCounter: "
                                              + elementCounter);
            counter = 6;

            // Setting connection to the c:/private/ directory.
            conn1.setFileConnection("private/");
            counter = 7;
            if (false == conn1.exists())
                throw new TestFailedException();
            counter = 71;
            if (false == conn1.isDirectory())
                throw new TestFailedException();
            counter = 72;
            if (-1 != conn1.directorySize(true))
                throw new TestFailedException("directory size: "
                                              + conn1.directorySize(true) + "\n");
            counter = 73;
            if (0 != conn1.lastModified())
                throw new TestFailedException("lastModified(): "
                                              + conn1.lastModified() + "\n");
            counter = 74;
            dirList = conn1.list();
            if (false == dirList.hasMoreElements())
                throw new TestFailedException();
            counter = 8;
            elementCounter = 0;
            failedFlag = false;
            while (dirList.hasMoreElements())
            {
                String obj = (String) dirList.nextElement();
                obj = obj.toLowerCase();
                // append("obj: " + obj + "\n");
                if (obj.equals("102033e6/"))
                    ++elementCounter;
                else
                    failedFlag = true;
            }// end while
            if (elementCounter != 1)
                throw new TestFailedException("elementCounter: "
                                              + elementCounter);
            if (failedFlag == true)
                throw new TestFailedException();
            counter = 9;

            try
            {
                conn1.setFileConnection(iOtherApplicationId);
                throw new TestFailedException(
                    "Connection to the forbidden directory succeed");
            }
            catch (SecurityException ex)
            {
            }
            counter = 900;

            // Setting connection to the c:/private/102033E6/ directory.
            conn1.setFileConnection("102033E6/");
            counter = 91;
            dirList = conn1.list();
            if (false == dirList.hasMoreElements())
                throw new TestFailedException();
            counter = 92;
            elementCounter = 0;
            failedFlag = false;
            while (dirList.hasMoreElements())
            {
                String obj = (String) dirList.nextElement();
                if (obj.equals("MIDlets/"))
                    ++elementCounter;
                else
                    failedFlag = true;
            }// end while
            if (elementCounter != 1)
                throw new TestFailedException("elementCounter: "
                                              + elementCounter);
            if (failedFlag == true)
                throw new TestFailedException();
            counter = 93;

            // Setting connection to the c:/private/102033E6/MIDlets directory.
            conn1.setFileConnection("MIDlets/");
            counter = 10;
            dirList = conn1.list();
            if (false == dirList.hasMoreElements())
                throw new TestFailedException();
            counter = 11;
            elementCounter = 0;
            failedFlag = false;
            url = System.getProperty("fileconn.dir.private");
            int startIndex = url.indexOf('[');
            int lastIndex = url.lastIndexOf(']');
            String uidDirStr = url.substring(startIndex, lastIndex + 1) + "/";
            // append("uidDirStr: " + uidDirStr);
            while (dirList.hasMoreElements())
            {
                String obj = (String) dirList.nextElement();
                // append("obj: " + obj);
                if (obj.equals(uidDirStr))
                    ++elementCounter;
                else
                    failedFlag = true;
            }// end while
            if (elementCounter != 1)
                throw new TestFailedException("elementCounter: "
                                              + elementCounter);
            if (failedFlag == true)
                throw new TestFailedException();
            counter = 12;

            // Setting connection to the c:/system/MIDlets/[#UID] directory.
            conn1.setFileConnection(uidDirStr);
            counter = 13;
            dirList = conn1.list();
            if (false == dirList.hasMoreElements())
                throw new TestFailedException();
            elementCounter = 0;
            failedFlag = false;
            while (dirList.hasMoreElements())
            {
                String obj = (String) dirList.nextElement();
                if (obj.equals("scratch/"))
                    ++elementCounter;
                else
                    failedFlag = true;
            }// end while
            if (elementCounter != 1)
                throw new TestFailedException("elementCounter: "
                                              + elementCounter);
            if (failedFlag == true)
                throw new TestFailedException();
            counter = 14;

            // Setting connection to the c:/system/MIDlets/[#UID]/scratch
            // directory.
            conn1.setFileConnection("scratch/");
            counter = 15;
            dirList = conn1.list();
            if (false == dirList.hasMoreElements())
                throw new TestFailedException();
            elementCounter = 0;
            failedFlag = false;
            while (dirList.hasMoreElements())
            {
                String obj = (String) dirList.nextElement();
                if (obj.equals("new dir/"))
                    ++elementCounter;
                else
                    failedFlag = true;
            }// end while
            if (elementCounter != 1)
                throw new TestFailedException("elementCounter: "
                                              + elementCounter);
            if (failedFlag == true)
                throw new TestFailedException();
            counter = 16;
            conn1.close();

            try
            {
                FileConnection tmpConn2 = (FileConnection) Connector
                                          .open("file:///e:/private/" + iOtherApplicationId);
                tmpConn2.close();
                throw new TestFailedException(
                    "Connection to the forbidden directory succeed");
            }
            catch (SecurityException ex)
            {
            }
            counter = 160;

            // Make a connection to the c:/private/ directory using
            // Connector.open() operation.
            conn1 = (FileConnection) Connector.open("file:///e:/private/");
            counter = 1000;
            if (false == conn1.exists())
                throw new TestFailedException();
            counter = 1001;
            if (false == conn1.isDirectory())
                throw new TestFailedException();
            counter = 1002;
            if (-1 != conn1.directorySize(true))
                throw new TestFailedException("directory size: "
                                              + conn1.directorySize(true) + "\n");
            counter = 1003;
            if (0 != conn1.lastModified())
                throw new TestFailedException("lastModified(): "
                                              + conn1.lastModified() + "\n");
            counter = 1004;
            dirList = conn1.list();
            if (false == dirList.hasMoreElements())
                throw new TestFailedException();
            counter = 1005;
            elementCounter = 0;
            failedFlag = false;
            while (dirList.hasMoreElements())
            {
                String obj = (String) dirList.nextElement();
                obj = obj.toLowerCase();
                // append("obj: " + obj + "\n");
                if (obj.equals("102033e6/"))
                    ++elementCounter;
                else
                    failedFlag = true;
            }// end while
            if (elementCounter != 1)
                throw new TestFailedException("elementCounter: "
                                              + elementCounter);
            if (failedFlag == true)
                throw new TestFailedException();
            counter = 1006;
            conn1.close();

            // Deleting 'new dir' directory.
            counter = 161;
            url = System.getProperty("fileconn.dir.private");
            conn1 = (FileConnection) Connector.open(url + "new dir");
            counter = 162;
            conn1.delete();
            counter = 163;
            conn1.close();

            assertTrue(" run_4021() ok ", true);
        }
        catch (Exception ex)
        {
            append(ex.toString());
            append("TEST FAILED!!!: run_4021(): counter: " + counter + "\n");
            assertTrue(" run_4021() FAILED!!! ", false);
        }
    }// end run_4021()

    //
    //
    //
    //
    private void run_4030()
    {
        FileConnection conn1 = null;
        int counter = 0;
        try
        {
            // Preconditions for this testcase:
            // 1. Add following line into the \epoc32\data\epoc.ini file:
            // _EPOC_DRIVE_E \epoc32\wins\e
            // 2. Install this midlet to the memory card.
            // 3. Remove 'scratch' directory from Midlet's directory if it
            // exists.
            // NOTE: Other test cases are not allowed to execute with this
            // testcase.
            // 'scratch' directory is not allowed to exist when this testcase
            // is executed. This is a reason for this restriction.

            // This test verifies that 'scratch' directory is created by
            // setFileConnection() operation if it doesn't already exist.

            conn1 = (FileConnection) Connector
                    .open("file:///e:/Private/102033E6/MIDlets/");
            counter = 1;
            String url = System.getProperty("fileconn.dir.private");
            int startIndex = url.indexOf('[');
            int lastIndex = url.lastIndexOf(']');
            String uidDirStr = url.substring(startIndex, lastIndex + 1) + "/";
            conn1.setFileConnection(uidDirStr);
            conn1.close();

            counter = 30;
            // Try to make a connection to the file in the restricted directory.
            uidDirStr = url.substring(0, lastIndex + 1);
            String dumpStr = url.substring(0, lastIndex + 1) + "/uids";
            try
            {
                conn1 = (FileConnection) Connector.open(dumpStr);
                throw new TestFailedException();
            }
            catch (SecurityException ex)
            {
            }
            counter = 31;
            conn1 = (FileConnection) Connector.open(uidDirStr);
            counter = 32;
            try
            {
                conn1.setFileConnection("uids");
                throw new TestFailedException();
            }
            catch (SecurityException ex)
            {
            }
            conn1.close();
            counter = 33;

            // Try to create a new directory to the restricted directory.
            dumpStr = url.substring(0, lastIndex + 1) + "/dumpDir/";
            try
            {
                conn1 = (FileConnection) Connector.open(dumpStr);
                throw new TestFailedException();
            }
            catch (SecurityException ex)
            {
            }
            counter = 34;
            conn1 = (FileConnection) Connector.open(uidDirStr);
            counter = 35;
            try
            {
                conn1.setFileConnection("dumpDir/");
                throw new TestFailedException();
            }
            catch (SecurityException ex)
            {
            }
            conn1.close();
            counter = 36;

            assertTrue(" run_4030() ok ", true);
        }
        catch (Exception ex)
        {
            append(ex.toString());
            append("TEST FAILED!!!: run_4030(): counter: " + counter + "\n");
            assertTrue(" run_4030() FAILED!!! ", false);
        }
    }// end run_4030()

    private void checkSecurityException(String aUri) throws IOException
    {
        try
        {
            FileConnection conn1 = (FileConnection) Connector.open(aUri);
            throw new TestFailedException();
        }
        catch (SecurityException ex)
        {
        }
    }

    //
    //
    //
    //
    private void run_4040()
    {
        FileConnection conn1 = null;
        int counter = 0;
        try
        {
            // Preconditions for this testcase:
            // 1. Add following line into the \epoc32\data\epoc.ini file:
            // _EPOC_DRIVE_E \epoc32\wins\e
            // 2. Install another midlet to the memory card.
            // 3. Install this testdriver midlet to the phone memory.
            // 4. Remove 'scratch' directory from Midlet's directory if it
            // exists.

            // Creating a file to the e:/ root.
            conn1 = (FileConnection) Connector
                    .open("file:///e:/newFile404.txt");
            counter = 3;
            if (conn1.exists())
                throw new TestFailedException();
            conn1.create();
            counter = 4;
            conn1.close();

            conn1 = (FileConnection) Connector.open("file:///e:/");
            if (!conn1.exists())
                throw new TestFailedException();
            counter = 6;
            Enumeration dirList = conn1.list("*", true);
            if (false == dirList.hasMoreElements())
                throw new TestFailedException();
            counter = 7;
            int elementCounter = 0;
            boolean failedFlag = false;
            String fileName = null;
            while (dirList.hasMoreElements())
            {
                fileName = (String) dirList.nextElement();
                fileName = fileName.toLowerCase();
                if (fileName.equals("newfile404.txt"))
                    ++elementCounter;
                if (fileName.equals("private/"))
                    throw new TestFailedException(
                        "Private/ directory shouldn't been returned");
            }// end while
            counter = 8;
            if (elementCounter != 1)
                throw new TestFailedException("elementCounter: "
                                              + elementCounter);
            counter = 9;
            conn1.close();

            conn1 = (FileConnection) Connector.open("file:///Memory card/");
            counter = 110;
            dirList = conn1.list("*", true);
            if (false == dirList.hasMoreElements())
                throw new TestFailedException();
            elementCounter = 0;
            while (dirList.hasMoreElements())
            {
                fileName = (String) dirList.nextElement();
                fileName = fileName.toLowerCase();
                if (fileName.equals("newfile404.txt"))
                    ++elementCounter;
                if (fileName.equals("private/"))
                    throw new TestFailedException(
                        "system/ directory shouldn't been returned");
            }// end while
            counter = 111;
            if (elementCounter != 1)
                throw new TestFailedException("elementCounter: "
                                              + elementCounter);
            counter = 112;
            // Deleting a 'newFile404.txt' file.
            conn1.setFileConnection("newFile404.txt");
            counter = 113;
            if (!conn1.exists())
                throw new TestFailedException();
            conn1.delete();
            counter = 114;
            conn1.close();

            checkSecurityException("file:///e:/private/");
            counter = 12;
            checkSecurityException("file:///e:/private/102033E6/");
            counter = 13;
            checkSecurityException("file:///e:/private/102033E6/MIDlets/");
            counter = 130;
            checkSecurityException("file:///e:/private/newSysteFile.txt");
            counter = 14;
            checkSecurityException("file:///Memory card/private/");
            counter = 15;
            checkSecurityException("file:///Memory card/private/MIDlets/");
            counter = 16;
            checkSecurityException("file:///Memory card/private/newSysteFile.txt");
            counter = 17;

            assertTrue(" run_4040() ok ", true);
        }
        catch (Exception ex)
        {
            append(ex.toString());
            append("TEST FAILED!!!: run_4040(): counter: " + counter + "\n");
            assertTrue(" run_4040() FAILED!!! ", false);
        }
    }// end run_4040()

    //
    //
    //
    //
    private void run_4050()
    {
        // This test verifies that FileConnection object is valid after user
        // denied
        // access to the setFileConnection() operation call.
        // Midlet can be installed whether c: or e: drive for execution of this
        // test.
        // Access domain have to be 'Trusted thrid-party' for this test.
        // Execution of the test:
        // 1. Install midlet.
        // 2. Execute it.
        // - Answer to first security prompt(read access) 'Yes'.
        // - Answer to second security prompt(write access) 'Yes'.
        // - Answer to third security prompt(read access) 'No'.
        FileConnection conn1 = null;
        int counter = 0;
        try
        {
            conn1 = (FileConnection) Connector
                    .open("file:///c:/data/Videos/myDir/");
            counter = 1;
            try
            {
                conn1.setFileConnection("file");
                throw new TestFailedException();
            }
            catch (SecurityException ex)
            {
            }
            counter = 2;
            Enumeration dirList = conn1.list("*", true);
            counter = 3;
            if (false == dirList.hasMoreElements())
                throw new TestFailedException();
            counter = 4;
            int elementCounter = 0;
            boolean failedFlag = false;
            String fileName = null;
            while (dirList.hasMoreElements())
            {
                fileName = (String) dirList.nextElement();
                if (fileName.equals("file"))
                    ++elementCounter;
                else
                    failedFlag = true;
            }// end while
            counter = 5;
            if (elementCounter != 1)
                throw new TestFailedException("elementCounter: "
                                              + elementCounter);
            if (failedFlag == true)
                throw new TestFailedException();
            counter = 6;
            conn1.close();

            assertTrue(" run_4050() ok ", true);
        }
        catch (Exception ex)
        {
            append(ex.toString());
            append("TEST FAILED!!!: run_4050(): counter: " + counter + "\n");
            assertTrue(" run_4050() FAILED!!! ", false);
        }
    }// end run_4050()

    //
    //
    //
    //
    private void run_4060()
    {
        // This test verifies creation and deletion of the "property"
        // directories.
        // Midlet have to be installed to the e: drive in this test.
        // Preconditions for this testcase:
        // 1. Add following line into the \epoc32\data\epoc.ini file:
        // _EPOC_DRIVE_E \epoc32\wins\e
        // Execution of the test:
        // 1. Install midlet to the e: drive for execution of this test.
        // Access domain can be 'Operator' in this test.
        // 2. Execute test.
        FileConnection conn1 = null;
        int counter = 0;
        try
        {
            ExecuteCreateAndDeleteInPropertyDirTest();

            assertTrue(" run_4060() ok ", true);
        }
        catch (Exception ex)
        {
            append(ex.toString());
            append("TEST FAILED!!!: run_4060(): counter: " + counter + "\n");
            assertTrue(" run_4060() FAILED!!! ", false);
        }
    }// end run_4060()

    //
    //
    //
    //
    private void run_4070()
    {
        // This test verifies that only allowed roots are returned by
        // listRoots()
        // operation.
        // Preconditions for this testcase:
        // 1. Add following line _EPOC_LocDrv_1 E: to the epoc.ini file.
        // epoc.ini file locates on the /epoc32/data/ directory.
        // This demonstrate that memory card is in the emulator.
        // 2. Add following line _EPOC_Active_LocDrv_1 -1 to below
        // of the _EPOC_LocDrv_1 E: line in the epoc.ini file.
        // This line indicates emulator that memorycard doesn't exists when
        // device is started.
        // 3. Comment out following line _EPOC_DRIVE_E \epoc32\wins\e if it
        // exists in the epoc.ini file.
        // 4. Use Trusted Third Party access domain in this test.
        // 5. Execute test.
        FileConnection conn1 = null;
        int counter = 0;
        try
        {
            Enumeration roots = FileSystemRegistry.listRoots();
            counter = 1;
            if (false == roots.hasMoreElements())
                throw new TestFailedException();
            counter = 2;
            int counterRoot = 0;
            boolean failedFlag = false;
            while (roots.hasMoreElements())
            {
                String obj = (String) roots.nextElement();
                // append("obj: " + obj);
                if (obj.equals("C:/"))
                {
                    ++counterRoot;
                    continue;
                }
                else if (obj.equals("Phone memory/"))
                {
                    ++counterRoot;
                    continue;
                }
                else
                    failedFlag = true;
            }// end while
            counter = 3;
            if (2 != counterRoot)
                throw new TestFailedException("counterRoot: " + counterRoot);
            if (true == failedFlag)
                throw new TestFailedException();
            counter = 4;

            MyFileSystemListener listener1 = new MyFileSystemListener();
            listener1.setState(-1000);
            counter = 100;
            if (!FileSystemRegistry.addFileSystemListener(listener1))
                throw new TestFailedException();
            counter = 101;
            append("Add memorycard.\n");
            // Thread.sleep(5000);
            while (listener1.getState() == -1000)
            {
                try
                {
                    Thread.sleep(100);
                }
                catch (InterruptedException e)
                {
                }
            }// end while()
            counter = 102;
            // append("4");
            if (listener1.getState() != FileSystemListener.ROOT_ADDED)
                throw new TestFailedException("listener1.getState(): "
                                              + listener1.getState());
            counter = 103;
            if (!FileSystemRegistry.removeFileSystemListener(listener1))
                throw new TestFailedException();
            counter = 104;

            roots = FileSystemRegistry.listRoots();
            counter = 5;
            if (false == roots.hasMoreElements())
                throw new TestFailedException();
            counter = 6;
            counterRoot = 0;
            failedFlag = false;
            while (roots.hasMoreElements())
            {
                String obj = (String) roots.nextElement();
                append("obj: " + obj);
                if (obj.equals("C:/"))
                {
                    ++counterRoot;
                    continue;
                }
                else if (obj.equals("Phone memory/"))
                {
                    ++counterRoot;
                    continue;
                }
                else if (obj.equals("E:/"))
                {
                    ++counterRoot;
                    continue;
                }
                else if (obj.equals("Memory card/"))
                {
                    ++counterRoot;
                    continue;
                }
                else
                    failedFlag = true;
            }// end while
            counter = 7;
            if (4 != counterRoot)
                throw new TestFailedException("counterRoot: " + counterRoot);
            if (true == failedFlag)
                throw new TestFailedException();
            counter = 8;

            append("Do not grant access for listRoots() operation call. \n");
            Thread.sleep(3000);
            try
            {
                roots = FileSystemRegistry.listRoots();
                counterRoot = 0;
                while (roots.hasMoreElements())
                    ++counterRoot;
                throw new TestFailedException("counterRoot: " + counterRoot);
            }
            catch (SecurityException ex)
            {
            }

            assertTrue(" run_4070() ok ", true);
        }
        catch (Exception ex)
        {
            append(ex.toString());
            append("TEST FAILED!!!: run_4070(): counter: " + counter + "\n");
            assertTrue(" run_4070() FAILED!!! ", false);
        }
    }// end run_4070()

}