/*
* Copyright (c) 2010 Nokia Corporation and/or its subsidiary(-ies).
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
* Description: FramePositioningControl
*
*/
package com.nokia.microedition.media.animation;

import javax.microedition.media.MediaException;
import javax.microedition.media.Player;

import org.eclipse.swt.graphics.ImageData;

import com.nokia.microedition.media.control.ControlImpl;
import com.nokia.mj.impl.utils.Logger;


public class FramePositioningControl extends ControlImpl implements
        javax.microedition.media.control.FramePositioningControl
{
    private int iTotalNoOfFrames;

    /**
     * Constructor of this Control
     * This constructor should be accessible only in this package, that's why it is being
     * declared as package protected.
     */
    FramePositioningControl(Player aPlayer)
    {
        iPlayer=aPlayer;
        iTotalNoOfFrames=((AnimationPlayer)iPlayer).getTotalNumberFrames();
    }

    /**
     * Converts the given frame number to the corresponding media time.
     * The method only performs the calculations. It does not position the media to the given frame.
     */
    public long mapFrameToTime(int aFrameNumber)
    {
        checkState();
        int totalNoOfFrames=iTotalNoOfFrames;
        // if invalid parameter is passed
        if (aFrameNumber < 0 || aFrameNumber > totalNoOfFrames)
            return -1;
        return ((AnimationPlayer)iPlayer).getMediaTimeForFrame(aFrameNumber);
    }

    /**
     * Converts the given media time to the corresponding frame number.
     * The method only performs the calculations. It does not position the media to the given media time.
     * The frame returned is the nearest frame that has a media time less than or equal to the given media time.
     * mapTimeToFrame(0) must not fail and must return the frame number of the first frame.
     * @param the input media time for the conversion in microseconds.
     * @return the converted frame number for the given media time. If the conversion fails, -1 is returned.
     */
    public int mapTimeToFrame(long aMediaTime)
    {
        checkState();
        return ((AnimationPlayer)iPlayer).findFrame(aMediaTime);
    }

    /**
     * Seek to a given video frame. The media time of the Player will be updated to reflect
     * the new position set.
     * This method can be called on a stopped or started Player. If the Player is in the Started state,
     * this method may cause the Player to change states. If that happens, the appropriate transition
     *  events will be posted by the Player when its state changes.
     * If the given frame number is less than the first or larger than the last frame number in the media,
     * seek will jump to either the first or the last frame respectively.
     * @param aFrameNumber the frame to seek to.
     * @return the actual number of frames skipped.
     */
    public int seek(int aFrameNumber)
    {
        final String DEBUG_STR = "FramePositionControl::seek";
        // Check the state of the player, it shouldn't be in closed state
        Logger.LOG(Logger.EJavaMMAPI, Logger.EInfo,DEBUG_STR + "+");
        checkState();
        int frameNumber = aFrameNumber;
        int totalNoOfFrames = iTotalNoOfFrames;
        if (aFrameNumber < 0)
        {
            frameNumber = 0;
        }
        else if (aFrameNumber >= totalNoOfFrames)
        {
            frameNumber = totalNoOfFrames-1;
        }
        long mediaTime = ((AnimationPlayer)iPlayer).getMediaTimeForFrame(frameNumber);
        try
        {
            iPlayer.setMediaTime(mediaTime);
        }
        catch (MediaException e)
        {
            // Just ignore the exception
            e.printStackTrace();
        }
        Logger.LOG(Logger.EJavaMMAPI, Logger.EInfo,DEBUG_STR + "- seeked to "+frameNumber);
        return frameNumber;
    }

    /**
     *  Skip a given number of frames from the current position. The media time of the Player will be
     *  updated to reflect the new position set.
     *  This method can be called on a stopped or started Player. If the Player is in the Started state,
     *  the current position is changing. Hence, the frame actually skipped to will not be exact.
     *  If the Player is in the Started state, this method may cause the Player to change states.
     *  If that happens, the appropriate transition events will be posted.
     *  If the given framesToSkip will cause the position to extend beyond the first or last frame,
     *  skip will jump to the first or last frame respectively.
     *  @param framesToSkip - the number of frames to skip from the current position.
     *  If framesToSkip is positive, it will seek forward by framesToSkip number of frames.
     *  If framesToSkip is negative, it will seek backward by framesToSkip number of frames.
     *  e.g. skip(-1) will seek backward one frame.
     *
     *  @return the actual number of frames skipped.
     */
    public int skip(int aFramesToSkip)
    {
//        final String DEBUG_STR = "FramePositionControl::skip";
        //Logger.LOG(Logger.EJavaMMAPI, Logger.EInfo,DEBUG_STR+"+");
        // check the state of the player, if it closed throw exception
        checkState();
        //int frameNumberToJump;
        int currentFrameIndex=((AnimationPlayer) iPlayer).getiFrameIndex();
        //If user provides the argument as Integer.max, adding any positive value to it will make it negative
        //Since a player can't be skipped more than the total number of frames, here we are making it restrictive.
        if (aFramesToSkip > iTotalNoOfFrames)
        {
            aFramesToSkip = iTotalNoOfFrames;
        }
        // we are going to utilize the seek function here
        // just get the current frame index from player and
        // add it to the number of frame to skip
        int seeked=seek(currentFrameIndex + aFramesToSkip);
        int numberOfFrameSkipped=seeked-currentFrameIndex;
        return numberOfFrameSkipped;
    }

}
