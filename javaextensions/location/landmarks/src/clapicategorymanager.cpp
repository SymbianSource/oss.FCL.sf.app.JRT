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
* Description:  Location API category manager
 *
*/


// INTERNAL INCLUDES
#include    "clapicategorymanager.h"
#include    "mlapilandmarkstore.h"
#include    "mlapilmdatabaseeventnotifier.h"
#include    "logger.h"
// EXTERNAL INCLUDES
#include    <EPos_CPosLmCategoryManager.h>
#include    <EPos_CPosLandmarkDatabase.h>

// UNNAMED LOCAL NAMESPACE
namespace
{
// Minimum usage of the native landmark database. If the usage drops
// below this level, the database will be compacted to avoid that the
// Landmark Server would do that which eventually locks the database
const TReal KLAPIMinCompactUsage = 0.7;
}

// ---------------------------------------------------------------------------
// CLAPICategoryManager::NewL
// ---------------------------------------------------------------------------
//
CLAPICategoryManager* CLAPICategoryManager::NewL(const TCtorParams& aParams)
{
    JELOG2(EJavaLocation);
    CLAPICategoryManager* self = CLAPICategoryManager::NewLC(aParams);
    CleanupStack::Pop(self);
    return self;
}

// ---------------------------------------------------------------------------
// CLAPICategoryManager::NewLC
// ---------------------------------------------------------------------------
//
CLAPICategoryManager* CLAPICategoryManager::NewLC(const TCtorParams& aParams)
{
    JELOG2(EJavaLocation);
    CLAPICategoryManager* self = new(ELeave) CLAPICategoryManager(aParams);
    CleanupStack::PushL(self);
    self->ConstructL();
    return self;
}

// ---------------------------------------------------------------------------
// CLAPICategoryManager::~CLAPICategoryManager
// ---------------------------------------------------------------------------
//
CLAPICategoryManager::~CLAPICategoryManager()
{
    JELOG2(EJavaLocation);
    if (iEventNotifier)
    {
        iEventNotifier->RemoveObserver(this);
    }
    iCategoryIds.Close();
    delete iCategoryNames;
    delete iCategoryManager;
}

// ---------------------------------------------------------------------------
// CLAPICategoryManager::ListCategoriesL
// ---------------------------------------------------------------------------
//
const CDesCArray& CLAPICategoryManager::ListCategoriesL()
{
    JELOG2(EJavaLocation);
    // Check if Event listening is supported so it is not necessary to
    // refresh the whole list of categories in the database. Note that
    // if the amount of categories is huge, this operation may take some
    // time so it is always better to track database events if possible
    if (!iEventNotifier || !iEventNotifier->IsEventListeningSupported())
    {
        RefreshCategoryListL();
    }
    // Return a reference to the current list of categories
    return *iCategoryNames;
}

// ---------------------------------------------------------------------------
// CLAPICategoryManager::AddCategoryL
// ---------------------------------------------------------------------------
//
void CLAPICategoryManager::AddCategoryL(const TDesC& aName)
{
    JELOG2(EJavaLocation);
    CPosLandmarkCategory* category = CPosLandmarkCategory::NewLC();
    // The native Landmarks API supports only specific length category names
    // There is no other option but to truncat the rest of the name off
    TPtrC name = aName.Left(KPosLmMaxCategoryNameLength);
    category->SetCategoryNameL(name);
    // Manager does not take the ownership
    TPosLmItemId id = iCategoryManager->AddCategoryL(*category);
    CleanupStack::PopAndDestroy(category);
    // Store the information about the category if it was successfully added
    if (id != KPosLmNullItemId)
    {
        // Store the new id and category name
        TInt position = iCategoryNames->InsertIsqL(name, ECmpCollated);
        // Insert the element to the same place in the id array
        TInt error = iCategoryIds.Insert(id, position);
        if (error != KErrNone)
        {
            // The arrays must be in sync so remove the added id
            iCategoryNames->Delete(position);
            User::Leave(error);
        }
    }
}

// ---------------------------------------------------------------------------
// CLAPICategoryManager::RemoveCategoryL
// ---------------------------------------------------------------------------
//
void CLAPICategoryManager::RemoveCategoryL(const TDesC& aName)
{
    JELOG2(EJavaLocation);
    // Remove the category if it exists. If not, this function call is
    // silently discareded as specified in the Location API specification
    TInt index = FindCategory(aName);
    if (index != KErrNotFound)
    {
        ExecuteAndDeleteLD(iCategoryManager->RemoveCategoryL(
                               iCategoryIds[index]));
        // Remove the category from the list
        iCategoryIds.Remove(index);
        iCategoryNames->Delete(index);
    }
}

// ---------------------------------------------------------------------------
// CLAPICategoryManager::AddLandmarkToCategoryL
// ---------------------------------------------------------------------------
//
void CLAPICategoryManager::AddLandmarkToCategoryL(MLAPILandmark& aLandmark,
        const TDesC& aCategoryName)
{
    JELOG2(EJavaLocation);
    TInt index = FindCategory(aCategoryName);
    // Leave if the requested category was not found
    __ASSERT_ALWAYS(index != KErrNotFound, User::Leave(KErrArgument));
    TLAPIItemId categoryId = iCategoryIds[index];

    // Load the landmark information from the database and update categories
    CPosLandmark* landmark = iLandmarkDatabase.ReadLandmarkLC(aLandmark.Id());
    landmark->AddCategoryL(categoryId);

    // Currently, the passed landmark does not have to know to which categories
    // it belongs to so there is no need to update the information in the
    // landmark. This will change in Location API v2.0

    // Update the existing landmark into the database.
    iLandmarkDatabase.UpdateLandmarkL(*landmark);
    CleanupStack::PopAndDestroy(landmark);
}

// ---------------------------------------------------------------------------
// CLAPICategoryManager::RemoveLandmarkFromCategoryL
// ---------------------------------------------------------------------------
//
void CLAPICategoryManager::RemoveLandmarkFromCategoryL(
    MLAPILandmark& aLandmark, const TDesC& aCategoryName)
{
    JELOG2(EJavaLocation);
    TInt index = FindCategory(aCategoryName);
    // The function call is silently discarded if the specified category
    // does not exist in the landmark store
    if (index != KErrNotFound)
    {
        TLAPIItemId categoryId = iCategoryIds[index];
        // Load the landmark information from the database and update categories
        CPosLandmark* lm = iLandmarkDatabase.ReadLandmarkLC(aLandmark.Id());
        lm->RemoveCategory(categoryId);
        // Update the existing landmark into the database.
        iLandmarkDatabase.UpdateLandmarkL(*lm);
        CleanupStack::PopAndDestroy(lm);

        // Currently, the passed landmark does not have to know to which
        // categories it belongs to so there is no need to update the information
        // in the landmark. This will change in Location API v2.0
    }
}

// ---------------------------------------------------------------------------
// CLAPICategoryManager::NotifyEvent
// ---------------------------------------------------------------------------
//
void CLAPICategoryManager::NotifyEvent(TPosLmEventType& aEventType,
                                       TPosLmItemId& aItemId)
{
    JELOG2(EJavaLocation);
    // Handle the change. The error must be ignored because there is no way
    // to recover from out of memory situation
    TRAP_IGNORE(HandleCategoryChangeL(aEventType, aItemId));
}

// ---------------------------------------------------------------------------
// CLAPICategoryManager::ConstructL
// ---------------------------------------------------------------------------
//
void CLAPICategoryManager::ConstructL()
{
    JELOG2(EJavaLocation);
    iCategoryManager = CPosLmCategoryManager::NewL(iLandmarkDatabase);
    iCategoryNames = new(ELeave) CDesCArrayFlat(5);
    // Refresh the category list for the first time here. Note that this
    // is called only here if the event notifier supports listening changes
    RefreshCategoryListL();
    // Start listening category changes if it is supported by the event notifier
    if (iEventNotifier && iEventNotifier->IsEventListeningSupported())
    {
        iEventNotifier->AddObserverL(this);
    }
}

// ---------------------------------------------------------------------------
// CLAPICategoryManager::CLAPICategoryManager
// ---------------------------------------------------------------------------
//
CLAPICategoryManager::CLAPICategoryManager(const TCtorParams& aParams) :
        iLandmarkDatabase(*(aParams.iLandmarkDatabase)), iEventNotifier(
            aParams.iEventNotifier)
{
    JELOG2(EJavaLocation);
    iLandmarkDatabasePtr = aParams.iLandmarkDatabase;
}

// ---------------------------------------------------------------------------
// CLAPICategoryManager::RefreshCategoryListL
// ---------------------------------------------------------------------------
//
void CLAPICategoryManager::RefreshCategoryListL()
{
    JELOG2(EJavaLocation);
    // Reset any existing category information
    iCategoryIds.Reset();
    iCategoryNames->Reset();
    // Initialize the list of current categories. The order is handled internally
    CPosLmItemIterator* iterator = iCategoryManager->CategoryIteratorL();
    CleanupStack::PushL(iterator);
    // Do not iterate categories if there is nothing to iterate
    if (iterator->NumOfItemsL() == 0)
    {
        CleanupStack::PopAndDestroy(iterator);
        return;
    }
    RArray<TPosLmItemId> ids;
    CleanupClosePushL(ids);
    // Get the prepared category identifiers from the iterator
    iterator->GetItemIdsL(ids, 0, iterator->NumOfItemsL());

    // Read all available categories
    TInt itemCount = ids.Count();
    for (TInt i = 0; i < itemCount; i++)
    {
        TPosLmItemId id = ids[i];
        // Read category information from the category manager
        DoReadCategoryL(id);
    }
    CleanupStack::PopAndDestroy(2, iterator); // The id array is the second
}

// ---------------------------------------------------------------------------
// CLAPICategoryManager::DoReadCategoryL
// ---------------------------------------------------------------------------
//
void CLAPICategoryManager::DoReadCategoryL(TPosLmItemId& aItemId)
{
    JELOG2(EJavaLocation);
    // Read category information from the category manager
    CPosLandmarkCategory* cat = iCategoryManager->ReadCategoryLC(aItemId);
    // Get the information from the category
    TPtrC namePtr;
    User::LeaveIfError(cat->GetCategoryName(namePtr));
    // Add the category to the name array and update the id array also
    TInt position = iCategoryNames->InsertIsqL(namePtr, ECmpCollated);
    // Insert new id or append if the array
    TInt error = iCategoryIds.Insert(aItemId, position);
    if (error != KErrNone)
    {
        // The arrays must be in sync so remove the added id
        iCategoryNames->Delete(position);
        User::Leave(error);
    }
    CleanupStack::PopAndDestroy(cat);
}

// ---------------------------------------------------------------------------
// CLAPICategoryManager::FindCategory
// ---------------------------------------------------------------------------
//
TInt CLAPICategoryManager::FindCategory(const TDesC& aName) const
{
    JELOG2(EJavaLocation);
    TInt pos = 0;
    TInt found = iCategoryNames->FindIsq(aName, pos, ECmpCollated);
    // Return the index of the category name. KErrNotFound if the category
    // was not found from the category names list
    return found == 0 ? pos : KErrNotFound;
}

void CLAPICategoryManager::CompactIfNeededL()
{
    JELOG2(EJavaLocation);

    LOG(EJavaLocation, EInfo, "CLAPICategoryManager::CompactIfNeededL ");

    // Check if the database has been closed
    __ASSERT_ALWAYS(iLandmarkDatabasePtr, User::Leave(KErrSessionClosed));

    CPosLandmarkDatabase::TSize databaseSize = iLandmarkDatabasePtr->SizeL();

    // the minimum compact limit. This prevents that Landmarks Server will
    // not do this operation and lock the database
    if (databaseSize.iUsage < KLAPIMinCompactUsage)
    {
        ExecuteAndDeleteLD(iLandmarkDatabasePtr->CompactL());
    }
}

// ---------------------------------------------------------------------------
// CLAPICategoryManager::HandleCategoryChangeL
// ---------------------------------------------------------------------------
//
void CLAPICategoryManager::HandleCategoryChangeL(TPosLmEventType& aEventType,
        TPosLmItemId& aItemId)
{
    JELOG2(EJavaLocation);
    LOG1(EJavaLocation, EInfo,
         "CLAPICategoryManager::HandleCategoryChangeL - event %d",
         aEventType);
    // Check the type of the event and perform the correct operation
    switch (aEventType)
    {
    case EPosLmEventCategoryUnknownChanges:
    {
        // Unknown changes. This may occur when multiple categories are
        // deleted at once. The whole list needs to be refreshed
        RefreshCategoryListL();
        break;
    }
    case EPosLmEventCategoryCreated: // Fallthrough
    case EPosLmEventCategoryUpdated: // Fallthrough
    case EPosLmEventCategoryDeleted:
    {
        // Update and delete events require deleting the existing category
        if (aEventType != EPosLmEventCategoryCreated)
        {
            TInt index = iCategoryIds.Find(aItemId);
            if (index != KErrNotFound)
            {
                iCategoryIds.Remove(index);
                iCategoryNames->Delete(index);
            }
        }

        // Read the newly added category from the database and add it to the
        // category name and identifier list. Delete event will not add new
        if (aEventType != EPosLmEventCategoryDeleted)
        {
            DoReadCategoryL(aItemId);
        }

        if (aEventType == EPosLmEventCategoryDeleted)
        {
            // Compact the store if needed
            CompactIfNeededL();
        }
        break;
    }
    default:
    {
        // Other events are simply ignored
        break;
    }
    }
}

// End of file
