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

import com.nokia.mj.test.file.utils.FileTestUtils;
import com.nokia.mj.test.file.utils.FileTestBase;
import com.nokia.mj.test.file.utils.TestFailedException;
import com.nokia.mj.impl.installer.utils.InstallerMain;

public class TestFile_0060 extends FileTestBase
{
    public TestFile_0060()
    {
    }

    public TestFile_0060(String aTestName, TestMethod aTestMethod)
    {
        super(aTestName, aTestMethod);
    }

    public void assertFalse(String aMsg, boolean aCondition)
    {
        assertTrue(aMsg, !aCondition);
    }

    public static void registerTests(TestSuite suite)
    {
        suite.addTest(new TestFile_0060("run_0060", new TestMethod()
        {
            public void run(TestCase tc)
            {
                ((TestFile_0060) tc).run_0060();
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

    public void run_0060()
    {
        FileConnection conn1 = null;
        int counter = 0;
        try
        {
            conn1 = (FileConnection) Connector.open("file:///" + iRoot
                                                    + "mainTestDir/test000/newFileTest45");
            counter = 1;
            conn1.create();
            counter = 2;
            OutputStream outStream = conn1.openOutputStream();
            counter = 3;
            byte[] byteArray1 = new byte[10];
            for (int i = 0; i < byteArray1.length; ++i)
            {
                byteArray1[i] = 75;
            }
            outStream.write(byteArray1);
            counter = 4;
            outStream.flush();
            outStream.close();
            conn1.close();

            // Re-open a file to check that data was written to the file
            // correctly.
            conn1 = (FileConnection) Connector
                    .open("file:///" + iRoot
                          + "mainTestDir/test000/newFileTest45",
                          Connector.READ, true);
            counter = 5;
            InputStream inStream = conn1.openInputStream();
            counter = 6;
            byte[] byteArray2 = new byte[15];
            int retValue = inStream.read(byteArray2);
            counter = 7;
            if (retValue != 10)
                throw new TestFailedException("retValue: " + retValue);
            counter = 8;
            for (int i = 0; i < retValue; ++i)
            {
                if (byteArray2[i] != 75)
                    throw new TestFailedException();
            }
            counter = 9;
            inStream.close();
            conn1.close();

            /**
             * The Following tests are currently not valid CSB: 23/08/2008 ->
             * Need to clarify how this thing works!!!
             *
             * <pre>
             *
             * //Testing a Connector.openDataInputStream() operation.
             * DataInputStream dataInputStream = Connector.openDataInputStream(&quot;file:///&quot;
             *         + iRoot + &quot;mainTestDir/test000/newFileTest45&quot;);
             * counter = 10;
             * retValue = dataInputStream.read(byteArray2);
             * counter = 11;
             * if (retValue != 10)
             *     throw new TestFailedException(&quot;retValue: &quot; + retValue);
             * counter = 12;
             * for (int i = 0; i &lt; retValue; ++i)
             * {
             *     if (byteArray2[i] != 75)
             *         throw new TestFailedException();
             * }
             * counter = 13;
             * dataInputStream.close();
             *
             * //Testing a Connector.openDataOutputStream() operation.
             * DataOutputStream dataOutputStream = Connector.openDataOutputStream(&quot;file:///&quot;
             *         + iRoot + &quot;mainTestDir/test000/newFileTest45&quot;);
             * counter = 14;
             * for (int i = 0; i &lt; byteArray1.length; ++i)
             * {
             *     byteArray1[i] = 76;
             * }
             * counter = 15;
             * dataOutputStream.write(byteArray1);
             * counter = 16;
             * dataOutputStream.flush();
             * dataOutputStream.close();
             *
             * //Testing a Connector.openInputStream() operation.
             * InputStream inputStream = Connector.openDataInputStream(&quot;file:///&quot; + iRoot
             *         + &quot;mainTestDir/test000/newFileTest45&quot;);
             * counter = 17;
             * retValue = inputStream.read(byteArray2);
             * counter = 18;
             * if (retValue != 10)
             *     throw new TestFailedException(&quot;retValue: &quot; + retValue);
             * counter = 19;
             * for (int i = 0; i &lt; retValue; ++i)
             * {
             *     if (byteArray2[i] != 76)
             *         throw new TestFailedException();
             * }
             * counter = 20;
             * inputStream.close();
             *
             * //Testing a Connector.openOutputStream() operation.
             * OutputStream outputStream = Connector.openOutputStream(&quot;file:///&quot; + iRoot
             *         + &quot;mainTestDir/test000/newFileTest45&quot;);
             * counter = 21;
             * for (int i = 0; i &lt; byteArray1.length; ++i)
             * {
             *     byteArray1[i] = 77;
             * }
             * counter = 22;
             * outputStream.write(byteArray1);
             * counter = 23;
             * outputStream.close();
             * counter = 24;
             *
             * inputStream = Connector.openDataInputStream(&quot;file:///&quot; + iRoot
             *         + &quot;mainTestDir/test000/newFileTest45&quot;);
             * counter = 25;
             * retValue = inputStream.read(byteArray2);
             * counter = 26;
             * if (retValue != 10)
             *     throw new TestFailedException(&quot;retValue: &quot; + retValue);
             * counter = 27;
             * for (int i = 0; i &lt; retValue; ++i)
             * {
             *     if (byteArray2[i] != 77)
             *         throw new TestFailedException();
             * }
             * counter = 28;
             * inputStream.close();
             * </pre>
             */
            assertTrue(" run_0060() ok ", true);
        }
        catch (Exception ex)
        {
            append(ex.toString());
            append("TEST FAILED!!!: run_0060(): counter: " + counter + "\n");
            assertTrue(" run_0060() FAILED!!! Counter: "+counter, false);
            return;
        }
    }// end run_0060()

}