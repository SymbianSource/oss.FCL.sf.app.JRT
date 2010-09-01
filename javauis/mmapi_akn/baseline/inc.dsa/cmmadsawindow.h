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


#ifndef CMMADSAWINDOW_H
#define CMMADSAWINDOW_H


//  INCLUDES
#include <w32std.h>
#include <e32std.h>
#include <mmf/common/mmfstandardcustomcommands.h>
#include "mmmadisplaywindow.h"
#include "cmmaeventsource.h"
#include "cmmaplayer.h"

//  CONSTANTS
const TInt KMMAVideoMinDimension = 32; // minimum video width and height

//  CLASS DECLARATION
/**
* This class implements MMMADisplayWindow functionality
* in Direct Screen Access based displays for Helix engine.
*/
NONSHARABLE_CLASS(CMMADSAWindow): public CBase,
        public MMMADisplayWindow
{
public:     // Constructors and destructors
    static CMMADSAWindow* NewL(
        CMMAEventSource* aEventSource,
        CMMAPlayer* aPlayer,
        RMMFVideoPlayControllerCustomCommands* aVPCustomCommand);

    virtual ~CMMADSAWindow();

private:  // Constructors and destructors
    CMMADSAWindow(
        CMMAEventSource* aEventSource,
        CMMAPlayer* aPlayer,
        RMMFVideoPlayControllerCustomCommands* aVPCustomCommand);

    void ConstructL();

public: // New methods
    TBool IsVisible() const;

public: // Methods derived from MMMADisplayWindow
    void SetDestinationBitmapL(CFbsBitmap* aBitmap);
    void DrawFrameL(const CFbsBitmap* aBitmap);
    void SetDrawRect(const TRect& aRect);
    void SetDrawRectThread(const TRect& aRect);
    const TRect& DrawRect();
    TSize WindowSize();
    void SetPosition(const TPoint& aPosition);
    void SetVisible(TBool aVisible, TBool aUseEventServer = ETrue);
    void SetWindowRect(const TRect& aRect,MMMADisplay::TThreadType aThreadType);
    const TRect& WindowRect();
    void ContainerDestroyed();
    /**
     * Notifies window that any drawing
     * via direct screen access must be aborted
     */
    void AbortDSA();

    /**
     * Allows window to draw
     * via direct screen access after MdcAbortDSA
     */
    void ResumeDSA();

private:
    static void StaticSetWindowPosition(
        CMMADSAWindow* aWindow,
        TPoint aPosition,
        CMMAEventSource* aEventSource,
        CMMAPlayer* aPlayer);

    static void SetWindowVisible(
        CMMADSAWindow* aWindow,
        TBool aVisible,
        CMMAEventSource* aEventSource,
        CMMAPlayer* aPlayer);

    static void StaticAbortDSA(
        CMMADSAWindow* aWindow,
        CMMAEventSource* aEventSource,
        CMMAPlayer* aPlayer);

    static void StaticResumeDSA(
        CMMADSAWindow* aWindow,
        CMMAEventSource* aEventSource,
        CMMAPlayer* aPlayer);
private:  // Data
    TRect iDrawRect;
    TRect iClientRect;

    /**
     * not owned, used for switching from UI thread to MMA thread
     */
    CMMAEventSource* iEventSource;

    /**
     * We must depend on player's state because direct screen access may
     * not be resumed before player is started.
     */
    CMMAPlayer* iPlayer;

    /**
     * not owned, used for controlling video position and visibility
     */
    RMMFVideoPlayControllerCustomCommands* iVideoPlayControllerCustomCommands;

    /**
     * Indicates if content need to be drawn.
     */
    TBool iVisible;

    /**
     * Indicates if DSA is aborted.
     */
    TBool iDSAAborted;
};

#endif // CMMADSAWINDOW_H
