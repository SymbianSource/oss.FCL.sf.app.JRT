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
* Description:  Handles PIM todo list <-> Agenda Model todo conversions
 *
*/


#ifndef CPIMTODOLISTADAPTER_H
#define CPIMTODOLISTADAPTER_H

// INCLUDES
#include "mpimtodolistadapter.h"
#include "cpimagnlistadapter.h"
#include "pimexternalchanges.h"

// FORWARD DECLARATIONS
class MPIMToDoItem;
class CPIMAgnToDoAdapter;

// CLASS DECLARATION

/**
 * PIM ToDo List Adapter class
 *
 *  To-do list adapter creates, reads, writes and removes to-do items
 *  in a native database. See also \ref MPIMListAdapter class description.
 */
NONSHARABLE_CLASS(CPIMToDoListAdapter): public CPIMAgnListAdapter,
        public MPIMToDoListAdapter
{
public: // Constructor and destructor

    /**
     * Two-phased constructor.
     */
    static CPIMToDoListAdapter* NewL(java::util::FunctionServer* aFuncServer,
    CCalSession *aCalSession);

    /**
     * Destructor.
     */
    ~CPIMToDoListAdapter();

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
     * MPIMToDoListAdapter object.
     *
     * @return The \ref MPIMListAdapter representation of the specialized
     *         list adapter.
     */
    MPIMListAdapter* GetPimListAdapter();

    /**
     * Creates a new To-Do item in the native Agenda File.
     * The adapter creates a new native database entry, sets its data
     * according to the data in \a aToDoItem, adds it to the Agenda File
     * and sets the Item ID of \a aToDoItem. \a aToDoItem must
     * contain valid data and have Item ID \ref KPIMNullItemID.
     *
     * @param aToDoItem The To-Do item to add to the Agenda File.
     *
     * @par Leaving:
     * The method leaves on error. Such error always means that the list
     * adapter is non-functional.
     */
    void CreateToDoItemL(MPIMToDoItem& aToDoItem);

    void CPIMToDoListAdapter::DoCreateToDoItemL(
        MPIMToDoItem& aToDoItem);

    /**
     * Reads an existing To-Do item from the Agenda File.
     * The adapter maps the Item ID in \a aToDoItem to a native Agenda Model
     * entry identifier, reads the entry and sets the data of
     * \a aToDoItem according to the data in the native entry.
     * \a aToDoItem must have valid Item ID that corresponds to an
     * existing native Agenda File entry.
     *
     * @param aToDoItem The To-Do item to be read from the Agenda File.
     *
     * @par Leaving:
     * The method leaves on error. Error codes should be interpreted as
     * follows:
     * @li \c KErrArgument - The Item ID of \a aToDoItem is
     *        \ref KPIMNullItemID and thus invalid.
     * @li \c KErrNotFound - No corresponding native Agenda File entry could
     *        be retrieved, although the Item ID of \a aToDoItem is not
     *        \ref KPIMNullItemID. The entry is probably removed from the
     *        native Agenda File.
     * @li Other - The list adapter is non-functional.
     */
    void ReadToDoItemL(MPIMToDoItem& aToDoItem);

    void CPIMToDoListAdapter::DoReadToDoItemL(
        MPIMToDoItem& aToDoItem);

    /**
     * Writes an existing To-Do item to the native Agenda File.
     * The adapter maps the Item ID in \a aToDoItem to a native Agenda Model
     * entry identifier, reads the entry and sets the data of the entry
     * according to the data in \a aToDoItem. \a aToDoItem must have valid
     * Item ID that corresponds to an existing native Agenda Model entry.
     *
     * @param aToDoItem The To-Do item to write to the Agenda File.
     *
     * @par Leaving:
     * The method leaves on error. Error codes should be interpreted as
     * follows:
     * @li \c KErrArgument - The Item ID of \a aToDoItem is
     *        \ref KPIMNullItemID and thus invalid.
     * @li \c KErrNotFound - No corresponding native database entry could
     *        be retrieved, although the Item ID of \a aToDoItem is not
     *        \ref KPIMNullItemID. The entry is probably removed from the
     *        native Agenda File.
     * @li \c KErrInUse - The entry is currently in use by some other
     *        client. The operation may succeed later.
     * @li Other - The list adapter is non-functional.
     */
    void WriteToDoItemL(MPIMToDoItem& aToDoItem);

    void CPIMToDoListAdapter::DoWriteToDoItemL(
        MPIMToDoItem& aToDoItem);

    /**
     * Removes an existing To-Do from the native Agenda File.
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
    void RemoveToDoItemL(TPIMItemID aItemID);

    void CPIMToDoListAdapter::DoRemoveToDoItemL(
        TPIMItemID aItemID);

protected:

    /**
     * C++ default constructor.
     */
    CPIMToDoListAdapter(java::util::FunctionServer* aFuncServer);

    /**
     * By default Symbian 2nd phase constructor is private.
     */
    void ConstructL(TInt aCalSessionInt);

private: // Member data

    CDesCArrayFlat* iToDoCategoryArray;
    CPIMAgnToDoAdapter* iAgnToDoAdapter;
};

#endif // CPIMTODOLISTADAPTER_H
// End of File
