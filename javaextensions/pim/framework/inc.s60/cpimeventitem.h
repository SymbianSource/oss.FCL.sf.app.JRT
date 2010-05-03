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
* Description:  Event item implementation.
 *
*/


#ifndef CPIMEVENTITEM_H
#define CPIMEVENTITEM_H

//  INCLUDES
#include "cpimitem.h"
#include "mpimeventitem.h"

// FORWARD DECLARATIONS
class MPIMEventAdapterManager;
class MPIMEventListAdapter;
class CPIMRepeatRule;
class CPIMEventValidator;

// CLASS DECLARATION

/**
 *  PIM item Event specialization.
 */
NONSHARABLE_CLASS(CPIMEventItem): public CPIMItem, public MPIMEventItem
{

public: // Constructors and destructor

    /**
     * Two-phased constructor.
     */
    static CPIMEventItem* NewL(
        const CPIMEventValidator& aValidator);

    static CPIMEventItem* NewLC(
        const CPIMEventValidator& aValidator);

    /**
     * Destructor.
     */
    virtual ~CPIMEventItem();

public: // New functions

    /**
     * Sets the associated event adapter manager and event list
     * adapter.
     *
     * @param aEventAdapterManager Adapter manager. Must not be NULL.
     * @param aEventListAdapter List adapter. May be NULL.
     */
    void SetEventAdapterAssociation(
        MPIMEventAdapterManager* aEventAdapterManager,
        MPIMEventListAdapter* aEventListAdapter);

    /**
     * Removes the event adapter associations.
     * Performs any housekeeping related to removing the adapter
     * associations, if necessary.
     */
    void RemoveAdapterAssociation();

    /**
     * Gets pointer to the Repeat Rule owned by this object and transfers
     * the ownership to the caller.
     */
    CPIMRepeatRule* GetRepeatTransferOwnership();

    int getRepeatHandle();

public: // Functions from CPIMItem

    TPIMListType ItemType() const;
    void commit();
    void ListClosed();
    TBool IsReadOnly(const TPIMField& aField);

public: // Functions from MPIMEventItem

    /**
     * Returns item data representation of this class
     * There are non-const and const version of this
     * method available
     */
    MPIMItemData& ItemData();
    const MPIMItemData& ItemData() const;

    MPIMRepeatRuleData* GetRepeat();
    const MPIMRepeatRuleData* GetRepeat() const;
    TBool IsRepeating() const;

    bool isItemRepeating()const;

    void SetRepeating(TBool aSetRepeating);

    void setItemRepeating(jboolean aSetRepeating);

    /**
     * Sets event item id. This functions was introduces because
     * event item ids are handled as strings
     *
     * @param aEventItemId New event item id. If one exists, it will
     *        be overwriten
     */
    void SetEventItemIdL(const TPIMItemID& aEventItemId);

    /**
     * Returns the item id of this event item. This function was
     * introduced because event item ids are handled as strings
     *
     * @return Event item id
     */
    const TPIMItemID EventItemId() const;

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

public: // functions from cpimbase item

    int getPreferredIndex(TPIMField aField) const;

protected: // Constructors

    /**
     * C++ constructor.
     */
    CPIMEventItem(const CPIMEventValidator& aValidator);

    /**
     * By default Symbian 2nd phase constructor is private.
     */
    void ConstructL();

private: // Data

    /** Associated event adapter manager. Not owned. May be NULL. */
    MPIMEventAdapterManager* iEventAdapterManager;

    /** Associated event list adapter. Not owned. May be NULL. */
    MPIMEventListAdapter* iEventListAdapter;

    /** Repeat rule, \b owned by this object. */
    CPIMRepeatRule* iRepeatRule;

    /** Repeating flag. */
    TBool iIsRepeating;

    /** Repeat rule ownership flag. */
    TBool iIsRepeatRuleOwned;
};

#endif // CPIMEVENTITEM_H
// End of File
