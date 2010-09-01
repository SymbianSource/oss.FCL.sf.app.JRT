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
* Description:  This class is base class for all players.
*
*/


#ifndef CMMAPLAYER_H
#define CMMAPLAYER_H

//  INCLUDES
#include "cmmacontrol.h"
#include "cmmasourcestream.h"
#include "cmmaplayerevent.h"
#include "mmmasourcestreamlistener.h"
#include "mmmaplayerstatelistener.h"
#include "mmmacallstateobserver.h"

//  FORWARD DECLARATIONS
class CMMADurationUpdater;
class CMMACallStateMonitor;

//  CONTANTS

// Not enough stream to realize to player
const TInt KErrNotEnoughStreams = -7002;

/**
 * Used in set loop count to indicate forever loop.
 */
const TInt KJavaRepeatForever = -1;

/**
 * Returned to Java if duration or mediatime is unknown.
 */
const TInt KTimeUnknown = -1;

//  CLASS DECLARATION
/**
*   This class is base class for all players.
*
*
*/

class CMMAPlayer : public CBase,
        public MMMASourceStreamListener, MMMACallStateObserver

{
public:
    /**
     * Player state. Same that in Java side.
     */
    enum TPlayerState
    {
        EClosed = 0,
        EUnrealized = 100,
        ERealized = 200,
        EPrefetched = 300,
        EStarted = 400
    };

public:
    /**
     * Deletes all owned members.
     */
    IMPORT_C virtual ~CMMAPlayer();
protected:
    /**
     * Initializes member variables to defaults.
     */
    IMPORT_C CMMAPlayer();

    /**
     * Second phase construct.
     */
    void ConstructL();

public: // static methods
    /**
     * Creates and adds source stream to the player. New stream will be
     * owned by this player.
     *
     * @param aJniEnv Used to create source stream.
     * @param aPlayer Player to use.
     * @param aEventSource Used to create source stream.
     * @param aReader Java side stream object.
     * @param aSourceStream New stream's pointer will be set to this.
     */
    static void StaticAddSourceStreamL(JNIEnv* aJniEnv,
                                       CMMAPlayer* aPlayer,
                                       CMMAEventSource* aEventSource,
                                       jobject aReader,
                                       CMMASourceStream** aSourceStream);

    /**
     * Sets Java listener object that will be used to post player events
     * and control specific events.
     *
     * @param aPlayer Player to use.
     * @param aListenerObject Java side listener object.
     * @param aJni Used to get method ids.
     * @param aPoster Used to post events.
     */
    static void StaticSetPlayerListenerObjectL(CMMAPlayer* aPlayer,
            jobject aListenerObject,
            JNIEnv* aJni,
            MMMAEventPoster* aPoster);

    /**
     * Initializes action completed callbacks.
     *
     * @param aPlayer Player to use.
     * @param aPlayerObject Java side Player object.
     * @param aJni Used to get method id.
     */
    static void StaticInitPlayerL(CMMAPlayer* aPlayer,
                                  jobject aPlayerObject,
                                  JNIEnv* aJni);

    /**
     * Static getter for control objects.
     *
     * @param aPlayer Player to use.
     * @param aIndex Control's index.
     */
    IMPORT_C static CMMAControl* StaticControl(CMMAPlayer* aPlayer,
            TInt aIndex);

public: // New methods

    /**
     * Realizes the player. Implementations do not necessarily add
     * functionality in this method. If successful player is in
     * ERealized state.
     */
    virtual void RealizeL();

    /**
     * Prefetches data. Usually data from source stream will be read. When
     * prefetch completes action completed event will be delivered. If
     * successful player is in EPrefetched state.
     */
    virtual void PrefetchL() = 0;

    /**
     * Start playing. Started event will be posted. If there is no error
     * player will be in EStarted state.
     */
    virtual void StartL(TBool aPostEvent) = 0;

    /**
     * Stops playing. After this player may be restarted with StartL method.
     * After this player is in EPrefetched state.
     *
     * @param aPostEvent Indicates if java be informed.
     */
    virtual void StopL(TBool aPostEvent) = 0;

    /**
     * Releases resources. Player state can be changed.
     */
    virtual void DeallocateL() = 0;

    /**
     * Close the Player and release its resources. After this player is in
     * EClosed state and cannot be used anymore.
     */
    IMPORT_C virtual void CloseL();

    /**
     * Gets duration.
     *
     * @param aDuration Duration or KTimeUnknown if not specified.
     */
    IMPORT_C virtual void GetDuration(TInt64* aDuration);

    /**
     * Sets media time.
     *
     * @param aTime Time to set. When method returns parameter contains
     *              actual media time set.
     */
    virtual void SetMediaTimeL(TInt64* aTime);

    /**
     * Gets media time.
     *
     * @param aMediaTime When method returns parameter contains the media
     * time.
     */
    virtual void GetMediaTime(TInt64* aMediaTime);

    /**
     * Sets loop count.
     *
     * @param aCount Indicates the number of times the content will be
     *               played. KJavaRepeatForever indicates looping
     *               indefintely.
     */
    IMPORT_C virtual void SetLoopCount(TInt aCount);

    /**
     * Get the content type of the media that's being played back by this
     * Player.
     * @return The content type being played back by this Player. NULL if
     * content type is not available.
     */
    HBufC* ContentType() const;

public: // from MMMACallStateObserver
	
    void HandleCallStateEventL(TUid aUid, TInt aKey);
    
public: // new methods
    /**
     * Sets Java listener object that will be used to post player events
     * and control specific events.
     *
     * @param aListenerObject Java side listener object.
     * @param aJni Used to get method ids.
     * @param aPoster Used to post events.
     */
    IMPORT_C virtual void SetPlayerListenerObjectL(jobject aListenerObject,
                                          JNIEnv* aJni,
                                          MMMAEventPoster* aPoster);

    /**
     * Returns total count of the controls owned by this player.
     *
     * @return Count of the controls.
     */
    IMPORT_C TInt ControlCount();

    /**
     * Gets control. Ownership is not tranferred.
     *
     * @param aIndex Control index.
     */
    IMPORT_C CMMAControl* Control(TInt aIndex);

    /**
     * Adds new control. Ownership is transferred to this class.
     *
     * @param aControl New control.
     */
    IMPORT_C virtual void AddControlL(CMMAControl* aControl);

    /**
     * Adds listener. All listeners will informed when player state changes.
     *
     * @param aListener New listener.
     */
    IMPORT_C void AddStateListenerL(MMMAPlayerStateListener* aListener);

    /**
     * Removes a player state listener.
     *
     * @param aListener Listener to be removed.
     */
    IMPORT_C void RemoveStateListener(MMMAPlayerStateListener* aListener);

    /**
     * @return Player's state.
     */
    inline TInt State();

    /**
     * Returns player's type. Player types are defined in derived player
     * headers.
     * @return Player type.
     */
    virtual const TDesC& Type();

    /**
     * Sets player's content type, which can be queried with ContentType
     * method. Ownership is transfered.
     * @param aContentType Player's content type.
     */
    void SetContentType(HBufC* aContentType);

    /**
     * Reset all source streams.
     */
    void ResetSourceStreams();

    /**
     * Refresh all the controls.
     */
    void RefreshControls();
    /**
    * delete all the controls.
    */
    void DeleteControls();

public: // methods for informing java player listeners

    /**
     * Post event which event data will be Long java object.
     *
     * @param aEventType Event's type specified in CMMAPlayerEvent.
     * @param aLongEventData Event data context.
     */
    void PostLongEvent(CMMAPlayerEvent::TEventType aEventType,
                       const TInt64& aLongEventData);

    /**
     * Post event which event data will be string.
     *
     * @param aEventType Event's type specified in CMMAPlayerEvent.
     * @param aStringEventData Event data context.
     */
    IMPORT_C void PostStringEvent(CMMAPlayerEvent::TEventType aEventType,
                                  const TDesC& aStringEventData);

    /**
     * Post event which event data will be a java object.
     *
     * @param aEventType Event's type specified in CMMAPlayerEvent.
     * @param aEventData Java object.
     */
    IMPORT_C void PostObjectEvent(CMMAPlayerEvent::TEventType aEventType,
                                  const jobject aEventData);

    /**
     * Completes action and allows Java side to continue.
     *
     * @param aError of the action or KErrNone.
     */
    IMPORT_C void PostActionCompleted(TInt aError);

protected: // new methods
    /**
     * @see StaticAddSourceStreamL
     */
    IMPORT_C virtual CMMASourceStream* AddSourceStreamL(JNIEnv* aJniEnv,
            CMMAEventSource* aEventSource,
            jobject aReader);

    /**
     * Changes player's state and informs all MMMAPlayerStateListeners.
     *
     * @param aState New state.
     */
    IMPORT_C void ChangeState(TPlayerState aState);

public: // from MMMASourceStreamListener
    virtual void ReadCompletedL(TInt aStatus, const TDesC8& aData);

protected: // Memeber data
    /**
     * Array of controls. All controls are owned by this class.
     */
    RPointerArray< CMMAControl > iControls;

    /**
     * Array of streams. All streams are owned by this class.
     */
    RPointerArray< CMMASourceStream > iSourceStreams;


    /**
     * Array of listeners. Not owned.
     */
    RPointerArray< MMMAPlayerStateListener > iStateListeners;

    /**
     * Used to inform java side of completion.
     * Owned.
     */
    CMMAEvent* iActionCompletedEvent;

    /**
     * Player state listener object.
     */
    jobject iListenerObject;

    /**
     * java method postEvent(String,Object)
     */
    jmethodID iPostEvent;
    /**
     * java method postObjectEvent(int,Object)
     */
    jmethodID iPostObjectEvent;

    /**
     * java method postLongEvent(int,long)
     */
    jmethodID iPostLongEvent;

    /**
     * java method postStringEvent(int,String)
     */
    jmethodID iPostStringEvent;

    /**
     * java method postControlEvent(int,String)
     */
    jmethodID iPostControlEvent;

    /**
     * Not owned.
     */
    MMMAEventPoster* iEventPoster;

    /**
     * Total number of wanted loops.
     */
    TInt iRepeatNumberOfTimes;

    /**
     * Indicates if looping indefintely.
     */
    TBool iRepeatForever;

    /**
     * Current loop count.
     */
    TInt iRepeatCount;

    /**
     * Content duration in microseconds.
     */
    TInt64 iDuration;

    /**
     * Current state of the player.
     */
    TPlayerState iState;

    /**
     * Event for notificating Java that memory allocation has failed.
     * Owned.
     */
    CMMAPlayerEvent* iOOMErrorEvent;

    /**
     * The content type being played back by this Player.
     * Owned. May be NULL until type is available.
     */
    HBufC* iContentType;

    /**
     * Player state listener that sends duration update event to Java
     * if duration has changed when player state changes.
     */
    CMMADurationUpdater* iDurationUpdater;

		/**
    * Used to get the callback for Call State Events.
    */
    CMMACallStateMonitor* iStateObserver; 
    
    /**
     *  Used to determine if the Player is in Paused state
     *  due to call or done by user.
     */
    TBool isPausedByCall;
};


inline TInt CMMAPlayer::State()
{
    return iState;
}

#endif // CMMAPLAYER_H
