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
* Description:  This class is used for playing camera.
*
*/


#ifndef CMMACAMERAPLAYER_H
#define CMMACAMERAPLAYER_H

//  INCLUDES
#include <ecam.h>
#include "cmmaplayer.h"
#include "mmmaguiplayer.h"
#include "mmmasnapshot.h"
#include "cmmacamerawindow.h"

// CONSTANTS
_LIT(KMMACameraPlayer, "CameraPlayer");

//  CLASS DECLARATION
/**
*   This class is used for playing camera.
*
*
*/

NONSHARABLE_CLASS(CMMACameraPlayer): public CMMAPlayer,
        public MMMAGuiPlayer,
        public MMMASnapshot,
        public MCameraObserver
{
public: // Construction
    /**
     * Creates new CMMACameraPlayer for the camera at certain index.
     * Index must be smaller CCamera::CamerasAvailable().
     * @param aCameraIndex Index of the camera.
     */
    static CMMACameraPlayer* NewLC(TInt aCameraIndex);

    //   Destructor
    ~CMMACameraPlayer();

protected:
    //   C++ constructor
    CMMACameraPlayer();
    void ConstructL(TInt aCameraIndex);

private: // new methods
    TInt64 CurrentTime();
    void ResolveViewFinderSizeL(TSize& aSize);
    void ResolveScreenSizeL(TSize& aSize);
    void ResolveCaptureSizes(const CCamera::TFormat aFormat,
                             const TInt aNumImageSizesSupported,
                             const TSize& aRequestSize,
                             TSize& aSourceSize,
                             TInt& aSourceIndex,
                             TInt& aLargestIndex);

public: // from CMMAPlayer
    void StartL();
    void StopL(TBool aPostEvent);
    void DeallocateL();
    void RealizeL();
    void PrefetchL();
    void GetDuration(TInt64* aDuration);
    void SetMediaTimeL(TInt64* aTime);
    void GetMediaTime(TInt64* aMediaTime);
    void CloseL();
    const TDesC& Type();
public: // From MCameraObserver
    void ReserveComplete(TInt aError);
    void PowerOnComplete(TInt aError);
    void ViewFinderFrameReady(CFbsBitmap& aFrame);

    void ImageReady(CFbsBitmap* aBitmap,
                    HBufC8* aData,
                    TInt aError);

    void FrameBufferReady(MFrameBuffer* aFrameBuffer,
                          TInt aError);

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

public: // New methods
    TInt CameraHandle();
    /**
     * Disables or enables viewfinder stopping.
     * Needed when using recording.
     *
     * @param aStopViewFinder If true viewfinder will be really stopped.
     */
    void SetViewFinderMode(TBool aStopViewFinder);

private: // Data
    /**
      * Window used to render viewfinder.
      * Owned.
      */
    CMMACameraWindow* iWindow;

    /**
     * Camera instance used for taking snapshots and recording.
     * iWindow uses this to create duplicated camera instance.
     * Owned.
     */
    CCamera* iCamera;
    MMMADisplay* iDisplay;

    // index to be used for size enumeration
    TInt iSourceSizeIndex;

    TInt64 iMediaTime;
    TInt64 iStartTime;

    TRequestStatus* iSnapshotStatus;

    // Owned.
    CFbsBitmap* iSnapshotBitmap;
    HBufC8* iSnapshotEncoded;

    TSize iSize;
    TBool iStopViewFinder;

    // inner class for waiting realize, owned
    class CRealizeWait : public CActiveSchedulerWait
    {
    public:
        TInt iError;
    };
    CRealizeWait* iRealizeWait;
};

#endif // CMMACAMERAPLAYER_H
