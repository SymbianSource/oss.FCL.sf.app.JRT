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


#include <avkon.hrh>
#include <eswtcore.rsg>
#include "swtmenubar.h"
#include "swtmenuarranger.h"


// ======== MEMBER FUNCTIONS ========


// ---------------------------------------------------------------------------
// CSwtMenuBar::CSwtMenuBar
// ---------------------------------------------------------------------------
//
inline CSwtMenuBar::CSwtMenuBar(CSwtMenuArranger& aMenuArranger)
        : iMenuArranger(aMenuArranger)
{
}

// ---------------------------------------------------------------------------
// CSwtMenuBar::~CSwtMenuBar
// ---------------------------------------------------------------------------
//
CSwtMenuBar::~CSwtMenuBar()
{
    delete iEikMenuBar;
}

// ---------------------------------------------------------------------------
// CSwtMenuBar::ConstructL
// ---------------------------------------------------------------------------
//
void CSwtMenuBar::ConstructL()
{
    iEikMenuBar = new(ELeave) CEikMenuBar;
    iEikMenuBar->ConstructL(&iMenuArranger, 0, R_SWT_MENUBAR);
    iEikMenuBar->SetContextMenuTitleResourceId(R_SWT_CONTEXTMENUBAR);
}

// ---------------------------------------------------------------------------
// CSwtMenuBar::NewL
// ---------------------------------------------------------------------------
//
CSwtMenuBar* CSwtMenuBar::NewL(CSwtMenuArranger& aMenuArranger)
{
    CSwtMenuBar* self = new(ELeave) CSwtMenuBar(aMenuArranger);
    CleanupStack::PushL(self);
    self->ConstructL();
    CleanupStack::Pop(self);
    return self;
}

// ---------------------------------------------------------------------------
// CSwtMenuBar::EikMenuBar
// ---------------------------------------------------------------------------
//
CEikMenuBar* CSwtMenuBar::EikMenuBar()
{
    return iEikMenuBar;
}

// ---------------------------------------------------------------------------
// CSwtMenuBar::TryDisplayMenuBarL
// ---------------------------------------------------------------------------
//
void CSwtMenuBar::TryDisplayMenuBarL(TBool aDisplayContextMenu, TBool aHasRadioGroup)
{
    if (aDisplayContextMenu)
    {
        TInt contextMenuTitleResourceId((aHasRadioGroup) ?
                                        R_SWT_CONTEXTMENUBAR_WITHRADIOGROUP : R_SWT_CONTEXTMENUBAR);
        iEikMenuBar->SetContextMenuTitleResourceId(contextMenuTitleResourceId);
        iEikMenuBar->TryDisplayContextMenuBarL();
    }
    else
    {
        TInt menuTitleResourceId((aHasRadioGroup) ?
                                 R_SWT_MENUBAR_WITHRADIOGROUP : R_SWT_MENUBAR);
        iEikMenuBar->SetMenuTitleResourceId(menuTitleResourceId);
        iEikMenuBar->TryDisplayMenuBarL();
    }

    // Replace the CBA observer of menu bar.
    CEikButtonGroupContainer* menuCba = CEikButtonGroupContainer::Current();
    menuCba->UpdateCommandObserverL(0, *this);
    menuCba->UpdateCommandObserverL(2, *this);
}

// ---------------------------------------------------------------------------
// CSwtMenuBar::AddToControlStackL
// ---------------------------------------------------------------------------
//
void CSwtMenuBar::AddToControlStackL()
{
    iEikonEnv->EikAppUi()->AddToStackL(this, ECoeStackPriorityDefault, ECoeStackFlagRefusesFocus);
    iIsOnControlStack = ETrue;
}

// ---------------------------------------------------------------------------
// CSwtMenuBar::RemoveFromControlStack
// ---------------------------------------------------------------------------
//
void CSwtMenuBar::RemoveFromControlStack()
{
    iEikonEnv->EikAppUi()->RemoveFromStack(this);
    iIsOnControlStack = EFalse;
}

// ---------------------------------------------------------------------------
// CSwtMenuBar::OfferKeyEventL
// From CCoeControl
// ---------------------------------------------------------------------------
//
TKeyResponse CSwtMenuBar::OfferKeyEventL(const TKeyEvent& aKeyEvent,
        TEventCode aType)
{
    return iEikMenuBar->OfferKeyEventL(aKeyEvent, aType);
}

// ---------------------------------------------------------------------------
// CSwtMenuBar::CountComponentControls
// From CCoeControl
// ---------------------------------------------------------------------------
//
TInt CSwtMenuBar::CountComponentControls() const
{
    return 1;
}

// ---------------------------------------------------------------------------
// CSwtMenuBar::ComponentControl
// From CCoeControl
// ---------------------------------------------------------------------------
//
CCoeControl* CSwtMenuBar::ComponentControl(TInt /*aIndex*/) const
{
    return iEikMenuBar;
}

// ---------------------------------------------------------------------------
// CSwtMenuBar::FocusChanged
// From CCoeControl
// ---------------------------------------------------------------------------
//
void CSwtMenuBar::FocusChanged(TDrawNow aDrawNow)
{
    iEikMenuBar->SetFocus(IsFocused(), aDrawNow);
}

// ---------------------------------------------------------------------------
// CSwtMenuBar::SizeChanged
// From CCoeControl
// ---------------------------------------------------------------------------
//
void CSwtMenuBar::SizeChanged()
{
    iEikMenuBar->SetSize(iSize);
}

// ---------------------------------------------------------------------------
// CSwtMenuBar::ProcessCommandL
// From MEikCommandObserver
// ---------------------------------------------------------------------------
//
void CSwtMenuBar::ProcessCommandL(TInt aCommandId)
{
    TEventCode eventType;
    TKeyEvent keyEvent;
    switch (aCommandId)
    {
    case EAknSoftkeyOk:
        eventType = EEventKey;
        keyEvent.iCode = static_cast<TUint>(EAknSoftkeyOk);
        keyEvent.iModifiers = EModifierPureKeycode;
        keyEvent.iRepeats = 0;
        iEikMenuBar->OfferKeyEventL(keyEvent, eventType);
        break;
    case EAknSoftkeySelect:
        eventType = EEventKey;
        keyEvent.iCode = static_cast<TUint>(EAknSoftkeySelect);
        keyEvent.iModifiers = EModifierPureKeycode;
        keyEvent.iRepeats = 0;
        iEikMenuBar->OfferKeyEventL(keyEvent, eventType);
        break;
    case EAknSoftkeyCancel:
        iMenuArranger.StopDisplayingMenuBar();
        break;
    default:
        break;
    }
}
