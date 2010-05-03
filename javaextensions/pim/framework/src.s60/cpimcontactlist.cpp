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
* Description:  Contact list specialization.
 *
*/


// INCLUDE FILES
#include  "cpimcontactlist.h"
#include  "mpimcontactadaptermanager.h"
#include  "mpimcontactlistadapter.h"
#include  "cpimcontactvalidator.h"
#include  "cpimcontactitem.h"
#include "logger.h"

CPIMContactList::CPIMContactList(const CPIMContactValidator& aValidator) :
        CPIMList(aValidator), iContactValidator(aValidator)
{
    JELOG2(EPim);
}

void CPIMContactList::ConstructL(
    MPIMContactAdapterManager* aContactAdapterManager,
    MPIMContactListAdapter* aContactListAdapter,
    MPIMLocalizationData* aLocalizationData)
{
    JELOG2(EPim);
    CPIMList::ConstructL(aLocalizationData,
                         aContactAdapterManager->GetAdapterManager(),
                         aContactListAdapter->GetPimListAdapter());

    iContactAdapterManager = aContactAdapterManager;
    iContactListAdapter = aContactListAdapter;
}

CPIMContactList* CPIMContactList::NewL(
    MPIMContactAdapterManager* aContactAdapterManager, // owned
    MPIMContactListAdapter* aContactListAdapter, // owned
    MPIMLocalizationData* aLocalizationData, // not owned
    const CPIMContactValidator& aValidator) // not owned
{
    JELOG2(EPim);
    CPIMContactList* self = new(ELeave) CPIMContactList(aValidator);

    CleanupStack::PushL(self);

    self->ConstructL(aContactAdapterManager, aContactListAdapter,
                     aLocalizationData);

    CleanupStack::Pop(self);

    return self;
}

CPIMContactList::~CPIMContactList()
{
    JELOG2(EPim);
    delete iContactListAdapter;
    delete iContactAdapterManager;
}

CPIMContactItem* CPIMContactList::DoCreateContactL()
{
    JELOG2(EPim);
    // Create
    CPIMContactItem* newContact = CPIMContactItem::NewL(iContactValidator);

    // Associate with list
    newContact->SetContactAdapterAssociation(iContactAdapterManager,
            iContactListAdapter);

    return newContact;
}

void CPIMContactList::close()
{
    JELOG2(EPim);
    if (!iContactListAdapter)
    {
        throw KErrSessionClosed ;
    }
    CPIMList::close();

    delete iContactListAdapter;
    iContactListAdapter = NULL;
}

// ---------------------------------------------------------------------------
// CPIMContactList::DoCreateItemL
// Creates a new empty PIM item or loads new item from the native database
// The new item is loaded with a matching item if such is specified.
// ---------------------------------------------------------------------------
//
CPIMItem* CPIMContactList::DoCreateItemL(const TPIMItemID& aItemId,
        CPIMItem* aMatchingItem)
{
    JELOG2(EPim);
    CPIMContactItem* newContact = DoCreateContactL();

    if (aItemId != KPIMNullItemID)
    {
        CleanupStack::PushL(newContact);
        __ASSERT_ALWAYS(iContactListAdapter, User::Leave(KErrSessionClosed));

        // Now it is necessary to check which types of contact is needed.
        // If the matching item is not NULL, the item loading can be
        // improved by loading minimal fields and fields needed to match items
        newContact->SetContactItemIdL(aItemId);
        if (aMatchingItem)
        {
            // Load minimal contact item with a matching item (e.g
            // load minimal fields and fields needed to match items
            CPIMContactItem* matchingContactItem =
                static_cast<CPIMContactItem*>(aMatchingItem);
            iContactListAdapter->ReadMinimalContactItemL(*newContact,
                    *matchingContactItem);
        }
        else
        {
            // Load minimal contact with no criteria if no matching
            // item is specified. This means that the items countais
            // very little data but it is very fast to load from the database
            iContactListAdapter->ReadMinimalContactItemL(*newContact);
        }
        newContact->UpdateUidFieldL(EPIMContactUid, aItemId);
        CleanupStack::Pop(newContact);
    }

    return newContact;
}

void CPIMContactList::DoUpdateItemL(CPIMItem& aItem)
{
    JELOG2(EPim);
    if (!iContactListAdapter)
    {
        User::Leave(KErrSessionClosed);
    }

    CPIMContactItem& contactItem = static_cast<CPIMContactItem&>(aItem);
    // Update from the native database (e.g item has propably modified outside
    // PIM API context. Reading minimal contact causes item reset so modfications
    // in the database are all reflected to PIM API
    iContactListAdapter->ReadMinimalContactItemL(contactItem);
    contactItem.UpdateUidFieldL(EPIMContactUid, aItem.GetId());
}

void CPIMContactList::DoDeleteItemL(CPIMItem& aItem)
{
    JELOG2(EPim);
    if (!iContactListAdapter)
    {
        User::Leave(KErrSessionClosed);
    }

    iContactListAdapter->RemoveContactItemL(aItem.GetId());
    aItem.UpdateUidFieldL(EPIMContactUid, TPtrC8(KPIMNullItemID));
}

//  End of File
