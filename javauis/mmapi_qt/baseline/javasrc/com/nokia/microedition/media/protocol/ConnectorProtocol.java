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
* Description:  Protocol used with locators that needs Connector.
*
*/


package com.nokia.microedition.media.protocol;

import javax.microedition.media.MediaException;

import com.nokia.microedition.media.Locator;
import com.nokia.microedition.media.InternalPlayer;
import com.nokia.microedition.media.ConnectorDataSource;
import com.nokia.microedition.media.ManagerImpl;

/**
 * Protocol used with locators that needs Connector.
 */
public class ConnectorProtocol implements Protocol
{
    /**
     * Default constructor.
     */
    public ConnectorProtocol()
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
        ConnectorDataSource dataSource = new ConnectorDataSource(aLocator);
        return ManagerImpl.getInstance().createInternalPlayer(dataSource);
    }
}
