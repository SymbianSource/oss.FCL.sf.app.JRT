/*
* Copyright (c) 2007 Nokia Corporation and/or its subsidiary(-ies).
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



#include <aknchoicelist.h>
#include "swtcomboproxy.h"


const TInt KSwtAknComboCtrlsCount = 1;


// ======== MEMBER FUNCTIONS ========


// ---------------------------------------------------------------------------
// CSwtComboProxy::NewL
// ---------------------------------------------------------------------------
//
EXPORT_C CSwtComboProxy* CSwtComboProxy::NewL(CCoeControl* aParent, CDesCArray* aItemArray)
{
    CSwtComboProxy* self = new(ELeave) CSwtComboProxy();
    CleanupStack::PushL(self);
    self->ConstructL(aParent, aItemArray);
    CleanupStack::Pop(self);
    return self;
}

// ---------------------------------------------------------------------------
// CSwtComboProxy::~CSwtComboProxy
// ---------------------------------------------------------------------------
//
EXPORT_C CSwtComboProxy::~CSwtComboProxy()
{
    delete iDelegate;
    iDelegate = NULL;
}

// ---------------------------------------------------------------------------
// CSwtComboProxy::ShowChoiceListL
// ---------------------------------------------------------------------------
//
EXPORT_C TInt CSwtComboProxy::ShowChoiceListL()
{
    return iDelegate->ShowChoiceListL();
}

// ---------------------------------------------------------------------------
// CSwtComboProxy::HideChoiceList
// ---------------------------------------------------------------------------
//
EXPORT_C void CSwtComboProxy::HideChoiceList()
{
    iDelegate->HideChoiceList();
}

// ---------------------------------------------------------------------------
// CSwtComboProxy::SetSelectedIndex
// ---------------------------------------------------------------------------
//
EXPORT_C void CSwtComboProxy::SetSelectedIndex(const TInt aIndex)
{
    iDelegate->SetSelectedIndex(aIndex);
}

// ---------------------------------------------------------------------------
// CSwtComboProxy::SelectedIndex
// ---------------------------------------------------------------------------
//
EXPORT_C TInt CSwtComboProxy::SelectedIndex() const
{
    return iDelegate->SelectedIndex();
}

// ---------------------------------------------------------------------------
// CSwtComboProxy::SetItems
// ---------------------------------------------------------------------------
//
EXPORT_C void CSwtComboProxy::SetItems(CDesCArray* aArray)
{
    iDelegate->SetItems(aArray);
}

// ---------------------------------------------------------------------------
// CSwtComboProxy::AddItemL
// ---------------------------------------------------------------------------
//
EXPORT_C TInt CSwtComboProxy::AddItemL(const TDesC* aDesC)
{
    return iDelegate->AddItemL(aDesC);
}

// ---------------------------------------------------------------------------
// CSwtComboProxy::RemoveItem
// ---------------------------------------------------------------------------
//
EXPORT_C void CSwtComboProxy::RemoveItem(const TInt aIndex)
{
    iDelegate->RemoveItem(aIndex);
}

// ---------------------------------------------------------------------------
// CSwtComboProxy::SetFlags
// ---------------------------------------------------------------------------
//
EXPORT_C void CSwtComboProxy::SetFlags(const TInt aFlags)
{
    iDelegate->SetFlags(aFlags);
}

// ---------------------------------------------------------------------------
// CSwtComboProxy::Flags
// ---------------------------------------------------------------------------
//
EXPORT_C TInt CSwtComboProxy::Flags() const
{
    return iDelegate->Flags();
}

// ---------------------------------------------------------------------------
// CSwtComboProxy::CountComponentControls
// From CCoeControl
// ---------------------------------------------------------------------------
//
EXPORT_C TInt CSwtComboProxy::CountComponentControls() const
{
    return KSwtAknComboCtrlsCount;
}

// ---------------------------------------------------------------------------
// CSwtComboProxy::ComponentControl
// From CCoeControl
// ---------------------------------------------------------------------------
//
EXPORT_C CCoeControl* CSwtComboProxy::ComponentControl(TInt /*aIdx*/) const
{
    return iDelegate;
}

// ---------------------------------------------------------------------------
// CSwtComboProxy::MinimumSize
// From CCoeControl
// ---------------------------------------------------------------------------
//
EXPORT_C TSize CSwtComboProxy::MinimumSize()
{
    return iDelegate->MinimumSize();
}

// ---------------------------------------------------------------------------
// CSwtComboProxy::SetBackground
// From CCoeControl
// ---------------------------------------------------------------------------
//
EXPORT_C void CSwtComboProxy::SetBackground(const MCoeControlBackground* aBackground)
{
    iDelegate->SetBackground(aBackground);
}

// ---------------------------------------------------------------------------
// CSwtComboProxy::IsFocused
// From CCoeControl
// ---------------------------------------------------------------------------
//
EXPORT_C TBool CSwtComboProxy::IsFocused() const
{
    return iDelegate->IsFocused();
}

// ---------------------------------------------------------------------------
// CSwtComboProxy::IsFocused
// From CCoeControl
// ---------------------------------------------------------------------------
//
EXPORT_C void CSwtComboProxy::SetFocus(TBool aFocus, TDrawNow aDrawNow /*=ENoDrawNow*/)
{
    iDelegate->SetFocus(aFocus, aDrawNow);
}

// ---------------------------------------------------------------------------
// CSwtComboProxy::HandleResourceChange
// From CCoeControl
// ---------------------------------------------------------------------------
//
EXPORT_C void CSwtComboProxy::HandleResourceChange(TInt aType)
{
    iDelegate->HandleResourceChange(aType);
}

// ---------------------------------------------------------------------------
// CSwtComboProxy::SizeChanged
// From CCoeControl
// ---------------------------------------------------------------------------
//
EXPORT_C void CSwtComboProxy::SizeChanged()
{
    iDelegate->SetRect(Rect());
}

// ---------------------------------------------------------------------------
// CSwtComboProxy::HandleControlEventL
// From MCoeControlObserver
// ---------------------------------------------------------------------------
//
EXPORT_C void CSwtComboProxy::HandleControlEventL(CCoeControl* aControl,
        TCoeEvent aEventType)
{
    if (aControl == iDelegate)
    {
        if (Observer())
        {
            Observer()->HandleControlEventL(this, aEventType);
        }
    }
}

// ---------------------------------------------------------------------------
// CSwtComboProxy::CSwtComboProxy
// ---------------------------------------------------------------------------
//
CSwtComboProxy::CSwtComboProxy()
        : CAknControl()
{
}

// ---------------------------------------------------------------------------
// CSwtComboProxy::ConstructL
// ---------------------------------------------------------------------------
//
void CSwtComboProxy::ConstructL(CCoeControl* aParent, CDesCArray* aItemArray)
{
    if (!aParent)
    {
        User::Leave(KErrArgument);
    }

    SetContainerWindowL(*aParent);

    iDelegate = CAknChoiceList::NewL(aParent, aItemArray);
    iDelegate->SetContainerWindowL(*this);
    iDelegate->SetObserver(this);

    SetComponentsToInheritVisibility(ETrue);
}
