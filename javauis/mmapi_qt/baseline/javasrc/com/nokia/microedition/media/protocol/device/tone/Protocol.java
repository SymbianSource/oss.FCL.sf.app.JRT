/*
* Copyright (c) 2002 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  This class is device://tone protocol
*
*/


package com.nokia.microedition.media.protocol.device.tone;

import com.nokia.microedition.media.InternalPlayer;
import com.nokia.microedition.media.PlugIn;
import com.nokia.microedition.media.Locator;
import com.nokia.microedition.media.tone.TonePlayer;
import javax.microedition.media.MediaException;
import javax.microedition.media.Manager;
import java.io.IOException;
import javax.microedition.media.protocol.DataSource;

/**
 * This class is device://tone protocol. This class also implements PlugIn
 * interface which is used from ManagerImpl.
 */
public class Protocol
        implements com.nokia.microedition.media.protocol.Protocol,
        PlugIn
{
    // Used to recognize supported locators.
    private static final String TONE_FILE_EXTENSION = ".jts";

    // Used to get supported protocols. jts supports same protocols than midi.
    private static final String MIDI_CONTENT_TYPE = "audio/midi";

    /**
     * Default constructor.
     */
    public Protocol()
    {
    }

    /**
     * From Protocol
     */
    public InternalPlayer createPlayer(Locator aLocator)
    throws java.io.IOException,
                MediaException,
                java.lang.SecurityException
    {
        if (aLocator.getParameters() != null)
        {
            // device://tone may not have properties.
            throw new MediaException(
                "Could not create player, parameters not supported: " +
                aLocator.getParameters());
        }
        return new TonePlayer();
    }

    /**
     * From PlugIn
     */
    public String[] getSupportedContentTypes(String aProtocol)
    {
        String[] types = new String[ 0 ];

        if ((aProtocol == null) ||
                isSupportedProtocol(aProtocol))
        {
            types = new String[ 1 ];
            types[ 0 ] = TonePlayer.TONE_CONTENT_TYPE;
        }
        // else empty array is returned
        return types;
    }

    /**
     * Returns true if protocol is supported.
     * @return true if protocol is supported.
     */
    private boolean isSupportedProtocol(String aProtocol)
    {
        // get all supported protocols
        String[] protocols = getSupportedProtocols(null);
        boolean isSupported = false;
        // protocol is supported if it is found from protols array
        for (int i = 0; i < protocols.length &&
                !isSupported; i++)
        {
            if (protocols[ i ].equals(aProtocol))
            {
                isSupported = true;
            }
        }
        return isSupported;
    }

    /**
     * From PlugIn
     */
    public String[] getSupportedProtocols(String aContentType)
    {
        String[] protocols = new String[ 0 ];
        if ((aContentType == null) ||
                aContentType.equals(TonePlayer.TONE_CONTENT_TYPE))
        {
            // tone and midi supports same protocols.
            protocols = Manager.getSupportedProtocols(
                            MIDI_CONTENT_TYPE);
        }
        // else empty array is returned
        return protocols;
    }

    /**
     * From PlugIn
     */
    public InternalPlayer createPlayer(DataSource aDataSource)
    throws MediaException, IOException
    {
        InternalPlayer player = null;
        String contentType = aDataSource.getContentType();
        String locator = aDataSource.getLocator();
        if ((contentType != null &&
                contentType.equals(TonePlayer.TONE_CONTENT_TYPE)) ||
                (locator != null &&
                 locator.endsWith(TONE_FILE_EXTENSION)))
        {
            player = new TonePlayer(aDataSource);
        }
        return player;
    }

    /**
     * From PlugIn. Empty implementation.
     */
    public void preparePlayer(InternalPlayer aPlayer) throws MediaException
    {
        // tone does not extend existing players
    }
}

