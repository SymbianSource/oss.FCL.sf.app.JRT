/*
* Copyright (c) 2009 Nokia Corporation and/or its subsidiary(-ies).
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
*                for Camera viewfinder usage.
*
*/


#ifndef CMMACAMERAWINDOW_H
#define CMMACAMERAWINDOW_H

//  INCLUDES
#include <ecam.h>
#include <reflcdui.h>
#include <w32std.h>
#include "mmmadisplaywindow.h"
#include "mmmadisplay.h"
//  CONSTANTS

//  FORWARD DECLARATIONS
class CFbsBitmap;
class RWsSession;
class CWsScreenDevice;
class RWindowBase;
class MMMADisplay;

//  CLASS DECLARATION
/**
* This class implements MMMADisplayWindow functionality
* to use with CMMACameraPlayer.
* It duplicates its camera instance within the UI thread
* and displays the viewfinder using the UI
* DirectScreenAccess resources.
*
* @since S60 v5.0
*/


NONSHARABLE_CLASS(CMMACameraWindow): public CBase,
        public MMMADisplayWindow,
        public MCameraObserver,
        public MUiEventConsumer,
        public MDirectScreenAccess
{
public:
    enum TUiCallbackType
    {
        EDeleteViewFinder = 1,
        EHideViewFinder,
        EShowViewFinder,
        EResetViewFinder,
        EDestroyWindow
    };

    /**
     * Two-phased constructor.
     *
     * @param  aCameraHandle A handle to existing CCamera instance.
     */
    static CMMACameraWindow* NewL(TInt aCameraHandle);


    /**
     * Destructor.
     */
    ~CMMACameraWindow();

    /**
     * Gets an information if the DirectViewFinder is started
     *
     * @since  S60 v5.0
     * @return ETrue if DirectViewFinder is active
     */
    TBool ViewFinderActive();

    /**
     * Notifies window about Started state change
     *
     * @since S60 v5.0
     * @param aStarted Indicates that camera player is in started state
     */
    void SetStarted(TBool aStarted);

    /**
     * Sets iDisplay
     * Used to invoke callbacks in UI thread
     *
     * @since  S60 v5.0
     * @param "aDisplay" A Display to be used for callbacks.
     *         When set, the duplicated CCamera is created in UI thread
     *         and the necessary DSA resources are got.
     *         When replacing an old Display (set earlier)
     *         all the old resources are deleted.
     *         NULL deletes all allocated UI resources.
     */
    void SetDisplay(MMMADisplay *aDisplay);


// from base class MMMADisplayWindow
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
    TDisplayWindowType GetDisplayWindowType() const;
    TBool IsVisible() const;
    void ContainerSet();
    void ContainerDestroyed();


// from base class MCameraObserver
    void ReserveComplete(TInt aError);
    void PowerOnComplete(TInt aError);
    void ViewFinderFrameReady(CFbsBitmap& aFrame);

    void ImageReady(CFbsBitmap* aBitmap,
                    HBufC8* aData,
                    TInt aError);

    void FrameBufferReady(MFrameBuffer* aFrameBuffer,
                          TInt aError);


// from base class MUiEventConsumer
    void MdcDSAResourcesCallback(
        RWsSession &aWs,
        CWsScreenDevice &aScreenDevice,
        RWindowBase &aWindow);
    void MdcUICallback(TInt aCallbackId);


// from base class MDirectScreenAccess
    void AbortNow(RDirectScreenAccess::TTerminationReasons aReasons);
    void Restart(RDirectScreenAccess::TTerminationReasons aReasons);


private:
    /**
     * Constructor.
     * @see CMMACameraWindow::NewL()
     */
    CMMACameraWindow(TInt aCameraIndex);

    /**
     * Creates UI Camera -
     * a new CCamera instance (duplicated from handle
     * given to the constructor)
     * Have to be called from the thread which UI Camera
     * will be controlled from
     *
     * @since  S60 v5.0
     * @param "aWs" WS session from the last MdcDSAResourcesCallback
     * @param "aScreenDevice" Screen from the last MdcDSAResourcesCallback
     * @param "aWindow" Window from the last MdcDSAResourcesCallback
     */
    void UIStartViewFinder(RWsSession &aWs, CWsScreenDevice &aScreenDevice, RWindowBase &aWindow);

    /**
     * If UI Camera have been created
     * it starts/hides the DirectViewFinder
     * Have to be called from UI thread in which UI Camera
     * have been created!
     *
     * @since  S60 v5.0
     * @param "aVisible" If true DirectViewFinder is started, else stopped
     */
    void SetViewFinderVisibility(TBool aVisible);

    /**
     * Stops the DirectViewFinder and deletes UI Camera instance
     * Have to be called from UI thread in which UI Camera
     * have been created before the CMMACameraWindow instance
     * is deleted!
     *
     * @since  S60 v5.0
     */
    void ReleaseUiResources();

    /**
     * The callback method invoked by starter timer
     *
     * @since  S60 v5.0
     * @param  aThis An instance of CMMACameraWindow
     * @return Always returns EFalse
     */
    static TInt StarterTimerCallback(TAny* aThis);

    /**
     * Starts viewfinder.
     *
     * @since  S60 v5.0
     */
    void StartViewFinder();

    /**
     * Resets (stops and starts) viewfinder
     *
     * @since  S60 v5.0
     */
    void ResetViewFinder();

    /**
     * Draws the underlayer of viewfinder.
     * Used to clear the alpha value.
     * Uses Direct Screen Access.
     *
     * @since  S60 v5.0
     * @return EFalse if DSA start failed, but may work in the next attempt.
               ETrue if DSA start passed.
     */
    TBool DrawUnderlayer();

    /**
     * Draws the error message to specified area.
     * Used in cases when viewfinder is unable to start
     *
     * @since  S60 v5.0
     * @param aError Viewfinder error id
     * @param aDrawRect Area to draw to
     */
    void DrawViewFinderErrorL(const TInt aError, const TRect& aDrawRect);

    /**
     * Deletes this object.
     *
     * @since  S60 v5.0
     */
    void Destroy();

private:  // data
    /**
     * Indicates the window visibility.
     */
    TBool iVisible;

    /**
     * Indicated that owning player is in Started state.
     */
    TBool iStarted;

    /**
     * Indicates that Direct viewfinder is displayed on screen.
     */
    TBool iViewFinderVisible;

    /**
     * Indicates that duplicated camera (iUICamera) is powered on.
     */
    TBool iCameraPowerOn;

    /**
     * Rectangle to display viewfinder on (Window coordinates).
     */
    TRect iDrawRect;

    /**
     * The area of the window.
     */
    TRect iClientRect;

    /**
     * The handle of original camera instance.
     */
    TInt iCameraHandle;

    /**
     * UI Camera instance
     * Duplicated from iCameraHandle.
     * Adjustable and usable from UI thread only.
     * Own.
     */
    CCamera* iUICamera;

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
     * Window to draw viewfinder on.
     * Adjustable and usable from UI thread only.
     * From UI thread.
     * Not owned.
     */
    RWindowBase* iWindow;

    /**
     * The timer to delay viewfinder start
     * to avoid many showing/hidding request
     * (for instance when scrolling the viewfinder)
     * Adjustable and usable from UI thread only.
     * Own.
     */
    CPeriodic* iStarterTimer;

    /**
     * Direct screen access used to draw the underlayer.
     * Adjustable and usable from UI thread only.
     * Own.
     */
    CDirectScreenAccess* iDirectAccess;

    /**
     * Count of remaining allowed DSA restarts.
     * In some cases (like screen orientation change)
     * the DSA fails to start for some time.
     * Therefore the start is tried more times.
     */
    TUint iRemainingDSAStartAttempts;

    /**
     * An icon used to display instead of viewfinder.
     * in case that viewfinder start fails.
     * Own.
     */
    CFbsBitmap* iErrorIconBitmap;

    /**
     * Bitmap mask of iErrorIconBitmap.
     * Own.
     */
    CFbsBitmap* iErrorIconMaskBitmap;
};

#endif // CMMACAMERAWINDOW_H
