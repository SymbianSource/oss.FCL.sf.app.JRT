/*******************************************************************************
 * Copyright (c) 2007, 2010 Nokia Corporation and/or its subsidiary(-ies).
 * All rights reserved. This program and the accompanying materials
 * are made available under the terms of the Eclipse Public License v1.0
 * which accompanies this distribution, and is available at
 * http://www.eclipse.org/legal/epl-v10.html
 *
 * Contributors:
 *     Nokia Corporation - S60 implementation
 *******************************************************************************/


#include "swtcontrolmenu.h"


// ======== MEMBER FUNCTIONS ========


// ---------------------------------------------------------------------------
// CSwtControlMenu::NewL
// ---------------------------------------------------------------------------
//
CSwtControlMenu* CSwtControlMenu::NewL(MSwtDisplay& aDisplay,
                                       TSwtPeer aPeer, TInt aStyle)
{
    return new(ELeave) CSwtControlMenu(aDisplay, aPeer, aStyle);
}

// ---------------------------------------------------------------------------
// CSwtControlMenu::CSwtControlMenu
// ---------------------------------------------------------------------------
//
CSwtControlMenu::CSwtControlMenu(MSwtDisplay& aDisplay,
                                 TSwtPeer aPeer, TInt aStyle)
        : CSwtMenu(aDisplay, aPeer, aStyle)
{
}

// ---------------------------------------------------------------------------
// CSwtControlMenu::~CSwtControlMenu
// ---------------------------------------------------------------------------
//
CSwtControlMenu::~CSwtControlMenu()
{
    // Not own
    iObserver = NULL;
}

// ---------------------------------------------------------------------------
// CSwtControlMenu::SetObserver
// ---------------------------------------------------------------------------
//
void CSwtControlMenu::SetObserver(MEikCommandObserver* aObserver)
{
    iObserver = aObserver;
}

// ---------------------------------------------------------------------------
// CSwtControlMenu::Observer
// ---------------------------------------------------------------------------
//
MEikCommandObserver* CSwtControlMenu::Observer() const
{
    return iObserver;
}

// ---------------------------------------------------------------------------
// CSwtControlMenu::ProcessCommandL
// ---------------------------------------------------------------------------
//
void CSwtControlMenu::ProcessCommandL(TInt aCommandId, TInt /*aItemIndex*/,
                                      TBool /*aOfferToSubMenus = EFalse*/) const
{
    const RPointerArray<MSwtMenuItem>& items = *Items();
    TInt count = items.Count();
    TBool commandFound = EFalse;
    for (TInt i = 0; i < count; i++)
    {
        if (items[i]->CommandId() == aCommandId)
        {
            commandFound = ETrue;
            break;
        }
    }
    if (iObserver && commandFound)
    {
        iObserver->ProcessCommandL(aCommandId);
    }
}
