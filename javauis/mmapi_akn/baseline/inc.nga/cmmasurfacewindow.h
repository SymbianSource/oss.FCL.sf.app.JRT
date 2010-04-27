/*
* Copyright (c) 2002-2007 Nokia Corporation and/or its subsidiary(-ies).
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
*               in graphics surface based displays for Helix engine.
*
*/

#ifndef CMMASURFACEWINDOW_H
#define CMMASURFACEWINDOW_H


//  INCLUDES
#include <w32std.h>
#include <reflcdui.h>
#include <graphics/surface.h>
#include <mmf/common/mmfvideosurfacecustomcommands.h>
#include <mediaclientvideodisplay.h>

#include <e32std.h>
#include <mmf/common/mmfstandardcustomcommands.h>
#include "mmmadisplaywindow.h"
#include "cmmaeventsource.h"
#include "cmmaplayer.h"

//  FORWARD DECLARATIONS
class RWsSession;
class CWsScreenDevice;
class RWindowBase;
class MMMADisplay;

//  CONSTANTS
const TInt KMMAVideoMinDimension = 32; // minimum video width and height

//  CLASS DECLARATION
/**
* This class implements MMMADisplayWindow functionality
* in video display to graphics surface based displays for Helix engine.
*/
NONSHARABLE_CLASS(CMMASurfaceWindow): public CBase,
        public MMMADisplayWindow,
        public MUiEventConsumer
{
public:
    /**
     * indicates what method needs to be called in UI thread context.
     */
    enum TUiCallbackType
    {
        ESetClipRect = 1,
        ERemoveSurface,
        ESetDrawRect,
        EInitVideoDisplay,
        EResetSurfaceParameters,
        ESetChangedSurfaceParameters,
        ECleanVideoDisplay,
        EDestroyWindow
    };

private:
    /**
     * indicates different video display initialization states
     *
     * video display state transition can happen in following order:
     * 1. when MIDlet code is:
     * Player player = Manager.createPlayer(..);
     * player.realize();
     * VideoControl vc = (VideoControl)player.getControl("VideoControl");
     * vc.initDisplayMode(..);
     * player.start();
     *
     * EUIResourcesAndSurfaceParametersNotSet
     * EUIResourcesSetAndSurfaceParametersNotSet
     * EUIResourcesAndSurfaceParametersSet
     *
     *          OR
     *
     * 2. when MIDlet code is:
     * Player player = Manager.createPlayer(..);
     * player.start();
     * VideoControl vc = (VideoControl)player.getControl("VideoControl");
     * vc.initDisplayMode(..);
     *
     * EUIResourcesAndSurfaceParametersNotSet
     * ESurfaceParametersSetAndUIResourcesNotSet
     * EUIResourcesAndSurfaceParametersSet
     *
     * InitVideoDisplayL() should be called only after EUIResourcesAndSurfaceParametersSet
     * state is reached. ie both UI resources and surface parameters becomes available.
     *
     * 3. for below mentioned MIDlet code case:
     * Player player = Manager.createPlayer(..);
     * player.start(); // state transition is
     *
     * EUIResourcesAndSurfaceParametersNotSet
     * ESurfaceParametersSetAndUIResourcesNotSet
     * note that InitVideoDisplayL() is not called and video is not displayed in this case.
     */
    enum TVideoDisplayInitState
    {
        /**
         * indicates ui resources and surface parameters are not yet set.
         * UI resources are RWindowBase, CWsScreenDevice and RWsSession.
         * Surface parameters are TSurfaceId, TRect and TVideoAspectRatio.
         */
        EUIResourcesAndSurfaceParametersNotSet = 1,
        /**
         * indicates UI resources are set and surface parameters not yet set.
         * UI resources are set when initDisplayMode() is called and RWindow becomes available.
         */
        EUIResourcesSetAndSurfaceParametersNotSet,
        /**
         * indicates UI resources are not yet set and surface parameters are set.
         * Surface parameters are set when player.start() is called.
         */
        ESurfaceParametersSetAndUIResourcesNotSet,
        /**
         * indicates UI resources and surface parameters are set.
         */
        EUIResourcesAndSurfaceParametersSet
    };

public:   // Constructors and destructors
    static CMMASurfaceWindow* NewL(
        CMMAEventSource* aEventSource,
        CMMAPlayer* aPlayer);

    virtual ~CMMASurfaceWindow();

private:  // Constructors and destructors
    CMMASurfaceWindow(
        CMMAEventSource* aEventSource,
        CMMAPlayer* aPlayer);

public: // Methods derived from MMMADisplayWindow
    void SetDestinationBitmapL(CFbsBitmap* aBitmap);
    void DrawFrameL(const CFbsBitmap* aBitmap);
    void SetDrawRect(const TRect& aRect);
    void SetDrawRectThread(const TRect& aRect);
    const TRect& DrawRect();
    TSize WindowSize();
    void SetPosition(const TPoint& aPosition);
    void SetVisible(TBool aVisible, TBool aUseEventServer = ETrue);
    void SetWindowRect(const TRect& aRect, MMMADisplay::TThreadType aThreadType);
    void SetVideoCropRegion(const TRect& aRect);
    void SetRWindowRect(const TRect& aRect, MMMADisplay::TThreadType aThreadType);
    const TRect& WindowRect();
    void ContainerDestroyed();
    void ContainerSet();

public: // from base class MUiEventConsumer
    void MdcDSAResourcesCallback(RWsSession &aWs,
                                 CWsScreenDevice &aScreenDevice,
                                 RWindowBase &aWindow);
    void MdcUICallback(TInt aCallbackId);

public: // New methods
    TBool IsVisible() const;
    void SetDisplay(MMMADisplay *aDisplay);

    /**
     * copies surface paramaters and intializes video display if
     * RWindow is already set, ie if intDisplayMode already called in midlet.
     * invokes InitVideoDisplayL() in UI thread context.
     *
     * @params aSurfaceId, aCropRect, aPixelAspectRatio surface paramaters.
     */
    void SetSurfaceParameters(const TSurfaceId& aSurfaceId,
                              const TRect& aCropRect,
                              const TVideoAspectRatio& aPixelAspectRatio);

    /**
     * invokes DoRemoveSurface() in UI thread context.
     */
    void RemoveSurface();

    /**
     * updates members variables with new surface parameters and invokes
     * DoSetChangedSurfaceParameters() in UI thread context.
     *
     * @params aSurfaceId, aCropRect, aPixelAspectRatio surface paramaters.
     */
    void SetChangedSurfaceParameters(const TSurfaceId& aSurfaceId,
                                     const TRect& aCropRect,
                                     const TVideoAspectRatio& aPixelAspectRatio);

private: // New methods
    /**
     * starts video rendering to a graphics surface.
     * restarts video rendering to a graphics surafce with changed parameters.
     * This method must always be executed in UI-Thread context.
     */
    void RedrawVideoL();

    /**
     * utility function to invoke RedrawVideoL() function with in a TRAP harness.
     *
     * @param aSurfaceWindow reference to CMMASurfaceWindow instance on which RedrawVideoL()
     * has to be invoked
     * @return TInt Symbian OS error code, KErrNone if no error
     */
    static TInt StaticRedrawVideo(CMMASurfaceWindow& aSurfaceWindow);

    /**
     * creates new instance of CMediaClientVideoDisplay and intializes it with
     * surfaces and windows.
     * This method must always be executed in UI-Thread context.
     */
    void InitVideoDisplayL();

    /**
     * updates CMediaClientVideoDisplay with new RWindow rect value.
     * This method must always be executed in UI-Thread context.
     */
    TInt SetClipRect();

    /**
     * updates CMediaClientVideoDisplay instance with new surface parameters.
     * This method must always be executed in UI-Thread context.
     */
    void DoSetChangedSurfaceParameters();

    /**
     * resets CMediaClientVideoDisplay instance with new surface parameters.
     * This method must always be executed in UI-Thread context.
     */
    void DoResetSurfaceParameters();

    /**
     * Removes Surface from RWindow
     * This method must always be executed in UI-Thread context.
     */
    void DoRemoveSurface();

    /**
     * removes surfaces and windows from CMediaClientVideoDisplay instance
     * and deletes the instance.
     * This method must always be executed in UI-Thread context.
     */
    void CleanVideoDisplay();

    /**
     * Deletes this object.
     *
     * @since  S60 v5.2
     */
    void Destroy();

private:  // Data
    /**
     * drawing area where video is rendered.
     */
    TRect iContentRect;

    /**
         * parent rectangle used for positioning contentRect.
         */
    TRect iParentRect;

    /**
     * Symbian RWindow rect
     */
    TRect iRWindowRect;

    /**
     * not owned, used for switching from UI thread to MMA thread
     */
    CMMAEventSource* iEventSource;

    /**
     * We must depend on player's state because direct screen access may
     * not be resumed before player is started.
     * not owned.
     */
    CMMAPlayer* iPlayer;

    /**
     * crop rectangle of video
     */
    TRect iVideoCropRegion;

    /**
     * owned, used for video display on surface
     * this instance is created & accessed in UI thread only.
     */
    CMediaClientVideoDisplay* iMediaClientVideoDisplay;

    /**
         * Display instance used to invoke UI callbacks.
         * Not owned.
         */
    MMMADisplay* iDisplay;

    /**
     * Window server session used by UI thread.
     * Adjustable and usable from UI thread only.
     * Not owned.
     */
    RWsSession* iWs;

    /**
     * Screen device used by UI thread.
     * Adjustable and usable from UI thread only.
     * Not owned.
     */
    CWsScreenDevice* iScreenDevice;

    /**
     * Window where video is displayed.
     * Adjustable and usable from UI thread only.
     * From UI thread.
     * Not owned.
     */
    RWindowBase* iWindow;

    /**
     * The surface to be created for composition.
     */
    TSurfaceId iSurfaceId;

    /**
         * The dimensions of the crop rectangle, relative to the video image.
         */
    TRect iCropRect;

    /**
         * The pixel aspect ratio to display video picture.
         */
    TVideoAspectRatio iPixelAspectRatio;

    /**
     * indicates the video display initialization state.
     *
     */
    TVideoDisplayInitState iVideoDisplayInitState;

    /**
     * Indicates if content need to be drawn.
     */
    TBool iVisible;

};

#endif // CMMASURFACEWINDOW_H
