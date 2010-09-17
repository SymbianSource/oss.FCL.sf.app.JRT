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
* Description:  This class is native player implementation of Player interface
*
*/


package com.nokia.microedition.media;

import javax.microedition.media.PlayerListener;
import javax.microedition.media.MediaException;
import com.nokia.microedition.media.NativeError;
import com.nokia.mj.impl.rt.support.Finalizer;
import com.nokia.mj.impl.utils.Logger;

public class PlayerImpl extends PlayerBase
{
    // CMMAPlayer handle
    protected int iPlayerHandle;

    // Controls container
    protected ControlContainer iControls;

    // object for waiting native asynchronous method calls
    protected Object iLockObject = new Object();
    protected Object iPrefetchLockObject = new Object();

    // native error code
    protected int iError;

    protected PlayerListenerImpl iPlayerListenerImpl;

    private Finalizer mFinalizer;

    private boolean iWaitFlag = true;

    private boolean iStartWaitFlag = true;

    /**
     * Constructor
     * @param aPlayerHandle handle to player (CMMAPlayer)
     */
    public PlayerImpl(int aPlayerHandle)
    {
        iPlayerHandle = aPlayerHandle;

        iPlayerListenerImpl = new PlayerListenerImpl(this);

        // Initialize player
        int err = _initPlayer(iPlayerListenerImpl,
                              ManagerImpl.getEventSource(),
                              iPlayerHandle);
        if (err < NativeError.KErrNone)
        {
            throw new OutOfMemoryError(NativeError.errorMessage(err));
        }

        mFinalizer = new Finalizer()
        {
            public void finalizeImpl()
            {
                doFinalize();
            }
        };

        iControls = ControlContainer.populateControls(this,
                    ManagerImpl.getEventSource(),
                    iPlayerHandle);
    }

    private void doFinalize()
    {
        if (mFinalizer != null)
        {
            registeredFinalize();
            mFinalizer = null;
        }
    }

    /**
     * This method is called when garbage collection occurs
     * to delete native object
     */
    protected void registeredFinalize()
    {
        close();
    }

    /**
     * Returns native player handle.
     * @return Native player handle.
     */
    public int getPlayerHandle()
    {
        return iPlayerHandle;
    }

    /**
     * @return Player's PlayerListenerImpl
     */
    public PlayerListenerImpl getPlayerListenerImpl()
    {
        return iPlayerListenerImpl;
    }

    /**
     * Adds control to the player.
     * @param aControl Player's control
     * @param aControlName Identifies the control. Name must contain
     * full package information
     */
    public void addControl(javax.microedition.media.Control aControl,
                           String aControlName)
    {
        iControls.addControl(aControl, aControlName);
    }

    /**
     * interface Controllable
     * Return controls
     * @return list of controls
     * @see Controllable
     */
    public javax.microedition.media.Control[] getControls()
    {
        closeCheck();
        unrealizedCheck();
        return iControls.getControls();
    }

    /**
     * interface Controllable
     * get control by content type
     * @param aControlType content type of wanted control
     * @return control
     * @see Controllable
     * @see Control
     */
    public javax.microedition.media.Control getControl(String aControlType)
    {
        closeCheck();
        unrealizedCheck();
        return iControls.getControl(aControlType);
    }

    /**
     * interface Player
     * @see Player
     */
    public void addPlayerListener(PlayerListener aPlayerListener)
    {
        closeCheck();
        iPlayerListenerImpl.addPlayerListener(aPlayerListener);
    }

    /**
     * From Player
     * @see Player
     */
    public void removePlayerListener(PlayerListener aPlayerListener)
    {
        closeCheck();
        iPlayerListenerImpl.removePlayerListener(aPlayerListener);
    }

    /**
     * From Player
     * @see Player
     */
    public int getState()
    {
        int state = CLOSED;
        if (iState != CLOSED)
        {
            Logger.LOG(Logger.EJavaMMAPI, Logger.EInfo,"PlayerImpl.java : before _getState()  ");
            state = _getState(ManagerImpl.getEventSource(), iPlayerHandle);
            Logger.LOG(Logger.EJavaMMAPI, Logger.EInfo,"PlayerImpl.java : after _getState()  ");
        }
        Logger.LOG(Logger.EJavaMMAPI, Logger.EInfo,"+ PlayerIMPl.getState() ");
        return state;
    }


    /**
     * From PlayerBase
     * @see PlayerBase
     */
    public void doRealize() throws MediaException
    {
        int err = _realize(ManagerImpl.getEventSource(), iPlayerHandle);

        if (err < NativeError.KErrNone)
        {
            throw new MediaException(
                "Realize failed, Symbian OS error: " + err);
        }
    }

    /**
     * From PlayerBase
     * @see PlayerBase
     */
    public void doPrefetch() throws MediaException
    {
        Logger.LOG(Logger.EJavaMMAPI, Logger.EInfo,"PlayerImpl.java : doPrefetch");
        synchronized (iPrefetchLockObject)
        {
            if (PREFETCHED == getState())
                return;
            synchronized (iLockObject)
            {
                Logger.LOG(Logger.EJavaMMAPI, Logger.EInfo,"PlayerImpl.java : doPrefetch inside try"+Thread.currentThread().getName());
                //int retval[] = new int[1];
                int err = _prefetch(ManagerImpl.getEventSource(), iPlayerHandle);
                Logger.LOG(Logger.EJavaMMAPI, Logger.EInfo,"PlayerImpl.java : doPrefetch err = " + err);
                if (err < NativeError.KErrNone)
                {
                    throw new MediaException(
                        "Prefetch failed, Symbian OS error: " + err);
                }
                if (iWaitFlag)
                {
                    try
                    {
                        // wait until actionCompleted( int aError ) is called
                        Logger.LOG(Logger.EJavaMMAPI, Logger.EInfo,"PlayerImpl.java : doPrefetch ibefore iLockObject");
                        iLockObject.wait();
                        Logger.LOG(Logger.EJavaMMAPI, Logger.EInfo,"PlayerImpl.java : doPrefetch after iLockObject");
                    }
                    catch (InterruptedException ie)
                    {
                        // cannot occur
                    }
                }
                Logger.LOG(Logger.EJavaMMAPI, Logger.EInfo,"PlayerImpl.java : doPrefetch Sync end");
            } // end   synchronized (iLockObject)
        }

        if (iError < NativeError.KErrNone)
        {
            throw new MediaException(
                "Prefetch failed, Symbian OS error: " + iError);
        }
    }

    /**
     * Notify that action is completed. Called from native side.
     * @param aError error code
     */
    private void actionCompleted(int aError)
    {
        Logger.LOG(Logger.EJavaMMAPI, Logger.EInfo,"actionCompleted callback11 aError ="+ aError + "    "+Thread.currentThread().getName());
        iError = aError;

        synchronized (iLockObject)
        {
            Logger.LOG(Logger.EJavaMMAPI, Logger.EInfo,"actionCompleted callback inside synchronized (iLockObject)");
            iLockObject.notify();
        }
    }

    private void actionCompletedFile()
    {
        Logger.LOG(Logger.EJavaMMAPI, Logger.EInfo,"PlayerImpl.java : actionCompletedFile");
        iWaitFlag = false;

    }

    private void actionCompletedStart()
    {
        Logger.LOG(Logger.EJavaMMAPI, Logger.EInfo,"PlayerImpl.java : actionCompletedStart");
        iStartWaitFlag = false;

    }

    /**
     * From PlayerBase
     * @see PlayerBase
     */
    public void doDeallocate()
    {
        int ret = _deallocate(ManagerImpl.getEventSource(), iPlayerHandle);
        if (ret < NativeError.KErrNone)
        {
            Logger.LOG(Logger.EJavaMMAPI, Logger.EInfo,
                       "_deallocate() failed: err=" + ret);
        }
    }

    /**
     * From Player.
     * @see Player
     */
    public void start() throws MediaException
    {
        Logger.LOG(Logger.EJavaMMAPI, Logger.EInfo,"PlayerImpl.java : start()");
        Logger.LOG(Logger.EJavaMMAPI, Logger.EInfo,"PlayerImpl.java : start() outside if(iSPrefetchOngoing)");
        prefetch();
        Logger.LOG(Logger.EJavaMMAPI, Logger.EInfo,"PlayerImpl, start, state = "+getState());
        // Only preteched player may be started. If player is already started
        // this method returns silently.
        if (getState() == PREFETCHED)
        {
            synchronized (iLockObject)
            {
                Logger.LOG(Logger.EJavaMMAPI, Logger.EInfo,"PlayerImpl, before calling native _start() thread is =" +Thread.currentThread().getName());
                Logger.LOG(Logger.EJavaMMAPI, Logger.EInfo,"PlayerImpl, before calling native _start()");
                int err = _start(ManagerImpl.getEventSource(), iPlayerHandle);
                Logger.LOG(Logger.EJavaMMAPI, Logger.EInfo,"PlayerImpl, before calling native _start( ) returned : err = "+err);
                if (err < NativeError.KErrNone)
                {
                    throw new MediaException(
                        "Start failed, Symbian OS error: " + err);
                }
                if (iError < NativeError.KErrNone)
                {
                    throw new MediaException(
                        "Start failed, Symbian OS error: " + iError);
                }
                if (iStartWaitFlag)
                {
                    try
                    {
                        // wait until actionCompleted( int aError ) is called
                        iLockObject.wait();   // only for tck run
                    }
                    catch (InterruptedException ie)
                    {
                        // cannot occur
                    }

                }
            }
        }
    }

    /**
     * From PlayerBase
     * @see PlayerBase
     */
    public void doStop() throws MediaException
    {
        /*
        Stops the Player. It will pause the playback at the current media time.
        Calling start will resume the playback from where it is stopped.
        If stop is called on a stopped Player, the request is ignored.
        */
        Logger.LOG(Logger.EJavaMMAPI, Logger.EInfo,"PlayerImpl.java : doStop()");
        int err = _stop(ManagerImpl.getEventSource(), iPlayerHandle);
        if (err < NativeError.KErrNone)
        {
            throw new MediaException(
                "Stop failed, Symbian OS error: " + err);
        }
        // After this request is completed,
        // the Player is in the PREFETCHED state.
    }

    /**
     * From PlayerBase
     * @see PlayerBase
     */
    public void doClose()
    {
        Logger.LOG(Logger.EJavaMMAPI, Logger.EInfo,"PlayerImpl.java : doclose() 1");
        _close(ManagerImpl.getEventSource(), iPlayerHandle);

        if (iPlayerHandle > NativeError.KErrNone)  // construction has succeed
        {
            Logger.LOG(Logger.EJavaMMAPI, Logger.EInfo,"PlayerImpl.java : doclose() 2");
            // notify the controls
            iControls.dispose();
            Logger.LOG(Logger.EJavaMMAPI, Logger.EInfo,"PlayerImpl.java : doclose() 3");

            // delete the native object
            _dispose(ManagerImpl.getEventSource(), iPlayerHandle);
            Logger.LOG(Logger.EJavaMMAPI, Logger.EInfo,"PlayerImpl.java : doclose() 4");
            iPlayerHandle = 0; // Do not use native object anymore
        }
    }
    /**
     * interface Player
     * @see Player
     */
    public void setLoopCount(int aCount)
    {
        super.setLoopCount(aCount);
        _setLoopCount(ManagerImpl.getEventSource(), iPlayerHandle, aCount);
    }

    /**
     * interface Player
     * @see Player
     */
    public long getDuration()
    {
        closeCheck();
        long duration = _duration(ManagerImpl.getEventSource(), iPlayerHandle);
        if (duration < NativeError.KErrNone)
        {
            duration = TIME_UNKNOWN;
        }
        return duration;
    }

    /**
     * From PlayerBase
     * @see PlayerBase
     */
    public long setMediaTime(long aNow) throws MediaException
    {
        long time = _setMediaTime(ManagerImpl.getEventSource(),
                                  iPlayerHandle,
                                  super.setMediaTime(aNow));
        if (time < NativeError.KErrNone)
        {
            throw new MediaException(
                "Could not set media time, Symbian OS error: " + time);
        }
        return time;
    }

    /**
     * interface Player
     * @see Player
     */
    public long getMediaTime()
    {
        closeCheck();
        long time = _getMediaTime(ManagerImpl.getEventSource(), iPlayerHandle);
        if (time < NativeError.KErrNone)
        {
            time = TIME_UNKNOWN;
        }
        return time;
    }


    /**
     * interface Player
     * @see Player
     */
    public String getContentType()
    {
        closeCheck();
        unrealizedCheck();

        return _getContentType(iPlayerHandle);
    }

    // JNI
    private native int _initPlayer(Object aPlayerListenerObject,
                                   int aEventSource,
                                   int aPlayerHandle);

    private static native int _start(int aEventPoster, int aPlayer);
    private static native int _stop(int aEventPoster, int aPlayer);
    private static native int _close(int aEventPoster, int aPlayer);
    private static native int _prefetch(int aEventPoster, int aPlayer);

    private static native int _setLoopCount(int aEventPoster, int aPlayer,
                                            int aLoopCount);

    private static native int _realize(int aEventPoster, int aPlayer);
    private static native long _duration(int aEventPoster, int aPlayer);
    private static native long _setMediaTime(int aEventPoster, int aPlayer,
            long aNow);

    private static native int _deallocate(int aEventPoster, int aPlayer);
    private static native long _getMediaTime(int aEventPoster, int aPlayer);
    private static native int _getState(int aEventPoster, int aPlayer);

    protected static native int _addSourceStream(int aEventPoster,
            int aPlayerHandle,
            Object aSourceStreamReader);

    private static native String _getContentType(int aPlayerHandle);

    private static native void _dispose(int aEventSourceHandle,
                                        int aObjectHandle);
}

// End of File
