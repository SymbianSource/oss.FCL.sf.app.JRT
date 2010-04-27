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
* Description:  Event item access interface.
 *
*/


#ifndef MPIMEVENTITEM_H
#define MPIMEVENTITEM_H

// INTERNAL INCLUDES
#include "pimtypes.h"

// EXTERNAL INCLUDES
#include <e32def.h>

//  FORWARD DECLARATIONS
class MPIMItemData;
class MPIMRepeatRuleData;

/**
 *  Interface for accessing event-specific parts of an item.
 *  See \ref MPIMItemData class.
 */
NONSHARABLE_CLASS(MPIMEventItem)
{
public: // Destructor

    /**
     * Destructor.
     */
    virtual ~MPIMEventItem()
    {};

public: // New functions

    //@{
    /**
     * Provides access to the \ref MPIMItemData representation of this
     * MPIMEventItem object.
     * There are const and non-const versions of this method.
     *
     * @return The \ref MPIMItemData representation of the item.
     */
    virtual MPIMItemData& ItemData() = 0;
    virtual const MPIMItemData& ItemData() const = 0;
    //@}

    /**
     * Provides access to the \ref MPIMRepeatRuleData object of this
     * MPIMEventItem object. Ownership of the repeat rule object
     * is retained within this object.
     *
     * @return Associated repeat rule.
     */
    virtual MPIMRepeatRuleData* GetRepeat() = 0;
    virtual const MPIMRepeatRuleData* GetRepeat() const = 0;

    /**
     * Checks whether the item is repeating (applies its repeat rule).
     *
     * @return ETrue if the item is repeating, EFalse otherwise.
     */
    virtual TBool IsRepeating() const = 0;

    /**
     * Sets the item either repeating or non-repeating.
     *
     * @param aSetRepeating If ETrue, the associated repeat rule is
     *        taken in use. If EFalse, the associated repeat rule
     *        is ignored from this point on.
     */
    virtual void SetRepeating(TBool aSetRepeating) = 0;

    /**
     * Sets event item id. This functions was introduces because
     * event item ids are handled as strings
     *
     * @param aEventItemId New event item id. If one exists, it will
     *        be overwriten
     */
    virtual void SetEventItemIdL(const TPIMItemID& aEventItemId) = 0;

    /**
     * Returns the item id of this event item. This function was
     * introduced because event item ids are handled as strings
     *
     * @return Event item id
     */
    virtual const TPIMItemID EventItemId() const = 0;

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

protected: // Non-public operations

    // Allow derivation with protected default constructor.
    MPIMEventItem()
    {}

private: // Prohibited operations
    // Prohibit copy constructor.
    MPIMEventItem(const MPIMEventItem&)
    {}

    // Prohibit assigment operator.
    MPIMEventItem& operator=(const MPIMEventItem&)
    {
        return *this;
    }

}
;

#endif // MPIMEVENTITEM_H
// End of File
