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


package com.nokia.mj.impl.gcf.utils;

import com.ibm.oti.connection.CreateConnection;
import java.io.IOException;
import javax.microedition.io.Connection;

/**
 * This is the base class for the Connection classes that implement
 * com.ibm.oti.connection.CreateConnection interface that is needed
 * for extending J9 GCF with new protocols.
 *
 * @author Nokia Corporation
 * @version 1.0
 */

public abstract class J9GcfConnectionBase implements CreateConnection
{

    public J9GcfConnectionBase()
    {
    }

    // Begin methods from CreateConnection

    public void setParameters(String aName, int aMode, boolean aTimeouts)
    throws IOException
    {
        throw new IOException(
            "Connection has to be initialized with setParameters2 method");
    }

    synchronized public Connection setParameters2(
        String aName, int aMode, boolean aTimeouts)
    throws IOException
    {
        return createConnection(aName, aMode, aTimeouts);
    }

    // End methods from CreateConnection

    // Inheriting Connection class should override this method
    // with the following piece of code in the same package
    // where the Protocol class exists:
    //
    //    if (iProtocol == null) { iProtocol = new Protocol(); }
    //    return iProtocol.openConnection(
    //        PROTOCOL_SCHEME + ":" + aName, aMode, aTimeouts);
    //
    protected abstract Connection createConnection(
        String aName, int aMode, boolean aTimeouts) throws IOException;

}
