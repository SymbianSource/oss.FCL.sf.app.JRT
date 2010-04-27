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
* Description:  Access interface to native ToDo database.
 *
*/


#ifndef MPIMTODOLISTADAPTER_H
#define MPIMTODOLISTADAPTER_H

//  INCLUDES
#include "pimcommon.h"

// FORWARD DECLARATIONS
class MPIMListAdapter;
class MPIMToDoItem;

// CLASS DECLARATION

/**
 *  To-do list adapter creates, reads, writes and removes to-do items
 *  in a native database. See also \ref MPIMListAdapter class description.
 */
class MPIMToDoListAdapter
{
public:
    // Destructor

    /**
     * Destructor.
     */
    virtual ~MPIMToDoListAdapter()
    {
    }

public:
    // New functions

    /**
     * Provides access to the \ref MPIMListAdapter representation of this
     * MPIMToDoListAdapter object.
     *
     * @return The \ref MPIMListAdapter representation of the specialized
     *         list adapter.
     */
    virtual MPIMListAdapter* GetPimListAdapter() = 0;

    /**
     * Creates a new to-do item in the native database.
     * The adapter creates a new native database entry, sets its data
     * according to the data in \a aToDoItem, adds it to the database
     * and sets the Item ID of \a aToDoItem. \a aToDoItem must
     * contain valid data and have Item ID \ref KPIMNullItemID.
     *
     * @param aToDoItem The to-do item to add to the database.
     *
     * @par Leaving:
     * @li KErrAbort - The item lacks some vital data that
     *     cannot be replaced with a default value.
     * @li Other - The list adapter is non-functional.
     */
    virtual void CreateToDoItemL(MPIMToDoItem& aToDoItem) = 0;

    /**
     * Reads an existing to-do item from the native database.
     * The adapter maps the Item ID in \a aToDoItem to a native database
     * entry identifier, reads the entry and sets the data of
     * \a aToDoItem according to the data in the native entry.
     * \a aToDoItem must have valid Item ID that corresponds to an
     * existing native database entry.
     *
     * @param aToDoItem The to-do item to be read from the database.
     *
     * @par Leaving:
     * The method leaves on error. Error codes should be interpreted as
     * follows:
     * @li \c KErrArgument - The Item ID of \a aToDoItem is
     *        \ref KPIMNullItemID and thus invalid.
     * @li \c KErrNotFound - No corresponding native database entry could
     *        be retrieved, although the Item ID of \a aToDoItem is not
     *        \ref KPIMNullItemID. The entry is probably removed from the
     *        native database.
     * @li Other - The list adapter is non-functional.
     */
    virtual void ReadToDoItemL(MPIMToDoItem& aToDoItem) = 0;

    /**
     * Writes an existing to-do item to the native database.
     * The adapter maps the Item ID in \a aToDoItem to a native database
     * entry identifier, reads the entry and sets the data of the entry
     * according to the data in \a aToDoItem. \a aToDoItem must have
     * valid Item ID that corresponds to an existing native database entry.
     *
     * @param aToDoItem The to-do item to write to the database.
     *
     * @par Leaving:
     * The method leaves on error. Error codes should be interpreted as
     * follows:
     * @li \c KErrArgument - The Item ID of \a aToDoItem is
     *        \ref KPIMNullItemID and thus invalid.
     * @li \c KErrNotFound - No corresponding native database entry could
     *        be retrieved, although the Item ID of \a aToDoItem is not
     *        \ref KPIMNullItemID. The entry is probably removed from the
     *        native database.
     * @li \c KErrInUse - The entry is currently in use by some other
     *        client. The operation may succeed later.
     * @li KErrAbort - The item lacks some vital data that
     *     cannot be replaced with a default value.
     * @li Other - The list adapter is non-functional.
     */
    virtual void WriteToDoItemL(MPIMToDoItem& aToDoItem) = 0;

    /**
     * Removes an existing to-do from the native database.
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
    virtual void RemoveToDoItemL(TPIMItemID aItemID) = 0;

protected:
    // Non-public operations

    // Allow derivation with protected default constructor.
    MPIMToDoListAdapter()
    {
    }

private:
    // Prohibited operations

    // Prohibit copy constructor.
    MPIMToDoListAdapter(const MPIMToDoListAdapter&)
    {
    }

    // Prohibit assigment operator.
    MPIMToDoListAdapter& operator=(const MPIMToDoListAdapter&)
    {
        return *this;
    }

};

#endif // MPIMTODOLISTADAPTER_H
// End of File

