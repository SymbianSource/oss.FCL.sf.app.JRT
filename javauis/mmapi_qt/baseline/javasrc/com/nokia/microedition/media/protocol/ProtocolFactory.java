/*
* Copyright (c) 2005 Nokia Corporation and/or its subsidiary(-ies).
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


package com.nokia.microedition.media.protocol;

import com.nokia.microedition.media.InternalPlayer;
import com.nokia.microedition.media.Locator;
import javax.microedition.media.MediaException;

/**
 * Factory for creating the Protocols
 */
public class ProtocolFactory
{
    private Protocol iConnectorProtocol = null;

    public static final String PROTOCOL_CLASS_NAME = ".Protocol";
    public static final String PROTOCOL_BASE_PACKAGE =
        "com.nokia.microedition.media.protocol.";

    /**
     * Constructor
     */
    public ProtocolFactory()
    {
        iConnectorProtocol = new ConnectorProtocol();
    }

    /**
     * Factory method for creating player from locator
     * Tries to load assosiated <code>Protocol</code> first, if this fails
     * then we try to create player by using <code>Connector</code>
     * @param aLocator Media locator
     * @return Player according to locator or null if cannot create
     */
    public InternalPlayer createPlayer(Locator aLocator)
    throws java.io.IOException,
                MediaException,
                java.lang.SecurityException
    {
        InternalPlayer player = null;
        try
        {
            Class protocolClass =
                Class.forName(PROTOCOL_BASE_PACKAGE +
                              aLocator.getProtocol() + PROTOCOL_CLASS_NAME);
            Protocol protocol = (Protocol)protocolClass.newInstance();
            player = protocol.createPlayer(aLocator);
        }
        catch (InstantiationException ie)
        {
            throw new MediaException("Instantiation failed: " + ie);
        }
        catch (IllegalAccessException iae)
        {
            throw new MediaException("Illegal access: " + iae);
        }
        catch (ClassNotFoundException cnfe)
        {
            // we did not find specified Protocol, trying to create player
            // with connector
            player = iConnectorProtocol.createPlayer(aLocator);
        }
        return player;
    }
}

