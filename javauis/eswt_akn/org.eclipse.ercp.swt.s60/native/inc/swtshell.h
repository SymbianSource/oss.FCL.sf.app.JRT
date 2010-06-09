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


#ifndef SWTSHELL_H
#define SWTSHELL_H


#include "swtdecorations.h"


/**
 * CSwtShell
 * @lib eswt
 */
NONSHARABLE_CLASS(CSwtShell)
        : public CSwtDecorations
        , public MSwtShell
{
// Own methods.
public:
    static CSwtShell* NewL(MSwtDisplay& aDisplay, TSwtPeer aPeer,
    MSwtShell* aParent, TInt aStyle);
protected:
    CSwtShell(MSwtDisplay& aDisplay, TSwtPeer aPeer,
              MSwtShell* aParent, TInt aStyle);
    virtual ~CSwtShell();
    void ConstructL();
    MSwtShell& GetTopShell() const;
    void RemoveAndRememberFocus();
    
    // Relocating an editor's Shell must be delegated to UiUtils 
    // while split editing is on (@see SetSplitInputShellPos)
    // CCoeControl::SetRect must not be called directly.
    void DoSetRect(const TRect& aRect);

// From CSwtComposite
protected:
    void SwtHandleResourceChangeL(TInt aType);

// From ASwtControlBase
protected:
    void DoPaint(const TRect& aRect) const;
    TRect BorderInnerRect() const;
    TRect BorderOuterRect(const TRect& aRect) const;
    void DoDelete();

// From CCoeControl
public:
    TKeyResponse OfferKeyEventL(const TKeyEvent& aKeyEvent, TEventCode aType);
    void HandlePointerEventL(const TPointerEvent& aPointerEvent);
    void MakeVisible(TBool aVisible);
    void SetDimmed(TBool aDimmed);
protected:
    void FocusChanged(TDrawNow aDrawNow);
    void Draw(const TRect& aRect) const;
    void SizeChanged();

// From MCoeControlBackground
public:
    void Draw(CWindowGc& aGc, const CCoeControl& aControl, const TRect& aRect) const;

// From MSwtControl
public:
    TBool IsFocusable(TInt aReason = KSwtFocusByApi) const;
    TBool SetSwtFocus(TInt aReason = KSwtFocusByApi);
    TBool GetVisible() const;
    TRect GetBounds() const;
    TPoint GetLocation() const;
    void Redraw();
    void SetLocation(const TPoint& aPoint);
    void SetWidgetSize(const TSize& aSize);
    void SetBounds(const TRect& aRect);
    TBool IsFocusControl();
    void MoveAbove(const MSwtControl* aControl);
    void MoveBelow(const MSwtControl* aControl);
    MSwtShell* ShellInterface()
    {
        return this;
    };
    const MSwtShell* ShellInterface() const
    {
        return this;
    };
    void HideSilently();
    void ShowSilently();
    inline TBool HiddenSilently() const
    {
        return iSilentlyHidden;
    };
    void SetVisible(TBool aVisible);

// From MSwtComposite
public:
    void ChildFocusChanged(MSwtControl& aControl);

// From MSwtDecorations
public:
    void SetMaximized(TBool aMaximized);
    void SetMinimized(TBool aMinimized);
    TBool GetMinimized() const;
    void SetTextL(const TDesC& aText);
    const TDesC* GetText() const;
    void SetImageL(const MSwtImage* aImage);
    void SetMenuBar(const MSwtMenu* aMenuBar);

// From MSwtShell
public:
    MSwtDecorations* Decorations();
    MSwtShell* GetParentShell();
    void ForceActive();
    CSwtPeerArray* GetChildShellsPeersL();
    MSwtDisplay& Display() const;
    TBool FullScreenMode() const;
    TInt GetImeInputMode() const;
    TSize GetMinimumSize() const;
    void SetImeInputMode(TInt aMode);
    void SetMinimumSize(const TSize& aMinimumSize);
    void SetStyle(TInt aStyle);
    const RSwtControlArray& GetFullTabListL();
    void ResetTabList();
    MSwtControl* FindTraversalTargetL(TInt aDetail,
                                      MSwtControl& aFocusControl, RSwtControlArray* aExcludedControls = NULL);
    void SetFocusControl(MSwtControl* aControl);
    TBool HasModalChild() const;
    void ActivityChanged(TBool aIsActive, TBool aSetFocus);
    void SetZPosition(TInt aZPosition);
    MSwtControl* FocusControl() const;
    void SetDefaultCommand(const MSwtCommand* aCommand);
    const MSwtCommand* DefaultCommand() const;
    void SetFullScreen(TBool aFullScreen);
    void SetControlGoingToStack(MSwtControl* aControl);
    MSwtControl* ControlGoingToStack() const;
    void SetControlGainingFocus(MSwtControl* aControl);
    MSwtControl* ControlGainingFocus() const;
    MSwtShell::TSwtStatusPaneStyle StatusPaneStyle() const
    {
        return MSwtShell::EStyleLargeStatusPane;
    };
    virtual TRect DefaultBounds() const;
    void SetToBeFocusedControl(MSwtControl* aControl);
    void CheckFocusMemory(MSwtControl& aUnregisteredControl);
    void SetUrgentPaintControl(MSwtControl* aControl);
    MSwtControl* UrgentPaintControl() const;
    TBool AsyncPainting() const;
    void SetAsyncPainting(TBool aStatus);
    void InstallCba();
    TBool HasCba() const;
    void SetTaskTip();
    TBool IsTaskTip() const;
    void DoSetLocation(const TPoint& aPoint);

// From MEikStatusPaneObserver
public:
    void HandleStatusPaneSizeChange();

private:
    void FinishRedraw() const;

// Data
private:
    /**
     * This Shell's focus control, may be NULL.
     * Not own.
     */
    MSwtControl* iFocusControl;

    /**
     * This Shell's focus control before it got deactivated, may be NULL.
     * Not own.
     */
    MSwtControl* iFocusMemory;

    /**
     * A pointer to the child control of this shell, which should be added
     * to the Appui's Control Stack, if needed. May be NULL.
     * Not own.
     */
    MSwtControl* iControlGoingToStack;

    /**
     * A pointer to the child control of this shell, which should be set
     * focused, if needed. May be NULL.
     * Not own.
     */
    MSwtControl* iControlGainingFocus;

    /**
     * This Shell's tab-list.
     */
    RSwtControlArray iFullTabList;

    /**
     * This Shell's default command, may be NULL.
     * Not own.
     */
    const MSwtCommand* iDefaultCommand;

    /**
     * Whether or not this Shell is full-screen
     */
    TBool iFullScreen;

    /**
     * The minimum size, not used for top-level shells
     */
    TSize iMinSize;

    /**
     * The value of the style at the construction. When the shell is set
     * to fullscreen, iStyle is modified. iSavedStyle is used to recover
     * original style when get back to non fullscreen mode.
     */
    TInt iSavedStyle;

    /**
     * Used by top level shell, because all top level shells share the
     * Title pane of the appUI.
     * Own.
     */
    TDesC* iTitleText;

    /**
     * Whether or not draw method is called by eSWT drawing framework
     * and not by Symbian drawing framework.
     */
    TBool iDrawCalledByOurFrwk;

    /**
     * Original window priority
     */
    TInt iWindowPriority;

    /**
     * Tells if the Shell has been sent to background while inactive.
     */
    TBool iSilentlyHidden;

    /**
     * While this is not null, no paint events are delivered to Java and all
     * native paint requests received in ::Draw() are served immediately.
     * Only the urgently painted control and its children are drawn.
     */
    MSwtControl* iUrgentPaintControl;

    /**
     * If there are no paint listeners, no painting trough Java. By default
     * this is set to EFalse. If any of the children registers a paint listener,
     * this goes ETrue.
     */
    TBool iAsyncPainting;

    /**
     * Once turned on can never be turned off.
     */
    TBool iHasCba;

    /**
     * Task tip style.
     */
    TBool iIsTaskTip;
};

#endif // SWTSHELL_H
