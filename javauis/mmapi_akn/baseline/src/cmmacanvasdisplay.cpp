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
* Description:  Draws to Canvas.
*
*/


//  Include Files
#include <jdebug.h>
#include <lcdui.h> // MMIDCanvas

#ifdef EXTENDED_LCDUI_CANVAS
#include <MMIDCanvasExtended.h>
#endif

#include "cmmacanvasdisplay.h"
#include "cmmabitmapwindow.h"

// CONSTRUCTION
// Static constructor, leaves pointer to cleanup-stack
CMMACanvasDisplay* CMMACanvasDisplay::NewLC(MMIDCanvas* aCanvas)
{
    CMMACanvasDisplay* self =
        new(ELeave) CMMACanvasDisplay(aCanvas);

    CleanupStack::PushL(self);
    self->Construct(&aCanvas->DirectContainer());
    return self;
}

// Destructor (virtual by CBase)
CMMACanvasDisplay::~CMMACanvasDisplay()
{
}

CMMACanvasDisplay::CMMACanvasDisplay(MMIDCanvas* aCanvas):
        iCanvas(aCanvas)
{
}

void CMMACanvasDisplay::SourceSizeChanged(const TSize& aSourceSize)
{
    DEBUG_INT("MMA::CMMACanvasDisplay::SourceSizeChanged %d",
              aSourceSize.iWidth);
    DEBUG_INT("MMA::CMMACanvasDisplay::SourceSizeChanged %d",
              aSourceSize.iHeight);

    iSourceSize = aSourceSize;
    // If user rect size was set or full screen was set then we need to take
    // that into account then the actual source size
    if(IsUserRectSet() || iFullScreen)
    {
        return;
    }
    TSize fullScreenSize(iCanvas->ContentSize());

    TBool sourceIsBigger = (aSourceSize.iWidth > fullScreenSize.iWidth ||
                            aSourceSize.iHeight > fullScreenSize.iHeight);

    if (iWindow)
    {
#ifdef RD_JAVA_NGA_ENABLED
        TPoint topLeft(0, 0);
        iWindow->SetVideoCropRegion(TRect(topLeft, aSourceSize));
#endif

        if (sourceIsBigger)
        {
            // Source is larger than display area.
            // Shrink draw are to fit in display.
            iWindow->SetDrawRect(ScaleToFullScreen(fullScreenSize, iSourceSize));
        }
        else
        {
            // source is smaller than display area
            iWindow->SetDrawRect(TRect(iUserRect.iTl, iSourceSize));
        }
    }
    SetClippingRegion();

    if (iUserRect.IsEmpty())
    {
        if (iWindow)
        {
            // Java side hasn't set size.
            iUserRect = iWindow->DrawRect();
        }

        if (!sourceIsBigger)
        {
            // Addjusting rect to top left corner.
            iUserRect = TRect(iUserRect.Size());
        }
    }
}


void CMMACanvasDisplay::SetFullScreenL(TBool aFullScreen)
{
    iFullScreen = aFullScreen;
    if (iContainerVisible)
    {
        RemoveClippingRegion();

        if (aFullScreen)
        {
            // use new scaled rect
            iWindow->SetDrawRect(ScaleToFullScreen(
                                     iCanvas->ContentSize(), iSourceSize));
        }
        else
        {
            // use size set from java
            iWindow->SetDrawRect(iUserRect);
        }

        AddClippingRegion();
    }
}

void CMMACanvasDisplay::SetWindowL(MMMADisplayWindow* aWindow)
{
    DEBUG("CMMACanvasDisplay::SetWindowL");
    CMMADisplay::SetWindowL(aWindow);
    if (!iWindow)
    {
        DEBUG("CMMACanvasDisplay::SetWindowL: NULL window, returning");
        return;
    }

    CFbsBitmap* bitmap = iCanvas->FrameBuffer();

    __ASSERT_DEBUG(bitmap,
                   User::Panic(_L("Canvas has no bitmap"),
                               KErrNotFound));

    iWindow->SetDestinationBitmapL(bitmap);

    // Check that container exists
    User::LeaveIfNull(iDirectContainer);

    DEBUG_INT2("MMA::CMMACanvasDisplay::SetWindowL iDirectContainer->MdcContentBounds() TL %d %d", iDirectContainer->MdcContentBounds().iTl.iX, iDirectContainer->MdcContentBounds().iTl.iY);
    DEBUG_INT2("MMA::CMMACanvasDisplay::SetWindowL iDirectContainer->MdcContentBounds() BR %d %d", iDirectContainer->MdcContentBounds().iBr.iX, iDirectContainer->MdcContentBounds().iBr.iY);

    iWindow->SetWindowRect(iDirectContainer->MdcContentBounds(), MMMADisplay::EMmaThread);
#ifdef RD_JAVA_NGA_ENABLED
    iWindow->SetRWindowRect(iDirectContainer->MdcContainerWindowRect(),
                            MMMADisplay::EMmaThread);
#endif
}

void CMMACanvasDisplay::SetDisplayLocationL(const TPoint& aPosition)
{
    // Move iUserRect top left corner to aPosition.
    TSize size(iUserRect.Size());
    iUserRect.iTl = aPosition;
    iUserRect.SetSize(size);

    if (iContainerVisible && !iFullScreen && iWindow)
    {
        iWindow->SetDrawRect(iUserRect);
        SetClippingRegion();
    }
    else
    {
        iResetDrawRect = ETrue;
    }
}

TPoint CMMACanvasDisplay::DisplayLocation()
{
    if (iWindow && iFullScreen)
    {
        return iWindow->DrawRect().iTl;
    }
    else
    {
        return iUserRect.iTl;
    }
}

void CMMACanvasDisplay::MdcContentBoundsChanged(const TRect& aRect)
{
    DEBUG_INT2("MID::CMMACanvasDisplay::MdcContentBoundsChanged aRect TL %d %d",
               aRect.iTl.iX, aRect.iTl.iY);
    DEBUG_INT2("MID::CMMACanvasDisplay::MdcContentBoundsChanged aRect BR %d %d",
               aRect.iBr.iX, aRect.iBr.iY);

    if (iWindow)
    {
        // Set new rect to window.
        iWindow->SetWindowRect(aRect, MMMADisplay::EUiThread);

        if (iFullScreen)
        {
            TRect fullRect = ScaleToFullScreen(iCanvas->ContentSize(),
                                               iSourceSize);

            // use SetDrawRectThread because this code is executed
            // in lcdui thread
            iWindow->SetDrawRectThread(fullRect);
        }
        else
        {
            // move to user defined position.
            iWindow->SetPosition(iUserRect.iTl);
        }
    }
    SetClippingRegion();
}

//  END OF FILE
