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
import javax.microedition.io.StreamConnection;
import javax.microedition.io.Connection;
import javax.microedition.io.Connector;
import javax.microedition.io.ConnectionNotFoundException;
import com.nokia.mj.impl.utils.Logger;

/**
 * DataSource that uses Connector to create connections and
 * InputStreams.
 */
public class ConnectorDataSource extends InputStreamDataSource
{
    // Connection object.
    StreamConnection iConnection;

    /**
     * Constructor.
     * @param aLocator Locator to connect.
     */
    public ConnectorDataSource(Locator aLocator)
    throws IOException, MediaException
    {
        super(aLocator.getLocatorString());
        open(); // open iConnection
        iSourceStream = new InputStreamSourceStream(
            iConnection.openInputStream());
    }

    /**
     * Open connection and initialize
     */
    protected void open() throws IOException, MediaException
    {
        try
        {
            Connection connection = Connector.open(getLocator(),
                                                   Connector.READ);

            // derived classes may do additional checks
            checkConnection(connection);

            // Only StreamConnection is supported
            iConnection = (StreamConnection)connection;
        }
        catch (ConnectionNotFoundException cnfe)
        {
            // ConnectionNotFoundException extends IOException,
            // we must throw MediaException in this case.
            throw new MediaException(
                "Connection could not be found, " + cnfe.getMessage());
        }
        catch (IllegalArgumentException iae)
        {
            // IllegalArgumentException can be thrown only if
            // connector or stream is null.
            throw new MediaException(
                "Connector stream is null, " + iae.getMessage());
        }
        catch (ClassCastException cce)
        {
            // Returned connection object was not StreamConnection
            // or connection was not supported in checkConnection method.
            throw new MediaException(cce.getMessage());
        }
    }

    /**
     * from DataSource
     * Connect to the stream
     * @throws IOException
     * @see DataSource
     */
    public void connect() throws IOException
    {
        // if connection is not null source is already connected.
        if (iConnection == null)
        {
            try
            {
                open();
                iSourceStream.setStream(iConnection.openInputStream());
            }
            catch (MediaException me)
            {
                // MediaException cannot be thrown from this method.
                throw new IOException(
                    "failed to connect to stream, " + me.getMessage());
            }
        }
    }

    /**
     * from DataSource
     * Disconnect from the stream
     */
    public void disconnect()
    {
        // If iConnection is null disconnect is already called.
        if (iConnection != null)
        {
            try
            {
                iSourceStream.close();
            }
            catch (IOException ioe)
            {
                // disconnect cannot throw any exception and because source will
                // be disconnected in Player's close or deallocate which can't
                // throw exceptions we have to hide the exception.
                Logger.WLOG(Logger.EJavaMMAPI,
                            "MMA::ConnectorDataSource::disconnect failed ", ioe);
            }
            try
            {
                iConnection.close();
                // source is disconnected and may be reconnected
                iConnection = null;
            }
            catch (IOException ioe)
            {
                // disconnect cannot throw any exception and because source will
                // be disconnected in Player's close or deallocate which can't
                // throw exceptions we have to hide the exception.
                Logger.WLOG(Logger.EJavaMMAPI,
                            "MMA::ConnectorDataSource::disconnect failed ", ioe);
            }
        }
    }

    /**
     * Protected method to connection object in derived classes.
     * @param aConnection Connection to check
     */
    protected void checkConnection(Connection aConnection) throws IOException
    {
        // This class does not do additional checks.
    }
}
// End of File

