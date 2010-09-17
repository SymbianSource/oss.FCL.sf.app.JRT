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
* Description:  This class is an imlementation of DataSource
*
*/


package com.nokia.microedition.media;

import javax.microedition.media.MediaException;
import java.io.IOException;
import javax.microedition.io.Connection;
import javax.microedition.io.HttpConnection;
import javax.microedition.media.Control;
import com.nokia.mj.impl.utils.Logger;

/**
 * This class extends ConnectorDataSource and adds http connection
 * return value checking.
 */
public class HttpDataSource extends ConnectorDataSource implements SeekControl
{
    private static final String SEEK_CONTROL = "SeekControl";

    // true when closed
    boolean iClosed = false;

    /**
     * Default constructor
     */
    public HttpDataSource(Locator aLocator)
    throws IOException, MediaException
    {
        super(aLocator);
    }

    /**
     * From ConnectorDataSource
     * @see ConnectorDataSource
     */
    protected void checkConnection(Connection aConnection) throws IOException
    {
        // Must handle also HTTP response codes
        HttpConnection httpConnection = (HttpConnection)aConnection;
        if (httpConnection.getResponseCode() != HttpConnection.HTTP_OK)
        {
            throw new IOException(
                "Could not connect to source, " + httpConnection.getResponseMessage());
        }
    }

    /**
     * From SeekControl
     */
    public void seek(int aWhere) throws IOException
    {
        // Only seek to start is supported.
        if (aWhere == 0)
        {
            disconnect();

            // do not connect if stream is closed
            if (!iClosed)
            {
                connect();
                Logger.LOG(Logger.EJavaMMAPI, Logger.EInfo,"HttpDataSource seek() after open");
            }

            if (iClosed)
            {
                // disconnect if stream was closed during connect operation
                disconnect();
            }
        }
        else
        {
            throw new IOException("Internal error, invalid position: " + aWhere);
        }
    }

    /**
     * From SeekControl
     */
    public void close()
    {
        iClosed = true;
    }

    /**
     * From Controllable
     */
    public Control getControl(String aControlType)
    {
        Control control = null;
        if (aControlType.equals(SEEK_CONTROL))
        {
            control = this;
        }
        // else return null
        return control;
    }
}
// End of File

