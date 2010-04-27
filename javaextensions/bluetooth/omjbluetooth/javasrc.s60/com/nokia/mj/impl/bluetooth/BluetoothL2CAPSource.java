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

package com.nokia.mj.impl.bluetooth;

import java.io.IOException;
import com.nokia.mj.impl.bluetooth.utils.DebugLog;

/**
 * Responsible for fetching data from the native side in a way suitable to
 * L2CAP.
 *
 * <pre>
 * Assumptions made when calling native reads:
 *     1. Native reads at max a buffer of size equal to that of negotiated
 *        receive MTU.
 *     2. Available() call to BluetoothStackS60 returns some positive value if
 *        data has already been read on the native side else returns 0.
 *     3. Call to clientReceive() in BlutoothStackS60 blocks until all or some
 *        of the data requested is available.
 * </pre>
 */
public class BluetoothL2CAPSource
{
    private BluetoothStackS60 iDataSource;

    private long iNativeHandle;

    private int iReadSize;

    private byte[] iReadBuffer;

    public BluetoothL2CAPSource(BluetoothStackS60 stack, long nativeHandle)
    {
        iNativeHandle = nativeHandle;
        iDataSource = stack;
        try
        {
            iReadSize = iDataSource.l2GetReceiveMTU(iNativeHandle);
        }
        catch (IOException ex)
        {
            iReadSize = 672;
            DebugLog.log("BluetoothL2CAPSource: Fetching receive MTU failed."
                         + " Setting buffer to " + iReadSize);
        }
        iReadBuffer = new byte[iReadSize];
    }

    public BluetoothL2CAPSource(BluetoothStackS60 stack, long nativeHandle,
                                int receiveMtu)
    {
        iNativeHandle = nativeHandle;
        iDataSource = stack;

        iReadSize = receiveMtu;
        iReadBuffer = new byte[iReadSize];
    }

    /**
     * Reads data required from native stack.
     * <p>
     * SPEC: Unlike read() on an java.io.InputStream, if the size of inBuf is
     * smaller than ReceiveMTU, then the portion of the L2CAP payload that will
     * fit into inBuf will be placed in inBuf, the rest will be discarded. If
     * the application is aware of the number of bytes (less than ReceiveMTU) it
     * will receive in any transaction, then the size of inBuf can be less than
     * ReceiveMTU and no data will be lost.
     *
     * @param handle
     *            native side handle
     * @param inBuf
     *            buffer requested by the application
     * @return the actual number of bytes read received.
     * @throws IOException
     *             in case an IO error occured
     */
    public int receive(long handle, byte[] inBuf) throws IOException
    {
        DebugLog.log("Java: BluetoothL2CAPSource: receive");
        int readBytes = iDataSource.clientReceive(handle, iReadBuffer);
        int lengthToCopy = (readBytes < inBuf.length) ? readBytes
                           : inBuf.length;

        System.arraycopy(iReadBuffer, 0, inBuf, 0, lengthToCopy);
        // Note: Discarding the rest of the data as inBuf asked was less than
        // the MTU value.
        DebugLog.log("Java: BluetoothL2CAPSource: receive: Return "
                     + lengthToCopy);
        return lengthToCopy;
    }
}
