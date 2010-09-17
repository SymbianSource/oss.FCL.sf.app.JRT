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

import javax.microedition.media.Control;
import javax.microedition.media.protocol.DataSource;
import javax.microedition.media.protocol.SourceStream;
import java.io.IOException;
import javax.microedition.io.StreamConnection;
import com.nokia.mj.impl.utils.Logger;
import javax.microedition.io.Connection;


/**
 * DataSource which has InputStreamSourceStream.
 */
public class InputStreamDataSource extends DataSource
{
    // Connection object.
    protected StreamConnection iConnection;

    // DataSource's stream
    protected InputStreamSourceStream iSourceStream;



    // string that describes the content-type of the media that the source
    // is providing.
    protected String iContentType;




    /**
     * Constructor.
     * @param aSourceStream SourceSteam
     * @param aType Content type.
     */
    public InputStreamDataSource(InputStreamSourceStream aSourceStream,
                                 String aType)
    {
        super(null);   // no locator
        iSourceStream = aSourceStream;
        iContentType = aType;
    }

    /**
     * Constructor with Locator and stream
     * @param aLocator Locator
     * @param aSourceStream SourceSteam
     * @param aType Content type.
     */
    public InputStreamDataSource(String aLocator,
                                 InputStreamSourceStream aSourceStream,
                                 String aType)
    {
        super(aLocator);
        iSourceStream = aSourceStream;
        iContentType = aType;
    }


    /**
     * Constructor with locator
     * @param aLocator Locator
     */
    public InputStreamDataSource(String aLocator)
    {
        super(aLocator);
    }

    /**
     * from DataSource
     * @return Content Type
     * @see DataSource
     */
    public String getContentType()
    {
        return iContentType;
    }

    /**
     * from DataSource
     * Connect to the stream
     * @throws IOException
     * @see DataSource
     */
    public void connect() throws IOException
    {
    }

    /**
     * from DataSource
     * Disconnect from the stream
     */
    public void disconnect()
    {
    }

    /**
     * from DataSource
     * Put DataSource to STARTED state
     * @throws IOException Throw if DataSource is in wrong state
     * @see DataSource
     */
    public void start() throws IOException
    {
    }

    /**
     * from DataSource
     * Stops DataSource
     * @see DataSource
     */
    public void stop()
    {
    }

    /**
     * from DataSource
     * return sourceStreams of the DataSource
     *
     * @exception IllegalStateException Thrown if the source is not connected.
     * @return set of source streams
     * @see DataSource
     */
    public SourceStream[] getStreams()
    {
        SourceStream[] streams = new SourceStream[ 1 ];
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
        return new Control[ 0 ];
    }

    /**
     * from interface Controllable
     * Return a control by the Control Type, not supported
     * @param aControlType type of the control
     * @return Control
     */
    public Control getControl(String aControlType)
    {
        return iSourceStream.getControl("SeekControl");
    }

    /**
    * public method to connection object in derived classes.
    * @returns aConnection object
    */
    public Connection getConnection()
    {
        Logger.LOG(Logger.EJavaMMAPI, Logger.EInfo,"InputStreamDataSource getConnection +");
        return iConnection;
    }




}
// End of File

