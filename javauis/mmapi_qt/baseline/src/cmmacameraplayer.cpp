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
#include <logger.h>
#include <fbs.h>
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

CMMACameraPlayer* CMMACameraPlayer::NewLC(TInt aCameraIndex)
{
    CMMACameraPlayer* self = new(ELeave) CMMACameraPlayer;
    CleanupStack::PushL(self);
    self->ConstructL(aCameraIndex);
    return self;
}

CMMACameraPlayer::~CMMACameraPlayer()
{
    LOG(EJavaMMAPI, EInfo, "CMMACameraPlayer::~CMMACameraPlayer");

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
        // iDisplay->UIGetCallback(
        //     *iWindow, CMMACameraWindow::EDestroyWindow);
    }
    else
    {
        delete iWindow;
    }
    iWindow = NULL;
}


CMMACameraPlayer::CMMACameraPlayer():
        iDisplay(NULL),
        iSourceSizeIndex(KErrNotFound),
        iStartTime(KErrNotFound)
{
    iStopViewFinder = ETrue;
}


void CMMACameraPlayer::ConstructL(TInt aCameraIndex)
{
    CMMAPlayer::ConstructL();

    if (aCameraIndex >= CCamera::CamerasAvailable())
    {
        // image capture is not supported
        User::Leave(KErrNotFound);
    }

    iCamera = CCamera::NewL(*this, aCameraIndex);

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
    LOG(EJavaMMAPI, EInfo, "CMMACameraPlayer::ResolveViewFinderSizeL");

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
    LOG(EJavaMMAPI, EInfo, "CMMACameraPlayer::ResolveScreenSizeL");

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

    LOG1(EJavaMMAPI, EInfo, "MMA::CMMACameraPlayer::ResolveCaptureSizes: aFormat = 0x%x", aFormat);

    // go through all supported sizes.
    // Notice: Capture sizes are assumed to be in order from smaller to larger sizes
    for (TInt i = 0; i < aNumImageSizesSupported; i++)
    {
        iCamera->EnumerateCaptureSizes(tmpSize,
                                       i,
                                       aFormat);

        LOG1(EJavaMMAPI, EInfo, "MMA::CMMACameraPlayer::ResolveCaptureSizes: tmpSize.iWidth = %d", tmpSize.iWidth);
        LOG1(EJavaMMAPI, EInfo, "MMA::CMMACameraPlayer::ResolveCaptureSizes: tmpSize.iHeight = %d", tmpSize.iHeight);

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

    LOG1(EJavaMMAPI, EInfo, "MMA::CMMACameraPlayer::ResolveCaptureSizes: sourceSizeIndex = %d", sourceSizeIndex);
    LOG1(EJavaMMAPI, EInfo, "MMA::CMMACameraPlayer::ResolveCaptureSizes: largestSizeIndex = %d", largestSizeIndex);

    aSourceSize = sourceSize;
    aSourceIndex = sourceSizeIndex;
    aLargestIndex = largestSizeIndex;
}

void CMMACameraPlayer::StartL()
{
    LOG1(EJavaMMAPI, EInfo, "CMMACameraPlayer:StartL iState %d", iState);

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

        // inform java side
        PostLongEvent(CMMAPlayerEvent::EStarted, time);
    }
    PostActionCompletedStart();
    PostActionCompleted(KErrNone);   // java start return
}

void CMMACameraPlayer::StopL(TBool aPostEvent)
{
    LOG1(EJavaMMAPI, EInfo, "CMMACameraPlayer::StopL", iState);
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
    LOG(EJavaMMAPI, EInfo, "MMA::CMMACameraPlayer::PrefetchL");
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
    LOG(EJavaMMAPI, EInfo, "MMA::CMMACameraPlayer::SetMediaTimeL ");
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
    LOG(EJavaMMAPI, EInfo, "MMA::CMMACameraPlayer::CloseL()");

    // cancel all activity
    iCamera->CancelCaptureImage();

    // Stop and release UI Camera instance
    iWindow->SetDisplay(NULL);

    // we don't need reserve camera anymore
    iCamera->Release();

    CMMAPlayer::CloseL();
}

const TDesC& CMMACameraPlayer::Type()
{
    return KMMACameraPlayer;
}

// MCameraObserver
void CMMACameraPlayer::ReserveComplete(TInt aError)
{
    ELOG1(EJavaMMAPI, "MMA::CMMACameraPlayer::ReserveComplete %d", aError);
    if (aError == KErrNone)
    {
        // camera will notify completion with PowerOnComplete method.
        iCamera->PowerOn();
    }
    else
    {
        // release lock and store error. State doesn't change.
        iRealizeWait->iError = aError;
        iRealizeWait->AsyncStop();
    }

}

void CMMACameraPlayer::PowerOnComplete(TInt aError)
{
    ELOG1(EJavaMMAPI, "MMA::CMMACameraPlayer::PowerOnComplete %d", aError);

    TSize viewFinderSize;
    TInt error = aError;

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
        }
    }

    iRealizeWait->iError = error;
    iRealizeWait->AsyncStop();
}

void CMMACameraPlayer::ViewFinderFrameReady(CFbsBitmap& /*aFrame*/)
{
    // Empty implementation of an interface method.
    // DirectViewFinder is used
    // instead of BitmapViewFinder
}

void CMMACameraPlayer::ImageReady(CFbsBitmap* aBitmap,
                                  HBufC8* aData,
                                  TInt aError)
{
    ELOG1(EJavaMMAPI, "MMA::CMMACameraPlayer::ImageReady %d", aError);

    // This method should never be called,
    // unless we are taking snapshot
    __ASSERT_DEBUG(iSnapshotStatus, User::Invariant());

    __ASSERT_DEBUG(!iSnapshotBitmap, User::Invariant());
    __ASSERT_DEBUG(!iSnapshotEncoded, User::Invariant());

    if (aError == KErrNone)
    {
        // this class has ownership of the bitmap until
        // snapshot bitmap is got from this class.
        iSnapshotBitmap = aBitmap;
        iSnapshotEncoded = aData;
    }

    // notify the caller, error code or KErrNone
    User::RequestComplete(iSnapshotStatus, aError);
    iWindow->SetStarted(ETrue);
    // Status is not needed anymore
    // and this class don't own the status.
    iSnapshotStatus = NULL;
}

void CMMACameraPlayer::FrameBufferReady(MFrameBuffer* /*aFrameBuffer*/,
                                        TInt /*aError*/)
{
    LOG(EJavaMMAPI, EInfo, "MMA::CMMACameraPlayer::FrameBufferReady");
    // this callback will never be called
    // Asserted in debug build to be sure.
    __ASSERT_DEBUG(EFalse, User::Invariant());
}

void CMMACameraPlayer::SetDisplayL(MMMADisplay* aDisplay)
{
    LOG(EJavaMMAPI, EInfo, "MMA::CMMACameraPlayer::SetDisplay");

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

    // changing status to pending
    *iSnapshotStatus = KRequestPending;

    // Source size not set in the beginning
    TSize sourceSize;

    // Use default if size is not specified.
    TInt sourceSizeIndex = iSourceSizeIndex;

    // Largest image size index
    TInt largestSizeIndex = 0;

    // Get camera characteristics
    TCameraInfo cameraInfo;
    iCamera->CameraInfo(cameraInfo);

    // Set default snapshot encoding type
    TEncoding encoding = EBitmap;

    // Set default image capture format
    CCamera::TFormat format = CCamera::EFormatFbsBitmapColor16M;

    LOG1(EJavaMMAPI, EInfo, "MMA::CMMACameraPlayer::TakeSnapshotL: cameraInfo.iImageFormatsSupported = 0x%x", cameraInfo.iImageFormatsSupported);
    LOG1(EJavaMMAPI, EInfo, "MMA::CMMACameraPlayer::TakeSnapshotL: cameraInfo.iNumImageSizesSupported = %d", cameraInfo.iNumImageSizesSupported);

    // Check if size was set in Java
    if (aSize.iWidth != KErrNotFound &&
            aSize.iHeight != KErrNotFound)
    {
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

    // start capture, ImageReady will be called when ready
    iWindow->SetStarted(EFalse);

    iCamera->CaptureImage();

    LOG1(EJavaMMAPI, EInfo, "MMA::CMMACameraPlayer::TakeSnapshotL: format = 0x%x", format);
    LOG1(EJavaMMAPI, EInfo, "MMA::CMMACameraPlayer::TakeSnapshotL: sourceSizeIndex = %d", sourceSizeIndex);
    LOG1(EJavaMMAPI, EInfo, "MMA::CMMACameraPlayer::TakeSnapshotL: encoding = %d", encoding);

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

//  END OF FILE
