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
* Description:  This class implements MMMADisplay
*
*/


//  Include Files
#include <jdebug.h>

#include "cmmadisplay.h"
#include "mmmadisplaywindow.h"

// Destructor (virtual by CBase)
CMMADisplay::~CMMADisplay()
{

    if (iDirectContainer)
    {
        // Remove clip rect if set
        if (!iClipRect.IsEmpty())
        {
            DEBUG("CMMADisplay::~CMMADisplay(): Removing clip rect");
            iDirectContainer->MdcRemoveContentBounds(iClipRect);
        }

        // Remove this object from MDirectContainer
        iDirectContainer->MdcRemoveContent(this);
    }
}


CMMADisplay::CMMADisplay():
        iVisible(EFalse),
        iFullScreen(EFalse),
        iContainerVisible(EFalse),
        iIsForeground(ETrue),
        iResetDrawRect(EFalse)
{
}

void CMMADisplay::Construct(MDirectContainer* aDirectContainer)
{
    // Components must have direct content.
    __ASSERT_DEBUG(aDirectContainer, User::Invariant());

    // Not owned
    iDirectContainer = aDirectContainer;

    // Get component visibility. Later visibility changes will
    // be informed through MDirectContent observer.
    iContainerVisible = iDirectContainer->MdcContainerVisibility();
    DEBUG_INT("MID::CMMADisplay::Construct iContainerVisible = %d", iContainerVisible);
    DEBUG_INT("MID::CMMADisplay::Construct iVisible = %d", iVisible);

    // Add this MDirectContent to the MDirectContainer. Cannot fail.
    iDirectContainer->MdcAddContent(this);
}

TRect CMMADisplay::ScaleToFullScreen(const TSize& aFullScreenSize,
                                     const TSize& aSourceSize)
{
    if ((aSourceSize.iWidth == 0) &&
            (aSourceSize.iHeight == 0))
    {
        // Avoid divide by zero.
        return TRect();
    }

    // Width or height will be full size.
    TRect drawRect(TPoint(0, 0),
                   aFullScreenSize);

    // Smaller dimension scale ratio will be scaled.
    // Changed to area calculation to avoid reals and dimension
    // with smaller area will be scaled.
    TInt vDif((aFullScreenSize.iWidth - aSourceSize.iWidth)
              * aFullScreenSize.iHeight);
    TInt hDif((aFullScreenSize.iHeight - aSourceSize.iHeight)
              * aFullScreenSize.iWidth);

    TPoint position(0, 0);

    // Check which side to scale to fullscreen size.
    if (hDif > vDif)
    {
        // Width is full screen width.
        // Scale height with aspect ratio.
        drawRect.iBr.iY = aFullScreenSize.iWidth * aSourceSize.iHeight
                          / aSourceSize.iWidth;
        // move rect middle of the screen
        position.iY = (aFullScreenSize.iHeight - drawRect.iBr.iY) / 2;
    }
    else
    {
        // Height is full screen height.
        // Scale width with aspect ratio.
        drawRect.iBr.iX = aFullScreenSize.iHeight * aSourceSize.iWidth
                          / aSourceSize.iHeight;
        // move rect middle of the screen
        position.iX = (aFullScreenSize.iWidth - drawRect.iBr.iX) / 2;
    }

    drawRect.Move(position);
    return drawRect;
}

// from MMMADisplay
void CMMADisplay::DrawFrameL(const CFbsBitmap* aBitmap)
{
    // This method is called only if bitmap is used.
    // Runs in mmapi thread
    if (iVisible && iWindow && iDirectContainer)
    {
        iWindow->DrawFrameL(aBitmap);
        iDirectContainer->MdcFlushContainer(iWindow->WindowRect());
    }
}

void CMMADisplay::SetClippingRegion()
{
    DEBUG("CMMADisplay::SetClippingRegion");

    if (!iWindow ||
            (iWindow->GetDisplayWindowType() == MMMADisplayWindow::EDisplayWindowTypeIsBitmap) ||
            iClipRect == iWindow->DrawRect() && iContainerVisible && iVisible)
    {
        return;
    }

    TBool refreshScreen(EFalse);
    // Remove first the current clip rect if set
    if (!iClipRect.IsEmpty())
    {
        DEBUG("CMMADisplay::SetClippingRegion: Removing old rect");
        iDirectContainer->MdcRemoveContentBounds(iClipRect);
        iClipRect.SetRect(0, 0, 0, 0);
        refreshScreen = ETrue;
    }
    // If visible then set a new clip rect
    if (iVisible && iContainerVisible)
    {
        iClipRect = iWindow->DrawRect();

        if (!iClipRect.IsEmpty())
        {
            DEBUG("CMMADisplay::SetClippingRegion: Adding new rect");
            // Add new clipping rect
            iDirectContainer->MdcAddContentBounds(iClipRect);
            refreshScreen = ETrue;
        }
    }
    if (refreshScreen)
    {
        // refresh screen
        iDirectContainer->MdcFlushContainer(iWindow->WindowRect());
    }
    DEBUG("CMMADisplay::SetClippingRegion -");
}

void CMMADisplay::RemoveClippingRegion()
{
    // Called in mmapi thread
    DEBUG("CMMADisplay::RemoveClippingRegion");

    if (!iWindow ||
            (iWindow->GetDisplayWindowType() == MMMADisplayWindow::EDisplayWindowTypeIsBitmap))
    {
        return;
    }
    // Remove first the current clip rect if set
    if (!iClipRect.IsEmpty())
    {
        DEBUG("CMMADisplay::RemoveClippingRegion: Removing old rect");
        iDirectContainer->MdcRemoveContentBounds(iClipRect);
        iClipRect.SetRect(0, 0, 0, 0);
        // refresh screen
        iDirectContainer->MdcFlushContainer(iWindow->WindowRect());
    }
}

void CMMADisplay::AddClippingRegion()
{
    // Called in mmapi thread
    DEBUG("CMMADisplay::AddClippingRegion");

    if (!iWindow ||
            (iWindow->GetDisplayWindowType() ==
             MMMADisplayWindow::EDisplayWindowTypeIsBitmap) ||
            iClipRect == iWindow->DrawRect())
    {
        return;
    }
    // If visible then set a new clip rect
    if (iVisible)
    {
        iClipRect = iWindow->DrawRect();

        if (!iClipRect.IsEmpty())
        {
            DEBUG("CMMADisplay::AddClippingRegion: Adding new rect");
            // Add new clipping rect
            iDirectContainer->MdcAddContentBounds(iClipRect);
            // refresh screen
            iDirectContainer->MdcFlushContainer(
                iWindow->WindowRect());
        }
    }
}

// from MMMADisplay
TSize CMMADisplay::DisplaySize()
{
    if (iWindow && iFullScreen)
    {
        return iWindow->DrawRect().Size();
    }
    else
    {
        return iUserRect.Size();
    }
}

// from MMMADisplay
void CMMADisplay::SetDisplaySizeL(const TSize& aSize)
{
    // user rect contains size set from java.
    iUserRect.SetSize(aSize);

    // Size change has no effect if fullscreen mode is on.
    // New size could be used when fullscreen is turned off.
    if (iContainerVisible && !iFullScreen && iWindow)
    {
        RemoveClippingRegion();
        iWindow->SetDrawRect(iUserRect);
        AddClippingRegion();
    }
    else
    {
        iResetDrawRect = ETrue;
    }
}

// from MMMADisplay
void CMMADisplay::SetVisible(TBool aValue)
{
    iVisible = aValue;
    // Window may be set visible only if container is on screen,

    DEBUG_INT("MID::CMMADisplay::SetVisible + iContainerVisible = %d", iContainerVisible);
    DEBUG_INT("MID::CMMADisplay::SetVisible iVisible = %d", iVisible);

    if (!iIsForeground)
    {
        DEBUG("MID::CMMADisplay::SetVisible - iIsForeground = 0");
        return;
    }
    // if not it can not be set visible.
    if (iWindow && iContainerVisible)
    {
        iWindow->SetVisible(aValue, EFalse);
        SetClippingRegion();
    }
}

// from MMMADisplay
void CMMADisplay::SetWindowL(MMMADisplayWindow* aWindow)
{
    // Sets new window. Ownership is not transferred.
    iWindow = aWindow;
}

// from MMMADisplay
MMMADisplayWindow* CMMADisplay::Window()
{
    return iWindow;
}

TBool CMMADisplay::IsVisible()
{
    DEBUG_INT("MID::CMMADisplay::IsVisible iContainerVisible = %d", iContainerVisible);
    DEBUG_INT("MID::CMMADisplay::IsVisible iVisible = %d", iVisible);
    // display is visible if container is on screen and
    // java side has set it visible
    return iVisible && iContainerVisible;
}

TBool CMMADisplay::IsFullScreen()
{
    return iFullScreen;
}

TBool CMMADisplay::HasContainer()
{
    return iDirectContainer != NULL;
}

void CMMADisplay::MdcContainerVisibilityChanged(TBool aVisible)
{
    DEBUG_INT("MID::CMMADisplay::MdcContainerVisibilityChanged aVisible = %d",
              aVisible);
    if (!iIsForeground && aVisible)
    {
        DEBUG("MID::CMMADisplay::MdcContainerVisibilityChanged Condition 1 ");
        return;
    }

    DEBUG("MID::CMMADisplay::MdcContainerVisibilityChanged After condition1");

    iContainerVisible = aVisible;

    if (iWindow)
    {
        // midlet comes to foreground (0 to 1 transition),
        // Allow turn on or turn off based on aVisible
        if (aVisible != iWindow->IsVisible())
        {
            // Allow
            DEBUG("MID::CMMADisplay::MdcContainerVisibilityChanged Allow ");
        }
        else //if( iContainerVisible == aVisible )
        {
            DEBUG("MID::CMMADisplay::MdcContainerVisibilityChanged iContainerVisible == aVisible ");
            // if state is not changed, we do not need to do it again
            return;
        }
    }

    DEBUG("MID::CMMADisplay::MdcContainerVisibilityChanged After condition2");

    if (iWindow)
    {
        DEBUG("MID::CMMADisplay::MdcContainerVisibilityChanged iWindow is valid ");
        // change is only needed if java side has set display visible or
        // if container loses focus
        if (!iContainerVisible || iVisible)
        {
            if (iResetDrawRect && aVisible && !iFullScreen)
            {
                iWindow->SetDrawRectThread(iUserRect);
                iResetDrawRect = EFalse;
            }
            if (iIsForeground)
            {
                iWindow->SetVisible(aVisible);
            }
            SetClippingRegion();
        }
    }
}

void CMMADisplay::MdcItemContentRectChanged(const TRect& /*aContentRect*/,
        const TRect& /*aScreenRect*/)
{
    // To be overwritten if used
    __ASSERT_DEBUG(EFalse, User::Invariant());
}

void CMMADisplay::MdcContainerWindowRectChanged(const TRect&
#ifdef RD_JAVA_NGA_ENABLED
        aRect
#endif
                                               )
{
    DEBUG("CMMADisplay::MdcContainerWindowRectChanged +");

#ifdef RD_JAVA_NGA_ENABLED
    if (iWindow)
    {
        DEBUG("CMMADisplay::MdcContainerWindowRectChanged, SetRWindowRect");
        iWindow->SetRWindowRect(aRect, MMMADisplay::EUiThread);
    }
#endif
}
void CMMADisplay::MdcContainerDestroyed()
{
    DEBUG("MMA::CMMADisplay::MdcContainerDestroyed");

    if (iWindow)
    {
        iWindow->ContainerDestroyed();
    }

    iDirectContainer = NULL;
}


void CMMADisplay::SetForeground(TBool aForeground, TBool aUseEventServer)
{
    DEBUG_INT("MMA::CMMADisplay::SetForeground %d", aForeground);
    iIsForeground = aForeground;

    if (aForeground)
    {
        if (iContainerVisible && !iWindow->IsVisible())
        {
            iWindow->SetVisible(ETrue, aUseEventServer);
        }
    }
    else
    {
        if (iWindow->IsVisible())
        {
            iWindow->SetVisible(EFalse, aUseEventServer);
        }
    }
}

void CMMADisplay::MdcAbortDSA()
{
    if (iWindow)
    {
        iWindow->AbortDSA();
    }
}


void CMMADisplay::MdcResumeDSA()
{
    if (iWindow)
    {
        iWindow->ResumeDSA();
    }
}


void CMMADisplay::UIGetDSAResources(
    MUiEventConsumer& aConsumer,
    MMMADisplay::TThreadType /* aThreadType */)
{
    if (iDirectContainer)
    {
        iDirectContainer->MdcGetDSAResources(aConsumer);
    }
}

void CMMADisplay::UIGetCallback(
    MUiEventConsumer& aConsumer,
    TInt aCallbackId)
{
    if (iDirectContainer)
    {
        iDirectContainer->MdcGetUICallback(aConsumer, aCallbackId);
    }
}

//  END OF FILE
