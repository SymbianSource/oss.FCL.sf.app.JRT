/*
* Copyright (c) 2008 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  Event item implementation.
 *
*/


// CLASS HEADER
#include "cpimeventitem.h"

// INTERNAL INCLUDES
#include "cpimitemdata.h"
#include "pimevent.h"
#include "mpimeventadaptermanager.h"
#include "mpimeventlistadapter.h"
#include "cpimrepeatrule.h"
#include "cpimeventvalidator.h"
#include "cpimmanager.h"
#include "pimpanics.h"
#include "logger.h"

// ---------------------------------------------------------------------------
// C++ constructor
// ---------------------------------------------------------------------------
//
CPIMEventItem::CPIMEventItem(const CPIMEventValidator& aValidator) :
        CPIMItem(aValidator), iIsRepeating(EFalse), iIsRepeatRuleOwned(ETrue)
{
    JELOG2(EPim);
}

// ---------------------------------------------------------------------------
// CPIMEventItem::ConstructL
// Second phase constructor
// ---------------------------------------------------------------------------
//
void CPIMEventItem::ConstructL()
{
    JELOG2(EPim);
    CPIMItem::ConstructL();

    iRepeatRule = CPIMRepeatRule::NewL();
}

// ---------------------------------------------------------------------------
// CPIMEventItem::NewL
// Two-phase constructor
// ---------------------------------------------------------------------------
//
CPIMEventItem* CPIMEventItem::NewL(const CPIMEventValidator& aValidator)
{
    JELOG2(EPim);
    CPIMEventItem* self = NewLC(aValidator);
    CleanupStack::Pop(self);
    return self;
}

// ---------------------------------------------------------------------------
// CPIMEventItem::NewLC
// Two-phase constructor. Leaves item to the cleanup stack
// ---------------------------------------------------------------------------
//
CPIMEventItem* CPIMEventItem::NewLC(const CPIMEventValidator& aValidator)
{
    JELOG2(EPim);
    CPIMEventItem* self = new(ELeave) CPIMEventItem(aValidator);
    CleanupStack::PushL(self);
    self->ConstructL();
    return self;
}

pimbaseitem* pimbaseitem::getEventItemInstance(pimbasemanager* aPimManager)
{
    JELOG2(EPim);
    CPIMManager* pimManager = reinterpret_cast<CPIMManager*>(aPimManager);
    CPIMEventItem* eventItem = NULL;
    TInt error = 0;
    TRAP(error,
    {
        const CPIMEventValidator& eventValidator = pimManager->EventValidator();
        eventItem = CPIMEventItem::NewL(eventValidator);
    }
        );
    if (error != KErrNone)
        throw KErrGeneral;
    return eventItem;
}

// ---------------------------------------------------------------------------
// Destructor
// ---------------------------------------------------------------------------
//
CPIMEventItem::~CPIMEventItem()
{
    JELOG2(EPim);
    if (iIsRepeatRuleOwned)
    {
        delete iRepeatRule;
    }
}

// ---------------------------------------------------------------------------
// CPIMEventItem::SetEventAdapterAssociation
// (other items were commented in a header)
// ---------------------------------------------------------------------------
//
void CPIMEventItem::SetEventAdapterAssociation(
    MPIMEventAdapterManager* aEventAdapterManager,
    MPIMEventListAdapter* aEventListAdapter)
{
    JELOG2(EPim);
    __ASSERT_DEBUG(aEventAdapterManager, User::Panic(KPIMPanicCategory,
                   EPIMPanicNullArgument));

    iEventAdapterManager = aEventAdapterManager;
    iEventListAdapter = aEventListAdapter;

    SetBaseAdapterAssociation(iEventAdapterManager->GetAdapterManager(),
                              iEventListAdapter ? iEventListAdapter->GetPimListAdapter() : NULL);
}

// ---------------------------------------------------------------------------
// CPIMEventItem::RemoveAdapterAssociation
// (other items were commented in a header)
// ---------------------------------------------------------------------------
//
void CPIMEventItem::RemoveAdapterAssociation()
{
    JELOG2(EPim);
    iEventAdapterManager = NULL;
    iEventListAdapter = NULL;

    CPIMItem::RemoveAdapterAssociation();
}

// ---------------------------------------------------------------------------
// CPIMEventItem::GetRepeatTransferOwnership
// (other items were commented in a header)
// ---------------------------------------------------------------------------
//
CPIMRepeatRule* CPIMEventItem::GetRepeatTransferOwnership()
{
    JELOG2(EPim);
    iIsRepeatRuleOwned = EFalse;
    return iRepeatRule;
}

// ---------------------------------------------------------------------------
// CPIMEventItem::ItemType
// (other items were commented in a header)
// ---------------------------------------------------------------------------
//
TPIMListType CPIMEventItem::ItemType() const
{
    JELOG2(EPim);
    return EPIMEventList;
}

// ---------------------------------------------------------------------------
// CPIMEventItem::commit
// (other items were commented in a header)
// ---------------------------------------------------------------------------
//
void CPIMEventItem::commit()
{
    JELOG2(EPim);
    TInt error = KErrNone;
    TRAP(error,
    {
        if (iEventAdapterManager)
        {
            // The item is associated with a list
            if (iEventListAdapter)
            {
                // The list is open
                if (iItemID->Compare(KPIMNullItemID) == 0)
                {
                    // The item does not have database entry
                    iEventListAdapter->CreateEventItemL(*this);
                }
                else
                {
                    iEventListAdapter->WriteEventItemL(*this);
                }

                SetModified(EFalse);
                UpdateUidFieldL(EPIMEventUid, iItemID->Des());
                UpdateRevisionFieldL(EPIMEventRevision, LastModified());
            }
            else
            {
                // The associated list is closed
                User::Leave(KErrDisconnected);   // codescanner::leave
            }
        }
        else
        {
            // The item is not associated with a list
            User::Leave(KErrDisMounted);   // codescanner::leave
        }
    }
        );
    if (error != KErrNone)
        throw error;
}

// ---------------------------------------------------------------------------
// CPIMEventItem::ListClosed
// (other items were commented in a header)
// ---------------------------------------------------------------------------
//
void CPIMEventItem::ListClosed()
{
    JELOG2(EPim);
    iEventListAdapter = NULL;
    CPIMItem::ListClosed();
}

// ---------------------------------------------------------------------------
// CPIMEventItem::SetEventItemIdL
// (other items were commented in a header)
// ---------------------------------------------------------------------------
//
void CPIMEventItem::SetEventItemIdL(const TPIMItemID& aEventItemId)
{
    JELOG2(EPim);
    // Set item id for the base class
    CPIMItem::SetIdL(aEventItemId);

    // Update UID field if it is supported
    if (iAdapterManager && iAdapterManager->IsSupportedField(
                EPIMEventUid))
    {
        UpdateUidFieldL(EPIMEventUid, aEventItemId);
    }
}

// ---------------------------------------------------------------------------
// CPIMEventItem::EventItemId
// (other items were commented in a header)
// ---------------------------------------------------------------------------
//
const TPIMItemID CPIMEventItem::EventItemId() const
{
    JELOG2(EPim);
    if (!iItemID)
    {
        return KPIMNullItemID();
    }
    return iItemID->Des();
}

// ---------------------------------------------------------------------------
// CPIMEventItem::SetLastModifiedL
// (other items were commented in a header)
// ---------------------------------------------------------------------------
//
void CPIMEventItem::SetLastModifiedL(TPIMDate aLastModified)
{
    JELOG2(EPim);
    CPIMItem::SetLastModifiedL(aLastModified);
    // Update revision field if it supported
    if (iAdapterManager && iAdapterManager->IsSupportedField(
                EPIMEventRevision))
    {
        UpdateRevisionFieldL(EPIMEventRevision, LastModified());
    }
}

// ---------------------------------------------------------------------------
// CPIMEventItem::PrepareForLoadL
// (other items were commented in a header)
// ---------------------------------------------------------------------------
//
void CPIMEventItem::PrepareForLoadL()
{
    JELOG2(EPim);
    // Prepare base class for loading from the database
    CPIMItem::PrepareForLoadL();
}

// ---------------------------------------------------------------------------
// CPIMEventItem::IsReadOnly
// (other items were commented in a header)
// ---------------------------------------------------------------------------
//
TBool CPIMEventItem::IsReadOnly(const TPIMField& aField)
{
    JELOG2(EPim);
    TBool retVal = EFalse;

    if (aField == EPIMEventUid || aField == EPIMEventRevision)
    {
        retVal = (iItemID->Compare(KPIMNullItemID) != 0);
    }

    return retVal;
}

// ---------------------------------------------------------------------------
// CPIMEventItem::GetItemData
// (other items were commented in a header)
// ---------------------------------------------------------------------------
//
MPIMItemData& CPIMEventItem::ItemData()
{
    JELOG2(EPim);
    return *iItemData;
}

// ---------------------------------------------------------------------------
// CPIMEventItem::GetItemData
// (other items were commented in a header)
// ---------------------------------------------------------------------------
//
const MPIMItemData& CPIMEventItem::ItemData() const
{
    JELOG2(EPim);
    return *iItemData;
}

// ---------------------------------------------------------------------------
// CPIMEventItem::GetRepeat
// (other items were commented in a header)
// ---------------------------------------------------------------------------
//
MPIMRepeatRuleData* CPIMEventItem::GetRepeat()
{
    JELOG2(EPim);
    return iRepeatRule;
}

// ---------------------------------------------------------------------------
// CPIMEventItem::GetRepeat
// (other items were commented in a header)
// ---------------------------------------------------------------------------
//
const MPIMRepeatRuleData* CPIMEventItem::GetRepeat() const
{
    JELOG2(EPim);
    return iRepeatRule;
}

// ---------------------------------------------------------------------------
// CPIMEventItem::IsRepeating
// (other items were commented in a header)
// ---------------------------------------------------------------------------
//
TBool CPIMEventItem::IsRepeating() const
{
    JELOG2(EPim);
    return iIsRepeating;
}

// ---------------------------------------------------------------------------
// CPIMEventItem::StaticIsRepeating
// (other items were commented in a header)
// ---------------------------------------------------------------------------
//
bool CPIMEventItem::isItemRepeating() const
{
    JELOG2(EPim);
    if (IsRepeating())
        return true;
    return false;
}

// ---------------------------------------------------------------------------
// CPIMEventItem::SetRepeating
// (other items were commented in a header)
// ---------------------------------------------------------------------------
//
void CPIMEventItem::SetRepeating(TBool aSetRepeating)
{
    JELOG2(EPim);
    iIsRepeating = aSetRepeating;
    SetModified();
}

void CPIMEventItem::setItemRepeating(jboolean aSetRepeating)
{
    JELOG2(EPim);
    SetRepeating(static_cast<TBool>(aSetRepeating));
}

int CPIMEventItem::getRepeatHandle()
{
    JELOG2(EPim);
    pimbaserepeatrule* repeatRule = NULL;
    repeatRule = GetRepeatTransferOwnership();
    // We now own the RepeatRule
    int repeatRuleHandle = reinterpret_cast<int>(repeatRule);
    return repeatRuleHandle;
}

int CPIMEventItem::getPreferredIndex(TPIMField /*aField*/) const
{
    JELOG2(EPim);
    // shouldnt reach this code
    return KErrGeneral;
}

//  End of File
