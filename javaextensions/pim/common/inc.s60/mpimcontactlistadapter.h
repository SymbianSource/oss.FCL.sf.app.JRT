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
* Description:  Access interface to a native Contact database.
 *
*/


#ifndef MPIMCONTACTLISTADAPTER_H
#define MPIMCONTACTLISTADAPTER_H

//  INCLUDES
#include "pimcommon.h"
#include "pimcontact.h"

// FORWARD DECLARATIONS
class MPIMListAdapter;
class MPIMContactItem;

// CLASS DECLARATION

/**
 *  Contact list adapter creates, reads, writes and removes contact items
 *  in a native database. See also \ref MPIMListAdapter class description.
 */
class MPIMContactListAdapter
{
public:
    // Destructor

    /**
     * Destructor.
     */
    virtual ~MPIMContactListAdapter()
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
     * Creates a new contact item (entry) in the native database.
     * The adapter creates a new native database entry, sets its data
     * according to the data in \a aContactItem, adds it to the database
     * and sets the Item ID of \a aContactItem. \a aContactItem must
     * contain valid data and have Item ID \ref KPIMNullItemID.
     *
     * @param aContactItem The contact item to add to the database.
     *
     * @par Leaving:
     * @li KErrAbort - The item lacks some vital data that
     *     cannot be replaced with a default value.
     * @li Other - The list adapter is non-functional.
     */
    virtual void CreateContactItemL(MPIMContactItem& aContactItem) = 0;

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
    virtual void ReadContactItemL(MPIMContactItem& aContactItem) = 0;

    /**
     * Reads an existing contact item from the native database
     * This version reads only minimal contact item data and hould be used
     * when listing contacts through the API. The named remote view is used
     * to read items from the database.
     *
     * @param aContactItem The contact item to be read from the database
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
    virtual void ReadMinimalContactItemL(MPIMContactItem& aContactItem) = 0;

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
    virtual void ReadMinimalContactItemL(MPIMContactItem& aContactItem,
                                         const MPIMContactItem& aMatchingContactItem) = 0;

    /**
     * Reads one field from the contact database and adds it to the
     * item. New view definition is used to filter the database
     *
     * @param aContactItem The contact item where the data is added
     * @param aContactField The contact fields which is to be read
     *        from the database
     */
    virtual void ReadContactFieldL(MPIMContactItem& aContactItem,
                                   TPIMContactField aContactField) = 0;

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
     * @li KErrAbort - The item lacks some vital data that
     *     cannot be replaced with a default value.
     * @li Other - The list adapter is non-functional.
     */
    virtual void WriteContactItemL(MPIMContactItem& aContactItem) = 0;

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
    virtual void RemoveContactItemL(TPIMItemID aItemID) = 0;

protected:
    // Non-public operations

    // Allow derivatin with protected default constructor.
    MPIMContactListAdapter()
    {
    }

private:
    // Prohibited operations

    // Prohibit copy constructor.
    MPIMContactListAdapter(const MPIMContactListAdapter&)
    {
    }

    // Prohibit assigment operator.
    MPIMContactListAdapter& operator=(const MPIMContactListAdapter&)
    {
        return *this;
    }

};

#endif // MPIMCONTACTLISTADAPTER_H
// End of File
