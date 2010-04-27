/*
* Copyright (c) 2009 Nokia Corporation and/or its subsidiary(-ies).
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


package com.nokia.mj.impl.utils;

import com.nokia.mj.impl.installer.utils.InstallerMain;
import com.nokia.mj.impl.utils.BufferedReader;

import java.io.ByteArrayInputStream;
import java.io.InputStreamReader;
import java.io.IOException;

import j2meunit.framework.Test;
import j2meunit.framework.TestCase;
import j2meunit.framework.TestMethod;
import j2meunit.framework.TestSuite;

/**
 * BufferedReader unit tests.
 */
public class BufferedReaderTests extends TestCase implements InstallerMain
{

    private static final String TEST_DATA = "one\ntwo\nthree";
    private static final int MAX_BUF_SIZE = 16;

    // Begin j2meunit test framework setup
    public void installerMain(String[] args)
    {
        TestSuite suite = new TestSuite(this.getClass().getName());

        suite.addTest(new BufferedReaderTests("testMark", new TestMethod()
        {
            public void run(TestCase tc)
            {
                ((BufferedReaderTests)tc).testMark();
            }
        }));
        suite.addTest(new BufferedReaderTests("testRead", new TestMethod()
        {
            public void run(TestCase tc)
            {
                ((BufferedReaderTests)tc).testRead();
            }
        }));
        suite.addTest(new BufferedReaderTests("testRead2", new TestMethod()
        {
            public void run(TestCase tc)
            {
                ((BufferedReaderTests)tc).testRead2();
            }
        }));
        suite.addTest(new BufferedReaderTests("testReadLine", new TestMethod()
        {
            public void run(TestCase tc)
            {
                ((BufferedReaderTests)tc).testReadLine();
            }
        }));
        suite.addTest(new BufferedReaderTests("testSkip", new TestMethod()
        {
            public void run(TestCase tc)
            {
                ((BufferedReaderTests)tc).testSkip();
            }
        }));

        com.nokia.mj.impl.utils.OmjTestRunner.run(suite);
    }

    public BufferedReaderTests()
    {
    }

    public BufferedReaderTests(String aTestName, TestMethod aTestMethod)
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

    public void testMark()
    {
        BufferedReader r = null;
        try
        {
            r = new BufferedReader(
                new InputStreamReader(
                    new ByteArrayInputStream(TEST_DATA.getBytes())));
            assertTrue("r.markSupported() != false",
                       r.markSupported() == false);
            try
            {
                r.mark(10);
                assertTrue("mark didn't throw exception", false);
            }
            catch (IOException ioe)
            {
            }
            catch (Throwable t)
            {
                assertTrue("mark threw wrong exception: " + t.toString(),
                           false);
            }
            try
            {
                r.reset();
                assertTrue("reset didn't throw exception", false);
            }
            catch (IOException ioe)
            {
            }
            catch (Throwable t)
            {
                assertTrue("reset threw wrong exception: " + t.toString(),
                           false);
            }
        }
        catch (Throwable t)
        {
            assertTrue(t.toString(), false);
            t.printStackTrace();
        }
        finally
        {
            if (r != null)
            {
                try
                {
                    r.close();
                }
                catch (IOException ioe)
                {
                }
            }
        }
    }

    public void testRead()
    {
        for (int i = 1; i < MAX_BUF_SIZE; i++)
        {
            testRead(i);
        }
    }

    private void testRead(int aBufSize)
    {
        BufferedReader r = null;
        try
        {
            r = new BufferedReader(
                new InputStreamReader(
                    new ByteArrayInputStream(TEST_DATA.getBytes())));
            try
            {
                for (int i = 0; r.ready(); i++)
                {
                    char c = (char)r.read();
                    assertTrue("incorrect char '" + c + "' at index " + i +
                               ", bufSize=" + aBufSize,
                               TEST_DATA.charAt(i) == c);
                }
                int eos = r.read();
                assertTrue("read: incorrect end of stream result: " + eos +
                           ", bufSize=" + aBufSize, eos == -1);
            }
            catch (IOException ioe)
            {
                assertTrue("read threw exception: " + ioe.toString(),
                           false);
            }
        }
        catch (Throwable t)
        {
            assertTrue(t.toString(), false);
            t.printStackTrace();
        }
        finally
        {
            if (r != null)
            {
                try
                {
                    r.close();
                }
                catch (IOException ioe)
                {
                }
            }
        }
    }

    public void testRead2()
    {
        for (int i = 1; i < MAX_BUF_SIZE; i++)
        {
            testRead2(i);
        }
    }

    private void testRead2(int aBufSize)
    {
        BufferedReader r = null;
        try
        {
            r = new BufferedReader(
                new InputStreamReader(
                    new ByteArrayInputStream(TEST_DATA.getBytes())));
            try
            {
                char[] buf = new char[5];
                for (int i = 0; r.ready(); i++)
                {
                    int count = r.read(buf);
                    String line = new String(buf);
                    String msg = "incorrect chars '" + line +
                                 "', i=" + i + ", bufSize=" + aBufSize;
                    switch (i)
                    {
                    case 0:
                        assertTrue(msg, count == 5 && line.equals("one\nt"));
                        break;
                    case 1:
                        assertTrue(msg, count == 5 && line.equals("wo\nth"));
                        break;
                    case 2:
                        assertTrue(msg, count == 3 && line.equals("reeth"));
                        break;
                    default:
                        assertTrue("incorrect number of reads" +
                                   ", bufSize=" + aBufSize, false);
                    }
                }
                int eos = r.read();
                assertTrue("read: incorrect end of stream result: " + eos +
                           ", bufSize=" + aBufSize, eos == -1);
            }
            catch (IOException ioe)
            {
                assertTrue("read threw exception: " + ioe.toString(),
                           false);
            }
        }
        catch (Throwable t)
        {
            assertTrue(t.toString(), false);
            t.printStackTrace();
        }
        finally
        {
            if (r != null)
            {
                try
                {
                    r.close();
                }
                catch (IOException ioe)
                {
                }
            }
        }
    }

    public void testReadLine()
    {
        for (int i = 1; i < MAX_BUF_SIZE; i++)
        {
            testReadLine(i);
        }
    }

    private void testReadLine(int aBufSize)
    {
        BufferedReader r = null;
        try
        {
            r = new BufferedReader(
                new InputStreamReader(
                    new ByteArrayInputStream(TEST_DATA.getBytes())));
            try
            {
                for (int i = 0; r.ready(); i++)
                {
                    String line = r.readLine();
                    String msg = "incorrect line " + i + ": '" + line +
                                 "', bufSize=" + aBufSize;
                    switch (i)
                    {
                    case 0:
                        assertTrue(msg, line.equals("one"));
                        break;
                    case 1:
                        assertTrue(msg, line.equals("two"));
                        break;
                    case 2:
                        assertTrue(msg, line.equals("three"));
                        break;
                    default:
                        assertTrue("incorrect number of lines" +
                                   ", bufSize=" + aBufSize, false);
                    }
                }
                String eos = r.readLine();
                assertTrue("readline: incorrect end of stream result: '" + eos +
                           "', bufSize=" + aBufSize, eos == null);
            }
            catch (IOException ioe)
            {
                assertTrue("readLine threw exception: " + ioe.toString(),
                           false);
            }
        }
        catch (Throwable t)
        {
            assertTrue(t.toString(), false);
            t.printStackTrace();
        }
        finally
        {
            if (r != null)
            {
                try
                {
                    r.close();
                }
                catch (IOException ioe)
                {
                }
            }
        }
    }

    public void testSkip()
    {
        for (int bufSize=1; bufSize<MAX_BUF_SIZE; bufSize++)
        {
            for (int skipAmount=0; skipAmount<MAX_BUF_SIZE+2; skipAmount++)
            {
                testSkip(bufSize, skipAmount);
            }
        }
    }

    private void testSkip(int aBufSize, int aSkipAmount)
    {
        BufferedReader r = null;
        try
        {
            r = new BufferedReader(
                new InputStreamReader(
                    new ByteArrayInputStream(TEST_DATA.getBytes())));
            try
            {
                for (int i = 0; r.ready(); i++)
                {
                    char c = (char)r.read();
                    assertTrue("incorrect char '" + c + "' at index " + i +
                               ", bufSize=" + aBufSize +
                               ", skipAmount=" + aSkipAmount,
                               TEST_DATA.charAt(i) == c);
                    r.skip(aSkipAmount);
                    i += aSkipAmount;
                }
                long eos = r.skip(1);
                assertTrue("skip: incorrect end of stream result: " + eos +
                           ", bufSize=" + aBufSize, eos == 0);
            }
            catch (IOException ioe)
            {
                assertTrue("Unexpected exception: " + ioe.toString(),
                           false);
            }
        }
        catch (Throwable t)
        {
            assertTrue(t.toString(), false);
            t.printStackTrace();
        }
        finally
        {
            if (r != null)
            {
                try
                {
                    r.close();
                }
                catch (IOException ioe)
                {
                }
            }
        }
    }
}
