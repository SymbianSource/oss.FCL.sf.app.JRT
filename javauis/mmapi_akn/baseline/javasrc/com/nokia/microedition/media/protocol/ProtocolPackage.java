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
 * Base class for Protocols those can contain different Protocols
 * e.g. "capture" protocol can handle also sub-protocols, depending
 * on what type of Player is created.
 * This class instantiates those automatically
 */
public class ProtocolPackage implements Protocol
{
    /**
     * Protected to allow this class class to be extended.
     */
    protected ProtocolPackage()
    {
    }

    /**
     * Implementation from Protocol class, this method will create
     * new protocol and call it's createPlayer
     * @param aLocator identifies player
     * @return new player
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
                Class.forName(ProtocolFactory.PROTOCOL_BASE_PACKAGE +
                              getProtocolName(aLocator) + "." +
                              getPackageName(aLocator) +
                              ProtocolFactory.PROTOCOL_CLASS_NAME);
            Protocol protocol = (Protocol)protocolClass.newInstance();
            player = protocol.createPlayer(aLocator);
        }
        catch (InstantiationException ie)
        {
            throw new MediaException("Internal error: " + ie);
        }
        catch (IllegalAccessException iae)
        {
            throw new MediaException("Internal error: " + iae);
        }
        catch (ClassNotFoundException cnfe)
        {
            throw new MediaException("Unable to create Player with locator: "
                                     + aLocator.getLocatorString());
        }
        return player;
    }

    /**
     * Returns the name of the package, this will be used for
     * creating sub-protocol with name. e.g. with locator "capture://audio"
     * this will return "audio"
     *
     * This method can be overridden if there is need for returning something else
     * than middle part of the locator
     * @return Middle part of the locator
     */
    protected String getPackageName(Locator aLocator) throws MediaException
    {
        return aLocator.getMiddlePart();
    }

    /**
     * Returns the name of the protocol, this will be used for
     * creating sub-protocol with name. e.g. with locator "capture://audio"
     * this will return "capture"
     *
     * This method can be overridden if there is need for returning something else
     * than protocol of the locator
     * @return Protocol of the locator
     */
    protected String getProtocolName(Locator aLocator) throws MediaException
    {
        return aLocator.getProtocol();
    }

}
