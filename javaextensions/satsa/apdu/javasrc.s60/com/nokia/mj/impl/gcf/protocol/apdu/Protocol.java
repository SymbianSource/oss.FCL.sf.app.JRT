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
* Description:  Implements the protocol interface
*
*/



package com.nokia.mj.impl.gcf.protocol.apdu;

import java.io.IOException;
import javax.microedition.io.Connection;
import com.nokia.mj.impl.satsa.APDUConnectionImpl;
import com.nokia.mj.impl.rt.support.Jvm;

/**
 * Creates APDUConnectionImpl class for communicating with smart card.
 */
public class Protocol
{

    static
    {
        try
        {
            Jvm.loadSystemLibrary("javasatsa");
        }
        catch (Exception e)
        {

        }
    }

    public Protocol()
    {
    }

    public javax.microedition.io.Connection openConnection(String aName, int aMode, boolean aTimeouts)
    throws IOException
    {
        return new APDUConnectionImpl(aName, aMode, aTimeouts);
    }
}