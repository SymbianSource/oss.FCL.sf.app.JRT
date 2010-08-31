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
* Description:  This class is general player implementation of Player interface
*
*/


package com.nokia.microedition.media;

import javax.microedition.media.TimeBase;
import javax.microedition.media.MediaException;
import javax.microedition.media.Manager;
import com.nokia.mj.impl.utils.Logger;

/**
 * This abstract player implementation must be derived. This class implements
 * general player functionality such as state checking.
 */
public abstract class PlayerBase implements InternalPlayer
{
    protected TimeBase iTimeBase;

    // state as defined in Player interface
    // at first player is unrealized
    protected int iState = UNREALIZED;

    /**
     * Constructor
     */
    protected PlayerBase()
    {
    }

    /**
     * Every method has to check if player is closed with this method
     */
    protected void closeCheck()
    {
        if (iState == CLOSED)
        {
            throw new IllegalStateException("Player is CLOSED.");
        }
    }

    /**
     * This method is from methods that are
     * not allowed to be called, when player
     * is in UNREALIZED state
     */
    protected void unrealizedCheck()
    {
        if (getState() == UNREALIZED)
        {
            throw new IllegalStateException("Player is UNREALIZED.");
        }
    }

    /**
     * From Player
     * @see Player
     */
    public TimeBase getTimeBase()
    {
        closeCheck();
        unrealizedCheck();

        // set default timebase
        if (iTimeBase==null)
        {
            iTimeBase = Manager.getSystemTimeBase();
        }

        return iTimeBase;
    }

    /**
     * From Player
     * @see Player
     */
    public void setTimeBase(TimeBase aMaster) throws MediaException
    {
        closeCheck();
        unrealizedCheck();

        // A MediaException is thrown if
        // setTimeBase is called on a STARTED  Player.
        if (getState() == STARTED)
        {
            throw new IllegalStateException("Player is STARTED");
        }

        if (aMaster == null)
        {
            // set default timebase
            iTimeBase = Manager.getSystemTimeBase();
        }
        else
        {
            if (aMaster instanceof SystemTimeBase)
            {
                iTimeBase = aMaster;
                return;
            }
            else
            {
                // default doesn't support any other TimeBases
                throw new MediaException("Invalid timebase or timebase not supported");
            }
        }
    }

    /**
     * From Player
     * @see Player
     */
    public void realize() throws MediaException
    {
        Logger.LOG(Logger.EJavaMMAPI, Logger.EInfo,"+ PlayerBase.realize() ");
        closeCheck();
        int state = getState();
        Logger.LOG(Logger.EJavaMMAPI, Logger.EInfo,"+ PlayerBase.realize() after getstate() ");

        // If realize is called when the Player is in the
        // REALIZED, PREFETCHTED or STARTED state,
        // the request will be ignored.
        if (state == UNREALIZED)
        {
            doRealize();
        }
        Logger.LOG(Logger.EJavaMMAPI, Logger.EInfo,"- PlayerBase.realize() ");
    }

    /**
     * Called from realize method. This method must be implemented in
     * derived class.
     * @see realize
     */
    abstract protected void doRealize() throws MediaException;

    /**
     * From Player
     * @see Player
     */
    public void prefetch() throws MediaException
    {
        realize();

        // prefetch must be ignored in other states
        if (getState() == REALIZED)
        {
            doPrefetch();
        }
    }

    /**
     * Called from prefetch method.
     * @see prefetch
     */
    abstract protected void doPrefetch() throws MediaException;

    /**
     * interface Player
     * @see Player
     */
    public void deallocate()
    {
        try
        {
            stop();
        }
        catch (MediaException me)
        {
            // if stop fails we can do nothing
            Logger.ELOG(Logger.EJavaMMAPI,
                        "MMA::PlayerBase: Stop failed in deallocate.", me);
        }
        if (getState() == PREFETCHED)
        {
            doDeallocate();
        }
    }

    /**
     * Called from deallocate method.
     */
    abstract protected void doDeallocate();

    /**
     * interface Player
     * @see Player
     */
    public void stop() throws MediaException
    {
        closeCheck();
        if (getState() == STARTED)
        {
            doStop();
        }
    }

    /**
     * Called from realize method.
     */
    abstract protected void doStop() throws MediaException;

    /**
     * interface Player
     * @see Player
     */
    public void close()
    {
        /*
        Releases all resources and cease all activity.
        The close method indicates that the Player will no
        longer be used and can shut itself down.
        Methods invoked on a closed Player will throw RuntimeExceptions.
        */
        if (iState != CLOSED)
        {
            deallocate();
            iState = CLOSED;
            doClose();
        }
    }

    /**
     * Called from close method.
     */
    abstract protected void doClose();

    /**
     * interface Player.
     * Derived classes may use this method to check parameter and state.
     * @see Player
     */
    public void setLoopCount(int aCount)
    {
        closeCheck();
        // -1 is valid count value meaning indefinitely looping
        if ((aCount == 0) ||
                (aCount < -1))
        {
            throw new java.lang.IllegalArgumentException(
                "Invalid loop count " + aCount + ", loop count cannot be 0 or < -1");
        }
        if (getState() == STARTED)
        {
            throw new IllegalStateException("Player is STARTED");
        }
    }

    /**
     * interface Player
     * @see Player
     */
    public long setMediaTime(long aNow) throws MediaException
    {
        closeCheck();
        unrealizedCheck();

        long now = aNow;

        if (now < 0)
        {
            // Setting the media time to negative values
            // will effectively set the media time to zero.
            now = 0;
        }
        return now;
    }
}

// End of File
