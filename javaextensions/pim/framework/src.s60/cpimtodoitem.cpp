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
* Description:  PIM ToDo item specialization.
 *
*/


// CLASS HEADER
#include  "cpimtodoitem.h"

// INTERNAL INCLUDES
#include "cpimitemdata.h"
#include "pimtodo.h"
#include "mpimadaptermanager.h"
#include "cpimtodovalidator.h"
#include "cpimmanager.h"
#include "pimpanics.h"
#include "logger.h"

// FORWARD DECLARATIONS
class MPIMToDoAdapterManager;
class MPIMToDoListAdapter;

// ---------------------------------------------------------------------------
// C++ constructor
// ---------------------------------------------------------------------------
//
CPIMToDoItem::CPIMToDoItem(const CPIMToDoValidator& aValidator) :
        CPIMItem(aValidator)
{
    JELOG2(EPim);
}

// ---------------------------------------------------------------------------
// CPIMToDoItem::NewL
// Two-phase constructor
// ---------------------------------------------------------------------------
//
CPIMToDoItem* CPIMToDoItem::NewL(const CPIMToDoValidator& aValidator)
{
    JELOG2(EPim);
    CPIMToDoItem* self = NewLC(aValidator);
    CleanupStack::Pop(self);
    return self;
}

// ---------------------------------------------------------------------------
// CPIMToDoItem::NewLC
// Two-phase constructor. Leaves item to the cleanup stack
// ---------------------------------------------------------------------------
//
CPIMToDoItem* CPIMToDoItem::NewLC(const CPIMToDoValidator& aValidator)
{
    JELOG2(EPim);
    CPIMToDoItem* self = new(ELeave) CPIMToDoItem(aValidator);
    CleanupStack::PushL(self);
    self->ConstructL();
    return self;
}

pimbaseitem* pimbaseitem::getToDoItemInstance(pimbasemanager* aPimManager)
{
    JELOG2(EPim);
    CPIMManager* pimManager = reinterpret_cast<CPIMManager*>(aPimManager);
    CPIMToDoItem* todoItem = NULL;
    TInt error = 0;
    TRAP(error,
    {
        const CPIMToDoValidator& todoValidator = pimManager->ToDoValidator();
        todoItem = CPIMToDoItem::NewL(todoValidator);
    }
        );
    if (error != KErrNone)
        throw KErrGeneral;
    return todoItem;
}

// ---------------------------------------------------------------------------
// Destructor
// ---------------------------------------------------------------------------
//
CPIMToDoItem::~CPIMToDoItem()
{
    JELOG2(EPim);
}

// ---------------------------------------------------------------------------
// CPIMToDoItem::SetToDoAdapterAssociation
// (other items were commented in a header)
// ---------------------------------------------------------------------------
//
void CPIMToDoItem::SetToDoAdapterAssociation(
    MPIMToDoAdapterManager* aToDoAdapterManager,
    MPIMToDoListAdapter* aToDoListAdapter)
{
    JELOG2(EPim);
    __ASSERT_DEBUG(aToDoAdapterManager, User::Panic(KPIMPanicCategory,
                   EPIMPanicNullArgument));

    iToDoAdapterManager = aToDoAdapterManager;
    iToDoListAdapter = aToDoListAdapter;

    SetBaseAdapterAssociation(iToDoAdapterManager->GetAdapterManager(),
                              iToDoListAdapter ? iToDoListAdapter->GetPimListAdapter() : NULL);
}

// ---------------------------------------------------------------------------
// CPIMToDoItem::RemoveAdapterAssociation
// (other items were commented in a header)
// ---------------------------------------------------------------------------
//
void CPIMToDoItem::RemoveAdapterAssociation()
{
    JELOG2(EPim);
    iToDoAdapterManager = NULL;
    iToDoListAdapter = NULL;

    CPIMItem::RemoveAdapterAssociation();
}

// ---------------------------------------------------------------------------
// CPIMToDoItem::ItemType
// (other items were commented in a header)
// ---------------------------------------------------------------------------
//
TPIMListType CPIMToDoItem::ItemType() const
{
    JELOG2(EPim);
    return EPIMToDoList;
}

// ---------------------------------------------------------------------------
// CPIMToDoItem::commit
// (other items were commented in a header)
// ---------------------------------------------------------------------------
//
void CPIMToDoItem::commit()
{
    JELOG2(EPim);
    TInt error = KErrNone;
    TRAP(error,
    {
        if (iToDoAdapterManager)
        {
            // The item is associated with a list
            if (iToDoListAdapter)
            {
                // The list is open
                if (iItemID->Compare(KPIMNullItemID) == 0)
                {
                    // The item does not have database entry
                    iToDoListAdapter->CreateToDoItemL(*this);
                }
                else
                {
                    iToDoListAdapter->WriteToDoItemL(*this);
                }

                SetModified(EFalse);
                UpdateUidFieldL(EPIMToDoUid, iItemID->Des());
                UpdateRevisionFieldL(EPIMToDoRevision, LastModified());
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
// CPIMToDoItem::ListClosed
// (other items were commented in a header)
// ---------------------------------------------------------------------------
//
void CPIMToDoItem::ListClosed()
{
    JELOG2(EPim);
    iToDoListAdapter = NULL;
    CPIMItem::ListClosed();
}

// ---------------------------------------------------------------------------
// CPIMToDoItem::SetToDoItemIdL
// (other items were commented in a header)
// ---------------------------------------------------------------------------
//
void CPIMToDoItem::SetToDoItemIdL(const TPIMItemID& aToDoItemId)
{
    JELOG2(EPim);
    // Remove old one
    CPIMItem::SetIdL(aToDoItemId);

    // Update UID field if it is supported
    if (iAdapterManager && iAdapterManager->IsSupportedField(
                EPIMToDoUid))
    {
        UpdateUidFieldL(EPIMToDoUid, aToDoItemId);
    }
}

// ---------------------------------------------------------------------------
// CPIMToDoItem::ToDoItemId
// (other items were commented in a header)
// ---------------------------------------------------------------------------
//
const TPIMItemID CPIMToDoItem::ToDoItemId() const
{
    JELOG2(EPim);
    if (!iItemID)
    {
        return KPIMNullItemID();
    }
    return iItemID->Des();
}

// ---------------------------------------------------------------------------
// CPIMToDoItem::SetLastModifiedL
// (other items were commented in a header)
// ---------------------------------------------------------------------------
//
void CPIMToDoItem::SetLastModifiedL(TPIMDate aLastModified)
{
    JELOG2(EPim);
    CPIMItem::SetLastModifiedL(aLastModified);
    if (iAdapterManager)
    {
        UpdateRevisionFieldL(EPIMToDoRevision, LastModified());
    }
}

// ---------------------------------------------------------------------------
// CPIMToDoItem::PrepareForLoadL
// (other items were commented in a header)
// ---------------------------------------------------------------------------
//
void CPIMToDoItem::PrepareForLoadL()
{
    JELOG2(EPim);
    // Prepare base class for loading from the database
    CPIMItem::PrepareForLoadL();
}

// ---------------------------------------------------------------------------
// CPIMToDoItem::IsReadOnly
// (other items were commented in a header)
// ---------------------------------------------------------------------------
//
TBool CPIMToDoItem::IsReadOnly(const TPIMField& aField)
{
    JELOG2(EPim);
    TBool retVal = EFalse;

    if (aField == EPIMToDoUid || aField == EPIMToDoRevision)
    {
        retVal = (iItemID->Compare(KPIMNullItemID) != 0);
    }

    return retVal;
}

// ---------------------------------------------------------------------------
// CPIMToDoItem::AddIntL
// (other items were commented in a header)
// ---------------------------------------------------------------------------
//
void CPIMToDoItem::AddIntL(TPIMField aField, TPIMAttribute aAttributes,
                           TInt aValue)
{
    JELOG2(EPim);
    if (aField == EPIMToDoPriority)
    {
        aValue = SimplifyPriority(aValue);
    }
    // else not the priority field; not interested
    CPIMItem::addInt(aField, aAttributes, aValue);
}

// ---------------------------------------------------------------------------
// CPIMToDoItem::SetIntL
// (other items were commented in a header)
// ---------------------------------------------------------------------------
//
void CPIMToDoItem::SetIntL(TPIMField aField, TInt aIndex,
                           TPIMAttribute aAttributes, TInt aValue)
{
    JELOG2(EPim);
    if (aField == EPIMToDoPriority)
    {
        aValue = SimplifyPriority(aValue);
    }
    // else not the priority field; not interested
    CPIMItem::setInt(aField, aIndex, aAttributes, aValue);
}

// ---------------------------------------------------------------------------
// CPIMToDoItem::SimplifyPriority
// (other items were commented in a header)
// ---------------------------------------------------------------------------
//
TInt CPIMToDoItem::SimplifyPriority(TInt aPriority)
{
    JELOG2(EPim);
    // Note: the lower the value, the higher the priority.
    TInt retVal = aPriority;

    if (aPriority == EPIMToDoPriorityMinValue) // 0
    {
        retVal = EPIMToDoPriorityMedium; // the default
    }
    else if ((EPIMToDoPriorityHigh <= aPriority) && (aPriority
             < EPIMToDoPriorityMedium))
    {
        retVal = EPIMToDoPriorityHigh;
    }
    else if ((EPIMToDoPriorityMedium <= aPriority) && (aPriority
             < EPIMToDoPriorityLow))
    {
        retVal = EPIMToDoPriorityMedium;
    }
    else if ((EPIMToDoPriorityLow <= aPriority) && (aPriority
             <= EPIMToDoPriorityMaxValue))
    {
        retVal = EPIMToDoPriorityLow;
    }

    // else don't touch it
    return retVal;
}

// ---------------------------------------------------------------------------
// CPIMToDoItem::GetItemData
// (other items were commented in a header)
// ---------------------------------------------------------------------------
//
MPIMItemData& CPIMToDoItem::ItemData()
{
    JELOG2(EPim);
    return *iItemData;
}

// ---------------------------------------------------------------------------
// CPIMToDoItem::GetItemData
// (other items were commented in a header)
// ---------------------------------------------------------------------------
//
const MPIMItemData& CPIMToDoItem::ItemData() const
{
    return *iItemData;
}

int CPIMToDoItem::getPreferredIndex(TPIMField /*aField*/) const
{
    // shouldnt reach this code
    return KErrGeneral;
}

//  End of File
