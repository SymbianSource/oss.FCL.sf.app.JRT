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


package com.nokia.mj.impl.installer.integrityservice;

import com.nokia.mj.impl.installer.utils.FileUtils;
import com.nokia.mj.impl.installer.utils.Log;

import java.io.InputStream;
import java.io.IOException;
import java.io.OutputStream;

/*
Journal file format:
  journalfile := operation*
  operation := operator operand+ status
  operand := drive uniqid file
  drive := <one byte telling drive, a=0, b=1, ...>
  uniqid := <four bytes telling drive unique id, MSB first>
  file := length <UTF-8 encoded string as byte array>
  length := <2 bytes telling the length of byte array that follows, MSB first>
  status := <byte telling if operation execution was succesful, 0=ok, -1=failure>
  operator := create | delete | copy | move | update
  create := <0x1>
  delete := <0x2>
  copy := <0x3>
  move := <0x4>
  update := <0x5>
*/

/**
 * Operation class implements supported integrity service operations
 * and their counter-operations.
 * @see Operand
 */
class Operation
{
    public static final byte CREATE = 0x1;
    public static final byte DELETE = 0x2;
    public static final byte COPY = 0x3;
    public static final byte MOVE = 0x4;
    public static final byte UPDATE = 0x5;

    public static final byte STATUS_OK = 0;
    public static final byte STATUS_FAIL = -1;

    private static final String CH_ENC = "UTF-8";

    private byte iOperator = 0;
    private Operand[] iOperands = null;
    private byte iStatus = STATUS_OK;

    Operation(byte aOperator)
    {
        iOperator = aOperator;
        iOperands = new Operand[] { null, null, null };
        iStatus = STATUS_OK;
    }

    Operation(byte aOperator, String aOperand) throws IOException
    {
        this(aOperator);
        iOperands[0] = new Operand(aOperand);
    }

    Operation(byte aOperator, String aOperand1, String aOperand2) throws IOException
    {
        this(aOperator);
        iOperands[0] = new Operand(aOperand1);
        iOperands[1] = new Operand(aOperand2);
    }

    Operation(byte aOperator, String aOperand1, String aOperand2, String aOperand3)
    throws IOException
    {
        this(aOperator);
        iOperands[0] = new Operand(aOperand1);
        iOperands[1] = new Operand(aOperand2);
        iOperands[2] = new Operand(aOperand3);
    }

    boolean execute()
    {
        if (IntegrityService.operationsLogged())
        {
            Log.log("IS Op Execute: " + this.toString());
        }
        boolean result = true;
        String[] files = getFiles();
        switch (iOperator)
        {
        case CREATE:
            // nop
            break;
        case DELETE:
            result = FileOps.move(files[0], files[1]);
            break;
        case MOVE:
            if (FileUtils.exists(files[1]))
            {
                result = FileOps.move(files[1], files[2]);
            }
            if (result)
            {
                result = FileOps.move(files[0], files[1]);
            }
            break;
        case COPY:
            if (FileUtils.exists(files[1]))
            {
                result = FileOps.move(files[1], files[2]);
            }
            if (result)
            {
                result = FileOps.copyAll(files[0], files[1]);
            }
            break;
        case UPDATE:
            result = FileOps.copy(files[0], files[1]);
            break;
        }
        return result;
    }

    boolean cancel()
    {
        if (IntegrityService.operationsLogged())
        {
            Log.log("IS Op Cancel: " + this.toString());
        }
        boolean result = true;
        String[] files = getFiles();
        switch (iOperator)
        {
        case CREATE:
            result = iOperands[0].canRollback();
            if (result)
            {
                result = FileOps.deleteAll(files[0]);
            }
            break;
        case DELETE:
            result = iOperands[0].canRollback() && iOperands[1].canRollback();
            if (result)
            {
                result = FileOps.move(files[1], files[0]);
            }
            break;
        case MOVE:
            result = iOperands[0].canRollback() && iOperands[1].canRollback();
            if (result)
            {
                result = FileOps.move(files[1], files[0]);
            }
            if (result && FileUtils.exists(files[2]))
            {
                result = iOperands[2].canRollback() && iOperands[1].canRollback();
                if (result)
                {
                    result = FileOps.move(files[2], files[1]);
                }
            }
            break;
        case COPY:
            if (FileUtils.exists(files[2]))
            {
                result = iOperands[2].canRollback() && iOperands[1].canRollback();
                if (result)
                {
                    result = FileOps.move(files[2], files[1]);
                }
            }
            else
            {
                result = iOperands[1].canRollback();
                if (result)
                {
                    result = FileOps.deleteAll(files[1]);
                }
            }
            break;
        case UPDATE:
            result = iOperands[0].canRollback() && iOperands[1].canRollback();
            if (result)
            {
                result = FileOps.move(files[1], files[0]);
            }
            break;
        }
        return result;
    }

    byte getStatus()
    {
        return iStatus;
    }

    void setStatus(byte aStatus)
    {
        iStatus = aStatus;
    }

    void read(InputStream aIn) throws IOException
    {
        readOperator(aIn);
        switch (iOperator)
        {
        case CREATE:
            readOperand(aIn, 0);
            break;
        case DELETE:
            // fall through
        case UPDATE:
            readOperand(aIn, 0);
            readOperand(aIn, 1);
            break;
        case MOVE:
            // fall through
        case COPY:
            readOperand(aIn, 0);
            readOperand(aIn, 1);
            readOperand(aIn, 2);
            break;
        }
        readStatus(aIn);
    }

    /**
     * Client must call write method before operation execution.
     */
    void write(OutputStream aOut) throws IOException
    {
        writeOperator(aOut);
        switch (iOperator)
        {
        case CREATE:
            writeOperand(aOut, 0);
            break;
        case DELETE:
            // fall through
        case UPDATE:
            writeOperand(aOut, 0);
            writeOperand(aOut, 1);
            break;
        case MOVE:
            // fall through
        case COPY:
            writeOperand(aOut, 0);
            writeOperand(aOut, 1);
            writeOperand(aOut, 2);
            break;
        }
        aOut.flush();
    }

    private void readOperator(InputStream aIn) throws IOException
    {
        iOperator = (byte)aIn.read();
    }

    private void readOperand(InputStream aIn, int aIndex) throws IOException
    {
        // Read drive.
        int drive = (byte)aIn.read(); // Must be casted to byte, otherwise -1 ==> 255
        // Read drive unique id.
        int id = 0;
        for (int i = 0; i < 4; i++)
        {
            id <<= 8;
            id |= aIn.read();
        }
        // Read filename.
        int length1 = aIn.read();
        int length2 = aIn.read();
        int length = ((length1 << 8) | length2);
        byte[] bytes = new byte[length];
        int readLength = aIn.read(bytes);
        if (readLength != length)
        {
            throw new IOException("IntegrityService journal file is corrupted, " +
                                  "read incorrect operand length, expected "
                                  + length + ", got " + readLength);
        }
        iOperands[aIndex] = new Operand(drive, id, new String(bytes, CH_ENC));
    }

    private void readStatus(InputStream aIn) throws IOException
    {
        if (aIn.available() > 0)
        {
            iStatus = (byte)aIn.read();
        }
        else
        {
            // Status has not been written, assume operation has failed.
            iStatus = STATUS_FAIL;
        }
        if (iStatus == STATUS_OK || iStatus == STATUS_FAIL)
        {
            // OK, got valid status.
        }
        else
        {
            throw new IOException("IntegrityService journal file is corrupted, invalid status: " + iStatus);
        }
    }

    private void writeOperator(OutputStream aOut) throws IOException
    {
        aOut.write(iOperator);
    }

    private void writeOperand(OutputStream aOut, int aIndex) throws IOException
    {
        // Write drive.
        int drive = iOperands[aIndex].getDrive();
        aOut.write(drive);
        // Write drive unique id.
        int id = iOperands[aIndex].getDriveUniqId();
        int[] ids = new int[4];
        for (int i = 0; i < 4; i++)
        {
            ids[i] = (id & 0xff);
            id >>= 8;
        }
        aOut.write(ids[3]);
        aOut.write(ids[2]);
        aOut.write(ids[1]);
        aOut.write(ids[0]);
        // Write filename.
        byte[] bytes = iOperands[aIndex].getFile().getBytes(CH_ENC);
        int length1 = (bytes.length >> 8);
        int length2 = bytes.length;
        aOut.write(length1);
        aOut.write(length2);
        aOut.write(bytes);
    }

    /**
     * Client must call writeStatus method after operation has been executed.
     */
    void writeStatus(OutputStream aOut) throws IOException
    {
        aOut.write(iStatus);
    }

    private String[] getFiles()
    {
        String[] files = new String[iOperands.length];
        for (int i = 0; i < iOperands.length; i++)
        {
            if (iOperands[i] != null)
            {
                files[i] = iOperands[i].getFile();
            }
        }
        return files;
    }

    public String toString()
    {
        StringBuffer buf = new StringBuffer();
        switch (iOperator)
        {
        case CREATE:
            buf.append("CREATE");
            break;
        case DELETE:
            buf.append("DELETE");
            break;
        case COPY:
            buf.append("COPY");
            break;
        case MOVE:
            buf.append("MOVE");
            break;
        case UPDATE:
            buf.append("UPDATE");
            break;
        default:
            buf.append("?");
        }
        for (int i = 0; i < iOperands.length; i++)
        {
            if (iOperands[i] == null)
            {
                break;
            }
            buf.append("|");
            buf.append(iOperands[i]);
        }
        return buf.toString();
    }
}
