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
import com.nokia.mj.impl.comms.CommsEndpoint;
import com.nokia.mj.impl.comms.CommsListener;
import com.nokia.mj.impl.comms.exception.CommsException;

import java.io.IOException;

public class TestCommsEndpoint extends TestCase
{
    private final int MODULE_ID_A           = 900;
    private final int MODULE_ID_B           = 901;
    private final int MODULE_ID_DATA_CHECK  = 902;
    private final int MODULE_ID_NO_REPLY    = 903;      // no reply is sent
    private final int MODULE_ID_SLEEP_1S    = 1000;     // reply msg is delayed by 1s
    private final int MODULE_ID_SLEEP_5S    = 5000;
    private final int MODULE_ID_SLEEP_10S   = 10000;

    private final int PLUGIN_ID_JAVACAPTAIN_COMMS_TESTER_C = 101; // see comms.h

    private final int SERVER_ADDRESS = CommsEndpoint.JAVA_CAPTAIN;
    private final String SERVER_NAME = "InstallerJavaCaptain";

    private final int TIMEOUT = 10;
    public TestCommsEndpoint()
    {
    }

    public TestCommsEndpoint(String sTestName, TestMethod rTestMethod)
    {
        super(sTestName, rTestMethod);
    }

    public Test suite()
    {
        TestSuite aSuite = new TestSuite();

        aSuite.addTest(new TestCommsEndpoint("testLoadCommsExtensionPlugin", new TestMethod()
        {
            public void run(TestCase tc)
            {
                ((TestCommsEndpoint) tc).testLoadCommsExtensionPlugin();
            }
        }));

        aSuite.addTest(new TestCommsEndpoint("testConstructor", new TestMethod()
        {
            public void run(TestCase tc)
            {
                ((TestCommsEndpoint) tc).testConstructor();
            }
        }));

        aSuite.addTest(new TestCommsEndpoint("testFind", new TestMethod()
        {
            public void run(TestCase tc)
            {
                ((TestCommsEndpoint) tc).testFind();
            }
        }));

        aSuite.addTest(new TestCommsEndpoint("testRegisterListener", new TestMethod()
        {
            public void run(TestCase tc)
            {
                ((TestCommsEndpoint) tc).testRegisterListener();
            }
        }));

        aSuite.addTest(new TestCommsEndpoint("testNoUnregisterListener", new TestMethod()
        {
            public void run(TestCase tc)
            {
                ((TestCommsEndpoint) tc).testNoUnregisterListener();
            }
        }));

        aSuite.addTest(new TestCommsEndpoint("testRegisterListenerInListener", new TestMethod()
        {
            public void run(TestCase tc)
            {
                ((TestCommsEndpoint) tc).testRegisterListenerInListener();
            }
        }));

        aSuite.addTest(new TestCommsEndpoint("testSend", new TestMethod()
        {
            public void run(TestCase tc)
            {
                ((TestCommsEndpoint) tc).testSend();
            }
        }));

        aSuite.addTest(new TestCommsEndpoint("testSendReceive", new TestMethod()
        {
            public void run(TestCase tc)
            {
                ((TestCommsEndpoint) tc).testSendReceive();
            }
        }));

        aSuite.addTest(new TestCommsEndpoint("testSendReceiveWithWaitForEver", new TestMethod()
        {
            public void run(TestCase tc)
            {
                ((TestCommsEndpoint) tc).testSendReceiveWithWaitForEver();
            }
        }));

        aSuite.addTest(new TestCommsEndpoint("testSendReceiveBigTimeout", new TestMethod()
        {
            public void run(TestCase tc)
            {
                ((TestCommsEndpoint) tc).testSendReceiveBigTimeout();
            }
        }));

        aSuite.addTest(new TestCommsEndpoint("testSendReceiveTimeout", new TestMethod()
        {
            public void run(TestCase tc)
            {
                ((TestCommsEndpoint) tc).testSendReceiveTimeout();
            }
        }));

        aSuite.addTest(new TestCommsEndpoint("testSendReceiveMultiThread", new TestMethod()
        {
            public void run(TestCase tc)
            {
                ((TestCommsEndpoint) tc).testSendReceiveMultiThread();
            }
        }));

        aSuite.addTest(new TestCommsEndpoint("testExitWhileSendReceivePending", new TestMethod()
        {
            public void run(TestCase tc)
            {
                ((TestCommsEndpoint) tc).testExitWhileSendReceivePending();
            }
        }));

        aSuite.addTest(new TestCommsEndpoint("testSendAndSendReceiveMultiThread", new TestMethod()
        {
            public void run(TestCase tc)
            {
                ((TestCommsEndpoint) tc).testSendAndSendReceiveMultiThread();
            }
        }));

        aSuite.addTest(new TestCommsEndpoint("testSendWhenNotConnected", new TestMethod()
        {
            public void run(TestCase tc)
            {
                ((TestCommsEndpoint) tc).testSendWhenNotConnected();
            }
        }));

        aSuite.addTest(new TestCommsEndpoint("testCheckMessageInListener", new TestMethod()
        {
            public void run(TestCase tc)
            {
                ((TestCommsEndpoint) tc).testCheckMessageInListener();
            }
        }));

        aSuite.addTest(new TestCommsEndpoint("testCheckNativePayload", new TestMethod()
        {
            public void run(TestCase tc)
            {
                ((TestCommsEndpoint) tc).testCheckNativePayload();
            }
        }));

        aSuite.addTest(new TestCommsEndpoint("testDestroy", new TestMethod()
        {
            public void run(TestCase tc)
            {
                ((TestCommsEndpoint) tc).testDestroy();
            }
        }));

        return aSuite;
    }

    public void assertEquals(byte[] a, byte[] b)
    {
        assertEquals(a.length, b.length);
        for (int i = 0; i < a.length; i++)
        {
            assertEquals(a[i], b[i]);
        }
    }

    public void testLoadCommsExtensionPlugin()
    {
        System.out.println("TestCommsEndpoint.testLoadCommsExtensionPlugin()");
        CommsEndpoint comms = null;
        try
        {
            comms = new CommsEndpoint();
            comms.connect(SERVER_ADDRESS);
            // loads comms extension plugin in JavaCaptain
            CommsMessage message = new CommsMessage();
            message.setModuleId(PLUGIN_ID_JAVACAPTAIN_COMMS_TESTER_C);
            CommsMessage msg = comms.sendReceive(message, TIMEOUT);
        }
        catch (Exception e)
        {
            String error = "\n\nError !!!!!!!!!!!!!!!!!!!!!!\n\n"+
                           "Loading comms extension plugin failed.\n"+
                           "Check that comms extension plugin is built (make).\n"+
                           "This failure causes other tests to fail also.\n\n";
            System.out.println(error);
            fail(error);
        }
        finally
        {
            comms.destroy();
        }
    }

    public void testConstructor()
    {
        System.out.println("TestCommsEndpoint.testConstructor()");

        CommsEndpoint comms = null;
        // ok case
        try
        {
            comms = new CommsEndpoint();
            comms.connect(SERVER_ADDRESS);
            comms.disconnect();
        }
        catch (CommsException e)
        {
            fail("ctor(SERVER_NAME) failed");
        }
        finally
        {
            comms.destroy();
            comms = null;
        }

        // multiple endpoints
        CommsEndpoint comms1 = null;
        CommsEndpoint comms2 = null;
        CommsEndpoint comms3 = null;
        try
        {
            comms1 = new CommsEndpoint();
            comms2 = new CommsEndpoint();
            comms3 = new CommsEndpoint();
            comms1.connect(SERVER_ADDRESS);
            comms2.connect(SERVER_ADDRESS);
            comms3.connect(SERVER_ADDRESS);
        }
        catch (CommsException e)
        {
            fail(" multiple endpoints failed");
        }
        finally
        {
            comms1.destroy();
            comms2.destroy();
            comms3.destroy();
        }

        // multiple connect/disconnect calls
        try
        {
            comms = new CommsEndpoint();
            comms.disconnect();

            comms.connect(SERVER_ADDRESS);
            comms.connect(SERVER_ADDRESS);

            comms.disconnect();
            comms.disconnect();
        }
        catch (CommsException e)
        {
            fail("multiple start/stop calls failed");
        }
        finally
        {
            comms.destroy();
            comms = null;
        }

        // connect fails
        try
        {
            comms = new CommsEndpoint();
            comms.connect(666313);
            fail("connect to non-existing server did not fail");
        }
        catch (CommsException e)
        {
        }
        finally
        {
            comms.destroy();
            comms = null;
        }
    }

    public void testFind()
    {
        System.out.println("TestCommsEndpoint.testFind()");

        CommsEndpoint comms = null;

        // ok case
        try
        {
            comms = CommsEndpoint.find(SERVER_NAME);
            assertNotNull(comms);
            comms.send(new CommsMessage());
        }
        catch (CommsException e)
        {
            fail("ok case");
        }
        finally
        {
            comms.destroy();
            comms = null;
        }

        // not existing
        comms = CommsEndpoint.find("not existing");
        assertEquals(comms, null);
        comms = null;

        // multiple endpoints
        CommsEndpoint comms1 = null;
        CommsEndpoint comms2 = null;
        CommsEndpoint comms3 = null;
        try
        {
            comms1 = CommsEndpoint.find(SERVER_NAME);
            comms2 = CommsEndpoint.find(SERVER_NAME);
            comms3 = CommsEndpoint.find(SERVER_NAME);

            assertNotNull(comms1);
            assertNotNull(comms2);
            assertNotNull(comms3);

            comms1.send(new CommsMessage());
            comms2.send(new CommsMessage());
            comms3.send(new CommsMessage());
        }
        catch (CommsException e)
        {
            fail(" multiple endpoints failed");
        }
        finally
        {
            comms1.destroy();
            comms2.destroy();
            comms3.destroy();
        }
    }

    private void testRegisterListener()
    {
        System.out.println("TestCommsEndpoint.testRegisterListener()");

        class TestListener implements CommsListener
        {
            public void processMessage(CommsMessage message) {}
        }

        CommsEndpoint comms = null;
        try
        {
            comms = new CommsEndpoint();

            // register listener
            try
            {
                comms.registerListener(MODULE_ID_A, new TestListener());
                comms.unregisterListener(MODULE_ID_A);
            }
            catch (CommsException e)
            {
                fail("register listener");
            }

            // register multiple listeners
            try
            {
                int i = 100;
                for (; i < 300; i++)
                {
                    comms.registerListener(i, new TestListener());
                }
                for (i--; i > 99; i--)
                {
                    comms.unregisterListener(i);
                }
            }
            catch (CommsException e)
            {
                fail("register multiple listeners");
            }

            // listener already exists
            try
            {
                comms.registerListener(MODULE_ID_A, new TestListener());
                comms.registerListener(MODULE_ID_A, new TestListener());
                fail("registerListener did not fail");
            }
            catch (CommsException e) {}

            // listener doesn't exists
            try
            {
                comms.unregisterListener(MODULE_ID_A);
                comms.unregisterListener(MODULE_ID_A);
                fail("unregisterListener did not fail");
            }
            catch (CommsException e) {}

            // listener null
            try
            {
                comms.registerListener(MODULE_ID_A, null);
                fail("registerListener did not fail with null");
            }
            catch (Exception e) {}

            // register default listener
            try
            {
                comms.registerDefaultListener(new TestListener());
                comms.unregisterDefaultListener();
            }
            catch (Exception e)
            {
                fail("registerDefaultListener");
            }

            // default listener already exists
            try
            {
                comms.registerDefaultListener(new TestListener());
                comms.registerDefaultListener(new TestListener());
                fail("registerListener did not fail");
            }
            catch (CommsException e) {}

            // default listener doesn't exists
            try
            {
                comms.unregisterDefaultListener();
                comms.unregisterDefaultListener();
                fail("unregisterDefaultListener did not fail");
            }
            catch (CommsException e) {}

            // default listener null
            try
            {
                comms.registerDefaultListener(null);
                fail("registerDefaultListener did not fail with null");
            }
            catch (Exception e) {}

        }
        finally
        {
            comms.destroy();
        }
    }

    private void testNoUnregisterListener()
    {
        System.out.println("TestCommsEndpoint.testNoUnregisterListener()");

        class TestListener implements CommsListener
        {
            public void processMessage(CommsMessage message) {}
        }

        CommsEndpoint comms = null;
        try
        {
            comms = new CommsEndpoint();
            comms.connect(SERVER_ADDRESS);

            // register multiple listeners but no unregister
            try
            {
                comms.registerDefaultListener(new TestListener());
                for (int i = 0; i < 300; i++)
                {
                    comms.registerListener(i, new TestListener());
                }
            }
            catch (CommsException e)
            {
                fail("register multiple listeners");
            }

        }
        finally
        {
            comms.destroy();
        }
    }

    public void testRegisterListenerInListener()
    {
        System.out.println("TestCommsEndpoint.testRegisterListenerInListener()");

        CommsEndpoint comms = null;
        try
        {
            comms = new CommsEndpoint();
            comms.connect(SERVER_ADDRESS);

            class TestListener implements CommsListener
            {
                private CommsEndpoint comms = null;
                public boolean pass = false;

                public TestListener(CommsEndpoint comms)
                {
                    this.comms = comms;
                }

                public void processMessage(CommsMessage message)
                {
                    try
                    {
                        comms.registerListener(MODULE_ID_B, this);
                        comms.unregisterListener(MODULE_ID_B);
                        pass = true;
                    }
                    catch (CommsException e)
                    {
                        e.printStackTrace();
                        fail("processMessage");
                    }
                }
            }
            TestListener listener = new TestListener(comms);
            comms.registerListener(MODULE_ID_A, listener);

            CommsMessage message = new CommsMessage();
            message.setMessageId(2);
            message.setModuleId(MODULE_ID_A);

            comms.send(message);
            Thread.sleep(300); // give time to receive message
            comms.unregisterListener(MODULE_ID_A);
            assertTrue(listener.pass);
        }
        catch (Exception e)
        {
            e.printStackTrace();
            fail("testRegisterListenerInListener");
        }
        finally
        {
            comms.destroy();
        }
    }

    public void testSend()
    {
        System.out.println("TestCommsEndpoint.testSend()");

        CommsEndpoint comms = null;
        try
        {
            comms = new CommsEndpoint();
            comms.connect(SERVER_ADDRESS);

            CommsMessage message = new CommsMessage();
            message.setMessageId(888);
            message.setModuleId(MODULE_ID_A);
            message.write("hello world");

            // normal case
            comms.send(message);

            // empty message
            comms.send(new CommsMessage());

            // null message
            try
            {
                comms.send(null);
                fail("send did not fail with null");
            }
            catch (IllegalArgumentException e) {}

            // large message
            CommsMessage bigOne = new CommsMessage();
            bigOne.setMessageId(888);
            bigOne.setModuleId(MODULE_ID_A);
            bigOne.write(new byte [50000]);
            comms.send(message);

        }
        catch (Exception e)
        {
            e.printStackTrace();
            fail("testSend failed");
        }
        finally
        {
            comms.destroy();
        }
    }

    public void testSendReceive()
    {
        System.out.println("TestCommsEndpoint.testSendReceive()");

        CommsEndpoint comms = null;
        try
        {
            comms = new CommsEndpoint();
            comms.connect(SERVER_ADDRESS);

            CommsMessage message = new CommsMessage();
            message.setMessageId(888);
            message.setModuleId(MODULE_ID_A);
            byte[] bytes = "abcdefghijklmnopqrstyvwxyz".getBytes();
            message.write(bytes);

            // normal case
            CommsMessage msg = comms.sendReceive(message, TIMEOUT);
            assertEquals(msg.getMessageId(), message.getMessageId());
            assertEquals(msg.getModuleId(), message.getModuleId());
            byte[] b = msg.readBytes();
            assertEquals(b, bytes);

            // empty message
            CommsMessage empty = new CommsMessage();
            empty.setModuleId(MODULE_ID_A); // module id needs to be set so that reply will be sent
            CommsMessage msg2 = comms.sendReceive(empty, TIMEOUT);
            assertNotNull(msg2);
            assertEquals(msg2.getMessageId(), empty.getMessageId());
            assertEquals(msg2.getModuleId(), empty.getModuleId());

            // null message
            try
            {
                comms.sendReceive(null, TIMEOUT);
                fail("sendReceive did not fail with null");
            }
            catch (IllegalArgumentException e) {}

            // large message
            CommsMessage bigOne = new CommsMessage();
            bigOne.setMessageId(888);
            bigOne.setModuleId(MODULE_ID_A);
            byte[] array = new byte [50000];
            for (int i = 0; i < array.length; i++)
            {
                array[i] = (byte)i;
            }
            bigOne.write(array);
            CommsMessage msg3 = comms.sendReceive(bigOne, TIMEOUT);
            assertEquals(msg3.getMessageId(), bigOne.getMessageId());
            assertEquals(msg3.getModuleId(), bigOne.getModuleId());
            byte[] big = msg3.readBytes();
            assertEquals(big, array);

            // Unicode string
            StringBuffer sb = new StringBuffer();
            sb.append('\u00a5'); // Japanese Yen symbol
            sb.append('\u01FC'); // Roman AE with acute accent
            sb.append('\u0391'); // Greek Capital Alpha
            sb.append('\u03A9'); // Greek Capital Omega
            sb.append('\u20ac'); // Euro symbol
            sb.append('\u20a8'); // Rupee symbol
            sb.append('\u040F'); // Cyrillic capital letter DZHE
            sb.append('\u062A'); // Arabic letter TEH

            String in = sb.toString();
            message = new CommsMessage();
            message.setModuleId(MODULE_ID_A);
            message.write(in);
            msg = comms.sendReceive(message, TIMEOUT);
            String out = msg.readString();
            assertEquals(in, out);

        }
        catch (Exception e)
        {
            e.printStackTrace();
            fail("testSendReceive failed");
        }
        finally
        {
            comms.destroy();
        }
    }

    public void testSendReceiveWithWaitForEver()
    {
        System.out.println("TestCommsEndpoint.testSendReceiveWithWaitForEver()");

        CommsEndpoint comms = null;
        try
        {
            comms = new CommsEndpoint();
            comms.connect(SERVER_ADDRESS);

            CommsMessage message = new CommsMessage();
            message.setMessageId(888);
            message.setModuleId(MODULE_ID_A);
            byte[] bytes = "abcdefghijklmnopqrstyvwxyz".getBytes();
            message.write(bytes);

            // normal case
            CommsMessage msg = comms.sendReceive(message, CommsEndpoint.WAIT_FOR_EVER);
            assertEquals(msg.getMessageId(), message.getMessageId());
            assertEquals(msg.getModuleId(), message.getModuleId());
            byte[] b = msg.readBytes();
            assertEquals(b, bytes);

            // empty message
            CommsMessage empty = new CommsMessage();
            empty.setModuleId(MODULE_ID_A); // module id needs to be set so that reply will be sent
            CommsMessage msg2 = comms.sendReceive(empty, CommsEndpoint.WAIT_FOR_EVER);
            assertNotNull(msg2);
            assertEquals(msg2.getMessageId(), empty.getMessageId());
            assertEquals(msg2.getModuleId(), empty.getModuleId());

            // null message
            try
            {
                comms.sendReceive(null, CommsEndpoint.WAIT_FOR_EVER);
                fail("sendReceive did not fail with null");
            }
            catch (IllegalArgumentException e) {}

            // large message
            CommsMessage bigOne = new CommsMessage();
            bigOne.setMessageId(888);
            bigOne.setModuleId(MODULE_ID_A);
            byte[] array = new byte [50000];
            for (int i = 0; i < array.length; i++)
            {
                array[i] = (byte)i;
            }
            bigOne.write(array);
            CommsMessage msg3 = comms.sendReceive(bigOne, CommsEndpoint.WAIT_FOR_EVER);
            assertEquals(msg3.getMessageId(), bigOne.getMessageId());
            assertEquals(msg3.getModuleId(), bigOne.getModuleId());
            byte[] big = msg3.readBytes();
            assertEquals(big, array);

        }
        catch (Exception e)
        {
            e.printStackTrace();
            fail("testSendReceiveWithWaitForEver failed");
        }
        finally
        {
            comms.destroy();
        }
    }

    public void testSendReceiveBigTimeout()
    {
        System.out.println("TestCommsEndpoint.testSendReceiveBigTimeout()");

        CommsEndpoint comms = null;
        try
        {
            comms = new CommsEndpoint();
            comms.connect(SERVER_ADDRESS);

            CommsMessage message = new CommsMessage();
            message.setMessageId(888);
            message.setModuleId(MODULE_ID_A);
            byte[] bytes = "abcdefghijklmnopqrstyvwxyz".getBytes();
            message.write(bytes);

            // OpenC timeout limit
            CommsMessage msg = comms.sendReceive(message, 2148);
            assertEquals(msg.getMessageId(), message.getMessageId());
            assertEquals(msg.getModuleId(), message.getModuleId());
            byte[] b = msg.readBytes();
            assertEquals(b, bytes);

            // 1h
            msg = comms.sendReceive(message, 3600);
            assertEquals(msg.getMessageId(), message.getMessageId());
            assertEquals(msg.getModuleId(), message.getModuleId());
            b = msg.readBytes();
            assertEquals(b, bytes);

            // 24h
            msg = comms.sendReceive(message, 86400);
            assertEquals(msg.getMessageId(), message.getMessageId());
            assertEquals(msg.getModuleId(), message.getModuleId());
            b = msg.readBytes();
            assertEquals(b, bytes);

        }
        catch (Exception e)
        {
            e.printStackTrace();
            fail("testSendReceiveBigTimeout failed");
        }
        finally
        {
            comms.destroy();
        }
    }


    public void testSendReceiveTimeout()
    {
        System.out.println("TestCommsEndpoint.testSendReceiveTimeout()");
        CommsEndpoint comms = null;
        try
        {
            comms = new CommsEndpoint();
            comms.connect(SERVER_ADDRESS);

            CommsMessage message = new CommsMessage();
            message.setMessageId(888);
            message.write("hello world");

            try
            {
                message.setModuleId(MODULE_ID_NO_REPLY);
                comms.sendReceive(message, 1);
                fail("sendReceive did not timeout (no reply)");
            }
            catch (CommsException ie) { }

            try
            {
                message.setModuleId(MODULE_ID_SLEEP_5S);
                comms.sendReceive(message, 3);
                fail("sendReceive did not timeout (5s)");
            }
            catch (CommsException ie) { }

            try
            {
                message.setModuleId(MODULE_ID_SLEEP_10S);
                comms.sendReceive(message, 8);
                fail("sendReceive did not timeout (10s)");
            }
            catch (CommsException ie) { }

        }
        catch (Exception e)
        {
            e.printStackTrace();
            fail("testSendReceiveTimeout");
        }
        finally
        {
            comms.destroy();
        }
    }

    private void testSendReceiveMultiThread()
    {
        System.out.println("TestCommsEndpoint.testSendReceiveMultiThread()");

        CommsEndpoint comms = null;
        try
        {
            comms = new CommsEndpoint();
            comms.connect(SERVER_ADDRESS);

            class Sender extends Thread
            {
                private CommsEndpoint comms = null;
                public int sendCount = 0;

                public Sender(CommsEndpoint comms)
                {
                    this.comms = comms;
                }

                public void run()
                {
                    for (int i=0; i<5; i++)
                    {
                        try
                        {
                            Thread.sleep((100));
                        }
                        catch (InterruptedException e) { }

                        try
                        {
                            CommsMessage message = new CommsMessage();
                            message.setMessageId(2);
                            message.setModuleId(MODULE_ID_A);
                            comms.sendReceive(message, TIMEOUT);
                            comms.sendReceive(message, CommsEndpoint.WAIT_FOR_EVER);
                            sendCount++;
                        }
                        catch (Exception e)
                        {
                            e.printStackTrace();
                            fail("run");
                        }
                    }
                }
            }

            Sender t1 = new Sender(comms);
            Sender t2 = new Sender(comms);
            Sender t3 = new Sender(comms);

            t1.start();
            t2.start();
            t3.start();
            t1.join();
            t2.join();
            t3.join();

            assertEquals(t1.sendCount, 5);
            assertEquals(t2.sendCount, 5);
            assertEquals(t3.sendCount, 5);

        }
        catch (Exception e)
        {
            e.printStackTrace();
            fail("testSendReceiveMultiThread");
        }
        finally
        {
            comms.destroy();
        }
    }

    private void testExitWhileSendReceivePending()
    {
        System.out.println("TestCommsEndpoint.testExitWhileSendReceivePending()");

        CommsEndpoint comms = null;
        try
        {
            comms = new CommsEndpoint();
            comms.connect(SERVER_ADDRESS);

            class Sender extends Thread
            {
                private CommsEndpoint comms = null;
                private int timeout = 0;
                public Sender(CommsEndpoint comms, int aTimeout)
                {
                    this.comms = comms;
                    timeout = aTimeout;
                }
                public void run()
                {
                    try
                    {
                        CommsMessage message = new CommsMessage();
                        message.setModuleId(MODULE_ID_NO_REPLY);
                        comms.sendReceive(message, timeout);
                    }
                    catch (CommsException ce)
                    {
                    }
                }
            }

            Sender t1 = new Sender(comms, CommsEndpoint.WAIT_FOR_EVER);
            Sender t2 = new Sender(comms, TIMEOUT);
            Sender t3 = new Sender(comms, CommsEndpoint.WAIT_FOR_EVER);
            Sender t4 = new Sender(comms, TIMEOUT);

            t1.start();
            t2.start();
            t3.start();
            t4.start();
            Thread.sleep(300);
            // exit while there is pending sendReceive threads
            comms.destroy();
            t1.join();
            t2.join();
            t3.join();
            t4.join();

        }
        catch (Exception e)
        {
            e.printStackTrace();
            fail("testExitWhileSendReceivePending");
        }
        finally
        {
            comms.destroy();
        }
    }

    private void testSendAndSendReceiveMultiThread()
    {
        System.out.println("TestCommsEndpoint.testSendAndSendReceiveMultiThread()");
        CommsEndpoint comms = null;
        try
        {
            class TestListener implements CommsListener
            {
                public void processMessage(CommsMessage message) { }
            }

            class Sender extends Thread
            {
                private CommsEndpoint comms = null;
                private boolean doSendReceive = false;
                public int sendCount = 0;

                public Sender(CommsEndpoint comms, boolean doSendReceive)
                {
                    this.comms = comms;
                    this.doSendReceive = doSendReceive;
                }

                public void run()
                {
                    for (int i=0; i<5; i++)
                    {
                        try
                        {
                            Thread.sleep((100));
                        }
                        catch (InterruptedException e) { }

                        try
                        {
                            CommsMessage message = new CommsMessage();
                            message.setMessageId(2);
                            message.setModuleId(MODULE_ID_A);
                            if (doSendReceive)
                            {
                                comms.sendReceive(message, TIMEOUT);
                                comms.sendReceive(message, CommsEndpoint.WAIT_FOR_EVER);
                            }
                            else
                            {
                                comms.send(message);
                            }
                            sendCount++;
                        }
                        catch (Exception e)
                        {
                            e.printStackTrace();
                            fail("run");
                        }
                    }
                }
            }

            comms = new CommsEndpoint();
            comms.connect(SERVER_ADDRESS);
            comms.registerListener(MODULE_ID_A, new TestListener());

            Sender t0 = new Sender(comms, true);
            Sender t1 = new Sender(comms, true);
            Sender t2 = new Sender(comms, true);
            Sender t3 = new Sender(comms, true);
            Sender t4 = new Sender(comms, true);
            Sender t5 = new Sender(comms, false);
            Sender t6 = new Sender(comms, false);
            Sender t7 = new Sender(comms, false);
            Sender t8 = new Sender(comms, false);
            Sender t9 = new Sender(comms, false);

            t0.start();
            t1.start();
            t2.start();
            t3.start();
            t4.start();
            t5.start();
            t6.start();
            t7.start();
            t8.start();
            t9.start();
            t0.join();
            t1.join();
            t2.join();
            t3.join();
            t4.join();
            t5.join();
            t6.join();
            t7.join();
            t8.join();
            t9.join();

            comms.unregisterListener(MODULE_ID_A);

            assertEquals(t0.sendCount, 5);
            assertEquals(t1.sendCount, 5);
            assertEquals(t2.sendCount, 5);
            assertEquals(t3.sendCount, 5);
            assertEquals(t4.sendCount, 5);
            assertEquals(t5.sendCount, 5);
            assertEquals(t6.sendCount, 5);
            assertEquals(t7.sendCount, 5);
            assertEquals(t8.sendCount, 5);
            assertEquals(t9.sendCount, 5);

        }
        catch (Exception e)
        {
            e.printStackTrace();
            fail("testSendAndSendReceiveMultiThread");
        }
        finally
        {
            comms.destroy();
        }
    }

    public void testSendWhenNotConnected()
    {
        System.out.println("TestCommsEndpoint.testSendWhenNotConnected()");

        CommsEndpoint comms = null;
        try
        {
            comms = new CommsEndpoint();

            // send message
            try
            {
                comms.send(new CommsMessage());
                fail("send did not fail");
            }
            catch (CommsException e) {}

            // send message
            try
            {
                comms.sendReceive(new CommsMessage(), TIMEOUT);
                fail("sendReceive did not fail");
            }
            catch (CommsException e) {}

        }
        catch (Exception e)
        {
            e.printStackTrace();
            fail("testSendWhenNotConnected failed");
        }
        finally
        {
            comms.destroy();
        }
    }


    private void testCheckMessageInListener()
    {
        System.out.println("TestCommsEndpoint.testCheckMessageInListener()");
        CommsEndpoint comms = null;
        try
        {
            class TestListener implements CommsListener
            {
                public boolean pass = false;
                String s = null;
                byte[] b = null;
                public TestListener(String s, byte[] b)
                {
                    this.s = s;
                    this.b = b;
                }

                public void processMessage(CommsMessage message)
                {
                    String str = message.readString();
                    byte[] arr = message.readBytes();

                    assertEquals(str.compareTo(s), 0);
                    assertEquals(arr, b);
                    pass = true;
                }
            }

            String s = "abcdefghijklmnopqrstyvwxyz";
            byte[] array = new byte[1500];
            for (int i=0; i<array.length; i++)
            {
                array[i] = (byte)i;
            }

            comms = new CommsEndpoint();
            comms.connect(SERVER_ADDRESS);
            TestListener listener = new TestListener(s, array);
            comms.registerListener(MODULE_ID_A, listener);

            CommsMessage message = new CommsMessage();
            message.setMessageId(2);
            message.setModuleId(MODULE_ID_A);
            message.write(s);
            message.write(array);

            comms.send(message);
            Thread.sleep(300); // give time to receive message
            comms.unregisterListener(MODULE_ID_A);

            assertTrue(listener.pass);

        }
        catch (Exception e)
        {
            e.printStackTrace();
            fail("testCheckMessageInListener");
        }
        finally
        {
            comms.destroy();
        }
    }

    private void testCheckNativePayload()
    {
        System.out.println("TestCommsEndpoint.testCheckNativePayload()");

        CommsEndpoint comms = null;
        try
        {
            // keep this in sync with native side test case

            comms = new CommsEndpoint();
            comms.connect(SERVER_ADDRESS);

            CommsMessage message = new CommsMessage();

            message.setMessageId(888);
            message.setModuleId(MODULE_ID_DATA_CHECK);

            String input = "abcdefghijklmnopqrstyvwxyz";
            // latin small letter a with diaeresis, latin small letter a with ring above, latin small letter o with diaeresis
            String utf16 = "\u00E4 \u00E5 \u00F6 \u20AC";
            String utf8 = new String(utf16.getBytes("UTF8"), "UTF8");

            // Japanese Yen symbol, Roman AE with acute accent, Greek Capital Alpha, Greek Capital Omega,
            // Euro symbol, Rupee symbol, Cyrillic capital letter DZHE, Arabic letter TEH
            String symbols = "\u00a5\u01FC\u0391\u03A9\u20ac\u20a8\u040F\u062A";

            // devanagari letter a, tibetan digit zero, katakana letter ya
            String letters = "\u0905 \u0F20 \u30E4";

            // check that all byte values go through ok
            // UTF-16 byte-order marks (0xfe, 0xff) do not appear in UTF-8
            byte[] array = new byte[0xff];
            for (int i=0; i< array.length; i++)
            {
                array[i] = (byte)i;
            }

            int in = 0xCAFEBABE;
            int zero = 0;
            int one = 1;
            int max = 0xFFFFFFFF;
            message.write(input);
            message.write(in);
            message.write(zero);
            message.write(one);
            message.write(max);
            message.write(utf8);
            message.write(utf16);
            message.write(symbols);
            message.write(symbols.getBytes("UTF8"));
            message.write(letters);
            message.write(letters.getBytes("UTF8"));
            message.write(array);

            CommsMessage msg = comms.sendReceive(message, 1);

            String output = msg.readString();
            int out = msg.readInt();
            zero = msg.readInt();
            one = msg.readInt();
            max = msg.readInt();
            String letters_out = msg.readString();
            String symbols_out = msg.readString();
            String utf16_out = msg.readString();
            String utf8_out = msg.readString();
            byte[] array_out = msg.readBytes();

            String s = "ABCDEFGHIJKLMNOPQRSTYWXYZ";
            int i = 0xDEADBEEF;

            assertEquals(output.compareTo(s), 0);
            assertEquals(out, i);
            assertEquals(zero, 0);
            assertEquals(one, 1);
            assertEquals(max, 0xFFFFFFFF);
            assertEquals(letters_out.compareTo(letters), 0);
            assertEquals(symbols_out.compareTo(symbols), 0);
            assertEquals(utf16_out.compareTo(utf16), 0);
            assertEquals(utf8_out.compareTo(utf8), 0);
            assertEquals(array_out, array);
        }
        catch (Exception e)
        {
            e.printStackTrace();
            fail("testCheckNativePayload");
        }
        finally
        {
            comms.destroy();
        }
    }


    public void testDestroy()
    {
        System.out.println("TestCommsEndpoint.testDestroy()");
        class TestListener implements CommsListener
        {
            public void processMessage(CommsMessage message) {}
        }

        CommsMessage msg = new CommsMessage();

        CommsEndpoint comms = null;
        // simple case
        try
        {
            comms = new CommsEndpoint();
            comms.destroy();
        }
        catch (CommsException e)
        {
            fail("simple case failed");
        }
        finally
        {
            comms.destroy();
            comms = null;
        }

        // destroy when connected
        try
        {
            comms = new CommsEndpoint();
            comms.connect(SERVER_ADDRESS);
            comms.destroy();
        }
        catch (CommsException e)
        {
            fail("destroy when connected failed");
        }
        finally
        {
            comms.destroy();
            comms = null;
        }

        // destroy when disconnected
        try
        {
            comms = new CommsEndpoint();
            comms.connect(SERVER_ADDRESS);
            comms.disconnect();
            comms.destroy();
        }
        catch (CommsException e)
        {
            fail("destroy when disconnected failed");
        }
        finally
        {
            comms.destroy();
            comms = null;
        }



        // calling methods after close
        try
        {
            comms = new CommsEndpoint();
            comms.destroy();
            comms.destroy();
            try
            {
                comms.connect(SERVER_ADDRESS);
                fail("connect");
            }
            catch (CommsException e) {}

            try
            {
                comms.disconnect();
                fail("disconnect");
            }
            catch (CommsException e) {}

            try
            {
                comms.registerListener(MODULE_ID_A, new TestListener());
                fail("registerListener");
            }
            catch (Exception e) {}

            try
            {
                comms.unregisterListener(MODULE_ID_A);
                fail("unregisterListener");
            }
            catch (Exception e) {}

            try
            {
                comms.registerDefaultListener(new TestListener());
                fail("registerDefaultListener");
            }
            catch (Exception e) {}

            try
            {
                comms.unregisterDefaultListener();
                fail("unregisterDefaultListener");
            }
            catch (Exception e) {}

            try
            {
                comms.send(msg);
                fail("send");
            }
            catch (Exception e) {}

            try
            {
                CommsMessage m = comms.sendReceive(msg, TIMEOUT);
                fail("send");
            }
            catch (Exception e) {}

        }
        catch (CommsException e)
        {
            fail(" multiple endpoints failed");
        }
        finally
        {
            comms.destroy();
            comms = null;
        }

    }

}

