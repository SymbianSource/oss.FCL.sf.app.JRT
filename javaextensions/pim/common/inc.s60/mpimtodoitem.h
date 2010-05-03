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
* Description:  ToDo item access interface.
 *
*/


#ifndef MPIMTODOITEM_H
#define MPIMTODOITEM_H

//  INCLUDES
#include "pimcommon.h"

//  FORWARD DECLARATIONS
class MPIMItemData;

/**
 *  Interface for accessing to-do-specific parts of an item.
 *  See \ref MPIMItemData class.
 */
class MPIMToDoItem
{
public:
    // Destructor

    /**
     * Destructor.
     */
    virtual ~MPIMToDoItem()
    {
    }
    ;

public:
    // New functions

    //@{
    /**
     * Provides access to the \ref MPIMItemData representation of this
     * MPIMToDoItem object.
     * There are const and non-const versions of this method.
     *
     * @return The \ref MPIMItemData representation of the item.
     */
    virtual MPIMItemData& ItemData() = 0;
    virtual const MPIMItemData& ItemData() const = 0;
    //@}

    /**
     * Sets todo item id. This functions was introduces because
     * todo item ids are handled as strings
     *
     * @param aToDoItemId New event item id. If one exists, it will
     *        be overwriten
     */
    virtual void SetToDoItemIdL(const TPIMItemID& aToDoItemId) = 0;

    /**
     * Returns the item id of this event item. This function was
     * introduced because event todo ids are handled as strings
     *
     * @return Event item id
     */
    virtual const TPIMItemID ToDoItemId() const = 0;

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
    MPIMToDoItem()
    {
    }

private:
    // Prohibited operations
    // Prohibit copy constructor.
    MPIMToDoItem(const MPIMToDoItem&)
    {
    }

    // Prohibit assigment operator.
    MPIMToDoItem& operator=(const MPIMToDoItem&)
    {
        return *this;
    }

};

#endif // MPIMTODOITEM_H
// End of File
