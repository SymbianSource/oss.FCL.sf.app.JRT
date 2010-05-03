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
* Description:  Access interface to native Event database.
 *
*/


#ifndef MPIMEVENTLISTADAPTER_H
#define MPIMEVENTLISTADAPTER_H

//  INCLUDES
#include "pimcommon.h"

// FORWARD DECLARATIONS
class MPIMListAdapter;
class MPIMEventItem;

// CLASS DECLARATION

/**
 *  To-do list adapter creates, reads, writes and removes event items
 *  in a native database. See also \ref MPIMListAdapter class description.
 */
class MPIMEventListAdapter
{
public:
    // Destructor

    /**
     * Destructor.
     */
    virtual ~MPIMEventListAdapter()
    {
    }

public:
    // New functions

    /**
     * Provides access to the \ref MPIMListAdapter representation of this
     * MPIMContactListAdapter object.
     *
     * @return The \ref MPIMListAdapter representation of the specialized
     *         list adapter.
     */
    virtual MPIMListAdapter* GetPimListAdapter() = 0;

    /**
     * Creates a new event item in the native database.
     * The adapter creates a new native database entry, sets its data
     * according to the data in \a aEventItem, adds it to the database
     * and sets the Item ID of \a aEventItem. \a aEventItem must
     * contain valid data and have Item ID \ref KPIMNullItemID.
     *
     * @param aEventItem The event item to add to the database.
     *
     * @par Leaving:
     * @li KErrAbort - The item lacks some vital data that
     *     cannot be replaced with a default value.
     * @li Other - The list adapter is non-functional.
     */
    virtual void CreateEventItemL(MPIMEventItem& aEventItem) = 0;

    /**
     * Reads an existing event item from the native database.
     * The adapter maps the Item ID in \a aEventItem to a native database
     * entry identifier, reads the entry and sets the data of
     * \a aEventItem according to the data in the native entry.
     * \a aEventItem must have valid Item ID that corresponds to an
     * existing native database entry.
     *
     * @param aEventItem The event item to be read from the database.
     *
     * @par Leaving:
     * The method leaves on error. Error codes should be interpreted as
     * follows:
     * @li \c KErrArgument - The Item ID of \a aEventItem is
     *        \ref KPIMNullItemID and thus invalid.
     * @li \c KErrNotFound - No corresponding native database entry could
     *        be retrieved, although the Item ID of \a aEventItem is not
     *        \ref KPIMNullItemID. The entry is probably removed from the
     *        native database.
     * @li Other - The list adapter is non-functional.
     */
    virtual void ReadEventItemL(MPIMEventItem& aEventItem) = 0;

    /**
     * Writes an existing event item to the native database.
     * The adapter maps the Item ID in \a aEventItem to a native database
     * entry identifier, reads the entry and sets the data of the entry
     * according to the data in \a aEventItem. \a aEventItem must have
     * valid Item ID that corresponds to an existing native database entry.
     *
     * @param aEventItem The event item to write to the database.
     *
     * @par Leaving:
     * The method leaves on error. Error codes should be interpreted as
     * follows:
     * @li \c KErrArgument - The Item ID of \a aEventItem is
     *        \ref KPIMNullItemID and thus invalid.
     * @li \c KErrNotFound - No corresponding native database entry could
     *        be retrieved, although the Item ID of \a aEventItem is not
     *        \ref KPIMNullItemID. The entry is probably removed from the
     *        native database.
     * @li \c KErrInUse - The entry is currently in use by some other
     *        client. The operation may succeed later.
     * @li KErrAbort - The item lacks some vital data that
     *     cannot be replaced with a default value.
     * @li Other - The list adapter is non-functional.
     */
    virtual void WriteEventItemL(MPIMEventItem& aEventItem) = 0;

    /**
     * Removes an existing event from the native database.
     * The adapter maps \a aItemID to a native database entry and removes
     * it.
     *
     * @param aItemID Item ID of the item to be removed.
     *
     * @par Leaving:
     * @li \c KErrArgument - \a aItemID is \ref KPIMNullItemID and thus
     *        invalid.
     * @li \c KErrNotFound - No native Agenda Model entry could be retrieved
     *        by \a aItemID. The entry is probably already removed.
     * @li \c KErrInUse - The entry is currently in use by some other
     *        client. The operation may succeed later.
     * @li Other - The list adapter is non-functional.
     */
    virtual void RemoveEventItemL(TPIMItemID aItemID) = 0;

protected:
    // Non-public operations

    // Allow derivation with protected default constructor.
    MPIMEventListAdapter()
    {
    }

private:
    // Prohibited operations

    // Prohibit copy constructor.
    MPIMEventListAdapter(const MPIMEventListAdapter&)
    {
    }

    // Prohibit assigment operator.
    MPIMEventListAdapter& operator=(const MPIMEventListAdapter&)
    {
        return *this;
    }

};

#endif // MPIMEVENTLISTADAPTER_H
// End of File

