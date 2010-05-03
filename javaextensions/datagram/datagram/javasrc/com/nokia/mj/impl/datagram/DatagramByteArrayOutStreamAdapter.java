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

import java.io.ByteArrayOutputStream;
import java.io.IOException;

/**
 * Datagram OutputStream Adapter class.
 *
 */

public class DatagramByteArrayOutStreamAdapter extends ByteArrayOutputStream
{

    /**
     * Constructs the DatagramOutStreamAdapter.
     *
     * @param aBuf the buffer used for the datagram
     */

    DatagramByteArrayOutStreamAdapter(byte[] aBuf)
    {
        buf = aBuf;
        count = aBuf.length; // when created buffer will be marked full, any
        // write should be preceded by reset()
    }

    /**
     * Returns the size of the internal buffer.
     *
     * @return buffer size
     */
    public int getLength()
    {
        return buf.length;
    }


    public synchronized void write(int aInt)
    {
        if (count == buf.length)
        {
            throw new ArrayIndexOutOfBoundsException();
        }
        super.write(aInt);
    }


    public synchronized void write(byte aData[], int aOffset, int aLength)
    {
        if (count + aLength > buf.length)
        {
            throw new ArrayIndexOutOfBoundsException(buf.length);
        }
        super.write(aData, aOffset, aLength);
    }

}
