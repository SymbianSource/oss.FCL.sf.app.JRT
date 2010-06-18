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
* Description:  Handles PIM todo list <-> Agenda Model todo conversions
 *
*/


// CLASS HEADER
#include "cpimtodolistadapter.h"

// INTERNAL INCLUDES
#include "cpimagntodoadapter.h"
#include "mpimtodoitem.h"
#include "logger.h"

// EXTERNAL INCLUDES
#include <calentryview.h>

// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// CPIMToDoListAdapter::CPIMToDoListAdapter
// C++ default constructor can NOT contain any code, that
// might leave.
// -----------------------------------------------------------------------------
//
CPIMToDoListAdapter::CPIMToDoListAdapter(
    java::util::FunctionServer* aFuncServer) :
        CPIMAgnListAdapter(aFuncServer)
{
    JELOG2(EPim);
    // Mark that the native database has not yet been read
    iChangesRead = EFalse;
}

// -----------------------------------------------------------------------------
// CPIMToDoListAdapter::ConstructL
// Symbian 2nd phase constructor can leave.
// -----------------------------------------------------------------------------
//
void CPIMToDoListAdapter::ConstructL()
{
    JELOG2(EPim);
    CPIMAgnListAdapter::ConstructL(MCalChangeCallBack::EChangeEntryTodo);
    iAgnToDoAdapter = CPIMAgnToDoAdapter::NewL(iFuncServer);
}

// -----------------------------------------------------------------------------
// CPIMToDoListAdapter::NewL
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
CPIMToDoListAdapter* CPIMToDoListAdapter::NewL(
    java::util::FunctionServer* aFuncServer)
{
    JELOG2(EPim);
    CPIMToDoListAdapter* self = new(ELeave) CPIMToDoListAdapter(aFuncServer);
    CleanupStack::PushL(self);
    CallMethodL(self, &CPIMToDoListAdapter::ConstructL, self->iFuncServer);
    CleanupStack::Pop(self);
    return self;
}

// Destructor
CPIMToDoListAdapter::~CPIMToDoListAdapter()
{
    JELOG2(EPim);
    Close();
}

// -----------------------------------------------------------------------------
// CPIMToDoListAdapter::GetPimListAdapter
// Provides access to the MPIMListAdapter representation of this object.
// Returns: MPIMListAdapter representation of this object.
// -----------------------------------------------------------------------------
MPIMListAdapter* CPIMToDoListAdapter::GetPimListAdapter()
{
    JELOG2(EPim);
    return this;
}

// -----------------------------------------------------------------------------
// CPIMToDoListAdapter::CreateToDoItemL
// Creates a new To-do item in the native database.
// The adapter creates a new native database entry, sets its data
// according to the data in aToDoItem, adds it to the database
// and sets the Item ID of aToDoItem. aToDoItem must
// contain valid data and have Item ID PIMNullItemID.
// -----------------------------------------------------------------------------
void CPIMToDoListAdapter::CreateToDoItemL(MPIMToDoItem& aToDoItem) // The framework item to create to native side.
{
    JELOG2(EPim);
    CallMethodL(this, &CPIMToDoListAdapter::DoCreateToDoItemL, aToDoItem,
                iFuncServer);
}

void CPIMToDoListAdapter::DoCreateToDoItemL(MPIMToDoItem& aToDoItem) // The framework item to create to native side.
{
    JELOG2(EPim);
    __ASSERT_ALWAYS(iAgnToDoAdapter || iCalEntryView, User::Leave(KErrNotReady));

    CCalEntry* entry = iAgnToDoAdapter->CreateItemToAgnL(aToDoItem);
    CleanupStack::PushL(entry);

    // Add the Entry to the Agenda model
    RPointerArray<CCalEntry> entryArray;
    CleanupClosePushL(entryArray);
    User::LeaveIfError(entryArray.Append(entry));

    TInt numSuccesfulEntry = 0; // not needed
    iCalEntryView->StoreL(entryArray, numSuccesfulEntry);

    CleanupStack::PopAndDestroy(); // entryArray close

    const TDesC8& uidDes = entry->UidL();
    aToDoItem.SetToDoItemIdL(uidDes);

    CleanupStack::PopAndDestroy(entry);

    // Read the created item to update the fields that either the item adapter
    // or the native Agenda model has altered or added to the PIM item fields.
    DoReadToDoItemL(aToDoItem);
}

// -----------------------------------------------------------------------------
// CPIMToDoListAdapter::ReadToDoItemL
// Reads an existing To-do item from the native database.
// The adapter maps the Item ID in aToDoItem to a native database
// entry identifier, reads the entry and sets the data of
// aToDoItem according to the data in the native entry.
// -----------------------------------------------------------------------------
void CPIMToDoListAdapter::ReadToDoItemL(MPIMToDoItem& aToDoItem) // The item to be read from the native side.
{
    JELOG2(EPim);
    CallMethodL(this, &CPIMToDoListAdapter::DoReadToDoItemL, aToDoItem,
                iFuncServer);
}

void CPIMToDoListAdapter::DoReadToDoItemL(MPIMToDoItem& aToDoItem) // The item to be read from the native side.
{
    JELOG2(EPim);
    __ASSERT_ALWAYS(iAgnToDoAdapter, User::Leave(KErrNotReady));

    const TPIMItemID& id = aToDoItem.ToDoItemId();
    CCalEntry* entry = FetchNativeEntryL(id, CCalEntry::ETodo);
    CleanupStack::PushL(entry);

    aToDoItem.PrepareForLoadL();
    iAgnToDoAdapter->ReadAgnToItemL(aToDoItem, *entry);

    TCalTime lastModified = entry->LastModifiedDateL();
    aToDoItem.SetLastModifiedL(lastModified.TimeUtcL());

    CleanupStack::PopAndDestroy(entry); // CCalEntry* entry
}

// -----------------------------------------------------------------------------
// CPIMToDoListAdapter::WriteToDoItemL
// Writes an existing To-do item to the native database.
// The adapter maps the Item ID in aToDoItem to a native database
// entry identifier, reads the entry and sets the data of the entry
// according to the data in aToDoItem.
// -----------------------------------------------------------------------------
void CPIMToDoListAdapter::WriteToDoItemL(MPIMToDoItem& aToDoItem) // The framework item to write to native side.
{
    JELOG2(EPim);
    CallMethodL(this, &CPIMToDoListAdapter::DoWriteToDoItemL, aToDoItem,
                iFuncServer);
}

void CPIMToDoListAdapter::DoWriteToDoItemL(MPIMToDoItem& aToDoItem) // The framework item to write to native side.
{
    JELOG2(EPim);
    __ASSERT_ALWAYS(iAgnToDoAdapter, User::Leave(KErrNotReady));

    const TPIMItemID& id = aToDoItem.ToDoItemId();
    CCalEntry* entry = FetchNativeEntryL(id, CCalEntry::ETodo);
    CleanupStack::PushL(entry);

    iAgnToDoAdapter->UpdateItemToAgnL(aToDoItem, *entry);
    RPointerArray<CCalEntry> entryArray;
    CleanupClosePushL(entryArray);
    User::LeaveIfError(entryArray.Append(entry));

    TInt numSuccessfulEntry(0); // not needed
    iCalEntryView->UpdateL(entryArray, numSuccessfulEntry);

    CleanupStack::PopAndDestroy(); // entryArray close
    CleanupStack::PopAndDestroy(entry);

    // Read the updated item to update the fields that either the item adapter
    // or the native Agenda model has altered or added to the PIM item fields.
    DoReadToDoItemL(aToDoItem);
}

// -----------------------------------------------------------------------------
// CPIMToDoListAdapter::RemoveToDoItemL
// Removes an existing To-do from the native database.
// The adapter maps aItemID to a native database entry and removes it.
// -----------------------------------------------------------------------------
void CPIMToDoListAdapter::RemoveToDoItemL(TPIMItemID aItemID) // The ID of the item to be removed.
{
    JELOG2(EPim);
    CallMethodL(this, &CPIMToDoListAdapter::DoRemoveToDoItemL, aItemID,
                iFuncServer);
}

void CPIMToDoListAdapter::DoRemoveToDoItemL(TPIMItemID aItemID) // The ID of the item to be removed.
{
    JELOG2(EPim);
    CCalEntry* entry = FetchNativeEntryL(aItemID, CCalEntry::ETodo);
    CleanupStack::PushL(entry);
    iCalEntryView->DeleteL(*entry);
    CleanupStack::PopAndDestroy(entry);
}

// -----------------------------------------------------------------------------
// CPIMToDoListAdapter::GetCategoriesL
// Returns: A zero-length array, ToDo list has no categories.
// -----------------------------------------------------------------------------
const CDesCArray& CPIMToDoListAdapter::GetCategoriesL()
{
    JELOG2(EPim);
    if (!iToDoCategoryArray)
    {
        iToDoCategoryArray = new(ELeave) CDesCArrayFlat(1); // codescanner::leave
    }
    return *iToDoCategoryArray; // No categories.
}

// -----------------------------------------------------------------------------
// CPIMToDoListAdapter::GetExternalItemModificationsL
// Provides the external changes to the items in the native database.
// Returns: An array of item state change objects. The ownership
//          of the array is transferred to the caller. Note that the
//          array elements contain heap-allocated data.
// -----------------------------------------------------------------------------
RPointerArray<CPIMItemStateChange>*
CPIMToDoListAdapter::GetExternalItemModificationsL()
{
    JELOG2(EPim);
    return GetExternalItemModificationsByEntryTypeL(CCalEntry::ETodo);
}

// -----------------------------------------------------------------------------
// CPIMToDoListAdapter::Close
// Used to inform the list adapter that the list has been closed.
// The list adapter may then release all resources it has reserved.
// -----------------------------------------------------------------------------
void CPIMToDoListAdapter::Close()
{
    JELOG2(EPim);
    delete iAgnToDoAdapter;
    iAgnToDoAdapter = NULL;
    delete iToDoCategoryArray;
    iToDoCategoryArray = NULL;
}

//  End of File
