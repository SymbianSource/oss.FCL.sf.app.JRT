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
import com.nokia.mj.impl.comms.CommsEndpoint;
import com.nokia.mj.impl.comms.CommsServerEndpoint;
import com.nokia.mj.impl.comms.CommsEndpointBase;
import com.nokia.mj.impl.comms.CommsListener;
import com.nokia.mj.impl.comms.exception.CommsException;

import com.nokia.mj.test.comms.utils.EchoListener;

public class TestCommsServerEndpoint extends TestCase
{

    private final int SERVER_ADDRESS = CommsEndpoint.JAVA_CAPTAIN + 1000;
    private final int MODULE_ID = 100;

    private final int TIMEOUT = 2;

    public TestCommsServerEndpoint()
    {
    }

    public TestCommsServerEndpoint(String sTestName, TestMethod rTestMethod)
    {
        super(sTestName, rTestMethod);
    }

    public Test suite()
    {
        TestSuite aSuite = new TestSuite();

        aSuite.addTest(new TestCommsServerEndpoint("testStartStop", new TestMethod()
        {
            public void run(TestCase tc)
            {
                ((TestCommsServerEndpoint) tc).testStartStop();
            }
        }));

        aSuite.addTest(new TestCommsServerEndpoint("testDestroy", new TestMethod()
        {
            public void run(TestCase tc)
            {
                ((TestCommsServerEndpoint) tc).testDestroy();
            }
        }));

        aSuite.addTest(new TestCommsServerEndpoint("testFind", new TestMethod()
        {
            public void run(TestCase tc)
            {
                ((TestCommsServerEndpoint) tc).testFind();
            }
        }));

        aSuite.addTest(new TestCommsServerEndpoint("testConnect", new TestMethod()
        {
            public void run(TestCase tc)
            {
                ((TestCommsServerEndpoint) tc).testConnect();
            }
        }));

        aSuite.addTest(new TestCommsServerEndpoint("testCheckPayload", new TestMethod()
        {
            public void run(TestCase tc)
            {
                ((TestCommsServerEndpoint) tc).testCheckPayload();
            }
        }));

        aSuite.addTest(new TestCommsServerEndpoint("testBadListener", new TestMethod()
        {
            public void run(TestCase tc)
            {
                ((TestCommsServerEndpoint) tc).testBadListener();
            }
        }));

        aSuite.addTest(new TestCommsServerEndpoint("testHasPermission", new TestMethod()
        {
            public void run(TestCase tc)
            {
                ((TestCommsServerEndpoint) tc).testHasPermission();
            }
        }));


        aSuite.addTest(new TestCommsServerEndpoint("testSendInProcessMessage", new TestMethod()
        {
            public void run(TestCase tc)
            {
                ((TestCommsServerEndpoint) tc).testSendInProcessMessage();
            }
        }));

        aSuite.addTest(new TestCommsServerEndpoint("testServerAndTwoClients", new TestMethod()
        {
            public void run(TestCase tc)
            {
                ((TestCommsServerEndpoint) tc).testServerAndTwoClients();
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

    public void testStartStop()
    {
        System.out.println("TestCommsServerEndpoint.testStartStop()");

        CommsServerEndpoint comms = null;
        // simple case
        try
        {
            comms = new CommsServerEndpoint();
            comms.start(SERVER_ADDRESS);
        }
        catch (CommsException e)
        {
            fail("ctor failed");
        }
        finally
        {
            comms.destroy();
            comms = null;
        }

        // multiple endpoints
        CommsServerEndpoint server1 = null;
        CommsServerEndpoint server2 = null;
        CommsServerEndpoint server3 = null;
        try
        {
            server1 = new CommsServerEndpoint();
            server2 = new CommsServerEndpoint();
            server3 = new CommsServerEndpoint();

            server1.start(SERVER_ADDRESS);
            server2.start(SERVER_ADDRESS+1);
            server3.start(SERVER_ADDRESS+2);

            server1.stop();
            server2.stop();
            server3.stop();
        }
        catch (CommsException e)
        {
            fail(" multiple endpoints failed");
        }
        finally
        {
            server1.destroy();
            server2.destroy();
            server3.destroy();
        }

        // multiple start/stop calls
        try
        {
            comms = new CommsServerEndpoint();
            comms.stop();

            comms.start(SERVER_ADDRESS);
            comms.start(SERVER_ADDRESS);

            comms.stop();
            comms.stop();
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

        // start fails
        try
        {
            server1 = new CommsServerEndpoint();
            server2 = new CommsServerEndpoint();

            server1.start(SERVER_ADDRESS);
            server2.start(SERVER_ADDRESS);
            fail("start did not fail");
        }
        catch (CommsException e)
        {
        }
        finally
        {
            server1.destroy();
            server2.destroy();
        }
    }


    public void testDestroy()
    {
        System.out.println("TestCommsServerEndpoint.testDestroy()");
        class TestListener implements CommsListener
        {
            public void processMessage(CommsMessage message) {}
        }
        TestListener listener = new TestListener();
        CommsMessage msg = new CommsMessage();

        CommsServerEndpoint server = null;
        // simple case
        try
        {
            server = new CommsServerEndpoint();
            server.destroy();
        }
        catch (CommsException e)
        {
            fail("simple case failed");
        }
        finally
        {
            server.destroy();
            server = null;
        }

        // destroy when started
        try
        {
            server = new CommsServerEndpoint();
            server.start(SERVER_ADDRESS);
            server.destroy();
        }
        catch (CommsException e)
        {
            fail("close when started failed");
        }
        finally
        {
            server.destroy();
            server = null;
        }

        // calling methods after close
        try
        {
            server = new CommsServerEndpoint();
            server.destroy();
            server.destroy();
            try
            {
                server.start(SERVER_ADDRESS);
                fail("start");
            }
            catch (CommsException e) {}

            try
            {
                server.stop();
                fail("stop");
            }
            catch (CommsException e) {}

            try
            {
                server.registerListener(MODULE_ID, listener);
                fail("registerListener");
            }
            catch (Exception e) {}

            try
            {
                server.unregisterListener(MODULE_ID);
                fail("unregisterListener");
            }
            catch (Exception e) {}

            try
            {
                server.registerDefaultListener(listener);
                fail("registerDefaultListener");
            }
            catch (Exception e) {}

            try
            {
                server.unregisterDefaultListener();
                fail("unregisterDefaultListener");
            }
            catch (Exception e) {}

            try
            {
                server.send(msg);
                fail("send");
            }
            catch (Exception e) {}

            try
            {
                CommsMessage m = server.sendReceive(msg, TIMEOUT);
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
            server.destroy();
            server = null;
        }

    }

    public void testFind()
    {
        System.out.println("TestCommsServerEndpoint.testFind()");

        CommsServerEndpoint comms = null;

        // not existing
        comms = CommsServerEndpoint.find("not existing");
        assertEquals(comms, null);
        comms = null;
    }

    public void testConnect()
    {
        System.out.println("TestCommsServerEndpoint.testConnect()");
        CommsMessage msg = new CommsMessage();

        CommsServerEndpoint server = null;
        CommsEndpoint client = null;
        // One client
        try
        {
            server = new CommsServerEndpoint();
            EchoListener listener = new EchoListener(server);
            server.start(SERVER_ADDRESS);
            server.registerDefaultListener(listener);

            client = new CommsEndpoint();
            client.connect(SERVER_ADDRESS);
            client.sendReceive(msg, TIMEOUT);

            client.disconnect();
            server.stop();

            server.start(SERVER_ADDRESS+1);
            client.connect(SERVER_ADDRESS+1);
            client.sendReceive(msg, TIMEOUT);

            client.disconnect();
            server.stop();
        }
        catch (Exception e)
        {
            fail("testConnect one client failed");
        }
        finally
        {
            server.destroy();
            server = null;
            client.destroy();
        }

        // multiple clients
        CommsEndpoint client1 = null;
        CommsEndpoint client2 = null;
        CommsEndpoint client3 = null;
        try
        {
            server = new CommsServerEndpoint();
            server.start(SERVER_ADDRESS);
            EchoListener listener = new EchoListener(server);
            server.registerDefaultListener(listener);

            client1 = new CommsEndpoint();
            client2 = new CommsEndpoint();
            client3 = new CommsEndpoint();

            client1.connect(SERVER_ADDRESS);
            client2.connect(SERVER_ADDRESS);
            client3.connect(SERVER_ADDRESS);
            client1.sendReceive(msg, TIMEOUT);
            client2.sendReceive(msg, TIMEOUT);
            client3.sendReceive(msg, TIMEOUT);

            client1.disconnect();
            client2.disconnect();
            client3.disconnect();
            server.stop();
        }
        catch (Exception e)
        {
            fail("testConnect multiple clients failed");
        }
        finally
        {
            server.destroy();
            server = null;
            client1.destroy();
            client2.destroy();
            client3.destroy();
        }
    }


    public void testCheckPayload()
    {
        System.out.println("TestCommsServerEndpoint.testCheckPayload()");

        CommsServerEndpoint server = null;
        CommsEndpoint comms = null;
        try
        {
            server = new CommsServerEndpoint();
            server.start(SERVER_ADDRESS);
            EchoListener listener = new EchoListener(server);
            server.registerDefaultListener(listener);

            comms = new CommsEndpoint();
            comms.connect(SERVER_ADDRESS);

            CommsMessage message = new CommsMessage();
            message.setMessageId(0xDEADBEEF);
            message.setModuleId(0xFF000000);
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
            CommsMessage msg2 = comms.sendReceive(empty, TIMEOUT);
            assertNotNull(msg2);
            assertEquals(msg2.getMessageId(), empty.getMessageId());
            assertEquals(msg2.getModuleId(), empty.getModuleId());

            // large message
            CommsMessage bigOne = new CommsMessage();
            bigOne.setMessageId(-1);
            bigOne.setModuleId(-1);
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
            message.write(in);
            msg = comms.sendReceive(message, TIMEOUT);
            String out = msg.readString();
            assertEquals(in, out);

        }
        catch (Exception e)
        {
            e.printStackTrace();
            fail("testCheckPayload failed");
        }
        finally
        {
            comms.destroy();
            server.destroy();
        }
    }

    public void testBadListener()
    {
        System.out.println("TestCommsServerEndpoint.testBadListener()");
        class BadListener implements CommsListener
        {
            public void processMessage(CommsMessage message)
            {
                int[] arr = new int[1];
                // throws exception on purpose
                int i = arr[arr.length+1];
            }
        }

        CommsServerEndpoint server = null;
        CommsEndpoint comms = null;
        try
        {
            server = new CommsServerEndpoint();
            server.registerDefaultListener(new BadListener());
            server.start(SERVER_ADDRESS);

            comms = new CommsEndpoint();
            comms.registerDefaultListener(new BadListener());
            comms.connect(SERVER_ADDRESS);

            // exception in server side processMessage
            comms.send(new CommsMessage());
            Thread.sleep(100); // give time to receive message

            server.unregisterDefaultListener();
            server.registerDefaultListener(new EchoListener(server));
            comms.sendReceive(new CommsMessage(), TIMEOUT);

            // exception in client side processMessage
            comms.send(new CommsMessage());
            Thread.sleep(100); // give time to receive message
            comms.sendReceive(new CommsMessage(), TIMEOUT);

        }
        catch (Exception e)
        {
            e.printStackTrace();
            fail("testBadListener failed");
        }
        finally
        {
            comms.destroy();
            server.destroy();
        }
    }


    public void testHasPermission()
    {
        System.out.println("TestCommsServerEndpoint.testHasPermission()");
        class PermissionListener implements CommsListener
        {
            private CommsEndpointBase iComms = null;
            public CommsMessage receivedMsg;

            public PermissionListener(CommsEndpointBase aComms)
            {
                iComms = aComms;
            }

            public void processMessage(CommsMessage aMessage)
            {
                try
                {
                    receivedMsg = new CommsMessage(aMessage.toByteArray());
                    CommsMessage msg = new CommsMessage();
                    msg.replyTo(aMessage);
                    iComms.send(msg);
                }
                catch (Exception e)
                {
                    System.out.println(e);
                }
            }
        }

        CommsServerEndpoint server = null;
        CommsEndpoint comms = null;
        try
        {
            server = new CommsServerEndpoint();
            PermissionListener listener = new PermissionListener(server);
            server.registerDefaultListener(listener);
            server.start(SERVER_ADDRESS);

            comms = new CommsEndpoint();
            comms.connect(SERVER_ADDRESS);

            CommsMessage msg = comms.sendReceive(new CommsMessage(), TIMEOUT);

            assertTrue(msg.hasPermission(CommsPermission.MANAGE_CERTIFICATES));
            assertTrue(msg.hasPermission(CommsPermission.INSTALL_APPLICATION));
            assertTrue(msg.hasPermission(CommsPermission.LAUNCH_APPLICATION));
            assertTrue(msg.hasPermission(CommsPermission.STOP_APPLICATION));

            assertTrue(listener.receivedMsg.hasPermission(CommsPermission.MANAGE_CERTIFICATES));
            assertTrue(listener.receivedMsg.hasPermission(CommsPermission.INSTALL_APPLICATION));
            assertTrue(listener.receivedMsg.hasPermission(CommsPermission.LAUNCH_APPLICATION));

            // javainstaller does not have STOP_APPLICATION permission in S60
            String platform = System.getProperty("os.name");
            if (platform != null && platform.equalsIgnoreCase("linux"))
            {
                assertTrue(listener.receivedMsg.hasPermission(CommsPermission.STOP_APPLICATION));
            }
            else
            {
                assertTrue(!listener.receivedMsg.hasPermission(CommsPermission.STOP_APPLICATION));
            }

            comms.disconnect();
            server.stop();
        }
        catch (Exception e)
        {
            e.printStackTrace();
            fail("testHasPermission failed");
        }
        finally
        {
            comms.destroy();
            server.destroy();
        }
    }


    public void testSendInProcessMessage()
    {
        System.out.println("TestCommsServerEndpoint.testSendInProcessMessage()");

        class TestListener extends EchoListener
        {
            int iReplyCount = 30;
            public TestListener(CommsEndpointBase aComms)
            {
                super(aComms);
            }

            public void processMessage(CommsMessage message)
            {
                super.processMessage(message);
                if (--iReplyCount<0)
                {
                    synchronized (this)
                    {
                        this.notifyAll();
                    }
                }
            }

            public void doWait()
            {
                synchronized (this)
                {
                    try
                    {
                        this.wait();
                    }
                    catch (InterruptedException ie) {}
                }
            }
        }

        CommsServerEndpoint server = null;
        CommsEndpoint comms = null;
        try
        {
            server = new CommsServerEndpoint();
            EchoListener listener = new EchoListener(server);
            server.registerDefaultListener(listener);
            server.start(SERVER_ADDRESS);

            comms = new CommsEndpoint();
            TestListener listener2 = new TestListener(comms);
            comms.registerDefaultListener(listener2);
            comms.connect(SERVER_ADDRESS);

            CommsMessage msg = new CommsMessage();
            msg.write("hello world");
            comms.send(msg);

            // message is beeing sent back and forth between listeners
            listener2.doWait();

            comms.disconnect();
            server.stop();
        }
        catch (Exception e)
        {
            e.printStackTrace();
            fail("testSendInProcessMessage failed");
        }
        finally
        {
            comms.destroy();
            server.destroy();
        }
    }

    public void testServerAndTwoClients()
    {
        System.out.println("TestCommsServerEndpoint.testServerAndTwoClients()");

        class RouterListener implements CommsListener
        {
            CommsEndpointBase iComms;
            CommsMessage iFirstMessage;

            public RouterListener(CommsEndpointBase aComms)
            {
                iComms = aComms;
            }

            public void processMessage(CommsMessage aMessage)
            {
                if (iFirstMessage == null)
                {
                    // delay reply until second message is received
                    iFirstMessage = aMessage;
                }
                else
                {
                    // reply to first client
                    CommsMessage reply = new CommsMessage();
                    reply.replyTo(iFirstMessage);
                    iComms.send(reply);
        
                    // reply to second client
                    reply.replyTo(aMessage);
                    iComms.send(reply);
                }
            }
        }

        class SendReceiver extends Thread
        {
            public void run()
            {
                try
                {
                    CommsEndpoint client = new CommsEndpoint();
                    client.connect(SERVER_ADDRESS);
                    CommsMessage reply = client.sendReceive(new CommsMessage(), CommsEndpoint.WAIT_FOR_EVER);
                    client.disconnect();
                    client.destroy();
                }
                catch (Exception e)
                {
                    e.printStackTrace();
                    fail("SendReceiver.run");
                }
            }
        }

        CommsServerEndpoint server = null;
        try
        {
            server = new CommsServerEndpoint();
            RouterListener listener = new RouterListener(server);
            server.registerDefaultListener(listener);
            server.start(SERVER_ADDRESS);

            SendReceiver client1 = new SendReceiver();
            SendReceiver client2 = new SendReceiver();

            client1.start();
            client2.start();
            client1.join();
            client2.join();

            server.stop();
        }
        catch (Exception e)
        {
            e.printStackTrace();
            fail("testServerAndTwoClients failed");
        }
        finally
        {
            server.destroy();
        }
    }


}

