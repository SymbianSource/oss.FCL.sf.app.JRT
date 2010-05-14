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


package com.nokia.microedition.media;

import javax.microedition.media.MediaException;
import javax.microedition.media.protocol.DataSource;

/**
 * Factory for creating native and java instances of the players
 */
public class NativePlayerFactory
{
    /**
     * Do not allow contruction.
     */
    private NativePlayerFactory()
    {
    }

    /**
     * Creates native and java instances with Locator
     * @param aLocator Locator used for creating native player
     * @param aSource The DataSource that provides the media content.
     * @return new instance of <code>InternalPlayer</code>
     */
    public static InternalPlayer createPlayer(Locator aLocator,
            DataSource aSource)
    throws java.io.IOException,
                MediaException,
                java.lang.SecurityException
    {
        int playerHandle = _createPlayerLocator(ManagerImpl.getEventSource(),
                                                ManagerImpl.getHandle(),
                                                aLocator.getProtocol(),
                                                aLocator.getMiddlePart(),
                                                aLocator.getParameters());
        return createPlayer(playerHandle, aSource);
    }

    /**
     * Creates native and java instances with Locator
     * @param aLocator Locator used for creating native player
     * @return new instance of <code>InternalPlayer</code>
     */
    public static InternalPlayer createPlayer(Locator aLocator)
    throws java.io.IOException,
                MediaException,
                java.lang.SecurityException
    {
        return createPlayer(aLocator, null);
    }

    /**
     * Creates native and java instances with content-type
     * @param aContentType Content type for creating native counter-part
     * @param aSource The DataSource that provides the media content.
     * @return new instance of <code>InternalPlayer</code>
     */
    public static InternalPlayer createPlayer(String aContentType,
            DataSource aSource)
    throws java.io.IOException,
                MediaException,
                java.lang.SecurityException
    {
        int playerHandle = _createPlayerContentType(ManagerImpl.getEventSource(),
                           ManagerImpl.getHandle(),
                           aContentType);
        return createPlayer(playerHandle, aSource);
    }

    /**
     * Creates native and java instances with header data
     * @param aHeaderData HeaderData used for recognizing content
     * @param aSource The DataSource that provides the media content.
     * @return new instance of <code>InternalPlayer</code>
     */
    public static InternalPlayer createPlayer(byte[] aHeaderData,
            DataSource aSource)
    throws java.io.IOException,
                MediaException,
                java.lang.SecurityException
    {
        int playerHandle = _createPlayerHeaderData(ManagerImpl.getEventSource(),
                           ManagerImpl.getHandle(),
                           aHeaderData);
        return createPlayer(playerHandle, aSource);
    }

    /**
     * Creates java instance from native handle and DataSource
     */
    private static InternalPlayer createPlayer(int aPlayerHandle,
            DataSource aSource)
    throws java.io.IOException,
                MediaException,
                java.lang.SecurityException
    {
        // if aPlayerHandle is 0 null will be returned
        InternalPlayer player = null;

        if (aPlayerHandle < 0)
        {
            // negative handle indicates native error code
            throw new MediaException(
                "Could not create player, Symbian OS error: " +
                aPlayerHandle);
        }
        else if (aPlayerHandle > 0)
        {
            // Native player was created
            if (aSource == null)
            {
                player = new PlayerImpl(aPlayerHandle);
            }
            else
            {
                player = new SourcePlayer(aSource, aPlayerHandle);
            }
        }
        return player;
    }

    private static native int _createPlayerLocator(int aEventSource,
            int aManagerHandle,
            String aProtocol,
            String aMiddlePart,
            String aParameters);

    private static native int _createPlayerContentType(int aEventSource,
            int aManagerHandle,
            String aContentType);

    private static native int _createPlayerHeaderData(int aEventSource,
            int aManagerHandle,
            byte[] aHeaderData);
}
