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
* Description:  Provides API to Create APDU connection
*
*/


package com.nokia.mj.impl.gcf.protocol.apdu;


import java.io.IOException;
import javax.microedition.apdu.APDUConnection;
import com.nokia.mj.impl.gcf.utils.J9GcfConnectionBase;

public class Connection extends J9GcfConnectionBase implements APDUConnection
{
    private APDUConnection iConnection = null;

    public Connection()
    {
    }

    protected javax.microedition.io.Connection createConnection(
        String aName, int aMode, boolean aTimeouts)
    throws IOException
    {
        iConnection = (APDUConnection)(new Protocol())
                      .openConnection("apdu:"+aName, aMode, aTimeouts);
        return (javax.microedition.io.Connection)iConnection;
    }



    // Begin methods from Connection

    public void close()
    throws IOException
    {
        iConnection.close();
    }

    // End methods from Connection

    // Begin methods from APDUConnection

    public byte[] changePin(int pinID)
    throws IOException
    {
        return iConnection.changePin(pinID);
    }

    public byte[] disablePin(int pinID)
    throws IOException
    {
        return iConnection.disablePin(pinID);
    }

    public byte[] enablePin(int pinID)
    throws IOException
    {
        return iConnection.enablePin(pinID);
    }

    public byte[] enterPin(int pinID)
    throws IOException
    {
        return iConnection.enterPin(pinID);
    }

    public byte[] exchangeAPDU(byte[] commandAPDU)
    throws IOException
    {
        return iConnection.exchangeAPDU(commandAPDU);
    }

    public byte[] getATR()
    {
        return iConnection.getATR();
    }

    public byte[] unblockPin(int blockedPinID, int unblockingPinID)
    throws IOException
    {
        return iConnection.unblockPin(blockedPinID, unblockingPinID);
    }

    // End methods from APDUConnection

}
