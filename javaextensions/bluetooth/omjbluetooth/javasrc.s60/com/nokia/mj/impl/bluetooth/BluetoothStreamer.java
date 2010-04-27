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
import java.io.InputStream;
import javax.microedition.io.Connector;
import com.nokia.mj.impl.gcf.utils.StreamConnectionBase;
import com.nokia.mj.impl.bluetooth.utils.DebugLog;

/**
 * For some wierd reason, we always need to do prefetch in case of Bluetooth.
 * Extending StreamConnectionBase to use its prefetch capabilities.
 */
public class BluetoothStreamer extends StreamConnectionBase
{
    private BluetoothStackS60 iDataSource;

    private long iNativeHandle;

    public BluetoothStreamer(BluetoothStackS60 aStack, long aHandle,
                             int aBufferSize)
    {
        super("BluetoothL2CAP", Connector.READ_WRITE, false, true, aBufferSize);
        iDataSource = aStack;
        iNativeHandle = aHandle;
    }

    public BluetoothStreamer(BluetoothStackS60 aStack, long aHandle)
    {
        super("BluetoothRFCOMM", Connector.READ_WRITE, false); // , 512, true);
        iDataSource = aStack;
        iNativeHandle = aHandle;
    }

    /**
     * If create native peer returns 0, StreamBase does not make calls to native
     * side. But also needs writeData and readData to be overridden and also
     * cancelRead and cancelWrite.
     */
    protected int createNativePeer(String aUri, int aMode, boolean aTimeouts)
    {
        return 0;
    }

    /**
     * Overriding writeData to prevent streams to go to native by itself.
     */
    public int writeData(byte[] aBytes, int aOffset, int aLength)
    {
        // Wll not be called either
        // return iDataSource.clientSend(iNativeHandle, aBytes, 0,
        // aBytes.length);
        return 0;
    }

    /**
     * Overriding writeData to prevent streams to go to native by itself.
     */
    public int readData(byte[] aBytes)
    {
        try
        {
            DebugLog.debug("BluetoothStreamer: readData: Goting to recive");
            return iDataSource.clientReceive(iNativeHandle, aBytes);
        }
        catch (IOException ex)
        {
            DebugLog.debug("BluetoothStreamer: readData: IOException caught");
            // General error
            return -1;
        }
        catch (Exception e)
        {
            DebugLog.debug("BluetoothStreamer: readData: "
                           + "Unexpected Exception caught Ignoring");
            return -1;
        }
        // return 0;
    }

    public void stopReading()
    {
        iDataSource.clientClose(iNativeHandle);
    }

    public void cancelWrite()
    {
        // Will never be called
    }

    public InputStream openInputStream() throws IOException
    {
        synchronized (iReadOperation)
        {
            synchronized (iCloseOperation)
            {
                ensureOpen();
                if (iInputStream == null)
                {
                    iInputStream = new BluetoothInputStream(this,
                                                            iCloseOperation, iSupportMarkAndReset);
                }
                else
                { // TCK2 compliance
                    throw new IOException();
                }

                if (iDoPrefetch)
                {
                    startPrefetch();
                }
            }
        }
        return iInputStream;
    }
}