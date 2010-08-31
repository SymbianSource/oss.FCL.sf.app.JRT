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


#ifndef CMMADISPLAY_H
#define CMMADISPLAY_H

//  INCLUDES
//#include <reflcdui.h>
#include "mmmadisplay.h"
#include "mmafunctionserver.h"
#include "qwidget.h"
#include "jni.h"
#include "jutils.h"
#include "mmmaguiplayer.h"

//  FORWARD DECLARATIONS
class MMMADisplayWindow;

//  CLASS DECLARATION
/**
*   This class implements MMMADisplay
*
*
*/

NONSHARABLE_CLASS(CMMADisplay): public CBase,public MMMADisplay
        /*,
        public MDirectContent*/
{
public:
    // Constructors and destructors
    ~CMMADisplay();    // Destructor ()

protected:  // Constructors and destructors
    // Default constructor, protected to allow derivation
    CMMADisplay();

    void Construct(MMAFunctionServer* eventSource ,JNIEnv* aJni, jobject javadisplayref);

public: // Methods derived from MMMADisplay
    void DrawFrameL(const CFbsBitmap* aBitmap);
    TSize DisplaySize();
    void SetDisplaySizeL(const TSize& aSize);
    void SetVisible(TBool aValue);
    void SetWindowL(MMMADisplayWindow* aWindow);
    MMMADisplayWindow* Window();
    TBool IsVisible();
    TBool IsFullScreen();
    virtual void SetForeground(TBool aIsForeground, TBool aUseEventServer);
    void SourceSizeChanged(const TSize& aSourceSize);
    void SetUIPlayer(MMMAGuiPlayer* player);
    TSize SourceSize();
    virtual void SourceSizeChanged(TInt aJavaControlWidth, TInt aJavaControlHeight,TInt x,TInt y, TRect aBoundsRect) = 0;
    TBool iseSWT();
    void SetFullscreenSize(TSize& aSize);
    /**
     * Gets notification that there is container to draw assigned
     *
     * @return ETrue if container have been set
     *         EFalse if container is not set
     */
    virtual TBool HasContainer();

    /**
     * Gets resources necessary to start DirectScreenAccess
     * Doesn't run in mmapi event server thread!
     *
     * @since S60 v5.0
     * @param "aConsumer" A consumer of callback
     * @param "aThreadType" Indicates the thread type (UI or MMAPI)
     */
   // void UIGetDSAResources(
   //     MUiEventConsumer& aConsumer,
    //    MMMADisplay::TThreadType aThreadType);

    /**
     * Invokes a callback in UI thread
     *
     * @since S60 v5.0
     * @param "aConsumer" A consumer of callback
     * @param "aCallbackId" A number identifying the callback
     */
    //virtual void UIGetCallback(
    //    MUiEventConsumer& aConsumer,
    //    TInt aCallbackId);
public: // Methods derived from MDirectContent
    /**
     * Not used method.
     */
   // virtual void MdcContainerWindowChangedL(RWindow* aWindow);

    /**
     * Same functionality is shared in canvas and item displays.
     */
   // virtual void MdcContainerVisibilityChanged(TBool aVisible);

    /**
     * Default implementation, panics in debug build. Must be written in
     * derived classes if needed.
     */
   // virtual void MdcItemContentRectChanged(const TRect& aContentRect,
//                                           const TRect& aScreenRect);

    /**
     * Notify content that container is about to be destoryed.
     */
   // virtual void MdcContainerDestroyed();
    /**
     * Notify player's window that any drawing
     * via direct screen access must be aborted
     */
   // void MdcAbortDSA();

    /**
     * Allow player's window to draw
     * via direct screen access after MdcAbortDSA
     */
   // void MdcResumeDSA();

protected:// New Methods
    /**
     * Scales drawarea to full screen. Aspect ratio of the source size
     * will be maintained.
     */
    TRect ScaleToFullScreen(const TSize& aFullScreenSize,
                            const TSize& aSourceSize);

    /**
     * Set clipping region to LCDUI UI component according
     * to current window drawrect so that the LCDUI component
     * does not paint over the area that is occupied by video.
     * Based on current visibility of video image, this method
     * either sets or removes the clipping region.
     * LCDUI component is then redrawn.
     */
    void SetClippingRegion();
    /**
     * MMAPI_UI 3.x req
     * set content bound to eSWT control
     */
    void SetContentBoundToJavaControl(const TRect& aRect);
	  /**
     * Remove currently set clip region and refresh canvas
     */
    void RemoveClippingRegion();

    /**
     * MMAPI_UI 3.x req
     * removes content bound from eSWT control
     */
		void RemoveContentBoundFromJavaControl(const TRect& aRect);
    /**
     * Add clip region
     */
    void AddClippingRegion();
    /**
     * MMAPI_UI 3.x req
     * Redraw the java side eSWT control
     */
    void RefreshJavaControl();

    /**
     * MMAPI_UI 3.x req
     * Reset the rectangle dimension in eSWT control
     */
    void ResetJavaRectObject(const TRect& aRect);

    /**
    * Handling the change in container visibility
    */
    void HandleContainerVisibilityChanged( TBool aVisible );

public:
    /**
    * called from java when shell visibility gets change
    */
    void SetContainerVisibility(TBool aValue);
    /**
    * called from java to set the window resources
    */
    void SetWindowResources(QWidget* qtWidget);

		/**
    * Trigger a function call CalledBackInUiThread() from java in UI thread
    * arg 'placeholder' is used to identify the function, to be called back from UI Thread
    */
    void GetCallbackInUiThread(TInt placeholder);

    /**
    * Called from java in UI thread context
    * arg 'placeholder' is used to identify the function, to be called back from UI Thread
    */
    void CalledBackInUiThread(TInt placeholder);


    void SetSourceSizeToDisplay(const TSize& aSourceSize);
    void SetDisplayPosition(TInt uiControlLocationX,TInt uiControlLocationY,TInt	videoControlLocationX,TInt videoControlLocationY);

protected:  // Data

    /**
     * Not owned.
     * Actual drawing place of the content.
     */
    MMMADisplayWindow* iWindow;

    /**
     * Source contents size.
     */
    TSize iSourceSize;

    /**
     * Indicates visibility set from from Java.
     */
    TBool iVisible;

    /**
     * Screenmode set from Java.
     */
    TBool iFullScreen;

    /**
     * Is native container visible.
     */
    TBool iContainerVisible;

    /**
     * Draw rect set from java.
     */
    TRect iUserRect;

    /**
     * Not owned, obtained from lcdui components.
     */
    //MDirectContainer* iDirectContainer;

    /**
     * Clip rectangle currently set to LCDUI.
     * NOTE: need to be removed at last during destruction phase!
     */
    TRect iClipRect;
    /**
     * Is midlet is in foreground ?
     */
    TBool iIsForeground;

    /**
     * Indicates that iUserRect has been changed
     * during container invisibility
     */
    TBool iResetDrawRect;
    // not owned
    MMAFunctionServer* iEventSource;
    JNIEnv* iJni;
    jobject iJavaDisplayObject;
	jclass iJavaDisplayClass;
	TSize iFullScreenSize;
	MMMAGuiPlayer* iUiPlayer;
	TBool iIseSWT;
	jmethodID setRectID;
	jfieldID iRectDimensionField;
	jfieldID iDisplayboundarrField;
	jmethodID setContentBoundID;
	jmethodID removeContentBoundID;
	jmethodID redrawControlID;
	jmethodID getCallBackMethodID;
	jobject iJavadisplayref;
	jmethodID getBoundRectID;
};

#endif // CMMADISPLAY_H
