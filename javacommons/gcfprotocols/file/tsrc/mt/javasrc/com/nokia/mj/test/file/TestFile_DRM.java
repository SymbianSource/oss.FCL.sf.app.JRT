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

import com.nokia.mj.test.file.utils.TestFailedException;
import com.nokia.mj.test.file.utils.FileTestUtils;
import com.nokia.mj.test.file.utils.FileTestBase;
import com.nokia.mj.impl.installer.utils.InstallerMain;

/**
 * Not to be run currently. Implementation for DRM file access is currently not
 * done. So all tests will fail.
 *
 */
public class TestFile_DRM extends FileTestBase
{
    public TestFile_DRM()
    {
    }

    public TestFile_DRM(String aTestName, TestMethod aTestMethod)
    {
        super(aTestName, aTestMethod);
    }

    public void assertFalse(String aMsg, boolean aCondition)
    {
        assertTrue(aMsg, !aCondition);
    }

    public static void registerTests(TestSuite suite)
    {
        suite.addTest(new TestFile_DRM("run_0090", new TestMethod()
        {
            public void run(TestCase tc)
            {
                ((TestFile_DRM) tc).run_0090();
            }
        }));

        suite.addTest(new TestFile_DRM("run_0091", new TestMethod()
        {
            public void run(TestCase tc)
            {
                ((TestFile_DRM) tc).run_0091();
            }
        }));

        suite.addTest(new TestFile_DRM("run_1131", new TestMethod()
        {
            public void run(TestCase tc)
            {
                ((TestFile_DRM) tc).run_1131();
            }
        }));

        suite.addTest(new TestFile_DRM("run_0271", new TestMethod()
        {
            public void run(TestCase tc)
            {
                ((TestFile_DRM) tc).run_0271();
            }
        }));

        suite.addTest(new TestFile_DRM("run_0441", new TestMethod()
        {
            public void run(TestCase tc)
            {
                ((TestFile_DRM) tc).run_0441();
            }
        }));

        suite.addTest(new TestFile_DRM("run_1321", new TestMethod()
        {
            public void run(TestCase tc)
            {
                ((TestFile_DRM) tc).run_1321();
            }
        }));

        suite.addTest(new TestFile_DRM("run_1661", new TestMethod()
        {
            public void run(TestCase tc)
            {
                ((TestFile_DRM) tc).run_1661();
            }
        }));

        suite.addTest(new TestFile_DRM("run_1421", new TestMethod()
        {
            public void run(TestCase tc)
            {
                ((TestFile_DRM) tc).run_1421();
            }
        }));

        suite.addTest(new TestFile_DRM("run_1851", new TestMethod()
        {
            public void run(TestCase tc)
            {
                ((TestFile_DRM) tc).run_1851();
            }
        }));

        suite.addTest(new TestFile_DRM("run_1741", new TestMethod()
        {
            public void run(TestCase tc)
            {
                ((TestFile_DRM) tc).run_1741();
            }
        }));

        suite.addTest(new TestFile_DRM("run_2032", new TestMethod()
        {
            public void run(TestCase tc)
            {
                ((TestFile_DRM) tc).run_2032();
            }
        }));

        suite.addTest(new TestFile_DRM("run_2221", new TestMethod()
        {
            public void run(TestCase tc)
            {
                ((TestFile_DRM) tc).run_2221();
            }
        }));

        suite.addTest(new TestFile_DRM("run_2222", new TestMethod()
        {
            public void run(TestCase tc)
            {
                ((TestFile_DRM) tc).run_2222();
            }
        }));

        suite.addTest(new TestFile_DRM("run_2451", new TestMethod()
        {
            public void run(TestCase tc)
            {
                ((TestFile_DRM) tc).run_2451();
            }
        }));

        suite.addTest(new TestFile_DRM("run_2574", new TestMethod()
        {
            public void run(TestCase tc)
            {
                ((TestFile_DRM) tc).run_2574();
            }
        }));

        suite.addTest(new TestFile_DRM("run_2651", new TestMethod()
        {
            public void run(TestCase tc)
            {
                ((TestFile_DRM) tc).run_2651();
            }
        }));

        suite.addTest(new TestFile_DRM("run_2793", new TestMethod()
        {
            public void run(TestCase tc)
            {
                ((TestFile_DRM) tc).run_2793();
            }
        }));

        suite.addTest(new TestFile_DRM("run_2801", new TestMethod()
        {
            public void run(TestCase tc)
            {
                ((TestFile_DRM) tc).run_2801();
            }
        }));

        suite.addTest(new TestFile_DRM("run_2931", new TestMethod()
        {
            public void run(TestCase tc)
            {
                ((TestFile_DRM) tc).run_2931();
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

    private void readingFileTest(String aUri)
    {
        int counter = 2000;
        try
        {
            FileConnection conn1 = (FileConnection) Connector.open(aUri,
                                   Connector.READ);
            InputStream stream = conn1.openInputStream();
            byte[] byteArray1 = new byte[1200];
            int retValue = stream.read(byteArray1);
            counter = 2001;
            if (retValue != 1025)
                throw new TestFailedException("retValue: " + retValue);
            counter = 2002;
            for (int i = 0; i < retValue; ++i)
            {
                if (byteArray1[i] != 56)
                    throw new TestFailedException();
            }
            counter = 2003;
            stream.close();
            conn1.close();
        }
        catch (Exception ex)
        {
            throw new TestFailedException("counter: " + counter + "\n"
                                          + ex.toString());
        }
    }

    private void throwSecurityException(String aUri, int aMode)
    throws IOException
    {
        try
        {
            FileConnection conn1 = (FileConnection) Connector.open(aUri, aMode);
            throw new TestFailedException();
        }
        catch (SecurityException ex)
        {
        }
    }

    private void throwIllegalArgumentException(String aUri, int aMode)
    throws IOException
    {
        try
        {
            FileConnection conn1 = (FileConnection) Connector.open(aUri, aMode);
            throw new TestFailedException();
        }
        catch (IllegalArgumentException ex)
        {
        }
    }

    private void testCounterDecreasedCase(String aUri)
    {
        int counter = 0;
        try
        {
            FileConnection conn1 = (FileConnection) Connector.open(aUri,
                                   Connector.READ);
            InputStream stream = conn1.openInputStream();
            byte[] byteArray1 = new byte[3000];
            int retValue = stream.read(byteArray1);
            counter = 1;
            if (retValue != 1025)
                throw new TestFailedException("retValue: " + retValue);
            counter = 2;
            for (int i = 0; i < retValue; ++i)
            {
                if (byteArray1[i] != 56)
                    throw new TestFailedException();
            }
            stream.close();
            counter = 3;
            stream = conn1.openInputStream();
            counter = 4;
            byteArray1 = new byte[3000];
            retValue = stream.read(byteArray1);
            counter = 5;
            stream.close();
            try
            {
                stream = conn1.openInputStream();
                // stream.read(byteArray1);
                throw new TestFailedException();
            }
            catch (SecurityException ex)
            {
            }
            counter = 6;
            conn1.close();
        }
        catch (Exception ex)
        {
            throw new TestFailedException("counter: " + counter + "\n"
                                          + ex.toString());
        }
    }

    private void testPreviewCase(String aUri)
    {
        int counter = 0;
        try
        {
            FileConnection conn1 = (FileConnection) Connector.open(aUri,
                                   Connector.READ);
            InputStream stream = conn1.openInputStream();
            byte[] byteArray1 = new byte[3000];
            int retValue = stream.read(byteArray1);
            counter = 1;
            if (retValue != 1025)
                throw new TestFailedException("retValue: " + retValue);
            counter = 2;
            for (int i = 0; i < retValue; ++i)
            {
                if (byteArray1[i] != 56)
                    throw new TestFailedException();
            }
            stream.close();

            counter = 4;
            stream = conn1.openInputStream();
            counter = 5;
            stream.read(byteArray1);
            counter = 6;
            stream.close();
            counter = 7;

            counter = 8;
            stream = conn1.openInputStream();
            counter = 9;
            stream.read(byteArray1);
            counter = 10;
            stream.close();
            counter = 11;

            counter = 12;
            stream = conn1.openInputStream();
            counter = 13;
            stream.read(byteArray1);
            counter = 14;
            stream.close();
            counter = 15;

            conn1.close();
        }
        catch (Exception ex)
        {
            throw new TestFailedException("counter: " + counter + "\n"
                                          + ex.toString());
        }
    }

    private void testDecCaseWithUnprotectedFile(String aUri, int aMode)
    {
        int counter = 1000;
        try
        {
            FileConnection conn1 = (FileConnection) Connector.open(aUri, aMode);
            counter = 1001;
            InputStream stream = conn1.openInputStream();
            byte[] byteArray1 = new byte[1026];
            counter = 1002;
            int retValue = stream.read(byteArray1);
            counter = 1003;
            if (retValue != 1025)
                throw new TestFailedException("retValue: " + retValue);
            counter = 1004;
            for (int i = 0; i < retValue; ++i)
            {
                if (byteArray1[i] != 4)
                    throw new TestFailedException("byteArray1[i]: "
                                                  + byteArray1[i]);
            }
            counter = 1005;
            retValue = stream.read(byteArray1);
            counter = 1006;
            if (retValue != -1)
                throw new TestFailedException();
            counter = 1007;
            stream.close();
            conn1.close();
        }
        catch (Exception ex)
        {
            throw new TestFailedException("counter: " + counter + "\n"
                                          + ex.toString());
        }
    }

    private void run_0090()
    {
        FileConnection conn1 = null;
        int counter = 0;
        try
        {
            // 1
            testPreviewCase("file:///"
                            + iRoot
                            + "data/videos/drmFiles/drmfile001DisplayPreview.dcf?drm=dec+display+preview");
            counter = 1;
            testPreviewCase("file:///"
                            + iRoot
                            + "data/videos/drmFiles/drmfile001PlayPreview.dcf?drm=dec+play+preview");
            counter = 2;
            testPreviewCase("file:///"
                            + iRoot
                            + "data/videos/drmFiles/drmfile001PrintPreview.dcf?drm=dec+print+preview");
            counter = 3;
            testPreviewCase("file:///"
                            + iRoot
                            + "data/videos/drmFiles/drmfile001ExecutePreview.dcf?dRm=dEc+exEcute+preView ");
            counter = 4;

            // 2
            testCounterDecreasedCase("file:///"
                                     + iRoot
                                     + "data/videos/drmFiles/drmfile002DecDisplayCount2.dcf?drm=dec+display");
            counter = 5;
            testCounterDecreasedCase("file:///"
                                     + iRoot
                                     + "data/videos/drmFiles/drmfile002DecPlayCount2.dcf?drm=dec+play ");
            counter = 6;
            testCounterDecreasedCase("file:///"
                                     + iRoot
                                     + "data/videos/drmFiles/drmfile002DecPrintCount2.dcf?drm=dec+print");
            counter = 7;
            testCounterDecreasedCase("file:///"
                                     + iRoot
                                     + "data/videos/drmFiles/drmfile002DecExecuteCount2.dcf?drm=dec+execute");
            counter = 8;

            // 3
            conn1 = (FileConnection) Connector.open("file:///" + iRoot
                                                    + "mainTestDir/test020/binaryFile.dat?drm=preView ",
                                                    Connector.READ);
            InputStream stream = conn1.openInputStream();
            byte[] byteArray3 = new byte[1026];
            counter = 20;
            int retValue = stream.read(byteArray3);
            counter = 21;
            if (retValue != 1025)
                throw new TestFailedException("retValue: " + retValue);
            retValue = stream.read(byteArray3);
            counter = 22;
            if (retValue != -1)
                throw new TestFailedException();
            counter = 23;
            stream.close();
            conn1.close();

            // 4
            conn1 = (FileConnection) Connector.open("file:///" + iRoot
                                                    + "data/videos/drmFiles/drmfile002EncCount1.dcf?drm=eNc",
                                                    Connector.READ);
            counter = 30;
            stream = conn1.openInputStream();
            counter = 31;
            byte[] byteArray1 = new byte[1400];
            retValue = stream.read(byteArray1);
            counter = 32;
            if (retValue != 1350)
                throw new TestFailedException("retValue: " + retValue);
            counter = 33;
            stream.close();
            counter = 34;
            stream = conn1.openInputStream();
            stream.close();
            conn1.close();

            // 5
            conn1 = (FileConnection) Connector.open(
                        "file:///Memory card/drmfile003DecPlayCount1?drm=dec+play",
                        Connector.READ);
            counter = 40;
            stream = conn1.openInputStream();
            counter = 41;
            byteArray1 = new byte[1300];
            retValue = stream.read(byteArray1);
            counter = 42;
            if (retValue != 1025)
                throw new TestFailedException();
            counter = 43;
            for (int i = 0; i < retValue; ++i)
            {
                if (byteArray1[i] != 56)
                    throw new TestFailedException();
            }
            stream.close();
            counter = 44;
            try
            {
                stream = conn1.openInputStream();
                stream.close();
                throw new TestFailedException();
            }
            catch (SecurityException ex)
            {
            }
            conn1.close();

            // 6
            conn1 = (FileConnection) Connector
                    .open(
                        "file:///Internal/Videos/drmFiles/drmfile004DecPlayCount1.dcf?drm=dec+pLay",
                        Connector.READ);
            counter = 50;
            stream = conn1.openInputStream();
            counter = 51;
            byteArray1 = new byte[1026];
            retValue = stream.read(byteArray1);
            counter = 52;
            if (retValue != 1025)
                throw new TestFailedException();
            counter = 53;
            for (int i = 0; i < retValue; ++i)
            {
                if (byteArray1[i] != 56)
                    throw new TestFailedException();
            }
            stream.close();
            counter = 54;
            try
            {
                stream = conn1.openInputStream();
                stream.close();
                throw new TestFailedException();
            }
            catch (SecurityException ex)
            {
            }
            conn1.close();

            // 7
            counter = 70;
            testDecCaseWithUnprotectedFile("file:///" + iRoot
                                           + "mainTestDir/test020/binaryFile.dat?drm=dec+play",
                                           Connector.READ);
            counter = 71;
            testDecCaseWithUnprotectedFile(
                "file:///"
                + iRoot
                + "mainTestDir/test020/binaryFile.dat?drm=dec+play+preview",
                Connector.READ);
            counter = 72;
            testDecCaseWithUnprotectedFile("file:///" + iRoot
                                           + "mainTestDir/test020/binaryFile.dat?drm=dec+display",
                                           Connector.READ);
            counter = 73;
            testDecCaseWithUnprotectedFile(
                "file:///"
                + iRoot
                + "mainTestDir/test020/binaryFile.dat?drm=dec+display+preview",
                Connector.READ);
            counter = 74;
            testDecCaseWithUnprotectedFile("file:///" + iRoot
                                           + "mainTestDir/test020/binaryFile.dat?drm=dec+execute",
                                           Connector.READ);
            counter = 75;
            testDecCaseWithUnprotectedFile(
                "file:///"
                + iRoot
                + "mainTestDir/test020/binaryFile.dat?drm=dec+execute+preview",
                Connector.READ);
            counter = 76;
            testDecCaseWithUnprotectedFile("file:///" + iRoot
                                           + "mainTestDir/test020/binaryFile.dat?drm=dec+print",
                                           Connector.READ);
            counter = 77;
            testDecCaseWithUnprotectedFile(
                "file:///"
                + iRoot
                + "mainTestDir/test020/binaryFile.dat?drm=dec+print+preview",
                Connector.READ);
            counter = 78;
            testDecCaseWithUnprotectedFile("file:///" + iRoot
                                           + "mainTestDir/test020/binaryFile.dat?drm=preview",
                                           Connector.READ_WRITE);
            counter = 79;
            // 8
            // %2B => +
            testCounterDecreasedCase("file:///"
                                     + iRoot
                                     + "data/videos/drmFiles/drmfile003_01DecPlayCount2.dcf?drm=dec%2Bplay");
            counter = 80;
            // 9
            testCounterDecreasedCase("file:///Internal/Videos/drmFiles/drmfile003_02DecPlayCount2.dcf?drm=dec%2Bplay");
            counter = 9;
            // 10
            testCounterDecreasedCase("file:///Internal/Videos/drmFiles/drmfile003_03DecPlayCount2.dcf?drm=dec%2Bplay;foo=bar");
            counter = 10;
            // 11
            readingFileTest("file:///Internal/Videos/drmFiles/drmfile003_04DecPlayCount50.dcf?foo=bar;drm=dec%2Bplay");
            counter = 11;
            // 12
            readingFileTest("file:///Internal/Videos/drmFiles/drmfile003_04DecPlayCount50.dcf?foo=bar;drm=dec%2Bplay;foo=bar");
            counter = 12;
            // 13
            readingFileTest("file:///Internal/Videos/drmFiles/drmfile003_04DecPlayCount50.dcf?foo=bar;drm=dec%2Bplay+preview");
            counter = 13;
            // 14
            readingFileTest("file:///Internal/Videos/drmFiles/drmfile003_04DecPlayCount50.dcf?foo=bar;drm=dec%2Bplay+preview;foo=bar");
            counter = 14;
            // 15
            // ';' character in the end of the uri is point of this test.
            readingFileTest("file:///Internal/Videos/drmFiles/drmfile003_04DecPlayCount50.dcf?foo=bar;drm=dec%2Bplay+preview;");
            counter = 15;
            // 16
            readingFileTest("file:///Internal/Videos/drmFiles/drmfile003_04DecPlayCount50.dcf? foo =b ar; drm=dec%2Bplay+preview; foo=bar");
            counter = 16;
            // 17
            readingFileTest("file:///Memory card/drmfile003_05DecPlayCount50.dcf? foo =b ar; drm=dec%2Bplay+preview; foo=bar");
            counter = 17;
            // 18
            readingFileTest("file:///Memory card/drmfile003_05DecPlayCount50.dcf? foo =b ar; drm=dec%2Bplay; foo=bar");
            counter = 18;
            // 19
            readingFileTest("file:///e:/drmfile003_05DecPlayCount50.dcf? foo =b ar; drm=dec%2Bplay+preview; foo=bar");
            counter = 19;
            // 20
            readingFileTest("file:///e:/drmfile003_05DecPlayCount50.dcf? foo =b ar; drm=dec%2Bplay; foo=bar");
            counter = 20;

            assertTrue(" run_0090() ok ", true);
        }
        catch (Exception ex)
        {
            append(ex.toString());
            append("TEST FAILED!!!: run_0090(): counter: " + counter + "\n");
            assertTrue(" run_0090() FAILED!!! Counter: " + counter, false);
            return;
        }
    }// end run_0090()

    private void run_0091()
    {
        FileConnection conn1 = null;
        int counter = 0;
        try
        {
            // 1
            throwSecurityException(
                "file:///"
                + iRoot
                + "data/videos/drmFiles/drmfile005DecPlayCount20.dcf?drm=dec+play",
                Connector.WRITE);
            counter = 1;
            // 2
            throwSecurityException(
                "file:///"
                + iRoot
                + "data/videos/drmFiles/drmfile005DecPlayCount20.dcf?drm=dec+play",
                Connector.READ_WRITE);
            // 3
            throwSecurityException(
                "file:///"
                + iRoot
                + "data/videos/drmFiles/drmfile005DecPlayCount20.dcf?drm=dec+play+preview",
                Connector.WRITE);
            counter = 3;
            // 4
            throwSecurityException(
                "file:///"
                + iRoot
                + "data/videos/drmFiles/drmfile005DecPlayCount20.dcf?drm=dec+play+preview",
                Connector.READ_WRITE);
            counter = 4;
            // 5
            throwSecurityException(
                "file:///"
                + iRoot
                + "data/videos/drmFiles/drmfile005DecPlayCount20.dcf?drm=enc",
                Connector.WRITE);
            counter = 5;
            // 6
            throwSecurityException(
                "file:///"
                + iRoot
                + "data/videos/drmFiles/drmfile005DecPlayCount20.dcf?drm=enc",
                Connector.READ_WRITE);
            counter = 6;
            // 7
            throwSecurityException(
                "file:///"
                + iRoot
                + "data/videos/drmFiles/drmfile005DecPlayCount20.dcf?drm=preview",
                Connector.WRITE);
            counter = 7;
            // 8
            throwSecurityException(
                "file:///"
                + iRoot
                + "data/videos/drmFiles/drmfile005DecPlayCount20.dcf?drm=preview",
                Connector.READ_WRITE);
            counter = 8;
            // 9
            throwSecurityException(
                "file:///"
                + iRoot
                + "data/videos/drmFiles/drmfile005DecPlayCount20.dcf?drm=dec+play",
                Connector.WRITE);
            counter = 9;
            // 10
            throwSecurityException(
                "file:///"
                + iRoot
                + "data/videos/drmFiles/drmfile005DecPlayCount20.dcf?drm=dec+play",
                Connector.READ_WRITE);
            counter = 10;
            // 11
            throwSecurityException("file:///" + iRoot
                                   + "mainTestDir/test020/binaryFile.dat?drm=dec+play",
                                   Connector.WRITE);
            counter = 11;
            // 12
            throwSecurityException(
                "file:///"
                + iRoot
                + "mainTestDir/test020/binaryFile.dat?drm=dec+play+preview",
                Connector.WRITE);
            counter = 12;
            // 13
            throwSecurityException("file:///" + iRoot
                                   + "mainTestDir/test020/binaryFile.dat?drm=enc",
                                   Connector.READ_WRITE);
            counter = 13;
            // 14
            throwSecurityException("file:///" + iRoot
                                   + "mainTestDir/test020/binaryFile.dat?drm=enc",
                                   Connector.WRITE);
            counter = 14;
            // 15
            throwIllegalArgumentException(
                "file:///"
                + iRoot
                + "data/videos/drmFiles/drmfile006DecPlayCount1.dcf?drm=dec+play;drm=dec+preview",
                Connector.READ);
            counter = 15;
            // 16
            throwIllegalArgumentException(
                "file:///"
                + iRoot
                + "data/videos/drmFiles/drmfile006DecPlayCount1.dcf?drm=dec+play;drm=preview",
                Connector.READ);
            counter = 16;
            // 17
            throwIllegalArgumentException(
                "file:///"
                + iRoot
                + "data/videos/drmFiles/drmfile006DecPlayCount1.dcf?drm=enc;drm=preview",
                Connector.READ);
            counter = 17;
            // 18
            throwSecurityException("file:///" + iRoot
                                   + "mainTestDir/test020/binaryFile.dat?drm=enc",
                                   Connector.READ);
            counter = 18;
            // 19
            throwIllegalArgumentException(
                "file:///"
                + iRoot
                + "data/videos/drmFiles/drmfile006DecPlayCount1.dcf?drm=play+display+preview",
                Connector.READ);
            counter = 19;
            // 20
            throwIllegalArgumentException(
                "file:///"
                + iRoot
                + "data/videos/drmFiles/drmfile006DecPlayCount1.dcf?drm=play+preview+display",
                Connector.READ);
            counter = 20;
            // 21
            throwIllegalArgumentException(
                "file:///"
                + iRoot
                + "data/videos/drmFiles/drmfile006DecPlayCount1.dcf?drm=preview+play",
                Connector.READ);
            counter = 21;
            // 25
            throwIllegalArgumentException(
                "file:///"
                + iRoot
                + "data/videos/drmFiles/drmfile006DecPlayCount1.dcf?drm=dec+preview+play",
                Connector.READ);
            counter = 25;
            // 26
            throwSecurityException(
                "file:///"
                + iRoot
                + "data/videos/drmFiles/drmfile006DecPlayCount1.dcf?drm=dec+print+preview",
                Connector.READ);
            counter = 26;
            // 27
            throwSecurityException(
                "file:///"
                + iRoot
                + "data/videos/drmFiles/drmfile006DecPlayCount1.dcf?drm=dec+print",
                Connector.READ);
            counter = 27;
            // 28
            throwSecurityException(
                "file:///"
                + iRoot
                + "mainTestDir/test020/binaryFile.dat?drm=dec+play+preview",
                Connector.READ_WRITE);
            counter = 28;
            // 29
            throwIllegalArgumentException(
                "file:///"
                + iRoot
                + "data/videos/drmFiles/drmfile006DecPlayCount1.dcf?drm=enc+preview",
                Connector.READ);
            counter = 29;
            // 30
            throwIllegalArgumentException(
                "file:///"
                + iRoot
                + "data/videos/drmFiles/drmfile006DecPlayCount1.dcf?drm=dec+blaa+preview",
                Connector.READ);
            counter = 30;
            // 31
            throwIllegalArgumentException("file:///" + iRoot
                                          + "mainTestDir/test020/binaryFile.dat?drm=dec+preview",
                                          Connector.READ_WRITE);
            counter = 31;
            // 32
            try
            {
                conn1 = (FileConnection) Connector
                        .open("file:///"
                              + iRoot
                              + "data/videos/drmFiles/drmfile005DecPlayCount20.dcf?drm=dec+play");
                throw new TestFailedException();
            }
            catch (SecurityException ex)
            {
            }
            counter = 32;
            // 33
            throwIllegalArgumentException(
                "file:///"
                + iRoot
                + "data/videos/drmFiles/drmfile006DecPlayCount1.dcf?drm=dec+play+execute",
                Connector.READ);
            counter = 33;
            // 34
            throwIllegalArgumentException(
                "file:///"
                + iRoot
                + "data/videos/drmFiles/drmfile006DecPlayCount1.dcf?drm=dec+play + execute",
                Connector.READ);
            counter = 34;
            // 35
            throwIllegalArgumentException(
                "file:///"
                + iRoot
                + "data/videos/drmFiles/drmfile006DecPlayCount1.dcf?drm=dec+play + preview",
                Connector.READ);
            counter = 35;
            // 36
            throwSecurityException(
                "file:///"
                + iRoot
                + "data/videos/drmFiles/drmfile005DecPlayCount20.dcf?drm=preview",
                Connector.READ);
            counter = 36;
            // 37
            throwSecurityException("file:///" + iRoot
                                   + "data/videos/drmFiles/drmfile005DecPlayCount20.dcf",
                                   Connector.READ);
            counter = 37;

            assertTrue(" run_0091() ok ", true);
        }
        catch (Exception ex)
        {
            append(ex.toString());
            append("TEST FAILED!!!: run_0091(): counter: " + counter + "\n");
            assertTrue(" run_0091() FAILED!!! Counter: " + counter, false);
            return;
        }
    }// end run_0091()

    private void run_1131()
    {
        FileConnection conn1 = null;
        FileConnection conn2 = null;
        int counter = 0;
        try
        {
            // 1
            conn1 = (FileConnection) Connector
                    .open(
                        "file:///"
                        + iRoot
                        + "data/videos/drmFiles/drmfile011DecPlayCount4.dcf?drm=dec+play",
                        Connector.READ);
            counter = 1;
            if (1330 != conn1.fileSize())
                throw new TestFailedException("file size: " + conn1.fileSize());
            counter = 2;
            conn1.close();

            // 2
            counter = 10;
            conn1 = (FileConnection) Connector
                    .open(
                        "file:///"
                        + iRoot
                        + "data/videos/drmFiles/drmfile011DecPlayCount4.dcf?drm=dec+play",
                        Connector.READ);
            counter = 11;
            InputStream in1 = conn1.openInputStream();
            counter = 12;
            conn2 = (FileConnection) Connector
                    .open(
                        "file:///"
                        + iRoot
                        + "data/videos/drmFiles/drmfile011DecPlayCount4.dcf?drm=dec+play",
                        Connector.READ);
            counter = 13;
            InputStream in2 = conn2.openInputStream();
            counter = 14;
            if (1330 != conn1.fileSize())
                throw new TestFailedException("file size: " + conn1.fileSize());
            counter = 15;
            if (1330 != conn2.fileSize())
                throw new TestFailedException("file size: " + conn1.fileSize());
            counter = 16;
            in1.close();
            in2.close();
            conn1.close();
            conn2.close();

            assertTrue(" run_1131() ok ", true);
        }
        catch (Exception ex)
        {
            append(ex.toString());
            append("TEST FAILED!!! Case: run_1131() counter: " + counter + "\n");
            assertTrue(" run_1131() FAILED!!! Counter: " + counter, false);
        }
    }// end run_1131()

    private void run_0271()
    {
        FileConnection conn1 = null;
        FileConnection conn2 = null;
        int counter = 0;
        InputStream in1 = null;
        InputStream in2 = null;
        try
        {
            // 1
            conn1 = (FileConnection) Connector
                    .open(
                        "file:///"
                        + iRoot
                        + "data/videos/drmFiles/drmfile007DecPlayCount2.dcf?drm=dec+play",
                        Connector.READ);
            counter = 1;
            System.out.println(" run_0271(): Counter = " + counter);
            conn2 = (FileConnection) Connector
                    .open(
                        "file:///"
                        + iRoot
                        + "data/videos/drmFiles/drmfile007DecPlayCount2.dcf?drm=dec+play",
                        Connector.READ);
            counter = 2;
            System.out.println(" run_0271(): Counter = " + counter);
            in1 = conn1.openInputStream();
            counter = 3;
            System.out.println(" run_0271(): Counter = " + counter);
            in2 = conn2.openInputStream();
            counter = 4;
            System.out.println(" run_0271(): Counter = " + counter);
            byte[] byteArray1 = new byte[1020];
            int retValue = in1.read(byteArray1);
            counter = 5;
            System.out.println(" run_0271(): Counter = " + counter
                               + " RetVal = " + retValue);
            if (retValue != 1020)
                throw new TestFailedException();
            counter = 6;
            System.out.println(" run_0271(): Counter = " + counter);
            for (int i = 0; i < retValue; ++i)
            {
                if (byteArray1[i] != 56)
                    throw new TestFailedException();
            }
            counter = 7;
            System.out.println(" run_0271(): Counter = " + counter);
            byteArray1 = new byte[5];
            retValue = in2.read(byteArray1);
            counter = 8;
            System.out.println(" run_0271(): Counter = " + counter);
            if (retValue != 5)
                throw new TestFailedException();
            counter = 9;
            System.out.println(" run_0271(): Counter = " + counter);
            for (int i = 0; i < retValue; ++i)
            {
                if (byteArray1[i] != 56)
                    throw new TestFailedException();
            }
            counter = 10;
            System.out.println(" run_0271(): Counter = " + counter);
            in1.close();
            in2.close();
            conn1.close();
            conn2.close();
            counter = 11;
            System.out.println(" run_0271(): Counter = " + counter);
            try
            {
                conn1 = (FileConnection) Connector
                        .open(
                            "file:///"
                            + iRoot
                            + "data/videos/drmFiles/drmfile007DecPlayCount2.dcf?drm=dec+play",
                            Connector.READ);
                in1.close();
                throw new TestFailedException();
            }
            catch (SecurityException ex)
            {
            }
            counter = 13;
            conn1.close();

            // 2
            conn1 = (FileConnection) Connector
                    .open(
                        "file:///"
                        + iRoot
                        + "data/videos/drmFiles/drmfile008DecPlayCount2.dcf?drm=dec+play",
                        Connector.READ);
            counter = 14;
            conn2 = (FileConnection) Connector
                    .open(
                        "file:///"
                        + iRoot
                        + "data/videos/drmFiles/drmfile008DecPlayCount2.dcf?drm=dec+play+preview",
                        Connector.READ);
            counter = 15;
            // Counter value is 1 after openInputStream() operation call.
            in1 = conn1.openInputStream();
            counter = 16;
            in2 = conn2.openInputStream();
            counter = 17;
            byteArray1 = new byte[1030];
            retValue = in1.read(byteArray1);
            counter = 18;
            if (retValue != 1025)
                throw new TestFailedException();
            counter = 19;
            for (int i = 0; i < retValue; ++i)
            {
                if (byteArray1[i] != 56)
                    throw new TestFailedException();
            }
            counter = 20;
            byteArray1 = new byte[1030];
            retValue = in2.read(byteArray1);
            counter = 21;
            if (retValue != 1025)
                throw new TestFailedException("retValue: " + retValue);
            counter = 22;
            for (int i = 0; i < retValue; ++i)
            {
                if (byteArray1[i] != 56)
                    throw new TestFailedException();
            }
            counter = 23;
            in1.close();
            in2.close();
            conn1.close();
            conn2.close();
            counter = 24;
            conn1 = (FileConnection) Connector
                    .open(
                        "file:///"
                        + iRoot
                        + "data/videos/drmFiles/drmfile008DecPlayCount2.dcf?drm=dec+play+preview",
                        Connector.READ);
            counter = 25;
            in1 = conn1.openInputStream();
            counter = 26;
            in1.close();
            in1 = conn1.openInputStream();
            counter = 27;
            in1.close();
            conn1.close();
            counter = 28;
            conn1 = (FileConnection) Connector
                    .open(
                        "file:///"
                        + iRoot
                        + "data/videos/drmFiles/drmfile008DecPlayCount2.dcf?drm=dec+play",
                        Connector.READ);
            counter = 29;
            // Counter value is 0 after openInputStream() operation call.
            in1 = conn1.openInputStream();
            in1.close();
            counter = 30;
            try
            {
                in1 = conn1.openInputStream();
                in1.read(byteArray1);
                in1.close();
                throw new TestFailedException();
            }
            catch (SecurityException ex)
            {
            }
            counter = 31;
            conn1.close();

            // 3
            conn1 = (FileConnection) Connector
                    .open(
                        "file:///"
                        + iRoot
                        + "data/videos/drmFiles/drmfile009DecPlayCount1.dcf?drm=enc",
                        Connector.READ);
            counter = 40;
            conn2 = (FileConnection) Connector
                    .open(
                        "file:///"
                        + iRoot
                        + "data/videos/drmFiles/drmfile009DecPlayCount1.dcf?drm=enc",
                        Connector.READ);
            counter = 41;
            in1 = conn1.openInputStream();
            counter = 42;
            in2 = conn2.openInputStream();
            counter = 43;
            byteArray1 = new byte[1360];
            retValue = in1.read(byteArray1);
            counter = 44;
            if (retValue != 1354)
                throw new TestFailedException();
            counter = 45;
            byteArray1 = new byte[1360];
            retValue = in2.read(byteArray1);
            counter = 47;
            if (retValue != 1354)
                throw new TestFailedException();
            counter = 48;
            in1.close();
            in2.close();
            conn1.close();
            conn2.close();
            counter = 50;
            conn1 = (FileConnection) Connector
                    .open(
                        "file:///"
                        + iRoot
                        + "data/videos/drmFiles/drmfile009DecPlayCount1.dcf?drm=enc",
                        Connector.READ);
            counter = 51;
            in1 = conn1.openInputStream();
            counter = 52;
            in1.close();
            in1 = conn1.openInputStream();
            counter = 53;
            in1.close();
            conn1.close();
            counter = 54;
            conn1 = (FileConnection) Connector
                    .open(
                        "file:///"
                        + iRoot
                        + "data/videos/drmFiles/drmfile009DecPlayCount1.dcf?drm=dec+play",
                        Connector.READ);
            counter = 55;
            in1 = conn1.openInputStream();
            in1.close();
            counter = 56;
            try
            {
                in1 = conn1.openInputStream();
                in1.close();
                throw new TestFailedException();
            }
            catch (SecurityException ex)
            {
            }
            counter = 57;
            conn1.close();

            assertTrue(" run_0271() ok ", true);
        }
        catch (Exception ex)
        {
            append(ex.toString());
            append("TEST FAILED!!! Case: run_0271() counter: " + counter + "\n");
            assertTrue(" run_0271() FAILED!!! Counter: " + counter, false);
        }
    }// end run_0271()

    private void run_0441()
    {
        FileConnection conn1 = null;
        FileConnection conn2 = null;
        int counter = 0;
        try
        {
            // 1
            conn1 = (FileConnection) Connector
                    .open(
                        "file:///"
                        + iRoot
                        + "data/videos/drmFiles/drmfile010DecPlayCount2.dcf?drm=dec+play",
                        Connector.READ);
            counter = 1;
            InputStream in1 = conn1.openInputStream();
            counter = 2;
            conn2 = (FileConnection) Connector
                    .open(
                        "file:///"
                        + iRoot
                        + "data/videos/drmFiles/drmfile010DecPlayCount2.dcf?drm=dec+play",
                        Connector.READ_WRITE);
            counter = 3;
            try
            {
                OutputStream out1 = conn2.openOutputStream();
                throw new TestFailedException();
            }
            catch (IOException ex)
            {
            }
            in1.close();
            conn1.close();
            conn2.close();
            counter = 4;
            conn1 = (FileConnection) Connector
                    .open(
                        "file:///"
                        + iRoot
                        + "data/videos/drmFiles/drmfile010DecPlayCount2.dcf?drm=dec+play",
                        Connector.READ);
            counter = 5;
            in1 = conn1.openInputStream();
            in1.close();
            counter = 6;
            try
            {
                in1 = conn1.openInputStream();
                in1.close();
                throw new TestFailedException();
            }
            catch (SecurityException ex)
            {
            }
            conn1.close();

            assertTrue(" run_0441() ok ", true);
        }
        catch (Exception ex)
        {
            append(ex.toString());
            append("TEST FAILED!!! Case: run_0441() counter: " + counter + "\n");
            assertTrue(" run_0441() FAILED!!! Counter: " + counter, false);
        }
    }// end run_0441()

    private void run_1321()
    {
        FileConnection conn1 = null;
        FileConnection conn2 = null;
        int counter = 0;
        try
        {
            // 1
            conn1 = (FileConnection) Connector
                    .open(
                        "file:///"
                        + iRoot
                        + "data/Videos/drmFiles/drmfile012DecPlayCount4.dcf?drm=dec+play",
                        Connector.READ);
            counter = 1;
            if (false == conn1.canWrite())
                throw new TestFailedException();
            counter = 100;
            conn2 = (FileConnection) Connector
                    .open(
                        "file:///"
                        + iRoot
                        + "data/Videos/drmFiles/drmfile012DecPlayCount4.dcf?drm=enc",
                        Connector.READ);
            if (false == conn2.canWrite())
                throw new TestFailedException();
            counter = 2;
            InputStream in1 = conn1.openInputStream();
            counter = 3;
            byte[] byteArray1 = new byte[2000];
            int retValue = in1.read(byteArray1);
            counter = 4;
            if (retValue != 1025)
                throw new TestFailedException("retValue: " + retValue);
            counter = 5;
            for (int i = 0; i < retValue; ++i)
            {
                if (byteArray1[i] != 56)
                    throw new TestFailedException();
            }
            counter = 6;
            InputStream in2 = conn2.openInputStream();
            counter = 7;
            byteArray1 = new byte[2000];
            retValue = in2.read(byteArray1);
            counter = 8;
            if (retValue != 1354)
                throw new TestFailedException();
            counter = 9;
            /*
             * for(int i=0; i<retValue;++i){ if(byteArray1[i] != 4) throw new
             * TestFailedException(); }
             */
            counter = 10;
            if (false == conn1.canWrite())
                throw new TestFailedException();
            counter = 11;
            if (false == conn2.canWrite())
                throw new TestFailedException();
            counter = 12;
            in1.close();
            conn1.close();
            in2.close();
            conn2.close();

            assertTrue(" run_1321() ok ", true);
        }
        catch (Exception ex)
        {
            append(ex.toString());
            append("TEST FAILED!!! Case: run_1321() counter: " + counter + "\n");
            assertTrue(" run_1321() FAILED!!! Counter: " + counter, false);
        }
    }// end run_1321()

    private void run_1661()
    {
        FileConnection conn1 = null;
        FileConnection conn2 = null;
        int counter = 0;
        try
        {
            // 1
            conn1 = (FileConnection) Connector
                    .open(
                        "file:///"
                        + iRoot
                        + "data/Videos/drmFiles/drmfile014DecPlayCount2.dcf?drm=dec+play",
                        Connector.READ);
            counter = 1;
            InputStream in1 = conn1.openInputStream();
            counter = 2;
            conn2 = (FileConnection) Connector
                    .open(
                        "file:///"
                        + iRoot
                        + "data/Videos/drmFiles/drmfile014DecPlayCount2.dcf?drm=dec+play",
                        Connector.READ_WRITE);
            counter = 3;
            in1.close();
            try
            {
                conn2.setWritable(false);
                throw new TestFailedException();
            }
            catch (SecurityException ex)
            {
            }
            counter = 4;
            conn1.close();
            conn2.close();

            assertTrue(" run_1661() ok ", true);
        }
        catch (Exception ex)
        {
            append(ex.toString());
            append("TEST FAILED!!! Case: run_1661() counter: " + counter + "\n");
            assertTrue(" run_1661() FAILED!!! Counter: " + counter, false);
        }
    }// end run_1661()

    private void run_1421()
    {
        FileConnection conn1 = null;
        FileConnection conn2 = null;
        int counter = 0;
        try
        {
            conn1 = (FileConnection) Connector
                    .open(
                        "file:///"
                        + iRoot
                        + "data/Videos/drmFiles/drmfile013DecPlayCount1.dcf?drm=dec+play",
                        Connector.READ);
            counter = 1;
            if (true == conn1.isHidden())
                throw new TestFailedException();
            counter = 100;
            conn2 = (FileConnection) Connector
                    .open(
                        "file:///"
                        + iRoot
                        + "data/Videos/drmFiles/drmfile013DecPlayCount1.dcf?drm=enc",
                        Connector.READ);
            if (true == conn2.isHidden())
                throw new TestFailedException();
            counter = 2;
            InputStream in1 = conn1.openInputStream();
            counter = 3;
            byte[] byteArray1 = new byte[1030];
            int retValue = in1.read(byteArray1);
            counter = 4;
            if (retValue != 1025)
                throw new TestFailedException("retValue: " + retValue);
            counter = 5;
            for (int i = 0; i < retValue; ++i)
            {
                if (byteArray1[i] != 56)
                    throw new TestFailedException();
            }
            counter = 6;
            InputStream in2 = conn2.openInputStream();
            counter = 7;
            byteArray1 = new byte[2000];
            retValue = in2.read(byteArray1);
            counter = 8;
            if (retValue != 1354)
                throw new TestFailedException("retValue: " + retValue);
            counter = 10;
            if (true == conn1.isHidden())
                throw new TestFailedException();
            counter = 11;
            if (true == conn2.isHidden())
                throw new TestFailedException();
            counter = 12;
            in1.close();
            conn1.close();
            in2.close();
            conn2.close();

            assertTrue(" run_1421() ok ", true);
        }
        catch (Exception ex)
        {
            append(ex.toString());
            append("TEST FAILED!!! Case: run_1421() counter: " + counter + "\n");
            assertTrue(" run_1421() FAILED!!! Counter: " + counter, false);
        }
    }// end run_1421()

    private void run_1851()
    {
        FileConnection conn1 = null;
        int counter = 0;
        try
        {
            // 1
            conn1 = (FileConnection) Connector
                    .open(
                        "file:///"
                        + iRoot
                        + "data/Videos/drmFiles/drmfile016DecPlayCount2.dcf?drm=dec+play",
                        Connector.READ);
            counter = 1;
            try
            {
                Enumeration fileList = conn1.list();
                throw new TestFailedException();
            }
            catch (IOException ex)
            {
            }
            conn1.close();
            counter = 2;

            // 2
            conn1 = (FileConnection) Connector
                    .open(
                        "file:///"
                        + iRoot
                        + "data/Videos/drmFiles/drmfile016DecPlayCount2.dcf?drm=dec+play",
                        Connector.READ);
            counter = 3;
            InputStream in1 = conn1.openInputStream();
            try
            {
                Enumeration fileList = conn1.list();
                throw new TestFailedException();
            }
            catch (IOException ex)
            {
            }
            conn1.close();
            in1.close();
            counter = 4;

            assertTrue(" run_1851() ok ", true);
            System.out.println(" run_1851() ok \n");
        }
        catch (Exception ex)
        {
            append(ex.toString());
            append("TEST FAILED!!! Case: run_1851() counter: " + counter + "\n");
            assertTrue(" run_1851() FAILED!!! Counter: " + counter, false);
        }
    }// end run_1851()

    private void run_1741()
    {
        FileConnection conn1 = null;
        FileConnection conn2 = null;
        int counter = 0;
        try
        {
            // 1
            conn1 = (FileConnection) Connector
                    .open(
                        "file:///"
                        + iRoot
                        + "data/Videos/drmFiles/drmfile015DecPlayCount3.dcf?drm=dec+play",
                        Connector.READ);
            counter = 1;
            InputStream in1 = conn1.openInputStream();
            counter = 2;
            conn2 = (FileConnection) Connector
                    .open(
                        "file:///"
                        + iRoot
                        + "data/Videos/drmFiles/drmfile015DecPlayCount3.dcf?drm=dec+play",
                        Connector.READ_WRITE);
            counter = 3;
            in1.close();
            try
            {
                conn2.setHidden(true);
                throw new TestFailedException();
            }
            catch (SecurityException ex)
            {
            }
            counter = 4;
            conn1.close();
            conn2.close();

            // 2
            conn1 = (FileConnection) Connector
                    .open(
                        "file:///"
                        + iRoot
                        + "data/Videos/drmFiles/drmfile015DecPlayCount3.dcf?drm=dec+play",
                        Connector.READ);
            counter = 1;
            in1 = conn1.openInputStream();
            counter = 2;
            conn2 = (FileConnection) Connector
                    .open(
                        "file:///"
                        + iRoot
                        + "data/Videos/drmFiles/drmfile015DecPlayCount3.dcf?drm=enc",
                        Connector.READ_WRITE);
            counter = 3;
            in1.close();
            try
            {
                conn2.setHidden(true);
                throw new TestFailedException();
            }
            catch (SecurityException ex)
            {
            }
            counter = 4;
            conn1.close();
            conn2.close();

            assertTrue(" run_1741() ok ", true);
        }
        catch (Exception ex)
        {
            append(ex.toString());
            append("TEST FAILED!!! Case: run_1741() counter: " + counter + "\n");
            assertTrue(" run_1741() FAILED!!! Counter: " + counter, false);
        }
    }// end run_1741()

    private void run_2032()
    {
        FileConnection conn1 = null;
        FileConnection conn2 = null;
        int counter = 0;
        try
        {
            // 1
            conn1 = (FileConnection) Connector
                    .open(
                        "file:///"
                        + iRoot
                        + "data/Videos/drmFiles/drmfile017DecPlayCount10.dcf?drm=dec+play",
                        Connector.READ);
            counter = 1;
            InputStream in1 = conn1.openInputStream();
            counter = 2;
            conn2 = (FileConnection) Connector
                    .open(
                        "file:///"
                        + iRoot
                        + "data/Videos/drmFiles/drmfile017DecPlayCount10.dcf?drm=dec+play",
                        Connector.READ_WRITE);
            counter = 3;
            in1.close();
            try
            {
                conn2.create();
                throw new TestFailedException();
            }
            catch (SecurityException ex)
            {
            }
            counter = 3;
            conn1.close();
            conn2.close();

            assertTrue(" run_2032() ok ", true);
        }
        catch (Exception ex)
        {
            append(ex.toString());
            append("TEST FAILED!!! Case: run_2032() counter: " + counter + "\n");
            assertTrue(" run_2032() FAILED!!! Counter: " + counter, false);
        }
    }// end run_2032()

    private void run_2221()
    {
        FileConnection conn1 = null;
        FileConnection conn2 = null;
        int counter = 0;
        try
        {
            // 1
            conn1 = (FileConnection) Connector
                    .open(
                        "file:///"
                        + iRoot
                        + "data/Videos/drmFiles/drmfile017DecPlayCount10.dcf?drm=dec+play",
                        Connector.READ);
            counter = 1;
            InputStream in1 = conn1.openInputStream();
            counter = 2;
            if (false == conn1.exists())
                throw new TestFailedException();
            counter = 3;
            in1.close();
            conn1.close();

            // 2
            conn1 = (FileConnection) Connector
                    .open(
                        "file:///"
                        + iRoot
                        + "data/Videos/drmFiles/drmfile017DecPlayCount10.dcf?drm=dec+play",
                        Connector.READ);
            counter = 4;
            if (false == conn1.exists())
                throw new TestFailedException();
            counter = 5;
            in1 = conn1.openInputStream();
            byte[] byteArray1 = new byte[1030];
            int retValue = in1.read(byteArray1);
            counter = 6;
            if (retValue != 1025)
                throw new TestFailedException();
            counter = 7;
            for (int i = 0; i < retValue; ++i)
            {
                if (byteArray1[i] != 56)
                    throw new TestFailedException();
            }
            counter = 8;
            in1.close();
            conn1.close();

            // 3
            conn1 = (FileConnection) Connector
                    .open(
                        "file:///"
                        + iRoot
                        + "data/Videos/drmFiles/drmfile017DecPlayCount10.dcf?drm=dec+play",
                        Connector.READ);
            counter = 9;
            conn2 = (FileConnection) Connector
                    .open(
                        "file:///"
                        + iRoot
                        + "data/Videos/drmFiles/drmfile017DecPlayCount10.dcf?drm=dec+play",
                        Connector.READ);
            counter = 10;
            in1 = conn1.openInputStream();
            counter = 11;
            if (false == conn1.exists())
                throw new TestFailedException();
            counter = 12;
            if (false == conn2.exists())
                throw new TestFailedException();
            counter = 13;
            in1.close();
            if (false == conn1.exists())
                throw new TestFailedException();
            counter = 14;
            if (false == conn2.exists())
                throw new TestFailedException();
            counter = 15;
            in1 = conn2.openInputStream();
            byteArray1 = new byte[1030];
            retValue = in1.read(byteArray1);
            counter = 16;
            if (retValue != 1025)
                throw new TestFailedException();
            counter = 17;
            for (int i = 0; i < retValue; ++i)
            {
                if (byteArray1[i] != 56)
                    throw new TestFailedException();
            }
            counter = 18;
            in1.close();
            conn1.close();
            conn2.close();

            // 4
            conn1 = (FileConnection) Connector
                    .open(
                        "file:///"
                        + iRoot
                        + "data/Videos/drmFiles/drmfile017DecPlayCount10.dcf?drm=dec+play",
                        Connector.READ);
            counter = 19;
            conn2 = (FileConnection) Connector
                    .open(
                        "file:///"
                        + iRoot
                        + "data/Videos/drmFiles/drmfile017DecPlayCount10.dcf?drm=enc",
                        Connector.READ);
            counter = 20;
            if (false == conn1.exists())
                throw new TestFailedException();
            counter = 21;
            if (false == conn2.exists())
                throw new TestFailedException();
            conn1.close();
            conn2.close();

            assertTrue(" run_2221() ok ", true);
        }
        catch (Exception ex)
        {
            append(ex.toString());
            append("TEST FAILED!!! Case: run_2221() counter: " + counter + "\n");
            assertTrue(" run_2221() FAILED!!! Counter: " + counter, false);
        }
    }// end run_2221()

    private void run_2222()
    {
        FileConnection conn1 = null;
        FileConnection conn2 = null;
        int counter = 0;
        try
        {
            // 1
            conn1 = (FileConnection) Connector
                    .open(
                        "file:///"
                        + iRoot
                        + "data/Videos/drmFiles/drmfile019DecPlayCount2.dcf?drm=dec+play",
                        Connector.READ);
            counter = 1;
            InputStream in1 = conn1.openInputStream();
            counter = 2;
            conn2 = (FileConnection) Connector
                    .open(
                        "file:///"
                        + iRoot
                        + "data/Videos/drmFiles/drmfile019DecPlayCount2.dcf?drm=dec+play",
                        Connector.READ_WRITE);
            counter = 3;
            if (false == conn1.exists())
                throw new TestFailedException();
            counter = 4;
            if (false == conn2.exists())
                throw new TestFailedException();
            counter = 5;
            in1.close();
            try
            {
                conn2.exists();
                throw new TestFailedException();
            }
            catch (SecurityException ex)
            {
            }
            counter = 6;
            conn1.close();
            conn2.close();

            // 2
            conn1 = (FileConnection) Connector
                    .open(
                        "file:///"
                        + iRoot
                        + "data/Videos/drmFiles/drmfile019DecPlayCount2.dcf?drm=dec+play",
                        Connector.READ);
            counter = 1;
            in1 = conn1.openInputStream();
            counter = 2;
            conn2 = (FileConnection) Connector
                    .open(
                        "file:///"
                        + iRoot
                        + "data/Videos/drmFiles/drmfile019DecPlayCount2.dcf?drm=enc",
                        Connector.READ_WRITE);
            counter = 3;
            if (false == conn1.exists())
                throw new TestFailedException();
            counter = 4;
            if (false == conn2.exists())
                throw new TestFailedException();
            counter = 5;
            in1.close();
            try
            {
                conn2.exists();
                throw new TestFailedException();
            }
            catch (SecurityException ex)
            {
            }
            counter = 6;
            conn1.close();
            conn2.close();

            assertTrue(" run_2222() ok ", true);
        }
        catch (Exception ex)
        {
            append(ex.toString());
            append("TEST FAILED!!! Case: run_2222() counter: " + counter + "\n");
            assertTrue(" run_2222() FAILED!!! Counter: " + counter, false);
        }
    }// end run_2222()

    private void run_2451()
    {
        FileConnection conn1 = null;
        FileConnection conn2 = null;
        int counter = 0;
        try
        {
            // 1
            conn1 = (FileConnection) Connector
                    .open(
                        "file:///"
                        + iRoot
                        + "data/Videos/drmFiles/drmfile020DecPlayCount3.dcf?drm=dec+play",
                        Connector.READ);
            counter = 1;
            InputStream in1 = conn1.openInputStream();
            counter = 2;
            conn2 = (FileConnection) Connector
                    .open(
                        "file:///"
                        + iRoot
                        + "data/Videos/drmFiles/drmfile020DecPlayCount3.dcf?drm=dec+play",
                        Connector.READ_WRITE);
            counter = 3;
            if (false == conn1.exists())
                throw new TestFailedException();
            counter = 4;
            if (false == conn2.exists())
                throw new TestFailedException();
            counter = 5;
            in1.close();
            try
            {
                conn2.delete();
                throw new TestFailedException();
            }
            catch (SecurityException ex)
            {
            }

            // 2
            conn1 = (FileConnection) Connector
                    .open(
                        "file:///"
                        + iRoot
                        + "data/Videos/drmFiles/drmfile020DecPlayCount3.dcf?drm=dec+play",
                        Connector.READ);
            counter = 10;
            in1 = conn1.openInputStream();
            counter = 11;
            conn2 = (FileConnection) Connector
                    .open(
                        "file:///"
                        + iRoot
                        + "data/Videos/drmFiles/drmfile020DecPlayCount3.dcf?drm=enc",
                        Connector.READ_WRITE);
            counter = 12;
            if (false == conn1.exists())
                throw new TestFailedException();
            counter = 13;
            if (false == conn2.exists())
                throw new TestFailedException();
            counter = 14;
            in1.close();
            try
            {
                conn2.delete();
                throw new TestFailedException();
            }
            catch (SecurityException ex)
            {
            }

            assertTrue(" run_2451() ok ", true);
        }
        catch (Exception ex)
        {
            append(ex.toString());
            append("TEST FAILED!!! Case: run_2451() counter: " + counter + "\n");
            assertTrue(" run_2451() FAILED!!! Counter: " + counter, false);
        }
    }// end run_2451()

    private void run_2574()
    {
        FileConnection conn1 = null;
        FileConnection conn2 = null;
        int counter = 0;
        try
        {
            // 1
            conn1 = (FileConnection) Connector
                    .open(
                        "file:///"
                        + iRoot
                        + "data/Videos/drmFiles/drmfile021DecPlayCount2.dcf?drm=dec+play",
                        Connector.READ);
            counter = 1;
            InputStream in1 = conn1.openInputStream();
            counter = 2;
            conn2 = (FileConnection) Connector
                    .open(
                        "file:///"
                        + iRoot
                        + "data/Videos/drmFiles/drmfile021DecPlayCount2.dcf?drm=dec+play",
                        Connector.READ_WRITE);
            counter = 3;
            in1.close();
            try
            {
                conn2.rename("new name");
                throw new TestFailedException();
            }
            catch (SecurityException ex)
            {
            }
            counter = 4;
            conn1.close();
            conn2.close();

            // 2
            conn1 = (FileConnection) Connector
                    .open(
                        "file:///"
                        + iRoot
                        + "data/Videos/drmFiles/drmfile021DecPlayCount2.dcf?drm=dec+play",
                        Connector.READ);
            counter = 10;
            in1 = conn1.openInputStream();
            counter = 11;
            conn2 = (FileConnection) Connector
                    .open(
                        "file:///"
                        + iRoot
                        + "data/Videos/drmFiles/drmfile021DecPlayCount2.dcf?drm=enc",
                        Connector.READ_WRITE);
            counter = 12;
            in1.close();
            try
            {
                conn2.rename("new name");
                throw new TestFailedException();
            }
            catch (SecurityException ex)
            {
            }
            counter = 13;
            conn1.close();
            conn2.close();

            assertTrue(" run_2574() ok ", true);

        }
        catch (Exception ex)
        {
            append(ex.toString());
            append("TEST FAILED!!! Case: run_2574() counter: " + counter + "\n");
            assertTrue(" run_2574() FAILED!!! Counter: " + counter, false);
        }
    }// end run_2574()

    private void run_2651()
    {
        FileConnection conn1 = null;
        FileConnection conn2 = null;
        int counter = 0;
        try
        {
            // 1
            conn1 = (FileConnection) Connector
                    .open(
                        "file:///"
                        + iRoot
                        + "data/Videos/drmFiles/drmfile022DecPlayCount2.dcf?drm=dec+play",
                        Connector.READ);
            counter = 1;
            InputStream in1 = conn1.openInputStream();
            counter = 2;
            conn2 = (FileConnection) Connector
                    .open(
                        "file:///"
                        + iRoot
                        + "data/Videos/drmFiles/drmfile022DecPlayCount2.dcf?drm=dec+play",
                        Connector.READ_WRITE);
            counter = 3;
            in1.close();
            try
            {
                conn2.truncate(5);
                throw new TestFailedException();
            }
            catch (SecurityException ex)
            {
            }
            counter = 4;
            conn1.close();
            conn2.close();

            // 2
            conn1 = (FileConnection) Connector
                    .open(
                        "file:///"
                        + iRoot
                        + "data/Videos/drmFiles/drmfile022DecPlayCount2.dcf?drm=dec+play",
                        Connector.READ);
            counter = 10;
            in1 = conn1.openInputStream();
            counter = 11;
            conn2 = (FileConnection) Connector
                    .open(
                        "file:///"
                        + iRoot
                        + "data/Videos/drmFiles/drmfile022DecPlayCount2.dcf?drm=enc",
                        Connector.READ_WRITE);
            counter = 12;
            in1.close();
            try
            {
                conn2.truncate(5);
                throw new TestFailedException();
            }
            catch (SecurityException ex)
            {
            }
            counter = 13;
            conn1.close();
            conn2.close();

            assertTrue(" run_2651() ok ", true);
            System.out.println(" run_2651() ok \n");
        }
        catch (Exception ex)
        {
            append(ex.toString());
            append("TEST FAILED!!! Case: run_2651() counter: " + counter + "\n");
            assertTrue(" run_2651() FAILED!!! Counter: " + counter, false);
        }
    }// end run_2651()

    private void run_2793()
    {
        FileConnection conn1 = null;
        int counter = 0;
        try
        {
            // 1
            conn1 = (FileConnection) Connector.open(
                        "file:///Internal/Videos/drmFiles/", Connector.READ_WRITE);
            counter = 1;
            try
            {
                conn1.setFileConnection("drmfile023DecPlayCount10.dcf");
                throw new TestFailedException();
            }
            catch (SecurityException ex)
            {
            }
            counter = 2;
            conn1.close();

            // 2
            conn1 = (FileConnection) Connector.open(
                        "file:///Internal/Videos/drmFiles/", Connector.READ);
            counter = 1;
            try
            {
                conn1.setFileConnection("drmfile023DecPlayCount10.dcf");
                throw new TestFailedException();
            }
            catch (SecurityException ex)
            {
            }
            counter = 2;
            conn1.close();

            // 2
            conn1 = (FileConnection) Connector.open(
                        "file:///Internal/Videos/drmFiles/", Connector.READ);
            counter = 3;
            try
            {
                conn1
                .setFileConnection("drmfile023DecPlayCount10.dcf?drm=dec+play");
                throw new TestFailedException();
            }
            catch (IOException ex)
            {
            }
            counter = 4;
            conn1.close();

            // 3
            conn1 = (FileConnection) Connector
                    .open(
                        "file:///Internal/Videos/drmFiles/drmfile023DecPlayCount10.dcf?drm=dec+play",
                        Connector.READ);
            counter = 5;
            try
            {
                conn1.setFileConnection("unprotectedFile.txt");
                throw new TestFailedException();
            }
            catch (IOException ex)
            {
            }
            counter = 6;
            conn1.close();

            // 4
            conn1 = (FileConnection) Connector
                    .open(
                        "file:///Internal/Videos/drmFiles/drmfile023DecPlayCount10.dcf?drm=dec+play",
                        Connector.READ);
            counter = 7;
            InputStream in1 = conn1.openInputStream();
            try
            {
                conn1.setFileConnection("unprotectedFile.txt");
                throw new TestFailedException();
            }
            catch (IOException ex)
            {
            }
            counter = 8;
            conn1.close();
            in1.close();

            assertTrue(" run_2793() ok ", true);
        }
        catch (Exception ex)
        {
            append(ex.toString());
            append("TEST FAILED!!! Case: run_2793() counter: " + counter + "\n");
            assertTrue(" run_2793() FAILED!!! Counter: " + counter, false);
        }
    }// end run_2793()

    private void run_2801()
    {
        FileConnection conn1 = null;
        int counter = 0;
        try
        {
            // 1
            FileTestUtils.testGetConnectionMethods(
                "file:///"
                + iRoot
                + "data/Videos/drmFiles/drmfile024DecDisplayCount50.dcf?drm=dec+display",
                "drmfile024DecDisplayCount50.dcf",
                "/" + iRoot + "data/Videos/drmFiles/",
                "file:///"
                + iRoot
                + "data/Videos/drmFiles/drmfile024DecDisplayCount50.dcf?drm=dec+display");
            counter = 1;
            FileTestUtils.testGetConnectionMethods(
                "file:///"
                + iRoot
                + "data/Videos/drmFiles/drmfile024DecPlayCount50.dcf?drm=dec+play",
                "drmfile024DecPlayCount50.dcf",
                "/" + iRoot + "data/Videos/drmFiles/",
                "file:///"
                + iRoot
                + "data/Videos/drmFiles/drmfile024DecPlayCount50.dcf?drm=dec+play");
            counter = 2;
            FileTestUtils.testGetConnectionMethods(
                "file:///"
                + iRoot
                + "data/Videos/drmFiles/drmfile024DecPrintCount50.dcf?drm=dec+print",
                "drmfile024DecPrintCount50.dcf",
                "/" + iRoot + "data/Videos/drmFiles/",
                "file:///"
                + iRoot
                + "data/Videos/drmFiles/drmfile024DecPrintCount50.dcf?drm=dec+print");
            counter = 3;
            FileTestUtils.testGetConnectionMethods(
                "file:///"
                + iRoot
                + "data/Videos/drmFiles/drmfile024DecExecuteCount50.dcf?drm=dec+execute",
                "drmfile024DecExecuteCount50.dcf",
                "/" + iRoot + "data/Videos/drmFiles/",
                "file:///"
                + iRoot
                + "data/Videos/drmFiles/drmfile024DecExecuteCount50.dcf?drm=dec+execute");
            counter = 4;
            FileTestUtils.testGetConnectionMethods(
                "file:///"
                + iRoot
                + "data/Videos/drmFiles/drmfile024DecDisplayCount50.dcf?drm=dec+display+preview",
                "drmfile024DecDisplayCount50.dcf",
                "/" + iRoot + "data/Videos/drmFiles/",
                "file:///"
                + iRoot
                + "data/Videos/drmFiles/drmfile024DecDisplayCount50.dcf?drm=dec+display+preview");
            counter = 5;
            FileTestUtils.testGetConnectionMethods(
                "file:///"
                + iRoot
                + "data/Videos/drmFiles/drmfile024DecPlayCount50.dcf?drm=dec+play+preview",
                "drmfile024DecPlayCount50.dcf",
                "/" + iRoot + "data/Videos/drmFiles/",
                "file:///"
                + iRoot
                + "data/Videos/drmFiles/drmfile024DecPlayCount50.dcf?drm=dec+play+preview");
            counter = 6;
            FileTestUtils.testGetConnectionMethods(
                "file:///"
                + iRoot
                + "data/Videos/drmFiles/drmfile024DecPrintCount50.dcf?drm=dec+print+preview",
                "drmfile024DecPrintCount50.dcf",
                "/" + iRoot + "data/Videos/drmFiles/",
                "file:///"
                + iRoot
                + "data/Videos/drmFiles/drmfile024DecPrintCount50.dcf?drm=dec+print+preview");
            counter = 7;
            FileTestUtils.testGetConnectionMethods(
                "file:///"
                + iRoot
                + "data/Videos/drmFiles/drmfile024DecExecuteCount50.dcf?drm=dec+execute+preview",
                "drmfile024DecExecuteCount50.dcf",
                "/" + iRoot + "data/Videos/drmFiles/",
                "file:///"
                + iRoot
                + "data/Videos/drmFiles/drmfile024DecExecuteCount50.dcf?drm=dec+execute+preview");
            counter = 8;
            FileTestUtils.testGetConnectionMethods(
                "file:///"
                + iRoot
                + "data/Videos/drmFiles/drmfile024DecPlayCount50.dcf?drm=enc",
                "drmfile024DecPlayCount50.dcf",
                "/" + iRoot + "data/Videos/drmFiles/",
                "file:///"
                + iRoot
                + "data/Videos/drmFiles/drmfile024DecPlayCount50.dcf?drm=enc");
            counter = 9;
            FileTestUtils.testGetConnectionMethods(
                "file:///Internal/Videos/drmFiles/drmfile024DecPrintCount50.dcf?drm=dec+prInt ",
                "drmfile024DecPrintCount50.dcf",
                "/Internal/Videos/drmFiles/",
                "file:///Internal/Videos/drmFiles/drmfile024DecPrintCount50.dcf?drm=dec+prInt%20");
            counter = 11;
            FileTestUtils.testGetConnectionMethods(
                "file:///Internal/Videos/drmFiles/drmfile024DecPrintCount50.dcf?foo=bar;drm=dec+prInt ",
                "drmfile024DecPrintCount50.dcf",
                "/Internal/Videos/drmFiles/",
                "file:///Internal/Videos/drmFiles/drmfile024DecPrintCount50.dcf?foo=bar;drm=dec+prInt%20");
            counter = 12;
            FileTestUtils.testGetConnectionMethods(
                "file:///"
                + iRoot
                + "data/Videos/drmFiles/drmfile024DecPrintCount50.dcf?foo = bar;drm=dec+prInt;foo = bar",
                "drmfile024DecPrintCount50.dcf",
                "/" + iRoot + "data/Videos/drmFiles/",
                "file:///"
                + iRoot
                + "data/Videos/drmFiles/drmfile024DecPrintCount50.dcf?foo%20=%20bar;drm=dec+prInt;foo%20=%20bar");
            counter = 13;
            FileTestUtils.testGetConnectionMethods(
                "file:///Memory card/drmfile025DecPrintCount50.dcf?foo = bar;drm=dec+prInt;foo = bar",
                "drmfile025DecPrintCount50.dcf",
                "/Memory card/",
                "file:///Memory%20card/drmfile025DecPrintCount50.dcf?foo%20=%20bar;drm=dec+prInt;foo%20=%20bar");
            counter = 14;
            FileTestUtils.testGetConnectionMethods(
                "file:///e:/drmfile025DecPrintCount50.dcf?drm=dEc+prInt+preview;foo = bar",
                "drmfile025DecPrintCount50.dcf", "/e:/",
                "file:///e:/drmfile025DecPrintCount50.dcf?drm=dEc+prInt+preview;foo%20=%20bar");
            counter = 15;

            assertTrue(" run_2801() ok ", true);
        }
        catch (Exception ex)
        {
            append(ex.toString());
            append("TEST FAILED!!! Case: run_2801() counter: " + counter + "\n");
            assertTrue(" run_2801() FAILED!!! Counter: " + counter, false);
        }
    }// end run_2801()

    private void run_2931()
    {
        FileConnection conn1 = null;
        FileConnection conn2 = null;
        int counter = 0;
        try
        {
            // 1
            conn1 = (FileConnection) Connector
                    .open(
                        "file:///"
                        + iRoot
                        + "data/Videos/drmFiles/drmfile025DecPlayCount3.dcf?drm=dec+play",
                        Connector.READ);
            counter = 1;
            InputStream in1 = conn1.openInputStream();
            counter = 2;
            long lastmodified = conn1.lastModified();
            counter = 3;
            if (0 == lastmodified)
                throw new TestFailedException();
            in1.close();
            conn1.close();

            // 2
            conn1 = (FileConnection) Connector
                    .open(
                        "file:///"
                        + iRoot
                        + "data/Videos/drmFiles/drmfile025DecPlayCount3.dcf?drm=dec+play",
                        Connector.READ);
            counter = 4;
            in1 = conn1.openInputStream();
            counter = 5;
            conn2 = (FileConnection) Connector
                    .open(
                        "file:///"
                        + iRoot
                        + "data/Videos/drmFiles/drmfile025DecPlayCount3.dcf?drm=dec+play",
                        Connector.READ);
            counter = 6;
            lastmodified = conn1.lastModified();
            counter = 7;
            if (0 == lastmodified)
                throw new TestFailedException();
            counter = 8;
            lastmodified = conn2.lastModified();
            counter = 9;
            if (0 == lastmodified)
                throw new TestFailedException();
            counter = 10;
            conn1.close();
            conn2.close();
            in1.close();

            // 3
            conn1 = (FileConnection) Connector
                    .open(
                        "file:///"
                        + iRoot
                        + "data/Videos/drmFiles/drmfile025DecPlayCount3.dcf?drm=dec+play",
                        Connector.READ);
            counter = 20;
            in1 = conn1.openInputStream();
            counter = 21;
            conn2 = (FileConnection) Connector
                    .open(
                        "file:///"
                        + iRoot
                        + "data/Videos/drmFiles/drmfile025DecPlayCount3.dcf?drm=enc",
                        Connector.READ);
            counter = 22;
            lastmodified = conn1.lastModified();
            counter = 23;
            if (0 == lastmodified)
                throw new TestFailedException();
            counter = 24;
            lastmodified = conn2.lastModified();
            counter = 25;
            if (0 == lastmodified)
                throw new TestFailedException();
            counter = 26;
            conn1.close();
            conn2.close();
            in1.close();

            assertTrue(" run_2931() ok ", true);
        }
        catch (Exception ex)
        {
            append(ex.toString());
            append("TEST FAILED!!! Case: run_2931() counter: " + counter + "\n");
            assertTrue(" run_2931() FAILED!!! Counter: " + counter, false);
        }
    }//end run_2931()

}