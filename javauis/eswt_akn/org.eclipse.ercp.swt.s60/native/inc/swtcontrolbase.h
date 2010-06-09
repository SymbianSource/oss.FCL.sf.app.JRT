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


#ifndef SWTCONTROLBASE_H
#define SWTCONTROLBASE_H


#include <AknsFrameBackgroundControlContext.h>
#include "eswtwidgetscore.h"


class CSwtUiUtils;
class CAknsBasicBackgroundControlContext;


/**
 * ASwtControlBase
 * Abstract helper base class for implementing MSwtControl
 * @lib eswt
 */
class ASwtControlBase
        : public MSwtControl
        , public MCoeControlBackground
{
// Data types
protected:
    enum TKeyScanCode
    {
        // CBA key scan codes
        EStdKeyCba1 = EStdKeyDevice0,
        EStdKeyCba2 = EStdKeyDevice1,
        EStdKeyCba3 = EStdKeyDevice2,
        EStdKeyCba4 = EStdKeyDevice3,

        // Rocker key scan codes
        EStdKeyRockerLeft   = EStdKeyDevice6,
        EStdKeyRockerRight  = EStdKeyDevice7,
        EStdKeyRockerUp     = EStdKeyDevice8,
        EStdKeyRockerDown   = EStdKeyDevice9,
        EStdKeyRockerMiddle = EStdKeyDeviceA
    };

    enum
    {
        ECtrlFuncShiftMask = EModifierCtrl | EModifierFunc | EModifierShift
    };

// Own methods
public:
    /**
     * Clips given rectangle to rectangle which can be fully visible in
     * its parent. Other controls in the same parent are not taken
     * into account.
     * @param aRect Rectangle to be clipped
     * @return Clipped rectangle
     */
    TRect ClipToVisibleRect(const TRect& aRect) const;

    /**
     * Provide the Skin background.
     * Should be called by the MopSupplyObject of all implementeded controls.
     */
    IMPORT_C TTypeUid::Ptr SwtMopSupplyObject(TTypeUid aId);

    MAknsControlContext* SwtMopSupplyCtrlContext(TBool aBeginSearchToThis = ETrue) const;

    /**
     * Set up a clipping region for a Composite not to draw the areas that will be
     * drawn by its children. This should be done by Composites to avoid flickering.
     * This function operates in window coordinates. You can use aOffset to convert
     * the region into client area coordinates for instance: aOffset = -ClientArea().iTl
     * or to screen coordinates: aOffset = +Window().Position()
     *
     * @param aComposite The composite to set the clipping region for
     * @param aGc        The GC to set the clipping region to (that will be used by
     *                   the composite to draw)
     * @param aDrawRect  The target drawing rectangle where drawing is about to be done into.
     * @param aOffset    Optional offset. Used to move the whole region.
     * @return           RRegion pointer to clipping region or null if clipping region was not set.
     *                   Caller owns the returned RRegion. It should be freed by calling
     *                   CancelChildClippingRegion after drawing is complete.
     */
    RRegion* SetupChildClippingRegion(const MSwtComposite* aComposite,
                                      CWindowGc& aGc,
                                      const TRect& aDrawRect,
                                      const TPoint& aOffset = TPoint(0, 0)) const;

    /**
     * Helper to appropriately cancel and free the clipping region set by a call to
     * SetupChildClippingRegion.
     *
     * @param aClippingRegion The RRegion pointer obtained from SetupChildClippingRegion
     * @param aGc             The GC that was passed to SetupChildClippingRegion
     */
    void CancelChildClippingRegion(RRegion* aClippingRegion, CWindowGc& aGc) const;

// Own methods
protected:
    IMPORT_C ASwtControlBase(MSwtDisplay& aDisplay, TSwtPeer aPeer,
                             MSwtComposite* aParent, TInt aStyle);

    IMPORT_C virtual ~ASwtControlBase();

    /**
     * Needs to be called by all children from their NewL(), after ConstructL().
     * Initialize this Object.
     */
    void InitControlBaseL();

    /**
     * Get the focus background of the Control at a certain moment in time.
     */
    IMPORT_C CAknsBasicBackgroundControlContext* FocusBackgroundNow() const;

    /**
     * Trigger focus background redraw.
     */
    void RedrawFocusBackground();

    /**
     * Checks if this controls should display a border / frame in case of Shells.
     */
    TBool HasBorderStyle() const;

    /**
     * Returns the size difference between the inner and outer rectangles of the border.
     */
    TSize BorderSizeDelta() const;

    /**
     * Get rect inside the borders.
     */
    IMPORT_C virtual TRect BorderInnerRect() const;

    /**
     * Add the borders size to a rect.
     */
    IMPORT_C virtual TRect BorderOuterRect(const TRect& aRect) const;

    IMPORT_C virtual void DrawBorder(const TRect& aRect) const;

    /**
     * Invalidates the specified area if the control is visible.
     * Window coordinates.
     * @param aRect The area to invalidate
     */
    void Invalidate(const TRect& aRect);

    /**
     * Invalidates the specified area if the control is visible.
     * Window coordinates.
     * @param aRect The area to invalidate
     */
    void ForceInvalidate(const TRect& aRect);

    /**
     * Needs to be called by all children from their FocusChanged(TDrawNow).
     * Performs common focus change tasks.
     */
    IMPORT_C void HandleFocusChanged(TDrawNow aDrawNow);
    void FocusabilityChanged();
    TKeyResponse HandleKeyL(const TKeyEvent& aKeyEvent, TEventCode aType, TBool aTraversalDoIt);
    TKeyResponse OfferKeyEventToCommandAndMenuArrangersL(const TKeyEvent& aKeyEvent,TEventCode aType);
    TKeyResponse OfferKeyEventToTabGroupL(const TKeyEvent& aKeyEvent,TEventCode aType);

    /**
     * Register this <code>Control</code> with its parent <code>Composite</code>
     *
     * This method must <b>not</b> be called from within <code>ContructL()</code>
     * but from within <code>NewL()</code> <b>after</b> <code>ConstructL()</code>
     * has been called. Failure in doing so may result in objects registering with
     * their parents before being fully constructed.
     */
    IMPORT_C void RegisterWithParentL();
    inline const MSwtFont* Font() const;

    /**
     * Converts a point from client to window coordinates
     */
    TPoint ClientToWindow(const TPoint& aPoint) const;

    /**
     * Converts a rectangle from client to window coordinates
     */
    TRect  ClientToWindow(const TRect&  aRect)  const;

    /**
     * Converts a point from window to client coordinates
     */
    TPoint WindowToClient(const TPoint& aPoint) const;

    IMPORT_C virtual const MSwtFont& DefaultFont() const;
    IMPORT_C virtual void DoPaint(const TRect& aRect) const;
    IMPORT_C void SetColorL(TLogicalColor aLogicalColor, const MSwtColor* aColor);
    IMPORT_C CSwtUiUtils& UiUtils() const;

    /**
     * Called just before dispose() ends.
     */
    IMPORT_C virtual void DoDelete();

    /**
     * Needs to be called by all inheritors from their SizeChanged().
     */
    IMPORT_C void HandleSizeChanged();

    /**
     * Needs to be called by all inheritors from their PositionChanged().
     */
    IMPORT_C void HandlePositionChanged();

    /**
     * Draws the non-window-owning component controls of the specified control
     * The GC must have already been activated.
     */
    static void DrawComponentControls(const CCoeControl& aControl,
                                      const TRect& aRect, CWindowGc& aGc, TBool aCanDrawOutsideRect);

    static TBool RectContainsRect(const TRect& aContainer, const TRect& aContainee);

    /**
     * Flushes the given gc operations to the given gc.
     */
    void FlushJavaBuffer(MSwtGc* aGc,TInt* aIntParams, TInt aIntCount, HBufC* aStrParams, RWindow* aWindow, const TRect& aDrawRect) const;

    /**
     * Check if compound control focused or one of the children focused.
     */
    TBool IsFocusedOrChildIsFocused() const;

    /**
     * Check if one of the children is focused.
     */
    TBool ChildIsFocused() const;

    /**
     * Same as SetFontL but without redraw.
     */
    void DoSetFontL(const MSwtFont* aFont);

    /**
     * Same as SetBackgroundL but without redraw.
     */
    void DoSetBackgroundL(const MSwtColor* aColor);

    /**
     * Same as SetForegroundL but without redraw.
     */
    void DoSetForegroundL(const MSwtColor* aColor);

private:
    /**
     * Computes the location a control must be set to
     * This method is not intended to be used with <code>Shell</code>s.
     * @return The control's new location in window coordinates
     * @param aLocation The control's new location relative to its parent.
     */
    TPoint ComputeLocationToSet(const TPoint& aLocation);

    /**
     * Convert a point from window coordinates to screen coordinates
     */
    TPoint WindowToScreen(const TPoint& aPoint) const;
    TBool  ParentHasDoNotDrawFlag();

    /**
     * Method for sending two key events instead of one key event. Used for emulation of diagonal keys
     */
    TKeyResponse HandleDiagonalKeysL(const TKeyEvent& aKeyEvent, TEventCode aType,
                                     TBool aTraversalDoIt, const TUint key1, const TStdScanCode scanCode1,
                                     const TUint key2, const TStdScanCode scanCode2);

    void CreateFocusBackgroundL();

    /**
     * Common tasks for size and location changes.
     */
    void HandleRectChanged();

// From MCoeControlBackground
public:
    IMPORT_C void Draw(CWindowGc& aGc, const CCoeControl& aControl, const TRect& aRect) const;

// From MSwtDrawable
public:
    IMPORT_C MSwtGc* NewGcL();
    IMPORT_C CBitmapDevice& GraphicsDevice();
    IMPORT_C void HandleUpdate();
    static void DestroyNativeGc(CBitmapContext* aGc);

// From MSwtControl
public:
    IMPORT_C TRect ClientRect() const;
    IMPORT_C TSwtPeer Dispose();
    IMPORT_C TSwtPeer JavaPeer() const;
    IMPORT_C TUint Flags() const;
    IMPORT_C void  Paint(const TRect& aRect,
                         TBool aPaintNatively = ETrue,
                         MSwtGc* aGc = NULL,
                         TInt* aIntParams = NULL,
                         TInt aIntCount = 0,
                         HBufC* aStrParams = NULL) const;
    IMPORT_C void  PaintUrgently(const TRect& aRect, TBool aInitDraw = ETrue) const;
    IMPORT_C TBool IsPaintingUrgently() const;
    IMPORT_C void  HandlePointerEventL(const TPointerEvent& aPointerEvent);
    IMPORT_C TBool IsFocusable(TInt aReason = KSwtFocusByApi) const;
    IMPORT_C TBool SetSwtFocus(TInt aReason = KSwtFocusByApi);
    IMPORT_C TInt  FocusBackgroundPolicy() const;
    IMPORT_C TBool GetEnabled() const;
    IMPORT_C TBool GetVisible() const;
    IMPORT_C TBool GetVisibleRecursively() const;
    IMPORT_C TRect GetBounds() const;
    IMPORT_C TInt  GetBorderWidth() const;
    IMPORT_C TBool IsDefaultBackgroundUse() const;
    IMPORT_C MAknsControlContext* GetControlContext() const;
    IMPORT_C TRgb  GetBackground() const;
    IMPORT_C TRgb  GetForeground() const;
    IMPORT_C void  SetBackgroundL(const MSwtColor* aColor);
    IMPORT_C void  SetForegroundL(const MSwtColor* aColor);
    IMPORT_C const MSwtFont* GetFont() const;
    IMPORT_C TPoint GetLocation() const;
    IMPORT_C const MSwtMenu* GetPopupMenu() const;
    IMPORT_C const MSwtMenu* GetControlMenu() const;
    IMPORT_C MSwtComposite* GetParent() const;
    IMPORT_C TBool IsDescentOf(const MSwtControl& aParent) const;
    IMPORT_C MSwtShell& GetShell() const;
    IMPORT_C MSwtShell& GetTopShell() const;
    IMPORT_C MSwtCaptionedControl* GetTopCaptionedControl() const;
    IMPORT_C TSize GetWidgetSize() const;
    IMPORT_C void  SetWidgetSize(const TSize& aSize);
    IMPORT_C TBool IsFocusControl();
    IMPORT_C void  MoveAbove(const MSwtControl* aControl);
    IMPORT_C void  MoveBelow(const MSwtControl* aControl);
    IMPORT_C void  Redraw();
    IMPORT_C void  RedrawRegion(const TRect& aRect);
    IMPORT_C void  SetRedraw(TBool aRedraw);
    IMPORT_C void  SetFontL(const MSwtFont* aFont);
    IMPORT_C void  SetVisible(TBool aVisible);
    IMPORT_C void  SetLocation(const TPoint& aPoint);
    IMPORT_C void  SetEnabled(TBool aEnabled);
    IMPORT_C void  SetBounds(const TRect& aRect);
    IMPORT_C void  SetPopupMenuL(const MSwtMenu* aMenu);
    IMPORT_C void  SetCapture(TBool aCapture);
    IMPORT_C TInt  Style() const;
    IMPORT_C void  Update() const;
    IMPORT_C void  ComputeTabListL(RSwtControlArray& aList);
    IMPORT_C TPoint ToControl(const TPoint& aPoint) const;
    IMPORT_C TPoint ToDisplay(const TPoint& aPoint) const;
    IMPORT_C TBool CbaButtonIsUsed(TInt aCbaButtonIndex) const;
    IMPORT_C TBool IsKeyUsed(TUint aKeyCode) const;
    IMPORT_C TBool MSKSelCmdEnabled() const;
    IMPORT_C HBufC* MSKLabelL() const;
    IMPORT_C TBool ForceTraverseEvent(TSwtTraversal aDetail, TBool aDoIt);
    IMPORT_C void  AddToControlStackL(TInt aPriority);
    IMPORT_C void  RemoveFromControlStack();
    IMPORT_C TBool IsOnControlStack() const;
    IMPORT_C void  UpdateDoNotDrawFlag();
    IMPORT_C TBool HasDoNotDrawFlag() const;
    void HideSilently() {};
    void ShowSilently() {};
    TBool HiddenSilently() const
    {
        return GetShell().Control()->HiddenSilently();
    };
    IMPORT_C TBool IsLongTapAnimationCandidate(const TPointerEvent& aPointerEvent) const;
    IMPORT_C void DoControlSpecificFeedback(const TBool& aFirstTap,
                                            const TBool& aTappedToChildRect, const TPointerEvent& aPointerEvent) const;
    IMPORT_C void PostMouseEventL(const TPointerEvent& aPointerEvent);
    IMPORT_C void EnableFocusHighlight(TBool aEnable);
    IMPORT_C TRect VisibleRect(TBool aVisibleBounds = EFalse) const;

protected:
    MSwtDisplay& iDisplay;
    const TSwtPeer iPeer;           // The Java peer object
    MSwtComposite* const iParent;   // The parent control, not owned, may be NULL if we are a Shell
    mutable MSwtShell* iShell;      // The first Shell parent up the parent tree.
    TUint iCtrlFlags;               // Misc. flags
    TInt iStyle;                    // The SWT style
    const MSwtFont* iFont;          // The font used in the control, may be NULL, ref counted.
    const MSwtMenu* iPopupMenu;     // The popup menu of the control, may be NULL, not owned.
    TBool iIsOnControlStack;        // True if this control is currently on the AppUi's Control Stack (needed by TextBase to be FEP aware).
    TInt  iStackedSetRedrawFalseNb; // The number of setRedraw(true) must be equal to the number of setRedraw(false) to allow drawing
    TBool iBackgrdDefaultUsed;
    CAknsFrameBackgroundControlContext* iFocusFrame; // Frame context used to draw focus highlight background.
    TBool iTraverse;                // True when a keypress causes traversing
    TPoint iPrevSwtPos;             // Used by HandleSizeChanged and HandlePositionChanged only
    TPoint iPrevCoePos;             // Used by HandleSizeChanged and HandlePositionChanged only
    TSize  iPrevCoeSize;            // Used by HandleSizeChanged and HandlePositionChanged only

    // True if control changed its focus.
    // Currently used for advanced tactile feedback.
    TBool iFocusChanged;

    /**
     * Used to implement visual pressed down feedback for some controls.
     */
    TBool iPressed;
};


inline const MSwtFont* ASwtControlBase::Font() const
{
    return iFont;
}


#endif // SWTCONTROLBASE_H
