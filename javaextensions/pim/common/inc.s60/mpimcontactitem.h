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
* Description:  Contact item access interface.
 *
*/


#ifndef MPIMCONTACTITEM_H
#define MPIMCONTACTITEM_H

// INTERNAL INCLUDES
#include "e32def.h"
#include "pimcommon.h"
#include "pimcontact.h"

//  FORWARD DECLARATIONS
class MPIMItemData;

/**
 *  Interface for accessing Contact-specific parts of an item.
 *  See \ref MPIMItemData class.
 */
class MPIMContactItem
{
public:
    // Destructor

    /**
     * Destructor.
     */
    virtual ~MPIMContactItem()
    {
    }

public:
    // New functions

    //@{
    /**
     * Provides access to the \ref MPIMItemData representation of this
     * MPIMContactItemData object.
     * There are const and non-const versions of this method.
     *
     * @return The \ref MPIMItemData representation of the item.
     */
    virtual MPIMItemData& ItemData() = 0;
    virtual const MPIMItemData& ItemData() const = 0;
    //@}

    /**
     * Provides a single preferred index among the indexes in the value
     * array of a specific field.
     *
     * @param aField The field.
     *
     * @return Preferred index in the value array. KPIMNoPreferredIndex
     *         if no preferred index is set.
     *
     * @par Leaving:
     * The method leaves on error. Error codes should be interpreted as
     * follows:
     * @li \c KErrArgument - \a aField is not valid for the implementing
     *     PIM item type.
     * @li \c KErrNotSupported - \a aField is not supported by this
     *     particular implementation.
     */
    virtual TInt getPreferredIndex(TPIMField aField) const = 0;

    /**
     * Sets new contact item id for this contact item
     *
     * @param aContactItemId New id for this contact item. Old is to
     *        be owerwriten
     */
    virtual void SetContactItemIdL(const TPIMItemID& aContactItemId) = 0;

    /**
     * Sets new contact item id for this contact item
     * This function was introduces for easier item id handling
     *
     * @param aContactItemId New id for this contact item. Old is to
     *        be owerwriten
     */
    virtual void SetContactItemIdL(const TUint aContactItemId) = 0;

    /**
     * Returns contact item id
     * The existing PIM item id is converted to a contact
     * contact database item id, so it is easier to use it
     *
     * @return Contact item id
     */
    virtual TUint ContactItemIdL() const = 0;

    /**
     * Sets the last modified date/time of the item
     * @param aLastModified Last modification date of this item
     */
    virtual void SetLastModifiedL(TPIMDate aLastModified) = 0;

    /**
     * Resets the item's internal state and remove all data
     * associated with this item. The item is ready for use
     * after reset
     */
    virtual void PrepareForLoadL() = 0;

protected:
    // Non-public operations

    // Allow derivation with protected default constructor.
    MPIMContactItem()
    {
    }

private:
    // Prohibited operations
    // Prohibit copy constructor.
    MPIMContactItem(const MPIMContactItem&)
    {
    }

    // Prohibit assigment operator.
    MPIMContactItem& operator=(const MPIMContactItem&)
    {
        return *this;
    }

};

#endif // MPIMCONTACTITEM_H
// End of File
