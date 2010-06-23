/*
* Copyright (c) 2008-2010 Nokia Corporation and/or its subsidiary(-ies).
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


package com.nokia.mj.impl.installer.downloader;

import com.nokia.mj.impl.fileutils.FileUtility;
import com.nokia.mj.impl.installer.utils.InstallerException;
import com.nokia.mj.impl.installer.utils.InstallerMain;
import com.nokia.mj.impl.installer.utils.Log;
import com.nokia.mj.impl.utils.Base64;

//import java.net.URL;
//import java.net.URLConnection;

import j2meunit.framework.Test;
import j2meunit.framework.TestCase;
import j2meunit.framework.TestMethod;
import j2meunit.framework.TestSuite;

/**
 * Downloader unit tests.
 */
public class DownloaderTest extends TestCase implements InstallerMain
{
    private static final String TEST_URL = "/installertest/HelloWorld.jar";
    private static final int TEST_URL_SIZE = 1511; // test data file size

    // Begin j2meunit test framework setup

    public void installerMain(String[] args)
    {
        TestSuite suite = new TestSuite(this.getClass().getName());

        suite.addTest(new DownloaderTest("testIsDownloadUrl", new TestMethod()
        {
            public void run(TestCase tc)
            {
                ((DownloaderTest)tc).testIsDownloadUrl();
            }
        }));

        suite.addTest(new DownloaderTest("testGetBasicAuthBase64", new TestMethod()
        {
            public void run(TestCase tc)
            {
                ((DownloaderTest)tc).testGetBasicAuthBase64();
            }
        }));

        suite.addTest(new DownloaderTest("testBasicDownloadGcf", new TestMethod()
        {
            public void run(TestCase tc)
            {
                ((DownloaderTest)tc).testBasicDownloadGcf();
            }
        }));

        suite.addTest(new DownloaderTest("testDownloadFailureGcf", new TestMethod()
        {
            public void run(TestCase tc)
            {
                ((DownloaderTest)tc).testDownloadFailureGcf();
            }
        }));

        com.nokia.mj.impl.utils.OmjTestRunner.run(suite);
    }

    public DownloaderTest()
    {
    }

    public DownloaderTest(String aTestName, TestMethod aTestMethod)
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

    /**
     * Test isDownUrl method.
     */
    public void testIsDownloadUrl()
    {
        try
        {
            String[] urls = new String[]
            {
                "http://a.b.c/A.jad",
                "https://a.b.c/A.jad",
                "HTTP://a.b.c/A.jad",
                "HTTPS://a.b.c/A.jad",
            };
            for (int i = 0; i < urls.length; i++)
            {
                assertTrue("Expected true for " + urls[i],
                           Downloader.isDownloadUrl(urls[i]));
            }

            urls = new String[]
            {
                "A.jad",
                "file:///A.jad",
            };
            for (int i = 0; i < urls.length; i++)
            {
                assertTrue("Expected false for " + urls[i],
                           !Downloader.isDownloadUrl(urls[i]));
            }
        }
        catch (Exception ex)
        {
            ex.printStackTrace();
            assertTrue("Unexpected exception: " + ex, false);
        }
    }

    /**
     * Test getBasicAuthBase64 method.
     */
    public void testGetBasicAuthBase64()
    {
        try
        {
            String username = "user";
            String password = "pwd";
            String basicAuthEnc =
                Downloader.getBasicAuthBase64(username, password);
            String basicAuthDec =
                new String(Base64.decode(basicAuthEnc), "ISO8859_1");
            assertTrue("Incorrect basic auth credentials: " + basicAuthDec,
                       basicAuthDec.equals(username + ":" + password));
        }
        catch (Exception ex)
        {
            ex.printStackTrace();
            assertTrue("Unexpected exception: " + ex, false);
        }
    }

    /**
     * Test a basic download case with given Downloader.
     */
    public void testBasicDownload(Downloader downloader,
                                  TestDlListener listener)
    {
        String server = System.getProperty(
                            "com.nokia.mj.impl.installer.test.server");
        assertNotNull("-server=<address>:<port> argument required.", server);

        try
        {
            String filename = "test.dat";
            DownloadInfo dlInfo = new DownloadInfo("http://" + server
                                                   + TEST_URL, filename);
            downloader.start(dlInfo);
            downloader.waitForCompletion();
            assertTrue("Result DlInfo is null",
                       listener.getDownloadInfo() != null);
            assertTrue("Result DlInfo has an exception: " +
                       listener.getDownloadInfo().getException(),
                       listener.getDownloadInfo().getException() == null);
            assertTrue("Downloaded file has wrong size",
                       new FileUtility(filename).fileSize() == TEST_URL_SIZE);
        }
        catch (Exception ex)
        {
            ex.printStackTrace();
            assertTrue("Unexpected exception: " + ex, false);
        }
    }
    public void testBasicDownloadGcf()
    {
        TestDlListener listener = new TestDlListener();
        Downloader downloader = GcfDownloader.getDownloader(listener);
        testBasicDownload(downloader, listener);
    }

    /**
     * Test a failing download case with given Downloader.
     */
    public void testDownloadFailure(Downloader downloader,
                                    TestDlListener listener)
    {
        try
        {
            String filename = "test.dat";
            DownloadInfo dlInfo =
                new DownloadInfo("http://unsupportedurl", filename);
            downloader.start(dlInfo);
            downloader.waitForCompletion();
            assertTrue("Result DlInfo is null",
                       listener.getDownloadInfo() != null);
            assertTrue("Result DlInfo does not have an exception",
                       listener.getDownloadInfo().getException() != null);
        }
        catch (Exception ex)
        {
            ex.printStackTrace();
            assertTrue("Unexpected exception: " + ex, false);
        }
    }
    public void testDownloadFailureGcf()
    {
        TestDlListener listener = new TestDlListener();
        Downloader downloader = GcfDownloader.getDownloader(listener);
        testDownloadFailure(downloader, listener);
    }

    private static void sleep(long aMillis)
    {
        try
        {
            Thread.sleep(aMillis);
        }
        catch (InterruptedException ie)
        {
        }
    }

    private static class TestDlListener implements DownloadListener
    {
        private DownloadInfo iDlInfo = null;
        public DownloadInfo getDownloadInfo()
        {
            return iDlInfo;
        }

        /** Download has started. */
        public void started(DownloadInfo aDlInfo)
        {
            Log.log("TestDlListener.started: " + aDlInfo);
        }

        /** Download has progressed. */
        public void updateProgress(DownloadInfo aDlInfo)
        {
            Log.log("TestDlListener.updateProgress: " + aDlInfo);
        }

        /** Download has ended. */
        public void ended(DownloadInfo aDlInfo)
        {
            iDlInfo = aDlInfo;
            Log.log("TestDlListener.ended: " + aDlInfo);
        }

        /**
         * Downloader needs username and password for HTTP authentication.
         * This method returns an array of two strings, the first being username
         * and second being password. If username and password cannot be obtained,
         * method returns null.
         *
         * @param aUrl URL for which username and password is needed.
         */
        public String[] getUsernamePassword(String aUrl)
        {
            Log.log("TestDlListener.getUsernamePassword: " + aUrl);
            return new String[] { "guest", "guest" };
        }
    }

    // Begin JavaNetTest

    /**
     * Test basic functionality of java.net.URLConnection.
     * /
    public void testJavaNet() {
        String[] args = new String[] {
            //"http://www.commonly.known.internet.address.com",
            //"http://test.my.code.com",
            // The expected values below are for this url:
            "http://ip.to.test.server:7070/java-server/resources/JarUnderTest.jar",
        };

        try {
            //System.out.println("Connecting to " + args[0]);
            URL url = new URL(args[0]);
            //System.out.println("Got url");
            URLConnection connection = url.openConnection();
            //System.out.println("Got connection");
            for (int i = 0; i < 3; i++) {
                String h = connection.getHeaderField(i);
                //System.out.println("Got header field " + i + ": " + h);
                switch (i) {
                case 0:
                    assertTrue("Got unexpected header " + i + ": " + h,
                               h.equals("HTTP/1.1 200 OK"));
                    break;
                case 1:
                    assertTrue("Got unexpected header " + i + ": " + h,
                               h.equals("Apache-Coyote/1.1"));
                    break;
                case 2:
                    assertTrue("Got unexpected header " + i + ": " + h,
                               h.equals("W/\"10241-1214380094000\""));
                    break;
                }
            }
            String contentType = connection.getContentType();
            //System.out.println("Got content type " + contentType);
            assertTrue("Got unexpected content type: " + contentType,
                       contentType.equals("application/java-archive"));
            int contentLength = connection.getContentLength();
            //System.out.println("Got content length " + contentLength);
            assertTrue("Got unexpected content length: " + contentLength,
                       contentLength == 10241);
        } catch (Exception ex) {
            //System.out.println("Unexpected exception: " + ex);
            ex.printStackTrace();
            assertTrue("Unexpected exception: " + ex, false);
        }
    }
    */

    // End JavaNetTest
}
