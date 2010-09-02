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
* Description:  Base class for the event and todo list adapters
 *
*/


// INCLUDE FILES
#include "cpimagnlistadapter.h"
#include "mpimitemdata.h"
#include "cleanupresetanddestroy.h"
#include "cpimagnserverwait.h"
#include "logger.h"

#include <calsession.h>
#include <calentryview.h>
#include <caltime.h>
#include <caliterator.h>

namespace
{
// CONSTANTS
const TInt KServerMaxWait = 30000000; // 30 secs = 3*10^7 microsecs
}

// ============================ MEMBER FUNCTIONS ===============================


// -----------------------------------------------------------------------------
// CPIMAgnListAdapter::CPIMAgnListAdapter
// C++ constructor.
// -----------------------------------------------------------------------------
//
CPIMAgnListAdapter::CPIMAgnListAdapter(java::util::FunctionServer* aFuncServer)
{
    JELOG2(EPim);
    iFuncServer = aFuncServer;
}

// -----------------------------------------------------------------------------
// CPIMAgnListAdapter::ConstructL
// Symbian 2nd phase constructor can leave.
// -----------------------------------------------------------------------------
//
void CPIMAgnListAdapter::ConstructL(

    MCalChangeCallBack::TChangeEntryType aEntryType,
    CCalSession* aCalSession)
{
    JELOG2(EPim);

    iServerWait = CPIMAgnServerWait::NewL();
    iCalSession = aCalSession;
    iCalEntryView = CCalEntryView::NewL(*iCalSession, *iServerWait);
    iServerWait->WaitCompleteL(KServerMaxWait);

    iCalSession->StartChangeNotification(this, aEntryType, ETrue, // include undated ToDos, if ToDos are observed
                                         TCalTime::MinTime(), TCalTime::MaxTime());
}

// Destructor
CPIMAgnListAdapter::~CPIMAgnListAdapter()
{
    JELOG2(EPim);
    CallMethod(this, &CPIMAgnListAdapter::DoClose, iFuncServer);
}

void CPIMAgnListAdapter::DoClose()
{
    JELOG2(EPim);
    delete iServerWait;
    iServerWait = NULL;
    if (iItemChangeArray) // exists if there are pending updates
    {
        iItemChangeArray->ResetAndDestroy();
        delete iItemChangeArray;
        iItemChangeArray = NULL;
    }
    CloseAgendaSession();
}

// -----------------------------------------------------------------------------
// CPIMAgnListAdapter::AddCategoryL
// No categories supported.
// -----------------------------------------------------------------------------
void CPIMAgnListAdapter::AddCategoryL(const TDesC& /* aNewCategory */)
{
    JELOG2(EPim);
    User::Leave(KErrNotSupported); // No categories in ToDo list.
}

// -----------------------------------------------------------------------------
// CPIMAgnListAdapter::DeleteCategoryL
// No categories supported.
// -----------------------------------------------------------------------------
void CPIMAgnListAdapter::DeleteCategoryL(const TDesC& /* aCategory */)
{
    JELOG2(EPim);
    User::Leave(KErrNotSupported); // No categories in ToDo list.
}

// -----------------------------------------------------------------------------
// CPIMAgnListAdapter::RenameCategoryL
// No categories supported.
// -----------------------------------------------------------------------------
void CPIMAgnListAdapter::RenameCategoryL(const TDesC& /* aOldCategory*/,
        const TDesC& /* aNewCategory */)
{
    JELOG2(EPim);
    User::Leave(KErrNotSupported); // No categories in ToDo list.
}

// -----------------------------------------------------------------------------
// CPIMAgnListAdapter::IsCategoriesExternallyModified
// Returns: EFalse; no categories supported.
// -----------------------------------------------------------------------------
TBool CPIMAgnListAdapter::IsCategoriesExternallyModified()
{
    JELOG2(EPim);
    return EFalse; // No categories.
}

// -----------------------------------------------------------------------------
// CPIMAgnListAdapter::CalChangeNotification
// Handles change notifications from calendar server, putting changes into
// change queue.
// -----------------------------------------------------------------------------
void CPIMAgnListAdapter::CalChangeNotification(
    MCalChangeCallBack::TChangeEntryType /*aChangeEntryType*/)
{
    JELOG2(EPim);
    iChangesRead = EFalse;
}

// -----------------------------------------------------------------------------
// CPIMAgnListAdapter::GetExternalCategoryModificationsL
// Returns: NULL; no categories supported.
// -----------------------------------------------------------------------------
RPointerArray<CPIMCategoryStateChange>*
CPIMAgnListAdapter::GetExternalCategoryModificationsL()
{
    JELOG2(EPim);
    return (RPointerArray<CPIMCategoryStateChange>*) NULL; // No categories.
}

// -----------------------------------------------------------------------------
// CPIMApptListAdapter::IsItemsExternallyModified
// Checks whether there have been external changes to the items in
// the native database after last call to
// GetExternalItemModifications or list adapter creation.
// Returns: ETrue  if there are any external changes
//          EFalse otherwise
// -----------------------------------------------------------------------------
MPIMListAdapter::TExternalItemChangeClass CPIMAgnListAdapter::IsItemsExternallyModified()
{
    JELOG2(EPim);
    if (!iChangesRead)
    {
        return MPIMListAdapter::EExternalChangesMajor;
    }
    return MPIMListAdapter::EExternalChangesNone;
}

// -----------------------------------------------------------------------------
// CPIMApptListAdapter::GetExternalItemModificationsByEntryTypeL
// Provides the external changes to the items in the native database.
// Returns: An array of item state change objects. The ownership
//          of the array is transferred to the caller. Note that the
//          array elements contain heap-allocated data.
// -----------------------------------------------------------------------------
RPointerArray<CPIMItemStateChange>*
CPIMAgnListAdapter::GetExternalItemModificationsByEntryTypeL(
    CCalEntry::TType aEntryType)
{
    JELOG2(EPim);
    CallMethodL(this,
                &CPIMAgnListAdapter::DoExternalItemModificationsByEntryTypeL,
                aEntryType, iFuncServer);
    RPointerArray<CPIMItemStateChange>* retVal = iItemChangeArray;
    iItemChangeArray = NULL;
    return retVal;
}

void CPIMAgnListAdapter::DoExternalItemModificationsByEntryTypeL(
    CCalEntry::TType aEntryType)
{
    JELOG2(EPim);
    EnsureOpenSessionL();

    if (!iChangesRead)
    {
        if (!iItemChangeArray)
        {
            iItemChangeArray
            = new(ELeave) RPointerArray<CPIMItemStateChange> (10);
        }

        // Read all the entries in the agenda file
        RPointerArray<CCalEntry> entryArray;
        CCalIter* iterator = CCalIter::NewL(*iCalSession);
        CleanupStack::PushL(iterator);

        const TDesC8* entryId = &(iterator->FirstL());
        while ((*entryId) != KNullDesC8())
        {
            iCalEntryView->FetchL(*entryId, entryArray);
            CleanupResetAndDestroyPushL(entryArray);

            // There should be at least one entry in the array. Other instances
            // will be ignored since child entries are currently not supported
            if (entryArray.Count() > 0)
            {
                // Get only the parent item from the array. Other items are
                // simply ignored since PIM API does not support them
                CCalEntry* entry = entryArray[0];
                // Check that the entry type is correct and add the item
                // to the change list
                if (entry->EntryTypeL() == aEntryType)
                {
                    HBufC8* changeId = entryId->AllocLC();
                    CPIMItemStateChange
                    * change =
                        new(ELeave) CPIMItemStateChange(changeId, EPIMExternalChangeNew);
                    CleanupStack::Pop(changeId);
                    CleanupStack::PushL(change);
                    iItemChangeArray->AppendL(change);
                    CleanupStack::Pop(change);
                }
            }
            CleanupStack::PopAndDestroy(&entryArray);
            // Get next calendar entry from the iterator
            entryId = &(iterator->NextL());
        }
        CleanupStack::PopAndDestroy(iterator);

        iChangesRead = ETrue;
    }
}

// -----------------------------------------------------------------------------
// CPIMAgnListAdapter::FetchNativeEntryL
// Fetches an entry according to a PIM item id.
// Returns: A calendar entry.
//          The ownership of the item is transferred to the caller.
// -----------------------------------------------------------------------------
//
CCalEntry* CPIMAgnListAdapter::FetchNativeEntryL(TPIMItemID aItemId,
        CCalEntry::TType aEntryType)
{
    JELOG2(EPim);
    EnsureOpenSessionL();
    __ASSERT_ALWAYS(aItemId != KPIMNullItemID, User::Leave(KErrArgument));

    RPointerArray<CCalEntry> entryArray;
    CleanupResetAndDestroyPushL(entryArray);
    iCalEntryView->FetchL(aItemId, entryArray);

    // Only one entry should be returned. NOTE that if there is no items
    // in the fetched array it means that the item might have been removed
    // so, we MUST NOT panic here as we did previously...
    __ASSERT_ALWAYS(entryArray.Count() > 0 && entryArray[0]->EntryTypeL()
                    == aEntryType, User::Leave(KErrNotFound));

    CCalEntry* entry = entryArray[0];
    // Take the ownership of the first item. Child instances are simply ignored
    entryArray.Remove(0);
    CleanupStack::PopAndDestroy(&entryArray);
    return entry;
}

// -----------------------------------------------------------------------------
// CPIMAgnListAdapter::CloseAgendaSession
// Closes the calendar server session, deleting session and model.
// -----------------------------------------------------------------------------
//
void CPIMAgnListAdapter::CloseAgendaSession()
{
    JELOG2(EPim);
    iCalSession->StopChangeNotification();
    delete iCalEntryView;
    iCalEntryView = NULL;
    iCalSession = NULL;

    iChangesRead = ETrue;
}

// -----------------------------------------------------------------------------
// CPIMAgnListAdapter::EnsureOpenSessionL
// Leaves if the session is closed.
// -----------------------------------------------------------------------------
//
void CPIMAgnListAdapter::EnsureOpenSessionL()
{
    JELOG2(EPim);
    __ASSERT_ALWAYS(iCalSession, User::Leave(KErrNotReady));
}

//  End of File
