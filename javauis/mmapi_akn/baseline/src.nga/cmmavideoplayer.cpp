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

//  INCLUDE FILES
#include <mmf/server/mmffile.h>
#include <jdebug.h>

#include "cmmavideoplayer.h"
#include "mmmadisplay.h"
#include "cmmaeventsource.h"
#include "cmmasurfacewindow.h"

// CONSTANTS
_LIT(KVideoControlName, "VideoControl");

CMMAVideoPlayer* CMMAVideoPlayer::NewLC(
    CMMAMMFResolver* aResolver)
{
    CMMAVideoPlayer* self =
        new(ELeave) CMMAVideoPlayer(aResolver);
    CleanupStack::PushL(self);
    self->ConstructL();
    return self;
}

CMMAVideoPlayer::~CMMAVideoPlayer()
{
    DEBUG("MMA::CMMAVideoPlayer::~CMMAVideoPlayer");

    // Window is not able to send any
    // callback requests to UI from now.
    if (iSurfaceWindow)
    {
        iSurfaceWindow->SetDisplay(NULL);
    }

    if (iDisplay && iDisplay->HasContainer())
    {
        // Window will delete itself
        // after all pending events are processed
        // (lazy delete)
        iDisplay->UIGetCallback(
            *iSurfaceWindow, CMMASurfaceWindow::EDestroyWindow);
    }
    else
    {
        delete iSurfaceWindow;
    }

    if (iDisplay)
    {
        TRAPD(err, iDisplay->SetWindowL(NULL));
        if (err != KErrNone)
        {
            __ASSERT_DEBUG(EFalse, User::Invariant());
        }
    }

    delete iEmptySnapshotImage;
    delete iActiveSchedulerWait;
}

CMMAVideoPlayer::CMMAVideoPlayer(
    CMMAMMFResolver* aResolver):
        CMMAAudioPlayer(aResolver),
        iVideoControllerCustomCommands(iController),
        iVideoPlayControllerCustomCommands(iController),
        iVideoPlaySurfaceSupportCustomCommands(iController)
{
    iMMASurface.iPrevSurfaceAvailable = EFalse;
}

void CMMAVideoPlayer::ConstructL()
{
    CMMAAudioPlayer::ConstructL();
    iActiveSchedulerWait = new(ELeave)CActiveSchedulerWait;
}

EXPORT_C void CMMAVideoPlayer::SetPlayerListenerObjectL(jobject aListenerObject,
        JNIEnv* aJni,
        MMMAEventPoster* aEventPoster)
{
    CMMAPlayer::SetPlayerListenerObjectL(aListenerObject,
                                         aJni,
                                         aEventPoster);

    // this method must be called only ones
    __ASSERT_DEBUG(!iSurfaceWindow, User::Invariant());

    // create window for videoplayer
    // event poster is always CMMAEventSource type.

    iSurfaceWindow = CMMASurfaceWindow::NewL(
                         static_cast< CMMAEventSource* >(iEventPoster),
                         this);
}

EXPORT_C void CMMAVideoPlayer::SetDisplayL(MMMADisplay* aDisplay)
{
    // now it is ready to draw
    iDisplay = aDisplay;

    iSurfaceWindow->SetDisplay(aDisplay);

    iDisplay->SetWindowL(iSurfaceWindow);

    // if state < prefeteched then we dont know actual source size yet
    // and it will be set after prefetch
    if (iState >= EPrefetched)
    {
        SourceSizeChanged();
    }
}
void CMMAVideoPlayer::RealizeL()
{
    DEBUG("CMMAVideoPlayer::RealizeL");
    // DataSource must have at least 1 stream or
    // we must have file to play
    if ((iSourceStreams.Count() == 0) && !iFileName)
    {
        User::Leave(KErrNotEnoughStreams);
    }

    // If file locator is used, then file is prefetched
    // in realized state so that FramePositioningControl
    // can acquire frame count in REALIZED state
    if (iFileName)
    {
        PrefetchFileL();
        if (!iActiveSchedulerWait->IsStarted())
        {
            iActiveSchedulerWait->Start();
        }
        // If the player has not changed state during wait,
        // Then there is an error condition.
        if (iState != ERealized)
        {
            User::Leave(KErrNotSupported);
        }
    }
    else
    {
        CMMAPlayer::RealizeL();
    }
}

void CMMAVideoPlayer::PrefetchL()
{
    DEBUG("CMMAVideoPlayer::PrefetchL");
    if (iFileName)
    {
        // File has already been prefetched when realizing

        // If initDisplayMode was called before prefetch,
        // then the display must notified about source size.
        if (iDisplay)
        {
            SourceSizeChanged();
        }

        ChangeState(EPrefetched);
        PostActionCompleted(KErrNone);
    }
    else
    {
        // Using TDes -- load the whole video
        iSourceStreams[ 0 ]->ReadAllL();
    }
    // CMMASourceStream will notify with ReadCompleted
}

EXPORT_C void CMMAVideoPlayer::ReadCompletedL(TInt aStatus, const TDesC8& aData)
{
    DEBUG_INT("CMMAVideoPlayer::ReadCompletedL: status = %d", aStatus);
    if (aStatus < KErrNone)
    {
        PostActionCompleted(aStatus);
    }
    else
    {
        TRAPD(err, PrefetchDataL(aData));
        if (err != KErrNone)
        {
            PostActionCompleted(err);
        }
        // action completed event will be delivered from handleEvent
    }
}

void CMMAVideoPlayer::HandleEvent(const TMMFEvent& aEvent)
{
    DEBUG_INT("MMA:CMMAVideoPlayer::HandleEvent %d", aEvent.iEventType.iUid);

    // event KMMFEventCategoryPlaybackComplete is handled by both Video
    // and Audio players. first it should be handled by Video player
    if (aEvent.iEventType == KMMFEventCategoryPlaybackComplete)
    {
        iSurfaceWindow->RemoveSurface();
    }

    // KNotCompleteVideoError can be notified when video is not complete
    // ( missing sound ) but still can be played. Because
    // CMMAAudioPlayer::HandleEvent fails with all negative error codes,
    // do not call it with KNotCompleteVideoError error when preparing.
    if ((aEvent.iErrorCode != KNotCompleteVideoError) ||
            (aEvent.iEventType != KMMFEventCategoryVideoPrepareComplete))
    {
        CMMAAudioPlayer::HandleEvent(aEvent);
    }

    if (aEvent.iEventType == KMMFEventCategoryVideoSurfaceCreated)
    {
        if (aEvent.iErrorCode == KErrNone)
        {
            TSurfaceId surfaceId;
            TRect cropRect;
            TVideoAspectRatio pixelAspectRatio;

            iVideoPlaySurfaceSupportCustomCommands.GetSurfaceParameters(surfaceId,
                    cropRect,
                    pixelAspectRatio);

            if (iMMASurface.iPrevSurfaceAvailable)
            {
                // free Surface
                TInt error = iVideoPlaySurfaceSupportCustomCommands.SurfaceRemoved(iMMASurface.iPrevSurfaceId);
                if (KErrNone != error)
                {
                    DEBUG_INT("CMMAVideoPlayer::HandleEvent:SurfaceRemoved error, %d", aEvent.iErrorCode);
                }
            }
            iMMASurface.iPrevSurfaceId = surfaceId;
            iMMASurface.iPrevSurfaceAvailable = ETrue;

            iSurfaceWindow->SetSurfaceParameters(surfaceId,
                                                 cropRect,
                                                 pixelAspectRatio);

            DEBUG("CMMAVideoPlayer::HandleEvent: KMMFEventCategoryVideoSurfaceCreated, surface parameters set");
        }
        else
        {
            DEBUG_INT("CMMAVideoPlayer::HandleEvent: error getting surface parameters, %d", aEvent.iErrorCode);
        }
    }
    else if (aEvent.iEventType == KMMFEventCategoryVideoSurfaceParametersChanged)
    {
        if (aEvent.iErrorCode == KErrNone)
        {
            TSurfaceId surfaceId;
            TRect cropRect;
            TVideoAspectRatio pixelAspectRatio;

            iVideoPlaySurfaceSupportCustomCommands.GetSurfaceParameters(surfaceId,
                    cropRect,
                    pixelAspectRatio);

            if (iMMASurface.iPrevSurfaceAvailable)
            {
                // free Surface
                TInt error = iVideoPlaySurfaceSupportCustomCommands.SurfaceRemoved(iMMASurface.iPrevSurfaceId);
                if (KErrNone != error)
                {
                    DEBUG_INT("CMMAVideoPlayer::HandleEvent:SurfaceRemoved error, %d", aEvent.iErrorCode);
                }
            }
            iMMASurface.iPrevSurfaceAvailable = ETrue;
            iMMASurface.iPrevSurfaceId = surfaceId;

            iSurfaceWindow->SetChangedSurfaceParameters(surfaceId,
                    cropRect,
                    pixelAspectRatio);

            DEBUG("CMMAVideoPlayer::HandleEvent: KMMFEventCategoryVideoSurfaceParametersChanged");
        }
        else
        {
            DEBUG_INT("CMMAVideoPlayer::HandleEvent: surface parameters changed error, %d", aEvent.iErrorCode);
        }
    }
    else if (aEvent.iEventType == KMMFEventCategoryVideoRemoveSurface)
    {
        if (aEvent.iErrorCode == KErrNone)
        {
            if (iMMASurface.iPrevSurfaceAvailable)
            {
                // free Surface
                TInt error = iVideoPlaySurfaceSupportCustomCommands.SurfaceRemoved(iMMASurface.iPrevSurfaceId);
                if (KErrNone != error)
                {
                    DEBUG_INT("CMMAVideoPlayer::HandleEvent:SurfaceRemoved error, %d", aEvent.iErrorCode);
                }
                iMMASurface.iPrevSurfaceAvailable = EFalse;
            }
        }
        else
        {
            DEBUG_INT("CMMAVideoPlayer::HandleEvent:KMMFEventCategoryVideoRemoveSurface error, %d", aEvent.iErrorCode);
        }
    }
    // video opened, preparing
    else if (aEvent.iEventType == KMMFEventCategoryVideoOpenComplete)
    {
        if (aEvent.iErrorCode == KErrNone)
        {
            TInt error = iVideoPlaySurfaceSupportCustomCommands.UseSurfaces();
            DEBUG_INT("MMA::CMMAVideoPlayer::HandleEvent::After UseSurfaces(), error = %d", error);
            TInt prepareError(iVideoPlayControllerCustomCommands.Prepare());
            if (prepareError != KErrNone)
            {
                // opening failed, notifying java
                PostActionCompleted(prepareError);
            }
        }
        else
        {
            // opening failed, notifying java
            PostActionCompleted(aEvent.iErrorCode);
        }
    }
    // final state of prefetch ( prepare completed )
    else if (aEvent.iEventType == KMMFEventCategoryVideoPrepareComplete)
    {
        // This callback must be handled differently depending on whether
        // player is created for a file locator or for a stream. When file
        // locator is used, this callback is made in realized state. For
        // stream it is made in prefetched state.
        PrepareDisplay();
        if (iFileName)
        {
            DEBUG("CMMAVideoPlayer::HandleEvent: Using filename, change state to REALIZED");

            // If there is an error condition, then the player state is not
            // changed, which indicates the error condition to StartL when
            // the ActiveSchedulerWait returns. KNotCompleteVideoError is not
            // considered as an error condition, instead it indicates that some
            // elements of the media cannot be played (e.g. video OR audio)
            if (aEvent.iErrorCode == KErrNone ||
                    aEvent.iErrorCode == KNotCompleteVideoError)
            {
                ChangeState(ERealized);
            }
            __ASSERT_DEBUG(iActiveSchedulerWait->IsStarted(), User::Invariant());
            iActiveSchedulerWait->AsyncStop();
        }
        else
        {
            DEBUG("CMMAVideoPlayer::HandleEvent: Not using filename, change state to PREFETCHED");
            CompletePrefetch(aEvent.iErrorCode);
        }
    }
    else            // in case of any other event
    {
        if (aEvent.iErrorCode != KErrNone)
        {
            PostActionCompleted(aEvent.iErrorCode);   //some other error
        }
    }
}

void CMMAVideoPlayer::CompletePrefetch(TInt aError)
{
    DEBUG_INT("CMMAVideoPlayer::CompletePrefetch + error = %d",aError);
    // Post KNotCompleteVideoError as KErrNone to the Java side, because
    // video can be played.
    if (aError == KNotCompleteVideoError)
    {
        DEBUG("CMMAVideoPlayer::CompletePrefetch  KNotCompleteVideoError ");
        // release java
        PostActionCompleted(KErrNone);
    }
    else
    {
        DEBUG("CMMAVideoPlayer::CompletePrefetch  CompleteVideoError ");
        // release java
        PostActionCompleted(aError);
    }

    if (aError == KErrNone || aError == KNotCompleteVideoError)
    {
        ChangeState(EPrefetched);
    }
    DEBUG("CMMAVideoPlayer::CompletePrefetch - ");
}

void CMMAVideoPlayer::PrepareDisplay()
{
    DEBUG("CMMAVideoPlayer::PrepareDisplay +");
    // construction should have leaved if iSurfaceWindow does not exist
    __ASSERT_DEBUG(iSurfaceWindow,
                   User::Panic(_L("CMMVideoPlayer::iSurfaceWindow is null"),
                               KErrArgument));

    //First place where we are certain that source size can be fetched
    TSize sourceSize;

    TInt err = iVideoControllerCustomCommands.GetVideoFrameSize(sourceSize);

    DEBUG_INT("MID::CMMAVideoPlayer::PrepareDisplay: GetVideoFrameSize err = %d", err);

    // Still we did not get the size of video
    if ((err != KErrNone) ||
            (sourceSize.iWidth <= 0) ||
            (sourceSize.iHeight <= 0))
    {
        DEBUG("MID::CMMAVideoPlayer::PrepareDisplay: No sourcesize found, using SurfaceWindow size");
        // setting size to window size (client rect)
        sourceSize = iSurfaceWindow->WindowSize();
    }

    // If 1x1 was got (the default size of form), it must be replaced
    // with a valid size as controller will not accept 1x1.
    if ((sourceSize.iWidth < KMMAVideoMinDimension) ||
            (sourceSize.iHeight < KMMAVideoMinDimension))
    {
        DEBUG("MID::CMMAVideoPlayer::PrepareDisplay: Unacceptable source size, using failsafe");
        // This is a special case and ought to be used only in
        // the rare case that real size is not got from stream.
        sourceSize = TSize(KMMAVideoMinDimension, KMMAVideoMinDimension);
    }

    iSourceSize = sourceSize;

    // If init has been already done
    if (iDisplay)
    {
        DEBUG("MID::CMMAVideoPlayer::PrepareDisplay: display exists, changing source size");
        SourceSizeChanged();
    }

    // Setting (in)visible if something has changed the DSA state
    // (e.g. prepare). If initDisplayMode is not called, this will always
    // set visibility to false.
    iSurfaceWindow->SetVisible(iSurfaceWindow->IsVisible(), EFalse);
    DEBUG("CMMAVideoPlayer::PrepareDisplay -");
}

EXPORT_C const TDesC& CMMAVideoPlayer::Type()
{
    return KMMAVideoPlayer;
}

EXPORT_C TSize CMMAVideoPlayer::SourceSize()
{
    return iSourceSize;
}

EXPORT_C MMMASnapshot::TEncoding CMMAVideoPlayer::TakeSnapshotL(TRequestStatus* aStatus,
        const TSize& /*aSize*/,
        const CMMAImageSettings& /*aSettings*/)
{
    if (iEmptySnapshotImage)
    {
        // Ealier snapshot was not got with SnapshotBitmap method.
        User::Leave(KErrInUse);
    }
    // frame can't be got from video player, but TCK requires that it should
    // be available. So returning empty image
    iEmptySnapshotImage = new(ELeave) CFbsBitmap();
    User::LeaveIfError(iEmptySnapshotImage->Create(TSize(1, 1),
                       EColor4K));


    User::RequestComplete(aStatus, KErrNone);

    // Return raw bitmap encoding and thus SnapshotEncoded() should not
    // get called later on.
    return EBitmap;
}

EXPORT_C CFbsBitmap* CMMAVideoPlayer::SnapshotBitmap()
{
    // snapshot is not supported, returning empty image
    CFbsBitmap* image = iEmptySnapshotImage;

    // ownership is transferred to caller
    iEmptySnapshotImage = NULL;
    return image;
}

EXPORT_C HBufC8* CMMAVideoPlayer::SnapshotEncoded()
{
    // This method should never be called.
    // Asserted in debug build to be sure.
    __ASSERT_DEBUG(EFalse, User::Invariant());

    return NULL;
}

EXPORT_C void CMMAVideoPlayer::NotifyWithStringEvent(
    CMMAPlayerEvent::TEventType aEventType,
    const TDesC& aStringEventData)
{
    PostStringEvent(aEventType, aStringEventData);
}

EXPORT_C MMMASnapshot* CMMAVideoPlayer::SnapshoterL()
{
    return this;
}

void CMMAVideoPlayer::SourceSizeChanged()
{
    iDisplay->SourceSizeChanged(iSourceSize);
    NotifyWithStringEvent(CMMAPlayerEvent::ESizeChanged, KVideoControlName);
}

//  END OF FILE
