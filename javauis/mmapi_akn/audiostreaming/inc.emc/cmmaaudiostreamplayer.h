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
#include "cmmaemcaudioplayer.h"
#include "mmmastreamhandlerlistener.h"
#include "cmmaemcresolver.h"
#include <MetaDataUtility.h>

class CMMADataSourceStream;
class CMMAStreamHandler;


//  CLASS DECLARATION
/**
*   This class is used for playing audio from stream source.
*/
NONSHARABLE_CLASS(CMMAAudioStreamPlayer): public CMMAEMCAudioPlayer,
        public MMMAStreamHandlerListener
{
public: // Construction
    /**
     * Creates new player.
     */
    static CMMAAudioStreamPlayer* NewLC(
        CMMAEMCResolver* aResolver);

    //   Destructor
    ~CMMAAudioStreamPlayer();

protected:
    //   C++ constructor
    CMMAAudioStreamPlayer(CMMAEMCResolver* aResolver);
    void ConstructL();

private: // New methods

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
    void DeallocateL();
    void StartL(TBool aPostEvent);
    void PrefetchL();
    CMMASourceStream* AddSourceStreamL(JNIEnv* aJNIEnv,
                                       CMMAEventSource* aEventSource,
                                       jobject aReader);

    void GetDuration(TInt64* aDuration);
    void StopL(TBool aPostEvent);

protected: // From CMMAEMCAudioPlayer
    void PlayCompleteL(TInt aError);

public:
    void AddDataSourceToStreamL();
    void CreateStreamL();
    CMetaDataUtility* MetaDataUtilityOwnership();

public: // from MMMAStreamHandlerListener
    void PrepareComplete(TInt aError);
    void StartComplete(TInt aError);
    void HandleError(TInt aError);

public: // from MControlObserver
    void Event(MControl* aControl,
               TUint aEventType,
               TAny* aEventObject);

private: // data
    // owned stream handler
    CMMAStreamHandler* iStreamHandler;

    // controller's state, prime panics if called twice
    TBool iControllerPrimed;

    // created here and ownership is transferred to
    // CMMAAudioStreamMetaDataControl by call to
    // MetaDataUtilityOwnership()
    CMetaDataUtility* iMetaDataUtility;
};

#endif // CMMAAUDIOSTREAMPLAYER_H
