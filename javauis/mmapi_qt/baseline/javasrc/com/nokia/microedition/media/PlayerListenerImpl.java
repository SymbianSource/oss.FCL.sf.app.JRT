/*
* Copyright (c) 2002-2007 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  This class is player listener implementation
*
*/


package com.nokia.microedition.media;

import javax.microedition.media.Player;
import javax.microedition.media.PlayerListener;
import javax.microedition.media.Control;
import javax.microedition.media.control.VideoControl;
import java.util.Vector;
import java.util.Enumeration;
//import com.symbian.midp.runtime.MIDletExecutor;
//import com.symbian.midp.runtime.MIDletInstance;
import com.nokia.mj.impl.utils.Logger;


public class PlayerListenerImpl
{
    // PlayerListener event types that come from native side
    protected static final int E_STARTED = 0;
    protected static final int E_STOPPED = 1;
    protected static final int E_STOPPED_AT_TIME = 2;
    protected static final int E_END_OF_MEDIA = 3;
    protected static final int E_DURATION_UPDATED = 4;
    protected static final int E_AUDIO_DEVICE_UNAVAILABLE = 5;
    protected static final int E_VOLUME_CHANGED = 6;
    protected static final int E_SIZE_CHANGED = 7;
    protected static final int E_ERROR = 8;
    protected static final int E_CLOSED = 9;
    protected static final int E_RECORD_STARTED = 10;
    protected static final int E_RECORD_STOPPED = 11;
    protected static final int E_PRIVATE_DATA_AVAILABLE = 12;
    protected static final int E_PRIVATE_DATA_OVERWRITTEN = 13;
    protected static final int E_BUFFERING_STARTED = 14;
    protected static final int E_BUFFERING_STOPPED = 15;
    protected static final int E_RECORD_ERROR = 16;
    // For global volume info
    protected static final int E_NOKIA_EXTERNAL_VOLUME_EVENT = 17;
    // for Audio Routing preferences change
    protected static final int E_AUDIO_ROUTING_PREFERENCE_CHANGED_EVENT = 18;

    private static final String KVideoControlClassName =
        "javax.microedition.media.control.VideoControl";

    // maps int event types to String event types
    private static Vector sEventTypes;

    static
    {
        sEventTypes = new Vector(19); // there's 17 event types + 1 global volume event + 1 Audio Routing Preference Change Event
        sEventTypes.addElement(PlayerListener.STARTED); // E_STARTED
        sEventTypes.addElement(PlayerListener.STOPPED); // E_STOPPED
        sEventTypes.addElement(PlayerListener.STOPPED_AT_TIME); // E_STOPPED_AT_TIME
        sEventTypes.addElement(PlayerListener.END_OF_MEDIA); // E_END_OF_MEDIA
        sEventTypes.addElement(PlayerListener.DURATION_UPDATED); // E_DURATION_UPDATED
        sEventTypes.addElement(PlayerListener.DEVICE_UNAVAILABLE); //   E_AUDIO_DEVICE_UNAVAILABLE
        sEventTypes.addElement(PlayerListener.VOLUME_CHANGED); //   E_VOLUME_CHANGED
        sEventTypes.addElement(PlayerListener.SIZE_CHANGED); // E_SIZE_CHANGED
        sEventTypes.addElement(PlayerListener.ERROR); // E_ERROR
        sEventTypes.addElement(PlayerListener.CLOSED); //   E_CLOSED
        sEventTypes.addElement(PlayerListener.RECORD_STARTED); //   E_RECORD_STARTED
        sEventTypes.addElement(PlayerListener.RECORD_STOPPED); //   E_RECORD_STOPPED
        sEventTypes.addElement(null); //    E_PRIVATE_DATA_AVAILABLE
        sEventTypes.addElement(null); //    E_PRIVATE_DATA_OVERWRITTEN
        sEventTypes.addElement(PlayerListener.BUFFERING_STARTED); //    E_BUFFERING_STARTED
        sEventTypes.addElement(PlayerListener.BUFFERING_STOPPED); //    E_BUFFERING_STOPPED
        sEventTypes.addElement(PlayerListener.RECORD_ERROR);        //  E_RECORD_ERROR
        sEventTypes.addElement("com.nokia.external.volume.event");  // E_NOKIA_EXTERNAL_VOLUME_EVENT
        sEventTypes.addElement("com.nokia.audiooutputchange.event");  // E_AUDIO_ROUTING_PREFERENCE_CHANGED_EVENT
    }

    private Vector iPlayerListeners = new Vector();
    private Player iPlayer;

    /**
     * Constructor
     * @param aPlayer Handle to player
     */
    public PlayerListenerImpl(Player aPlayer)
    {
        iPlayer = aPlayer;
    }

    /**
     * Adds PlayerListener
     * @param aPlayerListener Listener that will be added
     * @see PlayerListener
     */
    public synchronized void addPlayerListener(PlayerListener aPlayerListener)
    {
        if (aPlayerListener == null)
        {
            return;
        }
        if (!iPlayerListeners.contains(aPlayerListener))
        {
            iPlayerListeners.addElement(aPlayerListener);
        }
    }


    /**
     * Removes PlayerListener
     * @param aPlayerListener Listener that will be removed
     * @see PlayerListener
     */
    public synchronized void removePlayerListener(PlayerListener aPlayerListener)
    {
        iPlayerListeners.removeElement(aPlayerListener);
    }

    /**
     * Deliveres event to all registered PlayerListeners
     * @param aEventType type defined in PlayerListener
     * @param aEventData data for event
     */
    public synchronized void postEvent(String aEventType, Object aEventData)
    {
        Enumeration listeners = iPlayerListeners.elements();
        Logger.LOG(Logger.EJavaMMAPI, Logger.EInfo,"PlayerListenerImpl.java::PostEvent" + listeners);
        Logger.LOG(Logger.EJavaMMAPI, Logger.EInfo,"PlayerListenerImpl.java::PostEvent" + aEventType + aEventData );
        Logger.LOG(Logger.EJavaMMAPI, Logger.EInfo,"PlaerListenerImpl.java::PostEvent, this = "+this +" iPlayerListeners = "+iPlayerListeners);
        Logger.LOG(Logger.EJavaMMAPI, Logger.EInfo,"Listeners count = "+iPlayerListeners.size());
        while (listeners.hasMoreElements())
        {
            PlayerListener listener = (PlayerListener)listeners.nextElement();
            Logger.LOG(Logger.EJavaMMAPI, Logger.EInfo,"listener = "+listener);

            CallBackThread obj = new CallBackThread(aEventType,aEventData,listener,iPlayer);
            Thread t = new Thread(obj);
            t.start();
        }
    }

    /**
     * Posts a long event. Called from native implementation.
     * @param aEventType Event type
     * @param aEventData Event data
     */
    private void postLongEvent(int aEventType, long aEventData)
    {
        // event has come after this has been gc'd
        if (this == null)
        {
            return;
        }
        Long eventData = new Long(aEventData);
        postEvent(getEventType(aEventType), eventData);
    }

    /**
     * Posts a string event. Called from native implementation.
     * @param aEventType Event type
     * @param aEventData Event data
     */
    private void postStringEvent(int aEventType, String aEventData)
    {
        // event has come after this has been gc'd
        if (this == null)
        {
            return;
        }
        if (aEventType == E_SIZE_CHANGED)
        {
            VideoControl vc = (VideoControl)iPlayer.getControl(KVideoControlClassName);
            postEvent(getEventType(aEventType), vc);
        }
        else
        {
            postEvent(getEventType(aEventType), aEventData);
        }
    }

    /**
     * Posts object event. Called from native implementation.
     * @param aEventType Event type
     * @param aEventData Event data
     */
    private void postObjectEvent(int aEventType, Object aEventData)
    {
        // event has come after this has been gc'd
        if (this == null)
        {
            return;
        }
        postEvent(getEventType(aEventType), aEventData);
    }

    /**
     * Posts a control event. Called from native implementation.
     * @param aEventType Event type
     * @param aEventData Event data
     */
    private void postControlEvent(int aEventType, String aEventData)
    {
        // event has come after this has been gc'd
        if (this == null)
        {
            return;
        }
        Control eventData = iPlayer.getControl(aEventData);
        postEvent(getEventType(aEventType), eventData);
    }

    /**
     * Converts int eventType to String eventType
     * @param aEventType event type to be converted
     * @return converted event type
     */
    private String getEventType(int aEventType)
    {
        String eventType = null;
        eventType = (String)sEventTypes.elementAt(aEventType);
        return eventType;
    }
    
    class CallBackThread extends Thread
    {
        private String iEventType;
        private Object iEventData;
        private PlayerListener iListener;
        private Player iPlayer;
        
        public CallBackThread(String aEventType, Object aEventData, PlayerListener aListener, Player aPlayer)
        {
            iEventType = aEventType;
            iEventData = aEventData;
            iListener = aListener;
            iPlayer = aPlayer;
            
            
        }
        
        public void doCallBack()
        {
            
            
        }
        public void run()
        {
            iListener.playerUpdate(iPlayer,
                                  iEventType,
                                  iEventData);  
                                  
            Logger.LOG(Logger.EJavaMMAPI, Logger.EInfo,"after calling midlet playerupdate");
               
            
        }   
        
    }
}
