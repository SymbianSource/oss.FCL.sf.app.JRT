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
* Description:  rtsp protocol
*
*/


package com.nokia.microedition.media.protocol.rtsp;

import javax.microedition.media.MediaException;
import com.nokia.mj.impl.rt.legacy.Security;
import com.nokia.microedition.media.Locator;
import com.nokia.microedition.media.InternalPlayer;
import com.nokia.microedition.media.protocol.NativeBase;

/**
 * This class presents rtsp protocol and adds security prompt.
 */
public class Protocol extends NativeBase
{
    private static final String RTSP_CONNECTION_PERMISSION =
        "javax.microedition.io.Connector.rtsp";

    /**
     * Default constructor.
     */
    public Protocol()
    {
    }

    /**
     * From interface Protocol
     * @see Protocol
     */
    public InternalPlayer createPlayer(Locator aLocator)
    throws java.io.IOException,
                MediaException,
                java.lang.SecurityException
    {
        String permArgs[] = { aLocator.getLocatorString() };
        Security.ensurePermission(RTSP_CONNECTION_PERMISSION,
                                  RTSP_CONNECTION_PERMISSION,
                                  permArgs);
        return super.createPlayer(aLocator);
    }
}
