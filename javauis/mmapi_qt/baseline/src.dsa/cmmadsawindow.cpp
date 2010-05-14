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
#include <logger.h>
#include "cmmadsawindow.h"
#include "cmmaplayer.h"

CMMADSAWindow* CMMADSAWindow::NewL(
    MMAFunctionServer* aEventSource,
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
    LOG( EJavaMMAPI, EInfo, "CMMADSAWindow::~CMMADSAWindow");
}

CMMADSAWindow::CMMADSAWindow(
    MMAFunctionServer* aEventSource,
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
    MMAFunctionServer* aEventSource,
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
    LOG2( EJavaMMAPI, EInfo, "MID::CMMADSAWindow::SetDrawRect TL %d %d",
               aRect.iTl.iX, aRect.iTl.iY);
    LOG2( EJavaMMAPI, EInfo, "MID::CMMADSAWindow::SetDrawRect BR %d %d",
               aRect.iBr.iX, aRect.iBr.iY);

    // setting video draw rect and adjusting it to window
    iDrawRect = aRect;
    TRect drawRect = iDrawRect;
    drawRect.Move(iClientRect.iTl);

    if (!iVideoPlayControllerCustomCommands)
    {
        LOG( EJavaMMAPI, EInfo, "MID::CMMADSAWindow::SetDrawRect: no VideoPlayControllerCustomCommands set, aborting -");
        return;
    }

    if (iVisible)
    {
        LOG( EJavaMMAPI, EInfo, "MID::CMMADSAWindow::SetDrawRect: Abort DSA");
        iVideoPlayControllerCustomCommands->DirectScreenAccessEvent(EAbortDSA);
    }

    LOG2( EJavaMMAPI, EInfo, "MID::CMMADSAWindow::SetDrawRect: drawRect TL %d %d",
               drawRect.iTl.iX, drawRect.iTl.iY);
    LOG2( EJavaMMAPI, EInfo, "MID::CMMADSAWindow::SetDrawRect: drawRect BR %d %d",
               drawRect.iBr.iX, drawRect.iBr.iY);

    // Area where should be drawn is the intersection of drawRect and iClientRect.
    TRect areaRect = iClientRect;
    areaRect.Intersection(drawRect);

    LOG2( EJavaMMAPI, EInfo, "MID::CMMADSAWindow::SetDrawRect: areaRect TL %d %d",
               areaRect.iTl.iX, areaRect.iTl.iY);
    LOG2( EJavaMMAPI, EInfo, "MID::CMMADSAWindow::SetDrawRect: areaRect BR %d %d",
               areaRect.iBr.iX, areaRect.iBr.iY);

    if (iVisible && !iDSAAborted)
    {
        LOG( EJavaMMAPI, EInfo, "MID::CMMADSAWindow::SetDrawRect: SetDisplayWindow( drawRect, areaRect )");
        iVideoPlayControllerCustomCommands->SetDisplayWindow(
            drawRect , areaRect);

        LOG( EJavaMMAPI, EInfo, "MID::CMMADSAWindow::SetDrawRect: Update region (areaRect)");
        RRegion region(areaRect);
        iVideoPlayControllerCustomCommands->UpdateDisplayRegion(region);
        region.Close();

        LOG( EJavaMMAPI, EInfo, "MID::CMMADSAWindow::SetDrawRect: Resume DSA");
        iVideoPlayControllerCustomCommands->DirectScreenAccessEvent(EResumeDSA);
    }
    LOG( EJavaMMAPI, EInfo, "MID::CMMADSAWindow::SetDrawRect: -");
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
    MMAFunctionServer* aEventSource,
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
    LOG1( EJavaMMAPI, EInfo, "MID::CMMADSAWindow::IsVisible %d ", iVisible);
    return iVisible;
}

void CMMADSAWindow::SetVisible(TBool aVisible, TBool aUseEventServer)
{
    LOG2( EJavaMMAPI, EInfo, "MID::CMMADSAWindow::SetVisible avisible=%d useES=%d +", aVisible, aUseEventServer);
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
    LOG1( EJavaMMAPI, EInfo, "MID::CMMADSAWindow::SetVisible %d -", aVisible);
}

void CMMADSAWindow::SetWindowVisible(
    CMMADSAWindow* aWindow,
    TBool aVisible,
    MMAFunctionServer* aEventSource,
    CMMAPlayer* aPlayer)
{
    LOG1( EJavaMMAPI, EInfo, "MID::CMMADSAWindow::SetWindowVisible aVisible %d", aVisible);
    RPointerArray< CMMAPlayer > players = aEventSource->Players();
    // Invoke SetDrawRect only if the player has not been cleaned
    if (players.Find(aPlayer) != KErrNotFound)
    {
        LOG( EJavaMMAPI, EInfo, "MID::CMMADSAWindow::SetWindowVisible : Player found");
        if (aVisible != aWindow->iVisible)
        {
            LOG( EJavaMMAPI, EInfo, "MID::CMMADSAWindow::SetWindowVisible: Changed visibility");
            aWindow->iVisible = aVisible;
        }
        if (aVisible)
        {
            LOG( EJavaMMAPI, EInfo, "MID::CMMADSAWindow::SetWindowVisible: aVisible = 1");
            const TRect drawRect = aWindow->DrawRect();

            aWindow->SetDrawRect(drawRect);
        }
        else
        {
            LOG( EJavaMMAPI, EInfo, "MID::CMMADSAWindow::SetWindowVisible: aVisible = 0");
            TRect emptyRect(0, 0, 0, 0);

            RRegion region(emptyRect);
            aWindow->iVideoPlayControllerCustomCommands->UpdateDisplayRegion(region);
            region.Close();
            LOG( EJavaMMAPI, EInfo, "MID::CMMADSAWindow::SetWindowVisible: Abort DSA");

            aWindow->iVideoPlayControllerCustomCommands->
            DirectScreenAccessEvent(EAbortDSA);
        }

    }
}

void CMMADSAWindow::SetWindowRect(const TRect& aRect,MMMADisplay::TThreadType /*aThreadType*/)
{
    LOG2( EJavaMMAPI, EInfo, "MID::CMMADSAWindow::SetWindowRect aRect TL %d %d",
               aRect.iTl.iX, aRect.iTl.iY);
    LOG2( EJavaMMAPI, EInfo, "MID::CMMADSAWindow::SetWindowRect aRect BR %d %d",
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
    MMAFunctionServer* aEventSource,
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
    MMAFunctionServer* aEventSource,
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
