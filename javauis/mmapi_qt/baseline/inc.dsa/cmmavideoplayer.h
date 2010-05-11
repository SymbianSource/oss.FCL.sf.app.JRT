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
* Description:  This class is used for playing video.
*
*/


#ifndef CMMAVIDEOPLAYER_H
#define CMMAVIDEOPLAYER_H

//  INCLUDES
#include "cmmaaudioplayer.h"
#include "mmmaguiplayer.h"
#include "mmmasnapshot.h"
#include "cmmadsawindow.h"

// FORWARD DECLARATIONS
class MMAFunctionServer;

// CONSTANTS
// Error code from MMF meaning that video is missing sound,
// but still can be played.
const TInt KNotCompleteVideoError = -12017;
_LIT(KMMAVideoPlayer, "VideoPlayer");

//  CLASS DECLARATION
/**
*   This class is used for playing video.
*
*/

NONSHARABLE_CLASS(CMMAVideoPlayer): public CMMAAudioPlayer,
        public MMMAGuiPlayer,
        public MMMASnapshot
{
public: // Construction
    static CMMAVideoPlayer* NewLC(
        CMMAMMFResolver* aResolver);

    //   Destructor
    ~CMMAVideoPlayer();

protected:
    //   C++ constructor
    CMMAVideoPlayer(CMMAMMFResolver* aResolver);

    void ConstructL();

public: // from CMMAPlayer
    IMPORT_C void SetPlayerListenerObjectL(jobject aListenerObject,
                                           JNIEnv* aJni,
                                           MMMAEventPoster* aEventPoster);
    void RealizeL();
    void PrefetchL();

protected: // from CMMAudioPlayer
    IMPORT_C void ReadCompletedL(TInt aStatus, const TDesC8& aData);
    void HandleEvent(const TMMFEvent& aEvent);
    IMPORT_C const TDesC& Type();

public: // From MMMAGuiPlayer
    IMPORT_C void SetDisplayL(MMMADisplay* aDisplay);
    IMPORT_C TSize SourceSize();
    IMPORT_C void NotifyWithStringEvent(CMMAPlayerEvent::TEventType aEventType,
                                        const TDesC& aStringEventData);

    IMPORT_C MMMASnapshot* SnapshoterL();

public: // From MMMASnapshot
    IMPORT_C MMMASnapshot::TEncoding TakeSnapshotL(TRequestStatus* aStatus,
            const TSize& aSize,
            const CMMAImageSettings& aSettings);
    IMPORT_C CFbsBitmap* SnapshotBitmap();
    IMPORT_C HBufC8* SnapshotEncoded();

protected: // New methods
    void CompletePrefetch(TInt aError);
    void PrepareDisplay();
    void SourceSizeChanged();

protected: // Data
    CMMADSAWindow* iDSAWindow;
    RMMFVideoControllerCustomCommands iVideoControllerCustomCommands;
    RMMFVideoPlayControllerCustomCommands iVideoPlayControllerCustomCommands;

private: // Data
    MMMADisplay* iDisplay;

    TSize iSourceSize;

    TFileName iFileExtension;
    CFbsBitmap* iEmptySnapshotImage;

    CActiveSchedulerWait* iActiveSchedulerWait;
};

#endif // CMMAVIDEOPLAYER_H
