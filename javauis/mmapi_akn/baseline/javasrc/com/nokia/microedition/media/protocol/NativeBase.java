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

import com.nokia.microedition.media.Locator;
import com.nokia.microedition.media.InternalPlayer;
import com.nokia.microedition.media.NativePlayerFactory;
import javax.microedition.media.MediaException;

/**
 * Base class for native based protocols
 */
public class NativeBase implements Protocol
{
    /**
     * Protected constructor.
     */
    protected NativeBase()
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
        return NativePlayerFactory.createPlayer(aLocator);
    }

}
