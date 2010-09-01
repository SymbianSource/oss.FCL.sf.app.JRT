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
#include <sensrvdatalistener.h>
#include <sensrvchannel.h>
#include <sensrvorientationsensor.h>
#include <ECamOrientationCustomInterface2.h>
#include <ECamUIOrientationOverrideCustomAPI.h>
#include <ecam.h>
#include "cmmaplayer.h"
#include "mmmaguiplayer.h"
#include "mmmasnapshot.h"
#include "cmmacamerawindow.h"

// CONSTANTS
_LIT(KMMACameraPlayer, "CameraPlayer");

enum TCamActiveCamera
{
  ECamActiveCameraPrimary,
  ECamActiveCameraSecondary,
  ECamActiveCameraNone
};


enum TCamImageOrientation
{
  ECamOrientation0,
  ECamOrientation90,
  ECamOrientation180,
  ECamOrientation270,
  ECamOrientationIgnore,
  ECamOrientationCount
};

//  CLASS DECLARATION
/**
*   This class is used for playing camera.
*
*
*/

NONSHARABLE_CLASS(CMMACameraPlayer): public CMMAPlayer,
        public MMMAGuiPlayer,
        public MMMASnapshot,
        public MCameraObserver2,
        public MSensrvDataListener
{
public: // Construction
    /**
     * Creates new CMMACameraPlayer for the camera at certain index.
     * Index must be smaller CCamera::CamerasAvailable().
     * @param aCameraIndex Index of the camera.
     */
    static CMMACameraPlayer* NewLC(TCamActiveCamera aCameraIndex);

    //   Destructor
    ~CMMACameraPlayer();

protected:
    //   C++ constructor
    CMMACameraPlayer(TCamActiveCamera aCameraIndex);
    void ConstructL(TCamActiveCamera aCameraIndex);

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
    /**
     * Registers and start listening to sensor channel.
     */
    void UpdateSensorInfoL();

    /**
     * Copies bitmap image from camerabuffer to iSnapshotBitmap
     */
    void HandleBitmapCopyL(MCameraBuffer& aCameraBuffer,
                           CFbsBitmap* aSrcBitmap);

    /**
     * Determines amount of image rotation required for the given sensor
     * orientation input.
     */
    static TCamImageOrientation MapSensorOrientatio2CamOrientation( 
        const TSensrvOrientationData::TSensrvDeviceOrientation& aSensorOrientation, 
        TCamImageOrientation aLastImageOrientation,
        TCamActiveCamera aActiveCameraIndex);

    /**
     * Convert UI orienation setting value of type TCamImageOrientation
     * to MCameraOrientation TOrientation value.
     */
    static MCameraOrientation::TOrientation 
      Map2CameraOrientation(const TCamImageOrientation& aSettingOrientation);

public: // from CMMAPlayer
    void StartL(TBool aPostEvent);
    void StopL(TBool aPostEvent);
    void DeallocateL();
    void RealizeL();
    void PrefetchL();
    void GetDuration(TInt64* aDuration);
    void SetMediaTimeL(TInt64* aTime);
    void GetMediaTime(TInt64* aMediaTime);
    void CloseL();
    const TDesC& Type();
public: // From MCameraObserver2
    void HandleEvent(const TECAMEvent& aEvent);
    void ViewFinderReady(MCameraBuffer& aCameraBuffer,TInt aError);
    void ImageBufferReady(MCameraBuffer& aCameraBuffer,TInt aError);
    void VideoBufferReady(MCameraBuffer& aCameraBuffer,TInt aError);

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

private: // From MSensrvDataListener
    void DataReceived(CSensrvChannel& aChannel,
                       TInt aCount, 
                       TInt aDataLost);
    void DataError(CSensrvChannel& aChannel,
                    TSensrvErrorSeverity aError);
    void GetDataListenerInterfaceL(TUid aInterfaceUid,
                                    TAny*& aInterface);

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

    TCamActiveCamera iActiveCameraIndex;
    TCamImageOrientation  iImageOrientation;
    TCamImageOrientation  iLastImageOrientation;
    // owned
    CSensrvChannel* iAccSensorChannel;
    // not owned
    MCameraOrientation* iCustomInterfaceOrientation;
    // not owned
    MCameraUIOrientationOverride* iCustomInterfaceUIOrientationOverride;
};

#endif // CMMACAMERAPLAYER_H
