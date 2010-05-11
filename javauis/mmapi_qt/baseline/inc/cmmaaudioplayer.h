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
* Description:  This class is used for playing sounds
*
*/

#ifndef CMMAAUDIOPLAYER_H
#define CMMAAUDIOPLAYER_H

//  INCLUDES
#include "cmmammfplayerbase.h"
//#include <jutils.h>


_LIT(KMMAAudioPlayer, "AudioPlayer");

/**
Mixin class for playback complete notifications.
*/
class MPlaybackCompletedCallback
{
public:
    virtual void HandlePlaybackCompleteL() = 0;
    virtual void ErrorPlaybackComplete(TInt aError) = 0;
};

/**
Active object used for give MMF server time to complete its playback
*/
class CPlaybackCompletedCallback : public CTimer
{
public:// Constructor and destructor
    static CPlaybackCompletedCallback* NewL(MPlaybackCompletedCallback& aObs);
    ~CPlaybackCompletedCallback();

public: // new methods
    void Callback();

public: // From CTimer
    void RunL();
    TInt RunError(TInt aError);

private: // Constructor
    CPlaybackCompletedCallback(MPlaybackCompletedCallback& aObs);

private: // Data
    MPlaybackCompletedCallback& iObs;
};

//  CLASS DECLARATION
/**
*   This class is used for playing sounds
*
*/

NONSHARABLE_CLASS(CMMAAudioPlayer): public CMMAMMFPlayerBase,
        public MPlaybackCompletedCallback

{
public: // Construction
    /**
     * Creates new player.
     */
    static CMMAAudioPlayer* NewLC(
        CMMAMMFResolver* aResolver);

    //   Destructor
    ~CMMAAudioPlayer();

protected:
    //   C++ constructor
    CMMAAudioPlayer(CMMAMMFResolver* aResolver);
    void ConstructL();

protected: // New methods
    IMPORT_C virtual void PrefetchDataL(const TDesC8& aData);
    IMPORT_C virtual void PrefetchFileL();
    IMPORT_C virtual void PlayCompleteL(TInt aError);

public: // from CMMAPlayer
    void RealizeL();
    void PrefetchL();

    const TDesC& Type();

public: // from CMMAPlayer/MMMASourceStreamListener
    void ReadCompletedL(TInt aStatus, const TDesC8& aData);

public: // from MMMFControllerEventMonitorObserver
    void HandleEvent(const class TMMFEvent& aEvent);

public: // from MPlaybackCompletedCallback
    IMPORT_C void HandlePlaybackCompleteL();
    IMPORT_C void ErrorPlaybackComplete(TInt aError);

private: // data
    // Owned playback callback active object
    CPlaybackCompletedCallback* iPlaybackCompleted;
};

#endif // CMMAAUDIOPLAYER_H
