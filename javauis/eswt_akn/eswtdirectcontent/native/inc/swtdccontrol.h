/*
* Copyright (c) 2007-2009 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  THIS FILE IS NOT INCLUDED INTO ECLIPSE CVS DELIVERY
*
*/


#ifndef SWTDCCONTROL_H
#define SWTDCCONTROL_H

#ifndef RD_JAVA_NGA_ENABLED
#define SWTDCCONTROL_DSA_ENABLED
#endif

#include <swtcontrolbase.h>


#include <eiklabel.h>
#include <eikspace.h>
#include <w32std.h>

#include <eswtwidgetscore.h>
#include <mswtclient.h>

#include <mmmacontainer.h>

// MUiEventConsumer interface
#include <reflcdui.h>

// MSwtDcEventConsumer interface
#include "swtdceventconsumer.h"

class MSWTDirectContent;
class CSwtDcObserver;


NONSHARABLE_CLASS(CSwtDCControl)
        : public CCoeControl
        , public ASwtControlBase
        , public MSwtAppFocusObserver
        , public MMMAContainer
        , public MSwtDcEventConsumer
#ifdef SWTDCCONTROL_DSA_ENABLED        
        , public MDirectScreenAccess
#endif
{
public: // New methods
    static CSwtDCControl* NewL(MSwtDisplay& aDisplay,
    TSwtPeer aPeer,
    MSwtComposite* aParent,
    MMMADirectContent* aContent);
    ~CSwtDCControl();

protected: // New methods
    CSwtDCControl(MSwtDisplay& aDisplay,
                  TSwtPeer aPeer,
                  MSwtComposite* aParent);

    void ConstructL();

    void SetFullScreenState();
    void UpdateFullScreenState();
    TRect GetTopShellRect() const;
    void DoDraw();
    void DoCallDSAResourcesCallback(MUiEventConsumer *aConsumer);
    void DoCallUICallback(MUiEventConsumer *aConsumer, TInt aCallbackId);

public: // From CCoeControl
    void SetDimmed(TBool aDimmed);
    void HandleResourceChange(TInt aType);
    void MakeVisible(TBool aVisible);

protected: // From CCoeControl
    void Draw(const TRect& aRect) const;
    void FocusChanged(TDrawNow aDrawNow);
    void PositionChanged();
    void SizeChanged();
    TTypeUid::Ptr MopSupplyObject(TTypeUid aId);

public: // From MSwtControl
    CCoeControl& CoeControl();
    const CCoeControl& CoeControl() const;
    void ProcessKeyEventL(const TKeyEvent& aKeyEvent,
                          TEventCode aType);
    TInt GetBorderWidth() const;
    TRect ClientRect() const;
    TSize ComputeSizeL(TInt aWHint, TInt aHHint);
    void SetVisible(TBool aVisible);
    TSwtPeer Dispose();

public: // From MSwtAppFocusObserver
    void HandleAppFocusChangeL(TBool aFocused);

public: // From MSWTContainer
    void MdcRemoveContent();
    TBool MdcContainerVisibility() const;
    void MdcRepaint() const;
    void MdcGetContentRect(TRect& aControlRect,
                           TRect& aParentRect) const;

    void MdcInvalidate(const TSize& aPreferredSize);
    void MdcSetDisplayFullScreen(TBool aFullScreen);
    void MdcGetDSAResources(
        MUiEventConsumer& aConsumer,
        TBool aIsInUiThread);
    void MdcGetUICallback(MUiEventConsumer& aConsumer, TInt aCallbackId);
    TRect MdcContainerWindowRect() const;

public: // From MSwtDcFullscreenProvider
    void HandleDcEvent(int aType);
    
#ifdef SWTDCCONTROL_DSA_ENABLED  
public: // MDirectScreenAccess
    void Restart(RDirectScreenAccess::TTerminationReasons aReason);
    void AbortNow(RDirectScreenAccess::TTerminationReasons aReason);
#endif

public: // New methods
    /**
     * Sets client pointer to eSWT ui thread. It's needed with MdcRepaint
     * method.
     * @param aClient client to eSWT ui thread
     * @param aClientHeap client's heap
     */
    void SetClient(MSwtClient* aClient, RHeap& aClientHeap);

    /**
     * Deletes iClient instance
     */
    void DeleteClient();

    /**
     * Calls content's MdcContentRectChanged method
     */
    void InformContentRect();

    /**
     * Sets control's size to its parent size.
     * This method must be called from ui thread.
     */
    void SetDisplayFullScreen(TBool aFullScreen);

private:
    TBool IsControlActive() const;
#ifdef SWTDCCONTROL_DSA_ENABLED       
    TBool IsDsaRegionValid() const;
#endif
    TBool IsContentVisibilityAllowed() const;
    TBool IsFrameBufferUsed() const;

private: // Data
    // Own.
    MSwtClient* iClient;

    // Not own. heap where iClient is
    RHeap* iClientHeap;

    // Not own.
    MMMADirectContent* iContent;

#ifdef SWTDCCONTROL_DSA_ENABLED
    // Own.
    CDirectScreenAccess* iDSAccess;
#endif

    // Store video state
    TBool iFullScreen;

    // Store non-fullscreen size & position
    TPoint iNormalPos;
    TSize  iNormalSize;

    // Store last fade effect message
    TInt iLastFadeMessage;

#ifdef SWTDCCONTROL_DSA_ENABLED
    // True, is DSA was started already
    TBool iDsaWasStartedAlready;
#endif
    
    // Own.
    // This observer receives requests from any thread and asynchronously
    // calls a callback in ESWT thread to a given receiver
    CSwtDcObserver* iDcObserver;
    
    
};


#endif // SWTDCCONTROL_H



