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
* Description:  Common access interface to a native database.
 *
*/


#ifndef MPIMLISTADAPTER_H
#define MPIMLISTADAPTER_H

//  INCLUDES
#include <e32std.h>
#include <e32base.h>
#include <badesca.h>
#include "javacommonutils.h"
#include "fs_methodcall.h"
#include "functionserver.h"

// FORWARD DECLARATIONS
class CPIMCategoryStateChange;
class CPIMItemStateChange;

// CLASS DECLARATION

/**
 *  List adapters provide access to PIM entries and categories in native
 *  databases. Derived classes provide item read and write operations.
 *
 *  Only CBase-derived classes may implement this interface. Deriving other
 *  M-classes from this class is \b prohibited. Deriving specialized M-classes
 *  from this class might result in a "diamond-shaped" inheritance tree,
 *  because it is intended that the common parts of the item implementation
 *  implement this interface. Specialized implementations will be derived
 *  from that common implementation and those must implement the specialized
 *  interfaces (M-classes), too. Diamond-shaped inheritance is highly
 *  discouraged in Symbian OS. The case is resolved so that the M-classes
 *  that would be derived from this class provide a \e getter method, say
 *  \c "GetPimListAdapter()", that returns a pointer to this interface. The two
 *  interfaces access the same object, thus providing an "inheritance-like"
 *  accessibility to that object.
 */
class MPIMListAdapter
{
public:
    // Enumerations

    /**
     * Scale of external item changes.
     */
    enum TExternalItemChangeClass
    {
        // No changes
        EExternalChangesNone = 0,
        // Minor changes which are updated one by one
        EExternalChangesMinor = 1,
        // Major changes which cause refreshing all items
        EExternalChangesMajor = 2
    };

public:
    // New functions

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
    virtual void AddCategoryL(const TDesC& aNewCategory) = 0;

    /**
     * Deletes an existing category. If there is no such category, nothing
     * is done and the method returns successfully.
     *
     * @param aCategory The category to be deleted.
     *
     * @par Leaving:
     * The method leaves on error. Error codes should be interpreted as
     * follows:
     * @li \c KErrArgument - \a aCategory is invalid.
     * @li \c KErrNotSupported - The operation is not supported.
     * @li Other - The list adapter is non-functional.
     */
    virtual void DeleteCategoryL(const TDesC& aCategory) = 0;

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
    virtual void RenameCategoryL(const TDesC& aOldCategory,
                                 const TDesC& aNewCategory) = 0;

    /**
     * Checks whether there have been external changes to the categories in
     * the native database after last call to
     * \ref GetExternalCategoryModificationsL or list adapter creation.
     *
     * @return ETrue if there are any external changes, EFalse otherwise.
     */
    virtual TBool IsCategoriesExternallyModified() = 0;

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
    virtual RPointerArray<CPIMCategoryStateChange>*
    GetExternalCategoryModificationsL() = 0;

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
    virtual TExternalItemChangeClass IsItemsExternallyModified() = 0;

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
    virtual RPointerArray<CPIMItemStateChange>*
    GetExternalItemModificationsL() = 0;

    /**
     * Used to inform the list adapter that the list has been closed. The
     * list adapter may then release all resources it has reserved.
     * No method of the list adapter can be invoked after call to Close.
     */
    virtual void Close() = 0;

protected:
    // Non-public operations

    // Allow derivation with proteced default constructor.
    MPIMListAdapter()
    {
    }

    /**
     * Destructor.
     */
    virtual ~MPIMListAdapter()
    {
    }

private:
    // Prohibited operations

    // Prohibit copy constructor.
    MPIMListAdapter(const MPIMListAdapter&)
    {
    }

    // Prohibit assigment operator.
    MPIMListAdapter& operator=(const MPIMListAdapter&)
    {
        return *this;
    }

};

#endif // MPIMLISTADAPTER_H
// End of File
