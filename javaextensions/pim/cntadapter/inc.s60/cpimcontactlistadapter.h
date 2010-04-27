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


#ifndef CPIMCONTACTLISTADAPTER_H
#define CPIMCONTACTLISTADAPTER_H

// INTERNAL INCLUDES
#include "mpimcontactlistadapter.h"
#include "mpimlistadapter.h"
#include "pimexternalchanges.h"

// EXTERNAL INCLUDES
#include <cntdef.h>         // TContactItemId
#include <cntdbobs.h>       // MContactDbObserver
#include <e32base.h>
#include <badesca.h>

// FORWARD DECLARATIONS
class CPIMContactCategoryManager;
class CPIMContactItemAdapter;
class CContactDatabase;
class CContactChangeNotifier;
class CContactItemViewDef;
class CContactGroup;
class CCntFilter;

// CLASS DECLARATION

/**
 * Contact List Adapter
 * Manages Contact Items and Categories in Contacts Model.
 */
NONSHARABLE_CLASS(CPIMContactListAdapter): public CBase,
        public MContactDbObserver,
        //public MContactViewObserver,
        public MPIMListAdapter,
        public MPIMContactListAdapter
{
public: // Constructor and destructor

    /**
     * Two-phased constructor.
     */
    static CPIMContactListAdapter* NewL(java::util::FunctionServer* aFuncServer);

    /**
     * Destructor.
     */
    ~CPIMContactListAdapter();

public: // MContactDbObserver

    /**
     * Tests the contact database observer event type and handles it
     *
     * @param aEvent Provides information about the change event.
     */
    void HandleDatabaseEventL(TContactDbObserverEvent aEvent);


public: // MPIMListAdapter


    /**
     * Provides all categories currently existing in the native database.
     *
     * @return Array of categories. The contents of the array are not
     *         meant to be modified. The contents of the array may change
     *         whenever any category related method is called.
     *
     * @par Leaving:
     * The method leaves on error. Such error always means that the list
     * adapter is non-functional.
     */
    const CDesCArray& GetCategoriesL();

    /**
     * Adds a new category to the native database. If the category already
     * exists, nothing is done and the method returns successfully.
     *
     * @param aNewCategory The name of the category to be added.
     *
     * @par Leaving:
     * The method leaves on error. Error codes should be interpreted as
     * follows:
     * @li \c KErrArgument - \a aNewCategory is invalid.
     * @li Other - The list adapter is non-functional.
     */
    void AddCategoryL(const TDesC& aNewCategory);

    /**
     * Deletes an existing category. If there is no such category, nothing
     * is done and the method returns successfully.
     *
     * @param aCategory The category to be deleted.
     *
     * @par Leaving:
     * The method leaves on error. Error codes should be interpreted as
     * follows:
     * @li \c KErrNotFound - \a aCategory is invalid.
     * @li Other - The list adapter is non-functional.
     */
    void DeleteCategoryL(const TDesC& aCategory);

    /**
     * Renames an existing category. Entries in the old category are moved
     * to the new category.
     *
     * @param aOldCategory The old category name.
     * @param aNewCategory The new category name.
     *
     * @par Leaving:
     * The method leaves on error. Error codes should be interpreted as
     * follows:
     * @li \c KErrArgument - \a aNewCategory is invalid.
     * @li \c KErrNotFound - \a aOldCategory does not exist.
     * @li Other - The list adapter is non-functional.
     */
    void RenameCategoryL(const TDesC& aOldCategory, const TDesC& aNewCategory);

    /**
     * Checks whether there have been external changes to the categories in
     * the native database after last call to
     * \ref GetExternalCategoryModifications or list adapter creation.
     *
     * @return ETrue if there are any external changes, EFalse otherwise.
     */
    TBool IsCategoriesExternallyModified();

    /**
     * Provides the external changes to the categories in the native
     * database. See \ref IsCategoriesExternallyModified method.
     *
     * @return An array of category state change objects. The \b ownership
     *         of the array is transferred to the caller. Note that the
     *         array elements contain heap-allocated data.
     *
     * @par Leaving:
     * The method leaves with \c KErrCorrupt if updating the list of
     * external modifications has failed at some point. If the method has
     * leaved once, it must always leave.
     */
    RPointerArray<CPIMCategoryStateChange>*
    GetExternalCategoryModificationsL();

    /**
     * Checks whether there have been external changes to the items
     * (or entries) in the native database after last call to
     * GetExternalItemModificationsL() or list adapter creation.
     *
     * If GetExternalItemModificationsL() has never been called, all
     * entries in the native database are considered to be new.
     *
     * See GetExternalItemModificationsL().
     *
     * @return \c EExternalChangesNone if there are no changes;
     *         \c EExternalChangesMinor if there are changes that
     *         can be classified to new, removed or modified;
     */
    MPIMListAdapter::TExternalItemChangeClass IsItemsExternallyModified();

    /**
     * Provides the external changes to the items (or entries) in the
     * native database. See \ref IsItemsExternallyModified method.
     *
     * @return An array of item state change objects. The \b ownership
     *         of the array is transferred to the caller.
     *
     * @par Leaving:
     * The method leaves with \c KErrCorrupt if updating the list of
     * external modifications has failed at some point. If the method has
     * leaved once, it must always leave.
     */
    RPointerArray< CPIMItemStateChange>* GetExternalItemModificationsL();

    /**
     * Used to inform the list adapter that the list has been closed. The
     * list adapter may then release all resources it has reserved.
     * No method of the list adapter can be invoked after call to Close.
     */
    void Close();

    void CPIMContactListAdapter::DoClose();

public: // MPIMContactListAdapter

    /**
     * Provides access to the \ref MPIMListAdapter representation of this
     * MPIMContactListAdapter object.
     *
     * @return The \ref MPIMListAdapter representation of the specialized
     *         list adapter.
     */
    MPIMListAdapter* GetPimListAdapter();

    /**
     * Creates a new contact item (entry) in the native database.
     * The adapter creates a new native database entry, sets its data
     * according to the data in \a aContactItem, adds it to the database
     * and sets the Item ID of \a aContactItem. \a aContactItem must
     * contain valid data and have Item ID \ref KPIMNullItemID.
     *
     * @param aContactItem The contact item to add to the database.
     *
     * @par Leaving:
     * The method leaves on error. Such error always means that the list
     * adapter is non-functional.
     */
    void CreateContactItemL(MPIMContactItem& aContactItem);

    void CPIMContactListAdapter::DoCreateContactItemL(
        MPIMContactItem& aContactItem);

    /**
     * Reads an existing contact item from the native database.
     * The adapter maps the Item ID in \a aContactItem to a native database
     * entry identifier, reads the entry and sets the data of
     * \a aContactItem according to the data in the native entry.
     * \a aContactItem must have valid Item ID that corresponds to an
     * existing native database entry.
     *
     * @param aContactItem The contact item to be read from the database.
     *
     * @par Leaving:
     * The method leaves on error. Error codes should be interpreted as
     * follows:
     * @li \c KErrArgument - The Item ID of \a aContactItem is
     *        \ref KPIMNullItemID and thus invalid.
     * @li \c KErrNotFound - No corresponding native database entry could
     *        be retrieved, although the Item ID of \a aContactItem is not
     *        \ref KPIMNullItemID. The entry is probably removed from the
     *        native database.
     * @li Other - The list adapter is non-functional.
     */
    void ReadContactItemL(MPIMContactItem& aContactItem);

    void CPIMContactListAdapter::DoCallReadContactItemL(
        MPIMContactItem& aContactItem);

    /**
     * Reads an existing contact item from the native database
     * This version reads only minimal contact item data and should be used
     * when listing contacts through the API. Contacts Model view definition
     * is used to read items with this function
     *
     * @param aContactItem The contat item to be read from the database
     *
     * @par Leaving:
     * The methods leaves on error. Error codes should be interpreted as
     * follows:
     * @li \c KErrArgument - The Item ID of \a aContactItem is
     *        \ref KPIMNullItemID and thus invalid.
     * @li \c KErrNotFound - No corresponding native database entry could
     *        be retrieved, although the Item ID of \a aContactItem is not
     *        \ref KPIMNullItemID. The entry is probably removed from the
     *        native database.
     * @li Other - The list adapter is non-functional.
     */
    void ReadMinimalContactItemL(
        MPIMContactItem& aContactItem);

    void CPIMContactListAdapter::DoCallReadMinimalContactItemL(
        MPIMContactItem& aContactItem);

    /**
     * Reads an existing contact item from the native database
     * This version reads only minimal contact item data and should be used
     * when listing contacts through the API. This version takes a matching
     * item as a parameter which defines which fields are to be retrieved
     * from the native database. The fields include minimal fields also,
     * currently name array elements.
     *
     * @param aContactItem The contact item to be read from the database
     * @param aMatchingContactitem A contact which gives information which
     *        fields are to be retrieved from the native database
     *
     * @par Leaving:
     * The methods leaves on error. Error codes should be interpreted as
     * follows:
     * @li \c KErrArgument - The Item ID of \a aContactItem is
     *        \ref KPIMNullItemID and thus invalid.
     * @li \c KErrNotFound - No corresponding native database entry could
     *        be retrieved, although the Item ID of \a aContactItem is not
     *        \ref KPIMNullItemID. The entry is probably removed from the
     *        native database.
     * @li Other - The list adapter is non-functional.
     */
    void ReadMinimalContactItemL(
        MPIMContactItem& aContactItem,
        const MPIMContactItem& aMatchingContactItem);

    /**
     * Reads one field from the contact database and adds it to the
     * item. New view definition is used to filter the database
     *
     * @param aContactItem The contact item where the data is added
     * @param aContactField The contact fields which is to be read
     *        from the database
     */
    void ReadContactFieldL(MPIMContactItem& aContactItem,
                           TPIMContactField aContactField);

    void CPIMContactListAdapter::DoReadContactFieldL(
        MPIMContactItem& aContactItem,
        TPIMContactField aContactField);

    /**
     * Writes an existing contact item to the native database.
     * The adapter maps the Item ID in \a aContactItem to a native database
     * entry identifier, reads the entry and sets the data of the entry
     * according to the data in \a aContactItem. \a aContactItem must have
     * valid Item ID that corresponds to an existing native database entry.
     *
     * @param aContactItem The contact item to write to the database.
     *
     * @par Leaving:
     * The method leaves on error. Error codes should be interpreted as
     * follows:
     * @li \c KErrArgument - The Item ID of \a aContactItem is
     *        \ref KPIMNullItemID and thus invalid.
     * @li \c KErrNotFound - No corresponding native database entry could
     *        be retrieved, although the Item ID of \a aContactItem is not
     *        \ref KPIMNullItemID. The entry is probably removed from the
     *        native database.
     * @li \c KErrInUse - The entry is currently in use by some other
     *        client. The operation may succeed later.
     * @li Other - The list adapter is non-functional.
     */
    void WriteContactItemL(MPIMContactItem& aContactItem);

    void CPIMContactListAdapter::DoWriteContactItemL(
        MPIMContactItem& aContactItem);

    /**
     * Removes an existing contact from the native database.
     * The adapter maps \a aItemID to a native database entry and removes
     * it.
     *
     * @param aItemID Item ID of the item to be removed.
     *
     * @par Leaving:
     * @li \c KErrArgument - \a aItemID is \ref KPIMNullItemID and thus
     *        invalid.
     * @li \c KErrNotFound - No native database entry could be retrieved
     *        by \a aItemID. The entry is probably already removed.
     * @li \c KErrInUse - The entry is currently in use by some other
     *        client. The operation may succeed later.
     * @li Other - The list adapter is non-functional.
     */
    void RemoveContactItemL(TPIMItemID aItemID);

private:

    /**
     * C++ default constructor.
     */
    CPIMContactListAdapter(java::util::FunctionServer* aFuncServer);

    /**
     * Symbian 2nd phase constructor.
     */
    void ConstructL();

    /**
     * Adds an entry to external category change list.
     * Also causes the category cache to be flushed.
     *
     * @param aId Id of the changed category
     * @param aType type of the change
     */
    void ExternalGroupChangeL(TContactItemId aId,
                              TPIMExternalChangeType aType);

    /**
     * Adds an entry to external item change list.
     *
     * @param aId Id of the changed item
     * @param aType type of the change
     */
    void ExternalItemChangeL(TContactItemId aId,
                             TPIMExternalChangeType aType);

    /**
     * Filters all contact items from the database to the item changes list.
     */
    void FilterAllContactsL();

    /**
     * Initialized minimal view definition to include
     * requested fields when item is loaded from the native database
     */
    void InitializeMinimalViewDefinitionL();

    /**
     * Reads a contact item from the contacts database using specified
     * item view definition. The item must be associated with a list
     * (i.e it has to have an item id which is not zero)
     *
     * @param aContactItem PIM Contact item to which the data is read
     * @param aContactItemViewDef View definition which is used for
     *        reading the contact item from the database
     */
    void CPIMContactListAdapter::DoReadContactItemL(
        MPIMContactItem& aContactItem,
        const CContactItemViewDef& aContactItemViewDef);

    void IsDatabaseReadyL();

    void DoRemoveContactItemL(TPIMItemID aItemID);

private: // Data

    // Contacts Model database used for storing contact data, owned.
    CContactDatabase* iDatabase;

    // Notifies ContactListAdapter about changes in the database, owned.
    CContactChangeNotifier* iNotifier;

    // Handles categories, owned.
    CPIMContactCategoryManager* iCategoryManager;

    // Converts between PIM items <--> Contacts Model items, owned.
    CPIMContactItemAdapter* iItemAdapter;

    // View Definitions. Owned
    CContactItemViewDef* iMinimalFieldsViewDef;

    // Active scheduler wait for waiting remote view initialization
    // This class can be used as a data member, see definition of
    // CActiveSchedulerWait
    CActiveSchedulerWait iWait;

    // Array of unqueried category changes in the database, owned.
    // When the ownership is given away, this pointer is chaned to NULL.
    RPointerArray<CPIMCategoryStateChange>* iCategoryChanges;

    // Array of unqueried item changes in the database, owned.
    // When the ownership is given away, this pointer is chaned to NULL.
    RPointerArray< CPIMItemStateChange>* iItemChanges;

    // Have item changes been queried or not
    TBool iFirstItemChanges;

    java::util::FunctionServer* iFuncServer;

};

#endif // CPIMCONTACTLISTADAPTER_H
// End of File
