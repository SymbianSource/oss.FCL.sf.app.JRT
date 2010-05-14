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
* Description:  SourceStream that reads InputStream
*
*/


package com.nokia.microedition.media;

import com.nokia.microedition.media.InputStreamSeekControl;

import java.io.IOException;
import java.io.InputStream;
import javax.microedition.media.protocol.ContentDescriptor;
import javax.microedition.media.protocol.SourceStream;
import javax.microedition.media.Control;

/**
 * This class implements SourceStream interface. From the interface
 * only read and getSeekType method will be used. This class is used
 * to read InputStream.
 */
public class InputStreamSourceStream implements SourceStream
{
    // Stream used to read the content.
    protected InputStream iInputStream = null;

    private static final String SEEK_CONTROL = "SeekControl";

    private InputStreamSeekControl iSeekControl;
    /**
     * Constructor.
     * @param aInputStream An InputStream
     */
    public InputStreamSourceStream(InputStream aInputStream)
    {
        setStream(aInputStream);
        iSeekControl = new InputStreamSeekControl(aInputStream);
    }

    /**
     * Constructor.
     */
    public InputStreamSourceStream()
    {
    }

    /**
     * Sets new InputStream.
     * @param aInputStream An InputStream
     */
    public final void setStream(InputStream aInputStream)
    {
        iInputStream = aInputStream;
    }

    /**
     * Closes the InputStream. This method can only be called if MMA
     * must close the stream.
     *
     * @throws NullPointerException if setStream is not called.
     * @throws IOException If an I/O error occurs
     */
    public void close() throws IOException
    {
        iInputStream.close();
    }

    /**
     * Read from Input Stream
     * @param aBuffer Input Stream
     * @param aOffset where reading starts
     * @param aLength length of read data
     * @return bytes read
     * @throws IOException
     */
    public int read(byte[] aBuffer, int aOffset, int aLength) throws IOException
    {
        return iInputStream.read(aBuffer, aOffset, aLength);
    }

    /**
     * Seek type of the stream
     * @return seek type based on the inputstream's capability
     */
    public int getSeekType()
    {
        if (iInputStream != null && iInputStream.markSupported() == true)
        {

            return SEEKABLE_TO_START;
        }
        else
        {
            return NOT_SEEKABLE;
        }
    }

    /**
     * From interface SourceStream. Method not implemented.
     * @see SourceStream
     * @return size of the stream available
     */
    public int getTransferSize()
    {
        return 0;
    }

    /**
     * From interface SourceStream. Method not implemented.
     * @see SourceStream
     * @param aWhere where to seek
     * @return seeked position, allways the current position
     * @throws IOException
     */
    public long seek(long aWhere) throws IOException
    {
        return 0;
    }

    /**
     * From interface SourceStream. Method not implemented.
     * @see SourceStream
     * @return current position
     */
    public long tell()
    {
        return 0;
    }

    /**
     * From interface SourceStream. Method not implemented.
     * @see SourceStream
     * @return The content type
     * @see ContentDescriptor
     */
    public ContentDescriptor getContentDescriptor()
    {
        // Return empty content descriptor.
        // This method is not used.
        return new ContentDescriptor("");
    }

    /**
     * From interface SourceStream. Method not implemented.
     * @see SourceStream
     * @return content length which is allways 0
     */
    public long getContentLength()
    {
        return 0;
    }

    /**
     * From interface Controllable. Method not implemented.
     * @see SourceStream
     * @see Controllable
     */
    public Control[] getControls()
    {
        Control [] aControls = new Control[1];

        aControls[0] = iSeekControl;

        return aControls;
    }

    /**
     * From interface Controllable. Method not implemented.
     * @param aControlType wanted control type
     * @return Control null
     * @See Controllable
     */
    public Control getControl(String aControlType)
    {
        if (aControlType == SEEK_CONTROL && getSeekType() == SEEKABLE_TO_START)
        {
            return iSeekControl;
        }
        else
        {
            return null;
        }
    }
    /**
     * @author d35kumar 
     * @return
     */
    public InputStream getInputStream(){
    	return iInputStream;
    }
}

// End of File
