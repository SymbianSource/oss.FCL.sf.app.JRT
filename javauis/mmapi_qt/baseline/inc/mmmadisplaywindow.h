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
* Description:  Abstract base class for windows.
*
*/


#ifndef MMMADISPLAYWINDOW_H
#define MMMADISPLAYWINDOW_H


#include "mmmadisplay.h"
#include "qwidget.h"
#include "coecntrl.h"
#include "coemain.h"
//  FORWARD DECLARATIONS
class CFbsBitmap;
class CMMAPlayer;

//  CLASS DEFINITION
/*
-----------------------------------------------------------------------------

    DESCRIPTION

    Abstract base class for windows.

-----------------------------------------------------------------------------
*/
NONSHARABLE_CLASS(MMMADisplayWindow)
{
public:

    enum TDisplayWindowType
    {
        EDisplayWindowTypeNotSet = -1,
        EDisplayWindowTypeIsDSA,
        EDisplayWindowTypeIsBitmap,
        EDisplayWindowTypeIsCamera,
    };

    /**
     * Can be deleted through this interface.
     */
    virtual ~MMMADisplayWindow()
    {
    };

    /**
     * Sets destination bitmap for <code>DrawFrameL</code> method
     *
     * @param aBitmap destination Bitmap where DrawFrameL draws.
     */
    virtual void SetDestinationBitmapL(CFbsBitmap* aBitmap) = 0;

    /**
     * Draws bitmap to the rect set with <code>SetDrawRectL</code>
     * method. Bitmap may be scaled to fill whole rect. Implementation
     * may ignore <code>aBitmap</code> parameter and implements own
     * drawing procedure.
     *
     * @param aBitmap Bitmap to be drawn.
     */
    virtual void DrawFrameL(const CFbsBitmap* aBitmap) = 0;

    /**
     * Sets drawing area which is used in <code>DrawFrameL</code> method.
     *
     * THE CALL CANNOT ORIGINATE FROM UI THREAD.
     * (including the call via MMA event server)
     *
     * use SetDrawRectThread when in UI thread
     *
     * @param aRect New drawing area.
     */
    virtual void SetDrawRect(const TRect& aRect) = 0;

    /**
     * Calls SetDrawRect method through event source. This method must
     * be called instead of SetDrawRect if not executing in MMA
     * thread.
     *
     * THE CALL CANNOT ORIGINATE FROM MMA THREAD.
     * (including the call via UI event server)
     *
     * @param aRect New drawing area.
     */
    virtual void SetDrawRectThread(const TRect& aRect) = 0;

    /**
     * Sets drawing position which is used in <code>DrawFrameL</code> method.
     *
     * @param aPosition New drawing position.
     */
    virtual void SetPosition(const TPoint& aPosition) = 0;

    /**
     * Sets window visible or invisible. This method can be called from
     * MMA event server thread or from other context. aUseEventServer
     * must be set to EFalse if caller is already in MMA event server.
     *
     * @param aVisible visiblity
     * @param aUseEventServer Indicates if method must called through event
     *                        server.
     */
    virtual void SetVisible(TBool aVisible, TBool aUseEventServer = ETrue) = 0;

    /**
     * Returns rect which is used for drawing frames.
     *
     * @return Drawing area
     */
    virtual const TRect& DrawRect() = 0;

    /**
     * Returns whole window's size.
     *
     * @return Size of the window.
     */
    virtual TSize WindowSize() = 0;

    /**
     * Sets window size.
     *
     * @param aRect Windows size.
     */
    virtual void SetWindowRect(const TRect& /*aRect*/, MMMADisplay::TThreadType /*aThreadType*/) = 0;

    /**
     * Gets window rect.
     *
     * @returns Windows rect.
     */
    virtual const TRect& WindowRect() = 0;
    
    
        
    #ifdef RD_JAVA_NGA_ENABLED
    /**
     * Sets Crop region. used only in CMMASurfaceWindow
     *
     * @since S60 v5.2
     */
    virtual void SetVideoCropRegion(const TRect& /*aRect*/)
    {
        // Empty
    }

    /**
     * Sets RWindow rect. used only in CMMASurfaceWindow.
     * can be called from either MMA ES thread context or
     * UI thread context.
     *
     * @since S60 v5.2
     */
    virtual void SetRWindowRect(const TRect& /*aRect*/,
                                MMMADisplay::TThreadType /*aThreadType*/)
    {
        // Empty
    }
#endif

    /**
         *
         */
    virtual TDisplayWindowType GetDisplayWindowType() const
    {
        return EDisplayWindowTypeNotSet;
    }

    /**
     * Gets window visible or invisible.
     * @retrun ETrue if visible else EFalse. Default implementation always returns ETrue
     */
    virtual TBool IsVisible() const
    {
        return ETrue;
    }

    /**
     * Informs window that container which is window drawn on
     * is set from now. Implementation is now able to invoke
     * any UI callbacks
     *
     * @since S60 v5.0
     */
    virtual void ContainerSet()
    {
        // Empty
    }

    /**
     * Informs window that container which is window drawn on
     * is going to be deleted. Implementation should abort any
     * DirectScreenAccess and delete all the objects
     * instantiated within UI thread.
     * Called wihout mmapi event server directly from UI thread.
     * By default it doesn't do anything.
     *
     * @since S60 v5.0
     */
    virtual void ContainerDestroyed()
    {
        // Empty
    }

    /**
     * Notifies window that any drawing
     * via direct screen access must be aborted
     */
    virtual void AbortDSA()
    {
        // default empty implementation
    }

    /**
     * Allows window to draw
     * via direct screen access after MdcAbortDSA
     */
    virtual void ResumeDSA()
    {
        // default empty implementation
    }
    
    /**
     * gets window resources from QT
     */
    virtual void ProcureWindowResourcesFromQWidget(RWsSession * /*aWs*/,
    																										CWsScreenDevice* /*aScreenDevice*/,
  																													  RWindowBase* /*aWindow*/)
    {
        // default empty implementation
    }
    
    virtual void UICallback( TInt /*aCallbackId*/ )
    {
    }
};

#endif // MMMADISPLAYWINDOW_H
