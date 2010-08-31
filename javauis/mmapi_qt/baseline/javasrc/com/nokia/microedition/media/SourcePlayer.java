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
* Description:  This class is player with DataSource
*
*/


package com.nokia.microedition.media;

import javax.microedition.media.protocol.DataSource;
import javax.microedition.media.protocol.SourceStream;
import javax.microedition.media.MediaException;
import java.io.IOException;
import com.nokia.microedition.media.protocol.SourceStreamReader;
import com.nokia.mj.impl.utils.Logger;
/**
 * This class add to PlayerImpl DataSource handling.
 */
public class SourcePlayer extends PlayerImpl
{
    //
    protected DataSource iDataSource;

    // Used to SourceStream to native side
    protected SourceStreamReader iSourceStreamReader;

    /**
     * Constructor
     * @param aDataSource Handle to DataSource
     * @param aPlayerHandle handle to player (CMMAPlayer)
     */
    public SourcePlayer(DataSource aDataSource,
                        int aPlayerHandle) throws MediaException
    {
        super(aPlayerHandle);
        iDataSource = aDataSource;
        Logger.LOG(Logger.EJavaMMAPI, Logger.EInfo,"SourcePlayer player handle = " + aPlayerHandle);
        // Create reader for the stream
        iSourceStreamReader = new SourceStreamReader(
            aDataSource.getStreams()[ 0 ],
            aDataSource, // Controllable object
            ManagerImpl.getEventSource(),
            aPlayerHandle);

        // Create SourceStreamReader in native side
        int nativeHandle = _addSourceStream(ManagerImpl.getEventSource(),
                                            iPlayerHandle,
                                            iSourceStreamReader);

        // Failed to create a native-side CMMASourceStream
        if (nativeHandle <= 0)
        {
            throw new MediaException("Could not create Player, Symbian OS error: "
                                     + nativeHandle);
        }

        // Assing java object to corresponding native object
        iSourceStreamReader.setHandle(nativeHandle);
    }

    /**
     * From PlayerBase
     * @see PlayerBase
     */
    public void doPrefetch() throws MediaException
    {
        // DataSource connect must reset its SourceStream if DataSource is
        // disconnected.
        try
        {
            iDataSource.connect();
        }
        catch (IOException ioe)
        {
            throw new MediaException("IOException occurred, " + ioe.getMessage());
        }

        super.doPrefetch();
    }

    /**
     * interface PlayerImpl
     * @see PlayerImpl
     */
    public void doDeallocate()
    {
        if (iDataSource.getStreams()[ 0 ].getSeekType() ==
                SourceStream.NOT_SEEKABLE)
        {
            // if stream is not seekable we have to disconnect the whole source
            // and re-connect it if prefetch is called
            iDataSource.disconnect();
        }
        else
        {
            // stream is RANDOM_ACCESSIBLE or SEEKABLE_TO_START and can be
            // seeked to beginning of the stream
            iSourceStreamReader.resetStream();
        }

        super.doDeallocate();
    }

    /**
     * interface Player
     * @see Player
     */
    public void close()
    {
        super.close();
        // close threads from SourceStreamReaders
        iSourceStreamReader.close();
        iDataSource.disconnect();
    }
}

// End of File
