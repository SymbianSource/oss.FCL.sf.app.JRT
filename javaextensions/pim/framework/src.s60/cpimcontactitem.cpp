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
* Description:  Contact item implementation.
 *
*/


// CLASS HEADER
#include "cpimcontactitem.h"

// INTERNAL INCLUDES
#include "cpimitemdata.h"
#include "pimcontact.h"
#include "mpimcontactadaptermanager.h"
#include "mpimcontactlistadapter.h"
#include "mpimadaptermanager.h"
#include "mpimlistadapter.h"
#include "cpimcontactvalidator.h"
#include "cpimmanager.h"
#include "pimpanics.h"
#include "logger.h"

// ---------------------------------------------------------------------------
// C++ constructor
// ---------------------------------------------------------------------------
//
CPIMContactItem::CPIMContactItem(const CPIMContactValidator& aContactValidator) :
        CPIMItem(aContactValidator)
{
    JELOG2(EPim);
}

// ---------------------------------------------------------------------------
// CPIMContactItem::NewL
// Default two-phase constructor
// ---------------------------------------------------------------------------
//
CPIMContactItem* CPIMContactItem::NewL(
    const CPIMContactValidator& aContactValidator)
{
    JELOG2(EPim);
    CPIMContactItem* self = CPIMContactItem::NewLC(aContactValidator);
    CleanupStack::Pop(self);

    return self;
}

// ---------------------------------------------------------------------------
// CPIMContactItem::NewLC
// Default two-phase constructor. Leaves the item to the cleanup stack
// ---------------------------------------------------------------------------
//
CPIMContactItem* CPIMContactItem::NewLC(
    const CPIMContactValidator& aContactValidator)
{
    JELOG2(EPim);
    CPIMContactItem* self = new(ELeave) CPIMContactItem(aContactValidator);

    CleanupStack::PushL(self);
    self->ConstructL();

    return self;
}

pimbaseitem* pimbaseitem::getContactItemInstance(pimbasemanager* aPimManager)
{
    JELOG2(EPim);
    CPIMManager* pimManager = reinterpret_cast<CPIMManager*>(aPimManager);
    CPIMContactItem* contactItem = NULL;
    TInt error = 0;
    TRAP(error,
    {
        const CPIMContactValidator& contactValidator =
        pimManager->ContactValidator();
        contactItem = CPIMContactItem::NewL(contactValidator);
    }
        );
    if (error != KErrNone)
        throw KErrGeneral;
    return contactItem;
}

// ---------------------------------------------------------------------------
// Destructor
// ---------------------------------------------------------------------------
//
CPIMContactItem::~CPIMContactItem()
{
    JELOG2(EPim);
}

// ---------------------------------------------------------------------------
// CPIMContactItem::SetContactAdapterAssociation
// (other items were commented in a header)
// ---------------------------------------------------------------------------
//
void CPIMContactItem::SetContactAdapterAssociation(
    MPIMContactAdapterManager* aContactAdapterManager,
    MPIMContactListAdapter* aContactListAdapter)
{
    JELOG2(EPim);
    __ASSERT_DEBUG(aContactAdapterManager, User::Panic(KPIMPanicCategory,
                   EPIMPanicNullArgument));

    iContactAdapterManager = aContactAdapterManager;
    iContactListAdapter = aContactListAdapter;

    SetBaseAdapterAssociation(iContactAdapterManager->GetAdapterManager(),
                              iContactListAdapter ? iContactListAdapter->GetPimListAdapter(): NULL);
}

// ---------------------------------------------------------------------------
// CPIMContactItem::RemoveAdapterAssociation
// (other items were commented in a header)
// ---------------------------------------------------------------------------
//
void CPIMContactItem::RemoveAdapterAssociation()
{
    JELOG2(EPim);
    iContactAdapterManager = NULL;
    iContactListAdapter = NULL;

    CPIMItem::RemoveAdapterAssociation();
}

// ---------------------------------------------------------------------------
// CPIMContactItem::ItemType
// (other items were commented in a header)
// ---------------------------------------------------------------------------
//
TPIMListType CPIMContactItem::ItemType() const
{
    JELOG2(EPim);
    return EPIMContactList;
}

// ---------------------------------------------------------------------------
// CPIMContactItem::commit
// (other items were commented in a header)
// ---------------------------------------------------------------------------
//
void CPIMContactItem::commit()
{
    JELOG2(EPim);
    TInt error = KErrNone;
    TRAP(error,
    {
        if (iContactAdapterManager)
        {
            // The item is associated with a list
            if (iContactListAdapter)
            {
                // The list is open
                if (iItemID->Compare(KPIMNullItemID) == 0)
                {
                    // The item does not have database entry
                    iContactListAdapter->CreateContactItemL(*this);
                }
                else
                {
                    // Make sure that the item is fully loaded from the native
                    // database before committing the changes to it. Note that
                    // already modified fields must not be overwritten by the
                    // responsible list adapter
                    CPIMItem::LoadFullItemL();
                    iContactListAdapter->WriteContactItemL(*this);
                }

                SetModified(EFalse);
                UpdateUidFieldL(EPIMContactUid, iItemID->Des());   // codescanner::leave
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
// CPIMContactItem::ListClosed
// (other items were commented in a header)
// ---------------------------------------------------------------------------
//
void CPIMContactItem::ListClosed()
{
    JELOG2(EPim);
    iContactListAdapter = NULL;
    CPIMItem::ListClosed();
}

// ---------------------------------------------------------------------------
// CPIMContactItem::SetPreferredIndexStringL
// (other items were commented in a header)
// ---------------------------------------------------------------------------
//
void CPIMContactItem::SetPreferredIndexStringL(const TPIMField& aField,
        const TInt& aIndex, TPIMAttribute aAttribute)
{
    JELOG2(EPim);
    const TInt numValues = iItemData->CountValues(aField);

    // Clear "preferred index" attribute from all values of the field
    for (TInt i = 0; i < numValues; i++)
    {
        TPIMAttribute attributes = iItemData->AttributesL(aField, i);
        // Clear attributes
        attributes &= ~aAttribute;
        iItemData->SetAttributesL(aField, i, attributes);
    }

    // Set "preferred index" attribute to the given value of the field
    TPIMAttribute preferredIndexAttributes = iItemData->AttributesL(
                aField, aIndex);
    // Add preferred index attribute to this attribute set
    preferredIndexAttributes |= aAttribute;
    iItemData->SetAttributesL(aField, aIndex, preferredIndexAttributes);
}

// ---------------------------------------------------------------------------
// CPIMContactItem::AddStringL
// (other items were commented in a header)
// ---------------------------------------------------------------------------
//
void CPIMContactItem::AddStringL(TPIMField aField,
                                 TPIMAttribute aAttributes, HBufC* aValue)
{
    JELOG2(EPim);
    CPIMItem::AddStringL(aField, aAttributes, aValue);

    // If we got this far without leaving, the last value in the field is the
    // new one. Let's see if it has the preferred index set, and clear the
    // preferred index from any other values.

    const TInt lastValueIndex = iItemData->CountValues(aField) - 1;
    TPIMAttribute lastValueAttributes = iItemData->AttributesL(aField,
                                        lastValueIndex);

    // Set preferred attribute
    if (lastValueAttributes & EPIMContactAttrPreferred)
    {
        SetPreferredIndexStringL(aField, lastValueIndex,
                                 EPIMContactAttrPreferred);
    }
    // Set preferred SMS attribute
    if (lastValueAttributes & EPIMContactAttrSms)
    {
        SetPreferredIndexStringL(aField, lastValueIndex,
                                 EPIMContactAttrSms);
    }
}

// ---------------------------------------------------------------------------
// CPIMContactItem::SetStringL
// (other items were commented in a header)
// ---------------------------------------------------------------------------
//
void CPIMContactItem::SetStringL(TPIMField aField, TInt aIndex,
                                 TPIMAttribute aAttributes, HBufC* aValue)
{
    JELOG2(EPim);
    CPIMItem::SetStringL(aField, aIndex, aAttributes, aValue);

    // If we got this far without leaving, the freshly set value in the field
    // might have preferred index set. If so, we clear the preferred index
    // from any other values.

    TPIMAttribute realValueAttributes = iItemData->AttributesL(aField,
                                        aIndex);

    // Set preferred attribute
    if (realValueAttributes & EPIMContactAttrPreferred)
    {
        SetPreferredIndexStringL(aField, aIndex,
                                 EPIMContactAttrPreferred);
    }
    // Set preferred SMS attribute
    if (realValueAttributes & EPIMContactAttrSms)
    {
        SetPreferredIndexStringL(aField, aIndex, EPIMContactAttrSms);
    }
}

// ---------------------------------------------------------------------------
// CPIMContactItem::SetContactItemIdL
// (other items were commented in a header)
// ---------------------------------------------------------------------------
//
void CPIMContactItem::SetContactItemIdL(
    const TPIMItemID& aContactItemId)
{
    JELOG2(EPim);
    // Set item id for the base class
    CPIMItem::SetIdL(aContactItemId);

    // Update UID field if it is supported by this contact item
    if (iAdapterManager && iAdapterManager->IsSupportedField(
                EPIMContactUid))
    {
        UpdateUidFieldL(EPIMContactUid, aContactItemId);
    }
}

// ---------------------------------------------------------------------------
// CPIMContactItem::SetContactItemIdL
// (other items were commented in a header)
// ---------------------------------------------------------------------------
//
void CPIMContactItem::SetContactItemIdL(const TUint aContactItemId)
{
    JELOG2(EPim);
    TBuf8<KPIMItemIdDesSize> entryId;
    entryId.Num(static_cast<TUint>(aContactItemId));

    // Set item id for the base class
    CPIMItem::SetIdL(entryId);

    // Update UID field if it is supported by this contact item
    if (iAdapterManager && iAdapterManager->IsSupportedField(
                EPIMContactUid))
    {
        UpdateUidFieldL(EPIMContactUid, entryId);
    }
}

// ---------------------------------------------------------------------------
// CPIMContactItem::ContactItemId
// (other items were commented in a header)
// ---------------------------------------------------------------------------
//
TUint CPIMContactItem::ContactItemIdL() const
{
    JELOG2(EPim);
    // Return zero if there is no contact item id
    if (CPIMItem::GetId() == KPIMNullItemID)
    {
        return 0;
    }
    // Convert PIM item id to contact database
    // item id, so it is easier to use
    TUint id(0);
    TLex8 lex(iItemID->Des());
    TInt status = lex.Val(id);
    User::LeaveIfError(status);

    return id;
}

// ---------------------------------------------------------------------------
// CPIMContactItem::SetLastModifiedL
// (other items were commented in a header)
// ---------------------------------------------------------------------------
//
void CPIMContactItem::SetLastModifiedL(TPIMDate aLastModified)
{
    JELOG2(EPim);
    CPIMItem::SetLastModifiedL(aLastModified);
    // Update revision field if it is supported
    if (iAdapterManager && iAdapterManager->IsSupportedField(
                EPIMContactRevision))
    {
        UpdateRevisionFieldL(EPIMContactRevision, LastModified());
    }
}

// ---------------------------------------------------------------------------
// CPIMContactItem::PrepareForLoadL
// (other items were commented in a header)
// ---------------------------------------------------------------------------
//
void CPIMContactItem::PrepareForLoadL()
{
    JELOG2(EPim);
    // Prepare base class for loading from the database
    CPIMItem::PrepareForLoadL();
}

// ---------------------------------------------------------------------------
// CPIMContactItem::IsReadOnly
// (other items were commented in a header)
// ---------------------------------------------------------------------------
//
TBool CPIMContactItem::IsReadOnly(const TPIMField& aField)
{
    JELOG2(EPim);
    TBool retVal = EFalse;

    // UID and REVISION fields are read-only if the item
    // has been persisted
    if (aField == EPIMContactUid || aField == EPIMContactRevision)
    {
        retVal = (iItemID->Compare(KPIMNullItemID) != 0);
    }

    return retVal;
}

// ---------------------------------------------------------------------------
// CPIMContactItem::DoLoadFullItemL
// (other items were commented in a header)
// ---------------------------------------------------------------------------
//
void CPIMContactItem::DoLoadFullItemL()
{
    JELOG2(EPim);
    __ASSERT_DEBUG(iInitializedFields || iContactListAdapter,
                   User::Panic(KPIMPanicCategory, EPIMPanicInvalidState));

    // Read full contact item from the adapter. It means that all possible
    // fields are added to this item and the item becomes full.
    TRAPD(err, iContactListAdapter->ReadContactItemL(*this));
    // KErrNotFound indicates that item has been removed and we need to return
    // only those fields which are currently in the item
    __ASSERT_ALWAYS((err == KErrNone) || (err == KErrNotFound),
                    User::Leave(err));
    // Reset initialized field to mark that the item is fully loaded
    delete iInitializedFields;
    iInitializedFields = NULL;
}

// ---------------------------------------------------------------------------
// CPIMContactItem::DoLoadFieldL
// (other items were commented in a header)
// ---------------------------------------------------------------------------
//
void CPIMContactItem::DoLoadFieldL(const TPIMField aField)
{
    JELOG2(EPim);
    __ASSERT_DEBUG(iInitializedFields || iContactListAdapter,
                   User::Panic(KPIMPanicCategory, EPIMPanicInvalidState));

    // Note that revision and uid can not be loaded separately. Those fields
    // will be added to the item when it is generated and associated with a list
    if (aField != EPIMContactRevision && aField != EPIMContactUid)
    {
        iContactListAdapter->ReadContactFieldL(*this,
                                               static_cast<TPIMContactField>(aField));
    }

    // The field is now read form the adapter, so we add it to the list
    // because there is no need to load this field anymore
    iInitializedFields->AppendL(aField);
}

// ---------------------------------------------------------------------------
// CPIMContactItem::GetItemData
// (other items were commented in a header)
// ---------------------------------------------------------------------------
//
MPIMItemData& CPIMContactItem::ItemData()
{
    JELOG2(EPim);
    return *iItemData;
}

// ---------------------------------------------------------------------------
// CPIMContactItem::GetItemData
// (other items were commented in a header)
// ---------------------------------------------------------------------------
//
const MPIMItemData& CPIMContactItem::ItemData() const
{
    JELOG2(EPim);
    return *iItemData;
}

// ---------------------------------------------------------------------------
// CPIMContactItem::GetPreferredIndex
// (other items were commented in a header)
// ---------------------------------------------------------------------------
//

int CPIMContactItem::getPreferredIndex(TPIMField aField) const
{
    JELOG2(EPim);
    TInt error = KErrNone;
    int retVal = 0;
    TRAP(error,
    {
        LeaveIfInvalidOrUnsupportedFieldL(aField);
        retVal = DoGetPreferredIndexL(aField);
    });
    if (error != KErrNone)
        throw error;
    return retVal;
}

// ---------------------------------------------------------------------------
// CPIMContactItem::DoGetPreferredIndexL
// (other items were commented in a header)
// ---------------------------------------------------------------------------
//
TInt CPIMContactItem::DoGetPreferredIndexL(const TPIMField& aField) const
{
    JELOG2(EPim);
    const TPIMFieldDataType fieldType = fieldDataType(aField);

    // Check that the field is valid
    if (EPIMFieldInvalid == fieldType)
    {
        User::Leave(KErrArgument);
    }

    // The field is valid. If it is not a string field, it cannot have
    // preferred index (in the Nokia implementation).
    switch (fieldType)
    {
    case EPIMFieldString:
    {
        // Search through values of the field
        TInt count = iItemData->CountValues(aField);

        for (TInt i = 0; i < count; i++)
        {
            TPIMAttribute attributes = iItemData->AttributesL(
                                           aField, i);
            if ((attributes & EPIMContactAttrPreferred) != 0)
            {
                return i;
            }
        }
        return KPIMNoPreferredIndex;
    }
    default:
    {
        // Not supported.
        // We must not leave, but just ignore the query and return
        // "not set".
        return KPIMNoPreferredIndex;
    }
    }
}

//  End of File
