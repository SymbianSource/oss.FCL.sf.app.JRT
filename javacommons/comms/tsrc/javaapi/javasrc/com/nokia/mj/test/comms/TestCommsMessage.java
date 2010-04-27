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


package com.nokia.mj.test.comms;

import j2meunit.framework.Test;
import j2meunit.framework.TestCase;
import j2meunit.framework.TestSuite;
import j2meunit.framework.TestMethod;

import com.nokia.mj.impl.comms.CommsMessage;
import com.nokia.mj.impl.comms.CommsPermission;

public class TestCommsMessage extends TestCase
{

    public TestCommsMessage()
    {
    }

    public TestCommsMessage(String sTestName, TestMethod rTestMethod)
    {
        super(sTestName, rTestMethod);
    }

    public Test suite()
    {
        TestSuite aSuite = new TestSuite();

        aSuite.addTest(new TestCommsMessage("testGetSetHeaderFields", new TestMethod()
        {
            public void run(TestCase tc)
            {
                ((TestCommsMessage) tc).testGetSetHeaderFields();
            }
        }));

        aSuite.addTest(new TestCommsMessage("testReadWrite", new TestMethod()
        {
            public void run(TestCase tc)
            {
                ((TestCommsMessage) tc).testReadWrite();
            }
        }));

        aSuite.addTest(new TestCommsMessage("testEmptyMessage", new TestMethod()
        {
            public void run(TestCase tc)
            {
                ((TestCommsMessage) tc).testEmptyMessage();
            }
        }));

        aSuite.addTest(new TestCommsMessage("testIllegalArguments", new TestMethod()
        {
            public void run(TestCase tc)
            {
                ((TestCommsMessage) tc).testIllegalArguments();
            }
        }));

        aSuite.addTest(new TestCommsMessage("testHasPermission", new TestMethod()
        {
            public void run(TestCase tc)
            {
                ((TestCommsMessage) tc).testHasPermission();
            }
        }));

        return aSuite;
    }

    public void testGetSetHeaderFields()
    {
        System.out.println("TestCommsMessage.testGetSetHeaderFields()");

        CommsMessage msg = new CommsMessage();
        msg.setMessageId(100200);
        msg.setMessageRef(100201);
        msg.setModuleId(100202);
        CommsMessage msg2 = new CommsMessage(msg.toByteArray());

        // check headers
        assertEquals(msg.getMessageId(), msg2.getMessageId());
        assertEquals(msg.getMessageRef(), msg2.getMessageRef());
        assertEquals(msg.getModuleId(), msg2.getModuleId());
        assertEquals(msg.getReceiver(), msg2.getReceiver());
        assertEquals(msg.getSender(), msg2.getSender());

        CommsMessage msg3 = new CommsMessage();
        msg3.replyTo(msg);
        assertEquals(msg.getMessageId(), msg3.getMessageId());
        assertEquals(msg.getMessageRef(), msg3.getMessageRef());
        assertEquals(msg.getModuleId(), msg3.getModuleId());
        assertEquals(msg.getSender(), msg3.getReceiver());
        assertEquals(msg.getReceiver(), msg3.getSender());
    }

    public void testReadWrite()
    {
        System.out.println("TestCommsMessage.testReadWrite()");

        CommsMessage msg = new CommsMessage();
        msg.setMessageId(0);
        msg.setMessageRef(0xFFFFFFFF);
        msg.setModuleId(0xDEADBEEF);

        msg.write(666);
        msg.write("hello world");
        byte[] bytes = "\r\n \n\n\r\r21314".getBytes();
        msg.write(bytes);

        CommsMessage msg2 = new CommsMessage(msg.toByteArray());
        int i = msg2.readInt();
        String s = msg2.readString();
        byte[] b = msg2.readBytes();

        // check headers
        assertEquals(msg.getMessageId(), msg2.getMessageId());
        assertEquals(msg.getMessageRef(), msg2.getMessageRef());
        assertEquals(msg.getModuleId(), msg2.getModuleId());
        assertEquals(msg.getReceiver(), msg2.getReceiver());
        assertEquals(msg.getSender(), msg2.getSender());

        // check payload
        assertEquals(i, 666);
        assertEquals(s.compareTo("hello world"), 0);
        assertEquals(b.length, bytes.length);
        for (int j = 0; j < b.length; j++)
        {
            assertEquals(b[j], bytes[j]);
        }
    }

    public void testEmptyMessage()
    {
        System.out.println("TestCommsMessage.testEmptyMessage()");

        CommsMessage msg = new CommsMessage();

        assertEquals(msg.getMessageId(), 0);
        assertEquals(msg.getMessageRef(), 0);
        assertEquals(msg.getModuleId(), 0);
        assertEquals(msg.getReceiver(), 0);
        assertEquals(msg.getSender(), 0);

        int i = msg.readInt();
        String s = msg.readString();
        byte[] b = msg.readBytes();

        assertEquals(i, 0);
        assertEquals(s.compareTo(""), 0);
        assertEquals(b.length, 0);

        // copy empty message
        CommsMessage msg2 = new CommsMessage(msg.toByteArray());
        i = msg2.readInt();
        s = msg2.readString();
        b = msg2.readBytes();

        assertEquals(msg2.getMessageId(), 0);
        assertEquals(msg2.getMessageRef(), 0);
        assertEquals(msg2.getModuleId(), 0);
        assertEquals(msg2.getReceiver(), 0);
        assertEquals(msg2.getSender(), 0);

        assertEquals(i, 0);
        assertEquals(s.compareTo(""), 0);
        assertEquals(b.length, 0);
    }

    public void testIllegalArguments()
    {
        System.out.println("TestCommsMessage.testIllegalArguments()");

        CommsMessage msg = new CommsMessage();

        // write nulls
        String s = null;
        byte[] b = null;

        try
        {
            msg.write(s);
            fail("null accepted in write(string)");
        }
        catch (IllegalArgumentException e) {}

        try
        {
            msg.write(b);
            fail("null accepted in write(byte[])");
        }
        catch (IllegalArgumentException e) {}

        try
        {
            CommsMessage msg2 = new CommsMessage(b);
            fail("null accepted in ctor");
        }
        catch (IllegalArgumentException e) {}
    }

    public void testHasPermission()
    {
        System.out.println("TestCommsMessage.testHasPermission()");

        CommsMessage msg = new CommsMessage();

        assertTrue(!msg.hasPermission(CommsPermission.MANAGE_CERTIFICATES));
        assertTrue(!msg.hasPermission(CommsPermission.INSTALL_APPLICATION));
        assertTrue(!msg.hasPermission(CommsPermission.LAUNCH_APPLICATION));
        assertTrue(!msg.hasPermission(CommsPermission.STOP_APPLICATION));
    }

}
