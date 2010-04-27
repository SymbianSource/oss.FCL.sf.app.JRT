/*
* Copyright (c) 2005 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:
*
*/


#include <coemain.h>
#include <coecntrl.h>
#include <reflcdui.h>
#include <hal.h>

#include "LcdWindowSurface.h"

/**
 * Utility function to query the frame buffer attributes for screen aScreenNo
 * when set to displayMode aMode.
 */
LOCAL_C TInt GetScreenInfo(TAcceleratedBitmapInfo& aScreen, TInt aScreenNo, TDisplayMode aMode);

CLcdWindowSurface* CLcdWindowSurface::NewL(CCoeControl& aWindow, MDirectContainer& aContainer, TBool aUpdate)
{
    CLcdWindowSurface* surface = new(ELeave) CLcdWindowSurface(aContainer, aUpdate);
    CleanupStack::PushL(surface);
    surface->ConstructL(aWindow);
    CleanupStack::Pop(surface);
    return surface;
}

CLcdWindowSurface::CLcdWindowSurface(MDirectContainer& aContainer, TBool aUpdate)
        : iContainer(&aContainer)
        , iWsSession(CCoeEnv::Static()->WsSession())
        , iWsDevice(*CCoeEnv::Static()->ScreenDevice())
        , iUpdate(aUpdate)
{
    ASSERT(iContainer);
    //
    // Risk of configuration problem if screen mode does not match the configured
    // primary mode. In that case it would not be possible to use any of the
    // transforming blitters and a frame buffer bitmap would have to be used.
    //
    // Solution is to provide opaque converter blitters from/to all target modes,
    // or generic blitter (as BITGDI) via XRGB8888.
    //
    iImageType.iColorMode = iWsDevice.DisplayMode();
    iImageType.iAlphaMode = ENone;
    iImageType.iTransparency = ETransparencyNone;
}

CLcdWindowSurface::~CLcdWindowSurface()
{
    if (iContainer)
    {
        iContainer->MdcRemoveContent(this);
    }
    Deactivate();
    delete iWsContext;
    iEmptyRegion.Close();
    iUpdateRegion.Close();
}

void CLcdWindowSurface::ConstructL(CCoeControl& aWindow)
{
    iContainer->MdcAddContent(this);
    User::LeaveIfError(iWsDevice.CreateContext(iWsContext));
    if (aWindow.IsBackedUp())
    {
        User::Leave(KErrArgument);
    }
    else
    {
        ActivateL((RWindow*)aWindow.DrawableWindow());
    }
    TRect bounds(aWindow.PositionRelativeToScreen(), aWindow.Size());
    iBounds = bounds;
    iVisible = aWindow.IsVisible();
}

CLcdSurface::TSurfaceType CLcdWindowSurface::Type() const
{
    return CLcdSurface::EWindowSurface;
}

void CLcdWindowSurface::Update(const TRect& aRect)
{
    if (iStarted && iUpdate && iVisible)
    {
        ASSERT(iWsAccess);

        iUpdateRegion.AddRect(aRect);

        (void) DoUpdate();
    }
}

TInt CLcdWindowSurface::DoUpdate()
{
    CFbsScreenDevice* device = iWsAccess->ScreenDevice();
    RRegion& visibleRegion = *iWsAccess->DrawingRegion();

    const TInt updateCount  = iUpdateRegion.Count();
    const TInt visibleCount = visibleRegion.Count();
    if (updateCount && visibleCount)
    {
        iUpdateRegion.Intersect(visibleRegion);
        device->Update(iUpdateRegion);
    }
    iUpdateRegion.Clear();

    return updateCount;
}

void CLcdWindowSurface::Begin(TAcceleratedBitmapInfo& aBitmap, TBitmapLockCount& /*aCount*/)
{
    if (!iStarted && iEnabled)
    {
        iStarted = Start();
    }

#ifdef _DEBUG
    if (iStarted)
    {
        ASSERT(NULL != iScreen.iAddress);
    }
    else
    {
        ASSERT(NULL == iScreen.iAddress);
    }
#endif

    aBitmap = iScreen;
}

void CLcdWindowSurface::End(TBitmapLockCount& /*aCount*/)
{
}

TRect CLcdWindowSurface::Bounds() const
{
    return iBounds;
}

RRegion* CLcdWindowSurface::VisibleRegion() const
{
    if (iStarted)
    {
        ASSERT(iWsAccess);
        return iWsAccess->DrawingRegion();
    }
    return const_cast<RRegion*>(&iEmptyRegion);
}

CBitmapContext* CLcdWindowSurface::Context() const
{
    CBitmapContext* context = iWsAccess->Gc();
    if (context)
    {
        return context;
    }
    return iWsContext;
}

TImageType CLcdWindowSurface::ImageType() const
{
    return iImageType;
}

TBool CLcdWindowSurface::Start()
{
    ASSERT(iWsAccess);
    TRAPD(err, iWsAccess->StartL());
    if (KErrNone == err)
    {
        const TInt screenNo = iWsDevice.GetScreenNumber();

        //
        // DisplayMode may change dynamically on some devices.
        //
        const TDisplayMode displayMode = iWsDevice.DisplayMode();

        if (displayMode != iImageType.iColorMode)
        {
            // this surface doesn't match the current screen mode.
            // Any lcdgd device created for this surface will fail.
            err = KErrNotSupported;
        }
        else
        {
            //
            // Hardware bitmaps handles that reference screen framebuffers
            // start at -1 decending with the correspondance:
            //
            // handle = -(1 + screenNo) == ~screenNo
            //
            RHardwareBitmap screenHwb(~screenNo);

            TAcceleratedBitmapSpec screenSpec(screenHwb);

            err = screenSpec.GetInfo(iScreen);
            if (err)
            {
                // hardware bitmaps screen references may not be supported on
                // all ports. In that case we fall back to querying the HAL
                // in GetScreenInfo().
                //
                err = GetScreenInfo(iScreen, screenNo, displayMode);
            }
        }

        if (err)
        {
            Mem::FillZ(&iScreen, sizeof(iScreen));
            iWsAccess->Cancel();
        }

        //
        // Record current scale, size and rotation settings
        //
        TPixelsAndRotation sizeAndRotation;
        iWsDevice.GetDefaultScreenSizeAndRotation(sizeAndRotation);

        // screen size and orientation determine coordinate transform
        // we should be using during rendering.
        iScreenCoords.iSize        = sizeAndRotation.iPixelSize;
        iScreenCoords.iOrientation = sizeAndRotation.iRotation;
        iScreenCoords.iScale  = iWsDevice.GetCurrentScreenModeScale();
        iScreenCoords.iOrigin = iWsDevice.GetDefaultScreenModeOrigin();
    }

    return (KErrNone == err);
}

void CLcdWindowSurface::ActivateL(RWindow* aWindow)
{
    if (iWsWindow)
    {
        Deactivate();
    }
    iWsWindow = aWindow;
    if (iWsWindow)
    {
        iWsContext->Activate(*iWsWindow);
        iWsAccess = CDirectScreenAccess::NewL(iWsSession, iWsDevice, *iWsWindow, *this);
        iEnabled  = ETrue;
    }
}

void CLcdWindowSurface::Deactivate()
{
    if (iWsWindow && iWsContext)
    {
        iWsContext->Deactivate();
    }
    delete iWsAccess;
    iWsAccess = NULL;
    iStarted  = EFalse;
    iEnabled  = EFalse;
    iWsWindow = NULL;
}

/* CONTENT INTERFACE */

/**
Notify content that container visiblity has changed. The content must not
be displayed when it's container is invisible.
@param "aVisible" ETrue if the container is visible, EFalse if it is invisible.
*/
void CLcdWindowSurface::MdcContainerVisibilityChanged(TBool aVisible)
{
    iVisible = aVisible;
}

/**
Notify content that the container bounds have changed.
@param "aRect" Maximum area the content can occupy in screen co-ordinates.
The content should be clipped to this area.
*/
void CLcdWindowSurface::MdcContentBoundsChanged(const TRect& aRect)
{
    iBounds = aRect;
}

/**
Notify content that its display area has changed.
This should only be called when the content is displayed on an Item.
@param "aDisplayRect" Display area in Item co-ordinates.
@param "aScreenRect" Display area in screen co-ordinates.
*/
void CLcdWindowSurface::MdcItemContentRectChanged(const TRect& /*aContentRect*/,const TRect& /*aScreenRect*/)
{
    ASSERT(EFalse);
}

void CLcdWindowSurface::MdcContainerDestroyed()
{
    Deactivate();
    iContainer = NULL;
}


void CLcdWindowSurface::MdcAbortDSA()
{
    // Empty implementation.
    // This DSA event is delivered via AbortNow.
}


void CLcdWindowSurface::MdcResumeDSA()
{
    // Empty implementation.
    // This DSA event is delivered via Restart.
}


/**
 * MDirectScreenAccess
 */
void CLcdWindowSurface::AbortNow(RDirectScreenAccess::TTerminationReasons /*aReasons*/)
{
    iWsAccess->Cancel();
    iStarted=EFalse;
    iEnabled=EFalse;
}

void CLcdWindowSurface::Restart(RDirectScreenAccess::TTerminationReasons /*aReasons*/)
{
    //
    // Don't bother restarting until next Begin comes through.
    //
    iEnabled=ETrue;
}

RWindow* CLcdWindowSurface::Window() const
{
    return iWsWindow;
}

void CLcdWindowSurface::GetCoordinateSystem(TCoordinateSystem& aCoords) const
{
    aCoords = iScreenCoords;
}

LOCAL_C TUint Log2(TInt aVal)
{
    ASSERT(aVal > 0);
    TUint v = 1;
    TInt  x = 0;
    while (v < aVal)
    {
        v <<= 1;
        ++x;
    }
    return x;
}

LOCAL_C TInt GetScreenInfo(TAcceleratedBitmapInfo& aScreen, TInt aScreenNo, TDisplayMode aMode)
{
    // 'current mode' attributes. These should not need the
    // displayMode but this could depend on video driver.
    //
    TInt err     = 0;
    TInt mode    = 0;
    TInt address = 0;
    TInt width   = 0;
    TInt height  = 0;

    // 'request mode' attributes. These need the displayMode
    // passed in the attribute value argument.
    TInt offset     = aMode;
    TInt linePitch  = aMode;
    TInt bpp        = aMode;

    err = HAL::Get(aScreenNo, HALData::EDisplayMode, mode);
    ASSERT(mode == (TInt)aMode);

    if (KErrNone == err)
    {
        err = HAL::Get(aScreenNo, HALData::EDisplayMemoryAddress, address);
    }

    if (KErrNone == err)
    {
        err = HAL::Get(aScreenNo, HALData::EDisplayXPixels, width);
    }

    if (KErrNone == err)
    {
        err = HAL::Get(aScreenNo, HALData::EDisplayYPixels, height);
    }

    if (KErrNone == err)
    {
        offset = mode;  // Parameter must be set to mode on input.
        err = HAL::Get(aScreenNo, HALData::EDisplayOffsetToFirstPixel, offset);
    }

    if (KErrNone == err)
    {
        linePitch = mode;   // Parameter must be set to mode on input.
        err = HAL::Get(aScreenNo, HALData::EDisplayOffsetBetweenLines, linePitch);
    }

    if (KErrNone == err)
    {
        bpp = mode; // Parameter must be set to mode on input.
        err = HAL::Get(aScreenNo, HALData::EDisplayBitsPerPixel, bpp);
    }

    aScreen.iPhysicalAddress = NULL;
    aScreen.iDisplayMode  = aMode;
    aScreen.iAddress      = reinterpret_cast<TUint8*>(address + offset);
    aScreen.iLinePitch    = linePitch;
    aScreen.iSize.iWidth  = width;
    aScreen.iSize.iHeight = height;
    aScreen.iPixelShift   = Log2(bpp);

    return err;
}
