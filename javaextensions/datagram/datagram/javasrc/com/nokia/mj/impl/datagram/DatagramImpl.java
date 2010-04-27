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


package com.nokia.mj.impl.datagram;

import java.io.ByteArrayInputStream;
import java.io.DataInputStream;
import java.io.DataOutputStream;
import java.io.IOException;
import javax.microedition.io.Datagram;
import com.nokia.mj.impl.gcf.utils.UrlParser;
import com.nokia.mj.impl.utils.Logger;



/**
 * Datagram Implementation class.
 *
 */

public class DatagramImpl implements Datagram
{

    private byte[] iBuffer;
    private int iBufLength;
    private int iBufOffset;
    private String iAddress;
    private DataOutputStream iDataOutput;
    private DataInputStream iDataInput;
    private DatagramByteArrayOutStreamAdapter iByteOStreamAdapter;
    private static final int MAX_DATAGRAM_SIZE = 65535;

    private void validateParameters(byte[] aBuf, int aOffset, int aLength)
    {
        if (aLength < 0 || aOffset < 0 || aBuf == null || aLength > MAX_DATAGRAM_SIZE
                || aLength > (aBuf.length - aOffset))
        {
            throw new IllegalArgumentException("Invalid argument in newDatagram method");
        }
    }

    /**
     * Constructs the DatagramImpl.
     *
     * @param aBuf the buffer used for the datagram
     * @param aSize the size of the buffer
     * @param aAddress to which the datagram will be sent
     * @throws IOException in case of any IO errors
     */

    public DatagramImpl(byte[] aBuf, int aSize, String aAddress)
    throws IOException
    {

        validateParameters(aBuf, 0, aSize);
        iBuffer = aBuf;
        iBufLength = aSize;
        iBufOffset = 0;
        iByteOStreamAdapter = new DatagramByteArrayOutStreamAdapter(aBuf);
        ByteArrayInputStream tmpins = new ByteArrayInputStream(aBuf, 0, aSize);
        iDataInput = new DataInputStream(tmpins);
        iDataOutput = new DataOutputStream(iByteOStreamAdapter);
        iAddress = aAddress;
    }

    /**
     * Please refer JSR 118.
     */

    public String getAddress()
    {
        return iAddress;
    }

    /**
     * Get the data of the datagram.
     *
     * @return the data buffer as a byte array.
     */

    public byte[] getData()
    {
        return iBuffer;
    }

    /**
     * Please refer JSR 118.
     */

    public int getLength()
    {
        return iBufLength;
    }

    /**
     * Please refer JSR 118.
     */

    public int getOffset()
    {
        return iBufOffset;
    }

    /**
     * Please refer JSR 118.
     */

    public void setAddress(String addr) throws IOException
    {

        if (addr == null || addr.startsWith("datagram://")== false)
        {
            throw new IllegalArgumentException("Invalid URL: "+addr);
        }
        // to validate the addr, create a dummy uri
        UrlParser tmp = new UrlParser(addr);
        iAddress = addr;
    }

    /**
     * Please refer JSR 118.
     */

    public void setAddress(Datagram reference)
    {
        if (reference == null)
        {
            throw new IllegalArgumentException("Datagram is null");
        }

        String addr = reference.getAddress();

        // if the reference datagram does not have its address set then it will return
        // so check for null
        if (addr == null)
        {
            throw new IllegalArgumentException("Invalid URL: "+addr);
        }

        Logger.LOG(Logger.ESOCKET, Logger.EInfo,
                   "SetAddress(datagram) : addr = "+addr);

        iAddress = addr;
    }

    /**
     * Please refer JSR 118.
     */

    public void setLength(int len)
    {
        // creating newly using internal buffer with the new length
        setData(iBuffer, iBufOffset, len);

    }

    /**
     * Please refer JSR 118.
     */

    public void setData(byte[] buffer, int offset, int len)
    {

        validateParameters(buffer, offset, len);
        iBuffer = buffer;
        iBufLength = len;
        iBufOffset = offset;
        iByteOStreamAdapter = new DatagramByteArrayOutStreamAdapter(buffer);
        ByteArrayInputStream tmpins = new ByteArrayInputStream(buffer, offset,
                len);
        iDataInput = new DataInputStream(tmpins);
        iDataOutput = new DataOutputStream(iByteOStreamAdapter);
    }

    /**
     * Please refer JSR 118.
     */
    public void reset()
    {

        iDataInput = new DataInputStream(new ByteArrayInputStream(iBuffer, 0, 0));
        iByteOStreamAdapter.reset(); // will make the internal count = 0
        iDataOutput = new DataOutputStream(iByteOStreamAdapter);
        iBufOffset = 0;
        iBufLength = 0;
    }

    /**
     * Please refer JSR 118.
     */

    public void write(int b) throws IOException
    {

        try
        {
            iDataOutput.write(b);
            iBufLength = iByteOStreamAdapter.size();
        }
        catch (ArrayIndexOutOfBoundsException e)
        {
            throw new IOException("Datagram buffer size exceeded");
        }
    }

    /**
     * Please refer JSR 118.
     */

    public void write(byte[] b) throws IOException
    {
        try
        {
            iDataOutput.write(b);
            iBufLength = iByteOStreamAdapter.size();
        }
        catch (ArrayIndexOutOfBoundsException e)
        {
            throw new IOException("Datagram buffer size exceeded");
        }
    }

    /**
     * Please refer JSR 118.
     */

    public void write(byte[] b, int off, int len) throws IOException
    {
        try
        {
            iDataOutput.write(b, off, len);
            iBufLength = iByteOStreamAdapter.size();
        }
        catch (ArrayIndexOutOfBoundsException e)
        {
            throw new IOException("Datagram buffer size exceeded");
        }
    }

    /**
     * Please refer JSR 118.
     */

    public void writeBoolean(boolean v) throws IOException
    {
        try
        {
            iDataOutput.writeBoolean(v);
            iBufLength = iByteOStreamAdapter.size();
        }
        catch (ArrayIndexOutOfBoundsException e)
        {
            throw new IOException("Datagram buffer size exceeded");
        }
    }

    /**
     * Please refer JSR 118.
     */

    public void writeByte(int v) throws IOException
    {
        try
        {
            iDataOutput.writeByte(v);
            iBufLength = iByteOStreamAdapter.size();
        }
        catch (ArrayIndexOutOfBoundsException e)
        {
            throw new IOException("Datagram buffer size exceeded");
        }
    }

    /**
     * Please refer JSR 118.
     */

    public void writeShort(int v) throws IOException
    {
        try
        {
            iDataOutput.writeShort(v);
            iBufLength = iByteOStreamAdapter.size();
        }
        catch (ArrayIndexOutOfBoundsException e)
        {
            throw new IOException("Datagram buffer size exceeded");
        }

    }

    /**
     * Please refer JSR 118.
     */

    public void writeChar(int v) throws IOException
    {
        try
        {
            iDataOutput.writeChar(v);
            iBufLength = iByteOStreamAdapter.size();
        }
        catch (ArrayIndexOutOfBoundsException e)
        {
            throw new IOException("Datagram buffer size exceeded");
        }
    }

    /**
     * Please refer JSR 118.
     */

    public void writeInt(int v) throws IOException
    {

        try
        {
            iDataOutput.writeInt(v);
            iBufLength = iByteOStreamAdapter.size();
        }
        catch (ArrayIndexOutOfBoundsException e)
        {
            throw new IOException("Datagram buffer size exceeded");
        }
    }

    /**
     * Please refer JSR 118.
     */

    public void writeLong(long v) throws IOException
    {
        try
        {
            iDataOutput.writeLong(v);
            iBufLength = iByteOStreamAdapter.size();
        }
        catch (ArrayIndexOutOfBoundsException e)
        {
            throw new IOException("Datagram buffer size exceeded");
        }
    }

    /**
     * Please refer JSR 118.
     */

    public void writeFloat(float v) throws IOException
    {
        try
        {
            iDataOutput.writeFloat(v);
            iBufLength = iByteOStreamAdapter.size();
        }
        catch (ArrayIndexOutOfBoundsException e)
        {
            throw new IOException("Datagram buffer size exceeded");
        }
    }

    /**
     * Please refer JSR 118.
     */

    public void writeDouble(double v) throws IOException
    {
        try
        {
            iDataOutput.writeDouble(v);
            iBufLength = iByteOStreamAdapter.size();
        }
        catch (ArrayIndexOutOfBoundsException e)
        {
            throw new IOException("Datagram buffer size exceeded");
        }
    }

    /**
     * Please refer JSR 118.
     */

    public void writeChars(String s) throws IOException
    {
        try
        {
            iDataOutput.writeChars(s);
            iBufLength = iByteOStreamAdapter.size();
        }
        catch (ArrayIndexOutOfBoundsException e)
        {
            throw new IOException("Datagram buffer size exceeded");
        }
    }

    /**
     * Please refer JSR 118.
     */

    public void writeUTF(String s) throws IOException
    {
        try
        {
            Logger.LOG(Logger.ESOCKET, Logger.EInfo, "writeUTf - s = "
                       + s + "s.size = " + s.length());
            iDataOutput.writeUTF(s);
            Logger.LOG(Logger.ESOCKET, Logger.EInfo,
                       "iDataOutput.writeUTF success");
            iBufLength = iByteOStreamAdapter.size();
            Logger.LOG(Logger.ESOCKET, Logger.EInfo,
                       "iByteOStreamAdapter.size()success");
        }
        catch (ArrayIndexOutOfBoundsException e)
        {
            throw new IOException("Datagram buffer size exceeded");
        }
    }

    /**
     * Please refer JSR 118.
     */
    public void readFully(byte[] b) throws IOException
    {
        iDataInput.readFully(b);
    }

    /**
     * Please refer JSR 118.
     */

    public void readFully(byte[] b, int off, int len) throws IOException
    {
        iDataInput.readFully(b, off, len);
    }

    /**
     * Please refer JSR 118.
     */

    public int skipBytes(int n) throws IOException
    {
        return iDataInput.skipBytes(n);
    }

    /**
     * Please refer JSR 118.
     */

    public boolean readBoolean() throws IOException
    {
        return iDataInput.readBoolean();
    }

    /**
     * Please refer JSR 118.
     */

    public byte readByte() throws IOException
    {
        return iDataInput.readByte();
    }

    /**
     * Please refer JSR 118.
     */

    public int readUnsignedByte() throws IOException
    {
        return iDataInput.readUnsignedByte();
    }

    /**
     * Please refer JSR 118.
     */

    public short readShort() throws IOException
    {
        return iDataInput.readShort();
    }

    /**
     * Please refer JSR 118.
     */

    public int readUnsignedShort() throws IOException
    {
        return iDataInput.readUnsignedShort();
    }

    /**
     * Please refer JSR 118.
     */

    public char readChar() throws IOException
    {
        return iDataInput.readChar();
    }

    /**
     * Please refer JSR 118.
     */

    public int readInt() throws IOException
    {
        return iDataInput.readInt();
    }

    /**
     * Please refer JSR 118.
     */

    public long readLong() throws IOException
    {
        return iDataInput.readLong();
    }

    /**
     * Please refer JSR 118.
     */

    public float readFloat() throws IOException
    {
        return iDataInput.readFloat();

    }

    /**
     * Please refer JSR 118.
     */

    public double readDouble() throws IOException
    {
        return iDataInput.readDouble();
    }

    /**
     * Please refer JSR 118.
     */

    public String readUTF() throws IOException
    {
        return iDataInput.readUTF();
    }

}
