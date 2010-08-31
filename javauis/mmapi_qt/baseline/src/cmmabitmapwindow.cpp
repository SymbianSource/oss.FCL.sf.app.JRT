/*
* Copyright (c) 2002 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  This abstract class implements MMMADisplayWindow functionality
*                in CFbsBitmap based displays.
*
*/


//  Include Files
#include <logger.h>
#include <bitdev.h>
#include <AknIconUtils.h>
#include "cmmabitmapwindow.h"

// Destructor (virtual by CBase)
CMMABitmapWindow::~CMMABitmapWindow()
{
    delete iBitmap;
    delete iBitmapDevice;
    delete iBitmapContext;
}

CMMABitmapWindow* CMMABitmapWindow::NewL()
{
    CMMABitmapWindow* self = new(ELeave)CMMABitmapWindow();
    return self;
}

CMMABitmapWindow::CMMABitmapWindow():
        iDrawRect(0, 0, 0, 0),
        iClientRect(0, 0, 0, 0)
{
}

void CMMABitmapWindow::SetDestinationBitmapL(CFbsBitmap* aBitmap)
{
    CFbsBitmap* bitmap = new(ELeave)CFbsBitmap();
    CleanupStack::PushL(bitmap);
    User::LeaveIfError(bitmap->Duplicate(aBitmap->Handle()));

    // create context for bitmap
    CFbsBitmapDevice* bitmapDevice = CFbsBitmapDevice::NewL(aBitmap);
    CleanupStack::PushL(bitmapDevice);


    CGraphicsContext* bitmapContext = NULL;
    User::LeaveIfError(bitmapDevice->CreateContext(bitmapContext));

    CleanupStack::Pop(bitmapDevice);   // bitmapDevice
    CleanupStack::Pop(bitmap);   // bitmap

    delete iBitmap;
    iBitmap = bitmap;
    delete iBitmapDevice;
    iBitmapDevice = bitmapDevice;
    delete iBitmapContext;
    iBitmapContext = bitmapContext;

    if (iDrawRect.IsEmpty())
    {
        iDrawRect.SetSize(iBitmap->SizeInPixels());
    }
}

void CMMABitmapWindow::DrawFrameL(const CFbsBitmap* aBitmap)
{
    if (iBitmap)
    {
        // leave in this function will panic thread
        CFbsBitmap* bitmap = new(ELeave)CFbsBitmap();
        CleanupStack::PushL(bitmap);
        User::LeaveIfError(bitmap->Duplicate(aBitmap->Handle()));
        // set incoming bitmap display mode to 16MA
        if (EColor16MU == bitmap->DisplayMode())
        {
            bitmap->SetDisplayMode(EColor16MA);
        }
        AknIconUtils::ScaleBitmapL(iDrawRect, iBitmap, bitmap);
        CleanupStack::PopAndDestroy(bitmap);
    }
}

void CMMABitmapWindow::SetDrawRect(const TRect& aRect)
{
    iDrawRect = aRect;
}

void CMMABitmapWindow::SetDrawRectThread(const TRect& aRect)
{
    // Bitmap window's rect can be set in any thread.
    SetDrawRect(aRect);
}

const TRect& CMMABitmapWindow::DrawRect()
{
    return iDrawRect;
}

TSize CMMABitmapWindow::WindowSize()
{
    if (!iBitmap)
    {
        // bitmap not ready returning currently set draw rect
        return iDrawRect.Size();
    }
    return iBitmap->SizeInPixels();
}

void CMMABitmapWindow::SetPosition(const TPoint& /*aPosition*/)
{
    // ignored, this is done by framework
}

void CMMABitmapWindow::SetVisible(TBool /*aVisible*/, TBool /*aUseEventServer*/)
{
    // ignored, this is done by framework
}

void CMMABitmapWindow::SetWindowRect(const TRect& aRect ,MMMADisplay::TThreadType /*aThreadType*/)
{
    iClientRect = aRect;
}

const TRect& CMMABitmapWindow::WindowRect()
{
    return iClientRect;
}

//  END OF FILE
