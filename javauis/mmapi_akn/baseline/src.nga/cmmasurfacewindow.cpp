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
* Description:  This class implements MMMADisplayWindow functionality
*               in graphics surface based displays for Helix engine.
*
*/

//  Include Files
#include <AccMonitorInfo.h>
#include <AccMonitorCapabilities.h>
#include "jdebug.h"
#include "cmmasurfacewindow.h"
#include "cmmaplayer.h"

// Used for iDisplay member
#include "mmmadisplay.h"

EXPORT_C CMMASurfaceWindow* CMMASurfaceWindow::NewL(
    CMMAEventSource* aEventSource,
    CMMAPlayer* aPlayer,
    TBool aAVCableConnStatus)
{
    CMMASurfaceWindow* self =
        new(ELeave) CMMASurfaceWindow(aEventSource,
                                      aPlayer,
                                      aAVCableConnStatus);
    return self;
}

// Destructor (virtual by CBase)
CMMASurfaceWindow::~CMMASurfaceWindow()
{
    // It's not allowed to delete any nonsharable object here.
    // This must be done in Destroy().
    // If the iDisplay is set,
    // instance should be deleted by sending
    // event from UI to be received by MUiEventConsumer.
}

CMMASurfaceWindow::CMMASurfaceWindow(
    CMMAEventSource* aEventSource,
    CMMAPlayer* aPlayer,
    TBool aAVCableConnStatus):
        iEventSource(aEventSource),
        iPlayer(aPlayer),
        iVideoDisplayInitState(EUIResourcesAndSurfaceParametersNotSet),
        iAVCableConnected(aAVCableConnStatus)
{
    // Empty rect until video size is known
    iContentRect.SetRect(0, 0, 0, 0);
    iParentRect.SetRect(0, 0, 0, 0);
    iRWindowRect.SetRect(0, 0, 0, 0);
}

void CMMASurfaceWindow::SetDestinationBitmapL(CFbsBitmap* /*aBitmap*/)
{
    // Ignored, this window will not be used for actual drawing
}

void CMMASurfaceWindow::DrawFrameL(const CFbsBitmap* /*aBitmap*/)
{
    // Ignored, this window will not be used for actual drawing
}

void CMMASurfaceWindow::SetDrawRectThread(const TRect& aRect)
{
    DEBUG_INT2("MID::CMMASurfaceWindow::SetDrawRectThread TL %d %d",
               aRect.iTl.iX, aRect.iTl.iY);
    DEBUG_INT2("MID::CMMASurfaceWindow::SetDrawRectThread BR %d %d",
               aRect.iBr.iX, aRect.iBr.iY);

    iContentRect = aRect;
    TInt error = StaticRedrawVideo(*this);
    if (KErrNone != error)
    {
        DEBUG_INT("MID::CMMASurfaceWindow::SetDrawRectThread, StaticRedrawVideo error = %d", error);
    }
}

void CMMASurfaceWindow::SetRWindowRect(const TRect& aRect,
                                       MMMADisplay::TThreadType aThreadType)
{
    DEBUG_INT2("MID::CMMASurfaceWindow::SetRWindowRect TL %d %d",
               aRect.iTl.iX, aRect.iTl.iY);
    DEBUG_INT2("MID::CMMASurfaceWindow::SetRWindowRect BR %d %d",
               aRect.iBr.iX, aRect.iBr.iY);

    if (iRWindowRect == aRect)
    {
        return;
    }

    iRWindowRect = aRect;

    if (MMMADisplay::EMmaThread == aThreadType)
    {
        if (iDisplay)
        {
            iDisplay->UIGetCallback(*this,
                                    CMMASurfaceWindow::ESetClipRect);
        }
    }
    else if (MMMADisplay::EUiThread == aThreadType)
    {
        TInt error = SetClipRect();
        if (KErrNone != error)
        {
            DEBUG_INT("MID::CMMASurfaceWindow::SetRWindowRect, error = %d", error);
        }
    }
}

TInt CMMASurfaceWindow::SetClipRect()
{
    DEBUG("MID::CMMASurfaceWindow::SetClipRect");

    // CMediaClientVideoDisplay expects client to RemoveDisplayWindow
    // and AddDisplayWindow again everytime when RWindow rect changes.
    // If audio/video cable is connected do not reposition the video,
    // this is done to prevent video playback pause.
    if (iMediaClientVideoDisplay && iWindow && !iAVCableConnected)
    {
        iMediaClientVideoDisplay->RemoveDisplayWindow(*iWindow);

        TRect contentRect;
        if (iVisible)
        {
            contentRect = iContentRect;
        }
        else
        {
            TRect emptyRect(0,0,0,0);
            contentRect = emptyRect;
        }

        // align parent rect with respect to RWindow
        TRect relativeParentRect;
        relativeParentRect = iParentRect;
        relativeParentRect.Move(-iRWindowRect.iTl);

        // setting video draw rect and adjusting it to window
        TRect drawRect = contentRect;
        drawRect.Move(relativeParentRect.iTl);

        TRect clipRect(0,0,iRWindowRect.Width(),iRWindowRect.Height());
        TRAPD(error, iMediaClientVideoDisplay->AddDisplayWindowL(iWindow,
                clipRect, // new clip rect
                iVideoCropRegion,
                drawRect,  // video extent
                0.0f, // ignore
                0.0f, // ignore
                EVideoRotationNone,
                EAutoScaleBestFit,
                EHorizontalAlignCenter,
                EVerticalAlignCenter,
                (RWindow*)iWindow));

        return error;
    }
    return KErrNone;
}

void CMMASurfaceWindow::SetDrawRect(const TRect& aRect)
{
    DEBUG_INT2("MID::CMMASurfaceWindow::SetDrawRect TL %d %d",
               aRect.iTl.iX, aRect.iTl.iY);
    DEBUG_INT2("MID::CMMASurfaceWindow::SetDrawRect BR %d %d",
               aRect.iBr.iX, aRect.iBr.iY);

    iContentRect = aRect;
    if (iDisplay)
    {
        iDisplay->UIGetCallback(*this,
                                CMMASurfaceWindow::ESetDrawRect);
    }
}

TInt CMMASurfaceWindow::StaticRedrawVideo(CMMASurfaceWindow& aSurfaceWindow)
{
    TRAPD(error, aSurfaceWindow.RedrawVideoL());
    return error;
}

void CMMASurfaceWindow::RedrawVideoL()
{
    if (!iMediaClientVideoDisplay)
    {
        DEBUG("MID::CMMASurfaceWindow::RedrawVideoL: no MediaClientVideoDisplay set, aborting -");
        return;
    }

    if (iWindow)
    {
        TRect contentRect;
        if (iVisible)
        {
            contentRect = iContentRect;
        }
        else
        {
            TRect emptyRect(0,0,0,0);
            contentRect = emptyRect;
        }

        // align parent rect with respect to RWindow
        TRect relativeParentRect;
        relativeParentRect = iParentRect;
        relativeParentRect.Move(-iRWindowRect.iTl);

        // setting video draw rect and adjusting it to window
        TRect drawRect = contentRect;
        drawRect.Move(relativeParentRect.iTl);

        DEBUG_INT4("MID::CMMASurfaceWindow::RedrawVideoL: drawRect tl %d,%d br %d,%d",
                   drawRect.iTl.iX,drawRect.iTl.iY,drawRect.iBr.iX,drawRect.iBr.iY);

        iMediaClientVideoDisplay->SetVideoExtentL(*iWindow,
                drawRect,
                iVideoCropRegion);

        DEBUG("MID::CMMASurfaceWindow::RedrawVideoL: RedrawWindows()");

        iMediaClientVideoDisplay->RedrawWindows(iVideoCropRegion);
        ((RWindow*)iWindow)->Invalidate();
    }
}

const TRect& CMMASurfaceWindow::DrawRect()
{
    return iContentRect;
}

TSize CMMASurfaceWindow::WindowSize()
{
    return iParentRect.Size();
}

void CMMASurfaceWindow::SetPosition(const TPoint& aPosition)
{
    SetDrawRectThread(TRect(aPosition, iContentRect.Size()));
}

TBool CMMASurfaceWindow::IsVisible() const
{
    DEBUG_INT("MID::CMMASurfaceWindow::IsVisible %d ", iVisible);
    return iVisible;
}

void CMMASurfaceWindow::SetVisible(TBool aVisible, TBool aUseEventServer)
{
    DEBUG_INT("MID::CMMASurfaceWindow::SetVisible aVisible %d", aVisible);
    RPointerArray< CMMAPlayer > players = iEventSource->Players();

    if (players.Find(iPlayer) != KErrNotFound)
    {
        DEBUG("MID::CMMASurfaceWindow::SetVisible : Player found");
        if (aVisible != iVisible)
        {
            DEBUG("MID::CMMASurfaceWindow::SetVisible: Changed visibility");
            iVisible = aVisible;

            if (aUseEventServer)
            {
                TInt error = StaticRedrawVideo(*this);
                if (KErrNone != error)
                {
                    DEBUG_INT("MID::CMMASurfaceWindow::SetVisible, StaticRedrawVideo error = %d", error);
                }
            }
            else // in MMA thread, so switch to UI thread
            {
                if (iDisplay)
                {
                    iDisplay->UIGetCallback(*this,
                                            CMMASurfaceWindow::ESetDrawRect);
                }
            }
        }
    }
}

void CMMASurfaceWindow::SetWindowRect(const TRect& aRect,MMMADisplay::TThreadType /*aThreadType*/)
{
    DEBUG_INT2("MID::CMMASurfaceWindow::SetWindowRect aRect TL %d %d",
               aRect.iTl.iX, aRect.iTl.iY);
    DEBUG_INT2("MID::CMMASurfaceWindow::SetWindowRect aRect BR %d %d",
               aRect.iBr.iX, aRect.iBr.iY);

    iParentRect = aRect;
}

void CMMASurfaceWindow::SetVideoCropRegion(const TRect& aRect)
{
    // video size
    iVideoCropRegion = aRect;
}

const TRect& CMMASurfaceWindow::WindowRect()
{
    return iParentRect;
}

void CMMASurfaceWindow::ContainerDestroyed()
{
    // We are in UI thread context now.
    CleanVideoDisplay();
}

EXPORT_C void CMMASurfaceWindow::SetDisplay(MMMADisplay *aDisplay)
{
    DEBUG("MID::CMMASurfaceWindow::SetDisplay +");

    if (iDisplay != aDisplay)
    {
        if (iDisplay)
        {
            // Clear the resources created within the old Display
            iDisplay->UIGetCallback(
                *this, CMMASurfaceWindow::ECleanVideoDisplay);
        }

        // Set the new Display
        iDisplay = aDisplay;

        if (iDisplay)
        {
            // Get a DSA resources for the new Display
            iDisplay->UIGetDSAResources(*this, MMMADisplay::EMmaThread);
        }
    }

    DEBUG("MID::CMMASurfaceWindow::SetDisplay -");
}

void CMMASurfaceWindow::ContainerSet()
{
    // We are in UI thread now

    // Container was probably not set when
    // iDisplay was set,
    // we can now try get the DSA stuff again
    if (iDisplay)
    {
        // Get a DSA stuff for the new Display
        iDisplay->UIGetDSAResources(*this, MMMADisplay::EUiThread);
    }
}

void CMMASurfaceWindow::Destroy()
{
    DEBUG("MID::CMMASurfaceWindow::Destroy");
    // Delete itself
    delete this;
}

void CMMASurfaceWindow::MdcDSAResourcesCallback(
    RWsSession &aWs,
    CWsScreenDevice &aScreenDevice,
    RWindowBase &aWindow)
{
    DEBUG("MID::CMMASurfaceWindow::MdcDSAResourcesCallback");

    // We are in UI thread context now.
    iWs = &aWs;
    iScreenDevice = &aScreenDevice;
    iWindow = &aWindow;

    switch (iVideoDisplayInitState)
    {
    case EUIResourcesAndSurfaceParametersNotSet:
    {
        iVideoDisplayInitState =
            EUIResourcesSetAndSurfaceParametersNotSet;
    }
    break;
    case ESurfaceParametersSetAndUIResourcesNotSet:
    {
        iVideoDisplayInitState =
            EUIResourcesAndSurfaceParametersSet;
    }
    break;
    // can not occur
    case EUIResourcesSetAndSurfaceParametersNotSet:
    case EUIResourcesAndSurfaceParametersSet:
    default:
    {
        __ASSERT_DEBUG(EFalse, User::Invariant());
    }
    break;
    }

    if (iVideoDisplayInitState == EUIResourcesAndSurfaceParametersSet)
    {
        TRAPD(error, InitVideoDisplayL());
        if (KErrNone != error)
        {
            DEBUG_INT("MID::CMMASurfaceWindow::MdcDSAResourcesCallback, error = %d", error);
        }
    }
}

void CMMASurfaceWindow::MdcUICallback(TInt aCallbackId)
{
    // We are in UI thread context now.
    DEBUG_INT("MID::CMMASurfaceWindow::MdcUICallback CallbackId = %d", aCallbackId);

    TInt error = KErrNone;
    switch (aCallbackId)
    {
    case ESetClipRect:
    {
        error = SetClipRect();
        DEBUG_INT("MID::CMMASurfaceWindow::MdcUICallback,SetClipRect error = %d", error);
    }
    break;
    case ERemoveSurface:
    {
        DoRemoveSurface();
    }
    break;
    case ESetDrawRect:
    {
        error = StaticRedrawVideo(*this);
        DEBUG_INT("MID::CMMASurfaceWindow::MdcUICallback,StaticRedrawVideo error = %d", error);
    }
    break;
    case EInitVideoDisplay:
    {
        TRAP(error, InitVideoDisplayL());
        DEBUG_INT("MID::CMMASurfaceWindow::MdcUICallback,InitVideoDisplayL error = %d", error);
    }
    break;
    case ESetChangedSurfaceParameters:
    {
        DoSetChangedSurfaceParameters();
    }
    break;
    case EResetSurfaceParameters:
    {
        DoResetSurfaceParameters();
    }
    break;
    case ECleanVideoDisplay:
    {
        CleanVideoDisplay();
    }
    break;
    case EDestroyWindow:
    {
        Destroy();
    }
    break;
    default:
    {
        __ASSERT_DEBUG(EFalse, User::Invariant());
    }
    break;
    }
}

EXPORT_C void CMMASurfaceWindow::SetSurfaceParameters(const TSurfaceId& aSurfaceId,
        const TRect& aCropRect,
        const TVideoAspectRatio& aPixelAspectRatio)
{
    iSurfaceId = aSurfaceId;
    iCropRect = aCropRect;
    iPixelAspectRatio = aPixelAspectRatio;

    switch (iVideoDisplayInitState)
    {
    case EUIResourcesAndSurfaceParametersNotSet:
    {
        iVideoDisplayInitState =
            ESurfaceParametersSetAndUIResourcesNotSet;
    }
    break;
    case EUIResourcesSetAndSurfaceParametersNotSet:
    {
        iVideoDisplayInitState =
            EUIResourcesAndSurfaceParametersSet;
    }
    break;
    // control reaches below two switch cases when
    // playback is looped using setLoopCount() in java.
    case ESurfaceParametersSetAndUIResourcesNotSet:
    {
    }
    break;
    // update surface parameters and return with out calling
    // InitVideoDisplayL again.
    case EUIResourcesAndSurfaceParametersSet:
    {
        iDisplay->UIGetCallback(*this, CMMASurfaceWindow::EResetSurfaceParameters);
        return;
    }
    // break; not reachable
    default: // can not occur
    {
        __ASSERT_DEBUG(EFalse, User::Invariant());
    }
    break;
    }

    if (iVideoDisplayInitState == EUIResourcesAndSurfaceParametersSet)
    {
        iDisplay->UIGetCallback(*this, CMMASurfaceWindow::EInitVideoDisplay);
    }
}

EXPORT_C void CMMASurfaceWindow::SetChangedSurfaceParameters(const TSurfaceId& aSurfaceId,
        const TRect& aCropRect,
        const TVideoAspectRatio& aPixelAspectRatio)
{
    iSurfaceId = aSurfaceId;
    iCropRect = aCropRect;
    iPixelAspectRatio = aPixelAspectRatio;

    if (iDisplay)
    {
        iDisplay->UIGetCallback(*this, CMMASurfaceWindow::ESetChangedSurfaceParameters);
    }
}

EXPORT_C void CMMASurfaceWindow::RemoveSurface()
{
    if (iDisplay)
    {
        iDisplay->UIGetCallback(*this,
                                CMMASurfaceWindow::ERemoveSurface);
    }
}

void CMMASurfaceWindow::DoRemoveSurface()
{
    if (iMediaClientVideoDisplay)
    {
        iMediaClientVideoDisplay->RemoveSurface();
        DEBUG("MID::CMMASurfaceWindow::DoRemoveSurface, Surface Removed");
    }
}

void CMMASurfaceWindow::DoResetSurfaceParameters()
{
    __ASSERT_DEBUG((iMediaClientVideoDisplay != NULL), User::Invariant());

    if (iMediaClientVideoDisplay)
    {
        TInt error = iMediaClientVideoDisplay->SurfaceCreated(iSurfaceId,
                     iCropRect,
                     iPixelAspectRatio,
                     iVideoCropRegion);

        DEBUG_INT("MID::CMMASurfaceWindow::DoResetSurfaceParameters,SurfaceCreated error = %d", error);

        iMediaClientVideoDisplay->RedrawWindows(iVideoCropRegion);
    }
}

void CMMASurfaceWindow::DoSetChangedSurfaceParameters()
{
    if (iMediaClientVideoDisplay)
    {
        TInt error = iMediaClientVideoDisplay->SurfaceParametersChanged(iSurfaceId,
                     iCropRect,
                     iPixelAspectRatio);

        DEBUG_INT("MID::CMMASurfaceWindow::DoSetChangedSurfaceParameters,SurfaceParametersChanged, error = %d", error);

        iMediaClientVideoDisplay->RedrawWindows(iVideoCropRegion);
    }
}

void CMMASurfaceWindow::InitVideoDisplayL()
{
    if (iVideoDisplayInitState != EUIResourcesAndSurfaceParametersSet)
    {
        User::Leave(KErrNotReady);
    }

    // check that this is the first time we are creating instance.
    if (iMediaClientVideoDisplay)
    {
        __ASSERT_DEBUG(EFalse, User::Invariant());
    }

    iMediaClientVideoDisplay =
        CMediaClientVideoDisplay::NewL(iScreenDevice->GetScreenNumber(),
                                       iSurfaceId,
                                       iCropRect,
                                       iPixelAspectRatio);

    TInt error = iMediaClientVideoDisplay->SurfaceCreated(iSurfaceId,
                 iCropRect,
                 iPixelAspectRatio,
                 iVideoCropRegion);

    DEBUG_INT("MID::CMMASurfaceWindow::InitVideoDisplayL error = %d", error);
    User::LeaveIfError(error);

    TRect contentRect;
    if (iVisible)
    {
        contentRect = iContentRect;
    }
    else
    {
        TRect emptyRect(0,0,0,0);
        contentRect = emptyRect;
    }

    // align parent rect with respect to RWindow
    TRect relativeParentRect;
    relativeParentRect = iParentRect;
    relativeParentRect.Move(-iRWindowRect.iTl);

    // setting video draw rect and adjusting it to window
    TRect drawRect = contentRect;
    drawRect.Move(relativeParentRect.iTl);

    TRect clipRect(0,0,iRWindowRect.Width(),iRWindowRect.Height());
    iMediaClientVideoDisplay->AddDisplayWindowL(iWindow,
            clipRect,
            iVideoCropRegion,
            drawRect,  // video extent
            0.0f, // ignore
            0.0f, // ignore
            EVideoRotationNone,
            EAutoScaleBestFit,
            EHorizontalAlignCenter,
            EVerticalAlignCenter,
            (RWindow*)iWindow);

    DEBUG("MID::CMMASurfaceWindow::InitVideoDisplayL -");
}

void CMMASurfaceWindow::CleanVideoDisplay()
{
    DEBUG("MID::CMMASurfaceWindow::CleanVideoDisplay +");

    SetVisible(EFalse, ETrue);
    if (iMediaClientVideoDisplay)
    {
        if (iWindow)
        {
            iMediaClientVideoDisplay->RemoveDisplayWindow(*iWindow);
        }
        iMediaClientVideoDisplay->RemoveSurface();
        delete iMediaClientVideoDisplay;

        iMediaClientVideoDisplay = NULL;
        iWindow = NULL;
        iScreenDevice = NULL;
        iWs = NULL;
    }

    DEBUG("MID::CMMASurfaceWindow::CleanVideoDisplay -");
}

EXPORT_C void CMMASurfaceWindow::SetAVCableConnStatus(TBool aStatus)
{
    iAVCableConnected = aStatus;
    if (iDisplay && !iAVCableConnected)
    {
        iDisplay->UIGetCallback(*this,
                                CMMASurfaceWindow::ESetClipRect);
    }
}

//  END OF FILE
