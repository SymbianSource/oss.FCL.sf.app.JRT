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
* Description:  Handles PIM event list <-> Agenda Model conversions
 *
*/


// CLASS HEADER
#include "cpimeventlistadapter.h"

// INTERNAL INCLUDES
#include "mpimeventitem.h"
#include "cpimagneventadapter.h"
#include "logger.h"

// EXTERNAL INCLUDES
#include <calentryview.h>

// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// CPIMEventListAdapter::CPIMEventListAdapter
// C++ default constructor can NOT contain any code, that
// might leave.
// -----------------------------------------------------------------------------
//
CPIMEventListAdapter::CPIMEventListAdapter(
    java::util::FunctionServer* aFuncServer) :
        CPIMAgnListAdapter(aFuncServer)
{
    JELOG2(EPim);
    iChangesRead = EFalse;
}

// -----------------------------------------------------------------------------
// CPIMEventListAdapter::ConstructL
// Symbian 2nd phase constructor can leave.
// -----------------------------------------------------------------------------
//
void CPIMEventListAdapter::ConstructL(CCalEntry::TType aEntryType, 

									  CPIMAgnEventAdapter* aEventAdapter,
                                      TInt aCalSessionInt)
    {
    JELOG2(EPim);
    CCalSession* calSession = reinterpret_cast <CCalSession*> (aCalSessionInt);
    CPIMAgnListAdapter::ConstructL(MCalChangeCallBack::EChangeEntryEvent, calSession);
    iEntryType = aEntryType;
    iAgnAdapter = aEventAdapter;
}

// -----------------------------------------------------------------------------
// CPIMEventListAdapter::NewL
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
CPIMEventListAdapter* CPIMEventListAdapter::NewL(CCalEntry::TType aEntryType,        
        CPIMAgnEventAdapter* aEventAdapter, 
        java::util::FunctionServer* aFuncServer, CCalSession *aCalSession
        	)
    {
    JELOG2(EPim);
    CPIMEventListAdapter* self = new(ELeave) CPIMEventListAdapter(aFuncServer);
    CleanupStack::PushL(self);
    TInt calSessionInt = reinterpret_cast <TInt> (aCalSession);
    CallMethodL(self, &CPIMEventListAdapter::ConstructL, aEntryType,
                aEventAdapter, calSessionInt,self->iFuncServer);

    CleanupStack::Pop( self );
    return self;
}

// Destructor
CPIMEventListAdapter::~CPIMEventListAdapter()
{
    JELOG2(EPim);
    Close();
}

// -----------------------------------------------------------------------------
// CPIMEventListAdapter::GetPimListAdapter
// Provides access to the MPIMListAdapter representation of this object.
// Returns: MPIMListAdapter representation of this object.
// -----------------------------------------------------------------------------
MPIMListAdapter* CPIMEventListAdapter::GetPimListAdapter()
{
    JELOG2(EPim);
    return this;
}

// -----------------------------------------------------------------------------
// CPIMEventListAdapter::CreateEventItemL
// Creates a new Appt item in the native database.
// The adapter creates a new native database entry, sets its data
// according to the data in aEventItem, adds it to the database
// and sets the Item ID of aEventItem. aEventItem must
// contain valid data and have Item ID PIMNullItemID.
// -----------------------------------------------------------------------------
void CPIMEventListAdapter::CreateEventItemL(MPIMEventItem& aEventItem) // The framework item to create to native side.
{
    JELOG2(EPim);
    CallMethodL(this, &CPIMEventListAdapter::DoCreateEventItemL, aEventItem,
                iFuncServer);
}
void CPIMEventListAdapter::DoCreateEventItemL(MPIMEventItem& aEventItem) // The framework item to create to native side.
{
    JELOG2(EPim);
    __ASSERT_ALWAYS(iAgnAdapter || iCalEntryView, User::Leave(KErrNotReady));
    // Create agenda entry item from PIM item
    CCalEntry* entry = iAgnAdapter->CreateItemToAgnL(aEventItem);
    CleanupStack::PushL(entry);

    // Add the Entry to the Agenda model
    RPointerArray<CCalEntry> entryArray;
    CleanupClosePushL(entryArray);
    User::LeaveIfError(entryArray.Append(entry));

    TInt numSuccesfulEntry(0); // not needed
    iCalEntryView->StoreL(entryArray, numSuccesfulEntry);

    CleanupStack::PopAndDestroy(); // entryArray close

    const TDesC8& uidDes = entry->UidL();
    aEventItem.SetEventItemIdL(uidDes);

    CleanupStack::PopAndDestroy(entry);

    // Read the created item to update the fields that either the item adapter
    // or the native Agenda model has altered or added to the PIM item fields.
    DoReadEventItemL(aEventItem);
}

// -----------------------------------------------------------------------------
// CPIMEventListAdapter::ReadEventItemL
// Reads an existing Appt item from the native database.
// The adapter maps the Item ID in aEventItem to a native database
// entry identifier, reads the entry and sets the data of
// aEventItem according to the data in the native entry.
// -----------------------------------------------------------------------------
void CPIMEventListAdapter::ReadEventItemL(MPIMEventItem& aEventItem) // The item to be read from the native side
{
    JELOG2(EPim);
    CallMethodL(this, &CPIMEventListAdapter::DoReadEventItemL, aEventItem,
                iFuncServer);
}

void CPIMEventListAdapter::DoReadEventItemL(MPIMEventItem& aEventItem) // The item to be read from the native side
{
    JELOG2(EPim);
    __ASSERT_ALWAYS(iAgnAdapter, User::Leave(KErrNotReady));

    const TPIMItemID& id = aEventItem.EventItemId();
    CCalEntry* entry = FetchNativeEntryL(id, iEntryType);
    CleanupStack::PushL(entry);

    aEventItem.PrepareForLoadL();
    //CallMethod(iAgnAdapter, &CPIMAgnEventAdapter::ReadAgnToItemL,aEventItem, *entry,iFuncServer);
    iAgnAdapter->ReadAgnToItemL(aEventItem, *entry);

    TCalTime lastModified = entry->LastModifiedDateL();
    aEventItem.SetLastModifiedL(lastModified.TimeUtcL());

    CleanupStack::PopAndDestroy(entry);
}

// -----------------------------------------------------------------------------
// CPIMEventListAdapter::WriteEventItemL
// Writes an existing Appt item to the native database.
// The adapter maps the Item ID in aEventItem to a native database
// entry identifier, reads the entry and sets the data of the entry
// according to the data in aEventItem.
// -----------------------------------------------------------------------------
void CPIMEventListAdapter::WriteEventItemL(MPIMEventItem& aEventItem) // The framework item to write to native side.
{
    JELOG2(EPim);
    CallMethodL(this, &CPIMEventListAdapter::DoWriteEventItemL, aEventItem,
                iFuncServer);
}

void CPIMEventListAdapter::DoWriteEventItemL(MPIMEventItem& aEventItem) // The framework item to write to native side.
{
    JELOG2(EPim);
    __ASSERT_ALWAYS(iAgnAdapter, User::Leave(KErrNotReady));

    const TPIMItemID& id = aEventItem.EventItemId();
    CCalEntry* entry = FetchNativeEntryL(id, iEntryType);
    CleanupStack::PushL(entry);

    iAgnAdapter->UpdateItemToAgnL(aEventItem, *entry);
    RPointerArray<CCalEntry> entryArray;
    User::LeaveIfError(entryArray.Append(entry));
    CleanupClosePushL(entryArray);

    TInt numSuccesfulEntry(0); // not needed
    iCalEntryView->UpdateL(entryArray, numSuccesfulEntry);

    CleanupStack::PopAndDestroy(); // entryArray close
    CleanupStack::PopAndDestroy(entry);

    // Read the updated item to update the fields that either the item adapter
    // or the native Agenda model has altered or added to the PIM item fields.
    DoReadEventItemL(aEventItem);
}

// -----------------------------------------------------------------------------
// CPIMEventListAdapter::RemoveEventItemL
// Removes an existing Appt from the native database.
// The adapter maps aItemID to a native database entry and removes it.
// -----------------------------------------------------------------------------
void CPIMEventListAdapter::RemoveEventItemL(TPIMItemID aItemID)
{
    JELOG2(EPim);
    CallMethodL(this, &CPIMEventListAdapter::DoRemoveEventItemL, aItemID,
                iFuncServer);
}

void CPIMEventListAdapter::DoRemoveEventItemL(TPIMItemID aItemID)
{
    JELOG2(EPim);
    CCalEntry* entry = FetchNativeEntryL(aItemID, iEntryType);
    CleanupStack::PushL(entry);
    iCalEntryView->DeleteL(*entry);
    CleanupStack::PopAndDestroy(entry);
}

// -----------------------------------------------------------------------------
// CPIMEventListAdapter::GetCategoriesL
// Returns: A string array containing the categories defined for the PIM list.
// -----------------------------------------------------------------------------
const CDesCArray& CPIMEventListAdapter::GetCategoriesL()
{
    JELOG2(EPim);
    if (!iEventCategoryArray)
    {
        iEventCategoryArray = new(ELeave) CDesCArrayFlat(1);  // codescanner::leave
    }
    return *iEventCategoryArray; // No categories.
}

// -----------------------------------------------------------------------------
// CPIMEventListAdapter::GetExternalItemModificationsL
// Provides the external changes to the items in the native database.
// Returns: An array of item state change objects. The ownership
//          of the array is transferred to the caller. Note that the
//          array elements contain heap-allocated data.
// -----------------------------------------------------------------------------
RPointerArray<CPIMItemStateChange>*
CPIMEventListAdapter::GetExternalItemModificationsL()
{
    JELOG2(EPim);
    return GetExternalItemModificationsByEntryTypeL(iEntryType);
}

// -----------------------------------------------------------------------------
// CPIMEventListAdapter::Close
// Used to inform the list adapter that the list has been closed.
// The list adapter may then release all resources it has reserved.
// -----------------------------------------------------------------------------
void CPIMEventListAdapter::Close()
{
    JELOG2(EPim);
    delete iAgnAdapter;
    iAgnAdapter = NULL;
    delete iEventCategoryArray;
    iEventCategoryArray = NULL;
}

//  End of File
