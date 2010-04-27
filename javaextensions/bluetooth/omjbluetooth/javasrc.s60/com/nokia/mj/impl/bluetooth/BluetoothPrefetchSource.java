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

import java.io.InputStream;

public class BluetoothPrefetchSource
{
    private BluetoothStreamer iBluetoothStreamer;

    private InputStream iInputStream;

    private BluetoothL2CAPSource iL2capSource;

    private boolean isClosed;

    private Object iReadLock;

    public BluetoothPrefetchSource(BluetoothStreamer streamer,
                                   InputStream iStream)
    {
        iBluetoothStreamer = streamer;
        iInputStream = iStream;
        isClosed = false;
        iReadLock = new Object();
    }

    public BluetoothPrefetchSource(BluetoothL2CAPSource source)
    {
        iL2capSource = source;
        isClosed = false;
        iReadLock = new Object();
    }

    public Object getLock()
    {
        return iReadLock;
    }

    public void setClosed()
    {
        isClosed = true;
    }

    public boolean isClosed()
    {
        return isClosed;
    }

    public BluetoothL2CAPSource getL2Source()
    {
        return iL2capSource;
    }

    public BluetoothStreamer getStreamer()
    {
        return iBluetoothStreamer;
    }

    public InputStream getInputStream()
    {
        return iInputStream;
    }
}