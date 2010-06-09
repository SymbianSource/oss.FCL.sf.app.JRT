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


#include <bautils.h>
#include <eikon.hrh>
#include <eikbtgpc.h>
#include <eikenv.h>
#include <avkon.rsg>
#include <aknenv.h>
#include <akncontext.h>
#include <akntitle.h>
#include <aknnavide.h>
#include <AknsUtils.h>
#include <eswtcore.rsg>
#include <AknUtils.h>
#include <AknsDrawUtils.h>
#include <AknTextDecorationMetrics.h>
#ifdef RD_JAVA_S60_RELEASE_9_2
#include <AknPriv.hrh>
#include <aknappui.h>
#endif
#include <coreuiavkoneswt.h>
#include <swtlaffacade.h>
#include "eswtmobileextensions.h"
#include "eswtwidgetsexpanded.h"
#include "swtcontrolhelper.h"
#include "swtmessagebox.h"
#include "swtuiutils.h"
#include "swtbrowser.h"
#include "swtdisplaybase.h"

_LIT(KSeparator, "-");
const TInt KVariationFullScreen = 3;

// ======== MEMBER FUNCTIONS ========


// ---------------------------------------------------------------------------
// CSwtUiUtils::NewL
// ---------------------------------------------------------------------------
//
CSwtUiUtils* CSwtUiUtils::NewL(MSwtDisplay& aDisplay)
{
    CSwtUiUtils* self = new(ELeave) CSwtUiUtils(aDisplay);
    CleanupStack::PushL(self);
    self->ConstructL();
    CleanupStack::Pop(self);
    return self;
}

// ---------------------------------------------------------------------------
// CSwtUiUtils::~CSwtUiUtils
// ---------------------------------------------------------------------------
//
CSwtUiUtils::~CSwtUiUtils()
{
    ASSERT(iShells.Count() == 0);
    iShells.Close();

    delete iCba;
    iCba = NULL;

    iStatusPane = NULL;

    delete iNaviDecorator;
    iNaviDecorator = NULL;

    iActiveShell = NULL;
    iNextActiveShell = NULL;
    iNextFocusedControl = NULL;

    if (iAppName)
    {
        delete iAppName;
        iAppName = NULL;
    }

    if (iBrowserGarbage)
    {
        iBrowserGarbage->Control()->Destroy();
        iBrowserEngineLib.Close();
    }
    if (iBrowserShellGarbage)
    {
        iBrowserShellGarbage->Control()->Destroy();
    }
}

// ---------------------------------------------------------------------------
// CSwtUiUtils::RegisterShellL
// ---------------------------------------------------------------------------
//
void CSwtUiUtils::RegisterShellL(MSwtShell& aShell)
{
    ASSERT(iShells.Find(&aShell) == KErrNotFound);
    ASSERT(!CanBeActive(aShell));     // strict check
    User::LeaveIfError(iShells.Append(&aShell));
    aShell.Control()->HideSilently();
}

// ---------------------------------------------------------------------------
// CSwtUiUtils::UnregisterShell
// ---------------------------------------------------------------------------
//
void CSwtUiUtils::UnregisterShell(MSwtShell& aShell)
{
    TInt index = iShells.Find(&aShell);
    if (index == KErrNotFound)
    {
        return;
    }
    iShells.Remove(index);

    // If the application has focus and the unregistered shell was
    // active, find a new active shell. If the application is out of
    // focus, and this was the next shell to become active, clear it.
    // A new active shell will be searched when focus is gained.
    if (iFocusLost)
    {
        if (iNextActiveShell == &aShell)
        {
            iNextActiveShell = NULL;
        }
    }
    else
    {
        if (iActiveShell == &aShell)
        {
            SetShellFade(iActiveShell, EFalse);
            iActiveShell = NULL;
            FindAndActivateShell();
        }
    }
}

// ---------------------------------------------------------------------------
// CSwtUiUtils::SetActiveShell
// ---------------------------------------------------------------------------
//
TBool CSwtUiUtils::SetActiveShell(MSwtShell& aShell, TBool aSetFocus)
{
    if (iActiveShell == &aShell)
    {
        return ETrue;
    }

    if (CanBeActive(aShell))     // strict check
    {
        if (iFocusLost)
        {
            // Application out of focus, remember the next active shell.
            iNextActiveShell = &aShell;
            return EFalse;
        }
        else
        {
            // Application in focus, let the shell be active.
            DoSetActiveShell(aShell, aSetFocus);
            return ETrue;
        }
    }

    return EFalse;
}

// ---------------------------------------------------------------------------
// CSwtUiUtils::CanBeActive
// ---------------------------------------------------------------------------
//
TBool CSwtUiUtils::CanBeActive(MSwtShell& aShell, TBool aStrictCheck /*=ETrue*/) const
{
    MSwtControl& ctrl = *aShell.Control();

    // Must not have the NO_TRIM style (see 11.4.11 of Steve Northover's book on SWT)
    if (ctrl.Style() & KSwtStyleNoTrim)
    {
        return EFalse;
    }

    // Must be visible and enabled
    CCoeControl& coeCtrl = ctrl.CoeControl();
    if (!coeCtrl.IsVisible() || coeCtrl.IsDimmed())
    {
        return EFalse;
    }

    // If the new shell is modal or the parent of the new shell is the current
    // active modal shell then the new shell can still become active
    if (aStrictCheck && iActiveShell && IsApplicationModal(*iActiveShell))
    {
        if (!IsApplicationModal(aShell))
        {
            if (!aShell.Control()->IsDescentOf(*iActiveShell->Control()))
            {
                return EFalse;
            }
        }
    }

    // Same as above, while in background
    if (iFocusLost && aStrictCheck && iNextActiveShell && IsApplicationModal(*iNextActiveShell))
    {
        if (!IsApplicationModal(aShell))
        {
            if (!aShell.Control()->IsDescentOf(*iNextActiveShell->Control()))
            {
                return EFalse;
            }
        }
    }

    return ETrue;
}

// ---------------------------------------------------------------------------
// CSwtUiUtils::CanBeTopMost
// ---------------------------------------------------------------------------
//
TBool CSwtUiUtils::CanBeTopMost(MSwtShell& aShell) const
{
    MSwtControl& ctrl = *aShell.Control();

    // Must be visible and enabled
    CCoeControl& coeCtrl = ctrl.CoeControl();
    if (!coeCtrl.IsVisible())
    {
        return EFalse;
    }

    return ETrue;
}

// ---------------------------------------------------------------------------
// CSwtUiUtils::ShellActivabilityChanged
// Called from CSwtShell::MakeVisible and CSwtShell::SetDimmed
// ---------------------------------------------------------------------------
//
void CSwtUiUtils::ShellActivabilityChanged(MSwtShell& aShell)
{
    // Use the next active shell during no focus.
    MSwtShell* activeShell = NULL;
    if (iFocusLost)
    {
        activeShell = iNextActiveShell;
    }
    else
    {
        activeShell = iActiveShell;
    }

    // 1. Activate Shell
    // The new shell can become active if no shell was previously active
    // or if it is modal. This, only when setVisible() or setEnabled()
    // is called. If setActive() is called and the shell can become
    // active it will be set active anyway.
    if (CanBeActive(aShell))     // strict check
    {
        if (activeShell == &aShell)
        {
            return;
        }

        if (!activeShell || IsApplicationModal(aShell))
        {
            if (iFocusLost)
            {
                iNextActiveShell = &aShell;
            }
            else
            {
                DoSetActiveShell(aShell, ETrue);
            }
            return;
        }
    }

    // 2. Show Shell if it is a parent or a child of the currently active shell
    if (activeShell && activeShell != &aShell && aShell.Control()->CoeControl().IsVisible())
    {
        if (activeShell->Control()->IsDescentOf(*aShell.Control()))
        {
            // The active shell is a child of aShell
            MoveToEndOfQueue(aShell);
            MoveToEndOfQueue(*activeShell);
            aShell.Control()->ShowSilently();
            if (!aShell.GetParentShell() && iLastTopMostShell && iLastTopMostShell != &aShell)
            {
                iLastTopMostShell->Control()->HideSilently();
                // No need to remember the top most Shell till next activity change
                iLastTopMostShell = NULL;
            }
            TRAP_IGNORE(UpdateStatusPaneL());
            return;
        }
        if (aShell.Control()->IsDescentOf(*activeShell->Control()))
        {
            // The active shell is parent of aShell
            // The queue sorting is not important for child Shells.
            aShell.Control()->ShowSilently();
            return;
        }
    }

    // 3. Pick new active Shell if the active shell cannot be active anymore.
    // If the application is out of focus, let another shell become active
    // when focus is regained, otherwise deactivate and find new shell.
    if (activeShell == &aShell)
    {
        if (iFocusLost)
        {
            iNextActiveShell = NULL;
        }
        else
        {
            // Ancestors have priority in becoming active
            MSwtShell* parent = activeShell->GetParentShell();
            while (parent)
            {
                if (CanBeActive(*parent, EFalse))     // loose check
                {
                    DoSetActiveShell(*parent, ETrue);
                    break;
                }
                parent = parent->GetParentShell();
            }
            if (!parent)
            {
                if (!FindAndActivateShell())
                {
                    // Lost the last activateable Shell.
                    activeShell->ActivityChanged(EFalse, ETrue);
                    SetShellFade(activeShell, EFalse);
                    iActiveShell = NULL;
                    TRAP_IGNORE(UpdateStatusPaneL());
                }
            }
        }
    }

    // 4. Top Shells loosing ability of being topmost
    if (iLastTopMostShell && iLastTopMostShell == &aShell && !CanBeTopMost(aShell))
    {
        iLastTopMostShell = NULL;
        TRAP_IGNORE(UpdateStatusPaneL());
    }
}

// ---------------------------------------------------------------------------
// CSwtUiUtils::SetNextFocusedControl
// ---------------------------------------------------------------------------
//
void CSwtUiUtils::SetNextFocusedControl(MSwtControl* aNextFocusedControl)
{
    iNextFocusedControl = aNextFocusedControl;
}

// ---------------------------------------------------------------------------
// CSwtUiUtils::NextFocusedControl
// ---------------------------------------------------------------------------
//
MSwtControl* CSwtUiUtils::NextFocusedControl()
{
    return iNextFocusedControl;
}

// ---------------------------------------------------------------------------
// CSwtUiUtils::ChangeAppFocus
// ---------------------------------------------------------------------------
//
void CSwtUiUtils::ChangeAppFocus(MSwtShell& aShell, TBool aForeground)
{
    CCoeEnv* coeEnv = iDisplay.CoeEnv();
    TApaTask task(coeEnv->WsSession());
    task.SetWgId(coeEnv->RootWin().Identifier());
    if (aForeground)
    {
        iNextActiveShell = &aShell;
        task.BringToForeground();
    }
    else
    {
        task.SendToBackground();
    }
}

// ---------------------------------------------------------------------------
// CSwtUiUtils::BrowserDllEntry
// ---------------------------------------------------------------------------
//
TLibraryFunction CSwtUiUtils::BrowserDllEntryL()
{
    if (iBrowserDllEntry != NULL)
    {
        return iBrowserDllEntry;
    }
    if (KErrNone != iBrowserEngineLib.Load(_L("BrowserEngine.dll")))
    {
        User::Leave(KErrNoMemory);
    }
#ifdef __WINS__
    iBrowserDllEntry = iBrowserEngineLib.Lookup(10);
#else
    iBrowserDllEntry = iBrowserEngineLib.Lookup(1);
#endif
    return iBrowserDllEntry;
}

// ---------------------------------------------------------------------------
// CSwtUiUtils::ActiveBrowserCount
// ---------------------------------------------------------------------------
//
TInt CSwtUiUtils::ActiveBrowserCount() const
{
    return iCurrentBrowserCount;
}

// ---------------------------------------------------------------------------
// CSwtUiUtils::RegisterBrowser
// ---------------------------------------------------------------------------
//
void CSwtUiUtils::NotifyNewBrowserCreated()
{
    if (iBrowserGarbage)
    {
        iBrowserGarbage->Control()->Destroy();
        iBrowserGarbage = NULL;
        if (iShellGarbageCollectionRequested && iBrowserShellGarbage)
        {
            // There was request to destroy previous browser's shell,
            // so we destroy it now, when browser is gone
            iBrowserShellGarbage->Control()->Destroy();
            iShellGarbageCollectionRequested = EFalse;
            iBrowserShellGarbage = NULL;
        }
        else
        {
            // Old garbage shell was not requested to destroy, so there
            // will be new garbage when current browser is being destroyed.
            iBrowserShellGarbage = NULL;
        }
    }
    iCurrentBrowserCount++;
}

// ---------------------------------------------------------------------------
// CSwtUiUtils::GarbageCollect
// ---------------------------------------------------------------------------
//
void CSwtUiUtils::GarbageCollect(MSwtBrowser& aBrowser)
{
    if (!iBrowserGarbage)
    {
        // Collect browser
        iBrowserGarbage = &aBrowser;
        iBrowserShellGarbage = &aBrowser.Control()->GetShell();
        iShellGarbageCollectionRequested = EFalse;
    }
    iCurrentBrowserCount--;
}

// ---------------------------------------------------------------------------
// CSwtUiUtils::GarbageCollect
// ---------------------------------------------------------------------------
//
void CSwtUiUtils::GarbageCollect(MSwtShell& aShell)
{
    if (&aShell != iBrowserShellGarbage)
    {
        aShell.Control()->Destroy();
    }
    else
    {
        aShell.Control()->CoeControl().MakeVisible(EFalse);
        iShellGarbageCollectionRequested = ETrue;
    }
}

// ---------------------------------------------------------------------------
// CSwtUiUtils::CSwtUiUtils
// ---------------------------------------------------------------------------
//
CSwtUiUtils::CSwtUiUtils(MSwtDisplay& aDisplay)
        : iDisplay(aDisplay)
        , iFocusLost(EFalse)
        , iCurrentBrowserCount(0)
        , iBrowserGarbage(NULL)
        , iBrowserShellGarbage(NULL)
        , iShellGarbageCollectionRequested(EFalse)
        , iScrollBarBreadth(-1)
{
}

// ---------------------------------------------------------------------------
// CSwtUiUtils::ConstructL
// ---------------------------------------------------------------------------
//
void CSwtUiUtils::ConstructL()
{
    CEikonEnv& eikEnv = *iDisplay.CoeEnv();
    MEikAppUiFactory& appUiFactory = *(eikEnv.AppUiFactory());
    CEikAppUi& appUi = *(eikEnv.EikAppUi());

    // Set the status pane
    iStatusPane = appUiFactory.StatusPane();
    ASSERT(iStatusPane);
    iStatusPane->SetObserver(NULL);
    iStatusPane->MakeVisible(EFalse);
    iStatusPane->SetObserver(this);

    // Override window priorities. See TSwtWinPriority.
    // These traps cannot be packed into a single one since
    // we do not know which status pane controls exist.
    CCoeControl* ctrl = NULL;
    RDrawableWindow* win = NULL;

    // Context pane priority (image).
    TRAP_IGNORE(ctrl = iStatusPane->ContainerControlL(
                           TUid::Uid(EEikStatusPaneUidContext)));
    if (ctrl)
    {
        win = ctrl->DrawableWindow();
        if (win)
        {
            win->SetOrdinalPosition(
                win->OrdinalPosition(),
                ESwtWinPriorityStatusPane);
        }
    }

    // Title pane priority.
    ctrl = NULL;
    TRAP_IGNORE(ctrl = iStatusPane->ContainerControlL(
                           TUid::Uid(EEikStatusPaneUidTitle)));
    if (ctrl)
    {
        win = ctrl->DrawableWindow();
        if (win)
        {
            win->SetOrdinalPosition(
                win->OrdinalPosition(),
                ESwtWinPriorityStatusPane);
        }
    }

    // Navi pane priority.
    ctrl = NULL;
    TRAP_IGNORE(ctrl = iStatusPane->ContainerControlL(
                           TUid::Uid(EEikStatusPaneUidNavi)));
    if (ctrl)
    {
        win = ctrl->DrawableWindow();
        if (win)
        {
            win->SetOrdinalPosition(
                win->OrdinalPosition(),
                ESwtWinPriorityStatusPane);
        }
    }

    // Empty pane priority.
    ctrl = NULL;
    TRAP_IGNORE(ctrl = iStatusPane->ContainerControlL(
                           TUid::Uid(EEikStatusPaneUidEmpty)));
    if (ctrl)
    {
        win = ctrl->DrawableWindow();
        if (win)
        {
            win->SetOrdinalPosition(
                win->OrdinalPosition(),
                ESwtWinPriorityStatusPane);
        }
    }

    // Indi pane priority.
    ctrl = NULL;
    TRAP_IGNORE(ctrl = iStatusPane->ContainerControlL(
                           TUid::Uid(EEikStatusPaneUidIndic)));
    if (ctrl)
    {
        win = ctrl->DrawableWindow();
        if (win)
        {
            win->SetOrdinalPosition(
                win->OrdinalPosition(),
                ESwtWinPriorityStatusPane);
        }
    }

    // Clock pane priority.
    ctrl = NULL;
    TRAP_IGNORE(ctrl = iStatusPane->ContainerControlL(
                           TUid::Uid(EEikStatusPaneUidClock)));
    if (ctrl)
    {
        win = ctrl->DrawableWindow();
        if (win)
        {
            win->SetOrdinalPosition(
                win->OrdinalPosition(),
                ESwtWinPriorityStatusPane);
        }
    }

    // Digital clock pane priority.
    ctrl = NULL;
    TRAP_IGNORE(ctrl = iStatusPane->ContainerControlL(
                           TUid::Uid(EEikStatusPaneUidDigitalClock)));
    if (ctrl)
    {
        win = ctrl->DrawableWindow();
        if (win)
        {
            win->SetOrdinalPosition(
                win->OrdinalPosition(),
                ESwtWinPriorityStatusPane);
        }
    }

    // Signal pane priority.
    ctrl = NULL;
    TRAP_IGNORE(ctrl = iStatusPane->ContainerControlL(
                           TUid::Uid(EEikStatusPaneUidSignal)));
    if (ctrl)
    {
        win = ctrl->DrawableWindow();
        if (win)
        {
            win->SetOrdinalPosition(
                win->OrdinalPosition(),
                ESwtWinPriorityStatusPane);
        }
    }

    // Battery pane priority.
    ctrl = NULL;
    TRAP_IGNORE(ctrl = iStatusPane->ContainerControlL(
                           TUid::Uid(EEikStatusPaneUidBattery)));
    if (ctrl)
    {
        win = ctrl->DrawableWindow();
        if (win)
        {
            win->SetOrdinalPosition(
                win->OrdinalPosition(),
                ESwtWinPriorityStatusPane);
        }
    }

    // Create the CBA
    iCba = CEikButtonGroupContainer::NewL(
               CEikButtonGroupContainer::ECba,
               CEikButtonGroupContainer::EHorizontal,
               &appUi,
               NULL,
               CEikButtonGroupContainer::EAddToStack);

    // The Cba will be made visible by the firtst top shell to show
    // or by the first child Shell with commands to show.
    iCba->MakeVisible(EFalse);

#ifdef RD_JAVA_S60_RELEASE_9_2
    static_cast<CEikCba*>(iCba->ButtonGroup())->EnableItemSpecificSoftkey(EFalse);
#endif

    const TSize screenSize = iDisplay.CoeEnv()->ScreenDevice()->SizeInPixels();
    iCba->SetBoundingRect(TRect(screenSize));

    iCba->SetCommandL(0, EEikBidBlank, KNullDesC);
    iCba->SetCommandL(2, EEikBidBlank, KNullDesC);
}

// ---------------------------------------------------------------------------
// CSwtUiUtils::DoSetActiveShell
// ---------------------------------------------------------------------------
//
void CSwtUiUtils::DoSetActiveShell(MSwtShell& aShell, TBool aSetFocus)
{
    ASSERT(CanBeActive(aShell, EFalse));     // loose check

    if (iActiveShell == &aShell)
    {
        return;
    }

    MSwtShell* newParentShell = aShell.GetParentShell();
    MSwtShell& newTopMostShell = aShell.Control()->GetTopShell();
    TBool keepTopMost(EFalse);

    if (iActiveShell)
    {
        iActiveShell->ActivityChanged(EFalse, EFalse);
        SetShellFade(iActiveShell, EFalse);

        // Hide current active shell (no matter if it's dialog or top shell)
        // if the new shell is a top shell or if the top parent of the new
        // dialog shell is activateable.
        if (!newParentShell || (CanBeActive(newTopMostShell, EFalse)))       // loose check
        {
            if (iActiveShell == &newTopMostShell)
            {
                // This is essential to avoid flickering.
                keepTopMost = ETrue;
            }
            else
            {
                iActiveShell->Control()->HideSilently();
            }
        }
    }

    iActiveShell = &aShell;

    // If the new active Shell is a child Shell, make sure its top parent Shell
    // and all siblings are displayed behind. ShowSilently makes sure this happens
    // only if the Shells are visible.
    if (newParentShell && !keepTopMost)
    {
        MoveToEndOfQueue(newTopMostShell);
        newTopMostShell.Control()->ShowSilently();
    }

    // Bring back on screen and activate the new Shell.
    MoveToEndOfQueue(*iActiveShell);

    TRAP_IGNORE(UpdateStatusPaneL());

    iActiveShell->Control()->ShowSilently();
    iActiveShell->ActivityChanged(ETrue, aSetFocus);

    // Remember the new top most Shell for Shell activability changes.
    iLastTopMostShell = TopMostTopShell();


    SetShellFade(iActiveShell, ETrue);
}

// ---------------------------------------------------------------------------
// CSwtUiUtils::FindAndActivateShell
// ---------------------------------------------------------------------------
//
TBool CSwtUiUtils::FindAndActivateShell()
{
    TBool res = EFalse;
    const TInt count = iShells.Count();
    for (TInt i = count - 1; i >= 0; --i)
    {
        MSwtShell* shell = iShells[i];
        if (CanBeActive(*shell, EFalse))     // loose check
        {
            DoSetActiveShell(*shell, ETrue);
            res = ETrue;
            break;
        }
    }
    return res;
}

// ---------------------------------------------------------------------------
// CSwtUiUtils::IsApplicationModal
// ---------------------------------------------------------------------------
//
TBool CSwtUiUtils::IsApplicationModal(MSwtShell& aShell)
{
    return ((aShell.Control()->Style() & KSwtModalityMask)
            != KSwtStyleModeless);
}

// ---------------------------------------------------------------------------
// CSwtUiUtils::FreeBrowserRam
// ---------------------------------------------------------------------------
//
void CSwtUiUtils::FreeBrowserRamL(MSwtComposite* aComposite, TInt aEventType)
{
    ASSERT(aComposite);
    MSwtBrowser* browser = aComposite->Control()->BrowserInterface();
    if (browser)
    {
        if ((!IsAppFocused())
                || aEventType == CSwtLafFacade::GetConstant(CSwtLafFacade::EBrowserMemoryGood))
        {
            browser->HandleFreeRamEventL(aEventType);
        }
    }
    const RSwtControlArray* children = aComposite->Children();
    if (children)
    {
        for (TInt i = 0; i < children->Count(); ++i)
        {
            MSwtControl* child = (*children)[i];
            if (child->CompositeInterface())
            {
                FreeBrowserRamL(child->CompositeInterface(), aEventType);
            }
        }
    }
}

// ---------------------------------------------------------------------------
// CSwtUiUtils::MoveToEndOfQueue
// ---------------------------------------------------------------------------
//
void CSwtUiUtils::MoveToEndOfQueue(MSwtShell& aShell)
{
    TInt index = iShells.Find(&aShell);
    ASSERT(index != KErrNotFound);
    iShells.Remove(index);
    iShells.Append(&aShell);
}

// ---------------------------------------------------------------------------
// CSwtUiUtils::InlineReferenceFont
// ---------------------------------------------------------------------------
//
const CFont& CSwtUiUtils::InlineReferenceFont() const
{
    if (!iInlineFont)
    {
        // Pick the highest among Button, DateEditor, Text.
        iInlineFont = AknLayoutUtils::FontFromId(
                          CSwtLafFacade::GetFontId(CSwtLafFacade::EFont2MidpTextPaneT1Font, 1, 0));
    }
    ASSERT(iInlineFont);
    return *iInlineFont;
}

// ---------------------------------------------------------------------------
// CSwtUiUtils::HideIndicator
// ---------------------------------------------------------------------------
//
void CSwtUiUtils::HideIndicator(TInt aId)
{
    TUid uid = TUid::Uid(aId);
    CEikStatusPaneBase::TPaneCapabilities subPane = iStatusPane->PaneCapabilities(uid);
    if (subPane.IsPresent() && subPane.IsAppOwned())
    {
        CCoeControl* ctrl = NULL;
        TRAP_IGNORE(ctrl = iStatusPane->ControlL(uid));
        if (ctrl)
        {
            ctrl->MakeVisible(EFalse);
        }
    }
}

// ---------------------------------------------------------------------------
// CSwtUiUtils::HideIndicators
// ---------------------------------------------------------------------------
//
void CSwtUiUtils::HideIndicators()
{
    if (!iStatusPane)
        return;

    HideIndicator(EEikStatusPaneUidSignal);
    HideIndicator(EEikStatusPaneUidBattery);
    HideIndicator(EEikStatusPaneUidIndic);
    HideIndicator(EEikStatusPaneUidMessage);
    HideIndicator(EEikStatusPaneUidClock);
    HideIndicator(EEikStatusPaneUidDigitalClock);
}

// ---------------------------------------------------------------------------
// CSwtUiUtils::DoSetSplitInputShellPos
// ---------------------------------------------------------------------------
//
void CSwtUiUtils::DoSetSplitInputShellPos(const TPoint& aPos)
{
    MSwtShell& shell = iSplitInputView->GetShell();
    MSwtControl* temp = iSplitInputView;
    iSplitInputView = 0; // stop recursion.
    shell.DoSetLocation(aPos);
    iSplitInputView = temp;
}

// ---------------------------------------------------------------------------
// CSwtUiUtils::DoSetSplitInputViewSize
// ---------------------------------------------------------------------------
//
void CSwtUiUtils::DoSetSplitInputViewSize(const TSize& aSize)
{
    MSwtControl* temp = iSplitInputView;
    iSplitInputView = 0; // stop recursion.
    temp->SetWidgetSize(aSize);
    iSplitInputView = temp;
}

// ---------------------------------------------------------------------------
// CSwtUiUtils::ScrolledCompositeAncestor
// ---------------------------------------------------------------------------
//
MSwtControl* CSwtUiUtils::ScrolledCompositeAncestor(const MSwtControl& aControl) const
{
    MSwtComposite* parent = aControl.GetParent();
    const MSwtControl* control = &aControl;
    while (parent)
    {
        if (parent->ScrolledCompositeContent() == control)
        {
            return parent->Control();
        }
        control = parent->Control();
        parent = parent->Control()->GetParent();
    }

    return 0;
}

// ---------------------------------------------------------------------------
// CSwtUiUtils::GetSystemColor
// From MSwtUiUtils
// ---------------------------------------------------------------------------
//
TRgb CSwtUiUtils::GetSystemColor(TSwtColorId aId) const
{
    TRgb color(KRgbBlack);
    CEikonEnv* env = iDisplay.CoeEnv();
    MAknsSkinInstance* skin = AknsUtils::SkinInstance();

    switch (aId)
    {
    case ESwtColorWidgetBorder:
    case ESwtColorWidgetDarkShadow:
    case ESwtColorWidgetNormalShadow:
    case ESwtColorWidgetLightShadow:
    case ESwtColorWidgetHighlightShadow:
        // This border color is used also to draw the actual borders lines
        // See ASwtControlBase::DrawBorder()
        AknsUtils::GetCachedColor(skin, color, KAknsIIDQsnOtherColors,
                                  EAknsCIQsnOtherColorsCG12);
        break;
    case ESwtColorWidgetBackground:
        AknsUtils::GetCachedColor(skin, color, KAknsIIDQsnOtherColors,
                                  EAknsCIQsnOtherColorsCG10);
        break;
    case ESwtColorListBackground:
        AknsUtils::GetCachedColor(skin, color, KAknsIIDQsnOtherColors,
                                  EAknsCIQsnOtherColorsCG13);
        break;
    case ESwtColorListSelection:
        AknsUtils::GetCachedColor(skin, color, KAknsIIDQsnOtherColors,
                                  EAknsCIQsnOtherColorsCG14);
        break;
    case ESwtColorTitleBackground:
    case ESwtColorTitleBackgroundGradient:
    case ESwtColorTitleInactiveBackground:
    case ESwtColorTitleInactiveBackgroundGradient:
        AknsUtils::GetCachedColor(skin, color, KAknsIIDQsnOtherColors,
                                  EAknsCIQsnOtherColorsCG11);
        break;
    case ESwtColorWidgetForeground:
    case ESwtColorListForeground:
        AknsUtils::GetCachedColor(skin, color, KAknsIIDQsnTextColors,
                                  EAknsCIQsnTextColorsCG6);
        break;
    case ESwtColorListSelectionText:
        AknsUtils::GetCachedColor(skin, color, KAknsIIDQsnTextColors,
                                  EAknsCIQsnTextColorsCG10);
        break;
    case ESwtColorTitleForeground:
    case ESwtColorTitleInactiveForeground:
        AknsUtils::GetCachedColor(skin, color, KAknsIIDQsnTextColors,
                                  EAknsCIQsnTextColorsCG1);
        break;
    default:
        // Should not occur, but not an error
        __DEBUGGER();
        break;
    }

    return color;
}

// ---------------------------------------------------------------------------
// CSwtUiUtils::SystemFont
// From MSwtUiUtils
// ---------------------------------------------------------------------------
//
TFontSpec CSwtUiUtils::SystemFontSpec() const
{
    // Normal environment font
    const CFont* font = AknLayoutUtils::FontFromId(EAknLogicalFontPrimaryFont, NULL);
    return font->FontSpecInTwips();
}

// ---------------------------------------------------------------------------
// CSwtUiUtils::RunMessageBoxDlgLD
// From MSwtUiUtils
// ---------------------------------------------------------------------------
//
TInt CSwtUiUtils::RunMessageBoxDlgLD(MSwtDisplay& aDisplay, const TDesC& aTitle,
                                     const TDesC& aMessage, TInt aStyle) const
{
    return CSwtMessageBox::RunDlgLD(aDisplay, aTitle, aMessage, aStyle);
}

// ---------------------------------------------------------------------------
// CSwtUiUtils::GetParentTopShellOfActiveShell
// From MSwtUiUtils
// ---------------------------------------------------------------------------
//
MSwtShell* CSwtUiUtils::GetParentTopShellOfActiveShell() const
{
    if (!iActiveShell)
    {
        return NULL;
    }
    const MSwtControl* activeShellControl = iActiveShell->Control();
    return &GetControlTopShell(*activeShellControl);
}

// ---------------------------------------------------------------------------
// CSwtUiUtils::TopMostTopShell
// From MSwtUiUtils
// ---------------------------------------------------------------------------
//
MSwtShell* CSwtUiUtils::TopMostTopShell() const
{
    if (iActiveShell)
    {
        if (iActiveShell->GetParentShell())
        {
            if (CanBeTopMost(iActiveShell->Control()->GetTopShell()))
            {
                return &iActiveShell->Control()->GetTopShell();
            }
        }
        else
        {
            return iActiveShell;
        }
    }

    // Find last active Shell
    const TInt count = iShells.Count();
    for (TInt i = count - 1; i >= 0; --i)
    {
        MSwtShell* shell = iShells[i];
        if (!shell->GetParentShell() && CanBeTopMost(*shell))
        {
            return shell;
        }
    }

    return NULL;
}

// ---------------------------------------------------------------------------
// CSwtUiUtils::GetActiveShellPeer
// From MSwtUiUtils
// ---------------------------------------------------------------------------
//
TSwtPeer CSwtUiUtils::GetActiveShellPeer() const
{
    if (iActiveShell)
    {
        return iActiveShell->Control()->JavaPeer();
    }
    else
    {
        return NULL;
    }
}

// ---------------------------------------------------------------------------
// CSwtUiUtils::GetShellPeersL
// From MSwtUiUtils
// ---------------------------------------------------------------------------
//
CSwtPeerArray* CSwtUiUtils::GetShellPeersL() const
{
    const TInt count = iShells.Count();
    if (count == 0)
    {
        return NULL;
    }

    CSwtPeerArray* array = new(ELeave) CSwtPeerArray(count);
    CleanupStack::PushL(array);
    for (TInt i = 0; i < count; ++i)
    {
        array->AppendL(iShells[i]->Control()->JavaPeer());
    }
    CleanupStack::Pop(array);

    return array;
}

// ---------------------------------------------------------------------------
// CSwtUiUtils::GetFocusControl
// From MSwtUiUtils
// ---------------------------------------------------------------------------
//
TSwtPeer CSwtUiUtils::GetFocusControl() const
{
    if (iActiveShell)
    {
        MSwtControl* focusControl = iActiveShell->FocusControl();
        if (focusControl)
        {
            return focusControl->JavaPeer();
        }
        else
        {
            return iActiveShell->Control()->JavaPeer();
        }
    }
    return NULL;
}

// ---------------------------------------------------------------------------
// CSwtUiUtils::GetControlTopShell
// From MSwtUiUtils
// ---------------------------------------------------------------------------
//
MSwtShell& CSwtUiUtils::GetControlTopShell(const MSwtControl& aControl) const
{
    const MSwtControl* ctrl = &aControl;
    for (MSwtComposite* parent; (parent = ctrl->GetParent()) != NULL;)
    {
        ctrl = parent->Control();
    }
    MSwtShell* shell = const_cast<MSwtControl*>(ctrl)->ShellInterface();
    ASSERT(shell);
    return *shell;
}

// ---------------------------------------------------------------------------
// CSwtUiUtils::DrawFocusRect
// From MSwtUiUtils
// ---------------------------------------------------------------------------
//
void CSwtUiUtils::DrawFocusRect(MSwtGc& aGc, const TRect& aRect) const
{
    // Get highlight frame and highlight center graphics locations from
    // the skin.
    TAknLayoutRect topLeft = CSwtLafFacade::GetLayoutRect(
                                 CSwtLafFacade::EListHighlightSkinPlacingGeneralLine2, aRect);

    TAknLayoutRect bottomRight = CSwtLafFacade::GetLayoutRect(
                                     CSwtLafFacade::EListHighlightSkinPlacingGeneralLine5, aRect);

    TRect innerRect(topLeft.Rect().iBr, bottomRight.Rect().iTl);

    AknsDrawUtils::DrawFrame(AknsUtils::SkinInstance(),
                             static_cast<CFbsBitGc&>(aGc.BitmapContext()),
                             aRect,
                             innerRect,
                             KAknsIIDQsnFrList,
                             KAknsIIDQsnFrListCenter);

}

// ---------------------------------------------------------------------------
// CSwtUiUtils::Cba
// From MSwtUiUtils
// ---------------------------------------------------------------------------
//
CEikButtonGroupContainer& CSwtUiUtils::Cba() const
{
    ASSERT(iCba);
    return *iCba;
}

// ---------------------------------------------------------------------------
// CSwtUiUtils::UpdateStatusPaneL
// From MSwtUiUtils
// Calls to this should be kept to a severe minimum! This function is not anymore
// responsible of resizing the topmost shells since Shell's bounds should not
// depend on the current status pane height(See CSwtShell::DefaultBounds()).
// ---------------------------------------------------------------------------
//
void CSwtUiUtils::UpdateStatusPaneL()
{
    if (!iStatusPane)
    {
        return;
    }

    MSwtShell* topShell = TopMostTopShell();
    if (!topShell || topShell->FullScreenMode())
    {
        if (iSplitInputEditor)
        {
            // Split input editing is ongoing and the status pane is hidden.
            // Cache the new status pane visibility for when the split input ends.
            iSplitInputSPVisible = EFalse;
        }
        else
        {
            if (iStatusPane->IsVisible())
            {
                iStatusPane->SetObserver(NULL);
                iStatusPane->MakeVisible(EFalse);
            }
        }
        return;
    }

    // Title
    CAknTitlePane* titlePane  = static_cast<CAknTitlePane*>(iStatusPane->ControlL(
                                    TUid::Uid(EEikStatusPaneUidTitle)));
    const TDesC* title = NULL;
    if (topShell)
    {
        title = topShell->Decorations()->GetText();
    }
    if (titlePane)
    {
        if (title)
        {
            titlePane->SetTextL(*title);
        }
        else if (iAppName)
        {
            titlePane->SetTextL(*iAppName);
        }
        else
        {
            titlePane->SetTextToDefaultL();
        }
    }

    // Status
    CAknNavigationControlContainer* naviPane = static_cast<CAknNavigationControlContainer*>(
                iStatusPane->ControlL(TUid::Uid(EEikStatusPaneUidNavi)));
    if (naviPane)
    {
        if (iNaviDecorator)
        {
            naviPane->Pop(iNaviDecorator);
            delete iNaviDecorator;
            iNaviDecorator = NULL;
        }
    }

    MSwtShell::TSwtStatusPaneStyle style(MSwtShell::EStyleNoStatusPane);
    if (topShell && naviPane)
    {
        style = topShell->StatusPaneStyle();

        MSwtMobileShell* activeTopMobileShell = topShell->MobileShellInterface();
        if (activeTopMobileShell)
        {
            const TDesC* status = activeTopMobileShell->GetStatusText();
            TInt statusLength(status ? status->Length() : 0);
            TInt titleLength(title ? title->Length() : 0);

            HBufC* extendedStatus = HBufC::NewLC(titleLength + statusLength + 1);
            TPtr extendedStatusTxt(extendedStatus->Des());
            TBool landscape = CSwtLafFacade::IsLandscapeOrientation();
            if ((style == MSwtShell::EStyleSmallStatusPane) && !(landscape))
            {
                if (titleLength > 0)
                {
                    extendedStatusTxt.Append(*title);
                }
                if (statusLength > 0 && titleLength > 0)
                {
                    extendedStatusTxt.Append(KSeparator);
                }
            }
            if (extendedStatus)
            {
                if (statusLength)
                {
                    extendedStatusTxt.Append(*status);
                }
                iNaviDecorator = naviPane->CreateMessageLabelL(*extendedStatus);
                naviPane->PushL(*iNaviDecorator);
            }
            CleanupStack::PopAndDestroy(extendedStatus);
        }
    }

    // Image
    if (iStatusPane->PaneCapabilities(TUid::Uid(EEikStatusPaneUidContext)).IsInCurrentLayout())
    {
        CAknContextPane* contextPane = static_cast<CAknContextPane*>(
                                           iStatusPane->ControlL(TUid::Uid(EEikStatusPaneUidContext)));
        if (contextPane)
        {
            const MSwtImage* image = topShell ? topShell->Decorations()->GetImage() : NULL;
            if (image)
            {
                CFbsBitmap* bitmap = NULL;
                CFbsBitmap* mask   = NULL;
                bitmap = SwtControlHelper::GetCopyOfBitmapLC(&(image->Bitmap()));
                mask   = SwtControlHelper::GetCopyOfBitmapLC(image->MaskBitmap());
                CleanupStack::Pop(mask ? 2 : 1);
                contextPane->SetPicture(bitmap, mask);
            }
            else
            {
                TRAP_IGNORE(contextPane->SetPictureToDefaultL());
            }
        }
    }

    // Style changed last to avoid flickering
    iStatusPane->SetObserver(NULL);
    if (!topShell || style == MSwtShell::EStyleNoStatusPane)
    {
        if (iSplitInputEditor)
        {
            // Split input editing is ongoing and the status pane is hidden.
            // Cache the new status pane visibility for when the split input ends.
            iSplitInputSPVisible = EFalse;
        }
        else
        {
            iStatusPane->MakeVisible(EFalse);
        }
    }
    else
    {
        if (style == MSwtShell::EStyleLargeStatusPane)
        {
            iStatusPane->SwitchLayoutL(R_AVKON_STATUS_PANE_LAYOUT_USUAL);
        }
        else if (style == MSwtShell::EStyleSmallStatusPane)
        {
            iStatusPane->SwitchLayoutL(R_AVKON_STATUS_PANE_LAYOUT_SMALL_WITH_SIGNAL_PANE);
        }

        if (iSplitInputEditor)
        {
            // Split input editing is ongoing and the status pane is hidden.
            // Cache the new status pane visibility for when the split input ends.
            iSplitInputSPVisible = ETrue;
        }
        else
        {
            iStatusPane->MakeVisible(ETrue);

            java::ui::CoreUiAvkonAppUi* appUi = java::ui::CoreUiAvkonEswt::getInstance().getJavaUiAppUi();
            if (appUi && appUi->hidesIndicators())
            {
                HideIndicators();
            }

            // The above relies on DrawDeferred which is too slow for some cases.
            // For instance, the pane must draw immediately when the app draws the
            // first time otherwise the startup screenshot will miss the pane.
            iStatusPane->DrawNow();
        }
    }

    // This will cause HandleStatusPaneSizeChange notifications again
    if (!iSplitInputEditor)
    {
        iStatusPane->SetObserver(this);
    }

    // Shells are trusted to have the correct size already.
}

// ---------------------------------------------------------------------------
// CSwtUiUtils::HandleAppFocusGainedL
// From MSwtAppFocusObserver
// ---------------------------------------------------------------------------
//
void CSwtUiUtils::HandleAppFocusChangeL(TBool aFocused)
{
    if (aFocused)
    {
        if (!iFocusLost)
        {
            return;
        }

        ASSERT(!iActiveShell);

        // Update flag
        iFocusLost = EFalse;
        TBool getFocused(EFalse);
        if (iNextFocusedControl)
        {
            getFocused = iNextFocusedControl->SetSwtFocus(KSwtFocusByForce);
            iNextFocusedControl = NULL;
        }

        if (getFocused)
        {
            // do nothing
        }
        else if (iNextActiveShell)
        {
            // Activate the shell waiting to become active.
            DoSetActiveShell(*iNextActiveShell, ETrue);
        }
        else
        {
            // Search for a shell capable of becoming active.
            FindAndActivateShell();
        }
    }
    else
    {
        if (iFocusLost)
        {
            return;
        }

        // Update flag
        iFocusLost = ETrue;

        // Remember active shell
        iNextActiveShell = iActiveShell;

        // Deactivate the active shell
        if (iActiveShell)
        {
            iActiveShell->ActivityChanged(EFalse, EFalse);
            SetShellFade(iActiveShell, EFalse);
            iActiveShell = NULL;
        }
    }
}

// ---------------------------------------------------------------------------
// CSwtUiUtils::IsAppFocused
// From MSwtUiUtils
// ---------------------------------------------------------------------------
//
TBool CSwtUiUtils::IsAppFocused() const
{
    return !iFocusLost;
}

// ---------------------------------------------------------------------------
// CSwtUiUtils::GetCommandOkTitleL
// From MSwtUiUtils
// ---------------------------------------------------------------------------
//
const HBufC* CSwtUiUtils::GetCommandOkTitleL() const
{
    return iDisplay.CoeEnv()->AllocReadResourceL(R_SWT_SOFTKEY_TEXT_OK);
}

// ---------------------------------------------------------------------------
// CSwtUiUtils::GetCommandCancelTitleL
// From MSwtUiUtils
// ---------------------------------------------------------------------------
//
const HBufC* CSwtUiUtils::GetCommandCancelTitleL() const
{
    return iDisplay.CoeEnv()->AllocReadResourceL(R_SWT_SOFTKEY_TEXT_CANCEL);
}

// ---------------------------------------------------------------------------
// CSwtUiUtils::GetPointerEventStateMask
// From MSwtUiUtils
// ---------------------------------------------------------------------------
//
TInt CSwtUiUtils::GetPointerEventStateMask(TUint aPointerEventModifiers) const
{
    // Compute the state mask
    TInt stateMask = 0;
    if (aPointerEventModifiers & EModifierCtrl)
    {
        stateMask |= KSwtModifierControl;
    }
    if (aPointerEventModifiers & EModifierShift)
    {
        stateMask |=  KSwtModifierShift;
    }
    if (aPointerEventModifiers & EModifierAlt)
    {
        stateMask |= KSwtModifierAlt;
    }

    return stateMask;
}

// ---------------------------------------------------------------------------
// CSwtUiUtils::InlineReferencePadding
// From MSwtUiUtils
// ---------------------------------------------------------------------------
//
TMargins8 CSwtUiUtils::InlineReferencePadding() const
{
    if (iInlinePadding.iTop > 0)
        return iInlinePadding;

    // The top and bottom margins are given by list
    TRect mainRect(TRect::EUninitialized);
    TAknLayoutRect layoutRect;
    AknLayoutUtils::LayoutMetricsRect(AknLayoutUtils::EMainPane, mainRect);
    layoutRect = CSwtLafFacade::GetLayoutRect(CSwtLafFacade::EListScrollGenPane, mainRect, 0);
    layoutRect = CSwtLafFacade::GetLayoutRect(CSwtLafFacade::EListGenPane, layoutRect.Rect(), 0);

    TAknLayoutText txtLayoutRect;
    layoutRect = CSwtLafFacade::GetLayoutRect(CSwtLafFacade::EListSinglePane, layoutRect.Rect(), 0);
    txtLayoutRect = CSwtLafFacade::GetLayoutText(CSwtLafFacade::EListSinglePaneT1, layoutRect.Rect(), 0);

    TRect listItemPaddingRect(layoutRect.Rect().iTl, txtLayoutRect.TextRect().iTl);
    TInt listItemPaddingHeight = listItemPaddingRect.Height();

    // Left and right margins come from Button
    TRect tl = CSwtLafFacade::GetLayoutRect(CSwtLafFacade::EToolbarButtonPaneG2, TRect(), 0).Rect();
    TInt w = tl.Width();
    TInt h = tl.Height();
    TMargins8 res(w, h, w, h); // (aLeft, aTop, aRight, aBottom)

    res.iTop = listItemPaddingHeight;
    res.iBottom = listItemPaddingHeight;

    iInlinePadding = res;

    return iInlinePadding;
}

// ---------------------------------------------------------------------------
// CSwtUiUtils::InlineReferenceFontHeight
// From MSwtUiUtils
// ---------------------------------------------------------------------------
//
TInt CSwtUiUtils::InlineReferenceFontHeight() const
{
    if (iInlineFontHeight <= 0)
        iInlineFontHeight = InlineReferenceFont().FontMaxHeight();
    return iInlineFontHeight;
}


// ---------------------------------------------------------------------------
// CSwtUiUtils::SetAppNameL
// ---------------------------------------------------------------------------
//
void CSwtUiUtils::SetAppNameL(const TDesC* aNewName)
{
    if (iAppName)
    {
        delete iAppName;
        iAppName = NULL;
    }

    if (aNewName)
    {
        iAppName = aNewName->AllocL();
    }

    if (TopMostTopShell())
    {
        UpdateStatusPaneL();
    }
}

// ---------------------------------------------------------------------------
// CSwtUiUtils::GetBestImageSize
// ---------------------------------------------------------------------------
//
TSize CSwtUiUtils::GetBestImageSize(TInt aImageType)
{
    TSize maxImageSize(0, 0);
    switch (aImageType)
    {
    case ESwtListImage:
        maxImageSize = CSwtLafFacade::GetLayoutRect(
                           CSwtLafFacade::EListSingleLargeGraphicPaneG1, TRect(), 0).Rect().Size();
        break;
    case ESwtChoiceImage:
        maxImageSize = CSwtLafFacade::GetLayoutRect(
                           CSwtLafFacade::EListSingle2GraphicPaneG1Cp4, TRect()).Rect().Size();
        break;
    case ESwtAlertImage:
        maxImageSize = CSwtLafFacade::GetLayoutRect(
                           CSwtLafFacade::EPopupMidpNoteAlarmWindowG1, TRect(), 0).Rect().Size();
        break;
    default:
        break;
    }
    return maxImageSize;
}

// ---------------------------------------------------------------------------
// CSwtUiUtils::HandleResourceChangedL
// From MSwtUiUtils
// ---------------------------------------------------------------------------
//
void CSwtUiUtils::HandleResourceChangedL(TInt aType)
{
    if (aType == KEikDynamicLayoutVariantSwitch)
    {
        iScrollBarBreadth = -1;
        iInlineFont = NULL;
        iInlineFontHeight = -1;
        iInlinePadding.iTop = -1;

        // Sometimes CBA bounds are incorrect at this point in the case we have
        // a dialog shell active. As a result Shell gets wrong size.
        // Set the bounds here to make sure they are always correct.
        const TSize screenSize(iDisplay.CoeEnv()->ScreenDevice()->SizeInPixels());
        iCba->SetBoundingRect(TRect(screenSize));
    }

    TInt count = iShells.Count();
    for (TInt i = 0; i < count; ++i)
    {
        // Forward HandleResourceChange to all Shells (except active one because
        // already informed through control stack)
        MSwtShell* shell = iShells[i];
        if (shell != iActiveShell)
        {
            shell->Control()->CoeControl().HandleResourceChange(aType);
        }
    }
#ifdef RD_JAVA_S60_RELEASE_9_2
    if (aType == KAknSplitInputDisabled)
    {
        SetSplitInputEditor(0);
    }
#endif
}

// ---------------------------------------------------------------------------
// CSwtUiUtils::ScrollBarBreadth
// From MSwtUiUtils
// ---------------------------------------------------------------------------
//
TInt CSwtUiUtils::ScrollBarBreadth(const CEikScrollBar* aBar)
{
    if (!aBar)
    {
        return 0;
    }

    if (iScrollBarBreadth < 0)
    {
        iScrollBarBreadth = aBar->ScrollBarBreadth();
    }
#if 0
    else
    {
        ASSERT(aBar->ScrollBarBreadth() == iScrollBarBreadth);
    }
#endif
    return iScrollBarBreadth;
}

// ---------------------------------------------------------------------------
// CSwtUiUtils::NaviKeyInput
// From MSwtUiUtils
// ---------------------------------------------------------------------------
//
TBool CSwtUiUtils::NaviKeyInput() const
{
    return iNaviKeyInput;
}

// ---------------------------------------------------------------------------
// CSwtUiUtils::SetLastInputIsKey
// From MSwtUiUtils
// ---------------------------------------------------------------------------
//
void CSwtUiUtils::SetNaviKeyInput(TBool aStatus)
{
    TBool prevNaviKeyInput(iNaviKeyInput);
    iNaviKeyInput = aStatus;
    if (prevNaviKeyInput != iNaviKeyInput)
    {
        if (iActiveShell)
        {
            MSwtControl* ctrl = iActiveShell->FocusControl();
            if (ctrl)
                ctrl->Redraw();
        }
    }
}

// ---------------------------------------------------------------------------
// CSwtUiUtils::SetCbaVisible
// From MSwtUiUtils
// ---------------------------------------------------------------------------
//
void CSwtUiUtils::SetCbaVisible(TBool aVisible)
{
    ASSERT(iCba);
    if (aVisible == iCba->IsVisible())
    {
        return;
    }
    iCba->MakeVisible(aVisible);

#ifdef RD_JAVA_S60_RELEASE_9_2
    // To enable clock pane in landscape after MIDlet was started
    iCba->SetBoundingRect(TRect(0, 0, 0, 0));
#endif // RD_JAVA_S60_RELEASE_9_2
}

// ---------------------------------------------------------------------------
// CSwtUiUtils::GetPointerGrabbingControl
// From MSwtUiUtils
// ---------------------------------------------------------------------------
//
MSwtControl* CSwtUiUtils::GetPointerGrabbingControl()
{
    MSwtControl* grabbingControl = iPointerCaptureControl;
    if (grabbingControl)
    {
        return grabbingControl;
    }

    MSwtComposite* composite = NULL;

    if (iActiveShell)
    {
        composite = iActiveShell->Composite();
        grabbingControl = iActiveShell->Control();
    }

    MSwtControl* ctrl = NULL;
    while (composite)
    {
        ctrl = composite->GetPointerGrabbingControl();
        if (!ctrl)
            break;
        grabbingControl = ctrl;
        composite = grabbingControl->CompositeInterface();
    }

    return grabbingControl;
}

// ---------------------------------------------------------------------------
// CSwtUiUtils::HandleFreeRamEventL
// From MSwtUiUtils
// ---------------------------------------------------------------------------
//
TRect CSwtUiUtils::TaskTipRect() const
{
    TRect screenRect;
    AknLayoutUtils::LayoutMetricsRect(AknLayoutUtils::EScreen, screenRect);
    TAknLayoutRect layoutMainPane = CSwtLafFacade::GetLayoutRect(
                                        CSwtLafFacade::EMainEswtPane,
                                        screenRect,
                                        KVariationFullScreen);
    TRect mainPaneRect = layoutMainPane.Rect();
    TAknLayoutRect layoutShell = CSwtLafFacade::GetLayoutRect(
                                     CSwtLafFacade::EPopupEswtTasktipWindow,
                                     mainPaneRect);
    return layoutShell.Rect();
}

// ---------------------------------------------------------------------------
// CSwtUiUtils::SetPointerCaptureControl
// From MSwtUtils
// ---------------------------------------------------------------------------
//
void CSwtUiUtils::SetPointerCaptureControl(MSwtControl* aControl)
{
    if (aControl)
    {
        iPointerCaptureControl = aControl;
    }
    else
    {
        iPointerCaptureControl = NULL;
    }
}

// ---------------------------------------------------------------------------
// CSwtUiUtils::PointerCaptureControl
// From MSwtUtils
// ---------------------------------------------------------------------------
//
MSwtControl* CSwtUiUtils::PointerCaptureControl()
{
    return iPointerCaptureControl;
}

// ---------------------------------------------------------------------------
// CSwtUiUtils::SetSplitInputEditor
// From MSwtUtils
// ---------------------------------------------------------------------------
//
void CSwtUiUtils::SetSplitInputEditor(MSwtControl* aEditor)
{
    if (iSplitInputEditor == aEditor)
    {
        // This can happen when pressing Enter in a multi line editor.
        return;
    }

    CAknAppUi* appUi = (CAknAppUi*)iDisplay.CoeEnv()->EikAppUi();

    if (aEditor)
    {
        MSwtShell& shell = aEditor->GetShell();
        MSwtControl* view = ScrolledCompositeAncestor(*aEditor);
        TBool editorSwitch(iSplitInputEditor != 0);
        TBool keepView(view && view == iSplitInputView);
        TBool keepShell(editorSwitch && ((&iSplitInputEditor->GetShell()) == (&shell)));

        if (editorSwitch)
        {
            if (!keepShell)
            {
                // Move back the previous split input Shell keeping the SP hidden.
                DoSetSplitInputShellPos(iSplitInputShellPos);
            }
            // Otherwise no need for the shell to restore its original
            // position as it is going to be moved again right after this.

            if (!keepView)
            {
                // Resize back the previous split input view
                DoSetSplitInputViewSize(iSplitInputViewSize);
            }
        }

        iSplitInputEditor = aEditor;
        iSplitInputView = view;
        if (!iSplitInputView)
        {
            iSplitInputView = iSplitInputEditor;
        }

        if (!editorSwitch || !keepShell)
        {
            iSplitInputShellPos = shell.Control()->GetLocation();
        }

        if (!editorSwitch)
        {
            iSplitInputSPVisible = iStatusPane->IsVisible();
        }

        if (iStatusPane->IsVisible())
        {
            // Hide the SP. The observer must be cleared before so that
            // HandleStatusPaneSizeChange is not called on the Shell.
            iStatusPane->SetObserver(NULL);
            iStatusPane->MakeVisible(EFalse);
        }

        if (!keepView)
        {
            iSplitInputViewSize = TSize(0, 0);
            SetSplitInputViewSize(iSplitInputView->GetWidgetSize());
        }

        if (!editorSwitch && view)
        {
            TRAP_IGNORE(iDisplay.PostShowFocusedControlEventL(view->JavaPeer()));
        }
    }
    else
    {
        ASSERT(iSplitInputEditor);
        if (iSplitInputSPVisible)
        {
            iStatusPane->MakeVisible(ETrue);
            iStatusPane->SetObserver(this);
        }
        DoSetSplitInputShellPos(iSplitInputShellPos);
        DoSetSplitInputViewSize(iSplitInputViewSize);
        iSplitInputEditor = 0;
        iSplitInputView = 0;
    }
}

// ---------------------------------------------------------------------------
// CSwtUiUtils::SetSplitInputEditor
// From MSwtUtils
// ---------------------------------------------------------------------------
//
MSwtControl* CSwtUiUtils::SplitInputEditor() const
{
    return iSplitInputEditor;
}

// ---------------------------------------------------------------------------
// CSwtUiUtils::SetSplitInputEditor
// From MSwtUtils
// ---------------------------------------------------------------------------
//
MSwtControl* CSwtUiUtils::SplitInputView() const
{
    return iSplitInputView;
}

// ---------------------------------------------------------------------------
// CSwtUiUtils::SetSplitInputShellPos
// From MSwtUtils
// ---------------------------------------------------------------------------
//
void CSwtUiUtils::SetSplitInputShellPos(const TPoint& aOriginalPos)
{
    iSplitInputShellPos = aOriginalPos;
}

// ---------------------------------------------------------------------------
// CSwtUiUtils::SetSplitInputViewSize
// From MSwtUtils
// ---------------------------------------------------------------------------
//
void CSwtUiUtils::SetSplitInputViewSize(const TSize& aOriginalSize)
{
    if (aOriginalSize == iSplitInputViewSize)
    {
        return;
    }

    // Remember the size of the editor and resize it if needed
    // to fit into remaining space above the vkb.
    iSplitInputViewSize = aOriginalSize;
    CAknAppUi* appUi = (CAknAppUi*)iDisplay.CoeEnv()->EikAppUi();
    TRect appUiRect(appUi->ClientRect());
    TSize size(iSplitInputViewSize);
    if (size.iHeight > appUiRect.Height())
    {
        size.iHeight = appUiRect.Height();
    }
    DoSetSplitInputViewSize(size);
    AdjustSplitInputShellPos();
}

// ---------------------------------------------------------------------------
// CSwtUiUtils::AdjustSplitInputShellPos
// From MSwtUtils
// ---------------------------------------------------------------------------
//
void CSwtUiUtils::AdjustSplitInputShellPos()
{
    if (!iSplitInputView)
    {
        return;
    }

    CAknAppUi *appUi = (CAknAppUi*)(iDisplay.CoeEnv()->EikAppUi());
    TInt appUiHeight(appUi->ClientRect().Height());
    MSwtShell & shell = iSplitInputView->GetShell();
    TInt shellHeight(shell.Control()->GetWidgetSize().iHeight);
    TRect rect(iSplitInputView->VisibleRect());
    TInt y(-rect.iTl.iY + (appUiHeight - rect.Height()) / 2);
    if (y < (appUiHeight - shellHeight))
        y = appUiHeight - shellHeight;

    if (!shell.GetParentShell())
    {
        if (y > 0)
            y = 0;
    }
    else
    {
        if (y > iSplitInputShellPos.iY)
            y = iSplitInputShellPos.iY;

    }
    DoSetSplitInputShellPos(TPoint(iSplitInputShellPos.iX, y));
}

// ---------------------------------------------------------------------------
// CSwtUiUtils::HandleFreeRamEventL
// From MSwtUiUtils
// ---------------------------------------------------------------------------
//
void CSwtUiUtils::HandleFreeRamEventL(TInt aEventType)
{
    const TInt count = iShells.Count();
    for (TInt i = count - 1; i >= 0; --i)
    {
        MSwtShell* shell = iShells[i];
        FreeBrowserRamL(shell->Composite(), aEventType);
    }
}

// ---------------------------------------------------------------------------
// CSwtUiUtils::HandleStatusPaneSizeChange
// From MEikStatusPaneObserver
// ---------------------------------------------------------------------------
//
void CSwtUiUtils::HandleStatusPaneSizeChange()
{
    const TInt count = iShells.Count();
    for (TInt i = 0; i < count; ++i)
    {
        iShells[i]->HandleStatusPaneSizeChange();
    }
}

// ---------------------------------------------------------------------------
// CSwtUiUtils::CountFadedComponents
// From MAknFadedComponent
// ---------------------------------------------------------------------------
//
TInt CSwtUiUtils::CountFadedComponents()
{
    TInt res(0);
    if (CanBeFaded(iActiveShell))
    {
        res = 2; // Cba + Shell
        const RSwtControlArray* children = iActiveShell->Composite()->Children();
        if (children)
        {
            TInt count = children->Count();
            TInt j(0);
            for (TInt i = 0; i < count; i++)
            {
                MSwtControl* child = (*children)[i];
                if (child->IsShell())
                {
                    j++;
                }
            }
            res += j;
        }
    }
    return res;
}

// ---------------------------------------------------------------------------
// CSwtUiUtils::FadedComponent
// From MAknFadedComponent
// ---------------------------------------------------------------------------
//
CCoeControl* CSwtUiUtils::FadedComponent(TInt aIndex)
{
    if (CanBeFaded(iActiveShell))
    {
        if (aIndex == 0)
        {
            return iCba;
        }
        else if (aIndex == 1)
        {
            return &iActiveShell->Control()->CoeControl();
        }
        else
        {
            const RSwtControlArray* children = iActiveShell->Composite()->Children();
            if (children)
            {
                TInt count = children->Count();
                for (TInt i = 0, j = 0; i < count; i++)
                {
                    MSwtControl* child = (*children)[i];
                    if (child->IsShell())
                    {
                        if (aIndex - 2 == j)
                        {
                            return &child->CoeControl();
                        }
                        j++;
                    }
                }
            }
        }
    }

    // Shouldnt happen
    return NULL;
}

// ---------------------------------------------------------------------------
// CSwtUiUtils::SetShellFade
// ---------------------------------------------------------------------------
//
void CSwtUiUtils::SetShellFade(MSwtShell* aShell, TBool aStatus)
{
    if (CanBeFaded(aShell))
    {
        if (aStatus)
        {
            iFadingShell = aShell;
        }
        iFader.FadeBehindPopup(this, &aShell->Control()->CoeControl(), aStatus);
        if (!aStatus)
        {
            iFadingShell = NULL;
        }
    }
}

// ---------------------------------------------------------------------------
// CSwtUiUtils::CanBeFaded
// ---------------------------------------------------------------------------
//
TBool CSwtUiUtils::CanBeFaded(MSwtShell* aShell) const
{
    return (aShell && aShell->GetParentShell() && IsApplicationModal(*aShell));
}

