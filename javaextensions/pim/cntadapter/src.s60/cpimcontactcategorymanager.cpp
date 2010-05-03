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
* Description:  Handles category information
 *
*/


// CLASS HEADER
#include "cpimcontactcategorymanager.h"
#include "fs_methodcall.h"
#include "logger.h"

// EXTERNAL INCLUDES
#include <cntdef.h>
#include <cntdb.h>
#include <cntitem.h>
#include <cntfilt.h>

// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// CPIMContactCategoryManager::NewL
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
CPIMContactCategoryManager* CPIMContactCategoryManager::NewL(
    CContactDatabase& aDatabase,
    java::util::FunctionServer* aFuncServer)
{
    JELOG2(EPim);
    CPIMContactCategoryManager* self =
        new(ELeave) CPIMContactCategoryManager(aDatabase, aFuncServer);
    CleanupStack::PushL(self);
    self->ConstructL();
    CleanupStack::Pop(self);

    return self;
}

// destructor
CPIMContactCategoryManager::~CPIMContactCategoryManager()
{
    JELOG2(EPim);
    delete iCategories;
    delete iIDs;
}

// -----------------------------------------------------------------------------
// CPIMContactCategoryManager::AddToGroupL
// (other items were commented in a header)
// -----------------------------------------------------------------------------
//
void CPIMContactCategoryManager::AddToGroupL(TUint32 aId, TUint32 aGroup)
{
    JELOG2(EPim);
    // Add contact to database group
    iDatabase.AddContactToGroupL(aId, aGroup);
}

// -----------------------------------------------------------------------------
// CPIMContactCategoryManager::AddToGroupL
// (other items were commented in a header)
// -----------------------------------------------------------------------------
//
void CPIMContactCategoryManager::AddToGroupL(TUint32 aId, const TDesC& aGroup)
{
    JELOG2(EPim);
    GetCategoriesL(); // make sure the list is up to date
    TInt pos = 0;
    TInt found = iCategories->Find(aGroup, pos, ECmpNormal);
    if (found == 0)
    {
        AddToGroupL(aId, (*iIDs)[pos]);
    }
}

// -----------------------------------------------------------------------------
// CPIMContactCategoryManager::GroupLabelL
// (other items were commented in a header)
// -----------------------------------------------------------------------------
//
HBufC* CPIMContactCategoryManager::GroupLabelL(TContactItemId aGroupId)
{
    JELOG2(EPim);
    CContactItem* item = iDatabase.ReadContactLC(aGroupId);
    __ASSERT_ALWAYS(item->Type() == KUidContactGroup, User::Leave(KErrNotFound));
    CContactGroup* group = static_cast<CContactGroup*>(item);
    TPtrC groupLabel = group->GetGroupLabelL();
    HBufC* hLabel = groupLabel.AllocL();
    CleanupStack::PopAndDestroy(); // item
    return hLabel;
}

// -----------------------------------------------------------------------------
// CPIMContactCategoryManager::RemoveFromGroupL
// (other items were commented in a header)
// -----------------------------------------------------------------------------
//
void CPIMContactCategoryManager::RemoveFromGroupL(TUint32 aId, TUint32 aGroup)
{
    JELOG2(EPim);
    // Remove contact from specific group
    iDatabase.RemoveContactFromGroupL(aId, aGroup);
}

// -----------------------------------------------------------------------------
// CPIMContactCategoryManager::FlushCache
// Informs the manager to flush the cache at next GetCategories call
// -----------------------------------------------------------------------------
//
void CPIMContactCategoryManager::FlushCache()
{
    JELOG2(EPim);
    iCacheFlushed = ETrue;
}

// -----------------------------------------------------------------------------
// CPIMContactCategoryManager::GetCategoriesL
// Provides all categories currently existing in the native database.
// Returns: Array of categories
// -----------------------------------------------------------------------------
//
const CDesCArray& CPIMContactCategoryManager::CallGetCategoriesL()
{
    JELOG2(EPim);
    CallMethodL(this, &CPIMContactCategoryManager::RefreshCategoriesL,
                iFuncServer);
    return *iCategories;
}

const CDesCArray& CPIMContactCategoryManager::GetCategoriesL()
{
    JELOG2(EPim);
    RefreshCategoriesL();
    return *iCategories;
}

void CPIMContactCategoryManager::RefreshCategoriesL()
{
    JELOG2(EPim);
    if ((iCacheFlushed) || (NULL == iCategories))
    {
        CContactIdArray* groupArray = iDatabase.GetGroupIdListL();
        CleanupStack::PushL(groupArray);
        CCntFilter* filter = CCntFilter::NewLC();
        filter->SetIncludeNewContacts(EFalse);
        filter->SetContactFilterTypeALL(ETrue);
        iDatabase.FilterDatabaseL(*filter);
        CContactIdArray* idArray = filter->iIds;
        TInt idCount = idArray->Count();
        if (NULL == iCategories)
        {
            // We must allocate a new carray
            iCategories = new(ELeave) CDesCArrayFlat(1);
            iIDs = CContactIdArray::NewL();
        }
        else
        {
            // We already have a carray, we just reset it
            iCategories->Reset();
            iIDs->Reset();
        }
        for (TInt i = 0; i < idCount; i++)
        {
            TContactItemId id = (*idArray)[i];
            if (groupArray->Find(id) != KErrNotFound)
            {
                CContactGroup * group =
                    static_cast<CContactGroup*>(iDatabase.ReadContactLC(id));
                if (group->HasItemLabelField())
                {
                    iCategories->AppendL(group->GetGroupLabelL());
                    iIDs->AddL(id);
                }
                CleanupStack::PopAndDestroy(group);
            }
        }
        CleanupStack::PopAndDestroy(2); // filter, groupArray
    }
    iCacheFlushed = EFalse;
    //return *iCategories;
}

// -----------------------------------------------------------------------------
// CPIMContactCategoryManager::IsCategoryL
// Checks whether the given category exists
// Returns: ETrue if the category exists
//          EFalse otherwise
// -----------------------------------------------------------------------------
//
TBool CPIMContactCategoryManager::IsCategoryL(const TDesC& aCategory)
{
    JELOG2(EPim);
    const CDesCArray& categories = GetCategoriesL();
    TInt pos = 0;
    TInt found = categories.Find(aCategory, pos, ECmpNormal);
    return (found == 0);
}

// -----------------------------------------------------------------------------
// CPIMContactCategoryManager::AddCategoryL
// Adds a new category to the native database. If the category already
// exists, nothing is done and the method returns successfully.
// -----------------------------------------------------------------------------
//
void CPIMContactCategoryManager::AddCategoryL(const TDesC& aNewCategory)
{
    JELOG2(EPim);
    if (IsCategoryL(aNewCategory))
    {
        return;
    }
    // Create contact group. The returned group is not needed and it has
    // to be deleted. It is only essential that the group is generated to
    // the native contact database
    CContactItem* group = iDatabase.CreateContactGroupLC(aNewCategory);
    CleanupStack::PopAndDestroy(group);
    FlushCache(); // there has been a change
}

// -----------------------------------------------------------------------------
// CPIMContactCategoryManager::DeleteCategoryL
// Deletes an existing category. If there is no such category, nothing
// is done and the method returns successfully.
// -----------------------------------------------------------------------------
//
void CPIMContactCategoryManager::DeleteCategoryL(const TDesC& aCategory)
{
    JELOG2(EPim);
    GetCategoriesL(); // make sure the id list is refreshed if needed
    TInt pos = 0;
    if (iCategories->Find(aCategory, pos, ECmpNormal) == 0)
    {
        iDatabase.DeleteContactL((*iIDs)[pos]);
    }
    FlushCache(); // categories have changed
}

// -----------------------------------------------------------------------------
// CPIMContactCategoryManager::RenameCategoryL
// Renames an existing category. Entries in the old category are moved
// to the new category.
// -----------------------------------------------------------------------------
//
void CPIMContactCategoryManager::RenameCategoryL(const TDesC& aOldCategory, // the old category name
        const TDesC& aNewCategory) // the new category name
{
    JELOG2(EPim);
    GetCategoriesL(); // make sure the list is refreshed if needed
    TInt pos = 0;
    TInt found = iCategories->Find(aOldCategory, pos, ECmpNormal);
    if (found != 0)
    {
        User::Leave(KErrNotFound);
    }
    TContactItemId oldId = (*iIDs)[pos];
    pos = 0;
    found = iCategories->Find(aNewCategory, pos, ECmpNormal);
    if (found != 0)
    {
        // we can simply rename old category
        CContactGroup* oldGroup =
            static_cast<CContactGroup*>(iDatabase.OpenContactLX(oldId));
        CleanupStack::PushL(oldGroup);
        oldGroup->SetGroupLabelL(aNewCategory);
        iDatabase.CommitContactL(*oldGroup);
        CleanupStack::PopAndDestroy(); // oldGroup
        CleanupStack::Pop(); // oldGroup's lock record
    }
    else
    {
        // we must move the items from old group to the new group
        TContactItemId newId = (*iIDs)[pos];
        // renaming a group to same name should do nothing
        if (newId != oldId)
        {
            CContactGroup * oldGroup =
                static_cast<CContactGroup*>(iDatabase.ReadContactLC(oldId));
            const CContactIdArray* oldItems = oldGroup->ItemsContained();
            TInt idCount = oldItems->Count();
            for (TInt i = 0; i < idCount; i++)
            {
                AddToGroupL((*oldItems)[i], newId);
            }
            iDatabase.DeleteContactL(oldId);
            CleanupStack::PopAndDestroy(); // oldGroup
        }
    }
    FlushCache();
}

// -----------------------------------------------------------------------------
// CPIMContactCategoryManager::LabelFromCacheL
// Fetches a category name from the cache.
// -----------------------------------------------------------------------------
HBufC* CPIMContactCategoryManager::LabelFromCacheL(TContactItemId aCategoryID)
{
    JELOG2(EPim);
    __ASSERT_ALWAYS(iIDs, User::Leave(KErrNotReady));
    TInt index = iIDs->Find(aCategoryID);
    // Leave if category id was not found
    __ASSERT_ALWAYS(index != KErrNotFound, User::Leave(KErrArgument));
    return iCategories->MdcaPoint(index).AllocL();
}

// -----------------------------------------------------------------------------
// CPIMContactCategoryManager::CPIMContactCategoryManager
// C++ default constructor can NOT contain any code, that
// might leave.
// -----------------------------------------------------------------------------
//
inline CPIMContactCategoryManager::CPIMContactCategoryManager(
    CContactDatabase& aDatabase, java::util::FunctionServer* aFuncServer) // contact database
        :
        iDatabase(aDatabase), iFuncServer(aFuncServer)
{
    JELOG2(EPim);
    iCacheFlushed = EFalse;
}

// -----------------------------------------------------------------------------
// CPIMContactCategoryManager::ConstructL
// Symbian 2nd phase constructor can leave.
// -----------------------------------------------------------------------------
//
inline void CPIMContactCategoryManager::ConstructL()
{
    JELOG2(EPim);
    RefreshCategoriesL();
}

// End of file
