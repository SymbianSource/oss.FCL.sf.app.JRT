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
* Description:  Handles PIM contact list <-> Contacts Model conversions
 *
*/


// CLASS HEADER
#include "cpimcontactlistadapter.h"

// INTERNAL INCLUDES
#include "cpimcontactcategorymanager.h"
#include "cpimcontactitemadapter.h"
#include "mpimcontactitem.h"
#include "mpimitemdata.h"
#include "logger.h"

// EXTERNAL INCLUDES
#include <badesca.h>
#include <cntdb.h>
#include <cntitem.h> // CContactGroup
#include <cntfilt.h> // CCntFilter


// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// CPIMContactListAdapter::NewL
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
CPIMContactListAdapter* CPIMContactListAdapter::NewL(
    java::util::FunctionServer* aFuncServer)
{
    JELOG2(EPim);
    CPIMContactListAdapter* self =
        new(ELeave) CPIMContactListAdapter(aFuncServer);

    CallMethodL(self, &CPIMContactListAdapter::ConstructL, self->iFuncServer);

    return self;
}

// Destructor
CPIMContactListAdapter::~CPIMContactListAdapter()
{
    JELOG2(EPim);
    Close();
}
// -----------------------------------------------------------------------------
// CPIMContactListAdapter::HandleDatabaseEventL
// Tests the contact database observer event type and handles it. The ID of
// a contact affected by the change event, if relevant, can be retrieved
// via TContactDbObserverEvent::iContactId.
// -----------------------------------------------------------------------------
//
void CPIMContactListAdapter::HandleDatabaseEventL(
    TContactDbObserverEvent aEvent) // information about the change event
{
    JELOG2(EPim);
    switch (aEvent.iType)
    {
    case EContactDbObserverEventGroupAdded:
    {
        ExternalGroupChangeL(aEvent.iContactId, EPIMExternalChangeNew);
        break;
    }
    case EContactDbObserverEventGroupChanged:
    {
        ExternalGroupChangeL(aEvent.iContactId, EPIMExternalChangeModified);
        break;
    }
    case EContactDbObserverEventGroupDeleted:
    {
        ExternalGroupChangeL(aEvent.iContactId, EPIMExternalChangeRemoved);
        break;
    }
    case EContactDbObserverEventContactAdded:
    {
        ExternalItemChangeL(aEvent.iContactId, EPIMExternalChangeNew);
        break;
    }
    case EContactDbObserverEventContactChanged:
    {
        ExternalItemChangeL(aEvent.iContactId, EPIMExternalChangeModified);
        break;
    }
    case EContactDbObserverEventContactDeleted:
    {
        ExternalItemChangeL(aEvent.iContactId, EPIMExternalChangeRemoved);
        break;
    }
    default:
    {
        // we don't care about other changes
        return;
    }
    }
}


// -----------------------------------------------------------------------------
// CPIMContactListAdapter::GetCategoriesL
// Provides all categories currently existing in the native database.
// Returns: Array of categories
// -----------------------------------------------------------------------------
//
const CDesCArray& CPIMContactListAdapter::GetCategoriesL()
{
    JELOG2(EPim);
    __ASSERT_ALWAYS(iCategoryManager, User::Leave(KErrNotReady));
    return iCategoryManager->CallGetCategoriesL();
}

// -----------------------------------------------------------------------------
// CPIMContactListAdapter::AddCategoryL
// Adds a new category to the native database.
// If the category already  exists, nothing is done and the method returns
// successfully.
// -----------------------------------------------------------------------------
//
void CPIMContactListAdapter::AddCategoryL(const TDesC& aNewCategory)
{
    JELOG2(EPim);
    __ASSERT_ALWAYS(iCategoryManager, User::Leave(KErrNotReady));
    CallMethodL(iCategoryManager, &CPIMContactCategoryManager::AddCategoryL,
                aNewCategory, iFuncServer);
}

// -----------------------------------------------------------------------------
// CPIMContactListAdapter::DeleteCategoryL
// Deletes an existing category.
// If there is no such category, nothing is done and the method returns
// successfully.
// -----------------------------------------------------------------------------
//
void CPIMContactListAdapter::DeleteCategoryL(const TDesC& aCategory)
{
    JELOG2(EPim);
    __ASSERT_ALWAYS(iCategoryManager, User::Leave(KErrNotReady));
    CallMethodL(iCategoryManager, &CPIMContactCategoryManager::DeleteCategoryL,
                aCategory, iFuncServer);
}

// -----------------------------------------------------------------------------
// CPIMContactListAdapter::RenameCategoryL
// Renames an existing category.
// Entries in the old category are moved to the new category.
// -----------------------------------------------------------------------------
//
void CPIMContactListAdapter::RenameCategoryL(const TDesC& aOldCategory, // The old category name
        const TDesC& aNewCategory) // The new category name
{
    JELOG2(EPim);
    __ASSERT_ALWAYS(iCategoryManager, User::Leave(KErrNotReady));
    CallMethodL(iCategoryManager, &CPIMContactCategoryManager::RenameCategoryL,
                aOldCategory, aNewCategory, iFuncServer);
}

// -----------------------------------------------------------------------------
// CPIMContactListAdapter::IsCategoriesExternallyModified
// Checks whether there have been external changes to the categories in
// the native database after last call to
// GetExternalCategoryModifications or list adapter creation.
// Returns: ETrue: If there are any external changes
//          EFalse: Otherwise
// -----------------------------------------------------------------------------
//
TBool CPIMContactListAdapter::IsCategoriesExternallyModified()
{
    JELOG2(EPim);
    if (iCategoryChanges)
    {
        return ETrue;
    }
    return EFalse;
}

// -----------------------------------------------------------------------------
// CPIMContactListAdapter::GetExternalCategoryModificationsL
// Provides the external changes to the categories in the native database.
// Returns: An array of category state change objects. The ownership
//          of the array is transferred to the caller. Note that the
//          array elements contain heap-allocated data.
// -----------------------------------------------------------------------------
//
RPointerArray<CPIMCategoryStateChange>*
CPIMContactListAdapter::GetExternalCategoryModificationsL()
{
    JELOG2(EPim);
    CallMethodL(this, &CPIMContactListAdapter::IsDatabaseReadyL, iFuncServer);
    RPointerArray<CPIMCategoryStateChange>* retval = iCategoryChanges;
    iCategoryChanges = NULL;
    return retval;
}

// -----------------------------------------------------------------------------
// CPIMContactListAdapter::IsItemsExternallyModified
// Checks whether there have been external changes to the items in
// the native database after last call to
// GetExternalItemModifications or list adapter creation.
// Returns: EExternalChangesMinor: If there are any external changes
//          EExternalChangesNone: Otherwise
// -----------------------------------------------------------------------------
//
MPIMListAdapter::TExternalItemChangeClass CPIMContactListAdapter::IsItemsExternallyModified()
{
    JELOG2(EPim);
    if ((iItemChanges || iFirstItemChanges))
    {
        return MPIMListAdapter::EExternalChangesMinor;
    }
    return MPIMListAdapter::EExternalChangesNone;
}

// -----------------------------------------------------------------------------
// CPIMContactListAdapter::GetExternalItemModificationsL
// Provides the external changes to the items in the native database.
// Returns: An array of item state change objects. The ownership
//          of the array is transferred to the caller. Note that the
//          array elements contain heap-allocated data.
// -----------------------------------------------------------------------------
//
RPointerArray<CPIMItemStateChange>*
CPIMContactListAdapter::GetExternalItemModificationsL()
{
    JELOG2(EPim);
    CallMethodL(this, &CPIMContactListAdapter::IsDatabaseReadyL, iFuncServer);

    // the first time,
    // we check for all cards
    if (iFirstItemChanges)
    {
        //FilterAllContactsL();
        CallMethodL(this, &CPIMContactListAdapter::FilterAllContactsL,
                    iFuncServer);
        iFirstItemChanges = EFalse;
    }

    RPointerArray<CPIMItemStateChange>* retval = iItemChanges;
    iItemChanges = NULL;
    return retval;
}

// -----------------------------------------------------------------------------
// CPIMContactListAdapter::Close
// Used to inform the list adapter that the list has been closed.
// The list adapter may then release all resources it has reserved.
// -----------------------------------------------------------------------------
//
void CPIMContactListAdapter::Close()
{
    JELOG2(EPim);
    CallMethod(this, &CPIMContactListAdapter::DoClose, iFuncServer);
}

void CPIMContactListAdapter::DoClose()
{
    JELOG2(EPim);
    if (iCategoryChanges)
    {
        iCategoryChanges->ResetAndDestroy();
        delete iCategoryChanges;
        iCategoryChanges = NULL;
    }
    if (iItemChanges)
    {
        iItemChanges->ResetAndDestroy();
        delete iItemChanges;
        iItemChanges = NULL;
    }
    delete iNotifier;
    iNotifier = NULL;
    delete iItemAdapter;
    iItemAdapter = NULL;
    delete iCategoryManager;
    iCategoryManager = NULL;
    delete iDatabase;
    iDatabase = NULL;
    delete iMinimalFieldsViewDef;
    iMinimalFieldsViewDef = NULL;
}

// -----------------------------------------------------------------------------
// CPIMContactListAdapter::GetPimListAdapter
// Provides access to the MPIMListAdapter representation of this
// MPIMContactListAdapter object.
// -----------------------------------------------------------------------------
//
MPIMListAdapter* CPIMContactListAdapter::GetPimListAdapter()
{
    JELOG2(EPim);
    return this;
}

// -----------------------------------------------------------------------------
// CPIMContactListAdapter::CreateContactItemL
// Creates a new contact item (entry) in the native database.
// The adapter creates a new native database entry, sets its data
// according to the data in aContactItem, adds it to the database
// and sets the Item ID of aContactItem. aContactItem must
// contain valid data and have Item ID KPIMNullItemID.
// -----------------------------------------------------------------------------
//
void CPIMContactListAdapter::CreateContactItemL(MPIMContactItem& aContactItem) // The contact item to add
{
    JELOG2(EPim);
    CallMethodL(this, &CPIMContactListAdapter::DoCreateContactItemL,
                aContactItem, iFuncServer);
    ReadContactItemL(aContactItem);
}

void CPIMContactListAdapter::DoCreateContactItemL(MPIMContactItem& aContactItem) // The contact item to add
{
    JELOG2(EPim);
    __ASSERT_ALWAYS(iItemAdapter || iDatabase, User::Leave(KErrNotReady));
    CContactCard* card = iItemAdapter->CardL(aContactItem);
    CleanupStack::PushL(card);
    // Leaves with KErrDiskFull if there is not enough disk
    // space to create a new contact item
    TContactItemId id = iDatabase->AddNewContactL(*card);
    // Set new item id. If this leaves it means that
    // SetContactItemIdL leaved with KErrOutNoMemory
    TRAPD(err, aContactItem.SetContactItemIdL(id));
    if (KErrNone != err)
    {
        TBuf8<KPIMItemIdDesSize> entryId;
        entryId.Num(static_cast<TUint>(id));
        DoRemoveContactItemL(entryId);
        User::Leave(err);
    }
    iItemAdapter->UpdateCategoriesL(aContactItem, *card);
    CleanupStack::PopAndDestroy(card);
    // update possible changes (e.g. dropped attributes) to the item
    aContactItem.PrepareForLoadL();
}

// -----------------------------------------------------------------------------
// CPIMContactListAdapter::ReadContactItemL
// Reads an existing contact item from the native database.
// The adapter maps the Item ID in aContactItem to a native database
// entry identifier, reads the entry and sets the data of
// aContactItem according to the data in the native entry. This operation
// is very performance sensitive, so it must be used with cause and only
// when the full contact item is needed
// -----------------------------------------------------------------------------
//
void CPIMContactListAdapter::ReadContactItemL(MPIMContactItem& aContactItem) // The contact item to be read
{
    JELOG2(EPim);
    CallMethodL(this, &CPIMContactListAdapter::DoCallReadContactItemL,
                aContactItem, iFuncServer);
}

void CPIMContactListAdapter::DoCallReadContactItemL(
    MPIMContactItem& aContactItem) // The contact item to be read
{
    JELOG2(EPim);
    __ASSERT_ALWAYS(iItemAdapter || iDatabase, User::Leave(KErrNotReady));
    const TContactItemId id = aContactItem.ContactItemIdL();
    __ASSERT_ALWAYS(id != 0, User::Leave(KErrArgument));
    // If the contact item contains data we don't want to overwrite it
    // so we create a mask item view definition if needed. Otherwise
    // we just read full contact item from the database
    CContactItemViewDef* itemViewDef = CContactItemViewDef::NewLC(
                                           CContactItemViewDef::EMaskFields,
                                           CContactItemViewDef::EIncludeHiddenFields);
    CArrayFix<TPIMField>* fields = aContactItem.ItemData().FieldsLC();
    TInt count = fields->Count();

    // Mask fields and do not include. Note that if there is no fields in the
    // item the view should provide all possible fields from the database
    for (TInt i = 0; i < count; i++)
    {
        TPIMContactField field = static_cast<TPIMContactField>(fields->At(i));
        // Get Contacts Model fields mapped to PIM fields
        CArrayFix<TInt>* fieldArray = iItemAdapter->ContactsModelFieldTypeL(
                                          field);
        CleanupStack::PushL(fieldArray);
        // Add retrieved fields to the item view definition
        TInt fieldCount = fieldArray->Count();
        for (TInt j = 0; j < fieldCount; j++)
        {
            const TUid fieldUid =
                { fieldArray->At(j) };
            // Do not add the field to the view if it already contains
            // this type of a field (e.g the field is a name array element)
            if (itemViewDef->Find(fieldUid) == KErrNotFound)
            {
                itemViewDef->AddL(fieldUid);
            }
        }
        CleanupStack::PopAndDestroy(fieldArray);
    }
    DoReadContactItemL(aContactItem, *itemViewDef);
    CleanupStack::PopAndDestroy(2, itemViewDef);
}

// -----------------------------------------------------------------------------
// CPIMContactListAdapter::ReadMinimalContactItemL
// This version reads a minimal contact item from the native database by using
// contact database view definition. The view definition is initialized in
// the constructor of this class and needs to be up to date with currently
// required contact fields when the item is read for the first time. This
// function was introduced due to improve poor performance of the PIM API
// ContactList
// -----------------------------------------------------------------------------

void CPIMContactListAdapter::ReadMinimalContactItemL(
    MPIMContactItem& aContactItem)
{
    JELOG2(EPim);
    CallMethodL(this, &CPIMContactListAdapter::DoCallReadMinimalContactItemL,
                aContactItem, iFuncServer);
}

void CPIMContactListAdapter::DoCallReadMinimalContactItemL(
    MPIMContactItem& aContactItem)
{
    JELOG2(EPim);
    __ASSERT_ALWAYS(iItemAdapter || iDatabase, User::Leave(KErrNotReady));
    TContactItemId id = aContactItem.ContactItemIdL();
    __ASSERT_ALWAYS(id != 0, User::Leave(KErrArgument));

    // Reset item for reading
    aContactItem.PrepareForLoadL();

    // Use contact database filtering. Currently only all name elements
    // are retrieved from the database to improve performance of the list
    DoReadContactItemL(aContactItem, *iMinimalFieldsViewDef);

}

// -----------------------------------------------------------------------------
// CPIMContactListAdapter::ReadMinimalContactItemL
// This version reads a minimal contact item from the native database by using
// contact database view definition. The view definition is initialized in
// the constructor of this class and it can be modified to fetch specified fields
// by the matching item given as an argument to this function
// -----------------------------------------------------------------------------

void CPIMContactListAdapter::ReadMinimalContactItemL(
    MPIMContactItem& aContactItem, const MPIMContactItem& aMatchingContactItem)
{
    JELOG2(EPim);
    // Get fields from the matching item and add those to the view definition
    CArrayFix<TPIMField>* fields = aMatchingContactItem.ItemData().FieldsLC();
    TInt count = fields->Count();
    for (TInt i = 0; i < count; i++)
    {
        TPIMContactField field = static_cast<TPIMContactField>(fields->At(i));
        // Get Contacts Model fields mapped to PIM fields
        CArrayFix<TInt>* fieldArray = iItemAdapter->ContactsModelFieldTypeL(
                                          field);
        CleanupStack::PushL(fieldArray);
        // Add retrieved fields to the item view definition
        TInt fieldCount = fieldArray->Count();
        for (TInt j = 0; j < fieldCount; j++)
        {
            const TUid fieldUid =
                { fieldArray->At(j) };
            // Do not add the field to the view if it already contains
            // this type of a field (e.g the field is a name array element)
            if (iMinimalFieldsViewDef->Find(fieldUid) == KErrNotFound)
            {
                iMinimalFieldsViewDef->AddL(fieldUid);
            }
        }
        CleanupStack::PopAndDestroy(fieldArray);
    }
    CleanupStack::PopAndDestroy(fields);
    // Read contact item using minimal field definition
    ReadMinimalContactItemL(aContactItem);
    // Reset the view definition to its previous state
    InitializeMinimalViewDefinitionL();
}

// -----------------------------------------------------------------------------
// CPIMContactListAdapter::ReadContactFieldL
// Reads one field from the contact database
// -----------------------------------------------------------------------------
//
void CPIMContactListAdapter::ReadContactFieldL(MPIMContactItem& aContactItem,
        TPIMContactField aContactField)
{
    JELOG2(EPim);
    CallMethodL(this, &CPIMContactListAdapter::DoReadContactFieldL,
                aContactItem, aContactField, iFuncServer);
}

void CPIMContactListAdapter::DoReadContactFieldL(MPIMContactItem& aContactItem,
        TPIMContactField aContactField)
{
    JELOG2(EPim);
    __ASSERT_ALWAYS(iItemAdapter || iDatabase, User::Leave(KErrNotReady));
    TContactItemId id = aContactItem.ContactItemIdL();
    __ASSERT_ALWAYS(id != 0, User::Leave(KErrArgument));

    // Create new view definition
    CContactItemViewDef* itemViewDef = CContactItemViewDef::NewLC(
                                           CContactItemViewDef::EIncludeFields,
                                           CContactItemViewDef::EIncludeHiddenFields);
    CArrayFix<TInt>* fieldArray = iItemAdapter->ContactsModelFieldTypeL(
                                      aContactField);
    CleanupStack::PushL(fieldArray);
    // Add retrieved fields to the item view definition
    TInt fieldCount = fieldArray->Count();
    for (TInt i = 0; i < fieldCount; i++)
    {
        const TUid fieldUid =
            { fieldArray->At(i) };
        itemViewDef->AddL(fieldUid);
    }
    // The view cannot be empty so check the field count in debug builds
    __ASSERT_DEBUG(itemViewDef->Count() > 0, User::Panic(KPIMPanicCategory,
                   EPIMPanicInvalidState));
    CleanupStack::PopAndDestroy(fieldArray);
    DoReadContactItemL(aContactItem, *itemViewDef);
    CleanupStack::PopAndDestroy(itemViewDef);
}

// -----------------------------------------------------------------------------
// CPIMContactListAdapter::WriteContactItemL
// Writes an existing contact item to the native database.
// The adapter maps the Item ID in aContactItem to a native database
// entry identifier, reads the entry and sets the data of the entry
// according to the data in aContactItem.
// -----------------------------------------------------------------------------
//
void CPIMContactListAdapter::WriteContactItemL(MPIMContactItem& aContactItem) // The contact item to write
{
    JELOG2(EPim);
    CallMethodL(this, &CPIMContactListAdapter::DoWriteContactItemL,
                aContactItem, iFuncServer);
    ReadContactItemL(aContactItem);
}

void CPIMContactListAdapter::DoWriteContactItemL(MPIMContactItem& aContactItem) // The contact item to write
{
    JELOG2(EPim);
    __ASSERT_ALWAYS(iItemAdapter || iDatabase, User::Leave(KErrNotReady));
    TContactItemId id = aContactItem.ContactItemIdL();
    __ASSERT_ALWAYS(id != 0, User::Leave(KErrArgument));

    // OpenContactLX leaves the lock item in the cleanup stack
    CContactCard* contactItem =
        static_cast<CContactCard*>(iDatabase->OpenContactLX(id));
    CleanupStack::PushL(contactItem);
    iItemAdapter->UpdateCardL(aContactItem, *contactItem);
    iDatabase->CommitContactL(*contactItem);
    // Note: Even though the above line closed the contact, we can still
    // keep the lock item in the cleanup stack as closing a contact
    // twice is harmless
    iItemAdapter->UpdateCategoriesL(aContactItem, *contactItem);
    CleanupStack::PopAndDestroy(contactItem);
    CleanupStack::Pop(); // contactItem lock item
    // update possible changes (e.g. dropped attributes) to the item
    aContactItem.PrepareForLoadL();
}

// -----------------------------------------------------------------------------
// CPIMContactListAdapter::ReadContactItemL
// Removes an existing contact from the native database.
// The adapter maps aItemID to a native database entry and removes it.
// -----------------------------------------------------------------------------
//
void CPIMContactListAdapter::RemoveContactItemL(TPIMItemID aItemID)
{
    JELOG2(EPim);
    CallMethodL(this, &CPIMContactListAdapter::DoRemoveContactItemL, aItemID,
                iFuncServer);
}

void CPIMContactListAdapter::DoRemoveContactItemL(TPIMItemID aItemID)
{
    JELOG2(EPim);
    __ASSERT_ALWAYS(iDatabase, User::Leave(KErrNotReady));
    // Convert string id to integer id
    TUint id;
    TLex8 lex(aItemID);
    TInt status = lex.Val(id);
    User::LeaveIfError(status);
    // Delete contact from the contact database
    iDatabase->DeleteContactL(id);
}

// -----------------------------------------------------------------------------
// CPIMContactListAdapter::CPIMContactListAdapter
// C++ default constructor can NOT contain any code, that
// might leave.
// -----------------------------------------------------------------------------
//
CPIMContactListAdapter::CPIMContactListAdapter(
    java::util::FunctionServer* aFuncServer)
{
    JELOG2(EPim);
    iFirstItemChanges = ETrue;
    iFuncServer = aFuncServer;
}

// -----------------------------------------------------------------------------
// CPIMContactListAdapter::ConstructL
// Symbian 2nd phase constructor can leave.
// -----------------------------------------------------------------------------
//
void CPIMContactListAdapter::ConstructL()
{
    JELOG2(EPim);
    // Create default contact database if it does not exist. This may happen
    // if the current Phonebook engine is not running or started during
    // device boot. Usually, the database should be available but this is precaution
    if (!CContactDatabase::DefaultContactDatabaseExistsL())
    {
        iDatabase
        = CContactDatabase::CreateL(/*CContactDatabase::EMultiThread*/);
    }
    else
    {
        iDatabase = CContactDatabase::OpenL(/*CContactDatabase::EMultiThread*/);
    }

    // Create other contact adapter resources and initialize item view
    iCategoryManager
    = CPIMContactCategoryManager::NewL(*iDatabase, iFuncServer);
    iNotifier = CContactChangeNotifier::NewL(*iDatabase, this);
    iItemAdapter = CPIMContactItemAdapter::NewL(*iCategoryManager);
    // Initialize minimal fields view definition
    iMinimalFieldsViewDef = CContactItemViewDef::NewL(
                                CContactItemViewDef::EIncludeFields,
                                CContactItemViewDef::EIncludeHiddenFields);
    InitializeMinimalViewDefinitionL();
}

// -----------------------------------------------------------------------------
// CPIMContactListAdapter::ExternalGroupChangeL
// Adds an entry to external category change list.
// Also causes the category cache to be flushed.
// -----------------------------------------------------------------------------
//
void CPIMContactListAdapter::ExternalGroupChangeL(TContactItemId aId, // Id of the changed category
        TPIMExternalChangeType aType) // type of the change
{
    JELOG2(EPim);
    iCategoryManager->FlushCache();

    // This should never happen
    __ASSERT_ALWAYS(iCategoryManager, User::Leave(KErrNotReady));

    if (!iCategoryChanges)
    {
        iCategoryChanges
        = new(ELeave) RPointerArray<CPIMCategoryStateChange> (1);
    }
    CPIMCategoryStateChange* change = NULL;
    switch (aType)
    {
    case EPIMExternalChangeModified:
    {
        HBufC* newCategoryName = iCategoryManager->GroupLabelL(aId);
        CleanupStack::PushL(newCategoryName);
        HBufC* oldCategoryName = iCategoryManager->LabelFromCacheL(aId);
        CleanupStack::PushL(oldCategoryName);
        change
        = new(ELeave) CPIMCategoryStateChange(oldCategoryName, aType, newCategoryName);
        CleanupStack::Pop(2); // newCategoryName and oldCategoryName are
        // now owned by change
        break;
    }
    case EPIMExternalChangeNew:
    {
        HBufC* newCategoryName = iCategoryManager->GroupLabelL(aId);
        CleanupStack::PushL(newCategoryName);
        change = new(ELeave) CPIMCategoryStateChange(newCategoryName, aType);
        CleanupStack::Pop(); // newCategoryName is now owned by change
        break;
    }
    case EPIMExternalChangeRemoved:
    {
        HBufC* oldCategoryName = iCategoryManager->LabelFromCacheL(aId);
        CleanupStack::PushL(oldCategoryName);
        change = new(ELeave) CPIMCategoryStateChange(oldCategoryName, aType);
        CleanupStack::Pop(); // newCategoryName is now owned by change
        break;
    }
    default:
    {
        User::Leave(KErrArgument);
    }
    }

    CleanupStack::PushL(change);
    User::LeaveIfError(iCategoryChanges->Append(change));
    CleanupStack::Pop(change);
}

// -----------------------------------------------------------------------------
// CPIMContactListAdapter::ExternalItemChangeL
// Adds an entry to external item change list.
// -----------------------------------------------------------------------------
//
void CPIMContactListAdapter::ExternalItemChangeL(TContactItemId aId, // Id of the changed category
        TPIMExternalChangeType aType) // type of the change
{
    JELOG2(EPim);
    if (!iItemChanges)
    {
        iItemChanges = new(ELeave) RPointerArray<CPIMItemStateChange> (10);
    }
    HBufC8* changeId = HBufC8::NewLC(KPIMItemIdDesSize);
    TPtr8 entryId = changeId->Des();
    entryId.Num(static_cast<TUint>(aId));
    CPIMItemStateChange* change =
        new(ELeave) CPIMItemStateChange(changeId, aType);
    // The ownership of changeId is transferred to the item state change object
    CleanupStack::Pop(changeId);

    CleanupStack::PushL(change);
    iItemChanges->AppendL(change);
    CleanupStack::Pop(change);
}

// -----------------------------------------------------------------------------
// CPIMContactListAdapter::FilterAllContacts
// Filters all contact items from the database to the changes list
// -----------------------------------------------------------------------------
//
void CPIMContactListAdapter::FilterAllContactsL()
{
    JELOG2(EPim);
    CCntFilter* filter = CCntFilter::NewLC();
    filter->SetIncludeNewContacts(EFalse);
    if (!iItemChanges)
    {
        iItemChanges = new(ELeave) RPointerArray<CPIMItemStateChange> (10);
    }
    filter->SetContactFilterTypeALL(EFalse);
    filter->SetContactFilterTypeCard(ETrue);
    iDatabase->FilterDatabaseL(*filter);
    CContactIdArray* idArray = filter->iIds;
    TInt idCount = idArray->Count();
    TInt i = 0;
    for (i = 0; i < idCount; i++)
    {
        TContactItemId id = (*idArray)[i];
        TBool wasInList = EFalse;
        TInt changeCount = iItemChanges->Count();
        for (TInt j = 0; j < changeCount; j++)
        {
            TBuf8<KPIMItemIdDesSize> entryId;
            entryId.Num(static_cast<TUint>(id));
            if ((*iItemChanges)[j]->ItemID().Compare(entryId) == 0)
            {
                wasInList = ETrue;
            }
        }
        if (!wasInList)
        {
            ExternalItemChangeL(id, EPIMExternalChangeNew);
        }
    }
    CleanupStack::PopAndDestroy(); // filter
}

// -----------------------------------------------------------------------------
// CPIMContactListAdapter::InitializeMinimalViewDefinitionL
// Initializes minimal view definition
// -----------------------------------------------------------------------------
//
void CPIMContactListAdapter::InitializeMinimalViewDefinitionL()
{
    JELOG2(EPim);
    __ASSERT_DEBUG(iMinimalFieldsViewDef, User::Panic(KPIMPanicCategory,
                   EPIMPanicInvalidState));
    // Reset view definition
    iMinimalFieldsViewDef->Reset();
    // Add fields which are to retrieved from the native database
    // when minimal item is to be created or read
    iMinimalFieldsViewDef->AddL(KUidContactFieldAdditionalName);
    iMinimalFieldsViewDef->AddL(KUidContactFieldGivenName);
    iMinimalFieldsViewDef->AddL(KUidContactFieldFamilyName);
    iMinimalFieldsViewDef->AddL(KUidContactFieldGivenNamePronunciation);
    iMinimalFieldsViewDef->AddL(KUidContactFieldFamilyNamePronunciation);
    iMinimalFieldsViewDef->AddL(KUidContactFieldPrefixName);
    iMinimalFieldsViewDef->AddL(KUidContactFieldSuffixName);
}

// -----------------------------------------------------------------------------
// CPIMContactListAdapter::DoReadContactItemL
// Reads contact from the contacts database using item view definition
// -----------------------------------------------------------------------------
//
void CPIMContactListAdapter::DoReadContactItemL(MPIMContactItem& aContactItem,
        const CContactItemViewDef& aContactItemViewDef)
{
    JELOG2(EPim);
    const TContactItemId id = aContactItem.ContactItemIdL();
    __ASSERT_DEBUG(id != 0, User::Panic(KPIMPanicCategory,
                                        EPIMPanicInvalidItemID));
    // Read contact item using the item view definition
    CContactCard* contactItem =
        static_cast<CContactCard*>(iDatabase->ReadContactLC(id,aContactItemViewDef));
    // Set date and fill the PIM item
    TTime lastModified = contactItem->LastModified();
    aContactItem.SetLastModifiedL(lastModified);
    iItemAdapter->FillItemL(aContactItem, *contactItem);
    CleanupStack::PopAndDestroy(contactItem);
}

void CPIMContactListAdapter::IsDatabaseReadyL()
{
    JELOG2(EPim);
    __ASSERT_ALWAYS(iDatabase, User::Leave(KErrNotReady));
}

// End of file
