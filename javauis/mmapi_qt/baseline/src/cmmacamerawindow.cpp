/*
* Copyright (c) 2009 Nokia Corporation and/or its subsidiary(-ies).
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
*                for Camera viewfinder usage.
*
*/


//  Include Files
#include <eikenv.h>
#include <gdi.h>
#include <logger.h>
#include <AknsUtils.h>
#include <AknsDrawUtils.h>
#include "cmmacanvasdisplay.h"
#include "logger.h"

class CMMACanvasDisplay;

//#include <cameraapp.mbg>

// Used for iDisplay member

#include "cmmacamerawindow.h"

// Camera viewfinder start delay
const TInt KStarterTimeout = 100000; //100 msecs

// Margin of icon displayed instead of viewfinder in case of error
const TInt KErrorIconMargin = 5; //5 pixels


//_LIT(KCameraAppBitmapFile, "z:\\resource\\apps\\cameraapp.mif");


// ---------------------------------------------------------------------------
// CMMACameraWindow::NewL
// Two-phased constructor.
// Use this method to create a CMMACameraWindow instance.
// ---------------------------------------------------------------------------
//
CMMACameraWindow* CMMACameraWindow::NewL(TInt aCameraHandle)
{
    CMMACameraWindow* self = new(ELeave)CMMACameraWindow(aCameraHandle);
    return self;
}


// ---------------------------------------------------------------------------
// CMMACameraWindow::~CMMACameraWindow
// Destructor.
// NOTE: iUICamera have to be deleted by ReleaseUiResources() called
//       from UI thread
// ---------------------------------------------------------------------------
//
CMMACameraWindow::~CMMACameraWindow()
{
    // It's not allowed to delete any nonsharable object here.
    // This must be done in Destroy().
    // If the iDisplay is set,
    // instance should be deleted by sending
    // event from UI to be received by MUiEventConsumer.
}


// ---------------------------------------------------------------------------
// CMMACameraWindow::ViewFinderActive
// Gets an information if the viewfinder is started.
// ---------------------------------------------------------------------------
//
TBool CMMACameraWindow::ViewFinderActive()
{
    return iUICamera && iViewFinderVisible;
}


// ---------------------------------------------------------------------------
// CMMACameraWindow::SetStarted
// Notifies window about Started state change.
// If started, tries to start viewfinder.
// If not started, stops viewfinder.
// ---------------------------------------------------------------------------
//
void CMMACameraWindow::SetStarted(TBool aStarted)
{
    iStarted = aStarted;
    if (iDisplay)
    {
        if (iStarted)
        {
            //  iDisplay->UIGetCallback(
            //      *this, CMMACameraWindow::EShowViewFinder);
            LOG(EJavaMMAPI,EInfo,"CMMACameraWindow::SetStarted : before GetCallbackInUiThread + EShowViewFinder");
            iDisplay->GetCallbackInUiThread((TInt)CMMACameraWindow::EShowViewFinder);
            LOG(EJavaMMAPI,EInfo,"CMMACameraWindow::SetStarted : before GetCallbackInUiThread  - EShowViewFinder");
        }
        else
        {
            //   iDisplay->UIGetCallback(
            //       *this, CMMACameraWindow::EHideViewFinder);
            LOG(EJavaMMAPI,EInfo,"CMMACameraWindow::SetStarted : before GetCallbackInUiThread + EHideViewFinder");
            iDisplay->GetCallbackInUiThread((TInt)CMMACameraWindow::EHideViewFinder);
            LOG(EJavaMMAPI,EInfo,"CMMACameraWindow::SetStarted : before GetCallbackInUiThread  - EHideViewFinder");
        }
    }
}


// ---------------------------------------------------------------------------
// CMMACameraWindow::SetDisplay
// Sets iDisplay used to invoke callbacks from UI thread.
// ---------------------------------------------------------------------------
//
void CMMACameraWindow::SetDisplay(MMMADisplay *aDisplay)
{
    if (iDisplay != aDisplay)
    {
        if (iDisplay)
        {
            // Clear the resources created within the old Display
            //iDisplay->UIGetCallback(
            //    *this, CMMACameraWindow::EDeleteViewFinder);


            // instead make a call to java, shift to ui thread and call ReleaseUiResources() method in UI thread

            // here jobject and jclass is valid since the flow is first creat CMMACanvasDisplay and then
            // call setdeisplay of cameraplayer
            /*iJavaDisplayObject = iDisplay->GetJavaDisplayObject();
            iJavaDisplayClass = iDisplay->GetJavaDisplayClass();
            iJni = iDisplay->GetJNIEnv();

             jmethodID releaseUiResourceID = iJni->GetMethodID(
                                         iJavaDisplayClass,
                                         "releaseUiResource",
                                         "()I");

             iJni->CallIntMethod(iJavaDisplayObject,releaseUiResourceID);*/
            //MMAPI UI 3.x req.
            LOG(EJavaMMAPI,EInfo,"CMMACameraWindow::SetDisplay : before GetCallbackInUiThread +");
            iDisplay->GetCallbackInUiThread((TInt)CMMACameraWindow::EDeleteViewFinder);
            LOG(EJavaMMAPI,EInfo,"CMMACameraWindow::SetDisplay : before GetCallbackInUiThread  -");
        }

        // Set the new Display
        iDisplay = aDisplay;

        if (iDisplay)
        {
            // Get a DSA resources for the new Display
            //    iDisplay->UIGetDSAResources(*this, MMMADisplay::EMmaThread);

        }
    }
}


// ---------------------------------------------------------------------------
// From class MMMADisplayWindow
// CMMACameraWindow::SetDestinationBitmapL
//
// ---------------------------------------------------------------------------
//
void CMMACameraWindow::SetDestinationBitmapL(CFbsBitmap* /*aBitmap*/)
{
    // Ignored, this window will not be used for actual drawing
    LOG(EJavaMMAPI,EInfo,"CMMACameraWindow::SetDestinationBitmapL");
}


// ---------------------------------------------------------------------------
// From class MMMADisplayWindow
// CMMACameraWindow::DrawFrameL
//
// ---------------------------------------------------------------------------
//
void CMMACameraWindow::DrawFrameL(const CFbsBitmap* /*aBitmap*/)
{
    // Ignored, this window will not be used for actual drawing
}


// ---------------------------------------------------------------------------
// From class MMMADisplayWindow
// CMMACameraWindow::SetDrawRect
//
// ---------------------------------------------------------------------------
//
void CMMACameraWindow::SetDrawRect(const TRect& aRect)
{
    // MMAPI thread
    LOG2(EJavaMMAPI,EInfo,"MMA::CMMACameraWindow::SetDrawRect TL:%d,%d",
         aRect.iTl.iX, aRect.iTl.iY);
    LOG2(EJavaMMAPI,EInfo,"MMA::CMMACameraWindow::SetDrawRect BR:%d,%d",
         aRect.iBr.iX, aRect.iBr.iY);

    iDrawRect = aRect;

    if (iDisplay)
    {
        //iDisplay->UIGetCallback(*this, CMMACameraWindow::EResetViewFinder);
        LOG(EJavaMMAPI,EInfo,"CMMACameraWindow::SetDisplay : before GetCallbackInUiThread + EResetViewFinder");
        iDisplay->GetCallbackInUiThread((TInt)CMMACameraWindow::EResetViewFinder);
        LOG(EJavaMMAPI,EInfo,"CMMACameraWindow::SetDisplay : before GetCallbackInUiThread  - EResetViewFinder");

    }
}


// ---------------------------------------------------------------------------
// From class MMMADisplayWindow
// CMMACameraWindow::SetDrawRectThread
//
// ---------------------------------------------------------------------------
//
void CMMACameraWindow::SetDrawRectThread(const TRect& aRect)
{
    LOG2(EJavaMMAPI, EInfo, "MMA::CMMACameraWindow::SetDrawRectThread TL:%d,%d",
         aRect.iTl.iX, aRect.iTl.iY);
    LOG2(EJavaMMAPI, EInfo, "MMA::CMMACameraWindow::SetDrawRectThread BR:%d,%d",
         aRect.iBr.iX, aRect.iBr.iY);

    // Note: Runs in UI thread only

    // Bitmap window's rect can be set in any thread.
    if (iDrawRect != aRect)
    {
        iDrawRect = aRect;

        // Stop and start ViewFinder to update position
        ResetViewFinder();
    }

}


// ---------------------------------------------------------------------------
// From class MMMADisplayWindow
// CMMACameraWindow::DrawRect
//
// ---------------------------------------------------------------------------
//
const TRect& CMMACameraWindow::DrawRect()
{
    return iDrawRect;
}


// ---------------------------------------------------------------------------
// From class MMMADisplayWindow
// CMMACameraWindow::WindowSize
//
// ---------------------------------------------------------------------------
//
TSize CMMACameraWindow::WindowSize()
{
    return iDrawRect.Size();
}


// ---------------------------------------------------------------------------
// From class MMMADisplayWindow
// CMMACameraWindow::SetPosition
//
// ---------------------------------------------------------------------------
//
void CMMACameraWindow::SetPosition(const TPoint& aPosition)
{
    // Note: Runs in UI thread only
    SetDrawRectThread(TRect(aPosition, iDrawRect.Size()));
}


// ---------------------------------------------------------------------------
// From class MMMADisplayWindow
// CMMACameraWindow::SetVisible
//
// ---------------------------------------------------------------------------
//
void CMMACameraWindow::SetVisible(TBool aVisible, TBool aUseEventServer)
{
    iVisible = aVisible;

    if (aUseEventServer)
    {
        // We are in UI thread now.
        SetViewFinderVisibility(aVisible);
    }
    else
    {
        // MMAPI thread
        if (iDisplay)
        {
            if (aVisible)
            {
                // iDisplay->UIGetCallback(
                //    *this, CMMACameraWindow::EShowViewFinder);
                LOG(EJavaMMAPI,EInfo,"CMMACameraWindow::SetVisible : before GetCallbackInUiThread - EShowViewFinder");
                iDisplay->GetCallbackInUiThread((TInt)CMMACameraWindow::EShowViewFinder);
                LOG(EJavaMMAPI,EInfo,"CMMACameraWindow::SetVisible : before GetCallbackInUiThread  - EShowViewFinder");
            }
            else
            {
                // iDisplay->UIGetCallback(
                //     *this, CMMACameraWindow::EHideViewFinder);
                LOG(EJavaMMAPI,EInfo,"CMMACameraWindow::SetVisible : before GetCallbackInUiThread - EHideViewFinder");
                iDisplay->GetCallbackInUiThread((TInt)CMMACameraWindow::EHideViewFinder);
                LOG(EJavaMMAPI,EInfo,"CMMACameraWindow::SetVisible : before GetCallbackInUiThread  - EHideViewFinder");
            }
        }
    }
}


// ---------------------------------------------------------------------------
// From class MMMADisplayWindow
// CMMACameraWindow::SetWindowRect
//
// ---------------------------------------------------------------------------
//
void CMMACameraWindow::SetWindowRect(
    const TRect& aRect,
    MMMADisplay::TThreadType aThreadType)
{
    LOG2(EJavaMMAPI, EInfo, "MMA::CMMACameraWindow::SetWindowRect TL:%d,%d",
         aRect.iTl.iX, aRect.iTl.iY);
    LOG2(EJavaMMAPI, EInfo, "MMA::CMMACameraWindow::SetWindowRect BR:%d,%d",
         aRect.iBr.iX, aRect.iBr.iY);

    if (iClientRect != aRect)
    {
        iClientRect = aRect;

        if (aThreadType == MMMADisplay::EUiThread)
        {
            ResetViewFinder();
        }
        else
        {
            //  iDisplay->UIGetCallback(
            //      *this, CMMACameraWindow::EResetViewFinder);
            LOG(EJavaMMAPI,EInfo,"CMMACameraWindow::SetWindowRect : before GetCallbackInUiThread + EResetViewFinder");
            iDisplay->GetCallbackInUiThread((TInt)CMMACameraWindow::EHideViewFinder);
            LOG(EJavaMMAPI,EInfo,"CMMACameraWindow::SetWindowRect : before GetCallbackInUiThread  - EResetViewFinder");
        }
    }


}


// ---------------------------------------------------------------------------
// From class MMMADisplayWindow
// CMMACameraWindow::WindowRect
//
// ---------------------------------------------------------------------------
//
const TRect& CMMACameraWindow::WindowRect()
{
    return iClientRect;
}


// ---------------------------------------------------------------------------
// From class MMMADisplayWindow
// CMMACameraWindow::GetDisplayWindowType
//
// ---------------------------------------------------------------------------
//
MMMADisplayWindow::TDisplayWindowType
CMMACameraWindow::GetDisplayWindowType() const
{
    return EDisplayWindowTypeIsCamera;
}


// ---------------------------------------------------------------------------
// From class MMMADisplayWindow
// CMMACameraWindow::IsVisible
//
// ---------------------------------------------------------------------------
//
TBool CMMACameraWindow::IsVisible() const
{
    return iVisible;
}


// ---------------------------------------------------------------------------
// From class MMMADisplayWindow
// CMMACameraWindow::ContainerSet
//
// ---------------------------------------------------------------------------
//
void CMMACameraWindow::ContainerSet()
{
    // We are in UI thread now

    // Container was probably not set when
    // iDisplay was set,
    // we can now try get the DSA stuff again
    if (!iUICamera && iDisplay)
    {
        // Get a DSA stuff for the new Display
        // iDisplay->UIGetDSAResources(*this, MMMADisplay::EUiThread);
    }
}


// ---------------------------------------------------------------------------
// From class MMMADisplayWindow
// CMMACameraWindow::ContainerDestroyed
//
// ---------------------------------------------------------------------------
//
void CMMACameraWindow::ContainerDestroyed()
{
    // We are in UI thread now
    ReleaseUiResources();
}


// ---------------------------------------------------------------------------
// From class MCameraObserver
// CMMACameraWindow::ReserveComplete
//
// ---------------------------------------------------------------------------
//
void CMMACameraWindow::ReserveComplete(TInt aError)
{
    LOG1(EJavaMMAPI,EInfo,"MMA::CMMACameraWindow::ReserveComplete %d", aError);

    if (aError == KErrNone)
    {
        // camera will notify completion with PowerOnComplete method.
        iUICamera->PowerOn();
    }
}


// ---------------------------------------------------------------------------
// From class MCameraObserver
// CMMACameraWindow::PowerOnComplete
//
// ---------------------------------------------------------------------------
//
void CMMACameraWindow::PowerOnComplete(TInt aError)
{
    ELOG1(EJavaMMAPI, "MMA::CMMACameraWindow::PowerOnComplete %d", aError);

    if (aError == KErrNone)
    {
        iCameraPowerOn = ETrue;
        SetViewFinderVisibility(ETrue);
    }
}


// ---------------------------------------------------------------------------
// From class MCameraObserver
// CMMACameraWindow::ViewFinderFrameReady
//
// ---------------------------------------------------------------------------
//
void CMMACameraWindow::ViewFinderFrameReady(CFbsBitmap& /*aFrame*/)
{
    // Empty implementation of the interface method
    LOG(EJavaMMAPI,EInfo,"MMA::CMMACameraWindow::ViewFinderFrameReady");
}


// ---------------------------------------------------------------------------
// From class MCameraObserver
// CMMACameraWindow::ImageReady
//
// ---------------------------------------------------------------------------
//
void CMMACameraWindow::ImageReady(CFbsBitmap* /*aBitmap*/,
                                  HBufC8* /*aData*/,
                                  TInt /*aError*/)
{
    // Empty implementation of the interface method
    LOG(EJavaMMAPI,EInfo,"MMA::CMMACameraWindow::ImageReady");
}


// ---------------------------------------------------------------------------
// From class MCameraObserver
// CMMACameraWindow::FrameBufferReady
//
// ---------------------------------------------------------------------------
//
void CMMACameraWindow::FrameBufferReady(MFrameBuffer* /*aFrameBuffer*/,
                                        TInt /*aError*/)
{
    // Empty implementation of the interface method
    LOG(EJavaMMAPI,EInfo,"MMA::CMMACameraWindow::FrameBufferReady");
}


// ---------------------------------------------------------------------------
// From class MDirectScreenAccess.
// CMMACameraWindow::AbortNow
//
// ---------------------------------------------------------------------------
//
void CMMACameraWindow::AbortNow(
    RDirectScreenAccess::TTerminationReasons /*aReasons*/)
{
    SetViewFinderVisibility(EFalse);
}


// ---------------------------------------------------------------------------
// From class MDirectScreenAccess.
// CMMACameraWindow::Restart
//
// ---------------------------------------------------------------------------
//
void CMMACameraWindow::Restart(
    RDirectScreenAccess::TTerminationReasons /*aReasons*/)
{
    SetViewFinderVisibility(ETrue);
}


// ---------------------------------------------------------------------------
// CMMACameraWindow::CMMACameraWindow
// C++ constructor.
// ---------------------------------------------------------------------------
//
CMMACameraWindow::CMMACameraWindow(TInt aCameraHandle):
        iVisible(EFalse),
        iStarted(EFalse),
        iViewFinderVisible(EFalse),
        iCameraPowerOn(EFalse),
        iDrawRect(0, 0, 0, 0),
        iClientRect(0, 0, 0, 0),
        iCameraHandle(aCameraHandle),
        iUICamera(NULL),
        iDisplay(NULL),
        iStarterTimer(NULL),
        iDirectAccess(NULL),
        iErrorIconBitmap(NULL),
        iErrorIconMaskBitmap(NULL),
        iRWindowRect(0, 0, 0, 0)
{
}


// ---------------------------------------------------------------------------
// CMMACameraWindow::UIStartViewFinder
// Creates UI Camera.
// This CCamera instance is duplicated from handle given to the constructor.
// Have to be called from the thread which UI Camera will be controlled from.
// ---------------------------------------------------------------------------
//
void CMMACameraWindow::UIStartViewFinder(
    RWsSession &aWs,
    CWsScreenDevice &aScreenDevice,
    RWindowBase &aWindow)
{
    LOG(EJavaMMAPI,EInfo,"MMA::CMMACameraWindow::UIStartViewFinder");

    iWs = &aWs;
    iScreenDevice = &aScreenDevice;
    iWindow = &aWindow;

    if (!iUICamera)
    {
        TRAPD(error, iUICamera =
                  CCamera::NewDuplicateL(*this, iCameraHandle));

        ELOG1(EJavaMMAPI,
              "MMA::CMMACameraWindow::UIStartViewFinder - NewDuplicateL %d",
              error);

        if (error == KErrNone)
        {
            iUICamera->Reserve();
        }
    }
}


// ---------------------------------------------------------------------------
// CMMACameraWindow::SetViewFinderVisibility
// If UI Camera have been created, it starts/hides the DirectViewFinder.
// Have to be called from UI thread in which iUICamera instance was created.
// ---------------------------------------------------------------------------
//
void CMMACameraWindow::SetViewFinderVisibility(TBool aVisible)
{
    LOG1(EJavaMMAPI, EInfo, "MMA::CMMACameraWindow::SetViewFinderVisibility - %d",
         aVisible);

    if (!iUICamera || !iCameraPowerOn || iViewFinderVisible == aVisible)
    {
        return;
    }

    if (aVisible)
    {
        // Viewfinder is going to start
        if (!iVisible || !iStarted)
        {
            return;
        }

        iViewFinderVisible = ETrue;

        if (!iStarterTimer)
        {
            // If starter timer haven't been created yet, create an instance
            TRAPD(
                timerErr,
                iStarterTimer = CPeriodic::NewL(CActive::EPriorityIdle));

            if (timerErr != KErrNone)
            {
                ELOG1(EJavaMMAPI,
                      "MMA::CMMACameraWindow::SetViewFinderVisibility - timer error = %d",
                      timerErr);
                __ASSERT_DEBUG(EFalse, User::Invariant());
            }
        }

        if (!iStarterTimer->IsActive())
        {
            iStarterTimer->Start(
                KStarterTimeout,
                0,
                TCallBack(StarterTimerCallback, this));
        }
    }
    else
    {
        // Viewfinder is going to be cancelled
        LOG(EJavaMMAPI,EInfo,
            "MMA::CMMACameraWindow::SetViewFinderVisibility - Stopping VF");

        if (iStarterTimer && iStarterTimer->IsActive())
        {
            // Cancel the running starter timer
            iStarterTimer->Cancel();
        }

        if (iUICamera->ViewFinderActive())
        {
            iUICamera->StopViewFinder();
        }
        if (iDirectAccess->IsActive())
        {
            iDirectAccess->Cancel();
        }

        iViewFinderVisible = EFalse;
    }
}


// ---------------------------------------------------------------------------
// CMMACameraWindow::StarterTimerCallback
// Executed at the expiry of the iStartedTimer.
// ---------------------------------------------------------------------------
//
TInt CMMACameraWindow::StarterTimerCallback(TAny* aThis)
{
    CMMACameraWindow* self = static_cast<CMMACameraWindow*>(aThis);
    ASSERT(self);

    self->StartViewFinder();

    // We don't want to run the callback again
    return EFalse;
}


// ---------------------------------------------------------------------------
// From class MMMADisplayWindow
// CMMACameraWindow::SetRWindowRect
//
// ---------------------------------------------------------------------------
//
void CMMACameraWindow::SetRWindowRect(const TRect& aRect,
                                      MMMADisplay::TThreadType aThreadType)
{
    ELOG2(EJavaMMAPI,
          "MID::CMMACameraWindow::SetRWindowRect TL %d %d",
          aRect.iTl.iX, aRect.iTl.iY);
    ELOG2(EJavaMMAPI,
          "MID::CMMACameraWindow::SetRWindowRect BR %d %d",
          aRect.iBr.iX, aRect.iBr.iY);
    iRWindowRect = aRect;

    if (MMMADisplay::EMmaThread == aThreadType)
    {
        if (iDisplay)
        {
            //iDisplay->UIGetCallback(*this,
            //                         CMMACameraWindow::EResetViewFinder);
        }
    }
    else if (MMMADisplay::EUiThread == aThreadType)
    {
        ResetViewFinder();
    }
}


// ---------------------------------------------------------------------------
// CMMACameraWindow::StartViewFinder
// Starts viewfinder.
// ---------------------------------------------------------------------------
//
void CMMACameraWindow::StartViewFinder()
{
    LOG(EJavaMMAPI,EInfo," < StartViewFinder");

    ASSERT(iUICamera);
    ASSERT(iCameraPowerOn);
    ASSERT(iStarted);

    iStarterTimer->Cancel();

    // align client rect to RWindow, viewfinder
    // display co-ordinates is w.r.t to RWindow
    TRect relativeClientRect;
    relativeClientRect = iClientRect;
    relativeClientRect.Move(-iRWindowRect.iTl);

    // Set the drawing area
    TRect drawRect(iDrawRect);
    drawRect.Move(relativeClientRect.iTl);

    LOG2(EJavaMMAPI, EInfo,
         "MMA::CMMACameraWindow::StartViewFinder - Starting VF TL:%d,%d",
         drawRect.iTl.iX,
         drawRect.iTl.iY);
    LOG2(EJavaMMAPI, EInfo,
         "MMA::CMMACameraWindow::StartViewFinder - Starting VF BR:%d,%d",
         drawRect.iBr.iX,
         drawRect.iBr.iY);

    TRAPD(vfError, iUICamera->StartViewFinderDirectL(
              *iWs, *iScreenDevice, *iWindow, drawRect));

    if (vfError == KErrNone)
    {
        LOG(EJavaMMAPI,EInfo,"MMA::CMMACameraWindow::StartViewFinder - Start OK");
    }
    else
    {
        LOG1(EJavaMMAPI, EInfo,
             "MMA::CMMACameraWindow::StartViewFinder()  - \
StartViewFinderDirectL error=%d", vfError);

        TRAP_IGNORE(DrawViewFinderErrorL(vfError, drawRect));
    }

    LOG(EJavaMMAPI,EInfo," > StartViewFinder");
}


// ---------------------------------------------------------------------------
// CMMACameraWindow::DrawViewFinderError()
// Draws the error message to specified area.
// Used in cases when viewfinder is unable to start.
// ---------------------------------------------------------------------------
//
void CMMACameraWindow::DrawViewFinderErrorL(
    const TInt /*aError*/,
    const TRect& aDrawRect)
{

    ASSERT(iDirectAccess);
    TInt dcError = KErrNone;
    if (!iDirectAccess->IsActive())
    {
        TRAP(dcError, iDirectAccess->StartL());
    }

    TRect drawRect(aDrawRect);

    if (dcError == KErrNone)
    {
        drawRect.Intersection(iClientRect);
        drawRect.Move(-iWindow->AbsPosition());

        CFbsBitGc* directGc = iDirectAccess->Gc();
        directGc->SetClippingRect(drawRect);
        directGc->SetBrushColor(TRgb(128,128,128));
        directGc->SetPenColor(TRgb(128,0,0));
        directGc->SetBrushStyle(CGraphicsContext::ESolidBrush);
        directGc->SetPenStyle(CGraphicsContext::ESolidPen);
        directGc->SetDrawMode(CGraphicsContext::EDrawModeWriteAlpha);
        directGc->DrawRect(drawRect);

        if (!iErrorIconBitmap || !iErrorIconMaskBitmap)
        {
            if (iErrorIconBitmap)
            {
                delete iErrorIconBitmap;
                iErrorIconBitmap = NULL;
            }

            if (iErrorIconMaskBitmap)
            {
                delete iErrorIconMaskBitmap;
                iErrorIconMaskBitmap = NULL;
            }
            /*
                        AknsUtils::CreateIconL(
                            AknsUtils::SkinInstance(),
                            KAknsIIDQgnIndiCam4Camera,
                            iErrorIconBitmap,
                            iErrorIconMaskBitmap,
                            KCameraAppBitmapFile,
                            EMbmCameraappQgn_indi_cam4_camera,
                            EMbmCameraappQgn_indi_cam4_camera_mask
                        );
                        */
        }

        //TRect iconRect
        drawRect.iTl.iX += KErrorIconMargin;
        drawRect.iTl.iY += KErrorIconMargin;
        drawRect.iBr.iX -= KErrorIconMargin;
        drawRect.iBr.iY -= KErrorIconMargin;

        if (iErrorIconBitmap->SizeInPixels() != drawRect.Size())
        {
            AknIconUtils::SetSize(iErrorIconBitmap, drawRect.Size());
            AknIconUtils::SetSize(iErrorIconMaskBitmap, drawRect.Size());
        }

        directGc->BitBltMasked(
            drawRect.iTl, iErrorIconBitmap,
            TRect(iErrorIconBitmap->SizeInPixels()),
            iErrorIconMaskBitmap, EFalse);

        iDirectAccess->ScreenDevice()->Update();
    }

}


// ---------------------------------------------------------------------------
// CMMACameraWindow::ReleaseUiResources()
// Stops the DirectViewFinder and deletes UI Camera instance
// Have to be called from UI thread in which UI Camera
// have been created before the CMMACameraWindow instance
// is deleted.
// ---------------------------------------------------------------------------
//
void CMMACameraWindow::ReleaseUiResources()
{
    if (iUICamera)
    {
        SetViewFinderVisibility(EFalse);
        // iStarterTimer is cancelled by previous call
        // it can be deleted now
        delete iStarterTimer;
        iStarterTimer = NULL;
        iUICamera->Release();
        delete iUICamera;
        iUICamera = NULL;
        iCameraPowerOn = EFalse;
        iDirectAccess->Cancel();
        delete iDirectAccess;
        iDirectAccess = NULL;
        delete iErrorIconBitmap;
        iErrorIconBitmap = NULL;
        delete iErrorIconMaskBitmap;
        iErrorIconMaskBitmap = NULL;
    }
}


// ---------------------------------------------------------------------------
// CMMACameraWindow::ResetViewFinder
// Resets (stops and starts) viewfinder
// ---------------------------------------------------------------------------
//
void CMMACameraWindow::ResetViewFinder()
{

    if (iStarterTimer && !iStarterTimer->IsActive() && iStarted)
    {
        SetViewFinderVisibility(EFalse);
        SetViewFinderVisibility(ETrue);
    }
}


// ---------------------------------------------------------------------------
// CMMACameraWindow::Destroy
// Deletes this object.
// ---------------------------------------------------------------------------
//
void CMMACameraWindow::Destroy()
{
    // Delete itself
    delete this;
}


// ---------------------------------------------------------------------------
// From class MUiEventConsumer.
// CMMACameraWindow::MdcDSAResourcesCallback
// Callback to iDisplay->UIGetDSAResources().
// Invoked asynchronously from UI Event Server thread.
// ---------------------------------------------------------------------------
//
/*void CMMACameraWindow::MdcDSAResourcesCallback(
    RWsSession &aWs,
    CWsScreenDevice &aScreenDevice,
    RWindowBase &aWindow)
{
    TRAPD(error, iDirectAccess = CDirectScreenAccess::NewL(aWs,
                                 aScreenDevice,
                                 aWindow,
                                 *this));
    LOG1(EJavaMMAPI,EInfo,"MMA::CMMACameraWindow::MdcDSAResourcesCallback, error = %d", error);
    if (KErrNone != error)
    {
        return;
    }
    this->UIStartViewFinder(aWs, aScreenDevice, aWindow);
}
*/

// ---------------------------------------------------------------------------
// From class MUiEventConsumer.
// CMMACameraWindow::MdcUICallback
// Callback to iDisplay->UIGetCallback().
// Invoked asynchronously from UI Event Server thread.
// ---------------------------------------------------------------------------
//

void CMMACameraWindow::UICallback(TInt aCallbackId)
{
    LOG(EJavaMMAPI,EInfo,"MMA::CMMACameraWindow::MdcUICallback");

    switch (aCallbackId)
    {
    case EDeleteViewFinder:
        ReleaseUiResources();
        break;
    case EHideViewFinder:
        SetViewFinderVisibility(EFalse);
        break;
    case EShowViewFinder:
        SetViewFinderVisibility(ETrue);
        break;
    case EResetViewFinder:
        ResetViewFinder();
        break;
    case EDestroyWindow:
        Destroy();
        break;
    }
}


//  END OF FILE
