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


#ifndef CPIMCONTACTITEM_H
#define CPIMCONTACTITEM_H

// INTERNAL INCLUDES
#include "cpimitem.h"
#include "mpimcontactitem.h"

// EXTERNAL INCLUDES
#include <cntdef.h>

// FORWARD DECLARATIONS
class CPIMContactValidator;
class MPIMContactAdapterManager;
class MPIMContactListAdapter;

// CLASS DECLARATION

/**
 *  PIM item Contact specialization.
 *
 *  @par Notes:
 *  @li The Contact item implementation supports preferred index handling only
 *  on string fields (\ref EPIMFieldString). For all other field types the
 *  attribute is either leave intact (if the contact is not associated with
 *  a list), or dropped silently (if the contact is associated with a list).
 */
NONSHARABLE_CLASS(CPIMContactItem): public CPIMItem, public MPIMContactItem
{

public: // Constructors and destructor

    /**
     * Two-phased constructor.
     * @param aContactValidator Contact validator.
     */
    static CPIMContactItem* NewL(
        const CPIMContactValidator& aContactValidator);

    /**
     * Two-phased constructor, puts the created item into cleanup stack.
     * @param aContactValidator Contact validator.
     */
    static CPIMContactItem* NewLC(
        const CPIMContactValidator& aContactValidator);

    /**
     * Destructor.
     */
    virtual ~CPIMContactItem();

public: // New functions

    /**
     * Sets the associated contact adapter manager and contact list
     * adapter.
     *
     * @param aContactAdapterManager Adapter manager. Must not be NULL.
     * @param aContactListAdapter List adapter. May be NULL.
     */
    void SetContactAdapterAssociation(
        MPIMContactAdapterManager* aContactAdapterManager,
        MPIMContactListAdapter* aContactListAdapter);

    /**
     * Removes the contact adapter associations.
     * Performs any housekeeping related to removing the adapter
     * associations, if necessary.
     */
    void RemoveAdapterAssociation();

public: // Functions from CPIMItem

    TPIMListType ItemType() const;
    void commit();
    void ListClosed();

    /**
     * @par Notes:
     * Overridden so that first adds the string value using base class
     * AddStringL operation and then moves preferred index, if necessary.
     */
    void AddStringL(TPIMField aField,
                    TPIMAttribute aAttributes,
                    HBufC* aValue);

    /**
     * @par Notes:
     * Overridden so that first sets the string value using base class
     * SetStringL operation and then moves preferred index, if necessary.
     */
    void SetStringL(TPIMField aField,
                    TInt aIndex,
                    TPIMAttribute aAttributes,
                    HBufC* aValue);

    TBool IsReadOnly(const TPIMField& aField);

    /**
     * Loads full item from the list adapter. This function
     * should be overwriten in the base class if it supports
     * partial data initialization
     */
    void DoLoadFullItemL();

    /**
     * Loads a specific field from the list adapter. This function
     * should be overwriten in the base class if it supports
     * partial data initialization
     *
     * @param aField The field which is to be loaded from
     *        the associated list adapter
     */
    void DoLoadFieldL(const TPIMField aField);

public: // Functions from MPIMContactItem

    MPIMItemData& ItemData();
    const MPIMItemData& ItemData() const;

    /**
     * @par Notes
     * @li The implementation supports preferred index only on
     *     string fields (\ref EPIMFieldString).
     */

    int getPreferredIndex(TPIMField aField) const;
    /**
     * Sets new contact item id for this contact item
     *
     * @param aContactItemId New id for this contact item. Old is to
     *        be owerwriten
     */
    void SetContactItemIdL(const TPIMItemID& aContactItemId);

    /**
     * Sets new contact item id for this contact item
     * This function was introduces for easier item id handling
     *
     * @param aContactItemId New id for this contact item. Old is to
     *        be owerwriten
     */
    void SetContactItemIdL(const TUint aContactItemId);

    /**
     * Returns contact item id
     * The existing PIM item id is converted to a contact
     * contact database item id, so it is easier to use it
     *
     * @return Contact item id
     */
    TUint ContactItemIdL() const;

    /**
     * Sets the last modified date/time of the item
     * @param aLastModified Last modification date of this item
     */
    void SetLastModifiedL(TPIMDate aLastModified);

    /**
     * Resets the item's internal state and remove all data
     * associated with this item. The item is ready for use
     * after reset
     */
    void PrepareForLoadL();

public: // functions from pimbaseitem

    int getRepeatHandle()
    {
        return 0;
    }

    bool isItemRepeating() const
    {
        return false;
    }

    void setItemRepeating(jboolean /*aSetRepeating*/) {}

protected: // New functions

    /**
     * Set preferred index for a string field.
     * Clears the preferred index attribute from all values of given field
     * and then sets the preferred index attribute for the given index.
     *
     * @par Leaving:
     * @li \c KErrArgument \a aField is invalid.
     * @li \c KErrNotFound \a aIndex is invalid.
     */
    void SetPreferredIndexStringL(
        const TPIMField& aField,
        const TInt& aIndex,
        TPIMAttribute aAttribute);

    /**
     * Get preferred index (value with the "preferred" attribute set)
     * from given field.
     *
     * @return Index of the value with "preferred" attribute set; -1 if not
     *         found.
     *
     * @par Leaving:
     * @li \c KErrArgument - \a aField is not supported for Contact items.
     * @li Other - internal error.
     *
     * @par Notes
     * @li The implementation supports preferred index only on
     *     string fields (\ref EPIMFieldString).
     */
    TInt DoGetPreferredIndexL(const TPIMField& aField) const;

protected: // Constructors

    /**
     * C++ constructor.
     */
    CPIMContactItem(const CPIMContactValidator& aContactValidator);

    // Data

    /**
     * Associated contact adapter manager. Not owned by this object.
     * May be NULL.
     */
    MPIMContactAdapterManager* iContactAdapterManager;

    /**
     * Associated contact list adapter. Not owned by this object.
     * May be NULL.
     */
    MPIMContactListAdapter* iContactListAdapter;

};

#endif // CPIMCONTACTITEM_H
// End of File
