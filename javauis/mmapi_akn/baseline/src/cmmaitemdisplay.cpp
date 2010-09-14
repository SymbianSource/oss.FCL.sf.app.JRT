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
* Description:  Bitmap display that draws to the Java CustomItem's bitmap.
*
*/


//  Include Files
#include <jdebug.h>
#include <lcdui.h>

#include "cmmaitemdisplay.h"
#include "cmmabitmapwindow.h"

// CONSTRUCTION
// Static constructor, leaves pointer to cleanup-stack
CMMAItemDisplay* CMMAItemDisplay::NewLC(MMIDCustomItem* aCustomItem)
{
    CMMAItemDisplay* self = new(ELeave) CMMAItemDisplay(aCustomItem);
    CleanupStack::PushL(self);
    self->Construct(&(aCustomItem->DirectContainer()));
    return self;
}


// Destructor (virtual by CBase)
CMMAItemDisplay::~CMMAItemDisplay()
{
}


CMMAItemDisplay::CMMAItemDisplay(MMIDCustomItem* aCustomItem)
        : iItem(aCustomItem)
{
    iVisible = ETrue; // Item is visible at startup
}

void CMMAItemDisplay::SizeChangedL(CMMAItemDisplay* aDisplay,
                                   TInt /*aWidth*/,
                                   TInt /*aHeight*/)
// currently width and height is ignored
// may be used later to layout the image.
{
    if (aDisplay->iWindow)
    {
        CFbsBitmap* bitmap = aDisplay->iItem->FrameBuffer();
        aDisplay->iWindow->SetDestinationBitmapL(bitmap);
    }
}

void CMMAItemDisplay::SetFullScreenL(TBool aFullScreen)
{
    iFullScreen = aFullScreen;
    if (!iWindow)
    {
        return;
    }
    if (aFullScreen)
    {
        // switch to fullscreen
        iWindow->SetDrawRect(ScaleToFullScreen(
                                 iWindow->WindowSize(), iSourceSize));
    }
    else
    {
        // switch to normal screen
        iWindow->SetDrawRect(iUserRect);
    }
}

void CMMAItemDisplay::SetWindowL(MMMADisplayWindow* aWindow)
{
    CMMADisplay::SetWindowL(aWindow);
    if (!iWindow)
    {
        DEBUG("CMMAItemDisplay::SetWindowL: NULL window, returning");
        return;
    }
    iSourceSize = iWindow->WindowSize();
    CFbsBitmap* bitmap = iItem->FrameBuffer();

    if (bitmap)
    {
        iWindow->SetDestinationBitmapL(bitmap);
    }
}

void CMMAItemDisplay::SetDisplayLocationL(const TPoint& /*aPosition*/)
{
    // In item( USE_GUI_PRIMITIVE ) mode, this call will be ignored.
}


TPoint CMMAItemDisplay::DisplayLocation()
{
    // Java Item's location is always 0, 0
    return TPoint(0, 0);
}

void CMMAItemDisplay::SourceSizeChanged(const TSize& aSourceSize)
{
    DEBUG_INT("MMA::CMMAItemDisplay::SourceSizeChanged %d",
              aSourceSize.iWidth);
    DEBUG_INT("MMA::CMMAItemDisplay::SourceSizeChanged %d",
              aSourceSize.iHeight);

#ifdef RD_JAVA_NGA_ENABLED
    if (iWindow)
    {
        TPoint topLeft(0, 0);
        iWindow->SetVideoCropRegion(TRect(topLeft, aSourceSize));
    }
#endif

    iSourceSize = aSourceSize;
    if(IsUserRectSet() || iFullScreen)
    {
        return;
    }
    if (iWindow)
    {
        TRect clientRect(iUserRect.iTl, aSourceSize);

        iWindow->SetDrawRect(clientRect);
        // Setting initial window size if not already set, actual size will
        // be set in MdcItemContentRectChanged()
        if (iWindow->WindowSize() == TSize())
        {
            iWindow->SetWindowRect(clientRect, MMMADisplay::EMmaThread);
        }
    }

    SetClippingRegion();

    if (iUserRect.IsEmpty())
    {
        // Java side hasn't set size.
        iUserRect.SetSize(iSourceSize);
    }
}

void CMMAItemDisplay::StaticSourceSize(CMMAItemDisplay* aDisplay,
                                       TSize* aSize)
{
    *aSize = aDisplay->iUserRect.Size();
}

void CMMAItemDisplay::MdcItemContentRectChanged(const TRect& aContentRect,
        const TRect& aScreenRect)
{
    if (iWindow)
    {
        // Change windows rect.
        iWindow->SetWindowRect(aScreenRect, MMMADisplay::EUiThread);
        TRect drawRect = aContentRect;
        drawRect.Move(- aScreenRect.iTl);
        iWindow->SetDrawRectThread(drawRect);
    }
    SetClippingRegion();
}

void CMMAItemDisplay::MdcContentBoundsChanged(const TRect& /*aRect*/)
{
    // Do nothing in Item display
}

//  END OF FILE
