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
* Description:  Base class for the event and todo list adapters
 *
*/


#ifndef CPIMAGNLISTADAPTER_H
#define CPIMAGNLISTADAPTER_H

// INCLUDES
#include <e32base.h>
#include <calprogresscallback.h>
#include <calchangecallback.h>
#include <calentry.h>
#include <caltime.h>
#include <badesca.h>

#include "mpimlistadapter.h"
#include "pimexternalchanges.h"

// FORWARD DECLARATIONS
class CCalSession;
class CCalEntryView;
class CPIMToDoListAdapter;
class CPIMEventListAdapter;
class CPIMAgnServerWait;

// CLASS DECLARATION

/**
 * PIM Agenda List Adapter class, the base for Event and ToDo List Adapters
 */
NONSHARABLE_CLASS(CPIMAgnListAdapter): public CBase,
        public MPIMListAdapter,
        public MCalChangeCallBack
{
public: // destructor

    /**
     * Destructor.
     */
    virtual ~CPIMAgnListAdapter();

public: // Functions from MPIMListAdapter

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
    virtual const CDesCArray& GetCategoriesL() = 0;

    /**
     * Adds a new category to the native database. If the category already
     * exists, nothing is done and the method returns successfully.
     *
     * @param aNewCategory The name of the category to be added.
     *
     * @par Leaving:
     * The method leaves on error. Error codes should be interpreted as
     * follows:
     * @li \c KErrArgument - \a aNewCategory is invalid.
     * @li \c KErrNotSupported - The operation is not supported.
     * @li Other - The list adapter is non-functional.
     */
    void AddCategoryL(const TDesC& aNewCategory);

    /**
     * Deletes an existing category. If there is no such category, nothing
     * is done and the method returns successfully.
     *
     * @param aCategory The category to be deleted.
     *
     * @par Leaving:
     * The method leaves on error. Error codes should be interpreted as
     * follows:
     * @li \c KErrNotFound - \a aCategory is invalid.
     * @li \c KErrNotSupported - The operation is not supported.
     * @li Other - The list adapter is non-functional.
     */
    void DeleteCategoryL(const TDesC& aCategory);

    /**
     * Renames an existing category. Entries in the old category are moved
     * to the new category.
     *
     * @param aOldCategory The old category name.
     * @param aNewCategory The new category name.
     *
     * @par Leaving:
     * The method leaves on error. Error codes should be interpreted as
     * follows:
     * @li \c KErrArgument - \a aNewCategory is invalid.
     * @li \c KErrNotFound - \a aOldCategory does not exist.
     * @li \c KErrNotSupported - The operation is not supported.
     * @li Other - The list adapter is non-functional.
     */
    void RenameCategoryL(const TDesC& aOldCategory,
                         const TDesC& aNewCategory);

    /**
     * Checks whether there have been external changes to the categories in
     * the native database after last call to
     * \ref GetExternalCategoryModifications or list adapter creation.
     *
     * @return ETrue if there are any external changes, EFalse otherwise.
     */
    TBool IsCategoriesExternallyModified();

    /**
     * Provides the external changes to the categories in the native
     * database. See \ref IsCategoriesExternallyModified method.
     *
     * @return An array of category state change objects. The \b ownership
     *         of the array is transferred to the caller. If no
     *         changes are present, \c NULL is returned.
     *
     * @par Leaving:
     * The method leaves with \c KErrCorrupt if updating the list of
     * external modifications has failed at some point. If the method has
     * leaved once, it must always leave.
     */
    RPointerArray<CPIMCategoryStateChange>*
    GetExternalCategoryModificationsL();

    /**
     * Checks whether there have been external changes to the items
     * (or entries) in the native database after last call to
     * GetExternalItemModificationsL() or list adapter creation.
     *
     * If GetExternalItemModificationsL() has never been called, all
     * entries in the native database are considered to be new.
     *
     * See GetExternalItemModificationsL().
     *
     * @return \c EExternalChangesNone if there are no changes;
     *         \c EExternalChangesMinor if there are changes that
     *         can be classified to new, removed or modified;
     *         \c EExternalChangesMajor if there are changes that
     *         force all items to be refreshed.
     */
    MPIMListAdapter::TExternalItemChangeClass IsItemsExternallyModified();

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
    virtual RPointerArray< CPIMItemStateChange>*
    GetExternalItemModificationsL() = 0;

    /**
     * Used to inform the list adapter that the list has been closed. The
     * list adapter may then release all resources it has reserved.
     * No method of the list adapter can be invoked after call to Close.
     */
    virtual void Close() = 0;

public: // From MCalChangeCallBack

    void CalChangeNotification(
        MCalChangeCallBack::TChangeEntryType aChangeEntryType);

protected:

    /**
     * Fetches external item modifications from the native Agenda Model.
     *
     * @param aEntryType The Agenda Model entry types that are to be checked
     *
     * @return An array of item state change objects. The \b ownership
     *         of the array is transferred to the caller. If no
     *         changes are present, \c NULL is returned.
     *
     * @par Leaving:
     * This method may leave.
     */
    RPointerArray< CPIMItemStateChange>*
    GetExternalItemModificationsByEntryTypeL(
        CCalEntry::TType aEntryType);

    void DoExternalItemModificationsByEntryTypeL(CCalEntry::TType aEntryType);

    /**
     * Fetches a CAgnEntry from the native Agenda Model.
     *
     * @param aItemId The Agenda Model unique id of the entry to be fetched
     * @param aEntryType The Agenda Model entry type to be fetched
     *
     * @return A CAgnEntry according to framework item.
     * The \b ownership of the entry is transferred to the caller.
     *
     * @par Leaving:
     * This method may leave.
     */
    CCalEntry* FetchNativeEntryL(
        TPIMItemID aItemId,
        CCalEntry::TType aEntryType);

    /**
     * Closes the file which is open in the current agenda server session,
     * and also deletes both the Agenda Model and Agenda Server
     */
    void CloseAgendaSession();

    /**
     * Leaves with KErrNotReady if session is closed.
     */
    void EnsureOpenSessionL();

protected: // Constructors

    /**
     * C++ default constructor.
     * Allow derivation with protected default constructor.
     */
    CPIMAgnListAdapter(java::util::FunctionServer* aFuncServer);

    /**
     * By default Symbian 2nd phase constructor is private.
     *
     * @param aEntryType Entry type for change callbacks (ToDo/Event/all).
     */
    void ConstructL(MCalChangeCallBack::TChangeEntryType aEntryType);

    void DoClose();

protected: // Member data

    /** Session to calendar server. Owned. */
    CCalSession* iCalSession;

    /** Entry-based view to the database. Owned. */
    CCalEntryView* iCalEntryView;

    /** Agenda server operation synchronizer. Owned. */
    CPIMAgnServerWait* iServerWait;

    /** Item change array. May be NULL. Owned. */
    RPointerArray< CPIMItemStateChange>* iItemChangeArray;

    /** Indicates whether changes have been read since they have last
     * occurred */
    TBool iChangesRead;

    java::util::FunctionServer* iFuncServer;
};

#endif // CPIMAGNLISTADAPTER_H
// End of File
