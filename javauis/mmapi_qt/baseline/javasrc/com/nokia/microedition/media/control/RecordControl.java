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
* Description:  This class controls the recording of the media from a Player.
*
*/


package com.nokia.microedition.media.control;

import java.io.IOException;
import java.io.OutputStream;
import javax.microedition.media.MediaException;
import javax.microedition.media.PlayerListener;
import javax.microedition.io.Connector;
import javax.microedition.io.Connection;
import javax.microedition.io.StreamConnection;
import javax.microedition.io.HttpConnection;
import javax.microedition.io.ConnectionNotFoundException;
import com.nokia.microedition.media.protocol.OutputStreamWriter;
import com.nokia.microedition.media.PlayerImpl;
import com.nokia.mj.impl.media.PlayerPermission;
import com.nokia.mj.impl.utils.Logger;
import com.nokia.mj.impl.rt.support.ApplicationUtils;

/**
 * <code>RecordControl</code> controls the recording of media
 * from a <code>Player</code>.  <code>RecordControl</code> records
 * what's currently being played by the <code>Player</code>.
 * <p>
 * <h2>Example</h2>
 * <blockquote>
 * <pre>
 * try {
 *    // Create a Player that captures live audio.
 *    Player p = Manager.createPlayer("capture://audio");
 *    p.realize();
 *    // Get the RecordControl, set the record stream,
 *    // start the Player and record for 5 seconds.
 *    RecordControl rc = (RecordControl)p.getControl("RecordControl");
 *    ByteArrayOutputStream output = new ByteArrayOutputStream();
 *    rc.setRecordStream(output);
 *    rc.startRecord();
 *    p.start();
 *    Thread.currentThread().sleep(5000);
 *    rc.commit();
 *    p.close();
 * } catch (IOException ioe) {
 * } catch (MediaException me) {
 * } catch (InterruptedException ie) { }
 * </pre>
 * </blockquote>
 *
 * @see javax.microedition.media.Player
 */
public class RecordControl extends ControlImpl
        implements javax.microedition.media.control.RecordControl, Runnable
{

    protected static final int ERROR_NONE = 0; // native side returns if OK

    private OutputStreamWriter iOutputStreamWriter;
    private Connection iConnection;
    private boolean iStarted = false;

    // commit() must be allowed when RecordControl is already
    // committed from native side
    private boolean iIgnoreCommitState = false;

    private static final String REQUEST_NAME = "User-Agent";
    private static final String REQUEST_VALUE =
        "Profile/MIDP-2.0 Configuration/CLDC-1.0";
    private static final String CONTENT_TYPE = "Content-Type";

    private static final String RECORD_CONTROL_PERMISSION =
        "javax.microedition.media.control.RecordControl";

    /**
     * RecordControl constructor
     */
    public RecordControl()
    {
    }

    /**
     * Called when the Player is disposed
     */
    public void notifyDispose()
    {
        try
        {
            closeStream();
        }
        catch (IOException ioe)
        {
            // cannot throw error here
            Logger.WLOG(Logger.EJavaMMAPI,
                        "Closing stream failed: ", ioe);
        }
    }

    private void closeStream() throws IOException
    {
        if (iOutputStreamWriter != null)
        {
            // wait until the StreamWriter events are fully handled
            iOutputStreamWriter.close();
        }
        try
        {
            if (iConnection != null)
            {
                // If stream was created with setRecordLocation( String aLocator )
                // method there is connection object and we have to close
                // the OutputStream. Stream set with setRecordStream may not be
                // closed. Any open streams would cause the connection to be
                // held open until they themselves are closed.
                iOutputStreamWriter.closeStream();
            }
        }
        finally
        {
            iOutputStreamWriter = null;
            if (iConnection != null)
            {
                iConnection.close();
                iConnection = null;
            }
        }
    }

    /**
     * Set the output stream where the data will be
     * recorded.
     *
     * @param aStream The output stream where the data will be recorded.
     * @exception IllegalStateException Thrown if one of the following
     * conditions is true:
     * <ul>
     * <li>
     * <code>startRecord</code> has been called and <code>commit</code> has
     * not been called.
     * <li>
     * <code>setRecordLocation</code> has been called and <code>commit</code> has
     * not been called.
     * </ul>
     *
     * @exception IllegalArgumentException Thrown if
     * <code>stream</code> is null.
     *
     */
    public void setRecordStream(OutputStream aStream)
    {
        checkState();
        if (null == aStream)
        {
            throw new IllegalArgumentException("Stream is null");
        }

        // Stream is already set (and not commited)
        if (iOutputStreamWriter != null)
        {
            throw new IllegalStateException(
                "Stream is already set but not commited yet");
        }

        ApplicationUtils appUtils = ApplicationUtils.getInstance();
        PlayerPermission per = new PlayerPermission("audio/video recording","record");
        appUtils.checkPermission(per);

        privateSetRecordStream(aStream);
    }

    /**
     * Private method for setting record stream.
     * Called from setRecordStream and setRecordLocation methods.
     * @param aStream The output stream where the data will be recorded.
     */
    private void privateSetRecordStream(OutputStream aStream)
    {
        int playerHandle = ((PlayerImpl)iPlayer).getPlayerHandle();
        // Create a writer for OutputStream
        iOutputStreamWriter = new OutputStreamWriter(aStream,
                iEventSource,
                this,
                playerHandle);

        // Set the writer to the native object
        int handle = _setRecordStream(iEventSource,
                                      iControlHandle,
                                      iOutputStreamWriter);
        if (handle < ERROR_NONE)
        {
            iOutputStreamWriter = null;
            throw new IllegalArgumentException(
                "setRecordStream() failed, Symbian OS error: " + handle);
        }
        else
        {
            iOutputStreamWriter.start(handle);
        }
    }

    /**
     * Set the output location where the data will be recorded.
     *
     * @param aLocator The locator specifying where the
     * recorded media will be saved.  The locator must be
     * specified as a URL.
     * @exception IllegalStateException Thrown if one of the following
     * conditions is true:
     * <ul>
     * <li>
     * <code>startRecord</code> has been called and <code>commit</code> has
     * not been called.
     * <li>
     * <code>setRecordStream</code> has been called and <code>commit</code> has
     * not been called.
     * </ul>
     * @exception IllegalArgumentException Thrown if <code>locator</code>
     * is null.
     * @exception IOException Thrown if protocol is valid but the
     * media cannot be created at the specified location.
     * @exception MediaException Thrown if the locator is not in URL syntax
     * or it specifies a protocol that is not supported.
     * @exception SecurityException Thrown if the caller does not
     * have security permission to set the record location.
     */
    public void setRecordLocation(String aLocator)
    throws IOException, MediaException
    {
        checkState();
        if (aLocator == null)
        {
            throw new IllegalArgumentException("Locator is null");
        }

        // Stream is already set (and not commited)
        if (iOutputStreamWriter != null)
        {
            throw new IllegalStateException(
                "Record location is already set but not commited yet");
        }

        ApplicationUtils appUtils = ApplicationUtils.getInstance();
        PlayerPermission per = new PlayerPermission("audio/video recording","record");
        appUtils.checkPermission(per);

        Connection conn;
        try
        {
            conn = Connector.open(aLocator, Connector.WRITE);
        }
        catch (IllegalArgumentException iae)
        {
            throw new MediaException("Locator not supported: " + aLocator);
        }
        catch (ConnectionNotFoundException cnfe)
        {
            throw new MediaException("" + cnfe);
        }

        // Using HTTP post
        if (conn instanceof HttpConnection)
        {
            HttpConnection hc = (HttpConnection)conn;

            // Set the request method and headers
            hc.setRequestMethod(HttpConnection.POST);
            hc.setRequestProperty(REQUEST_NAME, REQUEST_VALUE);
            hc.setRequestProperty(CONTENT_TYPE, getContentType());

            // Getting the output stream may flush the headers
            privateSetRecordStream(hc.openOutputStream());
        }
        else if (conn instanceof StreamConnection)
        {
            // Using StreamConnection
            StreamConnection sc = (StreamConnection)conn;
            privateSetRecordStream(sc.openOutputStream());
        }
        else
        {
            conn.close();
            throw new MediaException("Unsupported connection type");
        }
        iConnection = conn;
    }


    /**
     * Return the content type of the recorded media.
     *
     * The content type is given in the
     * <a HREF="../Manager.html#content-type">content type syntax</a>.
     *
     * @return The content type of the media.
     */
    public String getContentType()
    {
        checkState();
        return _getContentType(iEventSource, iControlHandle);
    }


    /**
     * Start recording the media.
     * <p>
     * If the <code>Player</code> is already started, <code>startRecord</code>
     * will immediately start the recording.  If the <code>Player</code>
     * is not already started, <code>startRecord</code> will not
     * record any media.  It will put the recording in a "standby" mode.
     * As soon as the <code>Player</code> is started,
     * the recording will start right away.
     * <p>
     * If <code>startRecord</code> is called when the recording has
     * already started, it will be ignored.
     * <p>
     * When <code>startRecord</code> returns, the recording has started
     * and a <i>RECORD_STARTED</i> event will be delivered through the
     * <code>PlayerListener</code>.
     * <p>
     * If an error occurs while recording is in progress,
     * <i>RECORD_ERROR</i> event will be delivered via the PlayerListener.
     *
     * @exception IllegalStateException Thrown if any of the following
     * conditions is true:
     * <ul>
     * <li>
     * if <code>setRecordLocation</code> or <code>setRecordStream</code> has
     * not been called for the first time.
     * <li>
     * If <code>commit</code> has been called and
     * <code>setRecordLocation</code> or <code>setRecordStream</code>
     * has not been called.
     * </ul>
     */
    public void startRecord()
    {
        checkState();
        // Ignore if startRecord is called when the recording has already started
        if (iStarted)
        {
            return;
        }

        if (iOutputStreamWriter==null)
        {
            throw new IllegalStateException(
                "Output stream or location not set yet");
        }

        int rval = _startRecord(iEventSource, iControlHandle);
        if (rval != ERROR_NONE)
        {
            throw new RuntimeException("startRecord() failed, Symbian OS error: " + rval);
        }
        iStarted = true;
        iIgnoreCommitState = false;
    }

    /**
     * Stop recording the media.  <code>stopRecord</code> will not
     * automatically stop the <code>Player</code>.  It only stops
     * the recording.
     * <p>
     * Stopping the <code>Player</code> does not imply
     * a <code>stopRecord</code>.  Rather, the recording
     * will be put into a "standby" mode.  Once the <code>Player</code>
     * is re-started, the recording will resume automatically.
     * <p>
     * After <code>stopRecord</code>, <code>startRecord</code> can
     * be called to resume the recording.
     * <p>
     * If <code>stopRecord</code> is called when the recording has
     * already stopped, it will be ignored.
     * <p>
     * When <code>stopRecord</code> returns, the recording has stopped
     * and a <i>RECORD_STOPPED</i> event will be delivered through the
     * <code>PlayerListener</code>.
     */
    public void stopRecord()
    {
        checkState();
        // If stopRecord is called when the recording has already stopped,
        // it will be ignored.
        if (!iStarted)
        {
            return;
        }

        int rval = _stopRecord(iEventSource, iControlHandle);
        if (rval != ERROR_NONE)
        {
            throw new RuntimeException("stopRecord() failed, Symbian OS error: " + rval);
        }
        iStarted = false;
    }

    /**
     * Complete the current recording.
     * <p>
     * If the recording is in progress, <code>commit</code>
     * will implicitly call <code>stopRecord</code>.
     * <p>
     * To record again after <code>commit</code> has been called,
     * <code>setRecordLocation</code> or <code>setRecordStream</code>
     * must be called.
     *
     * @exception IOException Thrown if an I/O error occurs during commit.
     * The current recording is not valid. To record again,
     * <code>setRecordLocation</code> or <code>setRecordStream</code>
     * must be called.
     *
     */
    public void commit() throws IOException
    {
        checkState();
        // If commit has been called and setRecordLocation or setRecordStream
        // has not been called.
        if (null == iOutputStreamWriter)
        {
            if (iIgnoreCommitState)
            {
                return;
            }
            throw new IllegalStateException(
                "setRecordLocation() or setRecordStream() not called before calling commit()");
        }

        // If the recording is in progress, commit will implicitly call stopRecord
        if (iStarted)
        {
            stopRecord();
        }

        int rval = _commit(iEventSource, iControlHandle);

        try
        {
            // Do not commit Java stream if native commit failed.
            if (rval == ERROR_NONE)
            {
                iOutputStreamWriter.commit();
            }
        }
        finally
        {
            closeStream();
        }
        if (rval != ERROR_NONE)
        {
            throw new IOException("commit() failed, Symbian OS error: " + rval);
        }
    }

    /**
     * From Runnable.
     * This method is called when commit is started from native code
     * (record size limit is reached).
     */
    public void run()
    {
        // Recording is already stopped.
        iStarted = false;
        try
        {
            commit();
        }
        catch (Exception e)
        {
            // Because this method is called from new Thread no exceptions
            // can be thrown.
            ((PlayerImpl)iPlayer).getPlayerListenerImpl().postEvent(
                PlayerListener.RECORD_ERROR, "Commit failed" + e);

            Logger.ELOG(Logger.EJavaMMAPI, "MMA::RecordControl::run failed: ", e);
        }
        iIgnoreCommitState = true;
    }

    /**
     * Set the record size limit.  This limits the size of the
     * recorded media to the number of bytes specified.
     * <p>
     * When recording is in progress, <code>commit</code> will be
     * called implicitly in the following cases:
     * <ul>
     * <li>Record size limit is reached
     * <li>If the requested size is less than the already recorded size
     * <li>No more space is available.
     * </ul>
     * <p>
     * Once a record size limit has been set, it will remain so
     * for future recordings until it is changed by another
     * <code>setRecordSizeLimit</code> call.
     * <p>
     * To remove the record size limit, set it to
     * <code>Integer.MAX_VALUE</code>.
     * By default, the record size limit is not set.
     * <p>
     * Only positive values can be set.  Zero or negative values
     * are invalid and an <code>IllegalArgumentException</code>
     * will be thrown.
     *
     * @param aSize The record size limit in number of bytes.
     * @return The actual size limit set.
     * @exception IllegalArgumentException Thrown if the given size
     * is invalid.
     * @exception MediaException Thrown if setting the record
     * size limit is not supported.
     */
    public int setRecordSizeLimit(int aSize) throws MediaException
    {
        checkState();
        if (aSize <= 0)
        {
            throw new IllegalArgumentException("Size should be > 0");
        }
        int rval = _setRecordSizeLimit(iEventSource,
                                       iControlHandle,
                                       aSize);

        // @exception MediaException Thrown if setting the record
        // size limit is not supported.

        // Negative values are errors
        if (rval < ERROR_NONE)
        {
            throw new MediaException(
                "Setting record size limit is not supported, Symbian OS error: " + rval);
        }

        // Returns 0, if the buffer cannot be resized
        // Commit the buffer now
        if (rval==0)
        {
            try
            {
                commit();
            }
            catch (IOException ioe)
            {
                // Converts IOE to ME
                throw new MediaException("IOException occurred during commit, " + ioe);
            }

            rval = aSize;   // return the requested size
        }

        // Special case; do not return the actual size set when using MAX_VALUE
        // (this was in TCK tests; RecordControl/SetRecordSizeLimit )
        if (aSize == Integer.MAX_VALUE)
        {
            return Integer.MAX_VALUE;
        }

        return rval;
    }

    /**
     * Erase the current recording.
     * <p>
     * If the recording is in progress, <code>reset</code>
     * will implicitly call <code>stopRecord</code>.
     * <p>
     * Calling <code>reset</code> after <code>commit</code>
     * will have no effect on the current recording.
     * <p>
     * If the <code>Player</code> that is associated with this
     * <code>RecordControl</code> is closed, <code>reset</code>
     * will be called implicitly.
     *
     * @exception IOException Thrown if the current recording
     * cannot be erased. The current recording is not valid.
     * To record again, <code>setRecordLocation</code> or
     * <code>setRecordStream</code> must be called.
     *
     */
    public void reset() throws IOException
    {
        checkState();
        if (null == iOutputStreamWriter)
        {
            return;
        }

        // If the recording is in progress, reset will implicitly call stopRecord
        if (iStarted)
        {
            stopRecord();
        }

        int rval = _reset(iEventSource, iControlHandle);
        if (rval < ERROR_NONE)
        {
            // IOException thrown if the current recording cannot be erased.
            // To record again, setRecordLocation or setRecordStream must be called.
            // closing the stream
            closeStream();
            throw new IOException("reset() failed, Symbian OS error: " + rval);
        }
    }


    private static native int _startRecord(int aEventSource, int aControlHandle);
    private static native int _stopRecord(int aEventSource, int aControlHandle);

    private static native int _setRecordStream(int aEventSource,
            int aControlHandle,
            OutputStreamWriter aWriter);

    private static native String _getContentType(int aEventSource,
            int aControlHandle);

    private static native int _commit(int aEventSource, int aControlHandle);

    private static native int _setRecordSizeLimit(int aEventSource,
            int aControlHandle,
            int aSize);

    private static native int _reset(int aEventSource, int aControlHandle);
}

