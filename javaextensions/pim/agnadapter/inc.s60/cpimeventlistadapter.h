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
* Description:  Handles PIM event list <-> Agenda Model conversions
 *
*/


#ifndef CPIMEVENTLISTADAPTER_H
#define CPIMEVENTLISTADAPTER_H

// INCLUDES
#include "mpimeventlistadapter.h"
#include "cpimagnlistadapter.h"
#include "pimexternalchanges.h"

// FORWARD DECLARATIONS
class MPIMEventItem;
class CPIMAgnEventAdapter;

// CLASS DECLARATION

/**
 * PIM Event List Adapter class
 *
 *  Event list adapter creates, reads, writes and removes event items
 *  in a native database. See also \ref MPIMListAdapter class description.
 */
NONSHARABLE_CLASS(CPIMEventListAdapter): public CPIMAgnListAdapter,
        public MPIMEventListAdapter
{
public: // constructor and destructor

    /**
     * Two-phased constructor.
     */
    static CPIMEventListAdapter* NewL(
        CCalEntry::TType aEntryType,
        CPIMAgnEventAdapter* aEventAdapter,
        java::util::FunctionServer* aFuncServer);

    /**
     * Destructor.
     */
    virtual ~CPIMEventListAdapter();

public: // Overloaded functions from (CPIMAgnListAdapter) MPIMListAdapter

    /**
     * Provides all categories currently existing in the native database.
     *
     * @return Array of categories. The contents of the array are not
     *         meant to be modified.
     *
     * @par Leaving:
     * The method leaves on error. Such error always means that the list
     * adapter is non-functional.
     */
    const CDesCArray& GetCategoriesL();

    /**
     * Provides the external changes to the items (entries) in the
     * native database. See \ref IsItemsExternallyModified method.
     *
     * If GetExternalItemModificationsL has never been called, all
     * entries in the native database are considered to be new.
     *
     * \b Important: The interpretation of the returned data set
     * depends of the status provided by IsItemsExternallyModified():
     * @li \c EExternalChangesNone always means that there are no
     *     modifications and NULL will be returned.
     * @li \c EExternalChangesMinor means that the changes are
     *     applied over the existing set of items, reflecting the
     *     addition, removal and modification of the database
     *     entries.
     * @li \c EExternalChangesMajor means that the changes cannot
     *     be classified and the whole set of items must be
     *     refreshed. All current entries in the database are
     *     returned as new entries and it is up to the caller to
     *     discard the existing set of items and create new set of
     *     items or to deduce which of the given entries are new
     *     and which of the existing items have been removed.
     *     If any existing items are reused, their data content
     *     must be explicitly refreshed.
     *
     * @return An array of item state change objects. The \b ownership
     *         of the array is transferred to the caller. If no
     *         changes are present, \c NULL is returned.
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

public: // Functions from MPIMToDoListAdapter

    /**
     * Provides access to the \ref MPIMListAdapter representation of this
     * MPIMEventListAdapter object.
     *
     * @return The \ref MPIMListAdapter representation of the specialized
     *         list adapter.
     */
    MPIMListAdapter* GetPimListAdapter();

    /**
     * Creates a new event item in the native Agenda File.
     * The adapter creates a new native database entry, sets its data
     * according to the data in \a aEventItem, adds it to the Agenda File
     * and sets the Item ID of \a aEventItem. \a aEventItem must
     * contain valid data and have Item ID \ref KPIMNullItemID.
     *
     * @param aEventItem The event item to add to the Agenda File.
     *
     * @par Leaving:
     * The method leaves on error. Such error always means that the list
     * adapter is non-functional.
     */
    void CreateEventItemL(MPIMEventItem& aEventItem);

    void DoCreateEventItemL(MPIMEventItem& aEventItem);

    /**
     * Reads an existing event item from the Agenda File.
     * The adapter maps the Item ID in \a aEventItem to a native Agenda Model
     * entry identifier, reads the entry and sets the data of
     * \a aEventItem according to the data in the native entry.
     * \a aEventItem must have valid Item ID that corresponds to an
     * existing native Agenda File entry.
     *
     * @param aEventItem The event item to be read from the Agenda File.
     *
     * @par Leaving:
     * The method leaves on error. Error codes should be interpreted as
     * follows:
     * @li \c KErrArgument - The Item ID of \a aEventItem is
     *        \ref KPIMNullItemID and thus invalid.
     * @li \c KErrNotFound - No corresponding native Agenda File entry could
     *        be retrieved, although the Item ID of \a aEventItem is not
     *        \ref KPIMNullItemID. The entry is probably removed from the
     *        native Agenda File.
     * @li Other - The list adapter is non-functional.
     */
    void ReadEventItemL(MPIMEventItem& aEventItem);

    void DoReadEventItemL(MPIMEventItem& aEventItem);

    /**
     * Writes an existing event item to the native Agenda File.
     * The adapter maps the Item ID in \a aEventItem to a native Agenda Model
     * entry identifier, reads the entry and sets the data of the entry
     * according to the data in \a aEventItem. \a aEventItem must have
     * valid Item ID that corresponds to an existing native Agenda Model entry.
     *
     * @param aEventItem The event item to write to the Agenda File.
     *
     * @par Leaving:
     * The method leaves on error. Error codes should be interpreted as
     * follows:
     * @li \c KErrArgument - The Item ID of \a aEventItem is
     *        \ref KPIMNullItemID and thus invalid.
     * @li \c KErrNotFound - No corresponding native database entry could
     *        be retrieved, although the Item ID of \a aEventItem is not
     *        \ref KPIMNullItemID. The entry is probably removed from the
     *        native Agenda File.
     * @li \c KErrInUse - The entry is currently in use by some other
     *        client. The operation may succeed later.
     * @li Other - The list adapter is non-functional.
     */
    void WriteEventItemL(MPIMEventItem& aEventItem);

    void DoWriteEventItemL(MPIMEventItem& aEventItem);

    /**
     * Removes an existing event from the native Agenda File.
     * The adapter maps \a aItemID to a native Agenda Model entry and removes
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
    void RemoveEventItemL(TPIMItemID aItemID);

    void DoRemoveEventItemL(TPIMItemID aItemID);

protected:

    /**
     * C++ default constructor.
     */
    CPIMEventListAdapter(java::util::FunctionServer* aFuncServer);

    /**
     * By default Symbian 2nd phase constructor is private.
     */
    void ConstructL(
        CCalEntry::TType aEntryType,
        CPIMAgnEventAdapter* aEventAdapter);

private: // Member data

    CPIMAgnEventAdapter* iAgnAdapter;
    CDesCArrayFlat* iEventCategoryArray;
    CCalEntry::TType iEntryType;

};

#endif // CPIMEVENTLISTADAPTER_H
// End of File
