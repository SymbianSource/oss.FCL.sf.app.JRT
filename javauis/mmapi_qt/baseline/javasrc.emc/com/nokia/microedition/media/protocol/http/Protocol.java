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
* Description:  This class implements TimeBase
*
*/


package com.nokia.microedition.media.protocol.http;




import javax.microedition.media.MediaException;
import com.nokia.microedition.media.protocol.ConnectorProtocol;
import com.nokia.microedition.media.HttpDataSource;
import com.nokia.microedition.media.Locator;
import com.nokia.microedition.media.InternalPlayer;
import com.nokia.microedition.media.ManagerImpl;
import com.nokia.microedition.media.BufferDataSource;
import com.nokia.microedition.media.protocol.EMCSourceInfo;

public class Protocol extends ConnectorProtocol
{
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
        HttpDataSource dataSource = new HttpDataSource(aLocator);
        BufferDataSource bdc = new BufferDataSource(dataSource);
        (new EMCSourceInfo(bdc)).writeInfo(ManagerImpl.getEventSource(), ManagerImpl.getHandle());
        return ManagerImpl.getInstance().createInternalPlayer(bdc);
    }
}
