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
* Description:  This class is used for streaming audio.
*
*/

#ifndef CMMAAUDIOSTREAMPLAYER_H
#define CMMAAUDIOSTREAMPLAYER_H

//  INCLUDES
#include "CMMAAudioPlayer.h"
#include "MMMAStreamHandlerListener.h"

class CMMADataSourceStream;
class CMMAStreamHandler;


//  CLASS DECLARATION
/**
*   This class is used for playing audio from stream source.
*/
NONSHARABLE_CLASS(CMMAAudioStreamPlayer): public CMMAAudioPlayer,
        public MMMAStreamHandlerListener
{
public: // Construction
    /**
     * Creates new player.
     */
    static CMMAAudioStreamPlayer* NewLC(
        CMMAMMFResolver* aResolver);

    //   Destructor
    ~CMMAAudioStreamPlayer();

protected:
    //   C++ constructor
    CMMAAudioStreamPlayer(CMMAMMFResolver* aResolver);
    void ConstructL();

private: // New methods
    /**
     * Loops through iControllerInfos and tries to open those
     * @param aSourceUid Data source uid
     * @param aSourceData Data for source
     * @param aSinkUid Data sink uid
     * @param aSinkData Data for sink.
     * @param aPrioritySettings Controller's priority.
     * @return KErrNone if controller was opened
     */
    TInt DoOpen(TUid aSourceUid,
                const TDesC8& aSourceData,
                TUid aSinkUid,
                const TDesC8& aSinkData,
                TMMFPrioritySettings aPrioritySettings);

    /**
     * Starts playback and posts started event.
     */
    void PrivateStartL();

public: // New methods
    /**
     * Pause playback without sending stopped event or changing state
     */
    TInt Pause();

public: // from CMMAPlayer
    void StartL(TBool aPostEvent);
    void PrefetchL();
    CMMASourceStream* AddSourceStreamL(JNIEnv* aJNIEnv,
                                       CMMAEventSource* aEventSource,
                                       jobject aReader);
    void GetDuration(TInt64* aDuration);
    void StopL(TBool aPostEvent);

protected: // From CMMAAudioPlayer
    void PlayCompleteL(TInt aError);

public: // from MMMAStreamHandlerListener
    void PrepareComplete(TInt aError);
    void StartComplete(TInt aError);
    void HandleError(TInt aError);

private: // data
    // owned stream handler
    CMMAStreamHandler* iStreamHandler;

    // controller's state, prime panics if called twice
    TBool iControllerPrimed;
};

#endif // CMMAAUDIOSTREAMPLAYER_H
