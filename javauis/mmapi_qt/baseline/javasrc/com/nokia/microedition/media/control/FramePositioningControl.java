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
* Description:  FramePositioningControl controls frame position.
*
*/

package com.nokia.microedition.media.control;
import javax.microedition.media.Player;
import javax.microedition.media.MediaException;


/**
 * FramePositioningControl methods cannot throw any exceptions.
 * If an error occurs in native side, native side return default value.
 */
public class FramePositioningControl extends ControlImpl
        implements javax.microedition.media.control.FramePositioningControl
{
    // these constants come from native side
    protected static final int NO_ERROR = 0; // native side returns if OK
    protected static final int NOT_FOUND = -1; // value to return on error condition

    /**
     * @see FramePositioningControl
     */
    public int seek(int aFrameNumber)
    {
        checkState();

        int frameNumber = _seek(iControlHandle, iEventSource, aFrameNumber);

        // in case of error
        if (frameNumber < 0)
        {
            if (aFrameNumber <= 0)
            {
                frameNumber = 0;
            }
            else
            {
                frameNumber = Integer.MAX_VALUE;
            }
        }

        return frameNumber;
    }

    /**
     * @see FramePositioningControl
     */
    public int skip(int aFramesToSkip)
    {
        checkState();

        // Array will be used for input and output.
        // For output, room is needed for return value
        // and error code, thus length of 2.

        int[] values = new int[ 2 ];
        values[ 0 ] = aFramesToSkip;
        values[ 1 ] = NO_ERROR;

        int[] framesSkipped =
            _skip(iControlHandle,
                  iEventSource,
                  values);

        if (framesSkipped[ 1 ] != NO_ERROR)
        {
            throw new Error("skip() failed");
        }

        return framesSkipped[ 0 ];
    }

    /**
     * @see FramePositioningControl
     */
    public long mapFrameToTime(int aFrameNumber)
    {
        checkState();

        long frameMediaTime =
            _mapFrameToTime(iControlHandle,
                            iEventSource,
                            aFrameNumber);
        if (frameMediaTime < NO_ERROR)
        {
            frameMediaTime = NOT_FOUND; // error value
        }

        return frameMediaTime; // -1 if error
    }

    /**
     * @see FramePositioningControl
     */
    public int mapTimeToFrame(long aMediaTime)
    {
        checkState();

        // mapTimeToFrame(0) must not fail and must return
        // the frame number of the first frame.

        if (aMediaTime == 0)
        {
            return 0;
        }

        int frameNumber =
            _mapTimeToFrame(iControlHandle,
                            iEventSource,
                            aMediaTime);
        if (frameNumber < NO_ERROR)
        {
            frameNumber = NOT_FOUND; // error value
        }

        return frameNumber;
    }

    /**
     * Native implementation.
     *
     * @param aControlHandle Pointer to the native FramePositioningControl object.
     * @param aEventSourceHandle Pointer to the native event source object.
     * @param aFrameNumber Frame number to seek
     * @return Number of frame that is actually seeked to
     */
    private native int _seek(int iControlHandle,
                             int iEventSource,
                             int aFrameNumber);

    /**
     * Native implementation.
     *
     * @param aControlHandle Pointer to the native FramePositioningControl object.
     * @param aEventSourceHandle Pointer to the native event source object.
     * @param aValues element 0 Number of frames to skip
     * @return element 0 Actual number of frames skipped.
     *         element 1 Error code
     */
    private native int[] _skip(int iControlHandle,
                               int iEventSource,
                               int[] aValues);

    /**
     * Native implementation.
     *
     * @param aControlHandle Pointer to the native FramePositioningControl object.
     * @param aEventSourceHandle Pointer to the native event source object.
     * @param aFrameNumber The input frame number for conversion
     * @return The converted media time in microseconds for the given frame
     * (-1 on error).
     */
    private native long _mapFrameToTime(int iControlHandle,
                                        int iEventSource,
                                        int aFrameNumber);

    /**
     * Native implementation.
     *
     * @param aControlHandle Pointer to the native FramePositioningControl object.
     * @param aEventSourceHandle Pointer to the native event source object.
     * @param aMediaTime The input media time for the conversion in microseconds.
     * @return The converted frame number for the given media time (-1 on error).
     */
    private native int _mapTimeToFrame(int iControlHandle,
                                       int iEventSource,
                                       long aMediaTime);
}
