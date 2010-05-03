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

package com.nokia.mj.impl.comms;

import java.io.ByteArrayInputStream;
import java.io.DataInputStream;
import java.io.ByteArrayOutputStream;
import java.io.DataOutputStream;
import java.io.IOException;
import java.lang.IllegalArgumentException;
import com.nokia.mj.impl.utils.Logger;

/**
 * CommsMessage class provides container for message payload. It provides an
 * interface to manipulate payload as if it was input/output stream.
 * <p>
 * Comms message consists of header fields and message body.
 * Header is used to route message to correct endpoint and listener.
 * Header consists of following fields:
 * - module id, identifies which listener is used to handle message
 * - message id, identifies message
 * - receiver, identifies receiving endpoint
 * - sender, identifies sending endpoint
 * - message reference, identifies message
 * <p>
 * Message payload can be read and write using stream operations (read, write).
 * Message can contain multiple items (ints, strings etc) that must be
 * read in the same order as they were originally written.
 * <p>
 * There is no limit to message size but some other interprocess communication mechanism
 * should be considered if message sizes exceed 60k bytes.
 * <p>
 * It is important that application fills message headers correctly as
 * message routing and dispatching is done based on header information.
 * Following headers should be set always: module id, message id and message reference.
 * <p>
 * Example how to use CommsMessage class:
 * <pre>
 * <code>
 *       CommsMessage msg = new CommsMessage();
 *
 *       //  Following headers must be set always
 *       message.setMessageId(MESSAGE_ID);
 *       message.setModuleId(MODULE_ID);
 *
 *       // Following header should be set if replying
 *       message.setMessageReference(aMesssage.getMessageReference());
 *
 *       // set message payload
 *       message.write(1234);
 *       message.write("this is a string");
 *       message.write("this is a byte array".getBytes());
 *
 *       CommsMessage msg = comms.sendReceive(message, 10); // send the message
 *
 *       // get message payload
 *       int a    = msg.readInt();
 *       String b = msg.readString();
 *       byte[] c = msg.readBytes();
 * </code>
 * </pre>
 *
 * It's important that message payload is read in the same order is it was written to message.
 * To ensure that sending and receiving side handle message content properly application should
 * define encode/decode functions for each message or implement own message class,
 * which is derived from CommsMessage.
 * <p>
 * For, example wrapper class for own message could be defined as below
 * <pre>
 * <code>
 *    public class MyMessage extends CommsMessage
 *    {
 *        private String param1;
 *        private int param2;
 *
 *        public MyMessage() {
 *        }
 *
 *        // decode
 *        public MyMessage(CommsMessage msg) {
 *            param1 = msg.readString();
 *            param2 = msg.readInt();
 *        }
 *
 *        // encode
 *        public byte[] toByteArray() {
 *            write(param1);
 *            write(param2);
 *            return super.toByteArray();
 *        }
 *
 *        // getters and setters
 *        public String getParam1() { return param1; }
 *        public void setParam1(String s) { param1 = s; }
 *
 *        public int getParam2() { return param2; }
 *        public void setParam2(int i) { param2 = i; }
 *    }
 *
 *    MyMessage msg = new MyMessage();
 *    CommsMessage message = comms.sendReceive(msg, 5);
 *    MyMessage msg2 = new MyMessage(message);
 * </code>
 * </pre>
 * <p>
 * Alternatively, application could be define encode/decode methods as below
 * <pre>
 * <code>
 *    public class MyMessage
 *    {
 *        private String param1;
 *        private int param2;
 *
 *        public MyMessage(CommsMessage msg) {
 *            set(msg);
 *        }
 *
 *        // decode
 *        public set(CommsMessage msg) {
 *            param1 = msg.readString();
 *            param2 = msg.readInt();
 *        }
 *
 *        // encode
 *        public CommsMessage get() {
 *            CommsMessage msg;
 *            msg.write(param1);
 *            msg.write(param2);
 *            return msg;
 *        }
 *    }
 *
 *    MyMessage msg = new MyMessage();
 *    CommsMessage message = comms.sendReceive(msg.get(), 5);
 *    MyMessage msg2 = new MyMessage(message);
 * </code>
 * </pre>
 */

public class CommsMessage
{
    private int iMessageId;
    private int iReceiver;
    private int iSender;
    private int iModuleId;
    private int iMessageRef;
    private int iPermissions;

    private ByteArrayInputStream iInputStream = null;
    private ByteArrayOutputStream iOutputStream = null;

    private static final int MESSAGE_HEADER_LENGTH = 28;
    private static final String DEFAULT_CHARSET = "UTF-8";

    /**
     * Constructs a message. Content is initialized to an empty stream.
     */
    public CommsMessage()
    {
        iOutputStream = new ByteArrayOutputStream();
    }

    /**
     * Constructs a message from byte array.
     * Content is initialized to a copy of the byte array
     * @param aRawMessage Byte array used to initialize message
     */
    public CommsMessage(byte[] aRawMessage)
    {
        this();
        if (aRawMessage==null || aRawMessage.length < MESSAGE_HEADER_LENGTH)
        {
            throw new IllegalArgumentException("aRawMessage is null or too short");
        }
        fromByteArray(aRawMessage);
    }

    /**
     * Converts message to byte array
     * @return Message as byte array
     */
    public byte[] toByteArray()
    {
        byte[] msg = null;
        try
        {
            ByteArrayOutputStream baos = new ByteArrayOutputStream();
            DataOutputStream dos = new DataOutputStream(baos);

            dos.writeInt(reverseBytes(iMessageId));
            dos.writeInt(reverseBytes(MESSAGE_HEADER_LENGTH + iOutputStream.size()));
            dos.writeInt(reverseBytes(iReceiver));
            dos.writeInt(reverseBytes(iSender));
            dos.writeInt(reverseBytes(iModuleId));
            dos.writeInt(reverseBytes(iMessageRef));
            dos.writeInt(reverseBytes(iPermissions));

            //int len = MESSAGE_HEADER_LENGTH + iOutputStream.size();

            byte[] data = iOutputStream.toByteArray();
            dos.write(data, 0, data.length);

            msg = baos.toByteArray();
        }
        catch (Exception e)
        {
            Logger.ELOG(Logger.EJavaComms, "CommsMessage.toByteArray failed", e);
        }
        return msg;
    }

    /**
     * Initializes message header and output stream content
     * @param rawMessage
     */
    private void fromByteArray(byte[] aRawMessage)
    {
        try
        {
            iInputStream = new ByteArrayInputStream(aRawMessage);
            DataInputStream dis = new DataInputStream(iInputStream);

            iMessageId = reverseBytes(dis.readInt());
            int length = reverseBytes(dis.readInt()) - MESSAGE_HEADER_LENGTH;
            iReceiver = reverseBytes(dis.readInt());
            iSender = reverseBytes(dis.readInt());
            iModuleId = reverseBytes(dis.readInt());
            iMessageRef = reverseBytes(dis.readInt());
            iPermissions =  reverseBytes(dis.readInt());
        }
        catch (Exception e)
        {
            Logger.ELOG(Logger.EJavaComms, "CommsMessage.fromByteArray failed", e);
        }
    }

    /**
     * Returns message id, identifies message type
     * @return message id
     */
    public int getMessageId()
    {
        return iMessageId;
    }

    /**
     * Sets message id, identifies message type
     * @param aMessageId message id
     */
    public void setMessageId(int aMessageId)
    {
        iMessageId = aMessageId;
    }

    /**
     * Returns module id, identifies which listener is used to handle message
     * @return module id
     */
    public int getModuleId()
    {
        return iModuleId;
    }

    /**
     * Sets module id, identifies which listener is used to handle message
     * @param aModuleId module id
     */
    public void setModuleId(int aModuleId)
    {
        iModuleId = aModuleId;
    }

    /**
     * Returns receiver, identifies receiving endpoint
     * @return receiver
     */
    public int getReceiver()
    {
        return iReceiver;
    }

    /**
     * Set receiver, identifies receiving endpoint
     * @param aModuleId module id
     */
    public void setReceiver(int aReceiver)
    {
        iReceiver = aReceiver;
    }

    /**
     * Returns sender, identifies sending endpoint
     * @return sender
     */
    public int getSender()
    {
        return iSender;
    }

    /**
     * Sets sender, identifies sending endpoint
     * @return sender
     */
    public void setSender(int aSender)
    {
        iSender = aSender;
    }

    /**
     * Returns message reference, identifies message
     * @return message reference
     */
    public int getMessageRef()
    {
        return iMessageRef;
    }

    /**
     * Sets message reference, identifies message
     * @param aMessageRef message reference
     */
    public void setMessageRef(int aMessageRef)
    {
        iMessageRef = aMessageRef;
    }

    /**
     * Checks what permissions sender of the message has.
     * CommsPermission defines the set of supported permissions.
     * @see CommsPermission
     * @return true, if sender of the message has given permission
     */
    public boolean hasPermission(int aPermission)
    {
        return (iPermissions & aPermission) == aPermission;
    }

    /**
     * Sets headers so that reply is sent to given message
     * @param aMessageRef message reference
     */
    public void replyTo(CommsMessage aMessage)
    {
        setModuleId(aMessage.getModuleId());
        setMessageId(aMessage.getMessageId());
        setMessageRef(aMessage.getMessageRef());
        setReceiver(aMessage.getSender());
        setSender(aMessage.getReceiver());
    }


    /**
     * Writes an int to the underlying output stream
     * @param aValue an int to be written
     */
    public void write(int aValue)
    {
        String num = "" + aValue;
        write(num);
    }

    /**
     * Writes a string to the underlying output stream
     * @param aValue a string to be written
     */
    public void write(String aValue)
    {
        if (aValue==null)
        {
            throw new IllegalArgumentException("aValue is null");
        }
        try
        {
            write(aValue.getBytes(DEFAULT_CHARSET));
        }
        catch (java.io.UnsupportedEncodingException e)
        {
            Logger.ELOG(Logger.EJavaComms, "CommsMessage.write failed", e);
        }
    }

    /**
     * Writes a byte array to the underlying output stream
     * @param aValue a byte array to be written
     */
    public void write(byte[] aValue)
    {
        if (aValue==null)
        {
            throw new IllegalArgumentException("aValue is null");
        }

        try
        {
            DataOutputStream dos = new DataOutputStream(iOutputStream);

            // data is encoded as: length + space + payload
            String len = "" + aValue.length + " ";

            byte[] bytes = len.getBytes(DEFAULT_CHARSET);
            dos.write(bytes, 0, bytes.length);

            dos.write(aValue, 0, aValue.length);
        }
        catch (IOException e)
        {
            Logger.ELOG(Logger.EJavaComms, "CommsMessage.write failed", e);
        }
    }

    /**
     * Reads an int from underlaying input stream
     * @return the int value read
     */
    public int readInt()
    {
        String str = readString();

        int i = 0;
        try
        {
            i = Integer.parseInt(str);
        }
        catch (NumberFormatException e)
        {
            Logger.WLOG(Logger.EJavaComms, "CommsMessage.readInt failed", e);
        }
        return i;
    }

    /**
     * Reads a string from underlaying input stream
     * @return the string value read
     */
    public String readString()
    {
        byte[] bytes = readBytes();

        String str = "";
        try
        {
            str = new String(bytes, 0, bytes.length, DEFAULT_CHARSET);
        }
        catch (java.io.UnsupportedEncodingException e)
        {
            Logger.ELOG(Logger.EJavaComms, "CommsMessage.readString failed", e);
        }
        return str;
    }

    /**
     * Reads a byte array from underlaying input stream
     * @return the byte array read
     */
    public byte[] readBytes()
    {
        if (iInputStream == null)
        {
            return new byte[0];
        }

        int length = readLength(iInputStream);
        byte[] bytes = new byte[length];

        iInputStream.read(bytes, 0, bytes.length);

        return bytes;
    }

    /**
     * Reads the length of next item in the underlaying input stream
     * Message payload consists of blocks stored in the following format to stream:
     * length + separator (space) + payload as a byte array.
     * @return length
     */
    private int readLength(ByteArrayInputStream bais)
    {
        int length = 0;
        try
        {
            byte[] len = new byte[10]; // int can be max 10 chars as string (4294967295)
            byte delim = 32; // space
            DataInputStream dis = new DataInputStream(bais);
            int i = 0;
            for (; i < len.length; i++)
            {
                byte b = dis.readByte();
                if (b == delim)
                {
                    break;
                }
                len[i] = b;
            }

            String str = new String(len, 0, i, DEFAULT_CHARSET);
            length = Integer.parseInt(str);
        }
        catch (IOException e)
        {
            Logger.WLOG(Logger.EJavaComms, "CommsMessage.readLength failed", e);
        }
        return length;
    }

    /**
     * Performs little endian - big endian conversion.
     * Offers same functionality as Integer.reverseBytes in JDK1.5
     * @param aValue Value to byte swap.
     * @return       The value obtained by reversing the bytes in the specified int value.
     */
    private int reverseBytes(int aValue)
    {
        int b1 = (aValue >>  0) & 0xff;
        int b2 = (aValue >>  8) & 0xff;
        int b3 = (aValue >> 16) & 0xff;
        int b4 = (aValue >> 24) & 0xff;

        return b1 << 24 | b2 << 16 | b3 << 8 | b4 << 0;
    }
}
