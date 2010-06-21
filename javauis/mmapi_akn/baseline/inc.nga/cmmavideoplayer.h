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
#include <AccMonitor.h>
#include <jutils.h>
#include "cmmaaudioplayer.h"
#include "mmmaguiplayer.h"
#include "mmmasnapshot.h"
#include "cmmasurfacewindow.h"

// FORWARD DECLARATIONS
class CMMAEventSource;
class CAccMonitorInfo;

// CONSTANTS
// Error code from MMF meaning that video is missing sound,
// but still can be played.
const TInt KNotCompleteVideoError = -12017;
// Error blitting video to display
const TInt KMMVideoBlitError = -12015;
_LIT(KMMAVideoPlayer, "VideoPlayer");

//  CLASS DECLARATION
/**
*   This class is used for playing video.
*
*/
NONSHARABLE_CLASS(CMMAVideoPlayer): public CMMAAudioPlayer,
        public MMMAGuiPlayer,
        public MMMASnapshot,
        public MAccMonitorObserver
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

public: // from MAccMonitorObserver
    void ConnectedL(CAccMonitorInfo* aAccessoryInfo );
    void DisconnectedL(CAccMonitorInfo* aAccessoryInfo);
    void AccMonitorObserverError( TInt aError );

protected: // New methods
    void CompletePrefetch(TInt aError);
    void PrepareDisplay();
    void SourceSizeChanged();

protected: // Data
    CMMASurfaceWindow* iSurfaceWindow;
    RMMFVideoControllerCustomCommands iVideoControllerCustomCommands;
    RMMFVideoPlayControllerCustomCommands iVideoPlayControllerCustomCommands;
    RMMFVideoPlaySurfaceSupportCustomCommands iVideoPlaySurfaceSupportCustomCommands;

private: // Data
    // not owned
    MMMADisplay* iDisplay;

    TSize iSourceSize;

    TFileName iFileExtension;

    /**
     * struct to keep track of Surface
     */
    struct MMASurface
    {
        // owned, should be freed using
        // RMMFVideoPlaySurfaceSupportCustomCommands::SurfaceRemoved().
        TSurfaceId iPrevSurfaceId;
        TBool iPrevSurfaceAvailable;
    } iMMASurface;

    // owned
    CFbsBitmap* iEmptySnapshotImage;

    // owned
    CActiveSchedulerWait* iActiveSchedulerWait;

    /**
     * owned
     * used for listening to audio/video cable connection status.
     */
    CAccMonitor *iAccMonitor;
    RAccMonCapabilityArray iAccMonCapabilityArray;
};

#endif // CMMAVIDEOPLAYER_H
