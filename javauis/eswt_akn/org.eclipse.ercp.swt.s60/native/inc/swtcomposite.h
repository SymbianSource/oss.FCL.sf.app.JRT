/*******************************************************************************
 * Copyright (c) 2005, 2010 Nokia Corporation and/or its subsidiary(-ies).
 * All rights reserved. This program and the accompanying materials
 * are made available under the terms of the Eclipse Public License v1.0
 * which accompanies this distribution, and is available at
 * http://www.eclipse.org/legal/epl-v10.html
 *
 * Contributors:
 *     Nokia Corporation - S60 implementation
 *******************************************************************************/


#ifndef SWTCOMPOSITE_H
#define SWTCOMPOSITE_H


#include <AknControl.h>
#include <eiksbobs.h>
#include <mswtphysics.h>
#include "eswtwidgetscore.h"
#include "swtscrollablebase.h"


class CEikScrollBarFrame;
class CSwtScrollBar;
class CSwtUiUtils;

/**
 * CSwtComposite
 * @lib eswt
 */
NONSHARABLE_CLASS(CSwtComposite)
        : public CAknControl
        , public ASwtScrollableBase
        , public MSwtComposite
        , public MSwtPhysicsObserver
{
// Own api
public:
    static CSwtComposite* NewL(MSwtDisplay& aDisplay, TSwtPeer aPeer,
    MSwtComposite& aParent, TInt aStyle);

protected:
    CSwtComposite(MSwtDisplay& aDisplay, TSwtPeer aPeer,
                  MSwtComposite* aParent, TInt aStyle, TBool aVisibility, TBool aDimmed);
    ~CSwtComposite();
    void ConstructL();
    void CreateScrollBarFrameL();
    void UpdateFrameL();
    void RectChanged();
    virtual void SwtHandleResourceChangeL(TInt aType);

// From MCoeControlBackground
public:
    virtual void Draw(CWindowGc& aGc, const CCoeControl& aControl,
                      const TRect& aRect) const;

// From CCoeControl
public:
    TInt CountComponentControls() const;
    CCoeControl* ComponentControl(TInt aIndex) const;
    void MakeVisible(TBool aVisible);
    void SetDimmed(TBool aDimmed);
    TKeyResponse OfferKeyEventL(const TKeyEvent& aKeyEvent, TEventCode aType);
    void HandleResourceChange(TInt aType);

protected:
    void Draw(const TRect& aRect) const;
    void FocusChanged(TDrawNow aDrawNow);
    void PositionChanged();
    void SizeChanged();
    TTypeUid::Ptr MopSupplyObject(TTypeUid aId);

// From MEikScrollBarObserver
public:
    void HandleScrollEventL(CEikScrollBar* aScrollBar, TEikScrollEvent aEventType);

// From MSwtControl
public:
    CCoeControl& CoeControl();
    const CCoeControl& CoeControl() const;
    TBool IsFocusable(TInt aReason = KSwtFocusByApi) const;
    TInt FocusBackgroundPolicy() const;
    void ProcessKeyEventL(const TKeyEvent& aKeyEvent, TEventCode aType);
    void HandlePointerEventL(const TPointerEvent& aPointerEvent);
    TBool SetSwtFocus(TInt aReason = KSwtFocusByApi);
    TSize ComputeSizeL(TInt aWHint, TInt aHHint);
    MSwtComposite* CompositeInterface() const;
    void ComputeTabListL(RSwtControlArray& aList);
    void UpdateDoNotDrawFlag();
    void Update() const;

// From ASwtScrollableBase
protected:
    CEikScrollBarFrame* SbFrame() const;
    void SetVisibleScrollBar(TInt aStyle, TBool aVisible);

// From MSwtComposite
public:
    MSwtScrollable* Scrollable();
    void RegisterChildL(MSwtControl& aChild);
    void UnregisterChild(MSwtControl& aChild);
    CSwtPeerArray* GetChildrenPeersL();
    inline const RSwtControlArray* Children() const;
    void ChildFocusChanged(MSwtControl& aControl);
    TInt CountFocusableChildren(TInt aTryBreakAfter = -1,
                                const MSwtControl* aExclude = NULL, TBool aVisualFocusCount = ETrue) const;
    void SetTabListL(MSwtControl** aControls, TInt aCount);
    CSwtPeerArray* GetTabListL() const;
    void MoveChildAbove(MSwtControl& aChild1, const MSwtControl* aChild2);
    void MoveChildBelow(MSwtControl& aChild1, const MSwtControl* aChild2);
    void NotifyRadioGroup(MSwtButton* aSelectedButton);
    MSwtControl* GetPointerGrabbingControl() const;
    void SetScrolledCompositeContentL(MSwtControl* aContent);
    MSwtControl* ScrolledCompositeContent() const;
    TInt ScrolledCompositePysicsAction() const;
    void DrawScrollBarBackground(const TRect& aRect) const;

// From MSwtPhysicsObserver
public:
    void ViewPositionChanged(const TPoint& aNewPosition, TBool aDrawNow, TUint aFlags);
    void PhysicEmulationEnded();
    TPoint ViewPosition() const;

// Own api
private:
    void  DoSetTabListL(MSwtControl** aControls, TInt aCount);
    void  ProcessCompositePointerEventL(const TPointerEvent& aPointerEvent);
    void  InvalidateScrollBars();
    TBool HasFocusableControl(const MSwtControl* aControl) const;
    void  DoHandlePointerEventL(const TPointerEvent& aPointerEvent);
    TBool PhysicsEnabled() const;
    void  PhysicsResetGeometryL();
    void  PhysicsStartTimer();
    static TInt PhysicsTimerCallback(TAny* aPtr);
    void PaintUrgently() const;
#ifdef RD_JAVA_ADVANCED_TACTILE_FEEDBACK
    void UpdateTactileFeedbackDensity();
#endif //RD_JAVA_ADVANCED_TACTILE_FEEDBACK

// Data
protected:
    /**
     * For creating children arrays, this is the granularity param for the c.tor
     */
    static const TInt KChildrenGranularity;

    /**
     * The controls this control contains, none is owned, none is NULL
     */
    RSwtControlArray iChildren;

    /**
     * The tab-order of controls, only filled if different from iChildren
     */
    RSwtControlArray iTabList;

    /**
     * The scroll-bar frame, owned, may be NULL
     */
    CEikScrollBarFrame* iSbFrame;

private:
    /**
     * Composite's rectangle
     */
    TRect iRect;

    TBool iActuallyInRadioButtonSelect;

    /**
     * A pointer to the control where the
     * last button down occured.
     * It can be null. Not Own.
     */
    MSwtControl* iPointerGrabbingControl;

    /**
     * Deliver all pointer event to the scrollbar when this on.
     */
    TBool iVScrollBarGrabsPointerEvents;
    TBool iHScrollBarGrabsPointerEvents;

    /**
     * The content control, in case this is a ScrolledComposite
     */
    MSwtControl* iScrlCompContent; // Now own

    /**
     * Physics related
     */
    MSwtPhysics* iPhysics;     // Own
    CPeriodic* iPhysicsTimer;  // Own
    TPoint iPhysicsStartPos;
    TPoint iPhysicsLastPos;
    TPoint iPhysicsViewPos;
    TTime iPhysicsStartTime;
    TPointerEvent iPhysicsPointerDown;
    TInt iPhysicsAction;
    TBool iPhysicsBreaking;
    RLibrary iPhysicsLib;
    MSwtControl* iPhysicsFocusedCtrl; // Not own

#ifdef RD_JAVA_ADVANCED_TACTILE_FEEDBACK
    /**
     * Kinetic scrolling tactile feedback
     */
    TInt iLastTactileFeedbackPos;
    TInt iTactileFeedbackDensity;    
#endif //RD_JAVA_ADVANCED_TACTILE_FEEDBACK
};


/**
 * Returns the array containing the Composite's children
 */
inline const RSwtControlArray* CSwtComposite::Children() const
{
    return &iChildren;
}


#endif // SWTCOMPOSITE_H
