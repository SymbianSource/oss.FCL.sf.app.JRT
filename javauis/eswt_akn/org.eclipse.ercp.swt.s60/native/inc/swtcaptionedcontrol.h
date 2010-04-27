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


#ifndef SWTCAPTIONEDCONTROL_H
#define SWTCAPTIONEDCONTROL_H


#include <eikcapc.h>
#include "eswtwidgetscore.h"
#include "eswtmobileextensions.h"
#include "swtcontrolbase.h"


/**
 * CSwtCanvas
 * @lib eswt
 */
NONSHARABLE_CLASS(CSwtCaptionedControl)
        : public CEikCaptionedControl
        , public ASwtControlBase
        , public MSwtScrollable
        , public MSwtComposite
        , public MSwtCaptionedControl
{
// Own methods
public:
    /**
     * Two phase constructor
     * @param aDisplay  parent display
     * @param aPeer     java peer of this class
     * @param aParent   composite parent
     * @param aStyle    control style
     * @return pointer to an object of this class
     */
    static CSwtCaptionedControl* NewL(MSwtDisplay& aDisplay, TSwtPeer aPeer,
                                      MSwtComposite& aParent, TInt aStyle);

private:
    /**
     * First phase constructor.
     * @param aDisplay  parent display
     * @param aPeer     java peer of this class
     * @param aParent   composite parent
     * @param aStyle    control style
     */
    CSwtCaptionedControl(MSwtDisplay& aDisplay, TSwtPeer aPeer,
                         MSwtComposite& aParent, TInt aStyle);

    /**
     * Destructor
     */
    ~CSwtCaptionedControl();

    /**
     * Second phase constructor
     */
    void ConstructL();

    /**
     * Sets the emphasis on caption and trailer.
     * @param aDrawNow tells if a draw should happen or not
     */
    void SetEmphasis(TDrawNow aDrawNow);

    /**
     * Const child control interface getter
     * @return const child control interface
     */
    inline const MSwtControl* Child() const;

    /**
     * Modifiable child control interface getter
     * @return const child control interface
     */
    inline MSwtControl* Child();

    /**
     * Const child control getter
     * @return const child control
     */
    inline const CCoeControl* CoeChild() const;

    /**
     * Modifiable child control getter
     * @return const child control
     */
    inline CCoeControl* CoeChild();

    /**
     * Child control setter
     * @param aChild child control interface
     */
    inline void SetChild(MSwtControl* aChild);

    /**
     * Compute inner rect based on a given outer rect
     * @return rect
     */
    TRect ComputeInnerRect(const TRect& aRect) const;

    /**
     * Real size setter.
     * @param aSize
     */
    void DoSetSize(const TSize& aSize);

    /**
     * Default foreground setter.
     */
    void SetDefaultForegroundL();

    /**
     * Get the children of the current contained control.
     * @param aAllChildren returned array of children.
     * @param aParent parent for which the children will be returned.
     */
    void GetRecursiveChildrenL(RSwtControlArray& aAllChildren,
                               MSwtComposite& aParent);

    /**
     * Calculate the real font height covering the special case where the font
     * is open font and the height returned is differently calculated. If used
     * font is an open font then CEikLabel badly gets font height (for an open
     * font, HeightInPixels() returns the design height and that is notional
     * rather than exact). A workaround is to set a top margin.
     * @param aHeight the height result
     * @return ETrue if the font is open
     */
    TBool RealFontHeight(TInt& aHeight) const;

    /**
     * Getter for the default left margin.
     * @return width in pixels of the margin
     */
    TInt DefaultLeftMargin() const;

    /**
     * Resource change leaving helper
     */
    void SwtHandleResourceChangeL(TInt aType);

// From CCoeControl
public:
    TInt CountComponentControls() const;
    CCoeControl* ComponentControl(TInt aIndex) const;
    void MakeVisible(TBool aVisible);
    void SetDimmed(TBool aDimmed);
    TKeyResponse OfferKeyEventL(const TKeyEvent& aEvent,
                                TEventCode aType);
    TSize MinimumSize();
    void HandleResourceChange(TInt aType);

// From CCoeControl
protected:
    void FocusChanged(TDrawNow aDrawNow);
    void SizeChanged();
    void PositionChanged();

// From MSwtControl
public:
    CCoeControl& CoeControl();
    const CCoeControl& CoeControl() const;
    TBool IsFocusable(TInt aReason = KSwtFocusByApi) const;
    TInt FocusBackgroundPolicy() const;
    void ProcessKeyEventL(const TKeyEvent& aKeyEvent, TEventCode aType);
#ifdef RD_SCALABLE_UI_V2
    void HandlePointerEventL(const TPointerEvent& aPointerEvent);
#endif // RD_SCALABLE_UI_V2
    TBool SetSwtFocus(TInt aReason = KSwtFocusByApi);
    TRect ClientRect() const;
    TSize ComputeSizeL(TInt aWHint, TInt aHHint);
    void SetWidgetSize(const TSize& aSize);
    void SetFontL(const MSwtFont* aFont);
    void SetBounds(const TRect& aRect);
    MSwtComposite* CompositeInterface() const;
    void ComputeTabListL(RSwtControlArray& aList);
    MSwtCaptionedControl* CaptionedControlInterface() const;
    void UpdateDoNotDrawFlag();
    void SetForegroundL(const MSwtColor* aColor);

// From ASwtControlBase
protected:
    const MSwtFont& DefaultFont() const;

// From MSwtScrollable
public:
    MSwtControl* Control();
    void ComputeTrim(TRect& aRect) const;
    MSwtScrollBar* CreateScrollBarL(TSwtPeer aPeer, TInt aStyle);
    void SetVisibleScrollBar(TInt aStyle, TBool aVisible);
    void ScrollBarDestroyed(const MSwtScrollBar* aScrollbar);

// From MSwtComposite
public:
    MSwtScrollable* Scrollable();
    void RegisterChildL(MSwtControl& aChild);
    void UnregisterChild(MSwtControl& aChild);
    CSwtPeerArray* GetChildrenPeersL();
    const RSwtControlArray* Children() const;
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

// From MSwtCaptionedControl
public:
    MSwtComposite* Composite() const;
    void SetTextL(const TDesC& aString);
    void SetTrailingTextL(const TDesC& aString);
    const TDesC* GetText() const;
    const TDesC* GetTrailingText() const;
    void SetImageL(MSwtImage* aImage);
    void SetChildPreferredSize(const TSize& aSize);
    const CCoeControl* GetImageControl() const;

// Data types
private:
    class CControlProxy;

// Data
private:
    /**
     * Children control array.
     */
    RSwtControlArray iChildren;

    /**
     * Default font, cannot be NULL.
     * Own.
     */
    MSwtFont* iDefaultFont;

    /**
     * Latest known position, in window coords. Do not confuse this
     * with CSwtCaptionedControl::CControlProxy::iPrevPos.
     */
    TPoint iPrevPos;

    /**
     * Our inner rectangle for skinned frame
     */
    TRect iInnerRect;

    /**
     * Current custom foreground.
     * Not Own.
     */
    const MSwtColor* iForegroundColor;
    
    /**
     * Can be the child or null.
     */
    MSwtControl* iPointerGrabbingControl;
};


/**
 * CSwtCaptionedControl::CControlProxy
 * A class serving as a proxy between the captioned control and the actual
 * control. Its purpose is to overcome CEikCaptionedControl's use of SetRect().
 * It also serves as a way to avoid having to write MinimumSize() for each
 * and every control by calling the control's ComputeSizeL() method.
 * @lib eswt
 */
NONSHARABLE_CLASS(CSwtCaptionedControl::CControlProxy)
        : public CCoeControl
{
// From CCoeControl
public:
    TInt CountComponentControls() const;
    CCoeControl* ComponentControl(TInt aIndex) const;
    TSize MinimumSize();

// From CCoeControl
protected:
    void PositionChanged();
    void SizeChanged();

// Data
public:
    /**
     * The control we serve as a proxy to, may be NULL.
     * Not Own.
     */
    MSwtControl* iChild;

    /**
     * Preferred size of iControl computed on Java side.
     */
    TSize iPreferredSize;

// Data
private:
    /**
     * Latest known position, in window coords. Do not confuse this
     * with CSwtCaptionedControl::iPrevPos.
     */
    TPoint iPrevPos;
};

#endif // SWTCAPTIONEDCONTROL_H
