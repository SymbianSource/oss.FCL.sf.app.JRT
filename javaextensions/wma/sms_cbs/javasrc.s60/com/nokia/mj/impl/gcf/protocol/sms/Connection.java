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

package com.nokia.mj.impl.gcf.protocol.sms;

import com.nokia.mj.impl.gcf.utils.J9GcfConnectionBase;
import java.io.IOException;

/**
 * This class is specific to j9 Vm. It derives from J9GcfConnectionBase IBM VM's
 * class, so that when Connector.open is called, in case of j9 VM, the VM finds
 * com.nokia.mj.impl.gcf.protocol."scheme".Connection class and calls the
 * createConnection method to return a SmsConnection object .
 */
public class Connection extends J9GcfConnectionBase
{
    private static Protocol iSmsProtocol = null;

    public Connection()
    {

    }

    protected javax.microedition.io.Connection createConnection(String aName,
            int aMode, boolean aTimeouts) throws IOException
    {
        if (iSmsProtocol == null)
        {
            iSmsProtocol = new Protocol();
        }
        return iSmsProtocol.openConnection(aName, aMode, aTimeouts);
    }
}