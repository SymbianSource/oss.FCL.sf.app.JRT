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
* Description:  This abstract class implements MMMADisplayWindow functionality
*                in CFbsBitmap based displays.
*
*/


//  Include Files
#include <jdebug.h>
#include <bitdev.h>
#include "cmmaanimationwindow.h"

// Destructor (virtual by CBase)
CMMAAnimationWindow::~CMMAAnimationWindow()
{
    delete iBitContext;
}

CMMAAnimationWindow* CMMAAnimationWindow::NewL(
    CMMAEventSource* aEventSource)
{
    CMMAAnimationWindow* self =
        new(ELeave)CMMAAnimationWindow(aEventSource);
    return self;
}

CMMAAnimationWindow::CMMAAnimationWindow(CMMAEventSource* aEventSource):
        iEventSource(aEventSource)
{
}

void CMMAAnimationWindow::SetViewer(MIHLImageViewer* aViewer)
{
    iViewer = aViewer;
}

void CMMAAnimationWindow::SetDestinationBitmapL(CFbsBitmap* aBitmap)
{
    CMMABitmapWindow::SetDestinationBitmapL(aBitmap);
    User::LeaveIfError(iBitmapDevice->CreateBitmapContext(iBitContext));
}

void CMMAAnimationWindow::DrawFrameL(const CFbsBitmap* aBitmap)
{
    if (iBitmap)
    {
        iBitContext->BitBlt(iDrawRect.iTl, aBitmap);
    }
}

// Local wrapper function to SetDrawRect method
LOCAL_C void StaticSetDrawRect(
    CMMAAnimationWindow* aWindow,
    const TRect* aRect)
{
    aWindow->SetDrawRect(*aRect);
}

void CMMAAnimationWindow::SetDrawRect(const TRect& aRect)
{
    iDrawRect = aRect;
    if (iViewer)
    {
        TReal zoomRatio = 1.0;
        if ((iViewer->SourceSize().iWidth - aRect.Width()) >
                (iViewer->SourceSize().iHeight - aRect.Height()))
        {
            // calculate zoom ratio from width
            zoomRatio = (TReal)aRect.Width() / (TReal)iViewer->SourceSize().iWidth;
        }
        else
        {
            // calculate zoom ratio from height
            zoomRatio = (TReal)aRect.Height() / (TReal)iViewer->SourceSize().iHeight;
        }

        iViewer->SetViewerSize(iDrawRect.Size());
        iViewer->SetZoomRatio(zoomRatio);
    }
}

void CMMAAnimationWindow::SetDrawRectThread(const TRect& aRect)
{
    iEventSource->ExecuteV(&StaticSetDrawRect,
                           this,
                           &aRect);
}

//  END OF FILE
