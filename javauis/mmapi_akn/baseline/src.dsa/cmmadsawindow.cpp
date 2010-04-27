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
* Description:  This class implements MMMADisplayWindow functionality
*                in Direct Screen Access based displays for Helix engine.
*
*/


//  Include Files
#include <jdebug.h>
#include "cmmadsawindow.h"
#include "cmmaplayer.h"

CMMADSAWindow* CMMADSAWindow::NewL(
    CMMAEventSource* aEventSource,
    CMMAPlayer* aPlayer,
    RMMFVideoPlayControllerCustomCommands* aVPCustomCommand)
{
    CMMADSAWindow* self =
        new(ELeave) CMMADSAWindow(aEventSource,
                                  aPlayer,
                                  aVPCustomCommand);
    CleanupStack::PushL(self);
    self->ConstructL();
    CleanupStack::Pop();
    return self;
}

// Destructor (virtual by CBase)
CMMADSAWindow::~CMMADSAWindow()
{
    DEBUG("CMMADSAWindow::~CMMADSAWindow");
}

CMMADSAWindow::CMMADSAWindow(
    CMMAEventSource* aEventSource,
    CMMAPlayer* aPlayer,
    RMMFVideoPlayControllerCustomCommands* aVPCustomCommand)
        : iEventSource(aEventSource), iPlayer(aPlayer),
        iVideoPlayControllerCustomCommands(aVPCustomCommand),
        iDSAAborted(EFalse)
{
    // Nothing to do.
}

void CMMADSAWindow::ConstructL()
{
    iClientRect.SetRect(0, 0, 0, 0);

    // Empty rect until video size is known
    iDrawRect.SetRect(0, 0, 0, 0);
}


void CMMADSAWindow::SetDestinationBitmapL(CFbsBitmap* /*aBitmap*/)
{
    // Ignored, this window will not be used for actual drawing
}

void CMMADSAWindow::DrawFrameL(const CFbsBitmap* /*aBitmap*/)
{
    // Ignored, this window will not be used for actual drawing
}

// Local wrapper function to SetDrawRect method
LOCAL_C void StaticSetDrawRect(
    CMMADSAWindow* aWindow,
    const TRect* aRect,
    CMMAEventSource* aEventSource,
    CMMAPlayer* aPlayer)
{
    RPointerArray< CMMAPlayer > players = aEventSource->Players();
    // Invoke SetDrawRect only if the player has not been cleaned
    if (players.Find(aPlayer) != KErrNotFound)
    {
        aWindow->SetDrawRect(*aRect);
    }
}

void CMMADSAWindow::SetDrawRectThread(const TRect& aRect)
{
    // Call through event source changes thread to MMA event source thread
    // and new rect can be set with RMMFVideoPlayControllerCustomCommands
    // MMA player may be deleted after ExecuteV and before the static method
    // is called so Event source is used to check if a cleanup has
    // already been done for the player (for example due to MIDlet exit)
    iEventSource->ExecuteV(&StaticSetDrawRect,
                           this,
                           &aRect,
                           iEventSource,
                           iPlayer);
}

void CMMADSAWindow::SetDrawRect(const TRect& aRect)
{
    DEBUG_INT2("MID::CMMADSAWindow::SetDrawRect TL %d %d",
               aRect.iTl.iX, aRect.iTl.iY);
    DEBUG_INT2("MID::CMMADSAWindow::SetDrawRect BR %d %d",
               aRect.iBr.iX, aRect.iBr.iY);

    // setting video draw rect and adjusting it to window
    iDrawRect = aRect;
    TRect drawRect = iDrawRect;
    drawRect.Move(iClientRect.iTl);

    if (!iVideoPlayControllerCustomCommands)
    {
        DEBUG("MID::CMMADSAWindow::SetDrawRect: no VideoPlayControllerCustomCommands set, aborting -");
        return;
    }

    if (iVisible)
    {
        DEBUG("MID::CMMADSAWindow::SetDrawRect: Abort DSA");
        iVideoPlayControllerCustomCommands->DirectScreenAccessEvent(EAbortDSA);
    }

    DEBUG_INT2("MID::CMMADSAWindow::SetDrawRect: drawRect TL %d %d",
               drawRect.iTl.iX, drawRect.iTl.iY);
    DEBUG_INT2("MID::CMMADSAWindow::SetDrawRect: drawRect BR %d %d",
               drawRect.iBr.iX, drawRect.iBr.iY);

    // Area where should be drawn is the intersection of drawRect and iClientRect.
    TRect areaRect = iClientRect;
    areaRect.Intersection(drawRect);

    DEBUG_INT2("MID::CMMADSAWindow::SetDrawRect: areaRect TL %d %d",
               areaRect.iTl.iX, areaRect.iTl.iY);
    DEBUG_INT2("MID::CMMADSAWindow::SetDrawRect: areaRect BR %d %d",
               areaRect.iBr.iX, areaRect.iBr.iY);

    if (iVisible && !iDSAAborted)
    {
        DEBUG("MID::CMMADSAWindow::SetDrawRect: SetDisplayWindow( drawRect, areaRect )");
        iVideoPlayControllerCustomCommands->SetDisplayWindow(
            drawRect , areaRect);

        DEBUG("MID::CMMADSAWindow::SetDrawRect: Update region (areaRect)");
        RRegion region(areaRect);
        iVideoPlayControllerCustomCommands->UpdateDisplayRegion(region);
        region.Close();

        DEBUG("MID::CMMADSAWindow::SetDrawRect: Resume DSA");
        iVideoPlayControllerCustomCommands->DirectScreenAccessEvent(EResumeDSA);
    }
    DEBUG("MID::CMMADSAWindow::SetDrawRect: -");
}

const TRect& CMMADSAWindow::DrawRect()
{
    return iDrawRect;
}

TSize CMMADSAWindow::WindowSize()
{
    return iClientRect.Size();
}

void CMMADSAWindow::SetPosition(const TPoint& aPosition)
{
    // changing to MMA thread
    // MMA player may be deleted after ExecuteV and before the static method
    // is called so Event source is used to check if a cleanup has
    // already been done for the player (for example due to MIDlet exit)
    iEventSource->ExecuteV(&CMMADSAWindow::StaticSetWindowPosition,
                           this,
                           aPosition,
                           iEventSource,
                           iPlayer);
}

void CMMADSAWindow::StaticSetWindowPosition(
    CMMADSAWindow* aWindow,
    TPoint aPosition,
    CMMAEventSource* aEventSource,
    CMMAPlayer* aPlayer)
{
    RPointerArray< CMMAPlayer > players = aEventSource->Players();
    // Invoke SetDrawRect only if the player has not been cleaned
    if (players.Find(aPlayer) != KErrNotFound)
    {
        aWindow->SetDrawRect(TRect(aPosition, aWindow->iDrawRect.Size()));
    }
}

TBool CMMADSAWindow::IsVisible() const
{
    DEBUG_INT("MID::CMMADSAWindow::IsVisible %d ", iVisible);
    return iVisible;
}

void CMMADSAWindow::SetVisible(TBool aVisible, TBool aUseEventServer)
{
    DEBUG_INT2("MID::CMMADSAWindow::SetVisible avisible=%d useES=%d +", aVisible, aUseEventServer);
    // Event server may not be used if we already in mma event server thread
    if (aUseEventServer)
    {
        // MMA player may be deleted after ExecuteV and before the static method
        // is called so Event source is used to check if a cleanup has
        // already been done for the player (for example due to MIDlet exit)
        iEventSource->ExecuteV(&CMMADSAWindow::SetWindowVisible,
                               this,
                               aVisible,
                               iEventSource,
                               iPlayer);
    }
    else
    {
        SetWindowVisible(this, aVisible, iEventSource, iPlayer);
    }
    DEBUG_INT("MID::CMMADSAWindow::SetVisible %d -", aVisible);
}

void CMMADSAWindow::SetWindowVisible(
    CMMADSAWindow* aWindow,
    TBool aVisible,
    CMMAEventSource* aEventSource,
    CMMAPlayer* aPlayer)
{
    DEBUG_INT("MID::CMMADSAWindow::SetWindowVisible aVisible %d", aVisible);
    RPointerArray< CMMAPlayer > players = aEventSource->Players();
    // Invoke SetDrawRect only if the player has not been cleaned
    if (players.Find(aPlayer) != KErrNotFound)
    {
        DEBUG("MID::CMMADSAWindow::SetWindowVisible : Player found");
        if (aVisible != aWindow->iVisible)
        {
            DEBUG("MID::CMMADSAWindow::SetWindowVisible: Changed visibility");
            aWindow->iVisible = aVisible;
        }
        if (aVisible)
        {
            DEBUG("MID::CMMADSAWindow::SetWindowVisible: aVisible = 1");
            const TRect drawRect = aWindow->DrawRect();

            aWindow->SetDrawRect(drawRect);
        }
        else
        {
            DEBUG("MID::CMMADSAWindow::SetWindowVisible: aVisible = 0");
            TRect emptyRect(0, 0, 0, 0);

            RRegion region(emptyRect);
            aWindow->iVideoPlayControllerCustomCommands->UpdateDisplayRegion(region);
            region.Close();
            DEBUG("MID::CMMADSAWindow::SetWindowVisible: Abort DSA");

            aWindow->iVideoPlayControllerCustomCommands->
            DirectScreenAccessEvent(EAbortDSA);
        }

    }
}

void CMMADSAWindow::SetWindowRect(const TRect& aRect,MMMADisplay::TThreadType /*aThreadType*/)
{
    DEBUG_INT2("MID::CMMADSAWindow::SetWindowRect aRect TL %d %d",
               aRect.iTl.iX, aRect.iTl.iY);
    DEBUG_INT2("MID::CMMADSAWindow::SetWindowRect aRect BR %d %d",
               aRect.iBr.iX, aRect.iBr.iY);

    iClientRect = aRect;
}

const TRect& CMMADSAWindow::WindowRect()
{
    return iClientRect;
}


void CMMADSAWindow::AbortDSA()
{
    iDSAAborted = ETrue;

    // Always runs in UI thread
    iEventSource->ExecuteV(&StaticAbortDSA,
                           this,
                           iEventSource,
                           iPlayer);
}


void CMMADSAWindow::ResumeDSA()
{
    iDSAAborted = EFalse;

    // Always runs in UI thread
    iEventSource->ExecuteV(&StaticResumeDSA,
                           this,
                           iEventSource,
                           iPlayer);
}


void CMMADSAWindow::StaticAbortDSA(
    CMMADSAWindow* aWindow,
    CMMAEventSource* aEventSource,
    CMMAPlayer* aPlayer)
{
    RPointerArray< CMMAPlayer > players = aEventSource->Players();
    // Invoke SetDrawRect only if the player has not been cleaned
    if (players.Find(aPlayer) != KErrNotFound &&
            aWindow->iVisible && aWindow->iVideoPlayControllerCustomCommands)
    {
        TRect emptyRect(0, 0, 0, 0);

        RRegion region(emptyRect);
        aWindow->iVideoPlayControllerCustomCommands->UpdateDisplayRegion(region);
        region.Close();
        aWindow->iVideoPlayControllerCustomCommands->DirectScreenAccessEvent(EAbortDSA);
    }
}


void CMMADSAWindow::StaticResumeDSA(
    CMMADSAWindow* aWindow,
    CMMAEventSource* aEventSource,
    CMMAPlayer* aPlayer)
{
    RPointerArray< CMMAPlayer > players = aEventSource->Players();
    // Invoke SetDrawRect only if the player has not been cleaned
    if (players.Find(aPlayer) != KErrNotFound &&
            aWindow->iVisible && aWindow->iVideoPlayControllerCustomCommands)
    {
        TRect drawRect = aWindow->iDrawRect;
        drawRect.Move(aWindow->iClientRect.iTl);

        // Area where should be drawn is the intersection of drawRect and iClientRect.
        TRect areaRect = aWindow->iClientRect;
        areaRect.Intersection(drawRect);

        aWindow->iVideoPlayControllerCustomCommands->SetDisplayWindow(
            drawRect , areaRect);

        RRegion region(areaRect);
        aWindow->iVideoPlayControllerCustomCommands->UpdateDisplayRegion(region);
        region.Close();

        aWindow->iVideoPlayControllerCustomCommands->DirectScreenAccessEvent(EResumeDSA);
    }
}

void CMMADSAWindow::ContainerDestroyed()
{
    SetVisible(EFalse, ETrue);
}

//  END OF FILE
