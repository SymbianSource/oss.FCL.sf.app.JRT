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
* Description:  This class is used for playing camera.
*
*/


//  INCLUDE FILES
#include <sensrvchannelfinder.h>
#include <sensrvorientationsensor.h>
#include <jdebug.h>
#include <fbs.h>
#include <e32svr.h>
#include "cmmacameraplayer.h"
#include "tmmaparametervalidator.h"
#include "mmmadisplay.h"
#include "cmmacamerasound.h"

#if defined( __WINS__ )
#include <w32std.h>
#endif

// CONSTANTS
_LIT8(KImageJpegMime, "image/jpeg");
_LIT8(KJpegMime, "jpeg");
_LIT(KVideoControlName, "VideoControl");

CMMACameraPlayer* CMMACameraPlayer::NewLC(TCamActiveCamera aCameraIndex)
{
    CMMACameraPlayer* self = new(ELeave) CMMACameraPlayer(aCameraIndex);
    CleanupStack::PushL(self);
    self->ConstructL(aCameraIndex);
    return self;
}

CMMACameraPlayer::~CMMACameraPlayer()
{
    DEBUG("CMMACameraPlayer::~CMMACameraPlayer +");

    // Free (duplicated) UI camera resources first.
    // Window is not able to send any
    // callback requests to UI from now.
    if (iWindow)
    {
        iWindow->SetDisplay(NULL);
    }

    if (iCamera)
    {
        iCamera->CancelCaptureImage();
        iCamera->Release();
        delete iCamera;
    }

    delete iSnapshotEncoded;
    delete iSnapshotBitmap;
    delete iRealizeWait;

    if (iDisplay && iDisplay->HasContainer())
    {
        // Window will delete itself
        // after all pending events are processed
        // (lazy delete)
        iDisplay->UIGetCallback(
            *iWindow, CMMACameraWindow::EDestroyWindow);
    }
    else
    {
        delete iWindow;
    }
    iWindow = NULL;
    // Delete sensor api object
    delete iAccSensorChannel;
    DEBUG("CMMACameraPlayer::~CMMACameraPlayer -");
}


CMMACameraPlayer::CMMACameraPlayer(TCamActiveCamera aCameraIndex):
        iDisplay(NULL),
        iSourceSizeIndex(KErrNotFound),
        iStartTime(KErrNotFound),
        iSnapshotEncoded(NULL),
        iActiveCameraIndex(aCameraIndex),
        iImageOrientation(ECamOrientation0),
        iLastImageOrientation(ECamOrientation0)
{
    iStopViewFinder = ETrue;
}


void CMMACameraPlayer::ConstructL(TCamActiveCamera aCameraIndex)
{
    CMMAPlayer::ConstructL();

    if (aCameraIndex >= CCamera::CamerasAvailable())
    {
        // image capture is not supported
        User::Leave(KErrNotFound);
    }

    iCamera = CCamera::New2L(*this, aCameraIndex, 100);

    iCustomInterfaceOrientation =
      static_cast <MCameraOrientation*>(
        iCamera->CustomInterface(KCameraOrientationUid ));
    iCustomInterfaceUIOrientationOverride =
      static_cast <MCameraUIOrientationOverride*>(
      iCamera->CustomInterface(KCameraUIOrientationOverrideUid));
    // Fix to landscape mode
    iCustomInterfaceUIOrientationOverride->SetOrientationModeL(1);

    iWindow = CMMACameraWindow::NewL(iCamera->Handle());

    TCameraInfo cameraInfo;
    iCamera->CameraInfo(cameraInfo);

    if (cameraInfo.iNumImageSizesSupported < 1)
    {
        // image capture is not supported
        User::Leave(KErrNotFound);
    }

    // default snapshot size
    iSourceSizeIndex = cameraInfo.iNumImageSizesSupported - 1;

    iRealizeWait = new(ELeave)CRealizeWait;
}

TInt64 CMMACameraPlayer::CurrentTime()
{
    TTime time;
    time.HomeTime();
    return time.Int64();
}

void CMMACameraPlayer::ResolveViewFinderSizeL(TSize& aSize)
{
    DEBUG("CMMACameraPlayer::ResolveViewFinderSizeL");

    TSize resultSize;

    // The only way to find out the size is to start the view finder
    // with a proper size (screen size).

    ResolveScreenSizeL(resultSize);

    // StartViewFinderBitmapsL changes resultSize to
    // the used view finder size.
    // Used to get the source size only.
    iCamera->StartViewFinderBitmapsL(resultSize);

    // Bitmap viewfinder is not used anymore.
    iCamera->StopViewFinder();

    aSize = resultSize;
}

void CMMACameraPlayer::ResolveScreenSizeL(TSize& aSize)
{
    DEBUG("CMMACameraPlayer::ResolveScreenSizeL");

#if defined( __WINS__ )
    TSize size(0,0);
    RWsSession ws;

    if (ws.Connect() == KErrNone)
    {
        CleanupClosePushL(ws);

        CWsScreenDevice* wsScreenDevice = new(ELeave)CWsScreenDevice(ws);
        CleanupStack::PushL(wsScreenDevice);

        User::LeaveIfError(wsScreenDevice->Construct());

        size = wsScreenDevice->SizeInPixels();

        CleanupStack::PopAndDestroy(2);    // wsScreenDevice, ws.Close()
    }

    aSize = size;

#else
    TScreenInfoV01 info;
    TPckgBuf< TScreenInfoV01 > buf(info);

    UserSvr::ScreenInfo(buf);
    info = buf();

    aSize = info.iScreenSize;
#endif
}

void CMMACameraPlayer::ResolveCaptureSizes(const CCamera::TFormat aFormat,
        const TInt aNumImageSizesSupported,
        const TSize& aRequestSize,
        TSize& aSourceSize,
        TInt& aSourceIndex,
        TInt& aLargestIndex)
{
    // Largest image size
    TSize largestSize;
    // Index to largest image size
    TInt largestSizeIndex = 0;
    // Source size
    TSize sourceSize;
    // Default source size index not set
    TInt sourceSizeIndex = KErrNotFound;
    // Temporary size for iterating capture sizes
    TSize tmpSize;

    DEBUG_INT("MMA::CMMACameraPlayer::ResolveCaptureSizes: aFormat = 0x%x", aFormat);

    // go through all supported sizes.
    // Notice: Capture sizes are assumed to be in order from smaller to larger sizes
    for (TInt i = 0; i < aNumImageSizesSupported; i++)
    {
        iCamera->EnumerateCaptureSizes(tmpSize,
                                       i,
                                       aFormat);

        DEBUG_INT("MMA::CMMACameraPlayer::ResolveCaptureSizes: tmpSize.iWidth = %d", tmpSize.iWidth);
        DEBUG_INT("MMA::CMMACameraPlayer::ResolveCaptureSizes: tmpSize.iHeight = %d", tmpSize.iHeight);

        // Check if current is the largest
        if ((largestSize.iWidth < tmpSize.iWidth) &&
                (largestSize.iHeight < tmpSize.iHeight))
        {
            largestSize = tmpSize;
            largestSizeIndex = i;
        }

        // If wanted size is smaller than tmpSize we can use it
        if ((aRequestSize.iWidth <= tmpSize.iWidth) &&
                (aRequestSize.iHeight <= tmpSize.iHeight))
        {
            sourceSize = tmpSize;
            sourceSizeIndex = i;
        }
    }

    DEBUG_INT("MMA::CMMACameraPlayer::ResolveCaptureSizes: sourceSizeIndex = %d", sourceSizeIndex);
    DEBUG_INT("MMA::CMMACameraPlayer::ResolveCaptureSizes: largestSizeIndex = %d", largestSizeIndex);

    aSourceSize = sourceSize;
    aSourceIndex = sourceSizeIndex;
    aLargestIndex = largestSizeIndex;
}

void CMMACameraPlayer::StartL(TBool aPostEvent)
{
    DEBUG_INT("CMMACameraPlayer:StartL iState %d", iState);

    // start can't be called to not ready player
    if (iState == EPrefetched)
    {
        // camera passes ready images through
        // ViewFinderFrameReady method

        // set time when started
        iStartTime = CurrentTime();

        // inform java side
        ChangeState(EStarted);

        TInt64 time;
        GetMediaTime(&time);

        // Notify the camera window
        // about the status change
        iWindow->SetStarted(ETrue);

        if (aPostEvent)
        {
            // inform java side
            PostLongEvent(CMMAPlayerEvent::EStarted, time);
        }
    }
    PostActionCompleted(KErrNone);   // java start return
}

void CMMACameraPlayer::StopL(TBool aPostEvent)
{
    DEBUG_INT("CMMACameraPlayer::StopL", iState);
    if (iState == EStarted)
    {
        TInt64 time;
        GetMediaTime(&time);   // add played time to media time

        if (iStopViewFinder && iWindow->ViewFinderActive())
        {
            iWindow->SetStarted(EFalse);
        }
        iStartTime = KErrNotFound;

        if (aPostEvent)
        {
            PostLongEvent(CMMAPlayerEvent::EStopped, time);
        }

        // go back to prefetched state
        ChangeState(EPrefetched);
    }
}

void CMMACameraPlayer::DeallocateL()
{
    // If player is started when deallocate is called,
    // player is stopped from java side -> state is changed to
    // prefetched.
    // In prefetched state only reserved resource is
    // camera reserve( released with iCamera->Release() )
    // In realized state no resources have been reserved.
    // CHANGED: not releasing camera anymore, since it is already
    // done in realized state
    if (iState == EPrefetched)
    {
        ChangeState(ERealized);
    }
}


void CMMACameraPlayer::RealizeL()
{
    iCamera->Reserve();
    // this lock will be released when power on is completed (or error)
    if (!iRealizeWait->IsStarted())
    {
        iRealizeWait->Start();
    }
    User::LeaveIfError(iRealizeWait->iError);
    CMMAPlayer::RealizeL();
}


void CMMACameraPlayer::PrefetchL()
{
    DEBUG("MMA::CMMACameraPlayer::PrefetchL");
    // nothing to do here
    ChangeState(EPrefetched);
    PostActionCompleted(KErrNone);
}


void CMMACameraPlayer::GetDuration(TInt64* aDuration)
{
    // camera viewer has no duration.
    *aDuration = KTimeUnknown;
}


void CMMACameraPlayer::SetMediaTimeL(TInt64* /*aTime*/)
{
    DEBUG("MMA::CMMACameraPlayer::SetMediaTimeL ");
    // with camera media time is not supported.
}


void CMMACameraPlayer::GetMediaTime(TInt64* aMediaTime)
{
    if (iState == EStarted)
    {
        // add play time to media time
        iMediaTime += CurrentTime() - iStartTime;
        // set new start time
        iStartTime = CurrentTime();
    }

    // set value to parameter
    (*aMediaTime) = iMediaTime;
}

void CMMACameraPlayer::CloseL()
{
    DEBUG("MMA::CMMACameraPlayer::CloseL()");

    // cancel all activity
    iCamera->CancelCaptureImage();

    // Stop and release UI Camera instance
    iWindow->SetDisplay(NULL);

    // we don't need reserve camera anymore
    iCamera->Release();

    iCustomInterfaceOrientation = NULL;
    if (iAccSensorChannel)
    {
        iAccSensorChannel->StopDataListening();
    }

    if (iDisplay)
    {
        iDisplay->FixUIOrientation(EFalse);
    }

    CMMAPlayer::CloseL();
}

const TDesC& CMMACameraPlayer::Type()
{
    return KMMACameraPlayer;
}


void CMMACameraPlayer::HandleEvent(const TECAMEvent& aEvent)
{
    TInt error = aEvent.iErrorCode;
    if (KUidECamEventReserveComplete == aEvent.iEventType)
    {
        DEBUG_INT("MMA::CMMACameraPlayer::HandleEvent:KUidECamEventReserveComplete error  %d", error);
        if (error == KErrNone)
        {
            // camera will notify completion with PowerOnComplete method.
            iCamera->PowerOn();
        }
        else
        {
            // release lock and store error. State doesn't change.
            iRealizeWait->iError = error;
            iRealizeWait->AsyncStop();
        }
    }
    else if (KUidECamEventPowerOnComplete == aEvent.iEventType)
    {
        DEBUG_INT("MMA::CMMACameraPlayer::HandleEvent:KUidECamEventPowerOnComplete error  %d", error);

        TSize viewFinderSize;

        if (error == KErrNone)
        {
            // The view finder size must be known after prefetching.
            TRAP(error, ResolveViewFinderSizeL(viewFinderSize));
        }

        if (error == KErrNone)
        {
            iSize = viewFinderSize;

            if (iDisplay)
            {
                iDisplay->SourceSizeChanged(iSize);
                NotifyWithStringEvent(CMMAPlayerEvent::ESizeChanged, KVideoControlName);

                iDisplay->FixUIOrientation(ETrue);
            }
        }

        iRealizeWait->iError = error;
        iRealizeWait->AsyncStop();
    }
}

void CMMACameraPlayer::ViewFinderReady(MCameraBuffer& aCameraBuffer,
                                       TInt /*aError*/)
{
    aCameraBuffer.Release();
}
	
void CMMACameraPlayer::ImageBufferReady(MCameraBuffer& aCameraBuffer,TInt aError)
{
    DEBUG_INT("MMA::CMMACameraPlayer::ImageBufferReady %d", aError);

    // This method should never be called,
    // unless we are taking snapshot
    __ASSERT_DEBUG(iSnapshotStatus, User::Invariant());
    __ASSERT_DEBUG(!iSnapshotBitmap, User::Invariant());
    __ASSERT_DEBUG(!iSnapshotEncoded, User::Invariant());

    TInt error = aError;
    if (aError == KErrNone)
    {
        CFbsBitmap* srcBitmap = NULL;
        TRAP(error, HandleBitmapCopyL(aCameraBuffer, srcBitmap));
        DEBUG_INT("MMA::CMMACameraPlayer::ImageBufferReady HandleBitmapCopyL %d", error);

        // picture data is not of bitmap type
        if (!srcBitmap && (KErrNotSupported == error))
        {
            TDesC8* temp = NULL;
            TRAP(error, temp = aCameraBuffer.DataL(0));
            DEBUG_INT("CMMACameraPlayer::CopyImageDataL DataL error %d", error);
            if (KErrNone == error)
            {
                iSnapshotEncoded = temp->Alloc();
            }
        }
    }

    // notify the caller, error code or KErrNone
    User::RequestComplete(iSnapshotStatus, error);
    iWindow->SetStarted(ETrue);
    // Status is not needed anymore
    // and this class don't own the status.
    iSnapshotStatus = NULL;
    aCameraBuffer.Release();
}

void CMMACameraPlayer::VideoBufferReady(MCameraBuffer& aCameraBuffer,
                                        TInt /*aError*/)
{
    aCameraBuffer.Release();
}

void CMMACameraPlayer::HandleBitmapCopyL(MCameraBuffer& aCameraBuffer, 
                                         CFbsBitmap* aSrcBitmap)
{
    DEBUG("MMA::CMMACameraPlayer::HandleBitmapCopyL +");
    
    aSrcBitmap = &(aCameraBuffer.BitmapL(0)); // not owned

    iSnapshotBitmap = new(ELeave)CFbsBitmap();

    User::LeaveIfError(iSnapshotBitmap->Create(aSrcBitmap->SizeInPixels(),
                                             aSrcBitmap->DisplayMode()));
    CFbsBitmapDevice* bitmapDevice = NULL;
    CFbsBitGc* bitmapGc = NULL;

    bitmapDevice = CFbsBitmapDevice::NewL(iSnapshotBitmap);
    bitmapDevice->CreateContext(bitmapGc);
    bitmapGc->BitBlt(TPoint(0,0), aSrcBitmap);

    delete bitmapGc;
    delete bitmapDevice;
    DEBUG("MMA::CMMACameraPlayer::HandleBitmapCopyL -");
}

void CMMACameraPlayer::SetDisplayL(MMMADisplay* aDisplay)
{
    DEBUG("MMA::CMMACameraPlayer::SetDisplay");

    // now it is ready to draw
    iDisplay = aDisplay;

    // Passes display into iWindow.
    // Allocates all resources needed to use a camera DirectViewFinder.
    iWindow->SetDisplay(aDisplay);

    iDisplay->SetWindowL(iWindow);

    // Set view finder size to the display only if the view finder
    // size has been resolved.
    if (iSize != TSize(0, 0))
    {
        iDisplay->SourceSizeChanged(iSize);
        NotifyWithStringEvent(CMMAPlayerEvent::ESizeChanged, KVideoControlName);
    }

    iDisplay->FixUIOrientation(ETrue);
    UpdateSensorInfoL();
}

TSize CMMACameraPlayer::SourceSize()
{
    return iSize;
}

MMMASnapshot::TEncoding CMMACameraPlayer::TakeSnapshotL(TRequestStatus* aStatus,
        const TSize& aSize,
        const CMMAImageSettings& aSettings)
{
    __ASSERT_DEBUG(!iSnapshotStatus, User::Invariant());
    __ASSERT_DEBUG(!iSnapshotBitmap, User::Invariant());
    __ASSERT_DEBUG(!iSnapshotEncoded, User::Invariant());

    // snapshots can not be taken if player is not realized
    if (iState < ERealized)
    {
        User::Leave(KErrNotReady);
    }
    // save status which will be notified
    iSnapshotStatus = aStatus;

    // Source size not set in the beginning
    TSize sourceSize;

    // Use default if size is not specified.
    TInt sourceSizeIndex;
    
    if (aSize.iWidth != KErrNotFound &&
            aSize.iHeight != KErrNotFound)
        {
        sourceSizeIndex = iSourceSizeIndex;
        }
    else
        {
        sourceSizeIndex = 0;
        }
    
    // Largest image size index
    TInt largestSizeIndex = 0;

    // Get camera characteristics
    TCameraInfo cameraInfo;
    iCamera->CameraInfo(cameraInfo);

    // Set default snapshot encoding type
    TEncoding encoding = EEncoded;
    // Set default image capture format
    CCamera::TFormat format = CCamera::EFormatExif;

    DEBUG_INT("MMA::CMMACameraPlayer::TakeSnapshotL: cameraInfo.iImageFormatsSupported = 0x%x", cameraInfo.iImageFormatsSupported);
    DEBUG_INT("MMA::CMMACameraPlayer::TakeSnapshotL: cameraInfo.iNumImageSizesSupported = %d", cameraInfo.iNumImageSizesSupported);

    // Check if size was set in Java
    if (aSize.iWidth != KErrNotFound &&
            aSize.iHeight != KErrNotFound)
    {
        DEBUG("MMA::CMMACameraPlayer::TakeSnapshotL size was set in MIDlet");
        // Determine if jpeg capture was requested
        if ((*aSettings.iMimeType == KJpegMime) ||
                (*aSettings.iMimeType == KImageJpegMime))
        {
            // Shortcut for supported image formats
            TUint32 imageFormats = cameraInfo.iImageFormatsSupported;

            // Jpeg subtype constants
            _LIT8(KJpegJfif, "jfif");
            _LIT8(KJpegExif, "exif");

            // Resolve requested jpeg subtype from settings and camerainfo
            if ((imageFormats & CCamera::EFormatJpeg) &&
                    (*aSettings.iType == KJpegJfif))
            {
                // JFIF JPEG
                format = CCamera::EFormatJpeg;
                encoding = EEncoded;
            }
            else if ((imageFormats & CCamera::EFormatExif) &&
                     (*aSettings.iType == KJpegExif))
            {
                // EXIF JPEG
                format =  CCamera::EFormatExif;
                encoding = EEncoded;
            }
        }

        // Try to resolve nearest source size to the one requested,
        // except for the JFIF and EXIF jpeg subtypes the match has
        // to be exact otherwise non-encode capture format will be
        // used
        ResolveCaptureSizes(format,
                            cameraInfo.iNumImageSizesSupported,
                            aSize,
                            sourceSize,
                            sourceSizeIndex,
                            largestSizeIndex);

        // Format was either of the jpeg formats and requested size
        // didn't match the source size
        if ((format >= CCamera::EFormatJpeg &&
                format <= CCamera::EFormatExif) &&
                (aSize != sourceSize))
        {
            // Try again with an non-encoded format
            format = CCamera::EFormatFbsBitmapColor16M;
            encoding = EBitmap;
            ResolveCaptureSizes(format,
                                cameraInfo.iNumImageSizesSupported,
                                aSize,
                                sourceSize,
                                sourceSizeIndex,
                                largestSizeIndex);
        }

        if (sourceSizeIndex == KErrNotFound)
        {
            // If correct index was not found use the largest.
            sourceSizeIndex = largestSizeIndex;
        }
    }
    // else use default iSourceSizeIndex and default image capture format

    iCamera->PrepareImageCaptureL(format,
                                  sourceSizeIndex);

    // play sound when capturing image
    CMMACameraSound::PlayImageCaptureSoundL();
    
    // changing status to pending
    *iSnapshotStatus = KRequestPending;
    // start capture, ImageBufferReady will be called when ready
    iWindow->SetStarted(EFalse);

    iCamera->CaptureImage();

    DEBUG_INT("MMA::CMMACameraPlayer::TakeSnapshotL: format = 0x%x", format);
    DEBUG_INT("MMA::CMMACameraPlayer::TakeSnapshotL: sourceSizeIndex = %d", sourceSizeIndex);
    DEBUG_INT("MMA::CMMACameraPlayer::TakeSnapshotL: encoding = %d", encoding);

    // Inform caller which snapshot encoding was ultimately used
    return encoding;
}

CFbsBitmap* CMMACameraPlayer::SnapshotBitmap()
{
    CFbsBitmap* bitmap = iSnapshotBitmap;

    // ownership transfers to the caller
    iSnapshotBitmap = NULL;
    return bitmap;
}

HBufC8* CMMACameraPlayer::SnapshotEncoded()
{
    HBufC8* encoded = iSnapshotEncoded;

    // ownership transfers to the caller
    iSnapshotEncoded = NULL;
    return encoded;
}

void CMMACameraPlayer::NotifyWithStringEvent(
    CMMAPlayerEvent::TEventType aEventType,
    const TDesC& aStringEventData)
{
    PostStringEvent(aEventType, aStringEventData);
}

MMMASnapshot* CMMACameraPlayer::SnapshoterL()
{
    return this;
}

TInt CMMACameraPlayer::CameraHandle()
{
    return iCamera->Handle();
}

void CMMACameraPlayer::SetViewFinderMode(TBool aStopViewFinder)
{
    iStopViewFinder = aStopViewFinder;
}

void CMMACameraPlayer::UpdateSensorInfoL()
{
    DEBUG("CMMACameraPlayer::UpdateSensorInfoL +");
    CSensrvChannelFinder* channelFinder;
    channelFinder = CSensrvChannelFinder::NewL();
    CleanupStack::PushL(channelFinder);

    //List of found channels.
    RSensrvChannelInfoList channelInfoList;
    CleanupClosePushL(channelInfoList);

    //Create and fill channel search criteria.
    TSensrvChannelInfo channelInfo;
    channelInfo.iChannelType = KSensrvChannelTypeIdOrientationData;
    //Find the orientation channel
    channelFinder->FindChannelsL(channelInfoList, channelInfo);

    if(channelInfoList.Count() != 1)
    {
        DEBUG("CMMACameraPlayer::UpdateSensorInfoL, Orientation channel not found");
        User::Leave( KErrNotSupported );
    }
    else
    {
        DEBUG("CMMACameraPlayer::UpdateSensorInfoL, Orientation channel found");
    }

    iAccSensorChannel = CSensrvChannel::NewL(channelInfoList[ 0 ]);
    CleanupStack::PopAndDestroy(&channelInfoList);
    CleanupStack::PopAndDestroy(channelFinder);

    TRAPD(channelerror, iAccSensorChannel->OpenChannelL());
    if (channelerror!=KErrNone)
    {
        User::Leave( channelerror );
    }
    
    // start listening
    iAccSensorChannel->StartDataListeningL(this, //this object is data listener for this channel
                                            1, //aDesiredCount is one, i.e. each orientation change is notified separately
                                            1, //aMaximumCount is one, i.e. object count in receiving data buffer is one
                                            0);//buffering period is not used
    iLastImageOrientation = ECamOrientation0;
    DEBUG("CMMACameraPlayer::UpdateSensorInfoL -");
}

void CMMACameraPlayer::DataReceived(CSensrvChannel& aChannel,
                                    TInt /*aCount*/, 
                                    TInt /*aDataLost*/)
{
    DEBUG_INT("CMMACameraPlayer::DataReceived aChannel %d", aChannel.GetChannelInfo().iChannelType);

    if (KSensrvChannelTypeIdOrientationData == aChannel.GetChannelInfo().iChannelType)
    {
        TSensrvOrientationData orientationData;
        TPckg<TSensrvOrientationData> orientationPackage(orientationData);
        aChannel.GetData(orientationPackage);

        iImageOrientation =
                MapSensorOrientatio2CamOrientation(orientationData.iDeviceOrientation, 
                                                   iLastImageOrientation,
                                                   iActiveCameraIndex);

        TRAPD(error, iCustomInterfaceOrientation->SetOrientationL(Map2CameraOrientation(iImageOrientation)));
        if (KErrNone == error)
        {
            DEBUG_INT("CMMACameraPlayer::DataReceived SetOrientationL error = %d", error);
        }

        iLastImageOrientation = iImageOrientation;
    }
}

void CMMACameraPlayer::DataError(CSensrvChannel& /*aChannel*/,
                                 TSensrvErrorSeverity aError)
{
    DEBUG_INT("MMA::CMMACameraPlayer::DataError: aError = %d", aError);
    if ( ESensrvErrorSeverityFatal == aError ) 
    {
        // Delete sensor api object
        delete iAccSensorChannel;
        iAccSensorChannel = NULL;

        // Set orientation back to default if not already there.
        iImageOrientation   = ECamOrientation0;
    }
}

void CMMACameraPlayer::GetDataListenerInterfaceL(TUid /*aInterfaceUid*/,
                                                 TAny*& /*aInterface*/)
{
}

TCamImageOrientation CMMACameraPlayer::MapSensorOrientatio2CamOrientation( 
    const TSensrvOrientationData::TSensrvDeviceOrientation& aSensorOrientation, 
    TCamImageOrientation aLastImageOrientation,
    TCamActiveCamera aActiveCameraIndex)
{
    DEBUG_INT("MMA::CMMACameraPlayer::MapSensorOrientatio2CamOrientation: aSensorOrientation = %d", 
    aSensorOrientation);

    TCamImageOrientation cameraOrientation( ECamOrientation0 );

    // Primary camera rotation
    if (aActiveCameraIndex == ECamActiveCameraPrimary) 
    {
      switch(aSensorOrientation)
      {
        case TSensrvOrientationData::EOrientationDisplayUpwards:
            DEBUG("CMMACameraPlayer::MapSensorOrientatio2CamOrientation EOrientationDisplayUpwards");
            // If coming from upside down portrait...
            if ( ECamOrientation270 == aLastImageOrientation )
            {
                // Switch from upside down portrait to normal portrait.") )
                cameraOrientation = ECamOrientation90; // Set normal portrait
            }
            // If coming from upside down landscape...
            else if ( ECamOrientation180 == aLastImageOrientation )
            {
                // Switch from upside down landscape to normal landscape...") )
                cameraOrientation = ECamOrientation0; // Set normal lanscape
            }
            // If neither one, keep the current image orientation.
            else
            {
                cameraOrientation = ECamOrientationIgnore;
            }
            break;

        case TSensrvOrientationData::EOrientationDisplayDownwards:
            DEBUG("CMMACameraPlayer::MapSensorOrientatio2CamOrientation EOrientationDisplayDownwards");
            // Camera is pointing up now. Check if it was upside down previously.
            // If coming from upside down portrait...
            if ( ECamOrientation270 == aLastImageOrientation )
            {
                // Switch from upside down portrait to normal portrait...") )
                cameraOrientation = ECamOrientation90; // Set normal portrait
            }
            // If coming from upside down landscape...
            else if ( ECamOrientation180 == aLastImageOrientation )
            {
                // Switch from upside down landscape to normal landscape...") )
                cameraOrientation = ECamOrientation0; // Set normal lanscape
            }
            // If neither one, keep the current image orientation.
            else
            {
                cameraOrientation = ECamOrientationIgnore;
            }
        	  break;
        case TSensrvOrientationData::EOrientationDisplayRightUp:
            DEBUG("CMMACameraPlayer::MapSensorOrientatio2CamOrientation EOrientationDisplayRightUp");
        case TSensrvOrientationData::EOrientationUndefined:
            DEBUG("CMMACameraPlayer::MapSensorOrientatio2CamOrientation EOrientationUndefined");
            cameraOrientation = ECamOrientation0;     
            break;
        case TSensrvOrientationData::EOrientationDisplayUp:
            DEBUG("CMMACameraPlayer::MapSensorOrientatio2CamOrientation EOrientationDisplayUp");
            cameraOrientation = ECamOrientation90;
            break;
        case TSensrvOrientationData::EOrientationDisplayLeftUp:
            DEBUG("CMMACameraPlayer::MapSensorOrientatio2CamOrientation EOrientationDisplayLeftUp");
            cameraOrientation = ECamOrientation180;
            break;
        case TSensrvOrientationData::EOrientationDisplayDown:
            DEBUG("CMMACameraPlayer::MapSensorOrientatio2CamOrientation EOrientationDisplayDown");
            cameraOrientation = ECamOrientation270;
            break;
        default:
            DEBUG("Camera <> Unexpected orientation value");
            break;
        }
      }
    // Secondary camera rotations
    else if (aActiveCameraIndex == ECamActiveCameraSecondary)
    {
      if (aSensorOrientation == TSensrvOrientationData::EOrientationDisplayUp) // Portrait
      {
        DEBUG("CMMACameraPlayer::MapSensorOrientatio2CamOrientation Secondary-camera EOrientationDisplayUp");
        cameraOrientation = ECamOrientation180; 
      }
      else if (aSensorOrientation == TSensrvOrientationData::EOrientationDisplayDown)  // Upside down portrait
      {
        DEBUG("CMMACameraPlayer::MapSensorOrientatio2CamOrientation Secondary-camera EOrientationDisplayDown");
        cameraOrientation = ECamOrientation180;
      }
      else
      {
        DEBUG("CMMACameraPlayer::MapSensorOrientatio2CamOrientation Secondary-camera else");
        cameraOrientation = ECamOrientation0;
      }
    }

    return cameraOrientation;
}

MCameraOrientation::TOrientation CMMACameraPlayer::Map2CameraOrientation( 
    const TCamImageOrientation& aSettingOrientation )
{
    DEBUG("CMMACameraPlayer::Map2CameraOrientation +");
    switch(aSettingOrientation)
    {
    case ECamOrientation90:
      DEBUG("CMMACameraPlayer::Map2CameraOrientation ECamOrientation90");
      return MCameraOrientation::EOrientation90;
    case ECamOrientation180:
      DEBUG("CMMACameraPlayer::Map2CameraOrientation ECamOrientation180");
      return MCameraOrientation::EOrientation180;
    case ECamOrientation270:
      DEBUG("CMMACameraPlayer::Map2CameraOrientation ECamOrientation270");
      return MCameraOrientation::EOrientation270;
    case ECamOrientation0:    // <<fall through>>
    default:                  
      DEBUG("CMMACameraPlayer::Map2CameraOrientation ECamOrientation0 or default");
      return MCameraOrientation::EOrientation0;
    }
}

//  END OF FILE
