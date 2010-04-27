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

import com.nokia.mj.impl.gcf.utils.InputStreamBase;
import com.nokia.mj.impl.gcf.utils.StreamConnectionBase;

public class BluetoothInputStream extends InputStreamBase
{
    BluetoothInputStream(StreamConnectionBase aEndPoint, Object aCloseLock,
                         boolean aSupportMarkAndReset)
    {
        super(aEndPoint, aCloseLock, aSupportMarkAndReset);
    }

    protected synchronized int fillBytes(byte[] aBytes, int aOffset, int aLength)
    throws IOException
    {
        return readFromStreamBase(aBytes, aOffset, aLength);
    }
}