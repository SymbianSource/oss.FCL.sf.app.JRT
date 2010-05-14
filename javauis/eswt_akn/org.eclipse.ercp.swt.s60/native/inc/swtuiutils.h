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


#ifndef SWTUIUTILS_H
#define SWTUIUTILS_H


#include <AknPopupFader.h>
#include "eswtwidgetscore.h"


class CAknNavigationDecorator;


/**
 * CSwtUiUtils
 * @lib eswt.dll
 */
NONSHARABLE_CLASS(CSwtUiUtils)
        : public CBase
        , public MSwtUiUtils
        , public MEikStatusPaneObserver
        , public MAknFadedComponent
{
public:
    /**
     * Two phase constructor
     */
    static CSwtUiUtils* NewL(MSwtDisplay& aDisplay);

    /**
     * Destructor
     */
    ~CSwtUiUtils();

    /**
     * Register shell.
     * @param aShell
     */
    void RegisterShellL(MSwtShell& aShell);

    /**
     * Unregister shell.
     * @param aShell
     */
    void UnregisterShell(MSwtShell& aShell);

    /**
     * Try setting a shell active. See CanBeActive().
     * @param aShell The Shell to be made active.
     * @param aSetFocus Instructs the newly activated Shell to try and set
     *        the focus on one of its children.
     * @return whether the Shell was actually made active.
     */
    TBool SetActiveShell(MSwtShell& aShell, TBool aSetFocus);

    /**
     * Test if a shell can become active taking into account the current
     * active shell. For instance only modal Shells can become active
     * when there is already an active Shell.
     * @param aShell
     * @param aStrictCheck controls if modality is a factor
     */
    TBool CanBeActive(MSwtShell& aShell, TBool aStrictCheck = ETrue) const;

    /**
     * Test if a shell can be displayed as top most (behind active child).
     * @param aShell
     */
    TBool CanBeTopMost(MSwtShell& aShell) const;

    /**
     * Inform that the shell's activity may have changed.
     * @param aShell
     */
    void ShellActivabilityChanged(MSwtShell& aShell);

    /**
     * Sets next focused control. This function is called when app lost focus, which stores
     * a pointer of control that can be focused when app gains focus again.
     * Calling with NULL of course unsets.
     * @param aNextFocusedControl The pointer of control that can be focused when app gains focus again.
     */
    void SetNextFocusedControl(MSwtControl* aNextFocusedControl);

    /**
     * Returns the control that will gain focus when app gains focus again.
     */
    MSwtControl* NextFocusedControl();

    /**
     * Changes the application focus and sets the given shell to be active.
     * @param aShell The shell to be activated after gaining to focus.
     * @param aForeground brings application to foreground if True
     */
    void ChangeAppFocus(MSwtShell& aShell, TBool aForeground);

    /**
     * Browser engine dll loading must be done dynamically due to WSD.
     * This function returns the interface creation function of the dll.
     * The browser dll is loaded the first time this function is called.
     */
    TLibraryFunction BrowserDllEntryL();

    /**
     * Returns number of active browsers in applicaition.
     */
    TInt ActiveBrowserCount() const;

    /**
     * Notifies UiUtils that new instance of Browser was created
     * and therefore previous garbage browser (and its shell
     * if neccessary) can be removed.
     */
    void NotifyNewBrowserCreated();

    /**
     * Browser instance must be deleted only just before the application exits.
     * If attempted earlier, the browser engine dll will crash due to WSD.
     * This function sets the browser instance to be kept in limbo till app exit.
     */
    void GarbageCollect(MSwtBrowser& aBrowser);

    /**
     * Aside with browser we must collect also the window that contains it.
     */
    void GarbageCollect(MSwtShell& aShell);

private:
    /**
     * First phase constructor.
     */
    CSwtUiUtils(MSwtDisplay& aDisplay);

    /**
     * Second phase constructor.
     */
    void ConstructL();

    /**
     * Set active shell.
     * @param aShell
     * @param aSetFocus
     */
    void DoSetActiveShell(MSwtShell& aShell, TBool aSetFocus);

    /**
     * Search for a shell capable of being active and activate it. The Shell
     * list is searched in reverse order because this should match the
     * Shells Z-order.
     * @return TBool If no Shell found, return EFalse.
     */
    TBool FindAndActivateShell();

    /**
     * Test if a shell is application modal.
     * @param aShell
     * @return TBool
     */
    static TBool IsApplicationModal(MSwtShell& aShell);

    /**
     * Free memory from eSwt Browser and its children.
     * @param aComposite The pointer to composite
     * @return void
     */
    void FreeBrowserRamL(MSwtComposite* aComposite, TInt aEventType);

    /**
     * Move a Shell at the end of the Shell queue.
     */
    void MoveToEndOfQueue(MSwtShell& aShell);

    /**
     * Highest inline control default font. See InlineReferenceFontHeight.
     */
    const CFont& InlineReferenceFont() const;

    void HideIndicator(TInt aId);
    void HideIndicators();

// From MSwtUiUtils
public:
    TRgb GetSystemColor(TSwtColorId aId) const;
    TFontSpec SystemFontSpec() const;
    TInt RunMessageBoxDlgLD(MSwtDisplay& aDisplay, const TDesC& aTitle,
                            const TDesC& aMessage, TInt aStyle) const;
    MSwtShell* GetActiveShell() const
    {
        return iActiveShell;
    };
    MSwtShell* GetParentTopShellOfActiveShell() const;
    MSwtShell* TopMostTopShell() const;
    TSwtPeer GetActiveShellPeer() const;
    inline const RPointerArray<MSwtShell>& Shells() const;
    CSwtPeerArray* GetShellPeersL() const;
    TSwtPeer GetFocusControl() const;
    MSwtShell& GetControlTopShell(const MSwtControl& aControl) const;
    void DrawFocusRect(MSwtGc& aGc, const TRect& aRect) const;
    CEikButtonGroupContainer& Cba() const;
    void UpdateStatusPaneL();
    TBool IsAppFocused() const;
    const HBufC* GetCommandOkTitleL() const;
    const HBufC* GetCommandCancelTitleL() const;
    TInt  GetPointerEventStateMask(TUint aPointerEventModifiers) const;
    MSwtShell* FadingShell() const
    {
        return iFadingShell;
    };
    TMargins8 InlineReferencePadding() const;
    TInt InlineReferenceFontHeight() const;
    void SetAppNameL(const TDesC* aAppName);
    TSize GetBestImageSize(TInt aImageType);
    void HandleResourceChangedL(TInt aType);
    TInt ScrollBarBreadth(const CEikScrollBar* aBar);
    TBool NaviKeyInput() const;
    void SetNaviKeyInput(TBool aStatus);
    void SetCbaVisible(TBool aVisible);
    MSwtControl* GetPointerGrabbingControl();
    TRect TaskTipRect() const;
    void SetPointerCaptureControl(MSwtControl* aControl);
    MSwtControl* PointerCaptureControl();

protected:
    void HandleFreeRamEventL(TInt aEventType);

// From MSwtAppFocusObserver
public:
    void HandleAppFocusChangeL(TBool aFocused);

// From MEikStatusPaneObserver
public:
    void HandleStatusPaneSizeChange();

private:
    void SetShellFade(MSwtShell* aShell, TBool aStatus);
    TBool CanBeFaded(MSwtShell* aShell) const;

// From MAknFadedComponent
public:
    TInt CountFadedComponents();
    CCoeControl* FadedComponent(TInt aIndex);

// Data
private:
    /**
     * Display reference needed for getting CEikEnv for now.
     */
    MSwtDisplay& iDisplay;

    /**
     * All shells.
     * Not own.
     */
    RPointerArray<MSwtShell> iShells;

    /**
     * The currently active shell. Can be NULL.
     * Not own.
     */
    MSwtShell* iActiveShell;

    /**
     * The next shell to be made active. Can be NULL.
     * See HandleFocusLost(), HandleFocusLost()
     * Not own.
     */
    MSwtShell* iNextActiveShell;

    /**
     * The next control to focus.
     * Not own.
     */
    MSwtControl* iNextFocusedControl;

    /**
     * Focus lost flag.
     * See HandleFocusLost(), HandleFocusLost().
     */
    TBool iFocusLost;

    /**
     * CBA, cannot be NULL
     * Own.
     */
    CEikButtonGroupContainer* iCba;

    /**
     * The NaviDecorator which contain the text set by SetNaviPaneText()
     * Not own.
     */
    CAknNavigationDecorator* iNaviDecorator;

    /**
     * The Status Pane
     * Not own.
     */
    CEikStatusPane* iStatusPane;

    /**
     * Pointer to the control that has been
     * set pointer capturing with setCapture.
     * Not Own
     */
    MSwtControl* iPointerCaptureControl;

    /**
     * Browser engine dll.
     */
    RLibrary iBrowserEngineLib;

    /**
     * Entry point of browser engine dll.
     */
    TLibraryFunction iBrowserDllEntry;

    /**
     * Number of currently existing browser instances.
     */
    TInt iCurrentBrowserCount;

    /**
     * Browser instance must be deleted only just before the application exits.
     * If attempted earlier, the browser engine dll will crash due to WSD.
     * This is the browser instance to be kept in limbo till app exit.
     */
    MSwtBrowser* iBrowserGarbage;

    /**
     * Shell instance to be also garbage collected together with browser.
     * The browser needs to have the parent window alive till destroyed.
     */
    MSwtShell* iBrowserShellGarbage;

    /**
     * Flag signalling whether Browser's parent shell was requested to
     * collect.
     */
    TBool iShellGarbageCollectionRequested;

    /**
     * The string of status text separator.
     */
    HBufC* iSeparatorString;

    /**
     * Keep track of top most shells.
     */
    MSwtShell* iLastTopMostShell;

    /**
     * Used to fade the whole system behind modal child shells.
     */
    TAknPopupFader iFader;

    /**
     * The Shell which fades behind.
     */
    MSwtShell* iFadingShell;

    /**
     * Used by display appName to set the title for status pane.
     * Own.
     */
    TDesC* iAppName;

    /**
     * Cached scrollbar breadth. Valid till next resolution change.
     */
    TInt iScrollBarBreadth;

    /**
     * Inline reference stuff
     */
    mutable const CFont* iInlineFont;
    mutable TInt iInlineFontHeight;
    mutable TMargins8 iInlinePadding;

    /**
     * Key input flag.
     */
    TBool iNaviKeyInput;
};

/**
 * Return the existing shells
 */
inline const RPointerArray<MSwtShell>& CSwtUiUtils::Shells() const
{
    return iShells;
}


#endif // SWTUIUTILS_H
