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


//  INCLUDE FILES
#include <jdebug.h>

// For Image Handling Library (IHL)
#include "IHLImageFactory.h"
#include "MIHLFileImage.h"
#include "IHLViewerFactory.h"
#include "MIHLImageViewer.h"
#include "MIHLBitmap.h"
#include "IHLBitmapUtil.h"

// MMAPI includes
#include "mmmadisplay.h"

// Class header
#include "cmmaanimationplayer.h"
#include "cmmaanimationwindow.h"

namespace
{
const TInt64 KMMATimeUnknown = -1;
_LIT(KMMAAnimationContentType, "image/gif");

// Approximated minimum showing time of each frame is 0.12s
// this value basically depends on how fast device can load frames
// it is not quaranteed anyway that the media time is equal to
// clock time, so this needed to be only somewhat close

_LIT(KVideoControlName, "VideoControl");
}

CMMAAnimationPlayer* CMMAAnimationPlayer::NewLC()
{
    DEBUG("MMA::CMMAAnimationPlayer::NewLC");
    CMMAAnimationPlayer* self = new(ELeave) CMMAAnimationPlayer();
    CleanupStack::PushL(self);
    self->ConstructL();
    return self;
}

CMMAAnimationPlayer* CMMAAnimationPlayer::NewLC(const TDesC& aFileName)
{
    CMMAAnimationPlayer* self = NewLC();
    self->iFileName = aFileName.AllocL();
    return self;
}

CMMAAnimationPlayer::~CMMAAnimationPlayer()
{
    DEBUG("MMA::CMMAAnimationPlayer::~CMMAAnimationPlayer +");
    if (iViewer && iViewer->IsPlaying())
    {
        iViewer->Stop();
    }
    delete iWindow;
    delete iSnapshotBitmap;
    delete iViewer;
    // viewer has reference to iImage,
    // thus deletion order is important.
    delete iBitmap;
    delete iImage;

    delete iFileName;

    iFSession.Close();
    DEBUG("MMA::CMMAAnimationPlayer::~CMMAAnimationPlayer -");
}

CMMAAnimationPlayer::CMMAAnimationPlayer()
        : iFrameCount(0), iMediaTime(KMMATimeUnknown), iEndReached(EFalse),
        iSendEndOfMediaOnNextFrame(EFalse)
{
}

void CMMAAnimationPlayer::ConstructL()
{
    DEBUG("MMA::CMMAAnimationPlayer::ConstructL +");
    CMMAPlayer::ConstructL();
    HBufC* contentType = KMMAAnimationContentType().AllocL();
    SetContentType(contentType);

    // Connect to file session, needed also when playing from data
    User::LeaveIfError(iFSession.Connect());

    // File session must be share protected for IHL
    User::LeaveIfError(iFSession.ShareProtected());

    DEBUG("MMA::CMMAAnimationPlayer::ConstructL -");
}

void CMMAAnimationPlayer::SetPlayerListenerObjectL(
    jobject aListenerObject,
    JNIEnv* aJni,
    MMMAEventPoster* aEventPoster)
{
    CMMAPlayer::SetPlayerListenerObjectL(aListenerObject,
                                         aJni,
                                         aEventPoster);

    // this method must be called only ones
    __ASSERT_DEBUG(!iWindow, User::Invariant());

    // create window for animationplayer
    // event poster is always CMMAEventSource type.
    iWindow = CMMAAnimationWindow::NewL(
                  static_cast< CMMAEventSource* >(iEventPoster));
}

void CMMAAnimationPlayer::RealizeL()
{
    DEBUG("MMA::CMMAAnimationPlayer::RealizeL");
    // For file locator file must be prefetched here because
    // FramePositioningControl must know duration of media
    // in realized state
    if (iFileName)
    {
        TRAPD(err, PrefetchFileL());
        if (err != KErrNone)
        {
            User::Leave(err);
        }
    }
    CMMAPlayer::RealizeL();
}

void CMMAAnimationPlayer::PrefetchL()
{
    DEBUG("MMA::CMMAAnimationPlayer::PrefetchL +");
    __ASSERT_DEBUG((iSourceStreams.Count() > 0) || iFileName, User::Invariant());

    if (iFileName)
    {
        // file data is already fetched in when realizing

        // If initDisplayMode was called before prefetch,
        // then the display must notified about source size.
        if (iDisplay)
        {
            iDisplay->SourceSizeChanged(iSourceSize);
            NotifyWithStringEvent(CMMAPlayerEvent::ESizeChanged, KVideoControlName);
        }

        ChangeState(EPrefetched);
        PostActionCompleted(KErrNone);
    }
    else
    {
        // Using TDes -- load the whole animation
        iSourceStreams[ 0 ]->ReadAllL();
    }

    // CMMASourceStream will notify with ReadCompleted
    DEBUG("MMA::CMMAAnimationPlayer::PrefetchL -");
}

void CMMAAnimationPlayer::StartL()
{
    DEBUG("MMA::CMMAAnimationPlayer::StartL +");

    // If end of media has been reached, then
    // start from beginning
    if (iEndReached)
    {
        iEndReached = EFalse;
        iViewer->SetAnimationFrame(0);
        iMediaTime = 0;
    }
    PostLongEvent(CMMAPlayerEvent::EStarted, iMediaTime);

    // process current frame
    ProcessCurrentFrameL();

    // progress to next frame (start playback) only if rate is not zero
    if (iCurrentRate > 0)
    {
        iViewer->Play();
    }
    ChangeState(EStarted);
    PostActionCompleted(KErrNone);   // java start return

    DEBUG("MMA::CMMAAnimationPlayer::StartL -");
}

void CMMAAnimationPlayer::ProcessCurrentFrameL()
{
    if (iSendEndOfMediaOnNextFrame)
    {
        iSendEndOfMediaOnNextFrame = EFalse;
        // we are reached the end
        if (!iRepeatForever)
        {
            iRepeatCount++;
            if (iRepeatCount >= iRepeatNumberOfTimes)
            {
                DEBUG("CMMAAnimationPlayer:ProcessCurrentFrameL: Reached repeat count, Stopping");
                // end looping, do not send stopped event
                StopL(EFalse);
                iViewer->SetAnimationFrame(iFrameCount - 1);
                SetLoopCount(iRepeatNumberOfTimes);   // reset loop count

                // Signal that end of media has been reached so on next
                // start playback will be started from beginning. This is needed
                // because if user sets media time to end of media, then start
                // should not start from beginning but just deliver end of media.
                // After that, the next start should start from beginning.
                iEndReached = ETrue;
            }
        }
        PostLongEvent(CMMAPlayerEvent::EEndOfMedia, iMediaTime);
        DEBUG("CMMAAnimationPlayer:ProcessCurrentFrameL: sent END_OF_MEDIA");

        // Prevents this frame from being viewed if playback has terminated
        // (e.g. not looping)
        if (iEndReached)
        {
            return;
        }
    }

    // draw current frame to display if we have it
    if (iDisplay)
    {
        const CFbsBitmap& bitmap = iBitmap->Bitmap();
        iDisplay->DrawFrameL(&bitmap);
    }

    TInt currentFrame = iViewer->AnimationFrame();
    if (currentFrame == 0)
    {
        DEBUG("CMMAAnimationPlayer:ProcessCurrentFrameL: Reset mediatime");
        // reset media time when looping
        iMediaTime = 0;
    }
    iMediaTime += FrameDuration(currentFrame).Int();

    // Media time has gone over duration if user has
    // set media time explicitely to duration.
    if (iMediaTime > iDuration)
    {
        iMediaTime = iDuration;
    }

    if (currentFrame == (iFrameCount - 1))
    {
        // End has been reached, so EndOfMedia is sent when
        // duration of last frame has passed
        iSendEndOfMediaOnNextFrame = ETrue;
    }

    // inform observer
    if (iAnimationObserver)
    {
        iAnimationObserver->AnimationAdvancedL(iViewer->AnimationFrame(), iMediaTime);
    }
}

void CMMAAnimationPlayer::StopL(TBool aPostEvent)
{
    DEBUG("MMA::CMMAAnimationPlayer::StopL +");
    iViewer->Stop();
    // adjust mediatime
    if (aPostEvent)
    {
        PostLongEvent(CMMAPlayerEvent::EStopped, iMediaTime);
    }
    ChangeState(EPrefetched);
    DEBUG("MMA::CMMAAnimationPlayer::StopL -");
}

void CMMAAnimationPlayer::DeallocateL()
{
    DEBUG("MMA::CMMAAnimationPlayer::DeallocateL +");
    // If player is in starte state when deallocate is called,
    // player is stopped from java side -> state is changed to
    // prefetched.
    if (iViewer)
    {
        if (iViewer->IsPlaying())
            iViewer->Stop();

        delete iViewer;
        iViewer = NULL;
    }

    if (iState == EPrefetched)
    {
        ResetSourceStreams();
        iEndReached = EFalse;
        iSendEndOfMediaOnNextFrame = EFalse;
        ChangeState(ERealized);
    }
    DEBUG("MMA::CMMAAnimationPlayer::DeallocateL -");
}

void CMMAAnimationPlayer::GetDuration(TInt64* aDuration)
{
    *aDuration = iDuration;
}

TInt CMMAAnimationPlayer::FindFrame(TInt64 aTime)
{
    __ASSERT_DEBUG(iImage, User::Invariant());

    // if we are out of bounds
    if (aTime > iDuration)
    {
        return KErrNotFound;
    }

    TInt64 time = 0;
    TInt fIndex = 0;
    while ((time < aTime) && (fIndex < iFrameCount))
    {
        time += FrameDuration(fIndex++).Int();
    }

    // adjust to previous frame
    if (fIndex > 0)
    {
        fIndex--;
    }

    return fIndex;
}

TInt64 CMMAAnimationPlayer::MediaTimeForFrame(TInt aFrameIndex)
{
    __ASSERT_DEBUG((aFrameIndex <= iFrameCount) && (aFrameIndex >= 0),
                   User::Invariant());

    TInt64 time = 0;
    for (TInt fIndex = 0; fIndex < aFrameIndex; fIndex++)
    {
        time += FrameDuration(fIndex).Int();
    }
    return time;
}

TTimeIntervalMicroSeconds32 CMMAAnimationPlayer::FrameDuration(TInt aFrameIndex)
{
    __ASSERT_DEBUG(iImage, User::Invariant());
    TTimeIntervalMicroSeconds32 fDur = iImage->AnimationFrameDelay(aFrameIndex);
    const TTimeIntervalMicroSeconds32 KMMAMinimumFrameTime = 120000;

    if (fDur < KMMAMinimumFrameTime)
    {
        fDur = KMMAMinimumFrameTime;
    }
    return fDur;
}

void CMMAAnimationPlayer::SetMediaTimeL(TInt64* aTime)
{
    if (!iImage && !iViewer)
    {
        // not yet prefetched
        *aTime = KErrNotSupported;
    }
    else
    {
        // Media time of last frame is not the same as duration of
        // media, so if media time of duration is requested, then it must
        // be given out altough media time of last frame is lower than that.
        if (*aTime >= iDuration)
        {
            User::LeaveIfError(iViewer->SetAnimationFrame(iFrameCount - 1));
            iMediaTime = iDuration;
        }
        else
        {
            TInt frame = FindFrame(*aTime);
            User::LeaveIfError(iViewer->SetAnimationFrame(frame));
            iMediaTime = MediaTimeForFrame(frame);
        }
        *aTime = iMediaTime;
        iEndReached = EFalse;
        iSendEndOfMediaOnNextFrame = EFalse;
    }

}

void CMMAAnimationPlayer::GetMediaTime(TInt64* aMediaTime)
{
    *aMediaTime = iMediaTime;
}

const TDesC& CMMAAnimationPlayer::Type()
{
    return KMMAVideoPlayer;
}

void CMMAAnimationPlayer::ReadCompletedL(TInt aStatus, const TDesC8& aData)
{
    if (aStatus < KErrNone)
    {
        PostActionCompleted(aStatus);
    }
    else
    {
        TRAPD(err, PrefetchDataL(aData));
        if (err == KErrNone)
        {
            ChangeState(EPrefetched);
        }
        PostActionCompleted(err);
    }
}

void CMMAAnimationPlayer::PrefetchFileL()
{
    iImage = IHLImageFactory::OpenFileImageL(iFSession, *iFileName);
    PrepareViewerL();
}

void CMMAAnimationPlayer::PrefetchDataL(const TDesC8& aData)
{
    DEBUG_INT("MMA::CMMAAnimationPlayer::PrefetchDataL aData size %d",
              aData.Size());

    // Create source image from data
    iImage = IHLImageFactory::OpenBufferedFileImageL(iFSession, aData);
    PrepareViewerL();
}

TBool CMMAAnimationPlayer::IsFilePlayer()
{
    if (iFileName != NULL)
    {
        return ETrue;
    }
    return EFalse;
}

void CMMAAnimationPlayer::PrepareViewerL()
{
    // Non-animated gifs are not supported
    if (!(iImage->IsAnimation()))
    {
        User::Leave(KErrNotSupported);
    }

    // Store image dimensions
    iSourceSize = iImage->Size();

    // Create destination bitmap
    iBitmap = IHLBitmap::CreateL();
    User::LeaveIfError(iBitmap->Create(iSourceSize, iImage->DisplayMode()));

    // Create image viewer
    iViewer = IHLViewerFactory::CreateImageViewerL(
                  iSourceSize,
                  *iImage, // source
                  *iBitmap, // destination
                  *this);  // reference to MIHLViewerObserver

    // Set viewer for window
    iWindow->SetViewer(iViewer);

    // Store animation frame count locally
    iFrameCount = iViewer->AnimationFrameCount();

    // calculate duration
    iDuration = MediaTimeForFrame(iFrameCount);

    // set media time to begin
    iMediaTime = 0;

    // If init has been already done
    if (iDisplay)
    {
        iDisplay->SourceSizeChanged(iSourceSize);
        NotifyWithStringEvent(CMMAPlayerEvent::ESizeChanged, KVideoControlName);
    }
}

MIHLImageViewer* CMMAAnimationPlayer::Viewer()
{
    return iViewer;
}

void CMMAAnimationPlayer::SetAnimationObserver(MMMAAnimationObserver* aAnimationObserver)
{
    iAnimationObserver = aAnimationObserver;
}

TInt CMMAAnimationPlayer::SetRateL(TInt aRate)
{
    DEBUG("MMA:CMMAAnimationPlayer::SetRateL");
    if ((iState == EStarted) && (iCurrentRate != aRate))
    {
        if (aRate <= 0)
        {
            iViewer->Stop();
        }
        else
        {
            iViewer->Play();
        }
    }
    iCurrentRate = aRate;
    return iCurrentRate;
}

TInt CMMAAnimationPlayer::RateL()
{
    DEBUG("MMA:CMMAAnimationPlayer::RateL");
    return iCurrentRate;
}

void CMMAAnimationPlayer::SetDisplayL(MMMADisplay* aDisplay)
{
    // now it is ready to draw
    iDisplay = aDisplay;
    iDisplay->SetWindowL(iWindow);

    // if state < prefeteched then we dont know actual source size yet
    // and it will be set after prefetch
    if (iState >= EPrefetched ||
            (iFileName && iState == ERealized))
    {
        iDisplay->SourceSizeChanged(iSourceSize);
        NotifyWithStringEvent(CMMAPlayerEvent::ESizeChanged, KVideoControlName);
    }
}

TSize CMMAAnimationPlayer::SourceSize()
{
    return iSourceSize;
}

void CMMAAnimationPlayer::NotifyWithStringEvent(
    CMMAPlayerEvent::TEventType aEventType,
    const TDesC& aStringEventData)
{
    PostStringEvent(aEventType, aStringEventData);
}

MMMASnapshot* CMMAAnimationPlayer::SnapshoterL()
{
    return this;
}

MMMASnapshot::TEncoding CMMAAnimationPlayer::TakeSnapshotL(TRequestStatus* aStatus,
        const TSize& /*aSize*/,
        const CMMAImageSettings& /*aSettings*/)
{
    if (iBitmap)
    {
        // Bitmap has to be copied to get ownership of the bitmap instance.
        iSnapshotBitmap = IHLBitmapUtil::CopyBitmapL(iBitmap->Bitmap());
    }
    else
    {
        // When content comes from a stream, iBitmap is not available
        // until prefetched state is entered. In this case an empty bitmap
        // is returned instead.
        iSnapshotBitmap = new(ELeave) CFbsBitmap();
    }
    // notify the caller, error code or KErrNone
    User::RequestComplete(aStatus, KErrNone);

    // Return raw bitmap encoding and thus SnapshotEncoded() should not
    // get called later on.
    return EBitmap;
}

CFbsBitmap* CMMAAnimationPlayer::SnapshotBitmap()
{
    CFbsBitmap* bitmap = iSnapshotBitmap;
    // ownership is transferred to caller
    iSnapshotBitmap = NULL;
    return bitmap;
}

HBufC8* CMMAAnimationPlayer::SnapshotEncoded()
{
    // This method should never be called.
    // Asserted in debug build to be sure.
    __ASSERT_DEBUG(EFalse, User::Invariant());

    return NULL;
}

void CMMAAnimationPlayer::ViewerBitmapChangedL()
{
    if (iState == EStarted)
    {
        ProcessCurrentFrameL();
    }
}

void CMMAAnimationPlayer::ViewerError(TInt /*aError*/)
{
    // Not implemented currently because
    // not implemented by IHL either.
}

//  END OF FILE
