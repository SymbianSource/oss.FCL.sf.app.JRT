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
import com.nokia.mj.impl.installer.utils.InstallerMain;

public class TestFile_DataOutputStream extends FileTestBase
{
    public TestFile_DataOutputStream()
    {
    }

    public TestFile_DataOutputStream(String aTestName, TestMethod aTestMethod)
    {
        super(aTestName, aTestMethod);
    }

    public void assertFalse(String aMsg, boolean aCondition)
    {
        assertTrue(aMsg, !aCondition);
    }

    public static void registerTests(TestSuite suite)
    {
        suite.addTest(new TestFile_DataOutputStream("runDataTypeWrite",
                      new TestMethod()
        {
            public void run(TestCase tc)
            {
                ((TestFile_DataOutputStream) tc).runDataTypeWrite();
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

    public void runDataTypeWrite()
    {
        boolean passed = true;
        try
        {

            FileConnection conn = (FileConnection) Connector
                                  .open("file:///C:/Data/Images/dosTestFile.txt");
            if (conn.exists())
            {
                conn.delete();

            }
            conn.create();
            DataOutputStream dos = conn.openDataOutputStream();

            final boolean booleanToWrite = true;
            final byte byteToWrite = 65;
            final short shortToWrite = 12365;
            final int intToWrite = 65535;
            final long longToWrite = 1223372036854775808l;
            final char charToWrite = 'a';
            final String charsToWrite = new String("writeChars");
            final String utfToWrite = new String("writeUTF");

            boolean booleanRead = false;
            byte byteRead = 0;
            short shortRead = 0;
            int intRead = 0;
            long longRead = 0;
            char charRead = 'b';
            String charsRead = null;
            String utfRead = null;

            dos.writeBoolean(booleanToWrite);
            dos.writeByte(byteToWrite);
            dos.writeShort(shortToWrite);
            dos.writeInt(intToWrite);
            dos.writeLong(longToWrite);
            dos.writeChar(charToWrite);
            dos.writeUTF(utfToWrite);
            dos.flush();

            DataInputStream dis = conn.openDataInputStream();
            booleanRead = dis.readBoolean();
            byteRead = dis.readByte();
            shortRead = dis.readShort();
            intRead = dis.readInt();
            longRead = dis.readLong();
            charRead = dis.readChar();
            utfRead = dis.readUTF();

            if (booleanRead != booleanToWrite)
            {
                System.out.println("boolean read: " + booleanRead + " is not "
                                   + "equal to written one: " + booleanToWrite);
                passed = false;
            }

            if (byteRead != byteToWrite)
            {
                System.out.println("byte read: " + byteRead + " is not "
                                   + "equal to written one: " + byteToWrite);
                passed = false;
            }

            if (shortRead != shortToWrite)
            {
                System.out.println("short read: " + shortRead + " is not "
                                   + "equal to written one: " + shortToWrite);
                passed = false;
            }

            if (intRead != intToWrite)
            {
                System.out.println("int read: " + intRead + " is not "
                                   + "equal to written one: " + intToWrite);
                passed = false;
            }

            if (longRead != longToWrite)
            {
                System.out.println("long read: " + longRead + " is not "
                                   + "equal to written one: " + longToWrite);
                passed = false;
            }

            if (charRead != charToWrite)
            {
                System.out.println("char read: " + charRead + " is not "
                                   + "equal to written one: " + charToWrite);
                passed = false;
            }

            if (!utfRead.equals(utfToWrite))
            {
                System.out.println("string read (readUTF): " + utfRead
                                   + " is not equal to written one: " + utfToWrite);
                passed = false;
            }

            System.out.println("Passed: "+passed);
            assertTrue("DateInputStreamRead failed", passed);

        }
        catch (Exception ex)
        {
            append(ex.toString());
        }
    }//end run_0010()
}