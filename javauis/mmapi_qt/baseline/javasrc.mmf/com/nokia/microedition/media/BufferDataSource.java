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
* Description:  Buffered DataSource
*
*/


package com.nokia.microedition.media;

import javax.microedition.media.Control;
import javax.microedition.media.protocol.DataSource;
import javax.microedition.media.protocol.SourceStream;
import java.io.IOException;
import javax.microedition.io.Connection;
import com.nokia.mj.impl.utils.Logger;

/**
 * BufferDataSource is used to read header from DataSource's SourceStream.
 * This class delegates all method calls to DataSource given in constructor
 * except getStream method which returns buffered SourceStream.
 */
public class BufferDataSource extends DataSource
{
    // delegated source
    protected DataSource iDataSource;

    // buffers DataSource's first stream.
    protected BufferSourceStream iSourceStream;

    /**
     * Constructor.
     * @param aDataSource delegated DataSource
     */
    public BufferDataSource(DataSource aDataSource) throws IOException
    {
        super(aDataSource.getLocator());
        iDataSource = aDataSource;
        SourceStream ss = aDataSource.getStreams()[ 0 ];
        iSourceStream = new BufferSourceStream(ss);
    }

    /**
     * Return header data.
     * @return header data
     */
    public byte[] getHeader() throws IOException
    {
        return iSourceStream.getHeader();
    }

    /**
     * from DataSource
     * @return Content Type
     * @see DataSource
     */
    public String getContentType()
    {
        return iDataSource.getContentType();
    }

    /**
     * from DataSource
     * Connect to the stream
     * @throws IOException
     * @see DataSource
     */
    public void connect() throws IOException
    {
        Logger.LOG(Logger.EJavaMMAPI, Logger.EInfo,"BufferDataSource connect +");
        iDataSource.connect();
        Logger.LOG(Logger.EJavaMMAPI, Logger.EInfo,"BufferDataSource connect -");
    }

    /**
     * from DataSource
     * Disconnect from the stream
     */
    public void disconnect()
    {
        iDataSource.disconnect();
    }

    /**
     * from DataSource
     * Put DataSource to STARTED state
     * @throws IOException Throw if DataSource is in wrong state
     * @see DataSource
     */
    public void start() throws IOException
    {
        iDataSource.start();
    }

    /**
     * from DataSource
     * Stops DataSource
     * @see DataSource
     */
    public void stop() throws IOException
    {
        iDataSource.stop();
    }

    /**
     * from DataSource
     * return sourceStreams of the DataSource
     *
     * @return set of source streams
     * @see DataSource
     */
    public SourceStream[] getStreams()
    {
        // return all streams from delegated DataSource except first
        // buffered stream.
        SourceStream[] originalStreams = iDataSource.getStreams();
        SourceStream[] streams = new SourceStream[ originalStreams.length ];
        System.arraycopy(originalStreams, 0,
                         streams, 0, originalStreams.length);
        streams[ 0 ] = iSourceStream;
        return streams;
    }

    /**
     * from interface Controllable
     * Method return controls of the DataSource
     * @return Control
     * @see Controllable
     * @see DataSource
     */
    public Control[] getControls()
    {
        return iDataSource.getControls();
    }

    /**
     * from interface Controllable
     * Return a control by the Control Type, not supported
     * @param controlType type of the control
     * @return Control
     */
    public Control getControl(String aControlType)
    {
        return iDataSource.getControl(aControlType);
    }

    public Connection getConnection()
    {
        return iDataSource.getConnection();
    }

}
// End of File

