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
* Description:  This class is used for playing URL videos
*
*/


#ifndef CMMAVIDEOURLPLAYER_H
#define CMMAVIDEOURLPLAYER_H


//  INCLUDES
#include "cmmavideoplayer.h"
#include "mmmaguiplayer.h"
#include <es_sock.h>

// FORWARD DECLARATIONS
class CMMAEventSource;

//Can not open audio device or lost control of audio device
const TInt KErrMMAudioDevice = -12014;

//  CLASS DECLARATION
/**
*   This class is used for playing video.
*
*
*/

class CMMAVideoUrlPlayer: public CMMAVideoPlayer
{
protected:
    /**
     * Protected abstract base class for url player delegates.
     */
    class CMMAVideoUrlPlayerDelegate : public CBase
    {
    public:
        virtual ~CMMAVideoUrlPlayerDelegate();

    protected:
        /**
         * C++ constructor
         */
        CMMAVideoUrlPlayerDelegate(CMMAVideoUrlPlayer& aPlayer);

    public:
        /**
         * Handles StartL call for url player.
         */
        virtual void StartL(TBool aPostEvent) = 0;

        /**
         * Handles StopL call for url player.
         *
         * @param aPostEvent post event to Java side
         */
        virtual void StopL(TBool aPostEvent) = 0;

        /**
         * Gets media time for url player.
         *
         * @param aMediaTime When method returns parameter contains the media
         * time.
         */
        virtual void GetMediaTime(TInt64* aMediaTime) = 0;

        /**
         * Handles events for url player.
         *
         * @param aEvent event type
         */
        virtual void HandleEvent(const TMMFEvent& aEvent) = 0;

    protected: // data
        CMMAVideoUrlPlayer& iPlayer;
    };

    /**
     * Delegate class for non-live video streams
     */
    class CMMAVideoUrlPlayerClipStreamDelegate :
            public CMMAVideoUrlPlayerDelegate
    {
    public:
        CMMAVideoUrlPlayerClipStreamDelegate(CMMAVideoUrlPlayer& aPlayer);
        virtual ~CMMAVideoUrlPlayerClipStreamDelegate();

    public: // from CMMAVideoUrlPlayerDelegate
        void StartL(TBool aPostEvent);
        void StopL(TBool aPostEvent);
        void GetMediaTime(TInt64* aMediaTime);
        void HandleEvent(const TMMFEvent& aEvent);
    };

    /**
     * Delegate class for live video streams
     */
    class CMMAVideoUrlPlayerLiveStreamDelegate :
            public CMMAVideoUrlPlayerDelegate
    {
    public:
        static CMMAVideoUrlPlayerLiveStreamDelegate*
        NewL(CMMAVideoUrlPlayer& aPlayer);

        virtual ~CMMAVideoUrlPlayerLiveStreamDelegate();

    protected:
        CMMAVideoUrlPlayerLiveStreamDelegate(CMMAVideoUrlPlayer& aPlayer);
        void ConstructL();

    public: // from CMMAVideoUrlPlayerDelegate
        void StartL(TBool aPostEvent);
        void StopL(TBool aPostEvent);
        void GetMediaTime(TInt64* aMediaTime);
        void HandleEvent(const TMMFEvent& aEvent);

    private: // data
        CActiveSchedulerWait* iActiveSchedulerWait;

        /**
         * RTSP live stream returns current media time
         * from the server, but local media time must
         * start from zero. Thus start media time is
         * cached here.
         */
        TInt64 iMediaStartTime;

        /**
         * Hold player stop time value. It is otherwise
         * forgotten when player is stopped.
         */
        TInt64 iStoppedAtTime;
    };

public: // Construction
    static CMMAVideoUrlPlayer* NewLC(
        CMMAMMFResolver* aResolver,
        const TDesC& aUrl);

    //   Destructor
    IMPORT_C ~CMMAVideoUrlPlayer();

protected:
    //   C++ constructor
    IMPORT_C CMMAVideoUrlPlayer(CMMAMMFResolver* aResolver);
    IMPORT_C void ConstructL(const TDesC& aUrl);

public: // from CMMAPlayer
    IMPORT_C void RealizeL();
    IMPORT_C void PrefetchL();
    IMPORT_C void StartL(TBool aPostEvent);
    IMPORT_C void StopL(TBool aPostEvent);
    IMPORT_C void GetMediaTime(TInt64* aMediaTime);
    IMPORT_C void DeallocateL();
    IMPORT_C void CloseL();

protected: // New Methods
    void FindActiveIap(const TUint aConnectionCount, TUint& aActiveIap);
    virtual TBool IsLiveStreamL();

    /**
     * Handles events from delegate classes to
     * superclass HandleEvent.
     *
     * @param aEvent event data
     */
    void HandleEventToParent(const TMMFEvent& aEvent);

protected: // From CMMAVideoPlayer
    IMPORT_C void HandleEvent(const TMMFEvent& aEvent);

protected: // Data
    HBufC* iUrl; // Owned url
    RConnection iConnection;
    RSocketServ iSocketServ;
    TThreadPriority iOrigPriority;
    CMMAVideoUrlPlayerDelegate* iPlayerDelegate;
};

#endif // CMMAVIDEOURLPLAYER_H
