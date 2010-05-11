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
* Description:  Interface for displays, which can show images.
*
*/


#ifndef MMMADISPLAY_H
#define MMMADISPLAY_H

#include "jni.h"
//  FORWARD DECLARATIONS
class CFbsBitmap;
class MMMADisplayWindow;
class MUiEventConsumer;
class MMMAGuiPlayer;

//  CLASS DEFINITION
/*
-----------------------------------------------------------------------------

    DESCRIPTION

    Interface for displays, which can show images. Display properties
    can be changed through this interface.

-----------------------------------------------------------------------------
*/
NONSHARABLE_CLASS(MMMADisplay)
{
public:
    /**
      * Describes the currently running thread type.
      */
    enum TThreadType
    {
        EMmaThread = 0, // MMAPI thread
        EUiThread  = 1  // UI thread (LCDUI or ESWT)
    };

    /**
     * Display can be deleted through this interface.
     */
    virtual ~MMMADisplay()
    {
    };

    /**
     * Draws bitmap. Implementation may ignore <code>aBitmap</code>
     * parameter and implements own drawing procedure. This method must also redraw
     * the display if needed.
     *
     * @param aBitmap Bitmap to be drawn.
     */
    virtual void DrawFrameL(const CFbsBitmap* aBitmap) = 0;

    /**
     * Getter for display's size.
     *
     * @return Display size.
     */
    virtual TSize DisplaySize() = 0;

    /**
     * Sets display size or leaves if not supported or
     * size cannot be changed.
     *
     * @param aSize New size for the display.
     */
    virtual void SetDisplaySizeL(const TSize& aSize) = 0;

    /**
     * Sets display location.
     *
     * @param aPosition New position for the display.
     */
    virtual void SetDisplayLocationL(const TPoint& aPosition) = 0;

    /**
     * Returns position where display is drawn.
     *
     * @return Display location.
     */
    virtual TPoint DisplayLocation() = 0;

    /**
     * Sets display fullscreen mode on/off.
     *
     * @param aFullScreen Fullscreen mode.
     */
    virtual void SetFullScreenL(TBool aFullScreen) = 0;

    /**
     * Sets display visible or hides it.
     *
     * @param aVisible ETrue if display is visible,
     *                 EFalse if display is invisible.
     */
    virtual void SetVisible(TBool aVisible) = 0;

    /**
     * Called when source size is changed. This is also called when
     * source size is available for the first time.
     *
     * @param aSourceSize New content's size.
     */
    virtual void SourceSizeChanged(const TSize& aSourceSize) = 0;
    
    virtual TSize SourceSize() = 0;
    
		virtual void SetUIPlayer(MMMAGuiPlayer* player) = 0;
    /**
     * Sets display's window. Ownership is not transfered.
     *
     * @param aWindow New window used for drawing.
     **/
    virtual void SetWindowL(MMMADisplayWindow* aWindow) = 0;

    /**
     * Getter for window.
     *
     * @return Display's window or NULL if not set.
     */
    virtual MMMADisplayWindow* Window() = 0;

    /**
     * Returns visibility of the display.
     *
     * @return ETrue if display is visible,
     *         EFalse if display is invisible.
     */
    virtual TBool IsVisible() = 0;

    /**
     * Returns fullscreen state of the display.
     *
     * @return ETrue if display is fullscreen,
     *         EFalse if display is not fullscreen.
     */
    virtual TBool IsFullScreen() = 0;
    /**
     *Sets the foreground state of the midlet
     *
     * @param ETrue if midlet is in foreground,
     *         EFalse if midlet is in background
     */
    virtual void SetForeground(TBool aForeground, TBool aUseEventServer) = 0;

    /**
     * Gets notification that there is container to draw assigned
     *
     * @return ETrue if container have been set
     *         EFalse if container is not set
     */
    virtual TBool HasContainer() = 0;

    /**
     * Gets resources necessary to start DirectScreenAccess
     * Doesn't run in mmapi event server thread!
     *
     * @since S60 v5.0
     * @param "aConsumer" A consumer of callback
     * @param "aThreadType" Indicates the thread type (UI or MMAPI)
     */
   /* virtual void UIGetDSAResources(
        MUiEventConsumer& aConsumer,
        TThreadType aThreadType) = 0;
*/
    /**
     * Invokes a callback in UI thread
     *
     * @since S60 v5.0
     * @param "aConsumer" A consumer of callback
     * @param "aCallbackId" A number identifying the callback
     */
 /*   virtual void UIGetCallback(
        MUiEventConsumer& aConsumer,
        TInt aCallbackId) = 0;
        */
        
        /**
    * Trigger a function call CalledBackInUiThread() from java in UI thread
    * arg 'placeholder' is used to identify the function, to be called back from UI Thread
    */    
    virtual void GetCallbackInUiThread(TInt placeholder) = 0;
    
    /**
    * Called from java in UI thread context
    * arg 'placeholder' is used to identify the function, to be called back from UI Thread
    */ 
    virtual void CalledBackInUiThread(TInt placeholder) = 0;
};

#endif // MMMADISPLAY_H
