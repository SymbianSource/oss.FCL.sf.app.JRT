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
* Description:  PIM ToDo item specialization.
 *
*/


#ifndef CPIMTODOITEM_H
#define CPIMTODOITEM_H

//  INCLUDES
#include "cpimitem.h"
#include "mpimtodoitem.h"

#include "mpimtodoadaptermanager.h"
#include "mpimtodolistadapter.h"

// FORWARD DECLARATIONS
class CPIMToDoValidator;

// CLASS DECLARATION

/**
 *  PIM item to-do specialization.
 */
NONSHARABLE_CLASS(CPIMToDoItem): public CPIMItem,
        public MPIMToDoItem
{

public: // Constructors and destructor

    /**
     * Two-phased constructor.
     */
    static CPIMToDoItem* NewL(
        const CPIMToDoValidator& aValidator);

    static CPIMToDoItem* NewLC(
        const CPIMToDoValidator& aValidator);

    /**
     * Destructor.
     */
    virtual ~CPIMToDoItem();

public: // New functions

    /**
     * Sets the associated to-do adapter manager and to-do list
     * adapter.
     *
     * @param aToDoAdapterManager Adapter manager. Must not be NULL.
     * @param aToDoListAdapter List adapter. May be NULL.
     */
    void SetToDoAdapterAssociation(
        MPIMToDoAdapterManager* aToDoAdapterManager,
        MPIMToDoListAdapter* aToDoListAdapter);

    /**
     * Removes the to-do adapter associations.
     * Performs any housekeeping related to removing the adapter
     * associations, if necessary.
     */
    void RemoveAdapterAssociation();

public: // Functions from CPIMItem

    TPIMListType ItemType() const;
    void commit();
    void ListClosed();
    TBool IsReadOnly(const TPIMField& aField);

    void AddIntL(
        TPIMField aField,
        TPIMAttribute aAttributes,
        TInt aValue);

    void SetIntL(
        TPIMField aField,
        TInt aIndex,
        TPIMAttribute aAttributes,
        TInt aValue);

public: // Functions from MPIMToDoItem

    /**
     * Returns item data representation of this class
     * There are non-const and const version of this
     * method available
     */
    MPIMItemData& ItemData();
    const MPIMItemData& ItemData() const;

    /**
     * Sets todo item id. This functions was introduces because
     * todo item ids are handled as strings
     *
     * @param aToDoItemId New event item id. If one exists, it will
     *        be overwriten
     */
    void SetToDoItemIdL(const TPIMItemID& aToDoItemId);

    /**
     * Returns the item id of this event item. This function was
     * introduced because event todo ids are handled as strings
     *
     * @return Event item id
     */
    const TPIMItemID ToDoItemId() const;

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

    int getPreferredIndex(TPIMField aField) const;

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
     * Simplifies given priority between 0 and 9 (inclusive)
     * to 1 (High), 4 (Medium) or 7 (Low).
     *
     * @return Simplified priority. If the given value is out
     *         of the [0,9] range, the value is returned as such.
     */
    TInt SimplifyPriority(TInt aPriority);

protected: // Constructors

    /**
     * C++ constructor.
     */
    CPIMToDoItem(const CPIMToDoValidator& aValidator);

private: // Data

    /** Associated to-do adapter manager. Not owned. May be NULL. */
    MPIMToDoAdapterManager* iToDoAdapterManager;

    /** Associated to-do list adapter. Not owned. May be NULL. */
    MPIMToDoListAdapter* iToDoListAdapter;
};

#endif // CPIMTODOITEM_H
// End of File
