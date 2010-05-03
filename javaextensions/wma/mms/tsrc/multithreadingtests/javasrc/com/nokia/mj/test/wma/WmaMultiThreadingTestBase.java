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
package com.nokia.mj.test.wma;

import j2meunit.framework.*;
import javax.microedition.midlet.MIDlet;

import java.io.ByteArrayInputStream;
import java.io.InputStream;
import java.io.UnsupportedEncodingException;
import java.util.Vector;

import javax.microedition.io.Connector;
import javax.wireless.messaging.MessageConnection;
import javax.wireless.messaging.MessagePart;
import javax.wireless.messaging.MultipartMessage;
import javax.wireless.messaging.SizeExceededException;

import java.io.OutputStream;
import java.io.IOException;
import javax.microedition.io.file.FileConnection;

/**
 * Base class for WMA 2.0 (JSR-205) multithreading module test cases.
 * It provides methods for launching multiple threads/workers
 * performing different tasks (send MMS, receive MMS, close connection)
 *
 * Different multithreading scenarios can be achieved by setting different
 * values for the constants:
 *        CONNECTIONS = number of connections to be used
 *        CON_REP_FACTOR = connections repetitions factor
 *        RECEIVERS = number of receivers
 *        CLOSERS = number of closer threads (workers which do nothing
 *                            but closing a certain connection)
 *        MESSAGES = number of messages
 *        MSG_REP_FACTOR = messages repetitions factor
 */
public class WmaMultiThreadingTestBase extends WmaTestBase
{
    // task definitions. Workers are created with one of the following
    // tasks
    protected static final int TASK_UNDEFINED = 0;
    protected static final int TASK_CLOSE_CONNECTION = 1;
    protected static final int TASK_SEND_MESSAGE = 2;
    protected static final int TASK_RECEIVE_MESSAGE = 3;

    // a notifier used to wait for all the worker threads and report
    // the status to the caller. The multithreading tests must start
    // with calling the init() method, which initializes the notifier
    protected Notifier notifier = null;

    // constants which could be changed by the different test cases in
    // in order to achieve different multithreading scenarios
    protected    int CONNECTIONS = 0;
    protected    int CON_REP_FACTOR = Integer.MAX_VALUE;
    protected    int RECEIVERS = 0;
    protected int CLOSERS = 0;
    protected    int MESSAGES = 0;
    protected    int MSG_REP_FACTOR = Integer.MAX_VALUE;

    // the connections used by the workers for different tasks
    protected MessageConnection[] conns = null;

    // the messages to be sent
    protected MultipartMessage[] msgs = null;

    // the received messages. The receiving workers will update this
    // vector with all the received messages. After all the worker threads
    // have finished their tasks, this vector can be used to validate that
    // all the sent messages and the received ones are identical
    protected Vector receivedMessages = null;

    // the messages which were actually sent
    protected Vector sentMessages = null;

    // the error code which is maintained by the workers.
    protected String err;

    /**
    ** Constructor
    **/
    public WmaMultiThreadingTestBase()
    {
    }

    /**
    ** Constructor
    **/
    public WmaMultiThreadingTestBase(String sTestName, TestMethod rTestMethod)
    {
        super(sTestName, rTestMethod);
    }

    /**
    * Opens a set of connections to given URLs whose base is provided as parameter.
    * CON_REP_FACTOR dictates if all the connections are to be different or not.
    * @param URL The base URL for the connections to be created. The connections will
    * use this URL plus an integer which makes them unique
    */
    protected MessageConnection[] getConnections(String URL)
    throws WmaException
    {
        conns = new MessageConnection[CONNECTIONS];
        for (int i=0; i<CONNECTIONS; i++)
        {
            conns[i] = getConnection(URL + (i-(i/CON_REP_FACTOR)*CON_REP_FACTOR));
        }
        return conns;
    }

    /**
    * Creates a set of dummy messages.
    * @param conns The set of connections over which the messages are constructed
    * @param address The base destination address of the message
    * @param subject The base subject of the message
    */
    protected MultipartMessage[] getDummyMultipartMessages(
        MessageConnection[] conns,
        String address,
        String subject)
    throws SizeExceededException, UnsupportedEncodingException
    {
        msgs = new MultipartMessage[MESSAGES];
        int addrRepFactor = 1;
        if (RECEIVERS != 0)
        {
            addrRepFactor = RECEIVERS;
        }
        for (int i= 0; i<MESSAGES; i++)
        {
            msgs[i] = getDummyMultipartMessage(
                          conns[(i-(i/conns.length)*conns.length)],
                          address  + (i-(i/addrRepFactor)*addrRepFactor),
                          subject+(i-(i/MSG_REP_FACTOR)*MSG_REP_FACTOR));
            msgs[i].addAddress("cc", address  + (i-(i/addrRepFactor)*addrRepFactor));
            msgs[i].addAddress("bcc", address  + (i-(i/addrRepFactor)*addrRepFactor));
            String prio;
            String cEncoding;
            switch ((i-(i/3)*3))
            {
            case 0:
                prio = "high";
                cEncoding = "utf-8";
                break;
            case 1:
                prio = "low";
                cEncoding = "iso-8859-2";
                break;
            default:
                prio = "normal";
                cEncoding = "iso-8859-1";
                break;
            }
            msgs[i].setHeader("X-Mms-Priority", prio);
            MessagePart part = getDummyMessagePart(
                                   100 + (i-(i/MSG_REP_FACTOR)*MSG_REP_FACTOR),
                                   "contentID" + (i-(i/MSG_REP_FACTOR)*MSG_REP_FACTOR),
                                   "cLocation" + (i-(i/MSG_REP_FACTOR)*MSG_REP_FACTOR),
                                   cEncoding);
            msgs[i].addMessagePart(part);
        }

        return msgs;
    }

    /**
    ** Closes all the connections. This method should be called at the end of
    ** each multithreading module test
    **/
    protected void cleanup()
    {
        for (int i=0; i<CONNECTIONS; i++)
        {
            try
            {
                // Close the client connection.
                if (conns[i] != null)
                {
                    conns[i].close();
                }
            }
            catch (Exception e) {}
        }
    }

    /**
    ** The init method which must be called in the beginning of each multithreading
    ** module test. This method initializes the notifier.
    **/
    protected void init()
    {
        err = null;
        receivedMessages = new Vector();
        sentMessages = new Vector();
        if ((MESSAGES + CLOSERS + RECEIVERS) == 0)
        {
            return;
        }
        notifier = new Notifier(this, MESSAGES + CLOSERS + RECEIVERS);
        notifier.start();
    }

    /**
    ** This method starts as many senders as many messages are available
    **/
    protected void multipleSend(MessageConnection[] conns, MultipartMessage[] msgs)
    {
        Worker[] workers = new Worker[MESSAGES];
        for (int i=0; i<workers.length; i++)
        {
            workers[i] = new Worker(
                conns[(i-(i/conns.length)*conns.length)],
                msgs[(i-(i/msgs.length)*msgs.length)],
                TASK_SEND_MESSAGE);
        }
        for (int i=0; i<workers.length; i++)
        {
            workers[i].start();
        }
    }

    /**
    ** Variant method used for starting a set of sender workers
    **/
    protected void multipleSend(MessageConnection conn, MultipartMessage msg)
    {
        MessageConnection[] conns = new MessageConnection[1];
        conns[0] = conn;
        MultipartMessage[] msgs = new MultipartMessage[1];
        msgs[0] = msg;
        multipleSend(conns, msgs);
    }

    /**
    ** Variant method used for starting a set of sender workers
    **/
    protected void multipleSend(MessageConnection conn, MultipartMessage[] msgs)
    {
        MessageConnection[] conns = new MessageConnection[1];
        conns[0] = conn;
        multipleSend(conns, msgs);
    }
    /**
    ** This method starts a set of receivers
    **/
    protected void multipleReceive(MessageConnection[] conns)
    {
        Worker[] workers = new Worker[RECEIVERS];
        for (int i=0; i<workers.length; i++)
        {
            workers[i] = new Worker(
                conns[(i-(i/conns.length)*conns.length)],
                TASK_RECEIVE_MESSAGE,
                (RECEIVERS == 0 ? 1 : (MESSAGES/RECEIVERS)));
        }
        for (int i=0; i<workers.length; i++)
        {
            workers[i].start();
        }
    }

    /**
    ** Method used by closer threads
    **/
    protected void closeConnections(MessageConnection[] conns)
    {
        int workersCount = CLOSERS*conns.length;
        Worker[] workers = new Worker[workersCount];
        for (int i=0; i<conns.length; i++)
        {
            for (int j=0; j<CLOSERS; j++)
            {
                workers[j+i*CLOSERS] = new Worker(conns[i], TASK_CLOSE_CONNECTION);
            }
        }
        for (int i=0; i<workersCount; i++)
        {
            workers[i].start();
        }
    }

    /**
    ** The caller usesd this mehod to wait for all the workers
    **/
    protected String waitForResults()
    {
        // wait for all the threads to finish and collect the
        // errors (if any)
        synchronized (this)
        {
            try
            {
                wait();
            }
            catch (InterruptedException e) {};
        }
        return err;
    }

    /**
    ** This method checkes that the messages which were sent and the received ones are
    ** identical
    **/
    protected boolean validMessages()
    {
        if (sentMessages.size() != receivedMessages.size())
        {
            return false;
        }
        for (int i=0; i<receivedMessages.size(); i++)
        {
            boolean found = false;
            int j;
            String msgStr1 = msgToString((MultipartMessage) receivedMessages.elementAt(i), false).toLowerCase();
            for (j=0; j<sentMessages.size(); j++)
            {
                String msgStr2 = msgToString((MultipartMessage)sentMessages.elementAt(j), false).toLowerCase();
                if (msgStr1.equals(msgStr2))
                {
                    found = true;
                    break;
                }
            }
            if (found)
            {
                sentMessages.removeElementAt(j);
            }
            else
            {
                return false;
            }
        }
        return true;
    }

    /**
    ** The worker thread. It currently supports sending and receiving
    ** of MMSs and closing a connection
    **/
    class Worker extends Thread
    {
        private MessageConnection conn = null;
        private MultipartMessage msg = null;
        private String error = null;
        private String trace = null;
        private int task = TASK_UNDEFINED;
        private int taskRepetitions = 1;

        public Worker()
        {
            conn = null;
            msg = null;
            task = TASK_UNDEFINED;
            taskRepetitions = 0;
        }

        public Worker(MessageConnection aConnection, int aTask, int aTaskRepetitions)
        {
            conn = aConnection;
            msg = null;
            task = aTask;
            taskRepetitions = aTaskRepetitions;
        }

        public Worker(MessageConnection aConnection, int aTask)
        {
            conn = aConnection;
            msg = null;
            task = aTask;
            taskRepetitions = 1;
        }

        public Worker(MessageConnection aConnection, MultipartMessage aMsg, int aTask)
        {
            conn = aConnection;
            msg = aMsg;
            task = aTask;
            taskRepetitions = 1;
        }

        public void run()
        {
            for (int i=0; i<taskRepetitions; i++)
            {
                if (error != null)
                {
                    break;
                }
                if (task == TASK_UNDEFINED)
                {
                    error = "Sending message failed: connection is null";
                    return;
                }
                if (conn == null)
                {
                    error = "Sending message failed: connection is null";
                }
                else
                {
                    switch (task)
                    {
                    case TASK_SEND_MESSAGE:
                        send();
                        break;
                    case TASK_RECEIVE_MESSAGE:
                        receive();
                        break;
                    case TASK_CLOSE_CONNECTION:
                        close();
                        break;
                    }
                }
            }
            if (notifier != null)
            {
                notifier.nNotify();
            }
        }

        private void send()
        {
            try
            {
                sentMessages.addElement(msg);
                conn.send(msg);
            }
            catch (IOException ioe)
            {
                // some messages might time out, that's ok sometimes
                ioe.printStackTrace();
            }
            catch (Exception e)
            {
                error = "[Worker]: Sending message failed with exception: "
                        + e.toString();
                e.printStackTrace();
            }
        }

        private void receive()
        {
            try
            {
                msg = (MultipartMessage)conn.receive();
                if (msg != null)
                {
                    receivedMessages.addElement(msg);
                }
            }
            catch (IOException ioe)
            {
                // some messages might time out, that's ok sometimes
                ioe.printStackTrace();
            }
            catch (Exception e)
            {
                e.printStackTrace();
                error = "[Worker]: Receiving message failed with exception: "
                        + e.toString();
            }
        }

        private void close()
        {
            try
            {
                conn.close();
            }
            catch (Exception e)
            {
                error = "[Worker]: Closing failed with exception: "
                        + e.toString();
            }
        }

        public String getError()
        {
            return error;
        }
    }

    /**
    ** Notifier thread used to report to the caller when all the workers
    ** have finished their tasks
    **/
    class Notifier extends Thread
    {
        public int notificationsDone = 0;
        private Object owner = null;
        private int notificationsToMake = 0;

        public Notifier(Object anOwner, int aNotificationsToMake)
        {
            owner = anOwner;
            notificationsToMake = aNotificationsToMake;
        }

        public void run()
        {
            synchronized (this)
            {
                int i = notificationsToMake;
                while (i>0)
                {
                    nWait();
                    i = i - notificationsDone;
                }
            }
            if (owner != null)
            {
                synchronized (owner)
                {
                    owner.notify();
                }
            }
        }

        private synchronized void nWait()
        {
            notificationsDone = 0;
            try
            {
                wait();
            }
            catch (InterruptedException ie) {}
        }

        public synchronized void nNotify()
        {
            notificationsDone++;
            notify();
        }
    }
}
