/*
* Copyright (c) 2002-2009 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  This class is used for playing animated images.
*
*/


#ifndef CMMAANIMATIONPLAYER_H
#define CMMAANIMATIONPLAYER_H

//  INCLUDES
#include <w32std.h>
#include "cmmaplayer.h"
#include "mmmaguiplayer.h"
#include "mmmasnapshot.h"
#include "rmmatempfile.h"
#include "MIHLViewerObserver.h"
#include "MIHLImageViewer.h"

// FORWARD DECLARATIONS
class CMMAEventSource;
class CMMAAnimationWindow;
class MIHLFileImage;
class MIHLBitmap;
class CFbsBitmap;

// CONSTANTS
_LIT(KMMAVideoPlayer, "VideoPlayer");

NONSHARABLE_CLASS(MMMAAnimationObserver)
{
public: // new methods
    /**
     * Listener for animation advancing
     * @param aFrame current frame
     * @param aMediaTime current media time
     */
    virtual void AnimationAdvancedL(TInt aFrame, TInt64 aMediaTime) = 0;
};

//  CLASS DECLARATION
/**
*   This class is used for playing animated images.
*
*/
NONSHARABLE_CLASS(CMMAAnimationPlayer): public CMMAPlayer,
        public MMMAGuiPlayer,
        public MMMASnapshot,
        public MIHLViewerObserver
{
public: // Construction
    static CMMAAnimationPlayer* NewLC();
    static CMMAAnimationPlayer* NewLC(const TDesC& aFileName);

    //   Destructor
    ~CMMAAnimationPlayer();

protected:
    //   C++ constructor
    CMMAAnimationPlayer();
    void ConstructL();

public: // from CMMAPlayer
    void SetPlayerListenerObjectL(jobject aListenerObject,
                                  JNIEnv* aJni,
                                  MMMAEventPoster* aEventPoster);
    void RealizeL();
    void PrefetchL();
    void StartL(TBool aPostEvent);
    void StopL(TBool aPostEvent);
    void DeallocateL();
    void GetDuration(TInt64* aDuration);
    const TDesC& Type();
    void SetMediaTimeL(TInt64* aTime);
    void GetMediaTime(TInt64* aMediaTime);

public: // from CMMAPlayer/MMMASourceStreamListener
    void ReadCompletedL(TInt aStatus, const TDesC8& aData);

public: // new methods
    void PrefetchFileL();
    void PrefetchDataL(const TDesC8& aData);

    /**
     * Check whether this player is playing from a file locator
     * @return ETrue if is a file player, EFalse otherwise
     */
    TBool IsFilePlayer();

    // Finds current frame for given media time
    TInt FindFrame(TInt64 aTime);

    // Calculates media time for specified frame
    TInt64 MediaTimeForFrame(TInt aFrameIndex);

    // Frame duration of specified frame
    TTimeIntervalMicroSeconds32 FrameDuration(TInt aFrameIndex);

    MIHLImageViewer* Viewer();

    // Setter for frame listener, used for stop time control
    void SetAnimationObserver(MMMAAnimationObserver* aAnimationObserver);

    /**
     * @param aRate Rate to set in "milli-percentage"..
     * @return Actual rate set.
     */
    TInt SetRateL(TInt aRate);

    /**
     * @return The current playback rate in "milli-percentage".
     */
    TInt RateL();

private: // new methods
    void PrepareViewerL();
    void ProcessCurrentFrameL();

public: // From MMMAGuiPlayer
    void SetDisplayL(MMMADisplay* aDisplay);
    TSize SourceSize();
    void NotifyWithStringEvent(CMMAPlayerEvent::TEventType aEventType,
                               const TDesC& aStringEventData);

    MMMASnapshot* SnapshoterL();

public: // From MMMASnapshot
    MMMASnapshot::TEncoding TakeSnapshotL(TRequestStatus* aStatus,
                                          const TSize& aSize,
                                          const CMMAImageSettings& aSettings);
    CFbsBitmap* SnapshotBitmap();
    HBufC8* SnapshotEncoded();

public: // From MIHLViewerObserver
    virtual void ViewerBitmapChangedL();
    virtual void ViewerError(TInt aError);

private: // Data

    /**
     * Display to draw animation.
     */
    MMMADisplay* iDisplay;

    /**
     * Window to draw animation.
     */
    CMMAAnimationWindow* iWindow;

    /**
     * Dimensions of animation.
     */
    TSize iSourceSize;

    /**
     * Animated image to play. Owned.
     */
    MIHLFileImage* iImage;

    /**
     * Destination bitmap for animation. Owned.
     */
    MIHLBitmap* iBitmap;

    /**
     * Animation viewer. Owned.
     */
    MIHLImageViewer* iViewer;

    /**
     * Bitmap for snapshot. Ownership is transferred for
     * snapshot requester, but owned if not null.
     */
    CFbsBitmap* iSnapshotBitmap;

    /**
     * File system session for IHL interfacing
     */
    RFs iFSession;

    /**
     * Animation frame count. Held locally for optimization.
     */
    TInt iFrameCount;

    /**
     * Possible file name, owned
     */
    HBufC* iFileName;

    /**
     * Current Media Time
     */
    TInt64 iMediaTime;

    /**
     * Animation listener
     */
    MMMAAnimationObserver* iAnimationObserver;

    /**
     * Current rate
     */
    TInt iCurrentRate;

    /**
     * Hold the information whether playback should be started
     * from beginning on next start or from current position
     */
    TBool iEndReached;

    /**
     * EndOfMedia will be delivered when next frame arrives if
     * iSendEndOfMediaOnNextFrame is true
     */
    TBool iSendEndOfMediaOnNextFrame;
};

#endif // CMMAANIMATIONPLAYER_H
