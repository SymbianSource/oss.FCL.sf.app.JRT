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
* Description:  This class is capture://radio protocol
 *
*/

package com.nokia.microedition.media.protocol.capture.radio;

import com.nokia.microedition.media.Locator;
import com.nokia.microedition.media.InternalPlayer;
import com.nokia.microedition.media.PlayerImpl;
import javax.microedition.media.MediaException;
import com.nokia.mj.impl.rt.legacy.NativeError;
import com.nokia.microedition.media.ManagerImpl;
import com.nokia.amms.control.tuner.TunerControlImpl;

/**
 * This class is capture://radio protocol.
 */
public class Protocol
        implements com.nokia.microedition.media.protocol.Protocol
{
    // Constants
    private static final String TUNER_CONTROL_NAME =
        "javax.microedition.amms.control.tuner.TunerControl";

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
        //   JDEBUG( "AMMS Tuner Protocol createPlayer" );

        String locatorParameters = aLocator.getParameters();

        // Create native tuner player. The native tuner player
        // creates a native tuner control and adds it to its list.
        // Handle for the native player are returned.
        int playerHandle = 0;
        playerHandle = _createNativeTunerPlayer(
                           ManagerImpl.getEventSource(),
                           locatorParameters);

//      JDEBUG( "AMMS Tuner Protocol createPlayer playerHandle = " + playerHandle  );

        NativeError.check(playerHandle);

        PlayerImpl tunerPlayer = new PlayerImpl(playerHandle);

        return tunerPlayer;
    }

    /**
     * Creates native tuner player.
     * Native tuner player creates and adds native tuner control.
     *
     * @param aEventSourceHandle Handle to event source.
     * @param aLocatorParameters
     * @return Error code.
     */
    private native int _createNativeTunerPlayer(int aEventSourceHandle,
            String aLocatorParameters);
}
