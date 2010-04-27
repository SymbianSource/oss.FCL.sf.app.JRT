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

package com.nokia.mj.impl.gcf.protocol.btspp;

import java.io.IOException;

import com.intel.bluetooth.MicroeditionConnector;
import com.nokia.mj.impl.rt.support.Jvm;

public class Protocol
{
    public Protocol()
    {
    }

    public javax.microedition.io.Connection openConnection(String aUri,
            int aMode, boolean aTimeouts) throws IOException
    {
        return MicroeditionConnector.open("btspp:" + aUri, aMode, aTimeouts);
    }
}
