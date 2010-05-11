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
* Description:
*
*/


#ifndef CMMADCDISPLAY_H
#define CMMADCDISPLAY_H

//  INCLUDES
#include "mmmadisplay.h"
#include "mmmadirectcontent.h"
#include "cmmadisplay.h"

//  CONSTANTS

//  FORWARD DECLARATIONS
class MMMAGuiPlayer;
class MMMADisplayWindow;
class MMMAContainer;
class MMAFunctionServer;
class CMMADCRepaintEvent;

//  CLASS DECLARATION
/**
* This class is used with dynamic display mode to render direct content
* using direct container.
*/
NONSHARABLE_CLASS(CMMADCDisplay):
        public CMMADisplay,
        public MMMADirectContent
{
public:
    /**
     * Creates new instance.
     * @param aPlayer provides content
     */
    static CMMADCDisplay* NewLC(MMMAGuiPlayer* aPlayer,
    MMAFunctionServer* aEventSource,
    jobject aGUIObject);
    virtual ~CMMADCDisplay();

public: // From MMMADisplay
    void DrawFrameL(const CFbsBitmap* aBitmap);
    void SetDisplaySizeL(const TSize& aSize);
    void SetDisplayLocationL(const TPoint& aPosition);
    TPoint DisplayLocation();
    void SetFullScreenL(TBool aFullScreen);
    void SourceSizeChanged(const TSize& aSourceSize);
    TBool IsVisible();

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
     * @param "aThreadType" Indicates the thread type (ESWT or MMAPI)
     */
    void UIGetDSAResources(
        MUiEventConsumer& aConsumer,
        MMMADisplay::TThreadType aThreadType);

    /**
     * Invokes a callback in ESWT thread
     *
     * @since S60 v5.0
     * @param "aConsumer" A consumer of callback
     * @param "aCallbackId" A number identifying the callback
     */
    void UIGetCallback(MUiEventConsumer& aConsumer, TInt aCallbackId);

public: // From MMMADirectContent
    void MdcContainerVisibilityChanged(TBool aVisible);
    void MdcContentRectChanged(const TRect& aContentRect,
                               const TRect& aClipRect);
    void MdcContainerWindowRectChanged(const TRect& aRect);
    void MdcContainerDestroyed();
    void MdcSetContainer(MMMAContainer* aContainer);
    CFbsBitmap* MdcFrameBuffer() const;
    TSize MdcSourceSize();

public: // From CMMADisplay
    void MdcContentBoundsChanged(const TRect& aRect);

private: // new methods
    /**
     * Creates bitmap when bitmap mode is used
     * @return error code
     */
    TInt InitBitmapMode();

    static void SetDrawRectL(CMMADCDisplay* aDisplay,
                             TSize* aSize);

    CMMADCDisplay(MMMAGuiPlayer* aPlayer,
                  MMAFunctionServer* aEventSource,
                  jobject aGUIObject);
private: // data

    // Not own.
    MMMAContainer* iContainer;

    // Own.
    CFbsBitmap* iBitmap;

    // Not own
    MMAFunctionServer* iEventSource;

    // direct content java object
    jobject iGUIObject;

    // Own
    CMMADCRepaintEvent* iRepaint;

    // Not own
    MMMAGuiPlayer* iPlayer;
};

#endif // CMMADCDISPLAY_H
