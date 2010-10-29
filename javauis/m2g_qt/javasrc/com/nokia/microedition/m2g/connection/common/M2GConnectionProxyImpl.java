/*
* Copyright (c) 2005-2009 Nokia Corporation and/or its subsidiary(-ies).
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

package com.nokia.microedition.m2g.connection.common;

import javax.microedition.io.Connector;
import javax.microedition.io.StreamConnection;
import javax.microedition.io.ConnectionNotFoundException;
import java.io.IOException;
import java.lang.IllegalArgumentException;
import com.nokia.microedition.m2g.connection.*;

/**
 * Connection proxy class
 */
public class M2GConnectionProxyImpl extends M2GConnectionProxy

{
    //--------------------------------------------------
    // METHODS
    //--------------------------------------------------
    /**
     * Ctor.
     */
    public M2GConnectionProxyImpl()
    {
    }

    /**
     * Open a connection.
     * @see M2GConnectionProxy#open()
       */
    public void open(M2GUrlTokenizer aTokenizer) throws IOException
    {
        iBaseUrl   = aTokenizer.getBaseUrl();
        iSuffixUrl = null; // can't have DRM suffix when opening from stream
        iConnectionPolicy = new M2GConnectionPolicyImpl();
        try
        {
            iConnection = (StreamConnection)Connector.open(
                              aTokenizer.getUrl());
            iInputStream = iConnection.openInputStream();
        }
        catch (ConnectionNotFoundException e)
        {
            throw new IllegalArgumentException(e.getMessage());
        }
    }
}
